/*!
 * @file microsd_chunk.c
 * @brief Chunk-based file operations for SD card
 *
 * This module handles chunked file writing and reading with support for:
 * - Out-of-order chunk reception
 * - Chunk metadata tracking
 * - Sector caching for optimized sequential writes
 * - Directory entry creation for completed files
 *
 * @note Part of the modularized microSD driver system
 */

#include <stdio.h>
#include <string.h>

#include "microsd_driver.h"
#include "microsd_internal.h"
#include "microsd_spi.h"
#include "pico/stdlib.h"

/* ================================================================================================
 * SECTOR CACHE IMPLEMENTATION
 * ================================================================================================
 * Sector caching optimizes chunk writes by reducing read-modify-write cycles when multiple
 * chunks target the same sector (common in sequential chunk reception).
 */

/*! Sector cache for optimized chunk writes - defined here, declared extern in microsd_internal.h */
sector_cache_t g_sector_cache = {0};

/*!
 * @brief Flush cached sector to SD card if dirty
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @return bool                 true on success, false on failure
 */
static bool flush_sector_cache(filesystem_info_t const *const p_fs_info) {
    if (!g_sector_cache.valid || !g_sector_cache.dirty) {
        return true; // Nothing to flush
    }

    if (!microsd_write_block(g_sector_cache.sector_number, g_sector_cache.buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to flush cached sector %lu\n",
                    (unsigned long)g_sector_cache.sector_number);
        g_sector_cache.valid = false;
        g_sector_cache.dirty = false;
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Flushed cached sector %lu\n",
                (unsigned long)g_sector_cache.sector_number);

    g_sector_cache.dirty = false;
    return true;
}

/*!
 * @brief Get a sector for writing (from cache or SD card)
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @param[in] sector_number     Sector number to get
 * @param[out] sector_buffer    Buffer to receive sector data
 * @return bool                 true on success, false on failure
 */
static bool get_sector_for_write(filesystem_info_t const *const p_fs_info, uint32_t sector_number,
                                 uint8_t **sector_buffer) {
    // Check if requested sector is already cached
    if (g_sector_cache.valid && g_sector_cache.sector_number == sector_number) {
        *sector_buffer = g_sector_cache.buffer;
        return true;
    }

    // Flush current cache if dirty
    if (!flush_sector_cache(p_fs_info)) {
        return false;
    }

    // Read new sector into cache
    if (!microsd_read_block(sector_number, g_sector_cache.buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read sector %lu into cache\n",
                    (unsigned long)sector_number);
        g_sector_cache.valid = false;
        return false;
    }

    g_sector_cache.sector_number = sector_number;
    g_sector_cache.valid = true;
    g_sector_cache.dirty = false;

    *sector_buffer = g_sector_cache.buffer;
    return true;
}

/*!
 * @brief Mark cached sector as dirty (needs to be flushed)
 */
static void mark_sector_dirty(void) { g_sector_cache.dirty = true; }

/*!
 * @brief Invalidate sector cache
 */
static void invalidate_sector_cache(void) {
    g_sector_cache.valid = false;
    g_sector_cache.dirty = false;
}

/* ================================================================================================
 * HELPER FUNCTIONS
 * ================================================================================================
 * These static helper functions support chunk operations and directory entry creation.
 */

/*!
 * @brief Calculate exFAT name hash
 * @param[in] filename          Pointer to filename string
 * @param[in] length            Length of filename
 * @return uint16_t             Calculated hash value
 */
static uint16_t calculate_exfat_name_hash(char const *const filename, uint32_t length) {
    uint16_t hash = 0;
    for (uint32_t i = 0; i < length; i++) {
        hash = ((hash << 15) | (hash >> 1)) + (uint16_t)filename[i];
    }
    return hash;
}

/*!
 * @brief Calculate entry checksum for exFAT directory entry set
 * @param[in] p_entries         Pointer to directory entries
 * @param[in] count             Number of entries
 * @return uint16_t             Calculated checksum
 */
static uint16_t calculate_entry_checksum(uint8_t const *const p_entries, uint32_t count) {
    uint16_t checksum = 0;
    for (uint32_t i = 0; i < count * 32; i++) {
        if (i == 2 || i == 3)
            continue; // Skip checksum field itself
        checksum = ((checksum << 15) | (checksum >> 1)) + (uint16_t)p_entries[i];
    }
    return checksum;
}

/*!
 * @brief Get the next cluster in the FAT chain
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @param[in] cluster           Current cluster number
 * @return uint32_t             Next cluster number, or 0xFFFFFFFF if end of chain
 */
static uint32_t get_next_cluster(filesystem_info_t const *const p_fs_info, uint32_t cluster) {
    uint32_t fat_sector;
    uint32_t fat_offset;
    microsd_get_fat_entry_location(p_fs_info, cluster, &fat_sector, &fat_offset);

    uint8_t buffer[SD_BLOCK_SIZE];
    if (!microsd_read_block(fat_sector, buffer)) {
        return 0xFFFFFFFF;
    }

    uint32_t next_cluster = *((uint32_t *)&buffer[fat_offset]);
    return next_cluster;
}

/*!
 * @brief Extract filename from exFAT directory entry
 * @param[in] p_entry_buffer    Pointer to directory entry buffer
 * @param[in] entry_index       Index of the file entry
 * @param[in] secondary_count   Number of secondary entries
 * @param[out] p_filename       Output buffer for filename
 * @param[in] max_filename_len  Maximum length of filename buffer
 */
static void extract_filename_from_entry(uint8_t const *const p_entry_buffer, uint32_t entry_index,
                                        uint8_t secondary_count, char *const p_filename,
                                        uint32_t max_filename_len) {
    uint32_t char_index = 0;

    for (uint32_t j = 2; j <= secondary_count && (entry_index + j) < (SD_BLOCK_SIZE / 32); j++) {
        if (p_entry_buffer[(entry_index + j) * 32] == 0xC1) { /* Name entry */
            uint16_t const *utf16_chars =
                (uint16_t const *)&p_entry_buffer[(entry_index + j) * 32 + 2];
            for (uint32_t k = 0; k < 15 && char_index < (max_filename_len - 1); k++) {
                if (utf16_chars[k] == 0)
                    break;
                if (utf16_chars[k] < 128) {
                    p_filename[char_index++] = (char)utf16_chars[k];
                }
            }
        }
    }
    p_filename[char_index] = '\0';
}

/*!
 * @brief Search for a file in the root directory
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @param[in] filename          Filename to search for
 * @param[out] p_file_cluster   Output pointer for file's first cluster
 * @param[out] p_file_size      Output pointer for file size
 * @return bool                 true if file found, false otherwise
 */
static bool find_file_in_root_directory(filesystem_info_t const *const p_fs_info,
                                        char const *const filename, uint32_t *const p_file_cluster,
                                        uint32_t *const p_file_size) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t root_sector = microsd_cluster_to_sector(p_fs_info, p_fs_info->root_cluster);

    if (!microsd_read_block(root_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read root directory\n");
        return false;
    }

    /* Search for the file in root directory */
    for (uint32_t i = 0; i < (SD_BLOCK_SIZE / 32); i++) {
        uint8_t entry_type = buffer[i * 32];

        if (entry_type == 0x00) {
            break; /* End of directory */
        }

        if (entry_type == 0x85) { /* File entry */
            uint8_t secondary_count = buffer[i * 32 + 1];

            /* Get file info from stream extension */
            if ((i + 1) < (SD_BLOCK_SIZE / 32) && buffer[(i + 1) * 32] == 0xC0) {
                exfat_stream_entry_t const *stream =
                    (exfat_stream_entry_t const *)&buffer[(i + 1) * 32];
                *p_file_size = stream->data_length;
                *p_file_cluster = stream->first_cluster;
            }

            /* Extract and compare filename */
            char current_filename[256];
            extract_filename_from_entry(buffer, i, secondary_count, current_filename,
                                        sizeof(current_filename));

            if (strcmp(current_filename, filename) == 0) {
                MICROSD_LOG(MICROSD_LOG_INFO, "Found file '%s' at cluster %lu, size %lu bytes\n",
                            filename, (unsigned long)*p_file_cluster, (unsigned long)*p_file_size);
                return true;
            }

            i += secondary_count;
        }
    }

    MICROSD_LOG(MICROSD_LOG_ERROR, "File '%s' not found\n", filename);
    return false;
}

/*!
 * @brief Navigate to a specific cluster in a cluster chain
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @param[in] first_cluster     First cluster in the chain
 * @param[in] cluster_offset    Number of clusters to skip
 * @return uint32_t             Target cluster number, or 0xFFFFFFFF if not found
 */
static uint32_t navigate_to_cluster(filesystem_info_t const *const p_fs_info,
                                    uint32_t first_cluster, uint32_t cluster_offset) {
    uint32_t current_cluster = first_cluster;

    for (uint32_t i = 0; i < cluster_offset && current_cluster != 0xFFFFFFFF; i++) {
        current_cluster = get_next_cluster(p_fs_info, current_cluster);
    }

    return current_cluster;
}

/*!
 * @brief Read chunk data from cluster chain
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @param[in] start_cluster     Starting cluster for read
 * @param[in] offset_in_cluster Initial offset within first cluster
 * @param[out] p_output_buffer  Output buffer for chunk data
 * @param[in] bytes_to_read     Number of bytes to read
 * @param[out] p_bytes_read     Actual bytes read
 * @return bool                 true on success, false on failure
 */
static bool read_chunk_data_from_clusters(filesystem_info_t const *const p_fs_info,
                                          uint32_t start_cluster, uint32_t offset_in_cluster,
                                          uint8_t *const p_output_buffer, uint32_t bytes_to_read,
                                          uint32_t *const p_bytes_read) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t bytes_read = 0;
    uint32_t current_cluster = start_cluster;
    uint32_t bytes_per_cluster = p_fs_info->sectors_per_cluster * SD_BLOCK_SIZE;

    while (bytes_read < bytes_to_read && current_cluster != 0xFFFFFFFF) {
        uint32_t cluster_start_sector = microsd_cluster_to_sector(p_fs_info, current_cluster);
        uint32_t sector_in_cluster = offset_in_cluster / SD_BLOCK_SIZE;
        uint32_t offset_in_sector = offset_in_cluster % SD_BLOCK_SIZE;

        if (!microsd_read_block(cluster_start_sector + sector_in_cluster, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read sector %lu\n",
                        (unsigned long)(cluster_start_sector + sector_in_cluster));
            return false;
        }

        /* Calculate how many bytes to copy from this sector */
        uint32_t bytes_in_sector = SD_BLOCK_SIZE - offset_in_sector;
        uint32_t remaining = bytes_to_read - bytes_read;
        if (bytes_in_sector > remaining) {
            bytes_in_sector = remaining;
        }

        memcpy(p_output_buffer + bytes_read, buffer + offset_in_sector, bytes_in_sector);

        bytes_read += bytes_in_sector;
        offset_in_cluster += bytes_in_sector;

        /* Move to next cluster if needed */
        if (offset_in_cluster >= bytes_per_cluster && bytes_read < bytes_to_read) {
            current_cluster = get_next_cluster(p_fs_info, current_cluster);
            offset_in_cluster = 0;
        }
    }

    *p_bytes_read = bytes_read;
    return true;
}

/*!
 * @brief Log missing chunks for debugging
 * @param[in] p_metadata        Pointer to chunk metadata structure
 */
static void log_missing_chunks(chunk_metadata_t const *const p_metadata) {
    MICROSD_LOG(MICROSD_LOG_INFO, "Missing chunks: ");
    for (uint32_t i = 0; i < p_metadata->total_chunks; i++) {
        uint32_t byte_index = i / 8;
        uint32_t bit_index = i % 8;
        if (!(p_metadata->chunk_bitmap[byte_index] & (1 << bit_index))) {
            MICROSD_LOG(MICROSD_LOG_INFO, "%lu ", (unsigned long)i);
        }
    }
    MICROSD_LOG(MICROSD_LOG_INFO, "\n");
}

/*!
 * @brief Find free space in directory for new file entry
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @param[in] total_entries     Number of directory entries needed
 * @param[out] p_target_sector  Output pointer for target sector number
 * @param[out] p_entry_index    Output pointer for entry index within sector
 * @return bool                 true if space found, false otherwise
 */
static bool find_directory_free_space(filesystem_info_t const *const p_fs_info,
                                      uint32_t total_entries, uint32_t *const p_target_sector,
                                      uint32_t *const p_entry_index) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t max_sectors = p_fs_info->sectors_per_cluster;
    uint32_t root_base_sector = microsd_cluster_to_sector(p_fs_info, p_fs_info->root_cluster);

    for (uint32_t sector_offset = 0; sector_offset < max_sectors; sector_offset++) {
        uint32_t root_sector = root_base_sector + sector_offset;

        if (!microsd_read_block(root_sector, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR,
                        "Failed to read root directory sector %lu (offset %lu)\n",
                        (unsigned long)root_sector, (unsigned long)sector_offset);
            continue;
        }

        /* Find end of directory or deleted entries in this sector */
        uint32_t entry_idx = 0;
        uint32_t consecutive_free = 0;
        uint32_t free_start_index = 0;

        while (entry_idx < (SD_BLOCK_SIZE / 32)) {
            uint8_t entry_type = buffer[entry_idx * 32];

            if (entry_type == 0x00) {
                /* Found end of directory - all remaining entries are free */
                if (consecutive_free == 0) {
                    free_start_index = entry_idx;
                }
                consecutive_free = (SD_BLOCK_SIZE / 32) - entry_idx;
                MICROSD_LOG(MICROSD_LOG_DEBUG,
                            "Sector %lu: Found 0x00 at entry %lu, %lu entries available\n",
                            (unsigned long)sector_offset, (unsigned long)entry_idx,
                            (unsigned long)consecutive_free);
                break;
            }

            if (consecutive_free > 0) {
                consecutive_free++;
            } else {
                consecutive_free = 0;
            }

            entry_idx++;
        }

        /* Check if we have enough consecutive free space */
        if (consecutive_free >= total_entries) {
            *p_target_sector = root_sector;
            *p_entry_index = free_start_index;
            MICROSD_LOG(MICROSD_LOG_INFO,
                        "Found space in directory at sector %lu (offset %lu) entry %lu (%lu "
                        "entries available)\n",
                        (unsigned long)root_sector, (unsigned long)sector_offset,
                        (unsigned long)free_start_index, (unsigned long)consecutive_free);
            return true;
        }
    }

    MICROSD_LOG(MICROSD_LOG_ERROR,
                "Not enough space in directory (%lu needed, searched %lu sectors)\n",
                (unsigned long)total_entries, (unsigned long)max_sectors);
    return false;
}

/*!
 * @brief Create directory entry set for a file
 * @param[in] p_metadata        Pointer to chunk metadata structure
 * @param[out] p_entries        Output buffer for directory entries
 * @param[in] total_entries     Total number of entries to create
 */
static void create_directory_entry_set(chunk_metadata_t const *const p_metadata,
                                       uint8_t *const p_entries, uint32_t total_entries) {
    uint32_t filename_len = strlen(p_metadata->filename);
    uint32_t name_entries_needed = (filename_len + 14) / 15;

    memset(p_entries, 0, total_entries * 32);

    /* File entry */
    exfat_file_entry_t *file_entry = (exfat_file_entry_t *)&p_entries[0];
    file_entry->entry_type = EXFAT_TYPE_FILE;
    file_entry->secondary_count = total_entries - 1;
    file_entry->file_attributes = 0x00;
    file_entry->created_timestamp = get_current_time();
    file_entry->last_modified_timestamp = get_current_time();
    file_entry->last_accessed_timestamp = get_current_time();

    /* Stream extension entry */
    exfat_stream_entry_t *stream_entry = (exfat_stream_entry_t *)&p_entries[32];
    stream_entry->entry_type = EXFAT_TYPE_STREAM_EXTENSION;
    stream_entry->general_flags = 0x01;
    stream_entry->name_length = filename_len;
    stream_entry->name_hash = calculate_exfat_name_hash(p_metadata->filename, filename_len);
    stream_entry->valid_data_length = p_metadata->total_file_size;
    stream_entry->first_cluster = p_metadata->file_cluster;
    stream_entry->data_length = p_metadata->total_file_size;

    /* Create name entries */
    uint32_t char_index = 0;
    for (uint32_t name_entry_idx = 0; name_entry_idx < name_entries_needed; name_entry_idx++) {
        exfat_name_entry_t *name_entry =
            (exfat_name_entry_t *)&p_entries[64 + (name_entry_idx * 32)];
        name_entry->entry_type = EXFAT_TYPE_FILE_NAME;
        name_entry->general_flags = 0x00;

        for (uint32_t i = 0; i < 15 && char_index < filename_len; i++, char_index++) {
            name_entry->file_name[i] = (uint16_t)p_metadata->filename[char_index];
        }
    }

    /* Calculate and set checksum */
    uint16_t checksum = calculate_entry_checksum(p_entries, total_entries);
    file_entry->set_checksum = checksum;
}

/*!
 * @brief Write directory entry to SD card
 * @param[in] target_sector     Target sector to write to
 * @param[in] entry_index       Entry index within sector
 * @param[in] p_entries         Pointer to directory entries to write
 * @param[in] total_entries     Total number of entries
 * @return bool                 true on success, false on failure
 */
static bool write_directory_entry(uint32_t target_sector, uint32_t entry_index,
                                  uint8_t const *const p_entries, uint32_t total_entries) {
    uint8_t buffer[SD_BLOCK_SIZE];

    /* Read the target sector */
    if (!microsd_read_block(target_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read target directory sector\n");
        return false;
    }

    /* Write entries to directory */
    memcpy(&buffer[entry_index * 32], p_entries, total_entries * 32);

    /* Ensure end-of-directory marker */
    uint32_t next_entry_index = entry_index + total_entries;
    if (next_entry_index < (SD_BLOCK_SIZE / 32)) {
        memset(&buffer[next_entry_index * 32], 0, 32);
    }

    /* Write directory back to the sector */
    if (!microsd_write_block(target_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write directory entry\n");
        return false;
    }

    return true;
}

/*!
 * @brief Verify directory entry was written correctly
 * @param[in] target_sector     Sector to verify
 * @param[in] entry_index       Entry index to verify
 * @return bool                 true if verified, false otherwise
 */
static bool verify_directory_entry(uint32_t target_sector, uint32_t entry_index) {
    uint8_t verify_buffer[SD_BLOCK_SIZE];

    if (!microsd_read_block(target_sector, verify_buffer)) {
        MICROSD_LOG(MICROSD_LOG_WARN, "Failed to verify directory write\n");
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Directory write verified by read-back\n");

    /* Verify the file entry was actually written */
    uint8_t entry_type = verify_buffer[entry_index * 32];
    if (entry_type == EXFAT_TYPE_FILE) {
        exfat_file_entry_t const *verify_entry =
            (exfat_file_entry_t const *)&verify_buffer[entry_index * 32];
        MICROSD_LOG(MICROSD_LOG_INFO, "✓ File entry confirmed at sector %lu, entry %lu:\n",
                    (unsigned long)target_sector, (unsigned long)entry_index);
        MICROSD_LOG(MICROSD_LOG_INFO, "  Type: 0x%02X (File Entry)\n", entry_type);
        MICROSD_LOG(MICROSD_LOG_INFO, "  Secondary count: %u\n", verify_entry->secondary_count);
        MICROSD_LOG(MICROSD_LOG_INFO, "  Attributes: 0x%02X\n", verify_entry->file_attributes);
        MICROSD_LOG(MICROSD_LOG_INFO, "  Checksum: 0x%04X\n", verify_entry->set_checksum);
        return true;
    } else {
        MICROSD_LOG(MICROSD_LOG_ERROR, "✗ File entry NOT found at expected location!\n");
        MICROSD_LOG(MICROSD_LOG_ERROR, "  Expected type: 0x%02X, Got: 0x%02X\n", EXFAT_TYPE_FILE,
                    entry_type);
        return false;
    }
}

/*!
 * @brief Update and clear volume dirty bit
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @return bool                 true on success, false on failure
 */
static bool update_volume_flags(filesystem_info_t const *const p_fs_info) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t boot_sector_num = p_fs_info->partition_offset;
    exfat_boot_sector_t *boot_sector = (exfat_boot_sector_t *)buffer;

    /* Set dirty bit first */
    MICROSD_LOG(MICROSD_LOG_INFO, "Updating volume flags to mark filesystem as modified\n");
    if (microsd_read_block(boot_sector_num, buffer)) {
        boot_sector->volume_flags |= 0x0002;

        if (microsd_write_block(boot_sector_num, buffer)) {
            MICROSD_LOG(MICROSD_LOG_INFO, "✓ Volume flags updated (dirty bit set)\n");
        } else {
            MICROSD_LOG(MICROSD_LOG_WARN, "Failed to update volume flags\n");
        }
    } else {
        MICROSD_LOG(MICROSD_LOG_WARN, "Failed to read boot sector for volume flag update\n");
    }

    /* Delay to ensure SD card internal flash operations complete */
    MICROSD_LOG(MICROSD_LOG_INFO, "Waiting for SD card write buffer flush (100ms)...\n");
    sleep_ms(100);

    /* Clear dirty bit to indicate filesystem is now in consistent state */
    MICROSD_LOG(MICROSD_LOG_INFO, "Clearing volume dirty bit to finalize filesystem state\n");
    if (microsd_read_block(boot_sector_num, buffer)) {
        boot_sector->volume_flags &= ~0x0002;

        if (microsd_write_block(boot_sector_num, buffer)) {
            MICROSD_LOG(MICROSD_LOG_INFO, "✓ Volume dirty bit cleared - filesystem is clean\n");
            return true;
        } else {
            MICROSD_LOG(MICROSD_LOG_WARN, "Failed to clear volume dirty bit\n");
        }
    } else {
        MICROSD_LOG(MICROSD_LOG_WARN, "Failed to read boot sector for dirty bit clear\n");
    }

    return false;
}

/* ================================================================================================
 * PUBLIC API - CHUNK OPERATIONS
 * ================================================================================================
 */

/*!
 * @brief Initialize chunk-based file writing
 *
 * Sets up metadata structure and allocates cluster chain for the entire file.
 * This allows out-of-order chunk reception.
 *
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @param[in] filename          Filename for the chunked file
 * @param[in] total_chunks      Total number of chunks (including metadata chunk 0)
 * @param[in] chunk_size        Size of each chunk in bytes
 * @param[in] actual_file_size  Actual file size (0 to calculate from chunks)
 * @param[out] p_metadata       Pointer to chunk metadata structure to initialize
 * @return bool                 true on success, false on failure
 */
bool microsd_init_chunk_write(filesystem_info_t const *const p_fs_info, char const *const filename,
                              uint32_t const total_chunks, uint32_t const chunk_size,
                              uint32_t const actual_file_size, chunk_metadata_t *const p_metadata) {
    if (NULL == p_fs_info || NULL == filename || NULL == p_metadata || total_chunks == 0 ||
        chunk_size == 0) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid parameters for chunk write initialization\n");
        return false;
    }

    if (total_chunks > 256) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Too many chunks: %lu (max 256)\n",
                    (unsigned long)total_chunks);
        return false;
    }

    /* Use actual file size if provided, otherwise calculate from chunks */
    uint32_t total_file_size =
        (actual_file_size > 0) ? actual_file_size : ((total_chunks - 1) * chunk_size);

    MICROSD_LOG(MICROSD_LOG_INFO,
                "Initializing chunk write: file='%s', chunks=%lu, chunk_size=%lu, total_size=%lu\n",
                filename, (unsigned long)total_chunks, (unsigned long)chunk_size,
                (unsigned long)total_file_size);

    /* Initialize metadata structure */
    memset(p_metadata, 0, sizeof(chunk_metadata_t));
    p_metadata->total_chunks = total_chunks;
    p_metadata->chunk_size = chunk_size;
    p_metadata->chunks_received = 0;
    p_metadata->total_file_size = total_file_size;
    strncpy(p_metadata->filename, filename, sizeof(p_metadata->filename) - 1);

    /* Allocate cluster chain for the entire file */
    if (!microsd_allocate_cluster_chain(p_fs_info, total_file_size, &p_metadata->file_cluster)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to allocate cluster chain for chunked file\n");
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Allocated cluster chain starting at cluster %lu\n",
                (unsigned long)p_metadata->file_cluster);

    return true;
}

/*!
 * @brief Write a single chunk to the file with sector caching (can be out-of-order)
 *
 * This optimized version uses sector caching to reduce read-modify-write operations
 * when multiple chunks target the same sector (sequential writes).
 *
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @param[in,out] p_metadata    Pointer to chunk metadata structure
 * @param[in] chunk_index       Index of the chunk (0-based, 0 is metadata)
 * @param[in] p_chunk_data      Pointer to chunk data
 * @param[in] chunk_data_size   Size of chunk data (may be less than chunk_size for last chunk)
 * @return bool                 true on success, false on failure
 */
bool microsd_write_chunk(filesystem_info_t const *const p_fs_info,
                         chunk_metadata_t *const p_metadata, uint32_t const chunk_index,
                         uint8_t const *const p_chunk_data, uint32_t const chunk_data_size) {
    if (NULL == p_fs_info || NULL == p_metadata || NULL == p_chunk_data) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid parameters for chunk write\n");
        return false;
    }

    if (chunk_index >= p_metadata->total_chunks) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid chunk index: %lu (max: %lu)\n",
                    (unsigned long)chunk_index, (unsigned long)(p_metadata->total_chunks - 1));
        return false;
    }

    /* Check if this chunk was already received */
    uint32_t byte_index = chunk_index / 8;
    uint32_t bit_index = chunk_index % 8;

    if (p_metadata->chunk_bitmap[byte_index] & (1 << bit_index)) {
        MICROSD_LOG(MICROSD_LOG_WARN, "Chunk %lu already received, skipping\n",
                    (unsigned long)chunk_index);
        return true;
    }

    /* Chunk 0 is metadata - store it but don't write to file data area */
    if (chunk_index == 0) {
        MICROSD_LOG(MICROSD_LOG_INFO, "Received metadata chunk (index 0, size %lu bytes)\n",
                    (unsigned long)chunk_data_size);

        /* Mark metadata chunk as received */
        p_metadata->chunk_bitmap[byte_index] |= (1 << bit_index);
        p_metadata->chunks_received++;

        return true;
    }

    /* Calculate file offset for this chunk (chunk 0 is metadata, so data chunks start at 1) */
    uint32_t file_offset = (chunk_index - 1) * p_metadata->chunk_size;

    MICROSD_LOG(MICROSD_LOG_INFO, "Writing chunk %lu/%lu at offset %lu (%lu bytes)\n",
                (unsigned long)chunk_index, (unsigned long)(p_metadata->total_chunks - 1),
                (unsigned long)file_offset, (unsigned long)chunk_data_size);

    /* Calculate which cluster and sector to write to */
    uint32_t bytes_per_cluster = p_fs_info->sectors_per_cluster * SD_BLOCK_SIZE;
    uint32_t cluster_offset = file_offset / bytes_per_cluster;
    uint32_t offset_in_cluster = file_offset % bytes_per_cluster;

    /* Walk the cluster chain to find the target cluster */
    uint32_t current_cluster = p_metadata->file_cluster;
    for (uint32_t i = 0; i < cluster_offset && current_cluster != 0xFFFFFFFF; i++) {
        current_cluster = get_next_cluster(p_fs_info, current_cluster);
    }

    if (current_cluster == 0xFFFFFFFF) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to find cluster for chunk %lu\n",
                    (unsigned long)chunk_index);
        return false;
    }

    /* Write chunk data to the cluster(s) */
    uint32_t bytes_written = 0;
    while (bytes_written < chunk_data_size && current_cluster != 0xFFFFFFFF) {
        uint32_t cluster_start_sector = microsd_cluster_to_sector(p_fs_info, current_cluster);
        uint32_t sector_in_cluster = offset_in_cluster / SD_BLOCK_SIZE;
        uint32_t offset_in_sector = offset_in_cluster % SD_BLOCK_SIZE;
        uint32_t target_sector = cluster_start_sector + sector_in_cluster;

        /* Get sector (from cache or SD card) */
        uint8_t *sector_buffer = NULL;
        if (!get_sector_for_write(p_fs_info, target_sector, &sector_buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to get sector %lu for chunk write\n",
                        (unsigned long)target_sector);
            return false;
        }

        /* Copy chunk data to sector buffer */
        uint32_t bytes_in_sector = SD_BLOCK_SIZE - offset_in_sector;
        uint32_t remaining = chunk_data_size - bytes_written;
        if (bytes_in_sector > remaining) {
            bytes_in_sector = remaining;
        }

        memcpy(sector_buffer + offset_in_sector, p_chunk_data + bytes_written, bytes_in_sector);

        /* Mark sector as dirty (will be flushed later) */
        mark_sector_dirty();

        bytes_written += bytes_in_sector;
        offset_in_cluster += bytes_in_sector;

        /* Move to next cluster if needed */
        if (offset_in_cluster >= bytes_per_cluster && bytes_written < chunk_data_size) {
            current_cluster = get_next_cluster(p_fs_info, current_cluster);
            offset_in_cluster = 0;
        } else if (bytes_written < chunk_data_size) {
            /* Continue in same cluster, next sector */
            offset_in_sector = 0;
        }
    }

    /* Flush sector cache to ensure data is written */
    if (!flush_sector_cache(p_fs_info)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to flush sector cache after chunk %lu\n",
                    (unsigned long)chunk_index);
        return false;
    }

    /* Mark this chunk as received */
    p_metadata->chunk_bitmap[byte_index] |= (1 << bit_index);
    p_metadata->chunks_received++;

    MICROSD_LOG(MICROSD_LOG_INFO, "Chunk %lu written successfully (%lu/%lu chunks received)\n",
                (unsigned long)chunk_index, (unsigned long)p_metadata->chunks_received,
                (unsigned long)p_metadata->total_chunks);

    return true;
}

/*!
 * @brief Check if all chunks have been received
 * @param[in] p_metadata        Pointer to chunk metadata structure
 * @return bool                 true if all chunks received, false otherwise
 */
bool microsd_check_all_chunks_received(chunk_metadata_t const *const p_metadata) {
    if (NULL == p_metadata) {
        return false;
    }

    return (p_metadata->chunks_received == p_metadata->total_chunks);
}

/*!
 * @brief Finalize chunk-based writing and create directory entry
 *
 * This function:
 * - Flushes all cached sectors
 * - Checks for missing chunks
 * - Creates directory entry for the completed file
 * - Updates volume flags to mark filesystem as modified
 *
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @param[in] p_metadata        Pointer to chunk metadata structure
 * @return bool                 true on success, false on failure
 */
bool microsd_finalize_chunk_write(filesystem_info_t const *const p_fs_info,
                                  chunk_metadata_t const *const p_metadata) {
    uint32_t target_sector = 0;
    uint32_t entry_index = 0;
    uint8_t entries[32 * 20]; /* Buffer for directory entries */

    /* Validate parameters */
    if (NULL == p_fs_info || NULL == p_metadata) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid parameters for chunk write finalization\n");
        return false;
    }

    /* Ensure all cached sectors are flushed before finalizing */
    if (!flush_sector_cache(p_fs_info)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to flush sector cache during finalization\n");
        return false;
    }

    /* Invalidate cache to prevent stale data */
    invalidate_sector_cache();

    /* Check if all chunks were received and log missing ones if any */
    if (!microsd_check_all_chunks_received(p_metadata)) {
        MICROSD_LOG(MICROSD_LOG_WARN, "Not all chunks received: %lu/%lu\n",
                    (unsigned long)p_metadata->chunks_received,
                    (unsigned long)p_metadata->total_chunks);
        log_missing_chunks(p_metadata);
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Finalizing chunk write for file '%s' (cluster %lu, size %lu)\n",
                p_metadata->filename, (unsigned long)p_metadata->file_cluster,
                (unsigned long)p_metadata->total_file_size);

    /* Calculate directory entry requirements */
    uint32_t filename_len = strlen(p_metadata->filename);
    uint32_t name_entries_needed = (filename_len + 14) / 15;
    uint32_t total_entries = 2 + name_entries_needed;

    /* Find free space in directory */
    if (!find_directory_free_space(p_fs_info, total_entries, &target_sector, &entry_index)) {
        return false;
    }

    /* Create directory entry set */
    create_directory_entry_set(p_metadata, entries, total_entries);

    /* Write directory entry to SD card */
    if (!write_directory_entry(target_sector, entry_index, entries, total_entries)) {
        return false;
    }

    /* Final flush to ensure directory entry is written to physical media */
    if (!flush_sector_cache(p_fs_info)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to flush final directory write\n");
        return false;
    }

    /* Invalidate cache to ensure fresh reads */
    invalidate_sector_cache();

    /* Verify directory entry was written correctly */
    if (!verify_directory_entry(target_sector, entry_index)) {
        return false;
    }

    /* Force write of allocation bitmap to ensure cluster is marked as allocated */
    MICROSD_LOG(MICROSD_LOG_INFO, "Flushing allocation bitmap for cluster %lu\n",
                (unsigned long)p_metadata->file_cluster);
    if (!microsd_update_allocation_bitmap(p_fs_info, p_metadata->file_cluster)) {
        MICROSD_LOG(MICROSD_LOG_WARN, "Failed to re-flush allocation bitmap\n");
    }

    /* Update volume flags and clear dirty bit */
    update_volume_flags(p_fs_info);

    MICROSD_LOG(MICROSD_LOG_INFO, "Successfully finalized chunk write for file '%s'\n",
                p_metadata->filename);
    MICROSD_LOG(MICROSD_LOG_INFO, "File should now be visible on SD card\n");

    return true;
}

/*!
 * @brief Read chunks from a file
 *
 * Reads a specific chunk from a file on the SD card. Chunk 0 is treated as metadata
 * and returns 0 bytes read.
 *
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @param[in] filename          Filename to read
 * @param[out] p_chunk_data     Pointer to buffer for chunk data
 * @param[in] chunk_size        Size of each chunk
 * @param[in] chunk_index       Index of chunk to read (0 = metadata, 1+ = data chunks)
 * @param[out] p_bytes_read     Pointer to store actual bytes read
 * @return bool                 true on success, false on failure
 */
bool microsd_read_chunk(filesystem_info_t const *const p_fs_info, char const *const filename,
                        uint8_t *const p_chunk_data, uint32_t const chunk_size,
                        uint32_t const chunk_index, uint32_t *const p_bytes_read) {
    uint32_t file_cluster = 0;
    uint32_t file_size = 0;

    /* Validate parameters */
    if (NULL == p_fs_info || NULL == filename || NULL == p_chunk_data || NULL == p_bytes_read) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid parameters for chunk read\n");
        return false;
    }

    *p_bytes_read = 0;

    /* For chunk 0 (metadata), we don't read from file data */
    if (chunk_index == 0) {
        MICROSD_LOG(MICROSD_LOG_INFO, "Chunk 0 is metadata, returning empty\n");
        return true;
    }

    /* Find the file and get its cluster and size */
    if (!find_file_in_root_directory(p_fs_info, filename, &file_cluster, &file_size)) {
        return false; /* File not found */
    }

    /* Calculate file offset for this chunk (chunk 0 is metadata, so data chunks start at 1) */
    uint32_t file_offset = (chunk_index - 1) * chunk_size;

    if (file_offset >= file_size) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Chunk index %lu exceeds file size\n",
                    (unsigned long)chunk_index);
        return false;
    }

    /* Calculate how many bytes to read */
    uint32_t bytes_to_read = chunk_size;
    if (file_offset + bytes_to_read > file_size) {
        bytes_to_read = file_size - file_offset;
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Reading chunk %lu from file '%s' (offset %lu, size %lu)\n",
                (unsigned long)chunk_index, filename, (unsigned long)file_offset,
                (unsigned long)bytes_to_read);

    /* Navigate to the correct cluster in the chain */
    uint32_t bytes_per_cluster = p_fs_info->sectors_per_cluster * SD_BLOCK_SIZE;
    uint32_t cluster_offset = file_offset / bytes_per_cluster;
    uint32_t offset_in_cluster = file_offset % bytes_per_cluster;

    uint32_t current_cluster = navigate_to_cluster(p_fs_info, file_cluster, cluster_offset);

    if (current_cluster == 0xFFFFFFFF) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to find cluster for chunk %lu\n",
                    (unsigned long)chunk_index);
        return false;
    }

    /* Read chunk data from cluster chain */
    if (!read_chunk_data_from_clusters(p_fs_info, current_cluster, offset_in_cluster, p_chunk_data,
                                       bytes_to_read, p_bytes_read)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read chunk data\n");
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Successfully read chunk %lu (%lu bytes)\n",
                (unsigned long)chunk_index, (unsigned long)*p_bytes_read);

    return true;
}
