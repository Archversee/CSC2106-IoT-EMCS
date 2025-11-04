/*!
 * @file    microsd_spi.h
 * @brief   SPI interface layer for MicroSD card communication
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 */

#ifndef MICROSD_SPI_H
#define MICROSD_SPI_H

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

/*! Block size in bytes - SD cards use 512-byte sectors */
#define SD_BLOCK_SIZE (512U)

/*!
 * @brief Initialize SPI interface for SD card
 * @return bool true on success, false on failure
 */
bool microsd_spi_init(void);

/*!
 * @brief Transfer single byte via SPI
 * @param[in] data  Data byte to transfer
 * @return uint8_t  Received data byte
 */
uint8_t microsd_spi_transfer(uint8_t const data);

/*!
 * @brief Select SD card (CS low)
 * @return void
 */
void microsd_spi_cs_select(void);

/*!
 * @brief Deselect SD card (CS high)
 * @return void
 */
void microsd_spi_cs_deselect(void);

/*!
 * @brief Send command to SD card
 * @param[in] cmd   Command number
 * @param[in] arg   Command argument
 * @return uint8_t  Response byte
 */
uint8_t microsd_spi_send_command(uint8_t const cmd, uint32_t const arg);

/*!
 * @brief Read a block from SD card via SPI
 * @param[in]  block_num    Block number to read
 * @param[out] p_buffer     Pointer to buffer for read data (must be SD_BLOCK_SIZE bytes)
 * @return bool             true on success, false on failure
 */
bool microsd_spi_read_block(uint32_t const block_num, uint8_t *const p_buffer);

/*!
 * @brief Write a block to SD card via SPI
 * @param[in] block_num     Block number to write
 * @param[in] p_buffer      Pointer to data buffer (must be SD_BLOCK_SIZE bytes)
 * @return bool             true on success, false on failure
 */
bool microsd_spi_write_block(uint32_t const block_num, uint8_t const *const p_buffer);

#endif /* MICROSD_SPI_H */
