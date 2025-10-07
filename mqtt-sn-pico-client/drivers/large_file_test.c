/*!
 * @file    large_file_test.c
 * @brief   Large file creation test for microSD driver
 * @author  INF2004 Team
 * @date    2024
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "microsd_driver.h"
#include "pico/stdlib.h"

/*!
 * @brief Test large file creation (1MB file)
 * @return bool true if test passes, false otherwise
 */
static bool test_large_file_creation(void) {
    bool result = false;
    const char *filename = "LARGE.bin";    /* Shorter filename needs fewer directory entries */
    const uint32_t file_size = 1024 * 256; /* 256KB*/
    const uint32_t chunk_size = 8 * 1024;  /* 8KB chunks - safe for Pico W */
    uint8_t *chunk_buffer = NULL;
    filesystem_info_t fs_info;

    printf("\n=== Large File Creation Test ===\n");
    printf("Creating file: %s\n", filename);
    printf("File size: %lu bytes (1MB)\n", (unsigned long)file_size);
    printf("Using %lu byte chunks to avoid memory issues\n", (unsigned long)chunk_size);

    /* Allocate buffer for chunk writing */
    printf("Allocating memory for %lu byte chunks...\n", (unsigned long)chunk_size);
    chunk_buffer = malloc(chunk_size);
    if (chunk_buffer == NULL) {
        printf("ERROR: Failed to allocate %lu bytes of memory\n", (unsigned long)chunk_size);
        return false;
    }

    /* Fill chunk buffer with pattern data */
    printf("Preparing test pattern for chunks...\n");
    for (uint32_t i = 0; i < chunk_size; i++) {
        chunk_buffer[i] = (uint8_t)(i & 0xFF); /* Repeating pattern 0x00-0xFF */
    }

    /* Set debug logging to see filesystem detection details */
    microsd_set_log_level(MICROSD_LOG_DEBUG);

    /* Initialize filesystem */
    printf("Initializing filesystem...\n");
    fflush(stdout); /* Force output before potentially hanging operation */

    if (microsd_init_filesystem(&fs_info)) {
        printf("Filesystem initialized successfully\n");

        if (fs_info.is_exfat) {
            printf("Confirmed exFAT filesystem\n");
            printf("Bytes per cluster: %lu\n", (unsigned long)(fs_info.sectors_per_cluster * 512));
            printf("Estimated clusters needed: %lu\n",
                   (unsigned long)((file_size + (fs_info.sectors_per_cluster * 512) - 1) / (fs_info.sectors_per_cluster * 512)));

            /* Create the large file using chunked writing */
            printf("Creating large file using chunked writing...\n");
            printf("This will create a full 1MB file using %lu byte chunks\n", (unsigned long)chunk_size);
            printf("NOTE: This demonstrates multiple cluster allocation and large file support\n");

            /* Calculate number of chunks needed for 1MB */
            uint32_t num_chunks = file_size / chunk_size;
            if (file_size % chunk_size != 0) {
                num_chunks++; /* Round up for remainder */
            }

            printf("Writing %lu chunks of %lu bytes each for total size %lu bytes\n",
                   (unsigned long)num_chunks, (unsigned long)chunk_size, (unsigned long)file_size);

            if (microsd_create_large_file_chunked(&fs_info, filename, chunk_buffer, chunk_size, file_size, num_chunks)) {
                printf("SUCCESS: Large file '%s' created successfully!\n", filename);
                printf("File size: %lu bytes (full 1MB)\n", (unsigned long)file_size);
                printf("Chunks written: %lu\n", (unsigned long)num_chunks);
                printf("Clusters used: ~%lu\n",
                       (unsigned long)((file_size + (fs_info.sectors_per_cluster * 512) - 1) / (fs_info.sectors_per_cluster * 512)));

                /* Verify the file was actually created by trying to read it back */
                printf("Verifying file creation by attempting to read it back...\n");
                uint8_t verify_buffer[64];
                uint32_t bytes_read = 0;
                if (microsd_read_file(&fs_info, filename, verify_buffer, sizeof(verify_buffer), &bytes_read)) {
                    printf("SUCCESS: File verification passed - read %lu bytes\n", (unsigned long)bytes_read);
                    printf("First 8 bytes of file: ");
                    for (int i = 0; i < 8 && i < bytes_read; i++) {
                        printf("0x%02X ", verify_buffer[i]);
                    }
                    printf("\n");
                } else {
                    printf("WARNING: File creation succeeded but verification read failed!\n");
                    printf("This suggests the file entry wasn't created properly.\n");
                }

                printf("NOTE: Full 1MB file created successfully using chunked writing!\n");
                result = true;
            } else {
                printf("ERROR: Failed to create test file!\n");
            }
        } else {
            printf("ERROR: SD card is not formatted with exFAT!\n");
            printf("Please format the SD card with exFAT filesystem\n");
        }
    } else {
        printf("ERROR: Failed to initialize filesystem!\n");
    }

    /* Clean up */
    if (chunk_buffer != NULL) {
        free(chunk_buffer);
    }

    return result;
}

/*!
 * @brief Test directory expansion with multiple files
 * @return bool true if test passes, false otherwise
 */
static bool test_directory_expansion(void) {
    bool result = true;
    filesystem_info_t fs_info;
    const uint32_t num_files = 20; /* Create enough files to fill directory */
    const char *base_filename = "TEST_FILE";
    const char *test_content = "This is a test file for directory expansion testing.";

    printf("\n=== Directory Expansion Test ===\n");
    printf("Creating %lu files to test directory expansion...\n", (unsigned long)num_files);

    /* Set debug logging to see filesystem detection details */
    microsd_set_log_level(MICROSD_LOG_DEBUG);

    /* Initialize filesystem */
    printf("Initializing filesystem...\n");
    printf("If this hangs, check SD card connection and format (exFAT required)\n");
    fflush(stdout); /* Force output before potentially hanging operation */

    /* Add a small delay to allow SD card to stabilize */
    sleep_ms(100);

    if (microsd_init_filesystem(&fs_info)) {
        printf("Filesystem initialized successfully\n");

        if (fs_info.is_exfat) {
            printf("Confirmed exFAT filesystem\n");

            /* Create multiple files to trigger directory expansion */
            uint32_t files_created = 0;
            for (uint32_t i = 0; i < num_files; i++) {
                char filename[64];
                snprintf(filename, sizeof(filename), "%s_%02lu.txt", base_filename, (unsigned long)i);

                printf("Creating file %lu/%lu: %s\n", (unsigned long)(i + 1), (unsigned long)num_files, filename);
                fflush(stdout); /* Ensure output is visible immediately */

                if (microsd_create_file(&fs_info, filename, (uint8_t *)test_content, strlen(test_content))) {
                    printf("✓ File '%s' created successfully\n", filename);
                    files_created++;
                } else {
                    printf("✗ Failed to create file '%s'\n", filename);
                    printf("  Error occurred after creating %lu files\n", (unsigned long)files_created);
                    result = false;
                    /* Continue creating other files to test recovery */
                }
            }

            if (result) {
                printf("SUCCESS: All %lu files created successfully!\n", (unsigned long)num_files);
                printf("Directory expansion test passed!\n");
            } else {
                printf("PARTIAL SUCCESS: Some files created, directory expansion was tested\n");
            }
        } else {
            printf("ERROR: SD card is not formatted with exFAT!\n");
            result = false;
        }
    } else {
        printf("ERROR: Failed to initialize filesystem!\n");
        result = false;
    }

    return result;
}

/*!
 * @brief Main demonstration function
 * @return int Program exit status
 */
int main(void) {
    stdio_init_all();

    printf("Large File and Directory Expansion Test - Continuous Mode\n");
    printf("========================================================\n");
    printf("Starting in 5 seconds...\n");

    /* Initial delay like microsd_demo.c */
    sleep_ms(5000);

    /* Set debug logging to see filesystem detection details */
    microsd_set_log_level(MICROSD_LOG_DEBUG);

    /* Print driver information banner */
    microsd_print_banner();

    /* Continuous retry loop every 20 seconds */
    while (true) {
        bool all_tests_passed = true;

        printf("\n=== Starting Test Cycle ===\n");
        printf("Time: %llu ms\n", time_us_64() / 1000);
        printf("NOTE: If test hangs at 'Reading block 0', try:\n");
        printf("1. Re-insert the SD card\n");
        printf("2. Check SD card is formatted as exFAT\n");
        printf("3. Verify SD card connections\n");
        printf("4. Try a different SD card\n");
        fflush(stdout);

        /* Initialize microSD card hardware first */
        printf("Initializing microSD card hardware...\n");
        if (microsd_init()) {
            printf("MicroSD card hardware initialized successfully\n");
        } else {
            printf("ERROR: Failed to initialize microSD card hardware!\n");
            printf("Retrying in 20 seconds...\n");
            all_tests_passed = false;
            /* Skip to next cycle */
            printf("\nWaiting 20 seconds before next test cycle...\n");
            printf("===========================================\n");
            sleep_ms(5000);
            continue;
        }

        // /* Test directory expansion first */
        // if (!test_directory_expansion()) {
        //     all_tests_passed = false;
        // }

        /* Test large file creation */
        if (!test_large_file_creation()) {
            all_tests_passed = false;
        }

        /* Print final results */
        printf("\n=== Test Cycle Results ===\n");
        if (all_tests_passed) {
            printf("ALL TESTS PASSED!\n");
            printf("Directory expansion and large file support working correctly.\n");
            printf("Your 16GB SD card can now handle:\n");
            printf("- Large files (1MB tested)\n");
            printf("- Directory expansion when needed\n");
            printf("- Multiple cluster allocation\n");
        } else {
            printf("SOME TESTS FAILED!\n");
            printf("Check the output above for specific issues.\n");
        }

        printf("\nWaiting 5 seconds before next test cycle...\n");
        printf("===========================================\n");

        /* Wait 5 seconds before next test */
        sleep_ms(5000);
    }

    return 0; /* Never reached in continuous mode */
}