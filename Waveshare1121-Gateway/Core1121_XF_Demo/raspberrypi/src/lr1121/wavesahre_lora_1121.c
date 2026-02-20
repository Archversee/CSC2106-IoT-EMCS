/*****************************************************************************
 * | File      	 :   wavesahre_lora_1121.c
 * | Author      :   Waveshare team
 * | Function    :   Hardware underlying interface
 * | Info        :
 *                   
 *----------------
 * |This version :   V1.0
 * | Date        :   2025-06-27
 * | Info        :   Basic version
 *
 ******************************************************************************/
#include "wavesahre_lora_1121.h"

// Initialize the IO context by assigning GPIO pins to the LR1121 context structure
void lora_init_io_context(const void *context)
{
    ((lr1121_t *)context)->reset = RADIO_RESET;   // Reset pin
    ((lr1121_t *)context)->led = RADIO_LED;       // LED indicator pin
    ((lr1121_t *)context)->cs = RADIO_CS;         // Chip Select pin for SPI
    ((lr1121_t *)context)->irq = RADIO_IRQ;       // Interrupt pin
    ((lr1121_t *)context)->busy = RADIO_BUSY;     // Busy status pin
}

// Initialize the IO pins (configure GPIO modes)
void lora_init_io(const void *context)
{
    DEV_ModuleInit();  // Initialize the hardware module (GPIO, SPI, etc.)

    DEV_GPIO_Mode(((lr1121_t *)context)->reset, OUTPUT);  // Set reset pin as output
    DEV_GPIO_Mode(((lr1121_t *)context)->cs, OUTPUT);     // Set CS pin as output
    DEV_GPIO_Mode(((lr1121_t *)context)->busy, INPUT);    // Set busy pin as input

    DEV_Digital_Write(((lr1121_t *)context)->reset, 1 );  // Set reset pin to high
}

// Initialize interrupt pin and register the interrupt handler
void lora_init_irq(const void *context, void *handler)
{
    DEV_GPIO_INT(((lr1121_t *)context)->irq, handler);  // Attach IRQ handler to IRQ pin
}

// Initialize SPI interface for communication with LR1121
void lora_spi_init(const void* context)
{
    DEV_SPI_Init();  // SPI initialization
}

// Write bytes to LR1121 over SPI
void lora_spi_write_bytes(const void* context, uint8_t *wirte, const uint16_t wirte_length)
{
    DEV_SPI_Write_Bytes(wirte, wirte_length);  // Send byte array via SPI
}

// Read bytes from LR1121 over SPI
void lora_spi_read_bytes(const void* context, uint8_t *read, const uint16_t read_length)
{
    DEV_SPI_Read_Bytes(read, read_length);  // Read byte array via SPI
}

// Clear all pending modem events by polling until no more events are reported
lr1121_modem_response_code_t lr1121_modem_board_event_flush(const void* context)
{
    lr1121_modem_response_code_t modem_response_code = LR1121_MODEM_RESPONSE_CODE_OK;
    lr1121_modem_event_fields_t  event_fields;

    // Keep reading events until there are no more
    do
    {
        modem_response_code = lr1121_modem_get_event(context, &event_fields);
    } while (modem_response_code != LR1121_MODEM_RESPONSE_CODE_NO_EVENT);

    return modem_response_code;
}
