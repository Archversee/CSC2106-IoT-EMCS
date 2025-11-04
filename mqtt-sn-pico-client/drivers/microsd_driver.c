/*!
 * @file    microsd_driver.c
 * @brief   MicroSD card driver implementation for Raspberry Pi Pico W
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 */

#include "microsd_driver.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "microsd_internal.h"
#include "pico/stdlib.h"

/*! Driver state and statistics - defined here, declared extern in microsd_internal.h */
microsd_driver_info_t g_driver_info = {0};
microsd_log_level_t g_log_level = MICROSD_LOG_INFO;

/*! Log buffer for writing to SD card - defined here, declared extern in microsd_internal.h */
char g_log_buffer[LOG_BUFFER_SIZE];
uint32_t g_log_buffer_pos = 0;
bool g_log_to_file_enabled = false;

/*! Read retry configuration (HIGH PRIORITY FIX) */
#define MAX_READ_RETRIES 3     /* Maximum number of read retry attempts */
#define READ_RETRY_DELAY_MS 50 /* Delay between read retries in milliseconds */
#define READ_TIMEOUT_MS 2000   /* Increased timeout for read operations (was 1000 iterations) */
#define INTER_READ_DELAY_US                                                                        \
    100 /* Small delay between consecutive reads to prevent overwhelming card */

/*! Private function prototypes (SPI functions and wrapper functions) */
static uint8_t spi_transfer(uint8_t const data);
static void cs_select(void);
static void cs_deselect(void);
static uint8_t send_command(uint8_t const cmd, uint32_t const arg);

/* Wrapper function prototypes (call module functions) */
static uint32_t cluster_to_sector(filesystem_info_t const *const p_fs_info, uint32_t cluster);
static void get_fat_entry_location(filesystem_info_t const *const p_fs_info, uint32_t cluster,
                                   uint32_t *fat_sector, uint32_t *entry_offset);
static uint32_t find_free_cluster(filesystem_info_t const *const p_fs_info);
static bool mark_cluster_end(filesystem_info_t const *const p_fs_info, uint32_t cluster);
static bool update_allocation_bitmap(filesystem_info_t const *const p_fs_info, uint32_t cluster);
static bool expand_directory(filesystem_info_t const *const p_fs_info, uint32_t dir_cluster);
static bool allocate_cluster_chain(filesystem_info_t const *const p_fs_info, uint32_t data_length,
                                   uint32_t *const p_first_cluster);

/*!
 * @brief Transfer single byte via SPI
 * @param[in] data  Data byte to transfer
 * @return uint8_t  Received data byte
 */
static uint8_t spi_transfer(uint8_t const data) {
    uint8_t rx_data;
    spi_write_read_blocking(SD_SPI_PORT, &data, &rx_data, 1U);
    return rx_data;
}

/*!
 * @brief Select SD card (CS low)
 * @return void
 */
static void cs_select(void) {
    gpio_put(SD_CS_PIN, 0U);
    sleep_us(1U);
}

/*!
 * @brief Deselect SD card (CS high)
 * @return void
 */
static void cs_deselect(void) {
    sleep_us(1U);
    gpio_put(SD_CS_PIN, 1U);
    sleep_us(1U);
}

/*!
 * @brief Send command to SD card
 * @param[in] cmd   Command number
 * @param[in] arg   Command argument
 * @return uint8_t  Response byte
 */
static uint8_t send_command(uint8_t const cmd, uint32_t const arg) {
    uint8_t response;
    uint8_t retry = 0U;

    /* Send command packet */
    (void)spi_transfer(0x40U | cmd);          /* Command with start bit */
    (void)spi_transfer((arg >> 24U) & 0xFFU); /* Argument byte 3 */
    (void)spi_transfer((arg >> 16U) & 0xFFU); /* Argument byte 2 */
    (void)spi_transfer((arg >> 8U) & 0xFFU);  /* Argument byte 1 */
    (void)spi_transfer(arg & 0xFFU);          /* Argument byte 0 */

    /* CRC (only matters for CMD0 and CMD8) */
    if (CMD0 == cmd) {
        (void)spi_transfer(0x95U);
    } else if (CMD8 == cmd) {
        (void)spi_transfer(0x87U);
    } else {
        (void)spi_transfer(0x01U);
    }

    /* Wait for response */
    do {
        response = spi_transfer(0xFFU);
        retry++;
    } while ((0U != (response & 0x80U)) && (retry < 10U));

    return response;
}

bool microsd_init(void) {
    bool result = false;
    uint8_t response;
    uint8_t r7_resp[4];
    int timeout;

    /* Initialize driver info structure */
    strncpy(g_driver_info.driver_id, MICROSD_DRIVER_ID, sizeof(g_driver_info.driver_id) - 1);
    strncpy(g_driver_info.driver_uuid, MICROSD_DRIVER_UUID, sizeof(g_driver_info.driver_uuid) - 1);
    g_driver_info.version_major = MICROSD_DRIVER_VERSION_MAJOR;
    g_driver_info.version_minor = MICROSD_DRIVER_VERSION_MINOR;
    g_driver_info.version_patch = MICROSD_DRIVER_VERSION_PATCH;
    g_driver_info.is_initialized = false;
    g_driver_info.total_reads = 0;
    g_driver_info.total_writes = 0;
    g_driver_info.last_error_code = MICROSD_ERROR_NONE;

    MICROSD_LOG(MICROSD_LOG_INFO, "Initializing driver %s\n", MICROSD_DRIVER_UUID);

    /* Deinitialize SPI first (in case it was previously initialized) */
    spi_deinit(SD_SPI_PORT);
    sleep_ms(10U);

    /* Initialize SPI */
    (void)spi_init(SD_SPI_PORT, 100000U); /* Start at 100kHz - very slow */
    MICROSD_LOG(MICROSD_LOG_DEBUG, "SPI initialized at 100kHz\n");

    /* Configure GPIO pins */
    gpio_set_function(SD_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SD_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SD_SCK_PIN, GPIO_FUNC_SPI);

    /* CS pin as output */
    gpio_init(SD_CS_PIN);
    gpio_set_dir(SD_CS_PIN, GPIO_OUT);
    gpio_put(SD_CS_PIN, 1U);

    /* Set pull-ups on unused SD card pins, as per MAKER-PI-PICO datasheet */
    gpio_init(13);
    gpio_set_dir(13, GPIO_IN);
    gpio_pull_up(13);
    gpio_init(14);
    gpio_set_dir(14, GPIO_IN);
    gpio_pull_up(14);
    MICROSD_LOG(MICROSD_LOG_DEBUG, "GPIO pins configured\n");

    sleep_ms(50U); /* Wait longer for SD card to power up and settle */

    /* Send 80 clock cycles with CS high */
    cs_deselect();
    for (int i = 0; i < 10; i++) {
        (void)spi_transfer(0xFFU);
    }
    MICROSD_LOG(MICROSD_LOG_DEBUG, "Sent 80 clock cycles\n");

    /* Enter SPI mode */
    cs_select();
    MICROSD_LOG(MICROSD_LOG_DEBUG, "Sending CMD0...\n");
    response = send_command(CMD0, 0U);
    cs_deselect();
    MICROSD_LOG(MICROSD_LOG_DEBUG, "CMD0 response: 0x%02X\n", response);

    if (R1_IDLE_STATE == response) {
        MICROSD_LOG(MICROSD_LOG_INFO, "Card is in idle state. Sending CMD8...\n");
        /* Check voltage range */
        cs_select();
        response = send_command(CMD8, 0x01AAU);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "CMD8 response: 0x%02X\n", response);
        if (R1_IDLE_STATE == response) {
            /* Read the rest of R7 response */
            for (int i = 0; i < 4; i++) {
                r7_resp[i] = spi_transfer(0xFFU);
            }
            MICROSD_LOG(MICROSD_LOG_DEBUG, "R7 response: 0x%02X 0x%02X 0x%02X 0x%02X\n", r7_resp[0],
                        r7_resp[1], r7_resp[2], r7_resp[3]);
        }
        cs_deselect();

        /* Initialize card */
        MICROSD_LOG(MICROSD_LOG_INFO, "Sending ACMD41 to initialize...\n");
        timeout = 1000;
        do {
            cs_select();
            (void)send_command(CMD55, 0U);
            response = send_command(ACMD41, 0x40000000U);
            cs_deselect();
            MICROSD_LOG(MICROSD_LOG_DEBUG, "ACMD41 response: 0x%02X (timeout=%d)\n", response,
                        timeout);
            sleep_ms(1U);
            timeout--;
        } while ((R1_READY_STATE != response) && (timeout > 0));

        if (timeout > 0) {
            MICROSD_LOG(MICROSD_LOG_INFO, "Card initialized successfully!\n");
            /* Increase SPI speed */
            (void)spi_set_baudrate(SD_SPI_PORT, 10000000U); /* 10MHz */
            MICROSD_LOG(MICROSD_LOG_INFO, "SPI speed increased to 10MHz\n");
            g_driver_info.is_initialized = true;
            result = true;
        } else {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Card initialization timed out\n");
            g_driver_info.last_error_code = MICROSD_ERROR_ACMD41_TIMEOUT;
        }
    } else {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Card did not enter idle state (response: 0x%02X)\n",
                    response);
        g_driver_info.last_error_code = MICROSD_ERROR_CMD0_FAILED;
    }

    if (!result) {
        g_driver_info.last_error_code = MICROSD_ERROR_INIT_FAILED;
    }

    return result;
}

bool microsd_read_block(uint32_t const block_num, uint8_t *const p_buffer) {
    bool result = false;
    uint8_t response;
    int timeout;

    if (NULL == p_buffer) {
        return false;
    }

    /* HIGH PRIORITY FIX: Add retry logic for transient read failures */
    for (int retry_attempt = 0; retry_attempt < MAX_READ_RETRIES; retry_attempt++) {
        if (retry_attempt > 0) {
            MICROSD_LOG(MICROSD_LOG_WARN, "Read retry attempt %d/%d for block %lu\n",
                        retry_attempt + 1, MAX_READ_RETRIES, (unsigned long)block_num);
            sleep_ms(READ_RETRY_DELAY_MS);
        }

        MICROSD_LOG(MICROSD_LOG_DEBUG, "Reading block %lu\n", (unsigned long)block_num);

        /* Add small delay before read to prevent overwhelming SD card */
        sleep_us(INTER_READ_DELAY_US);

        cs_select();

        response = send_command(CMD17, block_num);
        if (0x00U == response) {
            /* HIGH PRIORITY FIX: Increased timeout from 1000 to 2000 iterations */
            timeout = READ_TIMEOUT_MS;
            do {
                response = spi_transfer(0xFFU);
                sleep_us(1); /* Small delay to give card time to respond */
                timeout--;
            } while ((0xFEU != response) && (timeout > 0));

            if (timeout > 0) {
                /* Read 512 bytes */
                for (int i = 0; i < SD_BLOCK_SIZE; i++) {
                    p_buffer[i] = spi_transfer(0xFFU);
                }

                /* Read CRC (ignore) */
                (void)spi_transfer(0xFFU);
                (void)spi_transfer(0xFFU);

                g_driver_info.total_reads++;
                result = true;
                MICROSD_LOG(MICROSD_LOG_DEBUG, "Block read successful\n");
                cs_deselect();
                break; /* Success - exit retry loop */
            } else {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Read timeout waiting for data token\n");
                g_driver_info.last_error_code = MICROSD_ERROR_READ_FAILED;
            }
        } else {
            MICROSD_LOG(MICROSD_LOG_ERROR, "CMD17 failed with response 0x%02X\n", response);
            g_driver_info.last_error_code = MICROSD_ERROR_READ_FAILED;
        }

        cs_deselect();

        /* If not the last attempt, log that we'll retry */
        if (!result && (retry_attempt < MAX_READ_RETRIES - 1)) {
            MICROSD_LOG(MICROSD_LOG_WARN, "Read failed, will retry...\n");
        }
    }

    if (!result) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Block %lu read failed after %d attempts\n",
                    (unsigned long)block_num, MAX_READ_RETRIES);
    }

    return result;
}

bool microsd_write_block(uint32_t const block_num, uint8_t const *const p_buffer) {
    bool result = false;
    uint8_t response;
    int timeout;

    if (NULL != p_buffer) {
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Starting write to block %lu\n", (unsigned long)block_num);
        cs_select();

        response = send_command(CMD24, block_num);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "CMD24 response: 0x%02X\n", response);
        if (0x00U == response) {
            /* Send data token */
            (void)spi_transfer(0xFEU);
            MICROSD_LOG(MICROSD_LOG_DEBUG, "Sent data token 0xFE\n");

            /* Send 512 bytes */
            for (int i = 0; i < SD_BLOCK_SIZE; i++) {
                (void)spi_transfer(p_buffer[i]);
            }
            MICROSD_LOG(MICROSD_LOG_DEBUG, "Sent %d bytes of data\n", SD_BLOCK_SIZE);

            /* Send dummy CRC */
            (void)spi_transfer(0xFFU);
            (void)spi_transfer(0xFFU);
            MICROSD_LOG(MICROSD_LOG_DEBUG, "Sent CRC\n");

            /* Check data response */
            response = spi_transfer(0xFFU);
            MICROSD_LOG(MICROSD_LOG_DEBUG, "Data response: 0x%02X\n", response);
            if (0x05U == (response & 0x1FU)) {
                MICROSD_LOG(MICROSD_LOG_DEBUG, "Data accepted by card\n");
                /* Wait for write to complete */
                timeout = 5000; /* Increased timeout to 5000 iterations */
                int busy_count = 0;
                do {
                    response = spi_transfer(0xFFU);
                    if (response == 0x00U) {
                        busy_count++;
                    }
                    if ((timeout % 1000) == 0) {
                        MICROSD_LOG(
                            MICROSD_LOG_DEBUG,
                            "Still waiting... response: 0x%02X, busy_count: %d, timeout: %d\n",
                            response, busy_count, timeout);
                    }
                    sleep_ms(1U); /* Add actual delay */
                    timeout--;
                } while ((0x00U == response) && (timeout > 0));

                MICROSD_LOG(MICROSD_LOG_DEBUG,
                            "Final write completion response: 0x%02X (timeout=%d, busy_count=%d)\n",
                            response, timeout, busy_count);
                if (timeout > 0) {
                    MICROSD_LOG(MICROSD_LOG_DEBUG, "Write completed successfully!\n");
                    g_driver_info.total_writes++;
                    result = true;
                } else {
                    MICROSD_LOG(MICROSD_LOG_ERROR, "Write completion timed out\n");
                    g_driver_info.last_error_code = MICROSD_ERROR_WRITE_FAILED;
                }
            } else {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Data rejected by card (0x%02X)\n", response);
                if ((response & 0x1FU) == 0x0B) {
                    MICROSD_LOG(MICROSD_LOG_ERROR, "Reason: CRC error\n");
                } else if ((response & 0x1FU) == 0x0D) {
                    MICROSD_LOG(MICROSD_LOG_ERROR, "Reason: Write error\n");
                } else {
                    MICROSD_LOG(MICROSD_LOG_ERROR, "Reason: Unknown error\n");
                }
                g_driver_info.last_error_code = MICROSD_ERROR_WRITE_FAILED;
            }
        } else {
            MICROSD_LOG(MICROSD_LOG_ERROR, "CMD24 failed with response 0x%02X\n", response);
            g_driver_info.last_error_code = MICROSD_ERROR_WRITE_FAILED;
        }

        cs_deselect();
    }

    return result;
}

bool microsd_read_byte(uint32_t const address, uint8_t *const p_data) {
    bool result = false;
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t const block_num = address / SD_BLOCK_SIZE;
    uint32_t const byte_offset = address % SD_BLOCK_SIZE;

    if (NULL != p_data) {
        if (microsd_read_block(block_num, buffer)) {
            *p_data = buffer[byte_offset];
            result = true;
        }
    }

    return result;
}

bool microsd_write_byte(uint32_t const address, uint8_t const data) {
    bool result = false;
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t const block_num = address / SD_BLOCK_SIZE;
    uint32_t const byte_offset = address % SD_BLOCK_SIZE;

    /* Read existing block first (Read-Modify-Write operation) */
    if (microsd_read_block(block_num, buffer)) {
        /* Modify the specific byte */
        buffer[byte_offset] = data;

        /* Write the modified block back to SD card */
        result = microsd_write_block(block_num, buffer);
    }

    return result;
}

/*!
 * @brief Read multiple bytes from SD card
 * @param[in] address     Starting address to read from
 * @param[out] p_buffer   Buffer to store read data
 * @param[in] length      Number of bytes to read
 * @return bool           true if successful, false otherwise
 */
bool microsd_read_bytes(uint32_t const address, uint8_t *const p_buffer, uint32_t const length) {
    bool result = false;

    if ((NULL != p_buffer) && (length > 0U)) {
        result = true;
        for (uint32_t i = 0U; i < length; i++) {
            if (!microsd_read_byte(address + i, &p_buffer[i])) {
                result = false;
                break;
            }
        }
    }

    return result;
}

/*!
 * @brief Write multiple bytes to SD card
 * @param[in] address     Starting address to write to
 * @param[in] p_buffer    Buffer containing data to write
 * @param[in] length      Number of bytes to write
 * @return bool           true if successful, false otherwise
 */
bool microsd_write_bytes(uint32_t const address, uint8_t const *const p_buffer,
                         uint32_t const length) {
    bool result = false;

    if ((NULL != p_buffer) && (length > 0U)) {
        result = true;
        for (uint32_t i = 0U; i < length; i++) {
            if (!microsd_write_byte(address + i, p_buffer[i])) {
                result = false;
                break;
            }
        }
    }

    return result;
}

/*!
 * @brief Get driver information and statistics
 * @param[out] p_info   Pointer to driver info structure
 * @return bool         true on success, false on failure
 */
bool microsd_get_driver_info(microsd_driver_info_t *const p_info) {
    if (NULL == p_info) {
        return false;
    }

    memcpy(p_info, &g_driver_info, sizeof(microsd_driver_info_t));
    return true;
}

/*!
 * @brief Set logging level for debugging
 * @param[in] level     Logging level to set
 * @return void
 */
void microsd_set_log_level(microsd_log_level_t level) {
    g_log_level = level;
    MICROSD_LOG(MICROSD_LOG_INFO, "Log level set to %d\n", level);
}

/*!
 * @brief Print driver identification banner
 * @return void
 */
void microsd_print_banner(void) {
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║                MicroSD Driver Information                ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ Driver ID    : %-40s ║\n", MICROSD_DRIVER_ID);
    printf("║ UUID         : %-40s ║\n", MICROSD_DRIVER_UUID);
    printf("║ Version      : v%d.%d.%d                                 ║\n",
           MICROSD_DRIVER_VERSION_MAJOR, MICROSD_DRIVER_VERSION_MINOR,
           MICROSD_DRIVER_VERSION_PATCH);
    printf("║ Status       : %-40s ║\n",
           g_driver_info.is_initialized ? "INITIALIZED" : "NOT INITIALIZED");
    printf("║ Total Reads  : %-40lu ║\n", (unsigned long)g_driver_info.total_reads);
    printf("║ Total Writes : %-40lu ║\n", (unsigned long)g_driver_info.total_writes);
    printf("║ Last Error   : 0x%04X                                   ║\n",
           g_driver_info.last_error_code);
    printf("╚══════════════════════════════════════════════════════════╝\n");
}

/*!
 * @brief Add log message to buffer for later writing to SD card
 * @param[in] level     Log level
 * @param[in] fmt       Format string
 * @param[in] ...       Variable arguments
 * @return void
 */
static void add_to_log_buffer(microsd_log_level_t level, const char *fmt, ...) {
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

/*!
 * @brief Enable/disable logging to file
 * @param[in] enable    true to enable, false to disable
 * @return void
 */
void microsd_enable_file_logging(bool enable) {
    g_log_to_file_enabled = enable;
    if (enable) {
        g_log_buffer_pos = 0; /* Reset buffer when enabling */
        memset(g_log_buffer, 0, LOG_BUFFER_SIZE);
    }
}

/*!
 * @brief Flush log buffer to SD card file
 * @param[in] p_fs_info     Filesystem info pointer
 * @return bool             true on success, false on failure
 */
static bool flush_log_to_file(filesystem_info_t const *const p_fs_info) {
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

// microsd_init_filesystem() implementation moved to microsd_exfat.c

/*!
 * @brief Check if cluster is free in allocation bitmap
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] cluster       Cluster number to check
 * @param[in] bitmap_cluster First cluster of allocation bitmap
 * @return bool             true if cluster is free (bit = 0), false if allocated (bit = 1)
 */
static bool is_cluster_free_in_bitmap(filesystem_info_t const *const p_fs_info, uint32_t cluster,
                                      uint32_t bitmap_cluster) {
    uint8_t buffer[SD_BLOCK_SIZE];

    /* Calculate which byte and bit within the bitmap (clusters start from index 2) */
    uint32_t bit_index = cluster - 2;
    uint32_t byte_offset = bit_index / 8;
    uint8_t bit_offset = bit_index % 8;

    /* Calculate which sector contains this byte */
    uint32_t bitmap_sector = cluster_to_sector(p_fs_info, bitmap_cluster);
    uint32_t sector_offset = byte_offset / SD_BLOCK_SIZE;
    uint32_t byte_in_sector = byte_offset % SD_BLOCK_SIZE;

    /* Read the bitmap sector */
    if (!microsd_read_block(bitmap_sector + sector_offset, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read allocation bitmap sector\n");
        return false; /* Assume allocated if we can't read */
    }

    /* Check if the bit is 0 (free) or 1 (allocated) */
    bool is_allocated = (buffer[byte_in_sector] & (1 << bit_offset)) != 0;
    return !is_allocated;
}

/*!
 * @brief Find allocation bitmap cluster in root directory
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @return uint32_t         Allocation bitmap cluster, or 0 if not found
 */
static uint32_t find_allocation_bitmap_cluster(filesystem_info_t const *const p_fs_info) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t root_sector = cluster_to_sector(p_fs_info, p_fs_info->root_cluster);

    /* Read root directory to find allocation bitmap directory entry */
    if (!microsd_read_block(root_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read root directory for bitmap search\n");
        return 0;
    }

    /* Search for allocation bitmap directory entry (TypeCode = 1, TypeImportance = 0) */
    for (int i = 0; i < SD_BLOCK_SIZE; i += 32) {
        uint8_t entry_type = buffer[i];

        /* Check if this is an allocation bitmap directory entry */
        if (entry_type == 0x81) { /* 0x81 = TypeCode=1, TypeImportance=0, TypeCategory=0, InUse=1 */
            /* Extract FirstCluster from bytes 20-23 */
            uint32_t bitmap_cluster = *(uint32_t *)&buffer[i + 20];
            return bitmap_cluster;
        }

        /* Stop at end-of-directory marker */
        if (entry_type == 0x00) {
            break;
        }
    }

    return 0;
}

/*!
 * @brief Find a free cluster (wrapper for module function)
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @return uint32_t         Free cluster number, or 0 if none found
 */
static uint32_t find_free_cluster(filesystem_info_t const *const p_fs_info) {
    return microsd_find_free_cluster(p_fs_info);
}

/*!
 * @brief Mark cluster as end-of-chain in FAT
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] cluster       Cluster number to mark
 * @return bool             true on success, false on failure
 */
static bool mark_cluster_end(filesystem_info_t const *const p_fs_info, uint32_t cluster) {
    return microsd_mark_cluster_end(p_fs_info, cluster);
}

/*!
 * @brief Calculate cluster start sector (wrapper for module function)
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] cluster       Cluster number
 * @return uint32_t         Sector number of cluster start
 */
static uint32_t cluster_to_sector(filesystem_info_t const *const p_fs_info, uint32_t cluster) {
    return microsd_cluster_to_sector(p_fs_info, cluster);
}

/*!
 * @brief Calculate FAT sector and offset (wrapper for module function)
 * @param[in]  p_fs_info    Pointer to filesystem info structure
 * @param[in]  cluster      Cluster number to locate in FAT
 * @param[out] fat_sector   Pointer to store the absolute sector number
 * @param[out] entry_offset Pointer to store the byte offset within sector
 * @return void
 */
static void get_fat_entry_location(filesystem_info_t const *const p_fs_info, uint32_t cluster,
                                   uint32_t *fat_sector, uint32_t *entry_offset) {
    microsd_get_fat_entry_location(p_fs_info, cluster, fat_sector, entry_offset);
}

/*!
 * @brief Expand directory by allocating a new cluster and linking it
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] dir_cluster   Directory cluster to expand
 * @return bool             true on success, false on failure
 */
static bool expand_directory(filesystem_info_t const *const p_fs_info, uint32_t dir_cluster) {
    return microsd_expand_directory(p_fs_info, dir_cluster);
}

/*!
 * @brief Check if directory needs expansion and expand if necessary
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] dir_cluster   Directory cluster to check
 * @param[in] entries_needed Number of entries needed
 * @return bool             true if space is available (after expansion if needed)
 */
static bool ensure_directory_space(filesystem_info_t const *const p_fs_info, uint32_t dir_cluster,
                                   uint32_t entries_needed) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t current_cluster = dir_cluster;
    uint32_t total_available_entries = 0;

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Checking directory space for %lu entries\n",
                (unsigned long)entries_needed);

    /* Walk through all clusters in the directory chain */
    while (current_cluster != 0xFFFFFFFF) {
        uint32_t cluster_sector = cluster_to_sector(p_fs_info, current_cluster);

        /* Check each sector in this cluster */
        for (uint32_t sector = 0; sector < p_fs_info->sectors_per_cluster; sector++) {
            if (!microsd_read_block(cluster_sector + sector, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read directory sector %lu\n",
                            (unsigned long)(cluster_sector + sector));
                return false;
            }

            /* Count available entries in this sector */
            uint32_t entries_in_sector = SD_BLOCK_SIZE / 32;
            for (uint32_t i = 0; i < entries_in_sector; i++) {
                uint8_t entry_type = buffer[i * 32];
                if (entry_type == 0x00) {
                    /* Found end-of-directory, count remaining entries */
                    total_available_entries += (entries_in_sector - i);
                    goto space_check_complete;
                }
            }
        }

        /* Move to next cluster in chain */
        uint32_t fat_sector, entry_offset;
        get_fat_entry_location(p_fs_info, current_cluster, &fat_sector, &entry_offset);

        if (!microsd_read_block(fat_sector, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector %lu\n",
                        (unsigned long)fat_sector);
            return false;
        }

        uint32_t *fat_entry = (uint32_t *)(buffer + entry_offset);
        current_cluster = *fat_entry;
    }

space_check_complete:
    MICROSD_LOG(MICROSD_LOG_DEBUG, "Total available entries in directory: %lu\n",
                (unsigned long)total_available_entries);

    if (total_available_entries >= entries_needed) {
        return true;
    }

    /* Need to expand directory */
    MICROSD_LOG(MICROSD_LOG_INFO, "Directory needs expansion (%lu available, %lu needed)\n",
                (unsigned long)total_available_entries, (unsigned long)entries_needed);

    /* Calculate how many clusters we need to add */
    uint32_t entries_per_cluster = p_fs_info->sectors_per_cluster * (SD_BLOCK_SIZE / 32);
    uint32_t clusters_needed =
        ((entries_needed - total_available_entries) + entries_per_cluster - 1) /
        entries_per_cluster;

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Need to add %lu cluster(s) for directory expansion\n",
                (unsigned long)clusters_needed);

    /* Expand directory by adding required clusters */
    for (uint32_t i = 0; i < clusters_needed; i++) {
        if (!expand_directory(p_fs_info, dir_cluster)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to expand directory (iteration %lu)\n",
                        (unsigned long)i);
            return false;
        }
    }

    return true;
}

// File operation helper functions moved to microsd_file.c:
// - calculate_exfat_name_hash()
// - calculate_entry_checksum()
// - write_cluster_chain_data()
// - get_next_cluster()

/* Wrapper functions for backward compatibility */
static bool update_allocation_bitmap(filesystem_info_t const *const p_fs_info, uint32_t cluster) {
    return microsd_update_allocation_bitmap(p_fs_info, cluster);
}

static bool allocate_cluster_chain(filesystem_info_t const *const p_fs_info, uint32_t data_length,
                                   uint32_t *const p_first_cluster) {
    return microsd_allocate_cluster_chain(p_fs_info, data_length, p_first_cluster);
}

// microsd_create_file() and microsd_read_file() implementations moved to microsd_file.c

// Helper functions still needed by large file operations in this file:
static uint16_t calculate_exfat_name_hash(char const *const filename, uint32_t length) {
    uint16_t hash = 0;
    for (uint32_t i = 0; i < length; i++) {
        uint16_t c = (uint16_t)(uint8_t)filename[i];
        if (c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
        hash = ((hash & 1) ? 0x8000 : 0) + (hash >> 1) + c;
    }
    return hash;
}

static uint16_t calculate_entry_checksum(uint8_t const *const p_entries, uint32_t count) {
    uint16_t checksum = 0;
    for (uint32_t i = 0; i < count * 32; i++) {
        if (i == 2 || i == 3) {
            continue;
        }
        checksum = ((checksum & 1) ? 0x8000 : 0) + (checksum >> 1) + p_entries[i];
    }
    return checksum;
}

static uint32_t get_next_cluster(filesystem_info_t const *const p_fs_info, uint32_t cluster) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t fat_sector, fat_offset;
    uint32_t next_cluster;

    microsd_get_fat_entry_location(p_fs_info, cluster, &fat_sector, &fat_offset);

    if (!microsd_read_block(fat_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector\n");
        return 0xFFFFFFFF;
    }

    next_cluster = (uint32_t)buffer[fat_offset] | ((uint32_t)buffer[fat_offset + 1] << 8) |
                   ((uint32_t)buffer[fat_offset + 2] << 16) |
                   ((uint32_t)buffer[fat_offset + 3] << 24);

    if (next_cluster >= 0xFFFFFFF8) {
        return 0xFFFFFFFF;
    }

    return next_cluster;
}

/*! ========================================================================
 *  PUBLIC API WRAPPERS - Filesystem Operations
 *  These forward to modular implementations in microsd_file.c
 *  ======================================================================== */

// microsd_init_filesystem() - implemented in microsd_exfat.c
// microsd_create_file() - implemented in microsd_file.c
// microsd_read_file() - implemented in microsd_file.c

/*! ========================================================================
 *  PUBLIC API WRAPPERS - Chunk Operations
 *  These forward to modular implementations in microsd_chunk.c
 *  ======================================================================== */

// microsd_init_chunk_write() - implemented in microsd_chunk.c
// microsd_write_chunk() - implemented in microsd_chunk.c
// microsd_finalize_chunk_write() - implemented in microsd_chunk.c
// microsd_check_all_chunks_received() - implemented in microsd_chunk.c
// microsd_read_chunk() - implemented in microsd_chunk.c

/*! ========================================================================
 *  PUBLIC API WRAPPERS - Utility Functions
 *  These forward to modular implementations in microsd_util.c
 *  ======================================================================== */

// microsd_list_directory() - implemented in microsd_util.c
// microsd_hex_dump() - implemented in microsd_util.c
// get_current_time() - implemented in microsd_util.c
