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

    /* Check if all chunks were received */
    if (!microsd_check_all_chunks_received(p_metadata)) {
        MICROSD_LOG(MICROSD_LOG_WARN, "Not all chunks received: %lu/%lu\n",
                    (unsigned long)p_metadata->chunks_received,
                    (unsigned long)p_metadata->total_chunks);

        /* Show which chunks are missing */
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

    MICROSD_LOG(MICROSD_LOG_INFO, "Finalizing chunk write for file '%s' (cluster %lu, size %lu)\n",
                p_metadata->filename, (unsigned long)p_metadata->file_cluster,
                (unsigned long)p_metadata->total_file_size);

    /* Create directory entry for the file */
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t filename_len = strlen(p_metadata->filename);
    uint32_t name_entries_needed = (filename_len + 14) / 15;
    uint32_t total_entries = 2 + name_entries_needed;

    /* Scan directory sectors to find free space */
    /* Only scan within the first cluster of the directory */
    uint32_t sector_offset = 0;
    uint32_t entry_index = 0;
    uint32_t max_sectors = p_fs_info->sectors_per_cluster; // Only scan first cluster
    bool found_space = false;
    uint32_t target_sector = 0;
    uint32_t root_base_sector = microsd_cluster_to_sector(p_fs_info, p_fs_info->root_cluster);

    for (sector_offset = 0; sector_offset < max_sectors; sector_offset++) {
        uint32_t root_sector = root_base_sector + sector_offset;

        if (!microsd_read_block(root_sector, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR,
                        "Failed to read root directory sector %lu (offset %lu)\n",
                        (unsigned long)root_sector, (unsigned long)sector_offset);
            continue;
        }

        /* Find end of directory or deleted entries in this sector */
        entry_index = 0;
        uint32_t consecutive_free = 0;
        uint32_t free_start_index = 0;

        while (entry_index < (SD_BLOCK_SIZE / 32)) {
            uint8_t entry_type = buffer[entry_index * 32];

            /* Check if entry is free (0x00) or deleted (high bit clear on in-use entries) */
            if (entry_type == 0x00) {
                /* Found end of directory - all remaining entries are free */
                if (consecutive_free == 0) {
                    free_start_index = entry_index;
                }
                consecutive_free = (SD_BLOCK_SIZE / 32) - entry_index;
                MICROSD_LOG(MICROSD_LOG_INFO,
                            "Sector %lu: Found 0x00 at entry %lu, %lu entries available\n",
                            (unsigned long)sector_offset, (unsigned long)entry_index,
                            (unsigned long)consecutive_free);
                break;
            }

            /* Continue counting consecutive free/deleted entries */
            if (consecutive_free > 0) {
                consecutive_free++;
            } else {
                consecutive_free = 0;
            }

            entry_index++;
        }

        /* Log if sector is completely full */
        if (consecutive_free == 0 && entry_index >= (SD_BLOCK_SIZE / 32)) {
            MICROSD_LOG(MICROSD_LOG_WARN,
                        "Sector %lu: Completely full (all %lu entries occupied)\n",
                        (unsigned long)sector_offset, (unsigned long)(SD_BLOCK_SIZE / 32));
        }

        /* Check if we have enough consecutive free space */
        if (consecutive_free >= total_entries) {
            target_sector = root_sector;
            entry_index = free_start_index;
            found_space = true;
            MICROSD_LOG(MICROSD_LOG_INFO,
                        "Found space in directory at sector %lu (offset %lu) entry %lu (%lu "
                        "entries available)\n",
                        (unsigned long)root_sector, (unsigned long)sector_offset,
                        (unsigned long)entry_index, (unsigned long)consecutive_free);
            break;
        }
    }

    if (!found_space) {
        MICROSD_LOG(MICROSD_LOG_ERROR,
                    "Not enough space in directory (%lu needed, searched %lu sectors)\n",
                    (unsigned long)total_entries, (unsigned long)sector_offset);
        return false;
    }

    /* Re-read the target sector to ensure we have the correct buffer */
    if (!microsd_read_block(target_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to re-read target directory sector\n");
        return false;
    }

    /* Create directory entry set */
    uint8_t entries[32 * 20];
    memset(entries, 0, sizeof(entries));

    /* File entry */
    exfat_file_entry_t *file_entry = (exfat_file_entry_t *)&entries[0];
    file_entry->entry_type = EXFAT_TYPE_FILE;
    file_entry->secondary_count = total_entries - 1;
    file_entry->file_attributes = 0x00;
    file_entry->created_timestamp = get_current_time();
    file_entry->last_modified_timestamp = get_current_time();
    file_entry->last_accessed_timestamp = get_current_time();

    /* Stream extension entry */
    exfat_stream_entry_t *stream_entry = (exfat_stream_entry_t *)&entries[32];
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
        exfat_name_entry_t *name_entry = (exfat_name_entry_t *)&entries[64 + (name_entry_idx * 32)];
        name_entry->entry_type = EXFAT_TYPE_FILE_NAME;
        name_entry->general_flags = 0x00;

        for (uint32_t i = 0; i < 15 && char_index < filename_len; i++, char_index++) {
            name_entry->file_name[i] = (uint16_t)p_metadata->filename[char_index];
        }
    }

    /* Calculate and set checksum */
    uint16_t checksum = calculate_entry_checksum(entries, total_entries);
    file_entry->set_checksum = checksum;

    /* Write entries to directory */
    memcpy(&buffer[entry_index * 32], entries, total_entries * 32);

    /* Ensure end-of-directory marker */
    uint32_t next_entry_index = entry_index + total_entries;
    if (next_entry_index < (SD_BLOCK_SIZE / 32)) {
        memset(&buffer[next_entry_index * 32], 0, 32);
    }

    /* Write directory back to the correct sector */
    if (!microsd_write_block(target_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write directory entry\n");
        return false;
    }

    /* Final flush to ensure directory entry is written to physical media */
    if (!flush_sector_cache(p_fs_info)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to flush final directory write\n");
        return false;
    }

    /* Invalidate cache to ensure fresh reads */
    invalidate_sector_cache();

    /* Force SD card internal buffer flush by reading back the directory sector
     * This ensures the SD card controller commits pending writes to flash memory */
    uint8_t verify_buffer[SD_BLOCK_SIZE];
    if (!microsd_read_block(target_sector, verify_buffer)) {
        MICROSD_LOG(MICROSD_LOG_WARN, "Failed to verify directory write\n");
    } else {
        MICROSD_LOG(MICROSD_LOG_INFO, "Directory write verified by read-back\n");

        /* Verify the file entry was actually written */
        uint8_t entry_type = verify_buffer[entry_index * 32];
        if (entry_type == EXFAT_TYPE_FILE) {
            exfat_file_entry_t *verify_entry =
                (exfat_file_entry_t *)&verify_buffer[entry_index * 32];
            MICROSD_LOG(MICROSD_LOG_INFO, "✓ File entry confirmed at sector %lu, entry %lu:\n",
                        (unsigned long)target_sector, (unsigned long)entry_index);
            MICROSD_LOG(MICROSD_LOG_INFO, "  Type: 0x%02X (File Entry)\n", entry_type);
            MICROSD_LOG(MICROSD_LOG_INFO, "  Secondary count: %u\n", verify_entry->secondary_count);
            MICROSD_LOG(MICROSD_LOG_INFO, "  Attributes: 0x%02X\n", verify_entry->file_attributes);
            MICROSD_LOG(MICROSD_LOG_INFO, "  Checksum: 0x%04X\n", verify_entry->set_checksum);
        } else {
            MICROSD_LOG(MICROSD_LOG_ERROR, "✗ File entry NOT found at expected location!\n");
            MICROSD_LOG(MICROSD_LOG_ERROR, "  Expected type: 0x%02X, Got: 0x%02X\n",
                        EXFAT_TYPE_FILE, entry_type);
            return false;
        }
    }

    /* Force write of allocation bitmap to ensure cluster is marked as allocated */
    MICROSD_LOG(MICROSD_LOG_INFO, "Flushing allocation bitmap for cluster %lu\n",
                (unsigned long)p_metadata->file_cluster);
    if (!microsd_update_allocation_bitmap(p_fs_info, p_metadata->file_cluster)) {
        MICROSD_LOG(MICROSD_LOG_WARN, "Failed to re-flush allocation bitmap\n");
    }

    /* Update volume flags to mark filesystem as "dirty" (modified)
     * This is CRITICAL for exFAT - the OS won't see changes without this */
    MICROSD_LOG(MICROSD_LOG_INFO, "Updating volume flags to mark filesystem as modified\n");
    uint32_t boot_sector_num = p_fs_info->partition_offset;
    exfat_boot_sector_t *boot_sector = (exfat_boot_sector_t *)buffer;

    if (microsd_read_block(boot_sector_num, buffer)) {
        /* Set the VolumeFlags dirty bit (bit 1) to indicate active/dirty state */
        boot_sector->volume_flags |= 0x0002;

        if (microsd_write_block(boot_sector_num, buffer)) {
            MICROSD_LOG(MICROSD_LOG_INFO, "✓ Volume flags updated (dirty bit set)\n");
        } else {
            MICROSD_LOG(MICROSD_LOG_WARN, "Failed to update volume flags\n");
        }
    } else {
        MICROSD_LOG(MICROSD_LOG_WARN, "Failed to read boot sector for volume flag update\n");
    }

    /* Additional delay to ensure SD card internal flash operations complete
     * SD cards may buffer writes internally - this delay allows commit to flash */
    MICROSD_LOG(MICROSD_LOG_INFO, "Waiting for SD card write buffer flush (100ms)...\n");
    sleep_ms(100);

    /* Clear the dirty bit to indicate filesystem is now in consistent state
     * This is CRITICAL - macOS and strict exFAT drivers won't mount if dirty bit remains set */
    MICROSD_LOG(MICROSD_LOG_INFO, "Clearing volume dirty bit to finalize filesystem state\n");
    if (microsd_read_block(boot_sector_num, buffer)) {
        /* Clear the VolumeFlags dirty bit (bit 1) to indicate clean/consistent state */
        boot_sector->volume_flags &= ~0x0002;

        if (microsd_write_block(boot_sector_num, buffer)) {
            MICROSD_LOG(MICROSD_LOG_INFO, "✓ Volume dirty bit cleared - filesystem is clean\n");
        } else {
            MICROSD_LOG(MICROSD_LOG_WARN, "Failed to clear volume dirty bit\n");
        }
    } else {
        MICROSD_LOG(MICROSD_LOG_WARN, "Failed to read boot sector for dirty bit clear\n");
    }

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
    if (NULL == p_fs_info || NULL == filename || NULL == p_chunk_data || NULL == p_bytes_read) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid parameters for chunk read\n");
        return false;
    }

    *p_bytes_read = 0;

    /* Find the file and get its cluster and size */
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t root_sector = microsd_cluster_to_sector(p_fs_info, p_fs_info->root_cluster);
    uint32_t file_cluster = 0;
    uint32_t file_size = 0;
    bool file_found = false;

    if (!microsd_read_block(root_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read root directory\n");
        return false;
    }

    /* Search for the file */
    for (uint32_t i = 0; i < (SD_BLOCK_SIZE / 32); i++) {
        uint8_t entry_type = buffer[i * 32];

        if (entry_type == 0x00) {
            break;
        }

        if (entry_type == 0x85) {
            uint8_t secondary_count = buffer[i * 32 + 1];

            /* Get file info from stream extension */
            if ((i + 1) < (SD_BLOCK_SIZE / 32) && buffer[(i + 1) * 32] == 0xC0) {
                exfat_stream_entry_t *stream = (exfat_stream_entry_t *)&buffer[(i + 1) * 32];
                file_size = stream->data_length;
                file_cluster = stream->first_cluster;
            }

            /* Extract filename */
            char current_filename[256] = "";
            uint32_t char_index = 0;

            for (uint32_t j = 2; j <= secondary_count && (i + j) < (SD_BLOCK_SIZE / 32); j++) {
                if (buffer[(i + j) * 32] == 0xC1) {
                    uint16_t *utf16_chars = (uint16_t *)&buffer[(i + j) * 32 + 2];
                    for (uint32_t k = 0; k < 15 && char_index < 255; k++) {
                        if (utf16_chars[k] == 0)
                            break;
                        if (utf16_chars[k] < 128) {
                            current_filename[char_index++] = (char)utf16_chars[k];
                        }
                    }
                }
            }
            current_filename[char_index] = '\0';

            if (strcmp(current_filename, filename) == 0) {
                file_found = true;
                break;
            }

            i += secondary_count;
        }
    }

    if (!file_found) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "File '%s' not found\n", filename);
        return false;
    }

    /* For chunk 0 (metadata), we don't read from file data */
    if (chunk_index == 0) {
        MICROSD_LOG(MICROSD_LOG_INFO, "Chunk 0 is metadata, returning empty\n");
        *p_bytes_read = 0;
        return true;
    }

    /* Calculate file offset for this chunk */
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

    /* Navigate to the correct cluster */
    uint32_t bytes_per_cluster = p_fs_info->sectors_per_cluster * SD_BLOCK_SIZE;
    uint32_t cluster_offset = file_offset / bytes_per_cluster;
    uint32_t offset_in_cluster = file_offset % bytes_per_cluster;

    uint32_t current_cluster = file_cluster;
    for (uint32_t i = 0; i < cluster_offset && current_cluster != 0xFFFFFFFF; i++) {
        current_cluster = get_next_cluster(p_fs_info, current_cluster);
    }

    if (current_cluster == 0xFFFFFFFF) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to find cluster for chunk\n");
        return false;
    }

    /* Read chunk data */
    uint32_t bytes_read = 0;
    while (bytes_read < bytes_to_read && current_cluster != 0xFFFFFFFF) {
        uint32_t cluster_start_sector = microsd_cluster_to_sector(p_fs_info, current_cluster);
        uint32_t sector_in_cluster = offset_in_cluster / SD_BLOCK_SIZE;
        uint32_t offset_in_sector = offset_in_cluster % SD_BLOCK_SIZE;

        if (!microsd_read_block(cluster_start_sector + sector_in_cluster, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read sector\n");
            return false;
        }

        uint32_t bytes_in_sector = SD_BLOCK_SIZE - offset_in_sector;
        uint32_t remaining = bytes_to_read - bytes_read;
        if (bytes_in_sector > remaining) {
            bytes_in_sector = remaining;
        }

        memcpy(p_chunk_data + bytes_read, buffer + offset_in_sector, bytes_in_sector);

        bytes_read += bytes_in_sector;
        offset_in_cluster += bytes_in_sector;

        if (offset_in_cluster >= bytes_per_cluster && bytes_read < bytes_to_read) {
            current_cluster = get_next_cluster(p_fs_info, current_cluster);
            offset_in_cluster = 0;
        }
    }

    *p_bytes_read = bytes_read;

    MICROSD_LOG(MICROSD_LOG_INFO, "Successfully read chunk %lu (%lu bytes)\n",
                (unsigned long)chunk_index, (unsigned long)bytes_read);

    return true;
}
