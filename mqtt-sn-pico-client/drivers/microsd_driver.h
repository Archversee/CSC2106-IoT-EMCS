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

/*!
 * @brief Initialize microSD card
 * @return bool true on success, false on failure
 */
bool microsd_init(void);

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

#endif /* MICROSD_DRIVER_H */
