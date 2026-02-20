#ifndef _DEV_GPIO_H_
#define _DEV_GPIO_H_
/***********************************************************************************************************************
			------------------------------------------------------------------------
			|\\\																///|
			|\\\					Hardware interface							///|
			------------------------------------------------------------------------
***********************************************************************************************************************/
#include <wiringPi.h>

#include <stdint.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


/* Pin Definitions */
#define RADIO_CS 8 
#define RADIO_RESET 22 
#define RADIO_BUSY 24 
#define RADIO_IRQ 23
#define RADIO_LED 18

/*------------------------------------------------------------------------------------------------------*/
uint8_t DEV_ModuleInit(void);
void    DEV_ModuleExit(void);

void DEV_GPIO_Mode(uint16_t Pin, uint16_t Mode);
void DEV_GPIO_INT(int32_t Pin, void *isr_handler);
void DEV_Digital_Write(uint16_t Pin, uint8_t Value);
uint8_t DEV_Digital_Read(uint16_t Pin);

void DEV_Delay_ms(uint32_t xms);
void DEV_Delay_us(uint32_t xus);
#endif
