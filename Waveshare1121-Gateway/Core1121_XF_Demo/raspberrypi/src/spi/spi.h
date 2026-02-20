#ifndef _DEV_SPI_H_
#define _DEV_SPI_H_
/***********************************************************************************************************************
			------------------------------------------------------------------------
			|\\\																///|
			|\\\					Hardware interface							///|
			------------------------------------------------------------------------
***********************************************************************************************************************/
#include "dev_gpio.h"
#include <wiringPiSPI.h>

#include <stdint.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>




/*------------------------------------------------------------------------------------------------------*/
void DEV_SPI_Init();
void DEV_SPI_Write_Bytes(uint8_t *tx_buf, const uint16_t length);
void DEV_SPI_Read_Bytes(uint8_t* rx_buf, const uint16_t length);

#endif
