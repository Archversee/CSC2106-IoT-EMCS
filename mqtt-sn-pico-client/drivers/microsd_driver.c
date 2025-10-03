/*!
 * @file    microsd_driver.c
 * @brief   MicroSD card driver implementation for Raspberry Pi Pico W
 * @author  INF2004 Team
 * @date    2024
 */

#include "microsd_driver.h"

#include <string.h>

#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

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
    int timeout;

    /* Initialize SPI */
    (void)spi_init(SD_SPI_PORT, 400000U); /* Start at 400kHz */

    /* Configure GPIO pins */
    gpio_set_function(SD_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SD_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SD_SCK_PIN, GPIO_FUNC_SPI);

    /* CS pin as output */
    gpio_init(SD_CS_PIN);
    gpio_set_dir(SD_CS_PIN, GPIO_OUT);
    gpio_put(SD_CS_PIN, 1U);

    sleep_ms(10U); /* Wait for SD card to power up */

    /* Send 80 clock cycles with CS high */
    cs_deselect();
    for (int i = 0; i < 10; i++) {
        (void)spi_transfer(0xFFU);
    }

    /* Enter SPI mode */
    cs_select();
    response = send_command(CMD0, 0U);
    cs_deselect();

    if (R1_IDLE_STATE == response) {
        /* Check voltage range */
        cs_select();
        response = send_command(CMD8, 0x01AAU);
        if (R1_IDLE_STATE == response) {
            /* Read the rest of R7 response */
            for (int i = 0; i < 4; i++) {
                (void)spi_transfer(0xFFU);
            }
        }
        cs_deselect();

        /* Initialize card */
        timeout = 1000;
        do {
            cs_select();
            (void)send_command(CMD55, 0U);
            response = send_command(ACMD41, 0x40000000U);
            cs_deselect();
            sleep_ms(1U);
            timeout--;
        } while ((R1_READY_STATE != response) && (timeout > 0));

        if (timeout > 0) {
            /* Increase SPI speed */
            (void)spi_set_baudrate(SD_SPI_PORT, 10000000U); /* 10MHz */
            result = true;
        }
    }

    return result;
}

bool microsd_read_block(uint32_t const block_num, uint8_t* const p_buffer) {
    bool result = false;
    uint8_t response;
    int timeout;

    if (NULL != p_buffer) {
        cs_select();

        response = send_command(CMD17, block_num);
        if (0x00U == response) {
            /* Wait for data token */
            timeout = 1000;
            do {
                response = spi_transfer(0xFFU);
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

                result = true;
            }
        }

        cs_deselect();
    }

    return result;
}

bool microsd_write_block(uint32_t const block_num, uint8_t const* const p_buffer) {
    bool result = false;
    uint8_t response;
    int timeout;

    if (NULL != p_buffer) {
        cs_select();

        response = send_command(CMD24, block_num);
        if (0x00U == response) {
            /* Send data token */
            (void)spi_transfer(0xFEU);

            /* Send 512 bytes */
            for (int i = 0; i < SD_BLOCK_SIZE; i++) {
                (void)spi_transfer(p_buffer[i]);
            }

            /* Send dummy CRC */
            (void)spi_transfer(0xFFU);
            (void)spi_transfer(0xFFU);

            /* Check data response */
            response = spi_transfer(0xFFU);
            if (0x05U == (response & 0x1FU)) {
                /* Wait for write to complete */
                timeout = 1000;
                do {
                    response = spi_transfer(0xFFU);
                    timeout--;
                } while ((0x00U == response) && (timeout > 0));

                if (timeout > 0) {
                    result = true;
                }
            }
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

    /* Read existing block */
    if (microsd_read_block(block_num, buffer)) {
        /* Modify the byte */
        buffer[byte_offset] = data;

        /* Write back the block */
        result = microsd_write_block(block_num, buffer);
    }

    return result;
}