/*!
 * @file    microsd_driver.c
 * @brief   MicroSD card driver implementation for Raspberry Pi Pico W
 * @author  INF2004 Team
 * @date    2024
 */

#include "microsd_driver.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

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

/*! Forward declarations for log buffer functions */
static void add_to_log_buffer(microsd_log_level_t level, const char *fmt, ...);
static bool flush_log_to_file(filesystem_info_t const *const p_fs_info);

/*! Logging macro */
#define MICROSD_LOG(level, fmt, ...)                                                               \
    do                                                                                             \
    {                                                                                              \
        if(g_log_level >= level)                                                                   \
        {                                                                                          \
            printf("[%s:%s] " fmt,                                                                 \
                   MICROSD_DRIVER_ID,                                                              \
                   (level == MICROSD_LOG_ERROR)  ? "ERROR"                                         \
                   : (level == MICROSD_LOG_WARN) ? "WARN"                                          \
                   : (level == MICROSD_LOG_INFO) ? "INFO"                                          \
                                                 : "DEBUG",                                        \
                   ##__VA_ARGS__);                                                                 \
            if(g_log_to_file_enabled)                                                              \
            {                                                                                      \
                add_to_log_buffer(level, fmt, ##__VA_ARGS__);                                      \
            }                                                                                      \
        }                                                                                          \
    } while(0)

/*! Private function prototypes */
static uint8_t spi_transfer(uint8_t const data);
static void cs_select(void);
static void cs_deselect(void);
static uint8_t send_command(uint8_t const cmd, uint32_t const arg);

/*!
 * @brief Transfer single byte via SPI
 * @param[in] data  Data byte to transfer
 * @return uint8_t  Received data byte
 */
static uint8_t spi_transfer(uint8_t const data)
{
    uint8_t rx_data;
    spi_write_read_blocking(SD_SPI_PORT, &data, &rx_data, 1U);
    return rx_data;
}

/*!
 * @brief Select SD card (CS low)
 * @return void
 */
static void cs_select(void)
{
    gpio_put(SD_CS_PIN, 0U);
    sleep_us(1U);
}

/*!
 * @brief Deselect SD card (CS high)
 * @return void
 */
static void cs_deselect(void)
{
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
static uint8_t send_command(uint8_t const cmd, uint32_t const arg)
{
    uint8_t response;
    uint8_t retry = 0U;

    /* Send command packet */
    (void) spi_transfer(0x40U | cmd);          /* Command with start bit */
    (void) spi_transfer((arg >> 24U) & 0xFFU); /* Argument byte 3 */
    (void) spi_transfer((arg >> 16U) & 0xFFU); /* Argument byte 2 */
    (void) spi_transfer((arg >> 8U) & 0xFFU);  /* Argument byte 1 */
    (void) spi_transfer(arg & 0xFFU);          /* Argument byte 0 */

    /* CRC (only matters for CMD0 and CMD8) */
    if(CMD0 == cmd)
    {
        (void) spi_transfer(0x95U);
    }
    else if(CMD8 == cmd)
    {
        (void) spi_transfer(0x87U);
    }
    else
    {
        (void) spi_transfer(0x01U);
    }

    /* Wait for response */
    do
    {
        response = spi_transfer(0xFFU);
        retry++;
    } while((0U != (response & 0x80U)) && (retry < 10U));

    return response;
}

bool microsd_init(void)
{
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
    (void) spi_init(SD_SPI_PORT, 100000U); /* Start at 100kHz - very slow */
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
    for(int i = 0; i < 10; i++)
    {
        (void) spi_transfer(0xFFU);
    }
    MICROSD_LOG(MICROSD_LOG_DEBUG, "Sent 80 clock cycles\n");

    /* Enter SPI mode */
    cs_select();
    MICROSD_LOG(MICROSD_LOG_DEBUG, "Sending CMD0...\n");
    response = send_command(CMD0, 0U);
    cs_deselect();
    MICROSD_LOG(MICROSD_LOG_DEBUG, "CMD0 response: 0x%02X\n", response);

    if(R1_IDLE_STATE == response)
    {
        MICROSD_LOG(MICROSD_LOG_INFO, "Card is in idle state. Sending CMD8...\n");
        /* Check voltage range */
        cs_select();
        response = send_command(CMD8, 0x01AAU);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "CMD8 response: 0x%02X\n", response);
        if(R1_IDLE_STATE == response)
        {
            /* Read the rest of R7 response */
            for(int i = 0; i < 4; i++)
            {
                r7_resp[i] = spi_transfer(0xFFU);
            }
            MICROSD_LOG(MICROSD_LOG_DEBUG,
                        "R7 response: 0x%02X 0x%02X 0x%02X 0x%02X\n",
                        r7_resp[0],
                        r7_resp[1],
                        r7_resp[2],
                        r7_resp[3]);
        }
        cs_deselect();

        /* Initialize card */
        MICROSD_LOG(MICROSD_LOG_INFO, "Sending ACMD41 to initialize...\n");
        timeout = 1000;
        do
        {
            cs_select();
            (void) send_command(CMD55, 0U);
            response = send_command(ACMD41, 0x40000000U);
            cs_deselect();
            MICROSD_LOG(
                MICROSD_LOG_DEBUG, "ACMD41 response: 0x%02X (timeout=%d)\n", response, timeout);
            sleep_ms(1U);
            timeout--;
        } while((R1_READY_STATE != response) && (timeout > 0));

        if(timeout > 0)
        {
            MICROSD_LOG(MICROSD_LOG_INFO, "Card initialized successfully!\n");
            /* Increase SPI speed */
            (void) spi_set_baudrate(SD_SPI_PORT, 10000000U); /* 10MHz */
            MICROSD_LOG(MICROSD_LOG_INFO, "SPI speed increased to 10MHz\n");
            g_driver_info.is_initialized = true;
            result = true;
        }
        else
        {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Card initialization timed out\n");
            g_driver_info.last_error_code = MICROSD_ERROR_ACMD41_TIMEOUT;
        }
    }
    else
    {
        MICROSD_LOG(
            MICROSD_LOG_ERROR, "Card did not enter idle state (response: 0x%02X)\n", response);
        g_driver_info.last_error_code = MICROSD_ERROR_CMD0_FAILED;
    }

    if(!result)
    {
        g_driver_info.last_error_code = MICROSD_ERROR_INIT_FAILED;
    }

    return result;
}

bool microsd_read_block(uint32_t const block_num, uint8_t *const p_buffer)
{
    bool result = false;
    uint8_t response;
    int timeout;

    if(NULL != p_buffer)
    {
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Reading block %lu\n", (unsigned long) block_num);
        cs_select();

        response = send_command(CMD17, block_num);
        if(0x00U == response)
        {
            /* Wait for data token */
            timeout = 1000;
            do
            {
                response = spi_transfer(0xFFU);
                timeout--;
            } while((0xFEU != response) && (timeout > 0));

            if(timeout > 0)
            {
                /* Read 512 bytes */
                for(int i = 0; i < SD_BLOCK_SIZE; i++)
                {
                    p_buffer[i] = spi_transfer(0xFFU);
                }

                /* Read CRC (ignore) */
                (void) spi_transfer(0xFFU);
                (void) spi_transfer(0xFFU);

                g_driver_info.total_reads++;
                result = true;
                MICROSD_LOG(MICROSD_LOG_DEBUG, "Block read successful\n");
            }
            else
            {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Read timeout waiting for data token\n");
                g_driver_info.last_error_code = MICROSD_ERROR_READ_FAILED;
            }
        }
        else
        {
            MICROSD_LOG(MICROSD_LOG_ERROR, "CMD17 failed with response 0x%02X\n", response);
            g_driver_info.last_error_code = MICROSD_ERROR_READ_FAILED;
        }

        cs_deselect();
    }

    return result;
}

bool microsd_write_block(uint32_t const block_num, uint8_t const *const p_buffer)
{
    bool result = false;
    uint8_t response;
    int timeout;

    if(NULL != p_buffer)
    {
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Starting write to block %lu\n", (unsigned long) block_num);
        cs_select();

        response = send_command(CMD24, block_num);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "CMD24 response: 0x%02X\n", response);
        if(0x00U == response)
        {
            /* Send data token */
            (void) spi_transfer(0xFEU);
            MICROSD_LOG(MICROSD_LOG_DEBUG, "Sent data token 0xFE\n");

            /* Send 512 bytes */
            for(int i = 0; i < SD_BLOCK_SIZE; i++)
            {
                (void) spi_transfer(p_buffer[i]);
            }
            MICROSD_LOG(MICROSD_LOG_DEBUG, "Sent %d bytes of data\n", SD_BLOCK_SIZE);

            /* Send dummy CRC */
            (void) spi_transfer(0xFFU);
            (void) spi_transfer(0xFFU);
            MICROSD_LOG(MICROSD_LOG_DEBUG, "Sent CRC\n");

            /* Check data response */
            response = spi_transfer(0xFFU);
            MICROSD_LOG(MICROSD_LOG_DEBUG, "Data response: 0x%02X\n", response);
            if(0x05U == (response & 0x1FU))
            {
                MICROSD_LOG(MICROSD_LOG_DEBUG, "Data accepted by card\n");
                /* Wait for write to complete */
                timeout = 5000; /* Increased timeout to 5000 iterations */
                int busy_count = 0;
                do
                {
                    response = spi_transfer(0xFFU);
                    if(response == 0x00U)
                    {
                        busy_count++;
                    }
                    if((timeout % 1000) == 0)
                    {
                        MICROSD_LOG(
                            MICROSD_LOG_DEBUG,
                            "Still waiting... response: 0x%02X, busy_count: %d, timeout: %d\n",
                            response,
                            busy_count,
                            timeout);
                    }
                    sleep_ms(1U); /* Add actual delay */
                    timeout--;
                } while((0x00U == response) && (timeout > 0));

                MICROSD_LOG(MICROSD_LOG_DEBUG,
                            "Final write completion response: 0x%02X (timeout=%d, busy_count=%d)\n",
                            response,
                            timeout,
                            busy_count);
                if(timeout > 0)
                {
                    MICROSD_LOG(MICROSD_LOG_DEBUG, "Write completed successfully!\n");
                    g_driver_info.total_writes++;
                    result = true;
                }
                else
                {
                    MICROSD_LOG(MICROSD_LOG_ERROR, "Write completion timed out\n");
                    g_driver_info.last_error_code = MICROSD_ERROR_WRITE_FAILED;
                }
            }
            else
            {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Data rejected by card (0x%02X)\n", response);
                if((response & 0x1FU) == 0x0B)
                {
                    MICROSD_LOG(MICROSD_LOG_ERROR, "Reason: CRC error\n");
                }
                else if((response & 0x1FU) == 0x0D)
                {
                    MICROSD_LOG(MICROSD_LOG_ERROR, "Reason: Write error\n");
                }
                else
                {
                    MICROSD_LOG(MICROSD_LOG_ERROR, "Reason: Unknown error\n");
                }
                g_driver_info.last_error_code = MICROSD_ERROR_WRITE_FAILED;
            }
        }
        else
        {
            MICROSD_LOG(MICROSD_LOG_ERROR, "CMD24 failed with response 0x%02X\n", response);
            g_driver_info.last_error_code = MICROSD_ERROR_WRITE_FAILED;
        }

        cs_deselect();
    }

    return result;
}

bool microsd_read_byte(uint32_t const address, uint8_t *const p_data)
{
    bool result = false;
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t const block_num = address / SD_BLOCK_SIZE;
    uint32_t const byte_offset = address % SD_BLOCK_SIZE;

    if(NULL != p_data)
    {
        if(microsd_read_block(block_num, buffer))
        {
            *p_data = buffer[byte_offset];
            result = true;
        }
    }

    return result;
}

bool microsd_write_byte(uint32_t const address, uint8_t const data)
{
    bool result = false;
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t const block_num = address / SD_BLOCK_SIZE;
    uint32_t const byte_offset = address % SD_BLOCK_SIZE;

    /* Read existing block first (Read-Modify-Write operation) */
    if(microsd_read_block(block_num, buffer))
    {
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
bool microsd_read_bytes(uint32_t const address, uint8_t *const p_buffer, uint32_t const length)
{
    bool result = false;

    if((NULL != p_buffer) && (length > 0U))
    {
        result = true;
        for(uint32_t i = 0U; i < length; i++)
        {
            if(!microsd_read_byte(address + i, &p_buffer[i]))
            {
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
bool microsd_write_bytes(uint32_t const address,
                         uint8_t const *const p_buffer,
                         uint32_t const length)
{
    bool result = false;

    if((NULL != p_buffer) && (length > 0U))
    {
        result = true;
        for(uint32_t i = 0U; i < length; i++)
        {
            if(!microsd_write_byte(address + i, p_buffer[i]))
            {
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
bool microsd_get_driver_info(microsd_driver_info_t *const p_info)
{
    if(NULL == p_info)
    {
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
void microsd_set_log_level(microsd_log_level_t level)
{
    g_log_level = level;
    MICROSD_LOG(MICROSD_LOG_INFO, "Log level set to %d\n", level);
}

/*!
 * @brief Print driver identification banner
 * @return void
 */
void microsd_print_banner(void)
{
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║                MicroSD Driver Information                ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ Driver ID    : %-40s ║\n", MICROSD_DRIVER_ID);
    printf("║ UUID         : %-40s ║\n", MICROSD_DRIVER_UUID);
    printf("║ Version      : v%d.%d.%d                                 ║\n",
           MICROSD_DRIVER_VERSION_MAJOR,
           MICROSD_DRIVER_VERSION_MINOR,
           MICROSD_DRIVER_VERSION_PATCH);
    printf("║ Status       : %-40s ║\n",
           g_driver_info.is_initialized ? "INITIALIZED" : "NOT INITIALIZED");
    printf("║ Total Reads  : %-40lu ║\n", (unsigned long) g_driver_info.total_reads);
    printf("║ Total Writes : %-40lu ║\n", (unsigned long) g_driver_info.total_writes);
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
static void add_to_log_buffer(microsd_log_level_t level, const char *fmt, ...)
{
    if(!g_log_to_file_enabled || g_log_buffer_pos >= LOG_BUFFER_SIZE - 256)
    {
        return; /* Buffer full or logging disabled */
    }

    /* Get current timestamp (approximate) */
    uint32_t timestamp_ms = to_ms_since_boot(get_absolute_time());

    /* Format timestamp and log level */
    int header_len = snprintf(&g_log_buffer[g_log_buffer_pos],
                              LOG_BUFFER_SIZE - g_log_buffer_pos,
                              "[%08lu][%s:%s] ",
                              (unsigned long) timestamp_ms,
                              MICROSD_DRIVER_ID,
                              (level == MICROSD_LOG_ERROR)  ? "ERROR"
                              : (level == MICROSD_LOG_WARN) ? "WARN"
                              : (level == MICROSD_LOG_INFO) ? "INFO"
                                                            : "DEBUG");

    if(header_len > 0 && g_log_buffer_pos + header_len < LOG_BUFFER_SIZE)
    {
        g_log_buffer_pos += header_len;

        /* Format the actual log message */
        va_list args;
        va_start(args, fmt);
        int msg_len = vsnprintf(
            &g_log_buffer[g_log_buffer_pos], LOG_BUFFER_SIZE - g_log_buffer_pos, fmt, args);
        va_end(args);

        if(msg_len > 0 && g_log_buffer_pos + msg_len < LOG_BUFFER_SIZE)
        {
            g_log_buffer_pos += msg_len;
        }
    }
}

/*!
 * @brief Enable/disable logging to file
 * @param[in] enable    true to enable, false to disable
 * @return void
 */
void microsd_enable_file_logging(bool enable)
{
    g_log_to_file_enabled = enable;
    if(enable)
    {
        g_log_buffer_pos = 0; /* Reset buffer when enabling */
        memset(g_log_buffer, 0, LOG_BUFFER_SIZE);
    }
}

/*!
 * @brief Flush log buffer to SD card file
 * @param[in] p_fs_info     Filesystem info pointer
 * @return bool             true on success, false on failure
 */
static bool flush_log_to_file(filesystem_info_t const *const p_fs_info)
{
    if(!g_log_to_file_enabled || g_log_buffer_pos == 0 || !p_fs_info)
    {
        return true; /* Nothing to do */
    }

    /* Create log filename with driver UUID */
    char log_filename[64];
    snprintf(log_filename, sizeof(log_filename), "%s.log", MICROSD_DRIVER_UUID);

    /* Use the existing file creation function to write log data */
    bool result =
        microsd_create_file(p_fs_info, log_filename, (uint8_t *) g_log_buffer, g_log_buffer_pos);

    if(result)
    {
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
bool microsd_init_filesystem(filesystem_info_t *const p_fs_info)
{
    uint8_t buffer[SD_BLOCK_SIZE];
    mbr_t *mbr;
    exfat_boot_sector_t *boot_sector;
    uint32_t partition_start = 0;

    if(NULL == p_fs_info)
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid filesystem info pointer\n");
        return false;
    }

    /* Initialize partition offset */
    p_fs_info->partition_offset = 0;

    /* Read block 0 (MBR or boot sector) */
    if(!microsd_read_block(0, buffer))
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read block 0\n");
        return false;
    }

    /* Check boot sector signature first */
    if(buffer[510] != 0x55 || buffer[511] != 0xAA)
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid boot sector signature\n");
        return false;
    }

    /* Debug: Show what we read from block 0 */
    MICROSD_LOG(MICROSD_LOG_DEBUG, "Block 0 signature: %02X %02X\n", buffer[510], buffer[511]);

    /* Hex dump first 64 bytes */
    MICROSD_LOG(MICROSD_LOG_DEBUG, "Block 0 hex dump (first 64 bytes):\n");
    for(int i = 0; i < 64; i += 16)
    {
        MICROSD_LOG(MICROSD_LOG_DEBUG,
                    "%04X: %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X "
                    "%02X %02X\n",
                    i,
                    buffer[i + 0],
                    buffer[i + 1],
                    buffer[i + 2],
                    buffer[i + 3],
                    buffer[i + 4],
                    buffer[i + 5],
                    buffer[i + 6],
                    buffer[i + 7],
                    buffer[i + 8],
                    buffer[i + 9],
                    buffer[i + 10],
                    buffer[i + 11],
                    buffer[i + 12],
                    buffer[i + 13],
                    buffer[i + 14],
                    buffer[i + 15]);
    }

    /* Check if this is a direct filesystem boot sector (no partition table) */
    if(memcmp(&buffer[3], "EXFAT   ", 8) == 0)
    {
        MICROSD_LOG(MICROSD_LOG_INFO, "Direct exFAT filesystem detected (no partition table)\n");
        partition_start = 0;
    }
    else if(memcmp(&buffer[82], "FAT32   ", 8) == 0 || memcmp(&buffer[54], "FAT16   ", 8) == 0)
    {
        MICROSD_LOG(MICROSD_LOG_INFO, "Direct FAT filesystem detected (not exFAT)\n");
        return false;
    }
    else
    {
        /* This is likely an MBR with partition table */
        MICROSD_LOG(MICROSD_LOG_INFO, "MBR with partition table detected\n");

        mbr = (mbr_t *) buffer;

        /* Look for exFAT partition (type 0x07) */
        bool partition_found = false;
        for(int i = 0; i < 4; i++)
        {
            MICROSD_LOG(MICROSD_LOG_DEBUG,
                        "Partition %d: type=0x%02X, first_lba=%lu, size=%lu\n",
                        i,
                        mbr->partitions[i].partition_type,
                        (unsigned long) mbr->partitions[i].first_lba,
                        (unsigned long) mbr->partitions[i].sector_count);

            if(mbr->partitions[i].partition_type == 0x07 && mbr->partitions[i].sector_count > 0)
            {
                partition_start = mbr->partitions[i].first_lba;
                partition_found = true;
                MICROSD_LOG(MICROSD_LOG_INFO,
                            "Found exFAT partition at sector %lu\n",
                            (unsigned long) partition_start);
                break;
            }
        }

        if(!partition_found)
        {
            MICROSD_LOG(MICROSD_LOG_ERROR, "No exFAT partition found in MBR\n");
            return false;
        }

        /* Read the actual boot sector from the partition */
        if(!microsd_read_block(partition_start, buffer))
        {
            MICROSD_LOG(MICROSD_LOG_ERROR,
                        "Failed to read partition boot sector at sector %lu\n",
                        (unsigned long) partition_start);
            return false;
        }

        MICROSD_LOG(MICROSD_LOG_DEBUG, "Partition boot sector hex dump (first 32 bytes):\n");
        for(int i = 0; i < 32; i += 16)
        {
            MICROSD_LOG(MICROSD_LOG_DEBUG,
                        "%04X: %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X "
                        "%02X %02X %02X\n",
                        i,
                        buffer[i + 0],
                        buffer[i + 1],
                        buffer[i + 2],
                        buffer[i + 3],
                        buffer[i + 4],
                        buffer[i + 5],
                        buffer[i + 6],
                        buffer[i + 7],
                        buffer[i + 8],
                        buffer[i + 9],
                        buffer[i + 10],
                        buffer[i + 11],
                        buffer[i + 12],
                        buffer[i + 13],
                        buffer[i + 14],
                        buffer[i + 15]);
        }
    }

    /* Now parse the exFAT boot sector */
    boot_sector = (exfat_boot_sector_t *) buffer;

    /* Check if it's exFAT filesystem */
    if(memcmp(boot_sector->filesystem_name, "EXFAT   ", 8) == 0)
    {
        MICROSD_LOG(MICROSD_LOG_INFO, "exFAT filesystem confirmed in partition\n");

        p_fs_info->is_exfat = true;
        p_fs_info->partition_offset = partition_start;

        /* Debug: Check volume flags for dirty bit and other states (AFTER setting partition_offset)
         */
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Volume flags: 0x%04X\n", boot_sector->volume_flags);
        if(boot_sector->volume_flags & 0x0002)
        {
            MICROSD_LOG(MICROSD_LOG_INFO, "Volume dirty bit is set - clearing it\n");
            boot_sector->volume_flags &= ~0x0002; /* Clear dirty bit */

            /* Write back the corrected boot sector to the CORRECT partition location */
            if(!microsd_write_block(p_fs_info->partition_offset, buffer))
            {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write corrected boot sector\n");
            }
            else
            {
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

        MICROSD_LOG(MICROSD_LOG_DEBUG,
                    "Partition offset: %lu\n",
                    (unsigned long) p_fs_info->partition_offset);
        MICROSD_LOG(MICROSD_LOG_DEBUG,
                    "Bytes per sector: %lu\n",
                    (unsigned long) p_fs_info->bytes_per_sector);
        MICROSD_LOG(MICROSD_LOG_DEBUG,
                    "Sectors per cluster: %lu\n",
                    (unsigned long) p_fs_info->sectors_per_cluster);
        MICROSD_LOG(MICROSD_LOG_DEBUG,
                    "Bytes per cluster: %lu\n",
                    (unsigned long) p_fs_info->bytes_per_cluster);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "FAT offset: %lu\n", (unsigned long) p_fs_info->fat_offset);
        MICROSD_LOG(MICROSD_LOG_DEBUG,
                    "Cluster heap offset: %lu\n",
                    (unsigned long) p_fs_info->cluster_heap_offset);
        MICROSD_LOG(
            MICROSD_LOG_DEBUG, "Root cluster: %lu\n", (unsigned long) p_fs_info->root_cluster);

        return true;
    }
    else
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Partition does not contain exFAT filesystem\n");
        MICROSD_LOG(MICROSD_LOG_DEBUG,
                    "Filesystem name bytes: %02X %02X %02X %02X %02X %02X %02X %02X\n",
                    boot_sector->filesystem_name[0],
                    boot_sector->filesystem_name[1],
                    boot_sector->filesystem_name[2],
                    boot_sector->filesystem_name[3],
                    boot_sector->filesystem_name[4],
                    boot_sector->filesystem_name[5],
                    boot_sector->filesystem_name[6],
                    boot_sector->filesystem_name[7]);
        return false;
    }
}

/*!
 * @brief Find a free cluster in the FAT
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @return uint32_t         Free cluster number, or 0 if none found
 */
static uint32_t find_free_cluster(filesystem_info_t const *const p_fs_info)
{
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t fat_sector = p_fs_info->partition_offset + p_fs_info->fat_offset;
    uint32_t entries_per_sector = SD_BLOCK_SIZE / sizeof(uint32_t);

    MICROSD_LOG(MICROSD_LOG_DEBUG,
                "Searching for free cluster in FAT (partition offset: %lu)\n",
                (unsigned long) p_fs_info->partition_offset);

    /* Search through FAT sectors */
    for(uint32_t sector = 0; sector < p_fs_info->fat_length; sector++)
    {
        if(!microsd_read_block(fat_sector + sector, buffer))
        {
            MICROSD_LOG(MICROSD_LOG_ERROR,
                        "Failed to read FAT sector %lu\n",
                        (unsigned long) (fat_sector + sector));
            continue;
        }

        uint32_t *fat_entries = (uint32_t *) buffer;

        /* Debug: Show first few FAT entries to understand allocation */
        if(sector == 0)
        {
            MICROSD_LOG(MICROSD_LOG_DEBUG, "FAT entries for clusters 2-7: ");
            for(uint32_t i = 0; i < 6 && i < entries_per_sector; i++)
            {
                MICROSD_LOG(MICROSD_LOG_DEBUG,
                            "cluster %lu=0x%08lX ",
                            (unsigned long) (i + 2),
                            (unsigned long) fat_entries[i]);
            }
            MICROSD_LOG(MICROSD_LOG_DEBUG, "\n");
        }

        for(uint32_t entry = 0; entry < entries_per_sector; entry++)
        {
            uint32_t cluster = sector * entries_per_sector + entry + 2; /* Clusters start at 2 */

            if(cluster >= p_fs_info->cluster_count + 2)
            {
                break; /* Beyond valid cluster range */
            }

            if(fat_entries[entry] == 0)
            {
                MICROSD_LOG(
                    MICROSD_LOG_DEBUG, "Found free cluster: %lu\n", (unsigned long) cluster);
                return cluster;
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
static bool mark_cluster_end(filesystem_info_t const *const p_fs_info, uint32_t cluster)
{
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t entries_per_sector = SD_BLOCK_SIZE / sizeof(uint32_t);
    uint32_t fat_sector = p_fs_info->partition_offset + p_fs_info->fat_offset +
                          ((cluster - 2) * sizeof(uint32_t)) / SD_BLOCK_SIZE;
    uint32_t entry_offset = ((cluster - 2) * sizeof(uint32_t)) % SD_BLOCK_SIZE;

    /* Read FAT sector */
    if(!microsd_read_block(fat_sector, buffer))
    {
        MICROSD_LOG(
            MICROSD_LOG_ERROR, "Failed to read FAT sector %lu\n", (unsigned long) fat_sector);
        return false;
    }

    /* Mark as end-of-chain (0xFFFFFFFF) */
    uint32_t *fat_entry = (uint32_t *) (buffer + entry_offset);
    *fat_entry = 0xFFFFFFFF;

    /* Write back */
    if(!microsd_write_block(fat_sector, buffer))
    {
        MICROSD_LOG(
            MICROSD_LOG_ERROR, "Failed to write FAT sector %lu\n", (unsigned long) fat_sector);
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Marked cluster %lu as end-of-chain\n", (unsigned long) cluster);
    return true;
}

/*!
 * @brief Calculate cluster start sector
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] cluster       Cluster number
 * @return uint32_t         Sector number of cluster start
 */
static uint32_t cluster_to_sector(filesystem_info_t const *const p_fs_info, uint32_t cluster)
{
    return p_fs_info->partition_offset + p_fs_info->cluster_heap_offset +
           (cluster - 2) * p_fs_info->sectors_per_cluster;
}

/*!
 * @brief Calculate exFAT name hash according to specification
 * @param[in] filename      Filename to hash
 * @param[in] length        Length of filename
 * @return uint16_t         Calculated name hash
 */
static uint16_t calculate_exfat_name_hash(char const *const filename, uint32_t length)
{
    uint16_t hash = 0;

    for(uint32_t i = 0; i < length; i++)
    {
        uint16_t c = (uint16_t) (uint8_t) filename[i];
        /* Convert to uppercase for hash calculation */
        if(c >= 'a' && c <= 'z')
        {
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
static bool update_allocation_bitmap(filesystem_info_t const *const p_fs_info, uint32_t cluster)
{
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t bitmap_cluster = 0;

    /* Find allocation bitmap by reading root directory entries */
    uint32_t root_sector = cluster_to_sector(p_fs_info, p_fs_info->root_cluster);

    /* Read root directory to find allocation bitmap directory entry */
    if(!microsd_read_block(root_sector, buffer))
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read root directory\n");
        return false;
    }

    /* Debug: Print all directory entries in root */
    MICROSD_LOG(MICROSD_LOG_INFO, "=== Root Directory Entries ===\n");
    for(int i = 0; i < SD_BLOCK_SIZE; i += 32)
    {
        uint8_t entry_type = buffer[i];
        if(entry_type == 0x00)
        {
            MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: End-of-directory marker\n", i / 32);
            break;
        }
        else if(entry_type != 0xFF)
        {
            MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: Type=0x%02X", i / 32, entry_type);
            if(entry_type & 0x80)
            {
                uint8_t type_code = entry_type & 0x1F;
                uint8_t type_importance = (entry_type >> 5) & 0x01;
                uint8_t type_category = (entry_type >> 6) & 0x01;
                MICROSD_LOG(MICROSD_LOG_INFO,
                            " (TypeCode=%d, Importance=%d, Category=%d)\n",
                            type_code,
                            type_importance,
                            type_category);

                /* Print FirstCluster and DataLength for entries that have them */
                if(type_category == 0)
                { /* Primary entry */
                    uint32_t first_cluster = *(uint32_t *) &buffer[i + 20];
                    uint64_t data_length = *(uint64_t *) &buffer[i + 24];
                    MICROSD_LOG(MICROSD_LOG_INFO,
                                "        FirstCluster=%u, DataLength=%llu\n",
                                first_cluster,
                                (unsigned long long) data_length);
                }
            }
            else
            {
                MICROSD_LOG(MICROSD_LOG_INFO, " (Unused)\n");
            }
        }
    }
    MICROSD_LOG(MICROSD_LOG_INFO, "=== End Directory Entries ===\n");

    /* Search for allocation bitmap directory entry (TypeCode = 1, TypeImportance = 0) */
    for(int i = 0; i < SD_BLOCK_SIZE; i += 32)
    {
        uint8_t entry_type = buffer[i];

        /* Check if this is an allocation bitmap directory entry */
        /* TypeCode = 1 (bits 0-4), TypeImportance = 0 (bit 5), TypeCategory = 0 (bit 6), InUse = 1
         * (bit 7) */
        if(entry_type == 0x81)
        { /* 0x81 = TypeCode=1, TypeImportance=0, TypeCategory=0, InUse=1 */
            /* Extract FirstCluster from bytes 20-23 */
            bitmap_cluster = *(uint32_t *) &buffer[i + 20];
            MICROSD_LOG(
                MICROSD_LOG_INFO, "Found allocation bitmap at cluster %u\n", bitmap_cluster);
            break;
        }

        /* Stop at end-of-directory marker */
        if(entry_type == 0x00)
        {
            break;
        }
    }

    if(bitmap_cluster == 0)
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Could not find allocation bitmap directory entry\n");

        /* Check if root directory is completely empty (needs initialization) */
        bool directory_empty = true;
        for(int i = 0; i < SD_BLOCK_SIZE; i += 32)
        {
            if(buffer[i] != 0x00)
            {
                directory_empty = false;
                break;
            }
        }

        if(directory_empty)
        {
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
    if(!microsd_read_block(bitmap_sector + sector_offset, buffer))
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read allocation bitmap sector\n");
        return false;
    }

    /* Set the bit for this cluster */
    buffer[byte_in_sector] |= (1 << bit_offset);

    /* Write back the bitmap sector */
    if(!microsd_write_block(bitmap_sector + sector_offset, buffer))
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write allocation bitmap sector\n");
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_DEBUG,
                "Updated allocation bitmap: cluster %lu marked as used\n",
                (unsigned long) cluster);
    return true;
}

/*!
 * @brief Calculate checksum for directory entry set
 * @param[in] p_entries     Pointer to directory entries
 * @param[in] count         Number of entries
 * @return uint16_t         Calculated checksum
 */
static uint16_t calculate_entry_checksum(uint8_t const *const p_entries, uint32_t count)
{
    uint16_t checksum = 0;

    for(uint32_t i = 0; i < count * 32; i++)
    {
        if(i == 2 || i == 3)
        {
            /* Skip checksum field itself */
            continue;
        }

        checksum = ((checksum & 1) ? 0x8000 : 0) + (checksum >> 1) + p_entries[i];
    }

    return checksum;
}

/*!
 * @brief Create a file in the exFAT filesystem
 * @param[in] p_fs_info     Pointer to filesystem info structure
 * @param[in] filename      Filename (null-terminated string)
 * @param[in] p_data        Pointer to file data
 * @param[in] data_length   Length of file data in bytes
 * @return bool             true on success, false on failure
 */
bool microsd_create_file(filesystem_info_t const *const p_fs_info,
                         char const *const filename,
                         uint8_t const *const p_data,
                         uint32_t const data_length)
{
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t root_sector, free_cluster, data_sector;
    uint32_t filename_len;

    if(NULL == p_fs_info || NULL == filename || NULL == p_data)
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid parameters for file creation\n");
        return false;
    }

    if(!p_fs_info->is_exfat)
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Not an exFAT filesystem\n");
        return false;
    }

    filename_len = strlen(filename);
    if(filename_len == 0 || filename_len > 255)
    {
        MICROSD_LOG(
            MICROSD_LOG_ERROR, "Invalid filename length: %lu\n", (unsigned long) filename_len);
        return false;
    }

    MICROSD_LOG(
        MICROSD_LOG_INFO, "Creating file: %s (%lu bytes)\n", filename, (unsigned long) data_length);

    /* Find free cluster for file data */
    free_cluster = find_free_cluster(p_fs_info);
    if(free_cluster == 0)
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "No free cluster available\n");
        return false;
    }

    /* Mark cluster as end-of-chain */
    if(!mark_cluster_end(p_fs_info, free_cluster))
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to mark cluster in FAT\n");
        return false;
    }

    /* Update allocation bitmap */
    if(!update_allocation_bitmap(p_fs_info, free_cluster))
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to update allocation bitmap\n");
        return false;
    }

    /* Write file data to cluster */
    data_sector = cluster_to_sector(p_fs_info, free_cluster);
    memset(buffer, 0, SD_BLOCK_SIZE);
    memcpy(buffer, p_data, (data_length > SD_BLOCK_SIZE) ? SD_BLOCK_SIZE : data_length);

    if(!microsd_write_block(data_sector, buffer))
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write file data\n");
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_DEBUG,
                "File data written to cluster %lu (sector %lu)\n",
                (unsigned long) free_cluster,
                (unsigned long) data_sector);

    /* Read root directory */
    root_sector = cluster_to_sector(p_fs_info, p_fs_info->root_cluster);
    if(!microsd_read_block(root_sector, buffer))
    {
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
    char *dot_pos = strrchr(filename, '.');

    if(dot_pos != NULL)
    {
        /* Has extension */
        size_t base_len = dot_pos - filename;
        strncpy(base_name, filename, base_len);
        base_name[base_len] = '\0';
        strcpy(extension, dot_pos);
    }
    else
    {
        /* No extension */
        strcpy(base_name, filename);
    }

    while(!filename_unique)
    {
        /* Generate filename with current version number */
        if(version_number == 0)
        {
            /* First try the original filename */
            strcpy(final_filename, filename);
        }
        else
        {
            /* Generate versioned filename: basename_N.ext */
            snprintf(final_filename,
                     sizeof(final_filename),
                     "%s_%lu%s",
                     base_name,
                     (unsigned long) version_number,
                     extension);
        }

        /* Check if this filename already exists in directory */
        filename_unique = true;
        for(uint32_t i = 0; i < (SD_BLOCK_SIZE / 32); i++)
        {
            uint8_t entry_type = buffer[i * 32];
            if(entry_type == 0x85)
            { /* File entry */
                uint8_t secondary_count = buffer[i * 32 + 1];

                /* Extract filename from name entries to compare */
                char existing_filename[256] = "";
                uint32_t char_index = 0;

                for(uint32_t j = 2; j <= secondary_count && (i + j) < (SD_BLOCK_SIZE / 32); j++)
                {
                    uint8_t name_entry_type = buffer[(i + j) * 32];
                    if(name_entry_type == 0xC1)
                    { /* Name entry */
                        uint16_t *utf16_chars = (uint16_t *) &buffer[(i + j) * 32 + 2];
                        for(uint32_t k = 0; k < 15 && char_index < 255; k++)
                        {
                            if(utf16_chars[k] == 0)
                                break; /* End of filename */
                            if(utf16_chars[k] < 128)
                            { /* ASCII character */
                                existing_filename[char_index++] = (char) utf16_chars[k];
                            }
                        }
                    }
                }
                existing_filename[char_index] = '\0';

                /* Compare with our proposed filename */
                if(strcmp(existing_filename, final_filename) == 0)
                {
                    filename_unique = false;
                    version_number++;
                    MICROSD_LOG(MICROSD_LOG_INFO,
                                "File '%s' already exists, trying version %lu\n",
                                final_filename,
                                (unsigned long) version_number);
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

    MICROSD_LOG(MICROSD_LOG_DEBUG,
                "Filename '%s' needs %lu name entries (%lu total)\n",
                final_filename,
                (unsigned long) name_entries_needed,
                (unsigned long) total_entries);

    /* Find empty directory entries */
    uint8_t *dir_entry = buffer;
    uint32_t entry_index = 0;

    /* Find end of directory - only insert at end-of-directory marker (0x00) */
    uint32_t available_entries = 0;
    uint32_t cleaned_entries = 0;

    /* First pass: Clean up corrupted/invalid file entries */
    for(uint32_t i = 3; i < (SD_BLOCK_SIZE / 32); i++)
    {
        uint8_t entry_type = dir_entry[i * 32];
        if(entry_type == 0x85)
        { /* File entry */
            /* Check for corrupted FirstCluster values */
            uint32_t first_cluster = *((uint32_t *) &dir_entry[i * 32 + 20]);
            if(first_cluster > 1000000)
            { /* Clearly invalid cluster number for small SD cards */
                MICROSD_LOG(MICROSD_LOG_INFO,
                            "Cleaning corrupted file entry %lu (invalid cluster %lu)\n",
                            (unsigned long) i,
                            (unsigned long) first_cluster);

                /* Clear this entry and its secondary entries */
                uint8_t secondary_count = dir_entry[i * 32 + 1];
                for(uint32_t j = 0; j <= secondary_count && (i + j) < (SD_BLOCK_SIZE / 32); j++)
                {
                    memset(&dir_entry[(i + j) * 32], 0, 32);
                    cleaned_entries++;
                }
                /* Skip past the cleared entries */
                i += secondary_count;
            }
        }
    }

    if(cleaned_entries > 0)
    {
        MICROSD_LOG(MICROSD_LOG_INFO,
                    "Cleaned %lu corrupted directory entries\n",
                    (unsigned long) cleaned_entries);

        /* Write the cleaned directory back to SD card */
        if(!microsd_write_block(root_sector, buffer))
        {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write cleaned directory\n");
            return false;
        }
        MICROSD_LOG(MICROSD_LOG_INFO, "Cleaned directory written successfully\n");
    }

    /* Second pass: Find end of directory */
    entry_index = 0;
    while(entry_index < (SD_BLOCK_SIZE / 32))
    {
        uint8_t entry_type = dir_entry[entry_index * 32];
        if(entry_type == 0x00)
        {
            /* Found end-of-directory marker - this is where we insert */
            MICROSD_LOG(MICROSD_LOG_DEBUG,
                        "Found end-of-directory at entry %lu\n",
                        (unsigned long) entry_index);
            available_entries = (SD_BLOCK_SIZE / 32) - entry_index;
            break;
        }
        /* Skip all other entries (system entries 0x81-0x83, file entries 0x85+, etc.) */
        entry_index++;
    }

    MICROSD_LOG(MICROSD_LOG_DEBUG,
                "Available directory entries: %lu, needed: %lu\n",
                (unsigned long) available_entries,
                (unsigned long) total_entries);

    if(available_entries < total_entries)
    {
        MICROSD_LOG(MICROSD_LOG_ERROR,
                    "Root directory is full (need %lu entries, only %lu available)\n",
                    (unsigned long) total_entries,
                    (unsigned long) available_entries);
        MICROSD_LOG(MICROSD_LOG_INFO,
                    "Directory contains %lu entries out of %lu maximum\n",
                    (unsigned long) entry_index,
                    (unsigned long) (SD_BLOCK_SIZE / 32));
        MICROSD_LOG(MICROSD_LOG_INFO, "Suggestion: Delete some files or reformat the SD card\n");
        return false;
    }

    /* Create directory entry set */
    uint8_t entries[32 * 5]; /* Max 5 entries: file + stream + 3 name entries */
    memset(entries, 0, sizeof(entries));

    /* File entry */
    exfat_file_entry_t *file_entry = (exfat_file_entry_t *) &entries[0];
    file_entry->entry_type = EXFAT_TYPE_FILE;
    file_entry->secondary_count =
        total_entries - 1;              /* Stream + Name entries (excluding file entry itself) */
    file_entry->file_attributes = 0x00; /* No special attributes - normal file */

    /* Use current time approximation - 2023-01-01 12:00:00 */
    uint32_t timestamp =
        ((2023 - 1980) << 25) | (1 << 21) | (1 << 16) | (12 << 11) | (0 << 5) | (0 >> 1);
    file_entry->created_timestamp = timestamp;
    file_entry->last_modified_timestamp = timestamp;
    file_entry->last_accessed_timestamp = timestamp;
    file_entry->created_10ms_increment = 0;
    file_entry->last_modified_10ms_increment = 0;

    /* Stream extension entry */
    exfat_stream_entry_t *stream_entry = (exfat_stream_entry_t *) &entries[32];
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
    for(uint32_t name_entry_idx = 0; name_entry_idx < name_entries_needed; name_entry_idx++)
    {
        exfat_name_entry_t *name_entry =
            (exfat_name_entry_t *) &entries[64 + (name_entry_idx * 32)];
        name_entry->entry_type = EXFAT_TYPE_FILE_NAME;
        name_entry->general_flags = 0x00;

        /* Convert filename to UTF-16 (simplified - ASCII only) */
        for(uint32_t i = 0; i < 15 && char_index < filename_len; i++, char_index++)
        {
            name_entry->file_name[i] = (uint16_t) final_filename[char_index];
        }

        MICROSD_LOG(
            MICROSD_LOG_DEBUG,
            "Name entry %lu: chars %lu-%lu\n",
            (unsigned long) name_entry_idx,
            (unsigned long) (char_index - (char_index < filename_len ? 15 : (filename_len % 15))),
            (unsigned long) (char_index - 1));
    }

    /* Calculate and set checksum for all entries */
    uint16_t checksum = calculate_entry_checksum(entries, total_entries);
    file_entry->set_checksum = checksum;

    /* Debug: Show the directory entries we're creating */
    MICROSD_LOG(
        MICROSD_LOG_DEBUG, "Directory entry set (%lu entries):\n", (unsigned long) total_entries);
    MICROSD_LOG(MICROSD_LOG_DEBUG,
                "  File entry: type=0x%02X, secondary_count=%d, checksum=0x%04X\n",
                file_entry->entry_type,
                file_entry->secondary_count,
                file_entry->set_checksum);
    MICROSD_LOG(MICROSD_LOG_DEBUG,
                "  Stream entry: type=0x%02X, name_len=%d, hash=0x%04X, cluster=%lu, size=%lu\n",
                stream_entry->entry_type,
                stream_entry->name_length,
                stream_entry->name_hash,
                (unsigned long) stream_entry->first_cluster,
                (unsigned long) stream_entry->data_length);

    /* Show all name entries */
    for(uint32_t i = 0; i < name_entries_needed; i++)
    {
        exfat_name_entry_t *name_entry = (exfat_name_entry_t *) &entries[64 + (i * 32)];
        MICROSD_LOG(MICROSD_LOG_DEBUG,
                    "  Name entry %lu: type=0x%02X, first 8 chars='%c%c%c%c%c%c%c%c'\n",
                    (unsigned long) i,
                    name_entry->entry_type,
                    (char) (name_entry->file_name[0] & 0xFF),
                    (char) (name_entry->file_name[1] & 0xFF),
                    (char) (name_entry->file_name[2] & 0xFF),
                    (char) (name_entry->file_name[3] & 0xFF),
                    (char) (name_entry->file_name[4] & 0xFF),
                    (char) (name_entry->file_name[5] & 0xFF),
                    (char) (name_entry->file_name[6] & 0xFF),
                    (char) (name_entry->file_name[7] & 0xFF));
    }

    /* Debug: Hex dump of directory entries */
    MICROSD_LOG(MICROSD_LOG_DEBUG, "Directory entry hex dump:\n");
    for(uint32_t i = 0; i < total_entries; i++)
    {
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Entry %lu: ", (unsigned long) i);
        for(uint32_t j = 0; j < 32; j += 8)
        {
            MICROSD_LOG(MICROSD_LOG_DEBUG,
                        "%02X %02X %02X %02X %02X %02X %02X %02X ",
                        entries[i * 32 + j],
                        entries[i * 32 + j + 1],
                        entries[i * 32 + j + 2],
                        entries[i * 32 + j + 3],
                        entries[i * 32 + j + 4],
                        entries[i * 32 + j + 5],
                        entries[i * 32 + j + 6],
                        entries[i * 32 + j + 7]);
        }
        MICROSD_LOG(MICROSD_LOG_DEBUG, "\n");
    }

    /* Copy entries to directory buffer */
    memcpy(&buffer[entry_index * 32], entries, total_entries * 32);

    /* Ensure proper end-of-directory marker */
    uint32_t next_entry_index = entry_index + total_entries;
    if(next_entry_index < (SD_BLOCK_SIZE / 32))
    {
        /* Clear the entry after our file to mark end-of-directory */
        memset(&buffer[next_entry_index * 32], 0, 32);
        MICROSD_LOG(MICROSD_LOG_DEBUG,
                    "Set end-of-directory marker at entry %lu\n",
                    (unsigned long) next_entry_index);
    }

    /* Write back root directory */
    if(!microsd_write_block(root_sector, buffer))
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write root directory\n");
        return false;
    }

    /* Verify directory was written correctly by reading it back */
    uint8_t verify_buffer[SD_BLOCK_SIZE];
    if(!microsd_read_block(root_sector, verify_buffer))
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to verify root directory write\n");
        return false;
    }

    /* Compare written vs read data */
    bool verify_ok = true;
    for(int i = 0; i < SD_BLOCK_SIZE; i++)
    {
        if(buffer[i] != verify_buffer[i])
        {
            MICROSD_LOG(MICROSD_LOG_ERROR,
                        "Directory verification failed at offset %d: wrote 0x%02X, read 0x%02X\n",
                        i,
                        buffer[i],
                        verify_buffer[i]);
            verify_ok = false;
            break;
        }
    }

    if(verify_ok)
    {
        MICROSD_LOG(MICROSD_LOG_INFO, "Directory write verification successful\n");
    }
    else
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Directory write verification failed\n");
        return false;
    }

    /* Force filesystem sync by reading multiple blocks to flush caches */
    uint8_t sync_buffer[SD_BLOCK_SIZE];
    for(int i = 0; i < 3; i++)
    {
        uint32_t sync_sector = p_fs_info->partition_offset + p_fs_info->cluster_heap_offset +
                               (4 * p_fs_info->sectors_per_cluster) + i;
        microsd_read_block(sync_sector, sync_buffer);
        sleep_ms(10); /* Small delay to ensure write completion */
    }

    /* Debug: Show final directory structure */
    MICROSD_LOG(MICROSD_LOG_INFO, "=== Final Root Directory Structure ===\n");
    for(int i = 0; i < SD_BLOCK_SIZE; i += 32)
    {
        uint8_t entry_type = buffer[i];
        if(entry_type == 0x00)
        {
            MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: End-of-directory marker\n", i / 32);
            break;
        }
        else if(entry_type != 0xFF && (entry_type & 0x80))
        {
            uint8_t type_code = entry_type & 0x1F;
            uint8_t type_importance = (entry_type >> 5) & 0x01;
            uint8_t type_category = (entry_type >> 6) & 0x01;

            if(type_code == 1 && type_importance == 0 && type_category == 0)
            {
                MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: Allocation Bitmap\n", i / 32);
            }
            else if(type_code == 2 && type_importance == 0 && type_category == 0)
            {
                MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: Up-case Table\n", i / 32);
            }
            else if(type_code == 3 && type_importance == 0 && type_category == 0)
            {
                MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: Volume Label\n", i / 32);
            }
            else if(type_code == 5 && type_importance == 0 && type_category == 0)
            {
                uint8_t secondary_count = buffer[i + 1];
                MICROSD_LOG(MICROSD_LOG_INFO,
                            "Entry %d: File (secondary_count=%d)\n",
                            i / 32,
                            secondary_count);
            }
            else if(type_code == 0 && type_importance == 0 && type_category == 1)
            {
                MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: Stream Extension\n", i / 32);
            }
            else if(type_code == 1 && type_importance == 0 && type_category == 1)
            {
                MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: File Name\n", i / 32);
            }
            else
            {
                MICROSD_LOG(MICROSD_LOG_INFO,
                            "Entry %d: Unknown (TypeCode=%d, Importance=%d, Category=%d)\n",
                            i / 32,
                            type_code,
                            type_importance,
                            type_category);
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
bool microsd_read_file(filesystem_info_t const *const p_fs_info,
                       char const *const filename,
                       uint8_t *const p_buffer,
                       uint32_t const buffer_size,
                       uint32_t *const p_bytes_read)
{
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t root_sector;
    uint32_t filename_len;
    uint32_t file_cluster = 0;
    uint32_t file_size = 0;
    bool file_found = false;

    if(NULL == p_fs_info || NULL == filename || NULL == p_buffer || NULL == p_bytes_read)
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid parameters for file reading\n");
        return false;
    }

    *p_bytes_read = 0;

    if(!p_fs_info->is_exfat)
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Not an exFAT filesystem\n");
        return false;
    }

    filename_len = strlen(filename);
    if(filename_len == 0 || filename_len > 255)
    {
        MICROSD_LOG(
            MICROSD_LOG_ERROR, "Invalid filename length: %lu\n", (unsigned long) filename_len);
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Reading file: %s\n", filename);

    /* Calculate root directory sector */
    root_sector = p_fs_info->partition_offset + p_fs_info->cluster_heap_offset +
                  ((p_fs_info->root_cluster - 2) * p_fs_info->sectors_per_cluster);

    /* Read root directory */
    if(!microsd_read_block(root_sector, buffer))
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read root directory\n");
        return false;
    }

    /* Search for the file in the directory */
    for(uint32_t i = 0; i < (SD_BLOCK_SIZE / 32); i++)
    {
        uint8_t entry_type = buffer[i * 32];

        if(entry_type == 0x00)
        {
            /* End of directory */
            break;
        }

        if(entry_type == 0x85)
        { /* File entry */
            uint8_t secondary_count = buffer[i * 32 + 1];

            /* Extract filename from name entries */
            char current_filename[256] = "";
            uint32_t char_index = 0;

            /* Look for stream extension entry (should be next entry) */
            if((i + 1) < (SD_BLOCK_SIZE / 32) && buffer[(i + 1) * 32] == 0xC0)
            {
                /* Get file size from stream extension */
                uint32_t *stream_data = (uint32_t *) &buffer[(i + 1) * 32];
                file_size = stream_data[2];    /* DataLength at offset 8 */
                file_cluster = stream_data[5]; /* FirstCluster at offset 20 */
            }

            /* Extract filename from name entries */
            for(uint32_t j = 2; j <= secondary_count && (i + j) < (SD_BLOCK_SIZE / 32); j++)
            {
                uint8_t name_entry_type = buffer[(i + j) * 32];
                if(name_entry_type == 0xC1)
                { /* Name entry */
                    uint16_t *utf16_chars = (uint16_t *) &buffer[(i + j) * 32 + 2];
                    for(uint32_t k = 0; k < 15 && char_index < 255; k++)
                    {
                        if(utf16_chars[k] == 0)
                            break; /* End of filename */
                        if(utf16_chars[k] < 128)
                        { /* ASCII character */
                            current_filename[char_index++] = (char) utf16_chars[k];
                        }
                    }
                }
            }
            current_filename[char_index] = '\0';

            /* Check if this is the file we're looking for */
            if(strcmp(current_filename, filename) == 0)
            {
                MICROSD_LOG(MICROSD_LOG_INFO,
                            "Found file '%s' at cluster %lu, size %lu bytes\n",
                            filename,
                            (unsigned long) file_cluster,
                            (unsigned long) file_size);
                file_found = true;
                break;
            }

            /* Skip past this file's entries */
            i += secondary_count;
        }
    }

    if(!file_found)
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "File '%s' not found\n", filename);
        return false;
    }

    /* Validate file parameters */
    if(file_size == 0)
    {
        MICROSD_LOG(MICROSD_LOG_INFO, "File '%s' is empty\n", filename);
        *p_bytes_read = 0;
        return true;
    }

    if(file_cluster == 0)
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid cluster number for file '%s'\n", filename);
        return false;
    }

    /* Calculate how many bytes to read */
    uint32_t bytes_to_read = (file_size < buffer_size) ? file_size : buffer_size;

    /* Read file data from cluster */
    uint32_t file_sector = p_fs_info->partition_offset + p_fs_info->cluster_heap_offset +
                           ((file_cluster - 2) * p_fs_info->sectors_per_cluster);

    MICROSD_LOG(
        MICROSD_LOG_DEBUG, "Reading file data from sector %lu\n", (unsigned long) file_sector);

    if(!microsd_read_block(file_sector, buffer))
    {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read file data\n");
        return false;
    }

    /* Copy file data to output buffer */
    memcpy(p_buffer, buffer, bytes_to_read);
    *p_bytes_read = bytes_to_read;

    MICROSD_LOG(MICROSD_LOG_INFO,
                "Successfully read %lu bytes from file '%s'\n",
                (unsigned long) bytes_to_read,
                filename);

    return true;
}
