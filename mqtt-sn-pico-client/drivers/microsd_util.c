/*!
 * @file    microsd_util.c
 * @brief   Utility functions for microSD driver (logging, hex dump, directory listing)
 * @author  INF2004 Project Team
 * @date    2025
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "microsd_internal.h"

void microsd_add_to_log_buffer(microsd_log_level_t level, const char *fmt, ...) {
    if (!g_log_to_file_enabled || g_log_buffer_pos >= LOG_BUFFER_SIZE - 256) {
        return; /* Buffer full or logging disabled */
    }

    /* Get current timestamp (approximate) */
    uint32_t timestamp_ms = to_ms_since_boot(get_absolute_time());

    /* Format timestamp and log level */
    int header_len = snprintf(&g_log_buffer[g_log_buffer_pos], LOG_BUFFER_SIZE - g_log_buffer_pos,
                              "[%08lu][%s:%s] ", (unsigned long)timestamp_ms, MICROSD_DRIVER_ID,
                              (level == MICROSD_LOG_ERROR)  ? "ERROR"
                              : (level == MICROSD_LOG_WARN) ? "WARN"
                              : (level == MICROSD_LOG_INFO) ? "INFO"
                                                            : "DEBUG");

    if (header_len > 0 && g_log_buffer_pos + header_len < LOG_BUFFER_SIZE) {
        g_log_buffer_pos += header_len;

        /* Format the actual log message */
        va_list args;
        va_start(args, fmt);
        int msg_len = vsnprintf(&g_log_buffer[g_log_buffer_pos], LOG_BUFFER_SIZE - g_log_buffer_pos,
                                fmt, args);
        va_end(args);

        if (msg_len > 0 && g_log_buffer_pos + msg_len < LOG_BUFFER_SIZE) {
            g_log_buffer_pos += msg_len;
        }
    }
}

bool microsd_flush_log_to_file(filesystem_info_t const *const p_fs_info) {
    if (!g_log_to_file_enabled || g_log_buffer_pos == 0 || !p_fs_info) {
        return true; /* Nothing to do */
    }

    /* Create log filename with driver UUID */
    char log_filename[64];
    snprintf(log_filename, sizeof(log_filename), "%s.log", MICROSD_DRIVER_UUID);

    /* Use the existing file creation function to write log data */
    bool result =
        microsd_create_file(p_fs_info, log_filename, (uint8_t *)g_log_buffer, g_log_buffer_pos);

    if (result) {
        /* Reset buffer after successful write */
        g_log_buffer_pos = 0;
        memset(g_log_buffer, 0, LOG_BUFFER_SIZE);
    }

    return result;
}

bool microsd_list_directory(filesystem_info_t const *const p_fs_info) {
    if (NULL == p_fs_info) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid parameter for directory listing\n");
        return false;
    }

    if (!p_fs_info->is_exfat) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Not an exFAT filesystem\n");
        return false;
    }

    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t root_sector = microsd_cluster_to_sector(p_fs_info, p_fs_info->root_cluster);
    uint32_t file_count = 0;
    uint32_t total_size = 0;

    MICROSD_LOG(MICROSD_LOG_INFO,
                "\n╔════════════════════════════════════════════════════════════════╗\n");
    MICROSD_LOG(MICROSD_LOG_INFO,
                "║                     SD CARD DIRECTORY LISTING                  ║\n");
    MICROSD_LOG(MICROSD_LOG_INFO,
                "╠════════════════════════════════════════════════════════════════╣\n");

    /* Scan through all sectors in the root directory cluster */
    for (uint32_t sector_offset = 0; sector_offset < p_fs_info->sectors_per_cluster;
         sector_offset++) {
        uint32_t sector_num = root_sector + sector_offset;

        if (!microsd_read_block(sector_num, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read directory sector %lu\n",
                        (unsigned long)sector_num);
            continue;
        }

        /* Parse directory entries (32 bytes each) */
        for (uint32_t entry_idx = 0; entry_idx < (SD_BLOCK_SIZE / 32); entry_idx++) {
            uint32_t offset = entry_idx * 32;
            uint8_t entry_type = buffer[offset];

            /* End of directory marker - but continue scanning other sectors */
            if (entry_type == 0x00) {
                break; /* Exit this sector's loop, continue to next sector */
            }

            /* Skip unused/deleted entries */
            if (entry_type == 0xFF || !(entry_type & 0x80)) {
                continue;
            }

            /* Check if this is a File Directory Entry (0x85) */
            if (entry_type == EXFAT_TYPE_FILE) {
                exfat_file_entry_t *file_entry = (exfat_file_entry_t *)&buffer[offset];
                uint32_t secondary_count = file_entry->secondary_count;

                /* The stream extension entry should be the first secondary entry */
                uint32_t stream_entry_idx = entry_idx + 1;
                uint32_t stream_offset = stream_entry_idx * 32;

                /* Check if stream entry is in the same sector */
                if (stream_offset >= SD_BLOCK_SIZE) {
                    /* Stream entry is in next sector - need to read it */
                    if (!microsd_read_block(sector_num + 1, buffer)) {
                        MICROSD_LOG(MICROSD_LOG_WARN,
                                    "Failed to read next sector for stream entry\n");
                        continue;
                    }
                    stream_offset = 0;
                    entry_idx = (uint32_t)-1; /* Reset to continue from start of next sector */
                }

                uint8_t stream_type = buffer[stream_offset];
                if (stream_type != EXFAT_TYPE_STREAM_EXTENSION) {
                    continue;
                }

                exfat_stream_entry_t *stream_entry = (exfat_stream_entry_t *)&buffer[stream_offset];

                /* Extract filename from name entries */
                char filename[256] = {0};
                uint32_t name_length = stream_entry->name_length;
                uint32_t name_chars_copied = 0;
                uint32_t name_entries_needed = (name_length + 14) / 15;

                for (uint32_t name_idx = 0;
                     name_idx < name_entries_needed && name_chars_copied < name_length;
                     name_idx++) {
                    uint32_t name_entry_idx = stream_entry_idx + 1 + name_idx;
                    uint32_t name_entry_offset = (name_entry_idx % 16) * 32;

                    /* If we've wrapped to next sector, read it */
                    if (name_entry_idx / 16 > stream_entry_idx / 16) {
                        if (!microsd_read_block(sector_num + (name_entry_idx / 16), buffer)) {
                            break;
                        }
                    }

                    exfat_name_entry_t *name_entry =
                        (exfat_name_entry_t *)&buffer[name_entry_offset];

                    for (uint32_t i = 0; i < 15 && name_chars_copied < name_length;
                         i++, name_chars_copied++) {
                        filename[name_chars_copied] = (char)(name_entry->file_name[i] & 0xFF);
                    }
                }

                filename[name_chars_copied] = '\0';

                /* Display file information */
                uint64_t file_size = stream_entry->data_length;
                uint32_t first_cluster = stream_entry->first_cluster;

                file_count++;
                total_size += file_size;

                MICROSD_LOG(MICROSD_LOG_INFO, "║ %2lu. %-50s ║\n", (unsigned long)file_count,
                            filename);
                MICROSD_LOG(MICROSD_LOG_INFO,
                            "║     Size: %-10llu bytes  Cluster: %-8lu              ║\n",
                            (unsigned long long)file_size, (unsigned long)first_cluster);
                MICROSD_LOG(MICROSD_LOG_INFO,
                            "╟────────────────────────────────────────────────────────────────╢\n");

                /* Skip secondary entries */
                entry_idx += secondary_count;
            }
        }
    }

    MICROSD_LOG(MICROSD_LOG_INFO,
                "╠════════════════════════════════════════════════════════════════╣\n");
    MICROSD_LOG(MICROSD_LOG_INFO,
                "║ Total Files: %-4lu                                             ║\n",
                (unsigned long)file_count);
    MICROSD_LOG(MICROSD_LOG_INFO,
                "║ Total Size:  %-10lu bytes                                   ║\n",
                (unsigned long)total_size);
    MICROSD_LOG(MICROSD_LOG_INFO,
                "╚════════════════════════════════════════════════════════════════╝\n\n");

    return true;
}

bool microsd_hex_dump(uint32_t const start_sector, uint32_t const num_sectors) {
    if (num_sectors == 0 || num_sectors > 256) {
        printf("ERROR: Invalid number of sectors (valid range: 1-256)\n");
        return false;
    }

    uint8_t buffer[SD_BLOCK_SIZE];

    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║              SD CARD HEX DUMP - SECTOR ANALYSIS                ║\n");
    printf("╠════════════════════════════════════════════════════════════════╣\n");
    printf("║ Start Sector: %-10lu                                        ║\n",
           (unsigned long)start_sector);
    printf("║ Number of Sectors: %-6lu                                     ║\n",
           (unsigned long)num_sectors);
    printf("║ Total Bytes: %-10lu (%.2f KB)                              ║\n",
           (unsigned long)(num_sectors * SD_BLOCK_SIZE),
           (float)(num_sectors * SD_BLOCK_SIZE) / 1024.0f);
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");

    for (uint32_t sector = 0; sector < num_sectors; sector++) {
        uint32_t sector_num = start_sector + sector;

        if (!microsd_read_block(sector_num, buffer)) {
            printf("\n✗ ERROR: Failed to read sector %lu\n", (unsigned long)sector_num);
            return false;
        }

        printf("┌─ Sector %lu (0x%08lX) ────────────────────────────────────────┐\n",
               (unsigned long)sector_num, (unsigned long)sector_num);

        // Print hex dump in 16-byte rows
        for (uint32_t offset = 0; offset < SD_BLOCK_SIZE; offset += 16) {
            // Print offset
            printf("│ %04lX  ", (unsigned long)offset);

            // Print hex values
            for (uint32_t i = 0; i < 16; i++) {
                if (offset + i < SD_BLOCK_SIZE) {
                    printf("%02X ", buffer[offset + i]);
                } else {
                    printf("   ");
                }

                // Add extra space in the middle
                if (i == 7) {
                    printf(" ");
                }
            }

            printf(" │ ");

            // Print ASCII representation
            for (uint32_t i = 0; i < 16; i++) {
                if (offset + i < SD_BLOCK_SIZE) {
                    uint8_t byte = buffer[offset + i];
                    // Print printable ASCII characters, '.' for non-printable
                    if (byte >= 32 && byte <= 126) {
                        printf("%c", byte);
                    } else {
                        printf(".");
                    }
                } else {
                    printf(" ");
                }
            }

            printf(" │\n");
        }

        // Print sector summary
        printf("└────────────────────────────────────────────────────────────────┘\n");

        // Identify common patterns in this sector
        bool all_zeros = true;
        bool all_ffs = true;
        for (uint32_t i = 0; i < SD_BLOCK_SIZE; i++) {
            if (buffer[i] != 0x00)
                all_zeros = false;
            if (buffer[i] != 0xFF)
                all_ffs = false;
            if (!all_zeros && !all_ffs)
                break;
        }

        if (all_zeros) {
            printf("  ℹ Sector is all zeros (empty/erased)\n");
        } else if (all_ffs) {
            printf("  ℹ Sector is all 0xFF (unformatted/erased)\n");
        }

        // Check for boot signature at end of sector
        if (buffer[510] == 0x55 && buffer[511] == 0xAA) {
            printf("  ✓ Boot signature detected (0x55AA)\n");
        }

        // Check for exFAT signature
        if (buffer[0] == 0xEB && buffer[1] == 0x76 && buffer[2] == 0x90) {
            printf("  ✓ exFAT boot sector detected\n");
            if (buffer[3] == 'E' && buffer[4] == 'X' && buffer[5] == 'F' && buffer[6] == 'A' &&
                buffer[7] == 'T') {
                printf("  ✓ exFAT filesystem name confirmed\n");
            }
        }

        // Check for MBR partition table
        if (sector_num == 0 && buffer[510] == 0x55 && buffer[511] == 0xAA) {
            printf("  ✓ MBR detected - Partition table:\n");
            for (int i = 0; i < 4; i++) {
                uint32_t part_offset = 446 + (i * 16);
                uint8_t status = buffer[part_offset];
                uint8_t type = buffer[part_offset + 4];
                uint32_t first_lba = buffer[part_offset + 8] | (buffer[part_offset + 9] << 8) |
                                     (buffer[part_offset + 10] << 16) |
                                     (buffer[part_offset + 11] << 24);

                if (type != 0) {
                    printf("    Partition %d: Type=0x%02X, Start LBA=%lu, Status=0x%02X\n", i + 1,
                           type, (unsigned long)first_lba, status);
                }
            }
        }

        printf("\n");

        // Add a small delay every few sectors to allow serial output to flush
        if ((sector + 1) % 4 == 0) {
            sleep_ms(10);
        }
    }

    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("✓ Hex dump complete - %lu sector%s analyzed\n", (unsigned long)num_sectors,
           num_sectors == 1 ? "" : "s");
    printf("═══════════════════════════════════════════════════════════════════\n\n");

    return true;
}

uint32_t get_current_time() {
    // Use dummy implementation for now
    // Once MQTT with wifi is working, we can get real time from NTP
    uint32_t dummy_time = 0x4B3A2C00; // Fixed timestamp (e.g., Jan 1, 2021)
    return dummy_time;
}
