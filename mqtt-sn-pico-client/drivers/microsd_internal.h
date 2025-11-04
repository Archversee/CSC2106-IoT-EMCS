/*!
 * @file    microsd_internal.h
 * @brief   Internal header for microSD driver modules - shared structures and functions
 * @author  INF2004 Project Team
 * @date    2025
 * @note    This file is for internal use by microSD driver modules only
 */

#ifndef MICROSD_INTERNAL_H
#define MICROSD_INTERNAL_H

#include <stdarg.h>

#include "microsd_driver.h"

/*! Driver state and statistics - shared across modules */
extern microsd_driver_info_t g_driver_info;
extern microsd_log_level_t g_log_level;

/*! Log buffer for writing to SD card */
#define LOG_BUFFER_SIZE 8192
extern char g_log_buffer[LOG_BUFFER_SIZE];
extern uint32_t g_log_buffer_pos;
extern bool g_log_to_file_enabled;

/*! Error codes for tracing */
#define MICROSD_ERROR_NONE (0x0000U)
#define MICROSD_ERROR_INIT_FAILED (0x0001U)
#define MICROSD_ERROR_CMD0_FAILED (0x0002U)
#define MICROSD_ERROR_CMD8_FAILED (0x0003U)
#define MICROSD_ERROR_ACMD41_TIMEOUT (0x0004U)
#define MICROSD_ERROR_READ_FAILED (0x0005U)
#define MICROSD_ERROR_WRITE_FAILED (0x0006U)

/*! Read retry configuration (HIGH PRIORITY FIX) */
#define MAX_READ_RETRIES 3     /* Maximum number of read retry attempts */
#define READ_RETRY_DELAY_MS 50 /* Delay between read retries in milliseconds */
#define READ_TIMEOUT_MS 2000   /* Increased timeout for read operations (was 1000 iterations) */
#define INTER_READ_DELAY_US                                                                        \
    100 /* Small delay between consecutive reads to prevent overwhelming card */

/*! Logging macro - shared across all modules */
#define MICROSD_LOG(level, fmt, ...)                                                               \
    do {                                                                                           \
        if (g_log_level >= level) {                                                                \
            printf("[%s:%s] " fmt, MICROSD_DRIVER_ID,                                              \
                   (level == MICROSD_LOG_ERROR)  ? "ERROR"                                         \
                   : (level == MICROSD_LOG_WARN) ? "WARN"                                          \
                   : (level == MICROSD_LOG_INFO) ? "INFO"                                          \
                                                 : "DEBUG",                                        \
                   ##__VA_ARGS__);                                                                 \
            if (g_log_to_file_enabled) {                                                           \
                microsd_add_to_log_buffer(level, fmt, ##__VA_ARGS__);                              \
            }                                                                                      \
        }                                                                                          \
    } while (0)

/*! Sector cache for optimized chunk writes */
typedef struct {
    uint32_t sector_number;        // Cached sector number
    uint8_t buffer[SD_BLOCK_SIZE]; // Sector data
    bool valid;                    // Is cache valid?
    bool dirty;                    // Does cache need to be written?
} sector_cache_t;

extern sector_cache_t g_sector_cache;

/* ========== SPI Module Functions (microsd_spi.c) ========== */

/*!
 * @brief Transfer single byte via SPI
 * @param[in] data  Data byte to transfer
 * @return uint8_t  Received data byte
 */
uint8_t microsd_spi_transfer(uint8_t const data);

/*!
 * @brief Select SD card (CS low)
 */
void microsd_cs_select(void);

/*!
 * @brief Deselect SD card (CS high)
 */
void microsd_cs_deselect(void);

/*!
 * @brief Send command to SD card
 * @param[in] cmd   Command number
 * @param[in] arg   Command argument
 * @return uint8_t  Response byte
 */
uint8_t microsd_send_command(uint8_t const cmd, uint32_t const arg);

/* ========== exFAT Module Functions (microsd_exfat.c) ========== */

/*!
 * @brief Calculate cluster start sector
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] cluster       Cluster number
 * @return uint32_t         Sector number of cluster start
 */
uint32_t microsd_cluster_to_sector(filesystem_info_t const *const p_fs_info, uint32_t cluster);

/*!
 * @brief Find a free cluster in the FAT
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @return uint32_t         Free cluster number, or 0 if none found
 */
uint32_t microsd_find_free_cluster(filesystem_info_t const *const p_fs_info);

/*!
 * @brief Mark cluster as end-of-chain in FAT
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] cluster       Cluster number to mark
 * @return bool             true on success, false on failure
 */
bool microsd_mark_cluster_end(filesystem_info_t const *const p_fs_info, uint32_t cluster);

/*!
 * @brief Update allocation bitmap to mark cluster as used
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] cluster       Cluster number to mark as allocated
 * @return bool             true on success, false on failure
 */
bool microsd_update_allocation_bitmap(filesystem_info_t const *const p_fs_info, uint32_t cluster);

/*!
 * @brief Get the next cluster in a cluster chain
 * @param[in] p_fs_info Pointer to filesystem info structure
 * @param[in] cluster   Current cluster number
 * @return uint32_t     Next cluster number, or 0xFFFFFFFF if end of chain
 */
uint32_t microsd_get_next_cluster(filesystem_info_t const *const p_fs_info, uint32_t cluster);

/*!
 * @brief Allocate cluster chain for large files
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] data_length   Length of data requiring allocation
 * @param[out] p_first_cluster Pointer to store first cluster number
 * @return bool             true on success, false on failure
 */
bool microsd_allocate_cluster_chain(filesystem_info_t const *const p_fs_info, uint32_t data_length,
                                    uint32_t *const p_first_cluster);

/*!
 * @brief Calculate FAT sector and offset for a given cluster number
 * @param[in]  p_fs_info    Pointer to filesystem info structure
 * @param[in]  cluster      Cluster number to locate in FAT
 * @param[out] fat_sector   Pointer to store the absolute sector number
 * @param[out] entry_offset Pointer to store the byte offset within sector
 */
void microsd_get_fat_entry_location(filesystem_info_t const *const p_fs_info, uint32_t cluster,
                                    uint32_t *fat_sector, uint32_t *entry_offset);

/*!
 * @brief Expand directory by allocating a new cluster and linking it
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] dir_cluster   Directory cluster to expand
 * @return bool             true on success, false on failure
 */
bool microsd_expand_directory(filesystem_info_t const *const p_fs_info, uint32_t dir_cluster);

/* ========== File Module Functions (microsd_file.c) ========== */

/*!
 * @brief Calculate exFAT name hash according to specification
 * @param[in] filename      Filename to hash
 * @param[in] length        Length of filename
 * @return uint16_t         Calculated name hash
 */
uint16_t microsd_calculate_name_hash(char const *const filename, uint32_t length);

/*!
 * @brief Calculate checksum for directory entry set
 * @param[in] p_entries     Pointer to directory entries
 * @param[in] count         Number of entries
 * @return uint16_t         Calculated checksum
 */
uint16_t microsd_calculate_entry_checksum(uint8_t const *const p_entries, uint32_t count);

/*!
 * @brief Write data across multiple clusters
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] first_cluster First cluster of the chain
 * @param[in] p_data        Pointer to data to write
 * @param[in] data_length   Length of data to write
 * @return bool             true on success, false on failure
 */
bool microsd_write_cluster_chain_data(filesystem_info_t const *const p_fs_info,
                                      uint32_t first_cluster, uint8_t const *const p_data,
                                      uint32_t data_length);

/* ========== Chunk Module Functions (microsd_chunk.c) ========== */

/*!
 * @brief Flush cached sector to SD card if dirty
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @return bool                 true on success, false on failure
 */
bool microsd_flush_sector_cache(filesystem_info_t const *const p_fs_info);

/*!
 * @brief Get a sector for writing (from cache or SD card)
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @param[in] sector_number     Sector number to get
 * @param[out] sector_buffer    Buffer to receive sector data pointer
 * @return bool                 true on success, false on failure
 */
bool microsd_get_sector_for_write(filesystem_info_t const *const p_fs_info, uint32_t sector_number,
                                  uint8_t **sector_buffer);

/*!
 * @brief Mark cached sector as dirty (needs to be flushed)
 */
void microsd_mark_sector_dirty(void);

/*!
 * @brief Invalidate sector cache
 */
void microsd_invalidate_sector_cache(void);

/* ========== Utility Module Functions (microsd_util.c) ========== */

/*!
 * @brief Add log message to buffer for later writing to SD card
 * @param[in] level     Log level
 * @param[in] fmt       Format string
 * @param[in] ...       Variable arguments
 */
void microsd_add_to_log_buffer(microsd_log_level_t level, const char *fmt, ...);

/*!
 * @brief Flush log buffer to SD card file
 * @param[in] p_fs_info     Filesystem info pointer
 * @return bool             true on success, false on failure
 */
bool microsd_flush_log_to_file(filesystem_info_t const *const p_fs_info);

#endif /* MICROSD_INTERNAL_H */
