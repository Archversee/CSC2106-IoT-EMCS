/**
 * @file microsd_file.c
 * @brief File operations for microSD exFAT filesystem
 *
 * This module handles:
 * - File creation with directory entries
 * - File reading from cluster chains
 * - Filename hashing and checksum calculation
 * - Writing data across multiple clusters
 * - Large file operations (chunked)
 * - Write throttling to prevent SD card overwhelm
 * - Volume dirty bit management for filesystem consistency
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "microsd_internal.h"

/* Write delay to prevent overwhelming the SD card (in milliseconds) */
#define SD_WRITE_DELAY_MS 10

/* ============================================================================
 * Helper Functions (Static - Internal to this module)
 * ============================================================================ */

/**
 * @brief Clear the exFAT volume dirty bit
 * @param p_fs_info Pointer to filesystem info structure
 * @return bool true on success, false on failure
 */
static bool clear_volume_dirty_bit(filesystem_info_t const *const p_fs_info) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t boot_sector_num = p_fs_info->partition_offset;

    /* Read boot sector */
    if (!microsd_read_block(boot_sector_num, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read boot sector for dirty bit clear\n");
        return false;
    }

    /* exFAT boot sector structure - VolumeFlags at offset 106 (0x6A) */
    exfat_boot_sector_t *boot_sector = (exfat_boot_sector_t *)buffer;

    /* Check if dirty bit is set */
    if (boot_sector->volume_flags & 0x0002) {
        /* Clear the VolumeFlags dirty bit (bit 1) */
        boot_sector->volume_flags &= ~0x0002;

        if (!microsd_write_block(boot_sector_num, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write boot sector to clear dirty bit\n");
            return false;
        }

        MICROSD_LOG(MICROSD_LOG_DEBUG, "Volume dirty bit cleared\n");

        /* Small delay to ensure write completes */
        sleep_ms(SD_WRITE_DELAY_MS);
    }

    return true;
}

/**
 * @brief Calculate exFAT name hash for directory entries
 * @param filename Filename to hash
 * @param length Length of filename
 * @return uint16_t Hash value
 */
static uint16_t calculate_exfat_name_hash(char const *const filename, uint32_t length) {
    uint16_t hash = 0;

    for (uint32_t i = 0; i < length; i++) {
        uint16_t c = (uint16_t)(uint8_t)filename[i];
        /* Convert to uppercase for hash calculation */
        if (c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
        hash = ((hash & 1) ? 0x8000 : 0) + (hash >> 1) + c;
    }

    return hash;
}

/**
 * @brief Calculate checksum for directory entry set
 * @param p_entries Pointer to directory entries
 * @param count Number of entries
 * @return uint16_t Calculated checksum
 */
static uint16_t calculate_entry_checksum(uint8_t const *const p_entries, uint32_t count) {
    uint16_t checksum = 0;

    for (uint32_t i = 0; i < count * 32; i++) {
        if (i == 2 || i == 3) {
            /* Skip checksum field itself */
            continue;
        }

        checksum = ((checksum & 1) ? 0x8000 : 0) + (checksum >> 1) + p_entries[i];
    }

    return checksum;
}

/**
 * @brief Write data across multiple clusters
 * @param p_fs_info Pointer to filesystem info structure
 * @param first_cluster First cluster of the chain
 * @param p_data Pointer to data to write
 * @param data_length Length of data to write
 * @return bool true on success, false on failure
 */
static bool write_cluster_chain_data(filesystem_info_t const *const p_fs_info,
                                     uint32_t first_cluster, uint8_t const *const p_data,
                                     uint32_t data_length) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t current_cluster = first_cluster;
    uint32_t bytes_written = 0;
    uint32_t bytes_per_cluster = p_fs_info->sectors_per_cluster * SD_BLOCK_SIZE;

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Writing %lu bytes across cluster chain starting at %lu\n",
                (unsigned long)data_length, (unsigned long)first_cluster);

    while (bytes_written < data_length && current_cluster != 0xFFFFFFFF) {
        uint32_t cluster_start_sector = microsd_cluster_to_sector(p_fs_info, current_cluster);
        uint32_t bytes_to_write_in_cluster = (data_length - bytes_written > bytes_per_cluster)
                                                 ? bytes_per_cluster
                                                 : (data_length - bytes_written);

        MICROSD_LOG(MICROSD_LOG_DEBUG, "Writing %lu bytes to cluster %lu\n",
                    (unsigned long)bytes_to_write_in_cluster, (unsigned long)current_cluster);

        /* Write data to each sector in this cluster */
        for (uint32_t sector = 0; sector < p_fs_info->sectors_per_cluster; sector++) {
            uint32_t bytes_in_sector = (bytes_to_write_in_cluster > SD_BLOCK_SIZE)
                                           ? SD_BLOCK_SIZE
                                           : bytes_to_write_in_cluster;

            if (bytes_in_sector == 0) {
                break; /* No more data to write */
            }

            /* Clear buffer and copy data */
            memset(buffer, 0, SD_BLOCK_SIZE);
            memcpy(buffer, p_data + bytes_written, bytes_in_sector);

            /* Write sector */
            if (!microsd_write_block(cluster_start_sector + sector, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write data sector %lu\n",
                            (unsigned long)(cluster_start_sector + sector));
                return false;
            }

            /* Add delay after write to prevent overwhelming the SD card */
            sleep_ms(SD_WRITE_DELAY_MS);

            bytes_written += bytes_in_sector;
            bytes_to_write_in_cluster -= bytes_in_sector;

            if (bytes_written >= data_length) {
                break; /* All data written */
            }
        }

        /* Move to next cluster in chain */
        if (bytes_written < data_length) {
            uint32_t fat_sector, entry_offset;
            microsd_get_fat_entry_location(p_fs_info, current_cluster, &fat_sector, &entry_offset);

            if (!microsd_read_block(fat_sector, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT for next cluster\n");
                return false;
            }

            uint32_t *fat_entry = (uint32_t *)(buffer + entry_offset);
            current_cluster = *fat_entry;
        }
    }

    if (bytes_written < data_length) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write all data: %lu of %lu bytes written\n",
                    (unsigned long)bytes_written, (unsigned long)data_length);
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Successfully wrote %lu bytes across cluster chain\n",
                (unsigned long)bytes_written);

    return true;
}

/**
 * @brief Get the next cluster in a cluster chain
 * @param p_fs_info Pointer to filesystem info structure
 * @param cluster Current cluster number
 * @return uint32_t Next cluster number, or 0xFFFFFFFF if end of chain
 */
static uint32_t get_next_cluster(filesystem_info_t const *const p_fs_info, uint32_t cluster) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t fat_sector, fat_offset;
    uint32_t next_cluster;

    /* Calculate FAT sector and offset using centralized function */
    microsd_get_fat_entry_location(p_fs_info, cluster, &fat_sector, &fat_offset);

    /* Read FAT sector */
    if (!microsd_read_block(fat_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector\n");
        return 0xFFFFFFFF;
    }

    /* Extract next cluster value */
    next_cluster = (uint32_t)buffer[fat_offset] | ((uint32_t)buffer[fat_offset + 1] << 8) |
                   ((uint32_t)buffer[fat_offset + 2] << 16) |
                   ((uint32_t)buffer[fat_offset + 3] << 24);

    /* Check for end of chain marker */
    if (next_cluster >= 0xFFFFFFF8) {
        return 0xFFFFFFFF; /* End of chain */
    }

    return next_cluster;
}

/**
 * @brief Extract filename from exFAT directory entries
 * @param p_entry_buffer Pointer to directory entry buffer
 * @param entry_index Index of the file entry in the buffer
 * @param secondary_count Number of secondary entries
 * @param p_filename Output buffer for filename
 * @param max_filename_len Maximum length of filename buffer
 */
static void extract_filename_from_entries(uint8_t const *const p_entry_buffer, uint32_t entry_index,
                                          uint8_t secondary_count, char *const p_filename,
                                          uint32_t max_filename_len) {
    uint32_t char_index = 0;

    /* Extract filename from name entries (starting at entry_index + 2) */
    for (uint32_t j = 2; j <= secondary_count && (entry_index + j) < (SD_BLOCK_SIZE / 32); j++) {
        uint8_t name_entry_type = p_entry_buffer[(entry_index + j) * 32];
        if (name_entry_type == 0xC1) { /* Name entry */
            uint16_t const *utf16_chars =
                (uint16_t const *)&p_entry_buffer[(entry_index + j) * 32 + 2];
            for (uint32_t k = 0; k < 15 && char_index < (max_filename_len - 1); k++) {
                if (utf16_chars[k] == 0)
                    break;                  /* End of filename */
                if (utf16_chars[k] < 128) { /* ASCII character */
                    p_filename[char_index++] = (char)utf16_chars[k];
                }
            }
        }
    }
    p_filename[char_index] = '\0';
}

/**
 * @brief Search for a file in a directory sector
 * @param p_fs_info Pointer to filesystem info structure
 * @param sector_buffer Sector buffer containing directory entries
 * @param target_filename Filename to search for
 * @param p_file_cluster Output pointer for file's first cluster
 * @param p_file_size Output pointer for file size
 * @return bool true if file found, false otherwise
 */
static bool search_directory_sector_for_file(filesystem_info_t const *const p_fs_info,
                                             uint8_t const *const sector_buffer,
                                             char const *const target_filename,
                                             uint32_t *const p_file_cluster,
                                             uint32_t *const p_file_size) {
    char current_filename[256];

    /* Search for the file in this directory sector */
    for (uint32_t i = 0; i < (SD_BLOCK_SIZE / 32); i++) {
        uint8_t entry_type = sector_buffer[i * 32];

        if (entry_type == 0x00) {
            /* End of directory entries in this sector */
            break;
        }

        if (entry_type == 0x85) { /* File entry */
            uint8_t secondary_count = sector_buffer[i * 32 + 1];

            /* Look for stream extension entry (should be next entry) */
            if ((i + 1) < (SD_BLOCK_SIZE / 32) && sector_buffer[(i + 1) * 32] == 0xC0) {
                /* Get file size and cluster from stream extension */
                uint32_t const *stream_data = (uint32_t const *)&sector_buffer[(i + 1) * 32];
                *p_file_size = stream_data[2];    /* DataLength at offset 8 */
                *p_file_cluster = stream_data[5]; /* FirstCluster at offset 20 */
            }

            /* Extract filename from name entries */
            extract_filename_from_entries(sector_buffer, i, secondary_count, current_filename,
                                          sizeof(current_filename));

            /* Check if this is the file we're looking for */
            if (strcmp(current_filename, target_filename) == 0) {
                MICROSD_LOG(MICROSD_LOG_INFO, "Found file '%s' at cluster %lu, size %lu bytes\n",
                            target_filename, (unsigned long)*p_file_cluster,
                            (unsigned long)*p_file_size);
                return true;
            }

            /* Skip past this file's entries */
            i += secondary_count;
        }
    }

    return false;
}

/**
 * @brief Search for a file in the directory tree
 * @param p_fs_info Pointer to filesystem info structure
 * @param filename Filename to search for
 * @param p_file_cluster Output pointer for file's first cluster
 * @param p_file_size Output pointer for file size
 * @return bool true if file found, false otherwise
 */
static bool find_file_in_directory(filesystem_info_t const *const p_fs_info,
                                   char const *const filename, uint32_t *const p_file_cluster,
                                   uint32_t *const p_file_size) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t current_dir_cluster = p_fs_info->root_cluster;
    uint32_t cluster_iteration = 0;
    const uint32_t max_clusters = 100; /* Safety limit to prevent infinite loops */

    /* Search through all directory clusters */
    while (current_dir_cluster != 0xFFFFFFFF && current_dir_cluster >= 2 &&
           current_dir_cluster < p_fs_info->cluster_count + 2 && cluster_iteration < max_clusters) {
        cluster_iteration++;

        /* Calculate sector for this directory cluster */
        uint32_t root_sector = p_fs_info->partition_offset + p_fs_info->cluster_heap_offset +
                               ((current_dir_cluster - 2) * p_fs_info->sectors_per_cluster);

        MICROSD_LOG(MICROSD_LOG_DEBUG, "Searching directory cluster %lu (sector %lu)\n",
                    (unsigned long)current_dir_cluster, (unsigned long)root_sector);

        /* Read all sectors in this directory cluster */
        for (uint32_t sec = 0; sec < p_fs_info->sectors_per_cluster; sec++) {
            if (!microsd_read_block(root_sector + sec, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR,
                            "Failed to read directory sector %lu (cluster %lu)\n",
                            (unsigned long)(root_sector + sec), (unsigned long)current_dir_cluster);
                return false; /* Read failure */
            }

            /* Search this sector for the file */
            if (search_directory_sector_for_file(p_fs_info, buffer, filename, p_file_cluster,
                                                 p_file_size)) {
                return true; /* File found! */
            }
        }

        /* Get next cluster in directory chain from FAT */
        uint32_t fat_sector = p_fs_info->partition_offset + p_fs_info->fat_offset +
                              (current_dir_cluster * 4) / SD_BLOCK_SIZE;
        uint32_t fat_offset = (current_dir_cluster * 4) % SD_BLOCK_SIZE;

        if (!microsd_read_block(fat_sector, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector %lu\n",
                        (unsigned long)fat_sector);
            return false;
        }

        uint32_t const *fat_entry = (uint32_t const *)&buffer[fat_offset];
        uint32_t next_cluster = *fat_entry;

        /* Check for valid cluster range or end-of-chain markers */
        if (next_cluster == 0xFFFFFFFF || next_cluster == 0xFFFFFFF8 || next_cluster < 2 ||
            next_cluster >= p_fs_info->cluster_count + 2) {
            /* End of chain or invalid cluster */
            MICROSD_LOG(MICROSD_LOG_DEBUG, "End of directory chain at cluster %lu (next=0x%08lX)\n",
                        (unsigned long)current_dir_cluster, (unsigned long)next_cluster);
            break;
        }

        current_dir_cluster = next_cluster;
    }

    MICROSD_LOG(MICROSD_LOG_ERROR, "File '%s' not found (searched %lu directory clusters)\n",
                filename, (unsigned long)cluster_iteration);
    return false;
}

/**
 * @brief Read file data from cluster chain
 * @param p_fs_info Pointer to filesystem info structure
 * @param first_cluster First cluster of the file
 * @param p_output_buffer Output buffer for file data
 * @param bytes_to_read Number of bytes to read
 * @param p_bytes_read Output pointer for actual bytes read
 * @return bool true on success, false on failure
 */
static bool read_file_data_from_clusters(filesystem_info_t const *const p_fs_info,
                                         uint32_t first_cluster, uint8_t *const p_output_buffer,
                                         uint32_t bytes_to_read, uint32_t *const p_bytes_read) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t bytes_read = 0;
    uint32_t current_cluster = first_cluster;

    while (bytes_read < bytes_to_read && current_cluster != 0xFFFFFFFF) {
        /* Calculate sector for this cluster */
        uint32_t cluster_sector = p_fs_info->partition_offset + p_fs_info->cluster_heap_offset +
                                  ((current_cluster - 2) * p_fs_info->sectors_per_cluster);

        MICROSD_LOG(MICROSD_LOG_DEBUG, "Reading from cluster %lu, sector %lu\n",
                    (unsigned long)current_cluster, (unsigned long)cluster_sector);

        /* Read all sectors in this cluster */
        for (uint32_t sector = 0;
             sector < p_fs_info->sectors_per_cluster && bytes_read < bytes_to_read; sector++) {
            if (!microsd_read_block(cluster_sector + sector, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read file data from sector %lu\n",
                            (unsigned long)(cluster_sector + sector));
                return false;
            }

            /* Calculate how many bytes to copy from this sector */
            uint32_t bytes_in_sector = SD_BLOCK_SIZE;
            if (bytes_read + bytes_in_sector > bytes_to_read) {
                bytes_in_sector = bytes_to_read - bytes_read;
            }

            /* Copy data from sector to output buffer */
            memcpy(p_output_buffer + bytes_read, buffer, bytes_in_sector);
            bytes_read += bytes_in_sector;
        }

        /* Move to next cluster in chain if we need more data */
        if (bytes_read < bytes_to_read) {
            current_cluster = get_next_cluster(p_fs_info, current_cluster);

            if (current_cluster >= 0xFFFFFFF8) {
                /* End of cluster chain */
                MICROSD_LOG(MICROSD_LOG_DEBUG, "Reached end of cluster chain (0x%08lX)\n",
                            (unsigned long)current_cluster);
                break;
            }
        }
    }

    *p_bytes_read = bytes_read;
    return true;
}

/* ============================================================================
 * Public API Functions
 * ============================================================================ */

/**
 * @brief Create a file in the exFAT filesystem
 * @param p_fs_info Pointer to filesystem info structure
 * @param filename Filename (null-terminated string)
 * @param p_data Pointer to file data
 * @param data_length Length of file data in bytes
 * @return bool true on success, false on failure
 */
bool microsd_create_file(filesystem_info_t const *const p_fs_info, char const *const filename,
                         uint8_t const *const p_data, uint32_t const data_length) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t root_sector, free_cluster;
    uint32_t filename_len;

    if (NULL == p_fs_info || NULL == filename || NULL == p_data) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid parameters for file creation\n");
        return false;
    }

    if (!p_fs_info->is_exfat) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Not an exFAT filesystem\n");
        return false;
    }

    filename_len = strlen(filename);
    if (filename_len == 0 || filename_len > 255) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid filename length: %lu\n",
                    (unsigned long)filename_len);
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Creating file: %s (%lu bytes)\n", filename,
                (unsigned long)data_length);

    /* Allocate cluster chain for file data (handles large files) */
    if (!microsd_allocate_cluster_chain(p_fs_info, data_length, &free_cluster)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to allocate cluster chain for file\n");
        return false;
    }

    /* Write file data across cluster chain */
    if (!write_cluster_chain_data(p_fs_info, free_cluster, p_data, data_length)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write file data\n");
        return false;
    }

    /* Always create versioned files - find the next available version number */
    char final_filename[256];
    uint32_t version_number = 0;
    bool filename_unique = false;

    /* Extract base filename and extension once */
    char base_name[240];
    char extension[16] = "";
    char *dot_pos = strrchr(filename, '.');

    if (dot_pos != NULL) {
        /* Has extension */
        size_t base_len = dot_pos - filename;
        strncpy(base_name, filename, base_len);
        base_name[base_len] = '\0';
        strcpy(extension, dot_pos);
    } else {
        /* No extension */
        strcpy(base_name, filename);
    }

    /* Calculate root directory sector */
    root_sector = microsd_cluster_to_sector(p_fs_info, p_fs_info->root_cluster);

    while (!filename_unique) {
        /* Generate filename with current version number */
        if (version_number == 0) {
            /* First try the original filename */
            strcpy(final_filename, filename);
        } else {
            /* Generate versioned filename: basename_N.ext */
            snprintf(final_filename, sizeof(final_filename), "%s_%lu%s", base_name,
                     (unsigned long)version_number, extension);
        }

        /* Check if this filename already exists in directory - search ALL sectors in root cluster
         */
        filename_unique = true;
        for (uint32_t sector = 0; sector < p_fs_info->sectors_per_cluster && filename_unique;
             sector++) {
            if (!microsd_read_block(root_sector + sector, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read root directory sector %lu\n",
                            (unsigned long)(root_sector + sector));
                return false;
            }

            for (uint32_t i = 0; i < (SD_BLOCK_SIZE / 32); i++) {
                uint8_t entry_type = buffer[i * 32];

                if (entry_type == 0x00) {
                    /* End of directory entries in this sector */
                    break;
                }

                if (entry_type == 0x85) { /* File entry */
                    uint8_t secondary_count = buffer[i * 32 + 1];

                    /* Extract filename from name entries to compare */
                    char existing_filename[256] = "";
                    uint32_t char_index = 0;

                    for (uint32_t j = 2; j <= secondary_count && (i + j) < (SD_BLOCK_SIZE / 32);
                         j++) {
                        uint8_t name_entry_type = buffer[(i + j) * 32];
                        if (name_entry_type == 0xC1) { /* Name entry */
                            uint16_t *utf16_chars = (uint16_t *)&buffer[(i + j) * 32 + 2];
                            for (uint32_t k = 0; k < 15 && char_index < 255; k++) {
                                if (utf16_chars[k] == 0)
                                    break;                  /* End of filename */
                                if (utf16_chars[k] < 128) { /* ASCII character */
                                    existing_filename[char_index++] = (char)utf16_chars[k];
                                }
                            }
                        }
                    }
                    existing_filename[char_index] = '\0';

                    /* Compare with our proposed filename */
                    if (strcmp(existing_filename, final_filename) == 0) {
                        filename_unique = false;
                        version_number++;
                        MICROSD_LOG(MICROSD_LOG_INFO,
                                    "File '%s' already exists, trying version %lu\n",
                                    final_filename, (unsigned long)version_number);
                        break;
                    }
                }
            }
        }
    }
    MICROSD_LOG(MICROSD_LOG_INFO, "Using unique filename: '%s'\n", final_filename);

    /* Use the unique filename for the rest of the function */
    filename_len = strlen(final_filename);

    /* Calculate number of name entries needed (15 UTF-16 chars per entry) */
    uint32_t name_entries_needed = (filename_len + 14) / 15; /* Round up */
    uint32_t total_entries = 2 + name_entries_needed;        /* File + Stream + Name entries */

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Filename '%s' needs %lu name entries (%lu total)\n",
                final_filename, (unsigned long)name_entries_needed, (unsigned long)total_entries);

    /* Find empty directory entries - search across ALL sectors in root cluster */
    uint32_t target_sector = 0;
    uint32_t entry_index = 0;
    uint32_t available_entries = 0;
    uint32_t cleaned_entries = 0;
    bool end_of_directory_found = false;

    /* First pass: Clean up corrupted/invalid file entries across all sectors */
    for (uint32_t sector = 0; sector < p_fs_info->sectors_per_cluster; sector++) {
        if (!microsd_read_block(root_sector + sector, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read root directory sector %lu\n",
                        (unsigned long)(root_sector + sector));
            return false;
        }

        bool sector_modified = false;
        for (uint32_t i = 3; i < (SD_BLOCK_SIZE / 32); i++) {
            uint8_t entry_type = buffer[i * 32];
            if (entry_type == 0x85) { /* File entry */
                /* Check for corrupted FirstCluster values */
                uint32_t first_cluster = *((uint32_t *)&buffer[i * 32 + 20]);
                if (first_cluster >
                    1000000) { /* Clearly invalid cluster number for small SD cards */
                    MICROSD_LOG(MICROSD_LOG_INFO,
                                "Cleaning corrupted file entry in sector %lu, entry %lu (invalid "
                                "cluster %lu)\n",
                                (unsigned long)sector, (unsigned long)i,
                                (unsigned long)first_cluster);

                    /* Clear this entry and its secondary entries */
                    uint8_t secondary_count = buffer[i * 32 + 1];
                    for (uint32_t j = 0; j <= secondary_count && (i + j) < (SD_BLOCK_SIZE / 32);
                         j++) {
                        memset(&buffer[(i + j) * 32], 0, 32);
                        cleaned_entries++;
                    }
                    sector_modified = true;
                    /* Skip past the cleared entries */
                    i += secondary_count;
                }
            }
        }

        if (sector_modified) {
            /* Write the cleaned directory sector back to SD card */
            if (!microsd_write_block(root_sector + sector, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write cleaned directory sector %lu\n",
                            (unsigned long)(root_sector + sector));
                return false;
            }

            /* Add delay after write to prevent overwhelming the SD card */
            sleep_ms(SD_WRITE_DELAY_MS);
        }
    }

    if (cleaned_entries > 0) {
        MICROSD_LOG(MICROSD_LOG_INFO,
                    "Cleaned %lu corrupted directory entries across all sectors\n",
                    (unsigned long)cleaned_entries);
    }

    /* Second pass: Find end of directory across all sectors */
    for (uint32_t sector = 0; sector < p_fs_info->sectors_per_cluster && !end_of_directory_found;
         sector++) {
        if (!microsd_read_block(root_sector + sector, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read root directory sector %lu\n",
                        (unsigned long)(root_sector + sector));
            return false;
        }

        for (uint32_t i = 0; i < (SD_BLOCK_SIZE / 32); i++) {
            uint8_t entry_type = buffer[i * 32];
            if (entry_type == 0x00) {
                /* Found end-of-directory marker - this is where we insert */
                target_sector = sector;
                entry_index = i;
                available_entries = (SD_BLOCK_SIZE / 32) - i;
                end_of_directory_found = true;
                MICROSD_LOG(MICROSD_LOG_DEBUG, "Found end-of-directory at sector %lu, entry %lu\n",
                            (unsigned long)sector, (unsigned long)i);
                break;
            }
        }
    }

    if (!end_of_directory_found) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Could not find end-of-directory marker in root cluster\n");
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Available directory entries: %lu, needed: %lu\n",
                (unsigned long)available_entries, (unsigned long)total_entries);

    if (available_entries < total_entries) {
        MICROSD_LOG(MICROSD_LOG_WARN,
                    "Root directory cluster full (need %lu entries, only %lu available)\n",
                    (unsigned long)total_entries, (unsigned long)available_entries);

        /* Attempt to expand directory by allocating new cluster */
        if (!microsd_expand_directory(p_fs_info, p_fs_info->root_cluster)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to expand root directory\n");
            MICROSD_LOG(MICROSD_LOG_INFO,
                        "Directory contains %lu entries out of %lu maximum per cluster\n",
                        (unsigned long)entry_index, (unsigned long)(SD_BLOCK_SIZE / 32));
            return false;
        }

        /* Find the expanded cluster by following the FAT chain */
        uint32_t current_dir_cluster = p_fs_info->root_cluster;
        uint32_t last_cluster = current_dir_cluster;
        uint32_t cluster_iteration = 0;
        const uint32_t max_clusters = 100;

        while (cluster_iteration < max_clusters) {
            cluster_iteration++;

            /* Get next cluster from FAT */
            uint32_t fat_sector = p_fs_info->partition_offset + p_fs_info->fat_offset +
                                  (current_dir_cluster * 4) / SD_BLOCK_SIZE;
            uint32_t fat_offset = (current_dir_cluster * 4) % SD_BLOCK_SIZE;

            if (!microsd_read_block(fat_sector, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT during directory expansion\n");
                return false;
            }

            uint32_t *fat_entry = (uint32_t *)&buffer[fat_offset];
            uint32_t next_cluster = *fat_entry;

            /* Check for end of chain or invalid cluster */
            if (next_cluster >= 0xFFFFFFF8 || next_cluster < 2 ||
                next_cluster >= p_fs_info->cluster_count + 2) {
                /* We've reached the end - last_cluster is the newly expanded one */
                break;
            }

            last_cluster = current_dir_cluster;
            current_dir_cluster = next_cluster;
        }

        /* Update root_sector to point to the newly expanded cluster */
        root_sector = p_fs_info->partition_offset + p_fs_info->cluster_heap_offset +
                      ((current_dir_cluster - 2) * p_fs_info->sectors_per_cluster);

        MICROSD_LOG(MICROSD_LOG_INFO,
                    "Directory expanded: now writing to cluster %lu (sector %lu)\n",
                    (unsigned long)current_dir_cluster, (unsigned long)root_sector);

        /* Read the newly expanded cluster (should be mostly empty) */
        if (!microsd_read_block(root_sector, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read expanded directory cluster\n");
            return false;
        }

        /* Reset entry_index to 0 since this is a new cluster */
        entry_index = 0;

        /* Update available entries count - new cluster should have full capacity */
        available_entries = (SD_BLOCK_SIZE / 32);
        MICROSD_LOG(
            MICROSD_LOG_INFO,
            "Directory expanded successfully, now have %lu available entries in new cluster\n",
            (unsigned long)available_entries);
    }

    /* Create directory entry set */
    uint8_t entries[32 * 5]; /* Max 5 entries: file + stream + 3 name entries */
    memset(entries, 0, sizeof(entries));

    /* File entry */
    exfat_file_entry_t *file_entry = (exfat_file_entry_t *)&entries[0];
    file_entry->entry_type = EXFAT_TYPE_FILE;
    file_entry->secondary_count =
        total_entries - 1;              /* Stream + Name entries (excluding file entry itself) */
    file_entry->file_attributes = 0x00; /* No special attributes - normal file */

    file_entry->created_timestamp = get_current_time();
    file_entry->last_modified_timestamp = get_current_time();
    file_entry->last_accessed_timestamp = get_current_time();
    file_entry->created_10ms_increment = 0;
    file_entry->last_modified_10ms_increment = 0;

    /* Stream extension entry */
    exfat_stream_entry_t *stream_entry = (exfat_stream_entry_t *)&entries[32];
    stream_entry->entry_type = EXFAT_TYPE_STREAM_EXTENSION;
    stream_entry->general_flags = 0x01; /* Allocation possible */
    stream_entry->name_length = filename_len;

    /* Calculate proper name hash */
    stream_entry->name_hash = calculate_exfat_name_hash(final_filename, filename_len);

    stream_entry->valid_data_length = data_length;
    stream_entry->first_cluster = free_cluster;
    stream_entry->data_length = data_length;

    /* Create name entries (multiple entries for long filenames) */
    uint32_t char_index = 0;
    for (uint32_t name_entry_idx = 0; name_entry_idx < name_entries_needed; name_entry_idx++) {
        exfat_name_entry_t *name_entry = (exfat_name_entry_t *)&entries[64 + (name_entry_idx * 32)];
        name_entry->entry_type = EXFAT_TYPE_FILE_NAME;
        name_entry->general_flags = 0x00;

        /* Convert filename to UTF-16 (simplified - ASCII only) */
        for (uint32_t i = 0; i < 15 && char_index < filename_len; i++, char_index++) {
            name_entry->file_name[i] = (uint16_t)final_filename[char_index];
        }

        MICROSD_LOG(
            MICROSD_LOG_DEBUG, "Name entry %lu: chars %lu-%lu\n", (unsigned long)name_entry_idx,
            (unsigned long)(char_index - (char_index < filename_len ? 15 : (filename_len % 15))),
            (unsigned long)(char_index - 1));
    }

    /* Calculate and set checksum for all entries */
    uint16_t checksum = calculate_entry_checksum(entries, total_entries);
    file_entry->set_checksum = checksum;

    /* Read the target sector again (in case it was modified during cleanup) */
    if (!microsd_read_block(root_sector + target_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read target directory sector %lu\n",
                    (unsigned long)(root_sector + target_sector));
        return false;
    }

    /* Copy entries to directory buffer */
    memcpy(&buffer[entry_index * 32], entries, total_entries * 32);

    /* Ensure proper end-of-directory marker */
    uint32_t next_entry_index = entry_index + total_entries;
    if (next_entry_index < (SD_BLOCK_SIZE / 32)) {
        /* Clear the entry after our file to mark end-of-directory */
        memset(&buffer[next_entry_index * 32], 0, 32);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Set end-of-directory marker at sector %lu, entry %lu\n",
                    (unsigned long)target_sector, (unsigned long)next_entry_index);
    }

    /* Write back the modified directory sector */
    if (!microsd_write_block(root_sector + target_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write root directory sector %lu\n",
                    (unsigned long)(root_sector + target_sector));
        return false;
    }

    /* Add delay after write to prevent overwhelming the SD card */
    sleep_ms(SD_WRITE_DELAY_MS);

    /* Clear the volume dirty bit to mark filesystem as clean */
    if (!clear_volume_dirty_bit(p_fs_info)) {
        MICROSD_LOG(MICROSD_LOG_WARN, "Failed to clear volume dirty bit after file creation\n");
        /* Continue anyway - file was created successfully */
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "File '%s' created successfully\n", final_filename);
    return true;
}

/**
 * @brief Read a file from the exFAT filesystem
 * @param p_fs_info Pointer to filesystem info structure
 * @param filename Filename to read (null-terminated string)
 * @param p_buffer Pointer to buffer to store file data
 * @param buffer_size Size of the buffer
 * @param p_bytes_read Pointer to store actual bytes read
 * @return bool true on success, false on failure
 */
bool microsd_read_file(filesystem_info_t const *const p_fs_info, char const *const filename,
                       uint8_t *const p_buffer, uint32_t const buffer_size,
                       uint32_t *const p_bytes_read) {
    uint32_t file_cluster = 0;
    uint32_t file_size = 0;
    uint32_t filename_len;

    /* Validate parameters */
    if (NULL == p_fs_info || NULL == filename || NULL == p_buffer || NULL == p_bytes_read) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid parameters for file reading\n");
        return false;
    }

    *p_bytes_read = 0;

    if (!p_fs_info->is_exfat) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Not an exFAT filesystem\n");
        return false;
    }

    filename_len = strlen(filename);
    if (filename_len == 0 || filename_len > 255) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid filename length: %lu\n",
                    (unsigned long)filename_len);
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Reading file: %s\n", filename);

    /* Search for the file in the directory tree */
    if (!find_file_in_directory(p_fs_info, filename, &file_cluster, &file_size)) {
        return false; /* File not found */
    }

    /* Validate file parameters */
    if (file_size == 0) {
        MICROSD_LOG(MICROSD_LOG_INFO, "File '%s' is empty\n", filename);
        *p_bytes_read = 0;
        return true;
    }
    if (file_cluster == 0) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid cluster number for file '%s'\n", filename);
        return false;
    }

    /* Calculate how many bytes to read (limited by buffer size) */
    uint32_t bytes_to_read = (file_size < buffer_size) ? file_size : buffer_size;

    /* Read file data from cluster chain */
    if (!read_file_data_from_clusters(p_fs_info, file_cluster, p_buffer, bytes_to_read,
                                      p_bytes_read)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read file data for '%s'\n", filename);
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Successfully read %lu bytes from file '%s'\n",
                (unsigned long)*p_bytes_read, filename);

    return true;
}
