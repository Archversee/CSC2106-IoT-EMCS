/*!
 * @file    microsd_spi.c
 * @brief   SPI interface layer for MicroSD card communication
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 */

#include "microsd_spi.h"

#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

/*! Static variables for tracking initialization state */
static bool g_spi_initialized = false;

/*!
 * @brief Initialize SPI interface for SD card
 * @return bool true on success, false on failure
 */
bool microsd_spi_init(void) {
    if (g_spi_initialized) {
        /* Already initialized, deinitialize first */
        spi_deinit(SD_SPI_PORT);
        sleep_ms(10U);
    }

    /* Initialize SPI at low speed for initialization */
    (void)spi_init(SD_SPI_PORT, 100000U); /* Start at 100kHz - very slow */

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

    g_spi_initialized = true;
    return true;
}

/*!
 * @brief Transfer single byte via SPI
 * @param[in] data  Data byte to transfer
 * @return uint8_t  Received data byte
 */
uint8_t microsd_spi_transfer(uint8_t const data) {
    uint8_t rx_data;
    spi_write_read_blocking(SD_SPI_PORT, &data, &rx_data, 1U);
    return rx_data;
}

/*!
 * @brief Select SD card (CS low)
 * @return void
 */
void microsd_spi_cs_select(void) {
    gpio_put(SD_CS_PIN, 0U);
    sleep_us(1U);
}

/*!
 * @brief Deselect SD card (CS high)
 * @return void
 */
void microsd_spi_cs_deselect(void) {
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
uint8_t microsd_spi_send_command(uint8_t const cmd, uint32_t const arg) {
    uint8_t response;
    uint8_t retry = 0U;

    /* Send command packet */
    (void)microsd_spi_transfer(0x40U | cmd);          /* Command with start bit */
    (void)microsd_spi_transfer((arg >> 24U) & 0xFFU); /* Argument byte 3 */
    (void)microsd_spi_transfer((arg >> 16U) & 0xFFU); /* Argument byte 2 */
    (void)microsd_spi_transfer((arg >> 8U) & 0xFFU);  /* Argument byte 1 */
    (void)microsd_spi_transfer(arg & 0xFFU);          /* Argument byte 0 */

    /* CRC (only matters for CMD0 and CMD8) */
    if (CMD0 == cmd) {
        (void)microsd_spi_transfer(0x95U);
    } else if (CMD8 == cmd) {
        (void)microsd_spi_transfer(0x87U);
    } else {
        (void)microsd_spi_transfer(0x01U);
    }

    /* Wait for response */
    do {
        response = microsd_spi_transfer(0xFFU);
        retry++;
    } while ((0U != (response & 0x80U)) && (retry < 10U));

    return response;
}

/*!
 * @brief Read a block from SD card via SPI
 * @param[in]  block_num    Block number to read
 * @param[out] p_buffer     Pointer to buffer for read data (must be SD_BLOCK_SIZE bytes)
 * @return bool             true on success, false on failure
 */
bool microsd_spi_read_block(uint32_t const block_num, uint8_t *const p_buffer) {
    bool result = false;
    uint8_t response;
    int timeout;

    if (NULL != p_buffer) {
        microsd_spi_cs_select();

        response = microsd_spi_send_command(CMD17, block_num);
        if (0x00U == response) {
            /* Wait for data token */
            timeout = 1000;
            do {
                response = microsd_spi_transfer(0xFFU);
                timeout--;
            } while ((0xFEU != response) && (timeout > 0));

            if (timeout > 0) {
                /* Read 512 bytes */
                for (int i = 0; i < SD_BLOCK_SIZE; i++) {
                    p_buffer[i] = microsd_spi_transfer(0xFFU);
                }

                /* Read CRC (ignore) */
                (void)microsd_spi_transfer(0xFFU);
                (void)microsd_spi_transfer(0xFFU);

                result = true;
            }
        }

        microsd_spi_cs_deselect();
    }

    return result;
}

/*!
 * @brief Write a block to SD card via SPI
 * @param[in] block_num     Block number to write
 * @param[in] p_buffer      Pointer to data buffer (must be SD_BLOCK_SIZE bytes)
 * @return bool             true on success, false on failure
 */
bool microsd_spi_write_block(uint32_t const block_num, uint8_t const *const p_buffer) {
    bool result = false;
    uint8_t response;
    int timeout;

    if (NULL != p_buffer) {
        microsd_spi_cs_select();

        response = microsd_spi_send_command(CMD24, block_num);
        if (0x00U == response) {
            /* Send data token */
            (void)microsd_spi_transfer(0xFEU);

            /* Send 512 bytes */
            for (int i = 0; i < SD_BLOCK_SIZE; i++) {
                (void)microsd_spi_transfer(p_buffer[i]);
            }

            /* Send dummy CRC */
            (void)microsd_spi_transfer(0xFFU);
            (void)microsd_spi_transfer(0xFFU);

            /* Check data response */
            response = microsd_spi_transfer(0xFFU);
            if (0x05U == (response & 0x1FU)) {
                /* Wait for write to complete */
                timeout = 5000; /* Increased timeout to 5000 iterations */
                do {
                    response = microsd_spi_transfer(0xFFU);
                    sleep_ms(1U); /* Add actual delay */
                    timeout--;
                } while ((0x00U == response) && (timeout > 0));

                if (timeout > 0) {
                    result = true;
                }
            }
        }

        microsd_spi_cs_deselect();
    }

    return result;
}
