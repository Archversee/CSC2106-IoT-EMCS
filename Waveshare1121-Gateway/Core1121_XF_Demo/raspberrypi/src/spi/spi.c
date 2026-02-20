/*****************************************************************************
 * | File         :   spi.c
 * | Author       :   Waveshare team
 * | Function     :   Hardware underlying interface
 * | Info         :
 * |                 SPI driver code for SPI communication.
 * ----------------
 * | This version :   V1.0
 * | Date         :   2025-06-26
 * | Info         :   Basic version
 *
 ******************************************************************************/
#include "spi.h"

/******************************************************************************
function:	SPI Function initialization and transfer
parameter:
Info:
******************************************************************************/
void DEV_SPI_Init()
{
    printf("WIRINGPI SPI Device\r\n");       
    if(wiringPiSPISetupMode(0, 10*1000*1000, 0) < 0)
    {
        printf("set wiringPi spi failed	!!! \r\n");
    }
}

void DEV_SPI_Write_Bytes(uint8_t *tx_buf, const uint16_t length)
{
    // Copy to the temporary buffer, because wiringPiSPIDataRW is a read-write function, it will modify the original array
    uint8_t buffer[length];
    memcpy(buffer, tx_buf, length);
    wiringPiSPIDataRW(0, buffer, length);
}

void DEV_SPI_Read_Bytes(uint8_t* rx_buf, const uint16_t length)
{
    memset(rx_buf, 0x00, length);
    wiringPiSPIDataRW(0, rx_buf, length);
}