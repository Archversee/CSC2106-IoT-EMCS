/*!
 * @file    main.c
 * @brief   MQTT-SN Pico Client main application
 * @author  INF2004 Team
 * @date    2024
 */

#include <stdio.h>

#include "drivers/microsd_driver.h"
#include "pico/stdlib.h"

/*! Test constants */
#define TEST_ADDRESS (1024U)
#define TEST_DATA (0xABU)
#define TEST_ITERATIONS (10U)
#define TEST_BASE_ADDRESS (2048U)
#define STARTUP_DELAY_MS (2000U)
#define TEST_DELAY_MS (500U)
#define ERROR_DELAY_MS (1000U)

/*!
 * @brief Main application entry point
 * @return int  Exit status (never returns)
 */
int main(void) {
    uint32_t test_count;
    uint8_t read_data;
    bool init_success;

    stdio_init_all();

    printf("MQTT-SN Pico Client - MicroSD Demo Starting...\n");
    sleep_ms(STARTUP_DELAY_MS);

    /* Initialize microSD card */
    printf("Initializing microSD card...\n");
    init_success = microsd_init();

    if (!init_success) {
        printf("ERROR: Failed to initialize microSD card!\n");
        while (1) {
            sleep_ms(ERROR_DELAY_MS);
        }
    }

    printf("MicroSD card initialized successfully!\n");

    /* Test single byte write and read */
    printf("\n=== Single Byte Test ===\n");

    printf("Writing byte 0x%02X to address %u...\n", TEST_DATA, TEST_ADDRESS);

    if (!microsd_write_byte(TEST_ADDRESS, TEST_DATA)) {
        printf("ERROR: Failed to write byte!\n");
    } else {
        printf("Byte written successfully!\n");
    }

    printf("Reading byte from address %u...\n", TEST_ADDRESS);

    if (!microsd_read_byte(TEST_ADDRESS, &read_data)) {
        printf("ERROR: Failed to read byte!\n");
    } else {
        printf("Read byte: 0x%02X\n", read_data);

        if (read_data == TEST_DATA) {
            printf("SUCCESS: Data matches!\n");
        } else {
            printf("ERROR: Data mismatch! Expected 0x%02X, got 0x%02X\n",
                   TEST_DATA, read_data);
        }
    }

    /* Continuous test loop */
    printf("\n=== Continuous Test ===\n");
    printf("Running continuous read/write test...\n");

    test_count = 0U;

    while (test_count < TEST_ITERATIONS) {
        uint8_t const test_value = (uint8_t)(test_count & 0xFFU);
        uint32_t const addr = TEST_BASE_ADDRESS + test_count;

        if (microsd_write_byte(addr, test_value)) {
            uint8_t verify_value;

            if (microsd_read_byte(addr, &verify_value)) {
                if (verify_value == test_value) {
                    printf("Test %u: SUCCESS (0x%02X)\n", test_count, test_value);
                } else {
                    printf("Test %u: FAILED - mismatch\n", test_count);
                }
            } else {
                printf("Test %u: FAILED - read error\n", test_count);
            }
        } else {
            printf("Test %u: FAILED - write error\n", test_count);
        }

        test_count++;
        sleep_ms(TEST_DELAY_MS);
    }

    printf("\nDemo completed! Ready for MQTT-SN client integration.\n");

    while (1) {
        sleep_ms(ERROR_DELAY_MS);
    }

    return 0;
}
