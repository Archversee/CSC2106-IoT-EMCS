/*!
 * @file    microsd_demo.c
 * @brief   MicroSD driver demonstration program
 * @author  INF2004 Team
 * @date    2024
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "microsd_driver.h"
#include "pico/stdlib.h"

/*! Test configuration constants */
#define TEST_BYTE_VALUE (0xABU)
#define TEST_ADDRESS_BASE (0x1000U)
#define TEST_ADDRESS_MULTI (0x2000U)
#define TEST_DATA_SIZE (5U)
#define EXPECTED_SUCCESS (0)
#define EXPECTED_FAILURE (-1)

/*! Test data array */
static uint8_t const test_data_array[TEST_DATA_SIZE] = {0x12U, 0x34U, 0x56U, 0x78U, 0xABU};

/*!
 * @brief Test single byte read/write operation
 * @return bool true if test passes, false otherwise
 */
static bool test_single_byte_operation(void)
{
    bool result = false;
    uint8_t const write_data = TEST_BYTE_VALUE;
    uint8_t read_data = 0x00U;
    uint32_t const test_address = TEST_ADDRESS_BASE;

    printf("\n=== Single Byte Test ===\n");
    printf("Test address: 0x%08lX\n", (unsigned long) test_address);

    /* First try to read what's already there */
    printf("Reading existing byte at test address...\n");
    if(microsd_read_byte(test_address, &read_data))
    {
        printf("Successfully read existing byte 0x%02X from microSD\n", read_data);
    }
    else
    {
        printf("ERROR: Read operation failed!\n");
        return false;
    }

    /* Write byte to microSD */
    printf("Writing test byte 0x%02X...\n", write_data);
    if(microsd_write_byte(test_address, write_data))
    {
        printf("Successfully wrote byte 0x%02X to microSD\n", write_data);

        /* Read byte from microSD */
        printf("Reading test byte...\n");
        if(microsd_read_byte(test_address, &read_data))
        {
            printf("Successfully read byte 0x%02X from microSD\n", read_data);

            /* Verify data integrity */
            printf("\nData verification:\n");
            printf("Written: 0x%02X\n", write_data);
            printf("Read:    0x%02X\n", read_data);

            if(write_data == read_data)
            {
                printf("SUCCESS: Single byte test passed!\n");
                result = true;
            }
            else
            {
                printf("ERROR: Data mismatch detected!\n");
            }
        }
        else
        {
            printf("ERROR: Read operation failed!\n");
        }
    }
    else
    {
        printf("ERROR: Write operation failed!\n");
    }

    return result;
}

/*!
 * @brief Test multiple byte read/write operations
 * @return bool true if test passes, false otherwise
 */
static bool test_multiple_byte_operation(void)
{
    bool result = true;
    uint32_t const base_address = TEST_ADDRESS_MULTI;

    printf("\n=== Multiple Byte Test ===\n");
    printf("Testing %u bytes at different addresses...\n", TEST_DATA_SIZE);

    for(uint32_t i = 0U; i < TEST_DATA_SIZE; i++)
    {
        uint32_t const addr = base_address + i;
        uint8_t read_val = 0x00U;

        /* Write byte */
        if(microsd_write_byte(addr, test_data_array[i]))
        {
            /* Read back and verify */
            if(microsd_read_byte(addr, &read_val))
            {
                if(test_data_array[i] == read_val)
                {
                    printf("Byte %lu: 0x%02X ✓\n", (unsigned long) i, test_data_array[i]);
                }
                else
                {
                    printf("ERROR: Mismatch at byte %lu: wrote 0x%02X, read 0x%02X\n",
                           (unsigned long) i,
                           test_data_array[i],
                           read_val);
                    result = false;
                }
            }
            else
            {
                printf("ERROR: Failed to read byte %lu\n", (unsigned long) i);
                result = false;
            }
        }
        else
        {
            printf("ERROR: Failed to write byte %lu\n", (unsigned long) i);
            result = false;
        }
    }

    if(result)
    {
        printf("SUCCESS: Multiple byte test passed!\n");
    }
    else
    {
        printf("ERROR: Multiple byte test failed!\n");
    }

    return result;
}

/*!
 * @brief Test creating a proper text file in exFAT filesystem
 * @return bool true if test passes, false otherwise
 */
static bool test_create_exfat_file(void)
{
    bool result = false;
    const char *message = "HELLO INF2004 CS31 GROUP\n";
    const char *filename = "HELLO_INF2004.txt";
    filesystem_info_t fs_info;

    printf("\n=== exFAT File Creation Test ===\n");
    printf("Creating file: %s\n", filename);
    printf("Message: %s", message);
    printf("Message length: %u bytes\n", (unsigned int) strlen(message));

    /* Initialize filesystem */
    printf("Initializing filesystem...\n");
    if(microsd_init_filesystem(&fs_info))
    {
        printf("Filesystem initialized successfully\n");

        if(fs_info.is_exfat)
        {
            printf("Confirmed exFAT filesystem\n");

            /* Create the file */
            printf("Creating file with proper exFAT directory entries...\n");
            if(microsd_create_file(&fs_info, filename, (uint8_t *) message, strlen(message)))
            {
                printf("SUCCESS: File '%s' created successfully!\n", filename);
                printf("The file should now be visible on Windows and Mac\n");
                result = true;
            }
            else
            {
                printf("ERROR: Failed to create file!\n");
            }
        }
        else
        {
            printf("ERROR: SD card is not formatted with exFAT!\n");
            printf("Please format the SD card with exFAT filesystem\n");
        }
    }
    else
    {
        printf("ERROR: Failed to initialize filesystem!\n");
    }

    return result;
}

static bool test_read_exfat_file(void)
{
    bool result = false;
    const char *filename = "HELLO_INF2004.txt";
    uint8_t read_buffer[64];
    uint32_t bytes_read = 0;
    filesystem_info_t fs_info;

    printf("\n=== exFAT File Read Test ===\n");
    printf("Reading file: %s\n", filename);

    /* Initialize filesystem */
    printf("Initializing filesystem...\n");
    if(microsd_init_filesystem(&fs_info))
    {
        printf("Filesystem initialized successfully\n");

        if(fs_info.is_exfat)
        {
            printf("Confirmed exFAT filesystem\n");

            /* Read the file */
            printf("Reading file contents...\n");
            if(microsd_read_file(&fs_info, filename, read_buffer, sizeof(read_buffer), &bytes_read))
            {
                printf(
                    "SUCCESS: Read %u bytes from file '%s'\n", (unsigned int) bytes_read, filename);
                printf("File contents:\n");
                fwrite(read_buffer, 1, bytes_read, stdout);
                printf("\n");
                result = true;
            }
            else
            {
                printf("ERROR: Failed to read file!\n");
            }
        }
        else
        {
            printf("ERROR: SD card is not formatted with exFAT!\n");
            printf("Please format the SD card with exFAT filesystem\n");
        }
    }
    else
    {
        printf("ERROR: Failed to initialize filesystem!\n");
    }

    return result;
}

/*!
 * @brief Main demonstration function
 * @return int Program exit status
 */
int main(void)
{
    bool all_tests_passed = true;
    int test_count = 1;

    (void) stdio_init_all();

    // Add a 5-second delay to allow the USB serial to enumerate
    for(int i = 0; i < 5; i++)
    {
        printf("Waiting... %d\n", 5 - i);
        sleep_ms(1000);
    }

    printf("MicroSD Driver Demo\n");
    printf("===================\n");
    printf("Running tests every 10 seconds...\n\n");

    while(true)
    {
        printf("=== Test Run #%d ===\n", test_count);
        all_tests_passed = true;

        /* Initialize microSD card */
        printf("Initializing microSD card...\n");
        if(microsd_init())
        {
            printf("MicroSD card initialized successfully\n");

            /* Set debug logging to see filesystem detection details */
            microsd_set_log_level(MICROSD_LOG_DEBUG);

            /* Print driver information banner */
            microsd_print_banner();

            /* Run test suite */
            printf("\nStarting test suite...\n");

            // /* Test 1: Single byte operation */
            // if (!test_single_byte_operation()) {
            //     all_tests_passed = false;
            // }

            // /* Test 2: Multiple byte operations */
            // if (!test_multiple_byte_operation()) {
            //     all_tests_passed = false;
            // }

            /* Test 3: exFAT file creation test */
            if(!test_create_exfat_file())
            {
                all_tests_passed = false;
            }

            /* Test 4: exFAT file read test */
            if(!test_read_exfat_file())
            {
                all_tests_passed = false;
            }

            /* Print final results */
            printf("\n=== Test Results ===\n");
            if(all_tests_passed)
            {
                printf("ALL TESTS PASSED!\n");
                printf("MicroSD driver demonstrates successful bidirectional data transfer.\n");
            }
            else
            {
                printf("SOME TESTS FAILED!\n");
                printf("Please check the microSD card connection and try again.\n");
            }
        }
        else
        {
            printf("ERROR: Failed to initialize microSD card\n");
            printf("Please check the microSD card connection and try again.\n");
        }

        printf("\n--- Waiting 10 seconds for next test run ---\n\n");
        sleep_ms(10000);
        test_count++;
    }

    return 0;
}
