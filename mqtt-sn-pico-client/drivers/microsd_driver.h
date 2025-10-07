/*!
 * @file    microsd_driver.h
 * @brief   MicroSD card driver for Raspberry Pi Pico W
 * @author  INF2004 Team
 * @date    2024
 */

#ifndef MICROSD_DRIVER_H
#define MICROSD_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

#include "hardware/spi.h"
#include "pico/stdlib.h"

/*! Driver identification and version */
#define MICROSD_DRIVER_ID "MICROSD_DRV_v1.0.0"
#define MICROSD_DRIVER_UUID "INF2004-CS31-MICROSD-2024"
#define MICROSD_DRIVER_VERSION_MAJOR (1U)
#define MICROSD_DRIVER_VERSION_MINOR (0U)
#define MICROSD_DRIVER_VERSION_PATCH (0U)

/*! Debug logging levels */
typedef enum {
    MICROSD_LOG_NONE = 0,
    MICROSD_LOG_ERROR = 1,
    MICROSD_LOG_WARN = 2,
    MICROSD_LOG_INFO = 3,
    MICROSD_LOG_DEBUG = 4
} microsd_log_level_t;

/*! Driver status structure */
typedef struct {
    char driver_id[32];
    char driver_uuid[64];
    uint8_t version_major;
    uint8_t version_minor;
    uint8_t version_patch;
    bool is_initialized;
    uint32_t total_reads;
    uint32_t total_writes;
    uint32_t last_error_code;
} microsd_driver_info_t;

/*! Pin definitions for Cytron Pico W Kit */
#define SD_SPI_PORT spi1
#define SD_MISO_PIN (12U)
#define SD_MOSI_PIN (11U)
#define SD_SCK_PIN (10U)
#define SD_CS_PIN (15U)

/*! SD Card Commands */
#define CMD0 (0U)    /*!< GO_IDLE_STATE */
#define CMD8 (8U)    /*!< SEND_IF_COND */
#define CMD17 (17U)  /*!< READ_SINGLE_BLOCK */
#define CMD24 (24U)  /*!< WRITE_BLOCK */
#define CMD55 (55U)  /*!< APP_CMD */
#define ACMD41 (41U) /*!< SD_SEND_OP_COND */

/*! Response types */
#define R1_IDLE_STATE (0x01U)
#define R1_READY_STATE (0x00U)

/*! Block size in bytes */
#define SD_BLOCK_SIZE (512U)

/*! MBR and filesystem structures */
#pragma pack(push, 1)

/*! MBR partition entry structure */
typedef struct {
    uint8_t status;         /*!< Partition status (0x80 = bootable, 0x00 = non-bootable) */
    uint8_t first_head;     /*!< First head (CHS addressing) */
    uint8_t first_sector;   /*!< First sector (CHS addressing) */
    uint8_t first_cylinder; /*!< First cylinder (CHS addressing) */
    uint8_t partition_type; /*!< Partition type (0x07 = exFAT, 0x0B/0x0C = FAT32) */
    uint8_t last_head;      /*!< Last head (CHS addressing) */
    uint8_t last_sector;    /*!< Last sector (CHS addressing) */
    uint8_t last_cylinder;  /*!< Last cylinder (CHS addressing) */
    uint32_t first_lba;     /*!< First LBA sector */
    uint32_t sector_count;  /*!< Number of sectors in partition */
} mbr_partition_entry_t;

/*! MBR (Master Boot Record) structure */
typedef struct {
    uint8_t boot_code[446];              /*!< Boot loader code */
    mbr_partition_entry_t partitions[4]; /*!< Partition table (4 entries) */
    uint16_t signature;                  /*!< MBR signature (0xAA55) */
} mbr_t;

/*! exFAT boot sector structure */
typedef struct {
    uint8_t jump_boot[3];              /*!< Jump instruction */
    uint8_t filesystem_name[8];        /*!< "EXFAT   " */
    uint8_t must_be_zero[53];          /*!< Must be zero */
    uint64_t partition_offset;         /*!< Partition offset in sectors */
    uint64_t volume_length;            /*!< Volume length in sectors */
    uint32_t fat_offset;               /*!< FAT offset in sectors */
    uint32_t fat_length;               /*!< FAT length in sectors */
    uint32_t cluster_heap_offset;      /*!< Cluster heap offset in sectors */
    uint32_t cluster_count;            /*!< Number of clusters */
    uint32_t first_cluster_of_root;    /*!< First cluster of root directory */
    uint32_t volume_serial_number;     /*!< Volume serial number */
    uint16_t filesystem_revision;      /*!< Filesystem revision */
    uint16_t volume_flags;             /*!< Volume flags */
    uint8_t bytes_per_sector_shift;    /*!< Bytes per sector shift */
    uint8_t sectors_per_cluster_shift; /*!< Sectors per cluster shift */
    uint8_t number_of_fats;            /*!< Number of FATs */
    uint8_t drive_select;              /*!< Drive select */
    uint8_t percent_in_use;            /*!< Percent in use */
    uint8_t reserved[7];               /*!< Reserved */
    uint8_t boot_code[390];            /*!< Boot code */
    uint16_t boot_signature;           /*!< Boot signature (0xAA55) */
} exfat_boot_sector_t;

/*! exFAT directory entry types */
#define EXFAT_TYPE_END_OF_DIRECTORY (0x00)
#define EXFAT_TYPE_ALLOCATION_BITMAP (0x81)
#define EXFAT_TYPE_UPCASE_TABLE (0x82)
#define EXFAT_TYPE_VOLUME_LABEL (0x83)
#define EXFAT_TYPE_FILE (0x85)
#define EXFAT_TYPE_STREAM_EXTENSION (0xC0)
#define EXFAT_TYPE_FILE_NAME (0xC1)

/*! exFAT file directory entry */
typedef struct {
    uint8_t entry_type;                   /*!< Entry type (0x85) */
    uint8_t secondary_count;              /*!< Number of secondary entries */
    uint16_t set_checksum;                /*!< Set checksum */
    uint16_t file_attributes;             /*!< File attributes */
    uint16_t reserved1;                   /*!< Reserved */
    uint32_t created_timestamp;           /*!< Created timestamp */
    uint32_t last_modified_timestamp;     /*!< Last modified timestamp */
    uint32_t last_accessed_timestamp;     /*!< Last accessed timestamp */
    uint8_t created_10ms_increment;       /*!< Created 10ms increment */
    uint8_t last_modified_10ms_increment; /*!< Last modified 10ms increment */
    uint8_t created_utc_offset;           /*!< Created UTC offset */
    uint8_t last_modified_utc_offset;     /*!< Last modified UTC offset */
    uint8_t last_accessed_utc_offset;     /*!< Last accessed UTC offset */
    uint8_t reserved2[7];                 /*!< Reserved */
} exfat_file_entry_t;

/*! exFAT stream extension directory entry */
typedef struct {
    uint8_t entry_type;         /*!< Entry type (0xC0) */
    uint8_t general_flags;      /*!< General flags */
    uint8_t reserved1;          /*!< Reserved */
    uint8_t name_length;        /*!< Name length */
    uint16_t name_hash;         /*!< Name hash */
    uint16_t reserved2;         /*!< Reserved */
    uint64_t valid_data_length; /*!< Valid data length */
    uint32_t reserved3;         /*!< Reserved */
    uint32_t first_cluster;     /*!< First cluster */
    uint64_t data_length;       /*!< Data length */
} exfat_stream_entry_t;

/*! exFAT file name directory entry */
typedef struct {
    uint8_t entry_type;     /*!< Entry type (0xC1) */
    uint8_t general_flags;  /*!< General flags */
    uint16_t file_name[15]; /*!< File name (UTF-16) */
} exfat_name_entry_t;

#pragma pack(pop)

/*! File system information structure */
typedef struct {
    bool is_exfat;                /*!< Is exFAT filesystem */
    uint32_t partition_offset;    /*!< Partition start sector (0 for no partitions) */
    uint32_t bytes_per_sector;    /*!< Bytes per sector */
    uint32_t sectors_per_cluster; /*!< Sectors per cluster */
    uint32_t bytes_per_cluster;   /*!< Bytes per cluster */
    uint32_t fat_offset;          /*!< FAT offset in sectors (relative to partition) */
    uint32_t fat_length;          /*!< FAT length in sectors */
    uint32_t cluster_heap_offset; /*!< Cluster heap offset in sectors (relative to partition) */
    uint32_t cluster_count;       /*!< Number of clusters */
    uint32_t root_cluster;        /*!< Root directory first cluster */
} filesystem_info_t;

/*!
 * @brief Initialize microSD card
 * @return bool true on success, false on failure
 */
bool microsd_init(void);

/*!
 * @brief Get driver information and statistics
 * @param[out] p_info   Pointer to driver info structure
 * @return bool         true on success, false on failure
 */
bool microsd_get_driver_info(microsd_driver_info_t* const p_info);

/*!
 * @brief Set logging level for debugging
 * @param[in] level     Logging level to set
 * @return void
 */
void microsd_set_log_level(microsd_log_level_t level);

/*!
 * @brief Print driver identification banner
 * @return void
 */
void microsd_print_banner(void);

/*!
 * @brief Read single byte from microSD card
 * @param[in]  address  Byte address to read from
 * @param[out] p_data   Pointer to store read data
 * @return bool         true on success, false on failure
 */
bool microsd_read_byte(uint32_t const address,
                       uint8_t* const p_data);

/*!
 * @brief Write single byte to microSD card
 * @param[in] address   Byte address to write to
 * @param[in] data      Data byte to write
 * @return bool         true on success, false on failure
 */
bool microsd_write_byte(uint32_t const address,
                        uint8_t const data);

/*!
 * @brief Read block from microSD card
 * @param[in]  block_num    Block number to read
 * @param[out] p_buffer     Pointer to buffer for read data
 * @return bool             true on success, false on failure
 */
bool microsd_read_block(uint32_t const block_num,
                        uint8_t* const p_buffer);

/*!
 * @brief Write block to microSD card
 * @param[in] block_num     Block number to write
 * @param[in] p_buffer      Pointer to data buffer
 * @return bool             true on success, false on failure
 */
bool microsd_write_block(uint32_t const block_num,
                         uint8_t const* const p_buffer);

/*!
 * @brief Initialize filesystem information
 * @param[out] p_fs_info    Pointer to filesystem info structure
 * @return bool             true on success, false on failure
 */
bool microsd_init_filesystem(filesystem_info_t* const p_fs_info);

/*!
 * @brief Create a file in the exFAT filesystem
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] filename      Filename (null-terminated string)
 * @param[in] p_data        Pointer to file data
 * @param[in] data_length   Length of file data in bytes
 * @return bool             true on success, false on failure
 */
bool microsd_create_file(filesystem_info_t const* const p_fs_info,
                         char const* const filename,
                         uint8_t const* const p_data,
                         uint32_t const data_length);

/*!
 * @brief Read a file from the exFAT filesystem
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] filename      Filename to read (null-terminated string)
 * @param[out] p_buffer     Pointer to buffer to store file data
 * @param[in] buffer_size   Size of the buffer
 * @param[out] p_bytes_read Pointer to store actual bytes read
 * @return bool             true on success, false on failure
 */
bool microsd_read_file(filesystem_info_t const* const p_fs_info,
                       char const* const filename,
                       uint8_t* const p_buffer,
                       uint32_t const buffer_size,
                       uint32_t* const p_bytes_read);

/*!
 * @brief Enable/disable logging to file
 * @param[in] enable    true to enable, false to disable
 * @return void
 */
void microsd_enable_file_logging(bool enable);

#endif /* MICROSD_DRIVER_H */
