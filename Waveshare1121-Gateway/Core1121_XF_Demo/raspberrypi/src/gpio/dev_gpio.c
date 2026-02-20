/*****************************************************************************
 * | File      	 :   gpio.c
 * | Author      :   Waveshare team
 * | Function    :   Hardware underlying interface
 * | Info        :
 *                   GPIO driver code
 *----------------
 * |This version :   V1.0
 * | Date        :   2024-11-19
 * | Info        :   Basic version
 *
 ******************************************************************************/
#include "dev_gpio.h"

/**
 * @brief Configure a GPIO pin as input or output
 *
 * This function initializes a GPIO pin with the specified mode (input or output).
 * If set as input, it also enables the pull-up resistor by default.
 *
 * @param Pin GPIO pin number
 * @param Mode GPIO mode: 0 or GPIO_MODE_INPUT for input, others for output
 */
void DEV_GPIO_Mode(uint16_t Pin, uint16_t Mode)
{
    /*
        0:  INPT
        1:  OUTP
    */
    if (Mode == 0 || Mode == INPUT)
    {
        pinMode(Pin, INPUT);
        pullUpDnControl(Pin, PUD_UP);
    }
    else
    {
        pinMode(Pin, OUTPUT);
    }
}

/**
 * @brief Configure a GPIO pin for interrupt handling
 *
 * This function sets up a GPIO pin to generate an interrupt on a negative edge
 * (falling edge) and registers the specified interrupt handler.
 *
 * @param Pin GPIO pin number
 * @param isr_handler Pointer to the interrupt handler function
 */
void DEV_GPIO_INT(int32_t Pin, void *isr_handler)
{
    pinMode(Pin, INPUT);
    pullUpDnControl(Pin, PUD_DOWN);
    wiringPiISR(Pin,INT_EDGE_RISING,isr_handler);
}

/**
 * @brief Set the logic level of a GPIO pin
 *
 * This function sets the logic level (high or low) of a GPIO pin.
 *
 * @param Pin GPIO pin number
 * @param Value Logic level: 0 for low, 1 for high
 */
void DEV_Digital_Write(uint16_t Pin, uint8_t Value)
{

    digitalWrite(Pin, Value);
}

/**
 * @brief Read the logic level of a GPIO pin
 *
 * This function reads and returns the current logic level of a GPIO pin.
 *
 * @param Pin GPIO pin number
 * @return uint8_t Logic level: 0 for low, 1 for high
 */
uint8_t DEV_Digital_Read(uint16_t Pin)
{
    uint8_t Read_value = 0;
    Read_value = digitalRead(Pin);
    return Read_value;
}

/**
 * delay x ms
 **/
void DEV_Delay_ms(uint32_t xms)
{
    delay(xms);
}

/**
 * delay x ms
 **/
void DEV_Delay_us(uint32_t xus)
{
    delayMicroseconds(xus);
}

/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
uint8_t DEV_ModuleInit(void)
{

    // if(wiringPiSetup() < 0)//use wiringpi Pin number table
    if (wiringPiSetupGpio() < 0)
    { // use BCM2835 Pin number table
        printf("set wiringPi lib failed	!!! \r\n");
        return 1;
    }
    else
    {
        printf("set wiringPi lib success  !!! \r\n");
    }

    return 0;
}

/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_ModuleExit(void)
{
}
