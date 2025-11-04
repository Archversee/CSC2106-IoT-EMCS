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
#include "pico/stdlib.h"

/*! Driver state and statistics */
static microsd_driver_info_t g_driver_info = {0};
static microsd_log_level_t g_log_level = MICROSD_LOG_INFO;

/*! Log buffer for writing to SD card */
#define LOG_BUFFER_SIZE 8192
static char g_log_buffer[LOG_BUFFER_SIZE];
static uint32_t g_log_buffer_pos = 0;
static bool g_log_to_file_enabled = false;

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
#define INTER_READ_DELAY_US \
    100 /* Small delay between consecutive reads to prevent overwhelming card */

/*! Forward declarations for log buffer functions */
static void add_to_log_buffer(microsd_log_level_t level, const char* fmt, ...);
static bool flush_log_to_file(filesystem_info_t const* const p_fs_info);

/*! Logging macro */
#define MICROSD_LOG(level, fmt, ...)                          \
    do {                                                      \
        if (g_log_level >= level) {                           \
            printf("[%s:%s] " fmt, MICROSD_DRIVER_ID,         \
                   (level == MICROSD_LOG_ERROR)  ? "ERROR"    \
                   : (level == MICROSD_LOG_WARN) ? "WARN"     \
                   : (level == MICROSD_LOG_INFO) ? "INFO"     \
                                                 : "DEBUG",   \
                   ##__VA_ARGS__);                            \
            if (g_log_to_file_enabled) {                      \
                add_to_log_buffer(level, fmt, ##__VA_ARGS__); \
            }                                                 \
        }                                                     \
    } while (0)

/*! Private function prototypes */
static uint8_t spi_transfer(uint8_t const data);
static void cs_select(void);
static void cs_deselect(void);
static uint8_t send_command(uint8_t const cmd, uint32_t const arg);
static bool expand_directory(filesystem_info_t const* const p_fs_info, uint32_t dir_cluster);
static bool update_allocation_bitmap(filesystem_info_t const* const p_fs_info, uint32_t cluster);
static bool mark_cluster_end(filesystem_info_t const* const p_fs_info, uint32_t cluster);
static uint32_t find_free_cluster(filesystem_info_t const* const p_fs_info);
static uint32_t cluster_to_sector(filesystem_info_t const* const p_fs_info, uint32_t cluster);
static bool is_cluster_free_in_bitmap(filesystem_info_t const* const p_fs_info, uint32_t cluster,
                                      uint32_t bitmap_cluster);
static uint32_t find_allocation_bitmap_cluster(filesystem_info_t const* const p_fs_info);
static void get_fat_entry_location(filesystem_info_t const* const p_fs_info, uint32_t cluster,
                                   uint32_t* fat_sector, uint32_t* entry_offset);

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

bool microsd_read_block(uint32_t const block_num, uint8_t* const p_buffer) {
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

bool microsd_write_block(uint32_t const block_num, uint8_t const* const p_buffer) {
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

bool microsd_read_byte(uint32_t const address, uint8_t* const p_data) {
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
bool microsd_read_bytes(uint32_t const address, uint8_t* const p_buffer, uint32_t const length) {
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
bool microsd_write_bytes(uint32_t const address, uint8_t const* const p_buffer,
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
bool microsd_get_driver_info(microsd_driver_info_t* const p_info) {
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
static void add_to_log_buffer(microsd_log_level_t level, const char* fmt, ...) {
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
static bool flush_log_to_file(filesystem_info_t const* const p_fs_info) {
    if (!g_log_to_file_enabled || g_log_buffer_pos == 0 || !p_fs_info) {
        return true; /* Nothing to do */
    }

    /* Create log filename with driver UUID */
    char log_filename[64];
    snprintf(log_filename, sizeof(log_filename), "%s.log", MICROSD_DRIVER_UUID);

    /* Use the existing file creation function to write log data */
    bool result =
        microsd_create_file(p_fs_info, log_filename, (uint8_t*)g_log_buffer, g_log_buffer_pos);

    if (result) {
        /* Reset buffer after successful write */
        g_log_buffer_pos = 0;
        memset(g_log_buffer, 0, LOG_BUFFER_SIZE);
    }

    return result;
}

/*!
 * @brief Initialize filesystem information
 * @param[out] p_fs_info    Pointer to filesystem info structure
 * @return bool             true on success, false on failure
 */
bool microsd_init_filesystem(filesystem_info_t* const p_fs_info) {
    uint8_t buffer[SD_BLOCK_SIZE];
    mbr_t* mbr;
    exfat_boot_sector_t* boot_sector;
    uint32_t partition_start = 0;

    if (NULL == p_fs_info) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid filesystem info pointer\n");
        return false;
    }

    /* Initialize partition offset */
    p_fs_info->partition_offset = 0;

    /* Read block 0 (MBR or boot sector) */
    if (!microsd_read_block(0, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read block 0\n");
        return false;
    }

    /* Check boot sector signature first */
    if (buffer[510] != 0x55 || buffer[511] != 0xAA) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid boot sector signature\n");
        return false;
    }

    /* Debug: Show what we read from block 0 */
    MICROSD_LOG(MICROSD_LOG_DEBUG, "Block 0 signature: %02X %02X\n", buffer[510], buffer[511]);

    /* Hex dump first 64 bytes */
    MICROSD_LOG(MICROSD_LOG_DEBUG, "Block 0 hex dump (first 64 bytes):\n");
    for (int i = 0; i < 64; i += 16) {
        MICROSD_LOG(MICROSD_LOG_DEBUG,
                    "%04X: %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X "
                    "%02X %02X\n",
                    i, buffer[i + 0], buffer[i + 1], buffer[i + 2], buffer[i + 3], buffer[i + 4],
                    buffer[i + 5], buffer[i + 6], buffer[i + 7], buffer[i + 8], buffer[i + 9],
                    buffer[i + 10], buffer[i + 11], buffer[i + 12], buffer[i + 13], buffer[i + 14],
                    buffer[i + 15]);
    }

    /* Check if this is a direct filesystem boot sector (no partition table) */
    if (memcmp(&buffer[3], "EXFAT   ", 8) == 0) {
        MICROSD_LOG(MICROSD_LOG_INFO, "Direct exFAT filesystem detected (no partition table)\n");
        partition_start = 0;
    } else if (memcmp(&buffer[82], "FAT32   ", 8) == 0 || memcmp(&buffer[54], "FAT16   ", 8) == 0) {
        MICROSD_LOG(MICROSD_LOG_INFO, "Direct FAT filesystem detected (not exFAT)\n");
        return false;
    } else {
        /* This is likely an MBR with partition table */
        MICROSD_LOG(MICROSD_LOG_INFO, "MBR with partition table detected\n");

        mbr = (mbr_t*)buffer;

        /* Look for exFAT partition (type 0x07) */
        bool partition_found = false;
        for (int i = 0; i < 4; i++) {
            MICROSD_LOG(MICROSD_LOG_DEBUG, "Partition %d: type=0x%02X, first_lba=%lu, size=%lu\n",
                        i, mbr->partitions[i].partition_type,
                        (unsigned long)mbr->partitions[i].first_lba,
                        (unsigned long)mbr->partitions[i].sector_count);

            if (mbr->partitions[i].partition_type == 0x07 && mbr->partitions[i].sector_count > 0) {
                partition_start = mbr->partitions[i].first_lba;
                partition_found = true;
                MICROSD_LOG(MICROSD_LOG_INFO, "Found exFAT partition at sector %lu\n",
                            (unsigned long)partition_start);
                break;
            }
        }

        if (!partition_found) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "No exFAT partition found in MBR\n");
            return false;
        }

        /* Read the actual boot sector from the partition */
        if (!microsd_read_block(partition_start, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read partition boot sector at sector %lu\n",
                        (unsigned long)partition_start);
            return false;
        }

        MICROSD_LOG(MICROSD_LOG_DEBUG, "Partition boot sector hex dump (first 32 bytes):\n");
        for (int i = 0; i < 32; i += 16) {
            MICROSD_LOG(MICROSD_LOG_DEBUG,
                        "%04X: %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X "
                        "%02X %02X %02X\n",
                        i, buffer[i + 0], buffer[i + 1], buffer[i + 2], buffer[i + 3],
                        buffer[i + 4], buffer[i + 5], buffer[i + 6], buffer[i + 7], buffer[i + 8],
                        buffer[i + 9], buffer[i + 10], buffer[i + 11], buffer[i + 12],
                        buffer[i + 13], buffer[i + 14], buffer[i + 15]);
        }
    }

    /* Now parse the exFAT boot sector */
    boot_sector = (exfat_boot_sector_t*)buffer;

    /* Check if it's exFAT filesystem */
    if (memcmp(boot_sector->filesystem_name, "EXFAT   ", 8) == 0) {
        MICROSD_LOG(MICROSD_LOG_INFO, "exFAT filesystem confirmed in partition\n");

        p_fs_info->is_exfat = true;
        p_fs_info->partition_offset = partition_start;

        /* Debug: Check volume flags for dirty bit and other states (AFTER setting partition_offset)
         */
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Volume flags: 0x%04X\n", boot_sector->volume_flags);
        if (boot_sector->volume_flags & 0x0002) {
            MICROSD_LOG(MICROSD_LOG_INFO, "Volume dirty bit is set - clearing it\n");
            boot_sector->volume_flags &= ~0x0002; /* Clear dirty bit */

            /* Write back the corrected boot sector to the CORRECT partition location */
            if (!microsd_write_block(p_fs_info->partition_offset, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write corrected boot sector\n");
            } else {
                MICROSD_LOG(MICROSD_LOG_INFO, "Volume dirty bit cleared successfully\n");
            }
        }
        p_fs_info->bytes_per_sector = 1U << boot_sector->bytes_per_sector_shift;
        p_fs_info->sectors_per_cluster = 1U << boot_sector->sectors_per_cluster_shift;
        p_fs_info->bytes_per_cluster = p_fs_info->bytes_per_sector * p_fs_info->sectors_per_cluster;
        p_fs_info->fat_offset = boot_sector->fat_offset;
        p_fs_info->fat_length = boot_sector->fat_length;
        p_fs_info->cluster_heap_offset = boot_sector->cluster_heap_offset;
        p_fs_info->cluster_count = boot_sector->cluster_count;
        p_fs_info->root_cluster = boot_sector->first_cluster_of_root;

        MICROSD_LOG(MICROSD_LOG_DEBUG, "Partition offset: %lu\n",
                    (unsigned long)p_fs_info->partition_offset);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Bytes per sector: %lu\n",
                    (unsigned long)p_fs_info->bytes_per_sector);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Sectors per cluster: %lu\n",
                    (unsigned long)p_fs_info->sectors_per_cluster);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Bytes per cluster: %lu\n",
                    (unsigned long)p_fs_info->bytes_per_cluster);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "FAT offset: %lu\n", (unsigned long)p_fs_info->fat_offset);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Cluster heap offset: %lu\n",
                    (unsigned long)p_fs_info->cluster_heap_offset);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Root cluster: %lu\n",
                    (unsigned long)p_fs_info->root_cluster);

        return true;
    } else {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Partition does not contain exFAT filesystem\n");
        MICROSD_LOG(MICROSD_LOG_DEBUG,
                    "Filesystem name bytes: %02X %02X %02X %02X %02X %02X %02X %02X\n",
                    boot_sector->filesystem_name[0], boot_sector->filesystem_name[1],
                    boot_sector->filesystem_name[2], boot_sector->filesystem_name[3],
                    boot_sector->filesystem_name[4], boot_sector->filesystem_name[5],
                    boot_sector->filesystem_name[6], boot_sector->filesystem_name[7]);
        return false;
    }
}

/*!
 * @brief Check if cluster is free in allocation bitmap
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] cluster       Cluster number to check
 * @param[in] bitmap_cluster First cluster of allocation bitmap
 * @return bool             true if cluster is free (bit = 0), false if allocated (bit = 1)
 */
static bool is_cluster_free_in_bitmap(filesystem_info_t const* const p_fs_info, uint32_t cluster,
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
static uint32_t find_allocation_bitmap_cluster(filesystem_info_t const* const p_fs_info) {
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
            uint32_t bitmap_cluster = *(uint32_t*)&buffer[i + 20];
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
 * @brief Find a free cluster in the FAT (checks both FAT and allocation bitmap)
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @return uint32_t         Free cluster number, or 0 if none found
 */
static uint32_t find_free_cluster(filesystem_info_t const* const p_fs_info) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t fat_sector = p_fs_info->partition_offset + p_fs_info->fat_offset;
    uint32_t entries_per_sector = SD_BLOCK_SIZE / sizeof(uint32_t);

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Searching for free cluster in FAT (partition offset: %lu)\n",
                (unsigned long)p_fs_info->partition_offset);

    /* Find allocation bitmap cluster */
    uint32_t bitmap_cluster = find_allocation_bitmap_cluster(p_fs_info);
    if (bitmap_cluster == 0) {
        MICROSD_LOG(MICROSD_LOG_WARN, "Allocation bitmap not found, using FAT only\n");
    }

    /* Search through FAT sectors */
    for (uint32_t sector = 0; sector < p_fs_info->fat_length; sector++) {
        if (!microsd_read_block(fat_sector + sector, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector %lu\n",
                        (unsigned long)(fat_sector + sector));
            continue;
        }

        uint32_t* fat_entries = (uint32_t*)buffer;

        /* Debug: Show first few FAT entries to understand allocation */
        if (sector == 0) {
            MICROSD_LOG(MICROSD_LOG_DEBUG, "FAT entries for clusters 2-7: ");
            for (uint32_t i = 0; i < 6 && i < entries_per_sector; i++) {
                MICROSD_LOG(MICROSD_LOG_DEBUG, "cluster %lu=0x%08lX ", (unsigned long)(i + 2),
                            (unsigned long)fat_entries[i]);
            }
            MICROSD_LOG(MICROSD_LOG_DEBUG, "\n");
        }

        for (uint32_t entry = 0; entry < entries_per_sector; entry++) {
            uint32_t cluster = sector * entries_per_sector + entry + 2; /* Clusters start at 2 */

            if (cluster >= p_fs_info->cluster_count + 2) {
                break; /* Beyond valid cluster range */
            }

            /* Check if cluster is free in FAT */
            if (fat_entries[entry] == 0) {
                /* Also check allocation bitmap if available */
                if (bitmap_cluster != 0) {
                    if (is_cluster_free_in_bitmap(p_fs_info, cluster, bitmap_cluster)) {
                        MICROSD_LOG(MICROSD_LOG_DEBUG,
                                    "Found free cluster: %lu (FAT and bitmap both show free)\n",
                                    (unsigned long)cluster);
                        return cluster;
                    } else {
                        MICROSD_LOG(MICROSD_LOG_DEBUG,
                                    "Cluster %lu free in FAT but allocated in bitmap, skipping\n",
                                    (unsigned long)cluster);
                    }
                } else {
                    /* No bitmap available, trust FAT */
                    MICROSD_LOG(MICROSD_LOG_DEBUG, "Found free cluster: %lu\n",
                                (unsigned long)cluster);
                    return cluster;
                }
            }
        }
    }

    MICROSD_LOG(MICROSD_LOG_ERROR, "No free clusters found\n");
    return 0;
}

/*!
 * @brief Mark cluster as end-of-chain in FAT
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] cluster       Cluster number to mark
 * @return bool             true on success, false on failure
 */
static bool mark_cluster_end(filesystem_info_t const* const p_fs_info, uint32_t cluster) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t fat_sector, entry_offset;

    get_fat_entry_location(p_fs_info, cluster, &fat_sector, &entry_offset);

    /* Read FAT sector */
    if (!microsd_read_block(fat_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector %lu\n",
                    (unsigned long)fat_sector);
        return false;
    }

    /* Mark as end-of-chain (0xFFFFFFFF) */
    uint32_t* fat_entry = (uint32_t*)(buffer + entry_offset);
    *fat_entry = 0xFFFFFFFF;

    /* Write back */
    if (!microsd_write_block(fat_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write FAT sector %lu\n",
                    (unsigned long)fat_sector);
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Marked cluster %lu as end-of-chain\n", (unsigned long)cluster);
    return true;
}

/*!
 * @brief Calculate cluster start sector
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] cluster       Cluster number
 * @return uint32_t         Sector number of cluster start
 */
static uint32_t cluster_to_sector(filesystem_info_t const* const p_fs_info, uint32_t cluster) {
    return p_fs_info->partition_offset + p_fs_info->cluster_heap_offset +
           (cluster - 2) * p_fs_info->sectors_per_cluster;
}

/*!
 * @brief Calculate FAT sector and offset for a given cluster number
 *
 * This function centralizes the FAT entry location calculation to ensure
 * consistency across all FAT access operations. In exFAT, each FAT entry
 * is 4 bytes (32 bits), so for cluster N, the FAT entry is at byte offset
 * N * 4 from the start of the FAT.
 *
 * @param[in]  p_fs_info    Pointer to filesystem info structure
 * @param[in]  cluster      Cluster number to locate in FAT
 * @param[out] fat_sector   Pointer to store the absolute sector number
 * @param[out] entry_offset Pointer to store the byte offset within sector
 * @return void
 */
static void get_fat_entry_location(filesystem_info_t const* const p_fs_info, uint32_t cluster,
                                   uint32_t* fat_sector, uint32_t* entry_offset) {
    /* Each FAT entry is 4 bytes (32 bits) in exFAT */
    uint32_t byte_offset = cluster * sizeof(uint32_t);

    /* Calculate which sector contains this FAT entry */
    *fat_sector =
        p_fs_info->partition_offset + p_fs_info->fat_offset + (byte_offset / SD_BLOCK_SIZE);

    /* Calculate byte offset within that sector */
    *entry_offset = byte_offset % SD_BLOCK_SIZE;
}

/*!
 * @brief Expand directory by allocating a new cluster and linking it
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] dir_cluster   Directory cluster to expand
 * @return bool             true on success, false on failure
 */
static bool expand_directory(filesystem_info_t const* const p_fs_info, uint32_t dir_cluster) {
    uint32_t new_cluster;
    uint8_t buffer[SD_BLOCK_SIZE];

    MICROSD_LOG(MICROSD_LOG_INFO, "Expanding directory cluster %lu\n", (unsigned long)dir_cluster);

    /* Find a free cluster for directory expansion */
    new_cluster = find_free_cluster(p_fs_info);
    if (new_cluster == 0) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "No free cluster available for directory expansion\n");
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Allocated cluster %lu for directory expansion\n",
                (unsigned long)new_cluster);

    /* Initialize the new directory cluster with zeros */
    memset(buffer, 0, SD_BLOCK_SIZE);
    for (uint32_t sector = 0; sector < p_fs_info->sectors_per_cluster; sector++) {
        uint32_t target_sector = cluster_to_sector(p_fs_info, new_cluster) + sector;
        if (!microsd_write_block(target_sector, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR,
                        "Failed to initialize new directory cluster sector %lu\n",
                        (unsigned long)target_sector);
            return false;
        }
    }

    /* Find the last cluster in the directory chain */
    uint32_t current_cluster = dir_cluster;
    uint32_t last_cluster = dir_cluster;

    while (current_cluster != 0xFFFFFFFF) {
        last_cluster = current_cluster;

        /* Read FAT entry for current cluster */
        uint32_t fat_sector, entry_offset;
        get_fat_entry_location(p_fs_info, current_cluster, &fat_sector, &entry_offset);

        if (!microsd_read_block(fat_sector, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector %lu\n",
                        (unsigned long)fat_sector);
            return false;
        }

        uint32_t* fat_entry = (uint32_t*)(buffer + entry_offset);
        current_cluster = *fat_entry;

        /* If this is the end-of-chain, we found our last cluster */
        if (current_cluster == 0xFFFFFFFF) {
            break;
        }
    }

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Last cluster in directory chain: %lu\n",
                (unsigned long)last_cluster);

    /* Link the last cluster to the new cluster */
    uint32_t fat_sector, entry_offset;
    get_fat_entry_location(p_fs_info, last_cluster, &fat_sector, &entry_offset);

    if (!microsd_read_block(fat_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector %lu for linking\n",
                    (unsigned long)fat_sector);
        return false;
    }

    uint32_t* fat_entry = (uint32_t*)(buffer + entry_offset);
    *fat_entry = new_cluster;

    if (!microsd_write_block(fat_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write FAT sector %lu for linking\n",
                    (unsigned long)fat_sector);
        return false;
    }

    /* Mark the new cluster as end-of-chain */
    if (!mark_cluster_end(p_fs_info, new_cluster)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to mark new cluster %lu as end-of-chain\n",
                    (unsigned long)new_cluster);
        return false;
    }

    /* Update allocation bitmap for the new cluster */
    if (!update_allocation_bitmap(p_fs_info, new_cluster)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to update allocation bitmap for cluster %lu\n",
                    (unsigned long)new_cluster);
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Successfully expanded directory: linked cluster %lu -> %lu\n",
                (unsigned long)last_cluster, (unsigned long)new_cluster);

    return true;
}

/*!
 * @brief Check if directory needs expansion and expand if necessary
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] dir_cluster   Directory cluster to check
 * @param[in] entries_needed Number of entries needed
 * @return bool             true if space is available (after expansion if needed)
 */
static bool ensure_directory_space(filesystem_info_t const* const p_fs_info, uint32_t dir_cluster,
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

        uint32_t* fat_entry = (uint32_t*)(buffer + entry_offset);
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

/*!
 * @brief Calculate exFAT name hash according to specification
 * @param[in] filename      Filename to hash
 * @param[in] length        Length of filename
 * @return uint16_t         Calculated name hash
 */
static uint16_t calculate_exfat_name_hash(char const* const filename, uint32_t length) {
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

/*!
 * @brief Update allocation bitmap to mark cluster as used
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] cluster       Cluster number to mark as allocated
 * @return bool             true on success, false on failure
 */
static bool update_allocation_bitmap(filesystem_info_t const* const p_fs_info, uint32_t cluster) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t bitmap_cluster = 0;

    /* Find allocation bitmap by reading root directory entries */
    uint32_t root_sector = cluster_to_sector(p_fs_info, p_fs_info->root_cluster);

    /* Read root directory to find allocation bitmap directory entry */
    if (!microsd_read_block(root_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read root directory\n");
        return false;
    }

    /* Debug: Print all directory entries in root */
    MICROSD_LOG(MICROSD_LOG_INFO, "=== Root Directory Entries ===\n");
    for (int i = 0; i < SD_BLOCK_SIZE; i += 32) {
        uint8_t entry_type = buffer[i];
        if (entry_type == 0x00) {
            MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: End-of-directory marker\n", i / 32);
            break;
        } else if (entry_type != 0xFF) {
            MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: Type=0x%02X", i / 32, entry_type);
            if (entry_type & 0x80) {
                uint8_t type_code = entry_type & 0x1F;
                uint8_t type_importance = (entry_type >> 5) & 0x01;
                uint8_t type_category = (entry_type >> 6) & 0x01;
                MICROSD_LOG(MICROSD_LOG_INFO, " (TypeCode=%d, Importance=%d, Category=%d)\n",
                            type_code, type_importance, type_category);

                /* Print FirstCluster and DataLength ONLY for specific entry types */
                if (entry_type == 0x81 || entry_type == 0x82) {
                    /* Allocation Bitmap (0x81) or Up-case Table (0x82) - have FirstCluster */
                    uint32_t first_cluster = *(uint32_t*)&buffer[i + 20];
                    uint64_t data_length = *(uint64_t*)&buffer[i + 24];
                    MICROSD_LOG(MICROSD_LOG_INFO, "        FirstCluster=%u, DataLength=%llu\n",
                                first_cluster, (unsigned long long)data_length);
                } else if (entry_type == 0xC0) {
                    /* Stream Extension - contains file data information */
                    uint32_t first_cluster = *(uint32_t*)&buffer[i + 20];
                    uint64_t data_length = *(uint64_t*)&buffer[i + 24];
                    MICROSD_LOG(MICROSD_LOG_INFO, "        FirstCluster=%u, DataLength=%llu\n",
                                first_cluster, (unsigned long long)data_length);
                }
                /* File Entry (0x85) doesn't have FirstCluster/DataLength, skip */
            } else {
                MICROSD_LOG(MICROSD_LOG_INFO, " (Unused)\n");
            }
        }
    }
    MICROSD_LOG(MICROSD_LOG_INFO, "=== End Directory Entries ===\n");

    /* Search for allocation bitmap directory entry (TypeCode = 1, TypeImportance = 0) */
    for (int i = 0; i < SD_BLOCK_SIZE; i += 32) {
        uint8_t entry_type = buffer[i];

        /* Check if this is an allocation bitmap directory entry */
        /* TypeCode = 1 (bits 0-4), TypeImportance = 0 (bit 5), TypeCategory = 0 (bit 6), InUse = 1
         * (bit 7) */
        if (entry_type == 0x81) { /* 0x81 = TypeCode=1, TypeImportance=0, TypeCategory=0, InUse=1 */
            /* Extract FirstCluster from bytes 20-23 */
            bitmap_cluster = *(uint32_t*)&buffer[i + 20];
            MICROSD_LOG(MICROSD_LOG_INFO, "Found allocation bitmap at cluster %u\n",
                        bitmap_cluster);
            break;
        }

        /* Stop at end-of-directory marker */
        if (entry_type == 0x00) {
            break;
        }
    }

    if (bitmap_cluster == 0) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Could not find allocation bitmap directory entry\n");

        /* Check if root directory is completely empty (needs initialization) */
        bool directory_empty = true;
        for (int i = 0; i < SD_BLOCK_SIZE; i += 32) {
            if (buffer[i] != 0x00) {
                directory_empty = false;
                break;
            }
        }

        if (directory_empty) {
            MICROSD_LOG(MICROSD_LOG_ERROR,
                        "Root directory is empty - this indicates an improperly formatted exFAT "
                        "filesystem\n");
            MICROSD_LOG(MICROSD_LOG_ERROR,
                        "Please format the SD card with a proper exFAT filesystem before use\n");
            return false;
        }
    }

    /* Now read the actual allocation bitmap */
    uint32_t bitmap_sector = cluster_to_sector(p_fs_info, bitmap_cluster);

    /* Calculate which byte and bit within the bitmap (clusters start from index 2) */
    uint32_t bit_index = cluster - 2;
    uint32_t byte_offset = bit_index / 8;
    uint8_t bit_offset = bit_index % 8;

    /* Calculate which sector contains this byte */
    uint32_t sector_offset = byte_offset / SD_BLOCK_SIZE;
    uint32_t byte_in_sector = byte_offset % SD_BLOCK_SIZE;

    /* Read the bitmap sector */
    if (!microsd_read_block(bitmap_sector + sector_offset, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read allocation bitmap sector\n");
        return false;
    }

    /* Set the bit for this cluster */
    buffer[byte_in_sector] |= (1 << bit_offset);

    /* Write back the bitmap sector */
    if (!microsd_write_block(bitmap_sector + sector_offset, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write allocation bitmap sector\n");
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Updated allocation bitmap: cluster %lu marked as used\n",
                (unsigned long)cluster);
    return true;
}

/*!
 * @brief Calculate checksum for directory entry set
 * @param[in] p_entries     Pointer to directory entries
 * @param[in] count         Number of entries
 * @return uint16_t         Calculated checksum
 */
static uint16_t calculate_entry_checksum(uint8_t const* const p_entries, uint32_t count) {
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

/*!
 * @brief Allocate cluster chain for large files
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] data_length   Length of data requiring allocation
 * @param[out] p_first_cluster Pointer to store first cluster number
 * @return bool             true on success, false on failure
 */
static bool allocate_cluster_chain(filesystem_info_t const* const p_fs_info, uint32_t data_length,
                                   uint32_t* const p_first_cluster) {
    uint32_t bytes_per_cluster = p_fs_info->sectors_per_cluster * SD_BLOCK_SIZE;
    uint32_t clusters_needed = (data_length + bytes_per_cluster - 1) / bytes_per_cluster;
    uint32_t first_cluster = 0;
    uint32_t prev_cluster = 0;

    if (NULL == p_first_cluster) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid parameter for cluster allocation\n");
        return false;
    }

    if (clusters_needed == 0) {
        clusters_needed = 1; /* Minimum one cluster */
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Allocating cluster chain for %lu bytes (%lu clusters)\n",
                (unsigned long)data_length, (unsigned long)clusters_needed);

    /* Allocate clusters and link them together */
    for (uint32_t i = 0; i < clusters_needed; i++) {
        uint32_t cluster = find_free_cluster(p_fs_info);
        if (cluster == 0) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to find free cluster %lu of %lu\n",
                        (unsigned long)(i + 1), (unsigned long)clusters_needed);
            return false;
        }

        if (i == 0) {
            first_cluster = cluster;
        }

        /* Link previous cluster to this one */
        if (prev_cluster != 0) {
            uint8_t buffer[SD_BLOCK_SIZE];
            uint32_t fat_sector, entry_offset;
            get_fat_entry_location(p_fs_info, prev_cluster, &fat_sector, &entry_offset);

            if (!microsd_read_block(fat_sector, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector for linking\n");
                return false;
            }

            uint32_t* fat_entry = (uint32_t*)(buffer + entry_offset);
            *fat_entry = cluster;

            if (!microsd_write_block(fat_sector, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write FAT sector for linking\n");
                return false;
            }

            MICROSD_LOG(MICROSD_LOG_INFO,
                        "Linked cluster %lu -> %lu (FAT sector %lu, offset %lu)\n",
                        (unsigned long)prev_cluster, (unsigned long)cluster,
                        (unsigned long)fat_sector, (unsigned long)entry_offset);
        }

        /* Update allocation bitmap for this cluster */
        if (!update_allocation_bitmap(p_fs_info, cluster)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to update allocation bitmap for cluster %lu\n",
                        (unsigned long)cluster);
            return false;
        }

        prev_cluster = cluster;
    }

    /* Mark the last cluster as end-of-chain */
    if (!mark_cluster_end(p_fs_info, prev_cluster)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to mark last cluster as end-of-chain\n");
        return false;
    }

    *p_first_cluster = first_cluster;
    MICROSD_LOG(MICROSD_LOG_INFO,
                "Successfully allocated cluster chain: %lu clusters starting at %lu\n",
                (unsigned long)clusters_needed, (unsigned long)first_cluster);

    return true;
}

/*!
 * @brief Write data across multiple clusters
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] first_cluster First cluster of the chain
 * @param[in] p_data        Pointer to data to write
 * @param[in] data_length   Length of data to write
 * @return bool             true on success, false on failure
 */
static bool write_cluster_chain_data(filesystem_info_t const* const p_fs_info,
                                     uint32_t first_cluster, uint8_t const* const p_data,
                                     uint32_t data_length) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t current_cluster = first_cluster;
    uint32_t bytes_written = 0;
    uint32_t bytes_per_cluster = p_fs_info->sectors_per_cluster * SD_BLOCK_SIZE;

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Writing %lu bytes across cluster chain starting at %lu\n",
                (unsigned long)data_length, (unsigned long)first_cluster);

    while (bytes_written < data_length && current_cluster != 0xFFFFFFFF) {
        uint32_t cluster_start_sector = cluster_to_sector(p_fs_info, current_cluster);
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

            bytes_written += bytes_in_sector;
            bytes_to_write_in_cluster -= bytes_in_sector;

            if (bytes_written >= data_length) {
                break; /* All data written */
            }
        }

        /* Move to next cluster in chain */
        if (bytes_written < data_length) {
            uint32_t fat_sector, entry_offset;
            get_fat_entry_location(p_fs_info, current_cluster, &fat_sector, &entry_offset);

            if (!microsd_read_block(fat_sector, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT for next cluster\n");
                return false;
            }

            uint32_t* fat_entry = (uint32_t*)(buffer + entry_offset);
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

/*!
 * @brief Create a file in the exFAT filesystem
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] filename      Filename (null-terminated string)
 * @param[in] p_data        Pointer to file data
 * @param[in] data_length   Length of file data in bytes
 * @return bool             true on success, false on failure
 */
bool microsd_create_file(filesystem_info_t const* const p_fs_info, char const* const filename,
                         uint8_t const* const p_data, uint32_t const data_length) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t root_sector, free_cluster, data_sector;
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
    if (!allocate_cluster_chain(p_fs_info, data_length, &free_cluster)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to allocate cluster chain for file\n");
        return false;
    }

    /* Write file data across cluster chain */
    if (!write_cluster_chain_data(p_fs_info, free_cluster, p_data, data_length)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write file data\n");
        return false;
    }

    /* Read root directory */
    root_sector = cluster_to_sector(p_fs_info, p_fs_info->root_cluster);
    if (!microsd_read_block(root_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read root directory\n");
        return false;
    }

    /* Always create versioned files - find the next available version number */
    char final_filename[256];
    uint32_t version_number = 0;
    bool filename_unique = false;

    /* Extract base filename and extension once */
    char base_name[240];
    char extension[16] = "";
    char* dot_pos = strrchr(filename, '.');

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

        /* Check if this filename already exists in directory */
        filename_unique = true;
        for (uint32_t i = 0; i < (SD_BLOCK_SIZE / 32); i++) {
            uint8_t entry_type = buffer[i * 32];
            if (entry_type == 0x85) { /* File entry */
                uint8_t secondary_count = buffer[i * 32 + 1];

                /* Extract filename from name entries to compare */
                char existing_filename[256] = "";
                uint32_t char_index = 0;

                for (uint32_t j = 2; j <= secondary_count && (i + j) < (SD_BLOCK_SIZE / 32); j++) {
                    uint8_t name_entry_type = buffer[(i + j) * 32];
                    if (name_entry_type == 0xC1) { /* Name entry */
                        uint16_t* utf16_chars = (uint16_t*)&buffer[(i + j) * 32 + 2];
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
                    MICROSD_LOG(MICROSD_LOG_INFO, "File '%s' already exists, trying version %lu\n",
                                final_filename, (unsigned long)version_number);
                    break;
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

    /* Find empty directory entries */
    uint8_t* dir_entry = buffer;
    uint32_t entry_index = 0;

    /* Find end of directory - only insert at end-of-directory marker (0x00) */
    uint32_t available_entries = 0;
    uint32_t cleaned_entries = 0;

    /* First pass: Clean up corrupted/invalid file entries */
    for (uint32_t i = 3; i < (SD_BLOCK_SIZE / 32); i++) {
        uint8_t entry_type = dir_entry[i * 32];
        if (entry_type == 0x85) { /* File entry */
            /* Check for corrupted FirstCluster values */
            uint32_t first_cluster = *((uint32_t*)&dir_entry[i * 32 + 20]);
            if (first_cluster > 1000000) { /* Clearly invalid cluster number for small SD cards */
                MICROSD_LOG(MICROSD_LOG_INFO,
                            "Cleaning corrupted file entry %lu (invalid cluster %lu)\n",
                            (unsigned long)i, (unsigned long)first_cluster);

                /* Clear this entry and its secondary entries */
                uint8_t secondary_count = dir_entry[i * 32 + 1];
                for (uint32_t j = 0; j <= secondary_count && (i + j) < (SD_BLOCK_SIZE / 32); j++) {
                    memset(&dir_entry[(i + j) * 32], 0, 32);
                    cleaned_entries++;
                }
                /* Skip past the cleared entries */
                i += secondary_count;
            }
        }
    }

    if (cleaned_entries > 0) {
        MICROSD_LOG(MICROSD_LOG_INFO, "Cleaned %lu corrupted directory entries\n",
                    (unsigned long)cleaned_entries);

        /* Write the cleaned directory back to SD card */
        if (!microsd_write_block(root_sector, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write cleaned directory\n");
            return false;
        }
        MICROSD_LOG(MICROSD_LOG_INFO, "Cleaned directory written successfully\n");
    }

    /* Second pass: Find end of directory */
    entry_index = 0;
    while (entry_index < (SD_BLOCK_SIZE / 32)) {
        uint8_t entry_type = dir_entry[entry_index * 32];
        if (entry_type == 0x00) {
            /* Found end-of-directory marker - this is where we insert */
            MICROSD_LOG(MICROSD_LOG_DEBUG, "Found end-of-directory at entry %lu\n",
                        (unsigned long)entry_index);
            available_entries = (SD_BLOCK_SIZE / 32) - entry_index;
            break;
        }
        /* Skip all other entries (system entries 0x81-0x83, file entries 0x85+, etc.) */
        entry_index++;
    }

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Available directory entries: %lu, needed: %lu\n",
                (unsigned long)available_entries, (unsigned long)total_entries);

    if (available_entries < total_entries) {
        MICROSD_LOG(MICROSD_LOG_WARN,
                    "Root directory cluster full (need %lu entries, only %lu available)\n",
                    (unsigned long)total_entries, (unsigned long)available_entries);

        /* Attempt to expand directory by allocating new cluster */
        if (!expand_directory(p_fs_info, p_fs_info->root_cluster)) {
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

            uint32_t* fat_entry = (uint32_t*)&buffer[fat_offset];
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
    exfat_file_entry_t* file_entry = (exfat_file_entry_t*)&entries[0];
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
    exfat_stream_entry_t* stream_entry = (exfat_stream_entry_t*)&entries[32];
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
        exfat_name_entry_t* name_entry = (exfat_name_entry_t*)&entries[64 + (name_entry_idx * 32)];
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

    /* Debug: Show the directory entries we're creating */
    MICROSD_LOG(MICROSD_LOG_DEBUG, "Directory entry set (%lu entries):\n",
                (unsigned long)total_entries);
    MICROSD_LOG(MICROSD_LOG_DEBUG,
                "  File entry: type=0x%02X, secondary_count=%d, checksum=0x%04X\n",
                file_entry->entry_type, file_entry->secondary_count, file_entry->set_checksum);
    MICROSD_LOG(MICROSD_LOG_DEBUG,
                "  Stream entry: type=0x%02X, name_len=%d, hash=0x%04X, cluster=%lu, size=%lu\n",
                stream_entry->entry_type, stream_entry->name_length, stream_entry->name_hash,
                (unsigned long)stream_entry->first_cluster,
                (unsigned long)stream_entry->data_length);

    /* Show all name entries */
    for (uint32_t i = 0; i < name_entries_needed; i++) {
        exfat_name_entry_t* name_entry = (exfat_name_entry_t*)&entries[64 + (i * 32)];
        MICROSD_LOG(
            MICROSD_LOG_DEBUG, "  Name entry %lu: type=0x%02X, first 8 chars='%c%c%c%c%c%c%c%c'\n",
            (unsigned long)i, name_entry->entry_type, (char)(name_entry->file_name[0] & 0xFF),
            (char)(name_entry->file_name[1] & 0xFF), (char)(name_entry->file_name[2] & 0xFF),
            (char)(name_entry->file_name[3] & 0xFF), (char)(name_entry->file_name[4] & 0xFF),
            (char)(name_entry->file_name[5] & 0xFF), (char)(name_entry->file_name[6] & 0xFF),
            (char)(name_entry->file_name[7] & 0xFF));
    }

    /* Debug: Hex dump of directory entries */
    MICROSD_LOG(MICROSD_LOG_DEBUG, "Directory entry hex dump:\n");
    for (uint32_t i = 0; i < total_entries; i++) {
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Entry %lu: ", (unsigned long)i);
        for (uint32_t j = 0; j < 32; j += 8) {
            MICROSD_LOG(MICROSD_LOG_DEBUG, "%02X %02X %02X %02X %02X %02X %02X %02X ",
                        entries[i * 32 + j], entries[i * 32 + j + 1], entries[i * 32 + j + 2],
                        entries[i * 32 + j + 3], entries[i * 32 + j + 4], entries[i * 32 + j + 5],
                        entries[i * 32 + j + 6], entries[i * 32 + j + 7]);
        }
        MICROSD_LOG(MICROSD_LOG_DEBUG, "\n");
    }

    /* Copy entries to directory buffer */
    memcpy(&buffer[entry_index * 32], entries, total_entries * 32);

    /* Ensure proper end-of-directory marker */
    uint32_t next_entry_index = entry_index + total_entries;
    if (next_entry_index < (SD_BLOCK_SIZE / 32)) {
        /* Clear the entry after our file to mark end-of-directory */
        memset(&buffer[next_entry_index * 32], 0, 32);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Set end-of-directory marker at entry %lu\n",
                    (unsigned long)next_entry_index);
    }

    /* Write back root directory */
    if (!microsd_write_block(root_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write root directory\n");
        return false;
    }

    /* Verify directory was written correctly by reading it back */
    uint8_t verify_buffer[SD_BLOCK_SIZE];
    if (!microsd_read_block(root_sector, verify_buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to verify root directory write\n");
        return false;
    }

    /* Compare written vs read data */
    bool verify_ok = true;
    for (int i = 0; i < SD_BLOCK_SIZE; i++) {
        if (buffer[i] != verify_buffer[i]) {
            MICROSD_LOG(MICROSD_LOG_ERROR,
                        "Directory verification failed at offset %d: wrote 0x%02X, read 0x%02X\n",
                        i, buffer[i], verify_buffer[i]);
            verify_ok = false;
            break;
        }
    }

    if (verify_ok) {
        MICROSD_LOG(MICROSD_LOG_INFO, "Directory write verification successful\n");
    } else {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Directory write verification failed\n");
        return false;
    }

    /* Force filesystem sync by reading multiple blocks to flush caches */
    uint8_t sync_buffer[SD_BLOCK_SIZE];
    for (int i = 0; i < 3; i++) {
        uint32_t sync_sector = p_fs_info->partition_offset + p_fs_info->cluster_heap_offset +
                               (4 * p_fs_info->sectors_per_cluster) + i;
        microsd_read_block(sync_sector, sync_buffer);
        sleep_ms(10); /* Small delay to ensure write completion */
    }

    /* Debug: Show final directory structure */
    MICROSD_LOG(MICROSD_LOG_INFO, "=== Final Root Directory Structure ===\n");
    for (int i = 0; i < SD_BLOCK_SIZE; i += 32) {
        uint8_t entry_type = buffer[i];
        if (entry_type == 0x00) {
            MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: End-of-directory marker\n", i / 32);
            break;
        } else if (entry_type != 0xFF && (entry_type & 0x80)) {
            uint8_t type_code = entry_type & 0x1F;
            uint8_t type_importance = (entry_type >> 5) & 0x01;
            uint8_t type_category = (entry_type >> 6) & 0x01;

            if (type_code == 1 && type_importance == 0 && type_category == 0) {
                MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: Allocation Bitmap\n", i / 32);
            } else if (type_code == 2 && type_importance == 0 && type_category == 0) {
                MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: Up-case Table\n", i / 32);
            } else if (type_code == 3 && type_importance == 0 && type_category == 0) {
                MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: Volume Label\n", i / 32);
            } else if (type_code == 5 && type_importance == 0 && type_category == 0) {
                uint8_t secondary_count = buffer[i + 1];
                MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: File (secondary_count=%d)\n", i / 32,
                            secondary_count);
            } else if (type_code == 0 && type_importance == 0 && type_category == 1) {
                MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: Stream Extension\n", i / 32);
            } else if (type_code == 1 && type_importance == 0 && type_category == 1) {
                MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: File Name\n", i / 32);
            } else {
                MICROSD_LOG(MICROSD_LOG_INFO,
                            "Entry %d: Unknown (TypeCode=%d, Importance=%d, Category=%d)\n", i / 32,
                            type_code, type_importance, type_category);
            }
        }
    }
    MICROSD_LOG(MICROSD_LOG_INFO, "=== End Final Directory Structure ===\n");

    MICROSD_LOG(MICROSD_LOG_INFO, "File '%s' created successfully\n", final_filename);
    return true;
}

/*!
 * @brief Read a file from the exFAT filesystem
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] filename      Filename to read (null-terminated string)
 * @param[out] p_buffer     Pointer to buffer to store file data
 * @param[in] buffer_size   Size of the buffer
 * @param[out] p_bytes_read Pointer to store actual bytes read
 * @return bool             true on success, false on failure
 */
bool microsd_read_file(filesystem_info_t const* const p_fs_info, char const* const filename,
                       uint8_t* const p_buffer, uint32_t const buffer_size,
                       uint32_t* const p_bytes_read) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t root_sector;
    uint32_t filename_len;
    uint32_t file_cluster = 0;
    uint32_t file_size = 0;
    bool file_found = false;

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

    /* Search through all directory clusters (not just the first one) */
    uint32_t current_dir_cluster = p_fs_info->root_cluster;
    uint32_t cluster_iteration = 0;
    const uint32_t max_clusters = 100; /* Safety limit to prevent infinite loops */

    while (current_dir_cluster != 0xFFFFFFFF && current_dir_cluster >= 2 &&
           current_dir_cluster < p_fs_info->cluster_count + 2 && !file_found &&
           cluster_iteration < max_clusters) {
        cluster_iteration++;

        /* Calculate sector for this directory cluster */
        root_sector = p_fs_info->partition_offset + p_fs_info->cluster_heap_offset +
                      ((current_dir_cluster - 2) * p_fs_info->sectors_per_cluster);

        MICROSD_LOG(MICROSD_LOG_INFO, "Searching directory cluster %lu (sector %lu)\n",
                    (unsigned long)current_dir_cluster, (unsigned long)root_sector);

        /* Read all sectors in this directory cluster */
        bool sector_read_failed = false;
        for (uint32_t sec = 0;
             sec < p_fs_info->sectors_per_cluster && !file_found && !sector_read_failed; sec++) {
            if (!microsd_read_block(root_sector + sec, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR,
                            "Failed to read root directory sector %lu (cluster %lu)\n",
                            (unsigned long)(root_sector + sec), (unsigned long)current_dir_cluster);
                /* Directory read failure - stop searching this cluster chain */
                sector_read_failed = true;
                break;
            }

            /* Search for the file in this directory sector */
            for (uint32_t i = 0; i < (SD_BLOCK_SIZE / 32); i++) {
                uint8_t entry_type = buffer[i * 32];

                /* Log all entry types for debugging */
                if (entry_type != 0x00 && entry_type != 0x85 && entry_type != 0xC0 &&
                    entry_type != 0xC1) {
                    MICROSD_LOG(
                        MICROSD_LOG_INFO,
                        "Found entry type 0x%02X at cluster %lu, sector offset %lu, entry %lu\n",
                        entry_type, (unsigned long)current_dir_cluster, (unsigned long)sec,
                        (unsigned long)i);
                }

                if (entry_type == 0x00) {
                    /* End of directory entries in this sector */
                    break;
                }

                if (entry_type == 0x85) { /* File entry */
                    MICROSD_LOG(
                        MICROSD_LOG_INFO,
                        "Found file entry (0x85) at cluster %lu, sector offset %lu, entry %lu\n",
                        (unsigned long)current_dir_cluster, (unsigned long)sec, (unsigned long)i);
                    uint8_t secondary_count = buffer[i * 32 + 1];

                    /* Extract filename from name entries */
                    char current_filename[256] = "";
                    uint32_t char_index = 0;

                    /* Look for stream extension entry (should be next entry) */
                    if ((i + 1) < (SD_BLOCK_SIZE / 32) && buffer[(i + 1) * 32] == 0xC0) {
                        /* Get file size from stream extension */
                        uint32_t* stream_data = (uint32_t*)&buffer[(i + 1) * 32];
                        file_size = stream_data[2];    /* DataLength at offset 8 */
                        file_cluster = stream_data[5]; /* FirstCluster at offset 20 */
                    }

                    /* Extract filename from name entries */
                    for (uint32_t j = 2; j <= secondary_count && (i + j) < (SD_BLOCK_SIZE / 32);
                         j++) {
                        uint8_t name_entry_type = buffer[(i + j) * 32];
                        if (name_entry_type == 0xC1) { /* Name entry */
                            uint16_t* utf16_chars = (uint16_t*)&buffer[(i + j) * 32 + 2];
                            for (uint32_t k = 0; k < 15 && char_index < 255; k++) {
                                if (utf16_chars[k] == 0)
                                    break;                  /* End of filename */
                                if (utf16_chars[k] < 128) { /* ASCII character */
                                    current_filename[char_index++] = (char)utf16_chars[k];
                                }
                            }
                        }
                    }
                    current_filename[char_index] = '\0';

                    /* Check if this is the file we're looking for */
                    if (strcmp(current_filename, filename) == 0) {
                        MICROSD_LOG(MICROSD_LOG_INFO,
                                    "Found file '%s' at cluster %lu, size %lu bytes\n", filename,
                                    (unsigned long)file_cluster, (unsigned long)file_size);
                        file_found = true;
                        break;
                    }

                    /* Skip past this file's entries */
                    i += secondary_count;
                }
            } /* end for each directory entry */
        } /* end for each sector in cluster */

        /* Get next cluster in directory chain from FAT */
        if (!file_found) {
            uint32_t fat_sector = p_fs_info->partition_offset + p_fs_info->fat_offset +
                                  (current_dir_cluster * 4) / SD_BLOCK_SIZE;
            uint32_t fat_offset = (current_dir_cluster * 4) % SD_BLOCK_SIZE;

            if (!microsd_read_block(fat_sector, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector %lu\n",
                            (unsigned long)fat_sector);
                /* FAT read failure - stop searching */
                break;
            }

            uint32_t* fat_entry = (uint32_t*)&buffer[fat_offset];
            uint32_t next_cluster = *fat_entry;

            /* Check for valid cluster range or end-of-chain markers */
            if (next_cluster == 0xFFFFFFFF || next_cluster == 0xFFFFFFF8 || next_cluster < 2 ||
                next_cluster >= p_fs_info->cluster_count + 2) {
                /* End of chain or invalid cluster */
                MICROSD_LOG(MICROSD_LOG_DEBUG,
                            "End of directory chain at cluster %lu (next=0x%08lX)\n",
                            (unsigned long)current_dir_cluster, (unsigned long)next_cluster);
                break;
            }

            current_dir_cluster = next_cluster;

            MICROSD_LOG(MICROSD_LOG_DEBUG, "Next directory cluster: %lu\n",
                        (unsigned long)current_dir_cluster);
        }
    } /* end while traversing directory clusters */

    if (!file_found) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "File '%s' not found (searched %lu directory clusters)\n",
                    filename, (unsigned long)cluster_iteration);
        return false;
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

    /* Calculate how many bytes to read */
    uint32_t bytes_to_read = (file_size < buffer_size) ? file_size : buffer_size;

    /* Read file data from cluster(s) - may span multiple sectors */
    uint32_t bytes_read = 0;
    uint32_t current_cluster = file_cluster;

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
            memcpy(p_buffer + bytes_read, buffer, bytes_in_sector);
            bytes_read += bytes_in_sector;
        }

        /* Move to next cluster in chain if we need more data */
        if (bytes_read < bytes_to_read) {
            /* Read FAT entry to get next cluster */
            uint32_t fat_sector, entry_offset;
            get_fat_entry_location(p_fs_info, current_cluster, &fat_sector, &entry_offset);

            if (!microsd_read_block(fat_sector, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT for next cluster\n");
                return false;
            }

            uint32_t* fat_entry = (uint32_t*)(buffer + entry_offset);
            uint32_t prev_cluster_for_log = current_cluster;
            current_cluster = *fat_entry;

            MICROSD_LOG(MICROSD_LOG_INFO,
                        "Reading FAT chain: cluster %lu -> 0x%08lX (FAT sector %lu, offset %lu)\n",
                        (unsigned long)prev_cluster_for_log, (unsigned long)current_cluster,
                        (unsigned long)fat_sector, (unsigned long)entry_offset);

            if (current_cluster >= 0xFFFFFFF8) {
                /* End of cluster chain */
                MICROSD_LOG(MICROSD_LOG_INFO, "Reached end of cluster chain (0x%08lX)\n",
                            (unsigned long)current_cluster);
                break;
            }
        }
    }

    *p_bytes_read = bytes_read;

    MICROSD_LOG(MICROSD_LOG_INFO, "Successfully read %lu bytes from file '%s'\n",
                (unsigned long)bytes_read, filename);

    return true;
}

/*!
 * @brief Get the next cluster in a cluster chain
 * @param[in] p_fs_info Pointer to filesystem info structure
 * @param[in] cluster   Current cluster number
 * @return uint32_t     Next cluster number, or 0xFFFFFFFF if end of chain
 */
static uint32_t get_next_cluster(filesystem_info_t const* const p_fs_info, uint32_t cluster) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t fat_sector, fat_offset;
    uint32_t next_cluster;

    /* Calculate FAT sector and offset using centralized function */
    get_fat_entry_location(p_fs_info, cluster, &fat_sector, &fat_offset);

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

/*!
 * @brief Read a large file using chunked reading to avoid memory limitations
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] filename      Filename to read (null-terminated string)
 * @param[out] p_chunk_data Pointer to chunk buffer to store data
 * @param[in] chunk_size    Size of each chunk in bytes
 * @param[in] max_size      Maximum file size to read (128KB limit)
 * @param[out] p_total_read Pointer to store total bytes read
 * @return bool             true on success, false on failure
 */
bool microsd_read_large_file_chunked(filesystem_info_t const* const p_fs_info,
                                     char const* const filename, uint8_t* const p_chunk_data,
                                     uint32_t const chunk_size, uint32_t const max_size,
                                     uint32_t* const p_total_read) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t root_sector;
    uint32_t filename_len;
    uint32_t file_cluster = 0;
    uint32_t file_size = 0;
    bool file_found = false;

    if (NULL == p_fs_info || NULL == filename || NULL == p_chunk_data || NULL == p_total_read) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid parameters for chunked file reading\n");
        return false;
    }

    *p_total_read = 0;

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

    if (chunk_size == 0 || max_size == 0 || max_size > (128 * 1024)) {
        MICROSD_LOG(MICROSD_LOG_ERROR,
                    "Invalid parameters: chunk_size=%lu, max_size=%lu (limit: 128KB)\n",
                    (unsigned long)chunk_size, (unsigned long)max_size);
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Reading large file: %s (max %lu bytes in %lu-byte chunks)\n",
                filename, (unsigned long)max_size, (unsigned long)chunk_size);

    /* Calculate root directory sector */
    root_sector = p_fs_info->partition_offset + p_fs_info->cluster_heap_offset +
                  ((p_fs_info->root_cluster - 2) * p_fs_info->sectors_per_cluster);

    /* Read root directory */
    if (!microsd_read_block(root_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read root directory\n");
        return false;
    }

    /* Search for the file in the directory */
    for (uint32_t i = 0; i < (SD_BLOCK_SIZE / 32); i++) {
        uint8_t entry_type = buffer[i * 32];

        if (entry_type == 0x00) {
            /* End of directory */
            break;
        }

        if (entry_type == 0x85) { /* File entry */
            uint8_t secondary_count = buffer[i * 32 + 1];

            /* Extract filename from name entries */
            char current_filename[256] = "";
            uint32_t char_index = 0;

            /* Look for stream extension entry (should be next entry) */
            if ((i + 1) < (SD_BLOCK_SIZE / 32) && buffer[(i + 1) * 32] == 0xC0) {
                /* Get file size from stream extension */
                uint32_t* stream_data = (uint32_t*)&buffer[(i + 1) * 32];
                file_size = stream_data[2];    /* DataLength at offset 8 */
                file_cluster = stream_data[5]; /* FirstCluster at offset 20 */
            }

            /* Extract filename from name entries */
            for (uint32_t j = 2; j <= secondary_count && (i + j) < (SD_BLOCK_SIZE / 32); j++) {
                uint8_t name_entry_type = buffer[(i + j) * 32];
                if (name_entry_type == 0xC1) { /* Name entry */
                    uint16_t* utf16_chars = (uint16_t*)&buffer[(i + j) * 32 + 2];
                    for (uint32_t k = 0; k < 15 && char_index < 255; k++) {
                        if (utf16_chars[k] == 0)
                            break;                  /* End of filename */
                        if (utf16_chars[k] < 128) { /* ASCII character */
                            current_filename[char_index++] = (char)utf16_chars[k];
                        }
                    }
                }
            }
            current_filename[char_index] = '\0';

            /* Check if this is the file we're looking for */
            if (strcmp(current_filename, filename) == 0) {
                MICROSD_LOG(MICROSD_LOG_INFO, "Found file '%s' at cluster %lu, size %lu bytes\n",
                            filename, (unsigned long)file_cluster, (unsigned long)file_size);
                file_found = true;
                break;
            }

            /* Skip past this file's entries */
            i += secondary_count;
        }
    }

    if (!file_found) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "File '%s' not found\n", filename);
        return false;
    }

    /* Validate file parameters */
    if (file_size == 0) {
        MICROSD_LOG(MICROSD_LOG_INFO, "File '%s' is empty\n", filename);
        *p_total_read = 0;
        return true;
    }

    if (file_cluster == 0) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid cluster number for file '%s'\n", filename);
        return false;
    }

    /* Limit file size to max_size */
    uint32_t bytes_to_read = (file_size < max_size) ? file_size : max_size;
    if (file_size > max_size) {
        MICROSD_LOG(MICROSD_LOG_WARN,
                    "File size (%lu bytes) exceeds limit (%lu bytes), reading first %lu bytes\n",
                    (unsigned long)file_size, (unsigned long)max_size,
                    (unsigned long)bytes_to_read);
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Reading %lu bytes from file in chunks of %lu bytes\n",
                (unsigned long)bytes_to_read, (unsigned long)chunk_size);

    /* Read file data in chunks */
    uint32_t current_cluster = file_cluster;
    uint32_t bytes_read = 0;
    uint32_t sectors_per_cluster = p_fs_info->sectors_per_cluster;
    uint32_t bytes_per_cluster = sectors_per_cluster * 512;

    while (bytes_read < bytes_to_read && current_cluster != 0xFFFFFFFF) {
        uint32_t cluster_start_sector = p_fs_info->partition_offset +
                                        p_fs_info->cluster_heap_offset +
                                        ((current_cluster - 2) * sectors_per_cluster);

        /* Read all sectors in this cluster */
        for (uint32_t sector = 0; sector < sectors_per_cluster && bytes_read < bytes_to_read;
             sector++) {
            if (!microsd_read_block(cluster_start_sector + sector, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read sector %lu\n",
                            (unsigned long)(cluster_start_sector + sector));
                return false;
            }

            /* Process this sector's data in chunks */
            uint32_t sector_offset = 0;
            while (sector_offset < 512 && bytes_read < bytes_to_read) {
                uint32_t bytes_in_chunk = chunk_size;
                uint32_t remaining_in_sector = 512 - sector_offset;
                uint32_t remaining_to_read = bytes_to_read - bytes_read;

                /* Adjust chunk size if needed */
                if (bytes_in_chunk > remaining_in_sector) {
                    bytes_in_chunk = remaining_in_sector;
                }
                if (bytes_in_chunk > remaining_to_read) {
                    bytes_in_chunk = remaining_to_read;
                }

                /* Copy chunk data */
                memcpy(p_chunk_data, buffer + sector_offset, bytes_in_chunk);

                MICROSD_LOG(MICROSD_LOG_DEBUG, "Read chunk: %lu bytes at offset %lu\n",
                            (unsigned long)bytes_in_chunk, (unsigned long)bytes_read);

                bytes_read += bytes_in_chunk;
                sector_offset += bytes_in_chunk;

                /* Process the chunk data here if needed */
                /* User can add custom processing logic for each chunk */
            }
        }

        /* Move to next cluster if needed */
        if (bytes_read < bytes_to_read) {
            current_cluster = get_next_cluster(p_fs_info, current_cluster);
        }
    }

    *p_total_read = bytes_read;

    MICROSD_LOG(MICROSD_LOG_INFO,
                "Successfully read %lu bytes from file '%s' using chunked reading\n",
                (unsigned long)bytes_read, filename);

    return true;
}

// helper function to get current time for pico_w
uint32_t get_current_time() {
    // Use dummy implementation for now
    // Once MQTT with wifi is working, we can get real time from NTP
    uint32_t dummy_time = 0x4B3A2C00;  // Fixed timestamp (e.g., Jan 1, 2021)
    return dummy_time;
}

/*!
 * @brief Create a large file using chunked writing to avoid memory limitations
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] filename      Filename to create (null-terminated string)
 * @param[in] p_chunk_data  Pointer to chunk data buffer
 * @param[in] chunk_size    Size of each chunk in bytes
 * @param[in] total_size    Total file size in bytes
 * @param[in] num_chunks    Number of chunks to write
 * @return bool             true on success, false on failure
 */
bool microsd_create_large_file_chunked(filesystem_info_t const* const p_fs_info,
                                       char const* const filename,
                                       uint8_t const* const p_chunk_data, uint32_t const chunk_size,
                                       uint32_t const total_size, uint32_t const num_chunks) {
    uint32_t free_cluster;

    if (NULL == p_fs_info || NULL == filename || NULL == p_chunk_data) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid parameters for chunked file creation\n");
        return false;
    }

    if (!p_fs_info->is_exfat) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Not an exFAT filesystem\n");
        return false;
    }

    uint32_t filename_len = strlen(filename);
    if (filename_len == 0 || filename_len > 255) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid filename length: %lu\n",
                    (unsigned long)filename_len);
        return false;
    }

    if (chunk_size == 0 || num_chunks == 0 || total_size == 0) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid chunk parameters\n");
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_INFO,
                "Creating large file: %s (%lu bytes in %lu chunks of %lu bytes)\n", filename,
                (unsigned long)total_size, (unsigned long)num_chunks, (unsigned long)chunk_size);

    /* Allocate cluster chain for entire file */
    if (!allocate_cluster_chain(p_fs_info, total_size, &free_cluster)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to allocate cluster chain for large file\n");
        return false;
    }

    /* Write data directly to clusters in chunks - no need to allocate full file buffer */
    MICROSD_LOG(MICROSD_LOG_INFO, "Writing file data in %lu chunks to allocated clusters\n",
                (unsigned long)num_chunks);

    uint32_t current_cluster = free_cluster;
    uint32_t bytes_written = 0;
    uint32_t sectors_per_cluster = p_fs_info->sectors_per_cluster;
    uint32_t bytes_per_cluster = sectors_per_cluster * 512;

    for (uint32_t chunk = 0; chunk < num_chunks; chunk++) {
        uint32_t bytes_to_write = chunk_size;

        /* Last chunk might be smaller */
        if (bytes_written + chunk_size > total_size) {
            bytes_to_write = total_size - bytes_written;
        }

        MICROSD_LOG(MICROSD_LOG_DEBUG, "Writing chunk %lu/%lu: %lu bytes\n",
                    (unsigned long)(chunk + 1), (unsigned long)num_chunks,
                    (unsigned long)bytes_to_write);

        /* Write chunk data directly to the clusters */
        uint32_t chunk_offset = 0;
        while (chunk_offset < bytes_to_write && current_cluster != 0xFFFFFFFF) {
            uint32_t cluster_start_sector =
                p_fs_info->cluster_heap_offset + ((current_cluster - 2) * sectors_per_cluster);
            uint32_t bytes_in_cluster = bytes_per_cluster;
            uint32_t remaining_in_chunk = bytes_to_write - chunk_offset;

            if (remaining_in_chunk < bytes_in_cluster) {
                bytes_in_cluster = remaining_in_chunk;
            }

            /* Write data to this cluster */
            uint32_t sector_offset = 0;
            while (sector_offset < bytes_in_cluster) {
                uint8_t sector_buffer[512];
                memset(sector_buffer, 0, sizeof(sector_buffer));

                uint32_t bytes_in_sector = 512;
                uint32_t remaining_in_cluster = bytes_in_cluster - sector_offset;
                if (remaining_in_cluster < 512) {
                    bytes_in_sector = remaining_in_cluster;
                }

                /* Fill sector with chunk pattern */
                uint32_t pattern_offset = chunk_offset % chunk_size;
                for (uint32_t i = 0; i < bytes_in_sector; i++) {
                    sector_buffer[i] = p_chunk_data[pattern_offset];
                    pattern_offset = (pattern_offset + 1) % chunk_size;
                }

                uint32_t sector_address = cluster_start_sector + (sector_offset / 512);
                if (!microsd_write_block(sector_address, sector_buffer)) {
                    MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write sector %lu\n",
                                (unsigned long)sector_address);
                    return false;
                }

                sector_offset += bytes_in_sector;
                chunk_offset += bytes_in_sector;
            }

            /* Move to next cluster if needed */
            if (chunk_offset < bytes_to_write) {
                /* Read FAT to get next cluster */
                uint32_t fat_sector, fat_offset;
                get_fat_entry_location(p_fs_info, current_cluster, &fat_sector, &fat_offset);

                uint8_t fat_buffer[512];
                if (!microsd_read_block(fat_sector, fat_buffer)) {
                    MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector %lu\n",
                                (unsigned long)fat_sector);
                    return false;
                }

                current_cluster = *((uint32_t*)(fat_buffer + fat_offset));
            }
        }

        bytes_written += bytes_to_write;

        if (bytes_written >= total_size) {
            break;
        }
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "File data written successfully (%lu bytes)\n",
                (unsigned long)bytes_written);

    /* Now we need to create the directory entry - use the same approach as microsd_create_file */
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t root_sector;
    char final_filename[256];
    uint32_t entry_found = 0;

    /* Normalize filename (uppercase) */
    for (uint32_t i = 0; i < filename_len; i++) {
        final_filename[i] = (char)toupper((int)filename[i]);
    }
    final_filename[filename_len] = '\0';

    /* Calculate number of directory entries needed */
    uint32_t name_entries_needed = (filename_len + 14) / 15; /* 15 chars per name entry */
    uint32_t total_entries = 2 + name_entries_needed;        /* File + Stream + Name entries */

    /* Limit to reasonable maximum to prevent buffer overflow */
    if (total_entries > 20) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Filename too long (needs %lu entries, max 20)\n",
                    (unsigned long)total_entries);
        return false;
    }

    /* Find free space in root directory */
    root_sector = p_fs_info->partition_offset + p_fs_info->cluster_heap_offset +
                  ((p_fs_info->root_cluster - 2) * p_fs_info->sectors_per_cluster);

    /* Search for contiguous free entries */
    for (uint32_t sector_offset = 0; sector_offset < p_fs_info->sectors_per_cluster;
         sector_offset++) {
        if (!microsd_read_block(root_sector + sector_offset, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read root directory sector\n");
            return false;
        }

        for (uint32_t entry_idx = 0; entry_idx < SD_BLOCK_SIZE / 32; entry_idx++) {
            uint32_t offset = entry_idx * 32;
            if (buffer[offset] == 0x00) { /* Found free entry */
                uint32_t consecutive_free = 0;

                /* Check if we have enough consecutive free entries */
                for (uint32_t check_idx = entry_idx;
                     check_idx < SD_BLOCK_SIZE / 32 && consecutive_free < total_entries;
                     check_idx++) {
                    if (buffer[check_idx * 32] == 0x00) {
                        consecutive_free++;
                    } else {
                        break;
                    }
                }

                if (consecutive_free >= total_entries) {
                    /* Found enough space */
                    entry_found = 1;

                    /* Create directory entry set */
                    uint8_t entries[32 *
                                    20]; /* Max 20 entries: file + stream + up to 18 name entries */
                    memset(entries, 0, sizeof(entries));

                    /* File entry */
                    exfat_file_entry_t* file_entry = (exfat_file_entry_t*)&entries[0];
                    file_entry->entry_type = EXFAT_TYPE_FILE;
                    file_entry->secondary_count = total_entries - 1;
                    file_entry->file_attributes = 0x00;

                    file_entry->created_timestamp = get_current_time();
                    file_entry->last_modified_timestamp = get_current_time();
                    file_entry->last_accessed_timestamp = get_current_time();
                    file_entry->created_10ms_increment = 0;
                    file_entry->last_modified_10ms_increment = 0;

                    /* Stream extension entry */
                    exfat_stream_entry_t* stream_entry = (exfat_stream_entry_t*)&entries[32];
                    stream_entry->entry_type = EXFAT_TYPE_STREAM_EXTENSION;
                    stream_entry->general_flags = 0x01;
                    stream_entry->name_length = filename_len;
                    stream_entry->name_hash =
                        calculate_exfat_name_hash(final_filename, filename_len);
                    stream_entry->valid_data_length = total_size;
                    stream_entry->first_cluster = free_cluster;
                    stream_entry->data_length = total_size;

                    /* Create name entries */
                    uint32_t char_index = 0;
                    for (uint32_t name_entry_idx = 0;
                         name_entry_idx < name_entries_needed && name_entry_idx < 18;
                         name_entry_idx++) {
                        uint32_t entry_offset = 64 + (name_entry_idx * 32);
                        if (entry_offset + 32 > sizeof(entries)) {
                            MICROSD_LOG(MICROSD_LOG_ERROR,
                                        "Buffer overflow prevented in name entries\n");
                            break;
                        }

                        exfat_name_entry_t* name_entry =
                            (exfat_name_entry_t*)&entries[entry_offset];
                        name_entry->entry_type = EXFAT_TYPE_FILE_NAME;
                        name_entry->general_flags = 0x00;

                        /* Convert filename to UTF-16 */
                        for (uint32_t i = 0; i < 15 && char_index < filename_len;
                             i++, char_index++) {
                            name_entry->file_name[i] = (uint16_t)final_filename[char_index];
                        }
                    }

                    /* Calculate checksum for entry set */
                    uint16_t checksum = calculate_entry_checksum(entries, total_entries);
                    file_entry->set_checksum = checksum;

                    /* Write entries to directory */
                    memcpy(&buffer[offset], entries, total_entries * 32);

                    if (!microsd_write_block(root_sector + sector_offset, buffer)) {
                        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write directory entry\n");
                        return false;
                    }

                    MICROSD_LOG(MICROSD_LOG_INFO, "Successfully created large file: %s\n",
                                filename);
                    return true;
                }
            }
        }
    }

    if (!entry_found) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "No space available in root directory\n");
        return false;
    }

    return true;
}

/*!
 * @brief Initialize chunk-based writing for a new file
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @param[in] filename          Filename for the chunked file
 * @param[in] total_chunks      Total number of chunks expected (including metadata chunk)
 * @param[in] chunk_size        Size of each chunk in bytes
 * @param[out] p_metadata       Pointer to metadata structure to initialize
 * @return bool                 true on success, false on failure
 */
bool microsd_init_chunk_write(filesystem_info_t const* const p_fs_info, char const* const filename,
                              uint32_t const total_chunks, uint32_t const chunk_size,
                              uint32_t const actual_file_size, chunk_metadata_t* const p_metadata) {
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
    if (!allocate_cluster_chain(p_fs_info, total_file_size, &p_metadata->file_cluster)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to allocate cluster chain for chunked file\n");
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Allocated cluster chain starting at cluster %lu\n",
                (unsigned long)p_metadata->file_cluster);

    return true;
}

/*! Sector cache for optimized chunk writes */
typedef struct {
    uint32_t sector_number;         // Cached sector number
    uint8_t buffer[SD_BLOCK_SIZE];  // Sector data
    bool valid;                     // Is cache valid?
    bool dirty;                     // Does cache need to be written?
} sector_cache_t;

static sector_cache_t g_sector_cache = {0};

/*!
 * @brief Flush cached sector to SD card if dirty
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @return bool                 true on success, false on failure
 */
static bool flush_sector_cache(filesystem_info_t const* const p_fs_info) {
    if (!g_sector_cache.valid || !g_sector_cache.dirty) {
        return true;  // Nothing to flush
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
static bool get_sector_for_write(filesystem_info_t const* const p_fs_info, uint32_t sector_number,
                                 uint8_t** sector_buffer) {
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
bool microsd_write_chunk(filesystem_info_t const* const p_fs_info,
                         chunk_metadata_t* const p_metadata, uint32_t const chunk_index,
                         uint8_t const* const p_chunk_data, uint32_t const chunk_data_size) {
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
        uint32_t cluster_start_sector = cluster_to_sector(p_fs_info, current_cluster);
        uint32_t sector_in_cluster = offset_in_cluster / SD_BLOCK_SIZE;
        uint32_t offset_in_sector = offset_in_cluster % SD_BLOCK_SIZE;
        uint32_t target_sector = cluster_start_sector + sector_in_cluster;

        /* Get sector (from cache or SD card) */
        uint8_t* sector_buffer = NULL;
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
bool microsd_check_all_chunks_received(chunk_metadata_t const* const p_metadata) {
    if (NULL == p_metadata) {
        return false;
    }

    return (p_metadata->chunks_received == p_metadata->total_chunks);
}

/*!
 * @brief Finalize chunk-based writing and create directory entry
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @param[in] p_metadata        Pointer to chunk metadata structure
 * @return bool                 true on success, false on failure
 */
bool microsd_finalize_chunk_write(filesystem_info_t const* const p_fs_info,
                                  chunk_metadata_t const* const p_metadata) {
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
    uint32_t max_sectors = p_fs_info->sectors_per_cluster;  // Only scan first cluster
    bool found_space = false;
    uint32_t target_sector = 0;
    uint32_t root_base_sector = cluster_to_sector(p_fs_info, p_fs_info->root_cluster);

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
    exfat_file_entry_t* file_entry = (exfat_file_entry_t*)&entries[0];
    file_entry->entry_type = EXFAT_TYPE_FILE;
    file_entry->secondary_count = total_entries - 1;
    file_entry->file_attributes = 0x00;
    file_entry->created_timestamp = get_current_time();
    file_entry->last_modified_timestamp = get_current_time();
    file_entry->last_accessed_timestamp = get_current_time();

    /* Stream extension entry */
    exfat_stream_entry_t* stream_entry = (exfat_stream_entry_t*)&entries[32];
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
        exfat_name_entry_t* name_entry = (exfat_name_entry_t*)&entries[64 + (name_entry_idx * 32)];
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
            exfat_file_entry_t* verify_entry =
                (exfat_file_entry_t*)&verify_buffer[entry_index * 32];
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
    if (!update_allocation_bitmap(p_fs_info, p_metadata->file_cluster)) {
        MICROSD_LOG(MICROSD_LOG_WARN, "Failed to re-flush allocation bitmap\n");
    }

    /* Update volume flags to mark filesystem as "dirty" (modified)
     * This is CRITICAL for exFAT - the OS won't see changes without this */
    MICROSD_LOG(MICROSD_LOG_INFO, "Updating volume flags to mark filesystem as modified\n");
    uint32_t boot_sector_num = p_fs_info->partition_offset;
    exfat_boot_sector_t* boot_sector = (exfat_boot_sector_t*)buffer;

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
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @param[in] filename          Filename to read
 * @param[out] p_chunk_data     Pointer to buffer for chunk data
 * @param[in] chunk_size        Size of each chunk
 * @param[in] chunk_index       Index of chunk to read (0 = metadata, 1+ = data chunks)
 * @param[out] p_bytes_read     Pointer to store actual bytes read
 * @return bool                 true on success, false on failure
 */
bool microsd_read_chunk(filesystem_info_t const* const p_fs_info, char const* const filename,
                        uint8_t* const p_chunk_data, uint32_t const chunk_size,
                        uint32_t const chunk_index, uint32_t* const p_bytes_read) {
    if (NULL == p_fs_info || NULL == filename || NULL == p_chunk_data || NULL == p_bytes_read) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid parameters for chunk read\n");
        return false;
    }

    *p_bytes_read = 0;

    /* Find the file and get its cluster and size */
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t root_sector = cluster_to_sector(p_fs_info, p_fs_info->root_cluster);
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
                exfat_stream_entry_t* stream = (exfat_stream_entry_t*)&buffer[(i + 1) * 32];
                file_size = stream->data_length;
                file_cluster = stream->first_cluster;
            }

            /* Extract filename */
            char current_filename[256] = "";
            uint32_t char_index = 0;

            for (uint32_t j = 2; j <= secondary_count && (i + j) < (SD_BLOCK_SIZE / 32); j++) {
                if (buffer[(i + j) * 32] == 0xC1) {
                    uint16_t* utf16_chars = (uint16_t*)&buffer[(i + j) * 32 + 2];
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
        uint32_t cluster_start_sector = cluster_to_sector(p_fs_info, current_cluster);
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

/*!
 * @brief List all files in the root directory of the SD card
 * @param[in] p_fs_info         Pointer to filesystem info structure
 * @return bool                 true on success, false on failure
 */
bool microsd_list_directory(filesystem_info_t const* const p_fs_info) {
    if (NULL == p_fs_info) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid parameter for directory listing\n");
        return false;
    }

    if (!p_fs_info->is_exfat) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Not an exFAT filesystem\n");
        return false;
    }

    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t root_sector = cluster_to_sector(p_fs_info, p_fs_info->root_cluster);
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
                exfat_file_entry_t* file_entry = (exfat_file_entry_t*)&buffer[offset];
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
                    entry_idx = -1; /* Reset to continue from start of next sector */
                }

                uint8_t stream_type = buffer[stream_offset];
                if (stream_type != EXFAT_TYPE_STREAM_EXTENSION) {
                    continue;
                }

                exfat_stream_entry_t* stream_entry = (exfat_stream_entry_t*)&buffer[stream_offset];

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

                    exfat_name_entry_t* name_entry =
                        (exfat_name_entry_t*)&buffer[name_entry_offset];

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

/*!
 * @brief Dump hex contents of SD card sectors for debugging/repair
 * @param[in] start_sector      Starting sector number to dump
 * @param[in] num_sectors       Number of sectors to dump
 * @return bool                 true on success, false on failure
 */
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
            if (buffer[i] != 0x00) all_zeros = false;
            if (buffer[i] != 0xFF) all_ffs = false;
            if (!all_zeros && !all_ffs) break;
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
            if (buffer[3] == 'E' && buffer[4] == 'X' && buffer[5] == 'F' &&
                buffer[6] == 'A' && buffer[7] == 'T') {
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
                uint32_t first_lba = buffer[part_offset + 8] |
                                     (buffer[part_offset + 9] << 8) |
                                     (buffer[part_offset + 10] << 16) |
                                     (buffer[part_offset + 11] << 24);

                if (type != 0) {
                    printf("    Partition %d: Type=0x%02X, Start LBA=%lu, Status=0x%02X\n",
                           i + 1, type, (unsigned long)first_lba, status);
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
    printf("✓ Hex dump complete - %lu sector%s analyzed\n",
           (unsigned long)num_sectors, num_sectors == 1 ? "" : "s");
    printf("═══════════════════════════════════════════════════════════════════\n\n");

    return true;
}
