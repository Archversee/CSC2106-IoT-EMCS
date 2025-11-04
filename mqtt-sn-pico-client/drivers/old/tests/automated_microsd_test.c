/*!
 * @file    automated_microsd_test.c
 * @brief   Automated unit tests for MicroSD driver with error recovery
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 *
 * Test Execution Counts (from traceability matrix):
 * - IT-2 (Initialize):      20 executions
 * - UT-5 (Block I/O):       50 executions
 * - UT-5 (Byte I/O RMW):    30 executions
 * - IT-2 (File operations): 25 executions
 *
 * Total: 125 test executions
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../microsd_driver.h"
#include "pico/stdlib.h"

/*! Test configuration */
#define TEST_INIT_EXECUTIONS 20
#define TEST_BLOCK_IO_EXECUTIONS 50
#define TEST_BYTE_IO_EXECUTIONS 30
#define TEST_FILE_OPS_EXECUTIONS 25

#define INIT_TIMEOUT_MS 10000
#define MAX_INIT_RETRIES 3
#define RECOVERY_DELAY_MS 2000

/*! Test result tracking */
typedef struct {
    uint32_t total_tests;
    uint32_t passed;
    uint32_t failed;
    uint32_t errors;
} test_results_t;

static test_results_t g_results = {0};

/*! Color codes for output */
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_RESET "\033[0m"

/*!
 * @brief Print test header
 */
static void print_test_header(const char* test_name, uint32_t iteration, uint32_t total) {
    printf("\n" COLOR_BLUE "[%s] Iteration %lu/%lu" COLOR_RESET "\n",
           test_name, (unsigned long)iteration, (unsigned long)total);
}

/*!
 * @brief Print test result
 */
static void print_result(bool passed, const char* message) {
    if (passed) {
        printf(COLOR_GREEN "✓ PASS" COLOR_RESET ": %s\n", message);
        g_results.passed++;
    } else {
        printf(COLOR_RED "✗ FAIL" COLOR_RESET ": %s\n", message);
        g_results.failed++;
    }
    g_results.total_tests++;
}

/*!
 * @brief Print error message
 */
static void print_error(const char* message) {
    printf(COLOR_RED "ERROR" COLOR_RESET ": %s\n", message);
    g_results.errors++;
}

/*!
 * @brief Initialize MicroSD with timeout and retry recovery
 *
 * @return true if initialization succeeded, false if all retries exhausted
 */
static bool init_microsd_with_recovery(void) {
    uint32_t attempt = 0;

    while (attempt < MAX_INIT_RETRIES) {
        attempt++;
        printf("  Initialization attempt %lu/%u...\n",
               (unsigned long)attempt, MAX_INIT_RETRIES);

        absolute_time_t start = get_absolute_time();
        bool init_success = microsd_init();
        int64_t elapsed_ms = absolute_time_diff_us(start, get_absolute_time()) / 1000;

        if (init_success) {
            printf("  ✓ Initialized in %lld ms\n", elapsed_ms);
            return true;
        }

        // Check if timeout occurred
        if (elapsed_ms >= INIT_TIMEOUT_MS) {
            print_error("Initialization timeout");
        } else {
            print_error("Initialization failed");
        }

        // Recovery: wait and retry
        if (attempt < MAX_INIT_RETRIES) {
            printf("  Waiting %d ms before retry...\n", RECOVERY_DELAY_MS);
            sleep_ms(RECOVERY_DELAY_MS);
        }
    }

    print_error("All initialization attempts failed");
    return false;
}

/*!
 * @brief Test IT-2: MicroSD Initialization
 * Target: 20 executions
 */
static void test_microsd_init(void) {
    printf("\n" COLOR_YELLOW "=== TEST IT-2: MicroSD Initialization ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_INIT_EXECUTIONS);

    for (uint32_t i = 1; i <= TEST_INIT_EXECUTIONS; i++) {
        print_test_header("IT-2 Init", i, TEST_INIT_EXECUTIONS);

        // Initialize with recovery
        bool init_ok = init_microsd_with_recovery();
        if (!init_ok) {
            print_result(false, "MicroSD initialization failed after retries");
            continue;
        }

        // Initialize filesystem
        static filesystem_info_t fs_info;
        bool fs_ok = microsd_init_filesystem(&fs_info);

        if (!fs_ok) {
            print_result(false, "Filesystem initialization failed");
            continue;
        }

        // Verify filesystem is exFAT
        if (!fs_info.is_exfat) {
            print_result(false, "SD card is not formatted as exFAT");
            continue;
        }

        // Verify filesystem parameters are reasonable
        bool params_ok = (fs_info.bytes_per_sector == 512 ||
                          fs_info.bytes_per_sector == 1024 ||
                          fs_info.bytes_per_sector == 2048 ||
                          fs_info.bytes_per_sector == 4096) &&
                         (fs_info.root_cluster > 0);

        if (!params_ok) {
            char msg[128];
            snprintf(msg, sizeof(msg),
                     "Invalid FS parameters: sector=%lu, root_cluster=%lu",
                     (unsigned long)fs_info.bytes_per_sector,
                     (unsigned long)fs_info.root_cluster);
            print_result(false, msg);
            continue;
        }

        print_result(true, "MicroSD and filesystem initialized successfully");

        // Small delay between tests
        sleep_ms(100);
    }
}

/*!
 * @brief Test UT-5: Block I/O
 * Target: 50 executions
 */
static void test_block_io(void) {
    printf("\n" COLOR_YELLOW "=== TEST UT-5: Block I/O ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_BLOCK_IO_EXECUTIONS);

    // Initialize once for all block I/O tests
    if (!init_microsd_with_recovery()) {
        print_error("Cannot perform block I/O tests without initialized SD card");
        return;
    }

    // Test buffers
    static uint8_t write_buffer[512];
    static uint8_t read_buffer[512];

    for (uint32_t i = 1; i <= TEST_BLOCK_IO_EXECUTIONS; i++) {
        print_test_header("UT-5 Block I/O", i, TEST_BLOCK_IO_EXECUTIONS);

        // Use different test block for each iteration (avoid wearing same block)
        uint32_t test_block = 1000 + i;

        // Fill write buffer with test pattern
        for (size_t j = 0; j < 512; j++) {
            write_buffer[j] = (uint8_t)((i + j) & 0xFF);
        }

        // Write block
        if (!microsd_write_block(test_block, write_buffer)) {
            print_result(false, "Block write failed");
            continue;
        }

        // Clear read buffer
        memset(read_buffer, 0, sizeof(read_buffer));

        // Read block
        if (!microsd_read_block(test_block, read_buffer)) {
            print_result(false, "Block read failed");
            continue;
        }

        // Verify data
        bool data_match = (memcmp(write_buffer, read_buffer, 512) == 0);

        if (!data_match) {
            // Find first mismatch for debugging
            for (size_t j = 0; j < 512; j++) {
                if (write_buffer[j] != read_buffer[j]) {
                    char msg[128];
                    snprintf(msg, sizeof(msg),
                             "Data mismatch at offset %zu: wrote 0x%02X, read 0x%02X",
                             j, write_buffer[j], read_buffer[j]);
                    print_result(false, msg);
                    break;
                }
            }
            continue;
        }

        char msg[64];
        snprintf(msg, sizeof(msg), "Block %lu read/write verified",
                 (unsigned long)test_block);
        print_result(true, msg);

        // Small delay between tests
        sleep_ms(10);
    }
}

/*!
 * @brief Test UT-5: Byte I/O (Read-Modify-Write)
 * Target: 30 executions
 */
static void test_byte_io(void) {
    printf("\n" COLOR_YELLOW "=== TEST UT-5: Byte I/O (RMW) ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_BYTE_IO_EXECUTIONS);

    for (uint32_t i = 1; i <= TEST_BYTE_IO_EXECUTIONS; i++) {
        print_test_header("UT-5 Byte I/O", i, TEST_BYTE_IO_EXECUTIONS);

        // Use different test address for each iteration
        uint32_t test_address = 512000 + (i * 10);
        uint8_t test_value = (uint8_t)(0xA0 + (i & 0x0F));

        // Write byte
        if (!microsd_write_byte(test_address, test_value)) {
            print_result(false, "Byte write failed");
            continue;
        }

        // Read byte
        uint8_t read_value = 0;
        if (!microsd_read_byte(test_address, &read_value)) {
            print_result(false, "Byte read failed");
            continue;
        }

        // Verify data
        if (read_value != test_value) {
            char msg[128];
            snprintf(msg, sizeof(msg),
                     "Byte mismatch at 0x%08lX: wrote 0x%02X, read 0x%02X",
                     (unsigned long)test_address, test_value, read_value);
            print_result(false, msg);
            continue;
        }

        char msg[64];
        snprintf(msg, sizeof(msg), "Byte 0x%02X at 0x%08lX verified",
                 test_value, (unsigned long)test_address);
        print_result(true, msg);

        // Small delay between tests
        sleep_ms(10);
    }
}

/*!
 * @brief Test IT-2: File Operations
 * Target: 25 executions
 */
static void test_file_operations(void) {
    printf("\n" COLOR_YELLOW "=== TEST IT-2: File Operations ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_FILE_OPS_EXECUTIONS);

    // Initialize filesystem once
    static filesystem_info_t fs_info;
    if (!microsd_init_filesystem(&fs_info)) {
        print_error("Cannot perform file operations without initialized filesystem");
        return;
    }

    for (uint32_t i = 1; i <= TEST_FILE_OPS_EXECUTIONS; i++) {
        print_test_header("IT-2 File Ops", i, TEST_FILE_OPS_EXECUTIONS);

        // Create unique test file name
        char filename[32];
        snprintf(filename, sizeof(filename), "TEST_%03lu.TXT", (unsigned long)i);

        // Create test data
        char test_data[256];
        snprintf(test_data, sizeof(test_data),
                 "Automated test file #%lu\n"
                 "This file was created by automated_microsd_test.c\n"
                 "Timestamp: %lu\n"
                 "Test pattern: ",
                 (unsigned long)i, (unsigned long)to_ms_since_boot(get_absolute_time()));

        // Add some binary test pattern
        size_t text_len = strlen(test_data);
        for (size_t j = 0; j < 50 && (text_len + j) < 255; j++) {
            test_data[text_len + j] = (char)(j & 0xFF);
        }
        size_t total_len = text_len + 50;

        // Create file
        if (!microsd_create_file(&fs_info, filename, (uint8_t*)test_data, total_len)) {
            print_result(false, "File creation failed");
            continue;
        }

        // Read file back
        static uint8_t read_buffer[512];
        uint32_t bytes_read = 0;
        if (!microsd_read_file(&fs_info, filename, read_buffer, sizeof(read_buffer), &bytes_read)) {
            print_result(false, "File read failed");
            continue;
        }

        // Verify file size
        if (bytes_read != total_len) {
            char msg[128];
            snprintf(msg, sizeof(msg),
                     "File size mismatch: wrote %zu bytes, read %lu bytes",
                     total_len, (unsigned long)bytes_read);
            print_result(false, msg);
            continue;
        }

        // Verify file content
        bool content_match = (memcmp(test_data, read_buffer, total_len) == 0);
        if (!content_match) {
            print_result(false, "File content mismatch");
            continue;
        }

        char msg[64];
        snprintf(msg, sizeof(msg), "File %s created and verified (%zu bytes)",
                 filename, total_len);
        print_result(true, msg);

        // Small delay between tests
        sleep_ms(50);
    }
}

/*!
 * @brief Print final test summary
 */
static void print_summary(void) {
    printf("\n");
    printf("================================================================================\n");
    printf(COLOR_YELLOW "                    AUTOMATED MICROSD TEST SUMMARY" COLOR_RESET "\n");
    printf("================================================================================\n");
    printf("\n");

    printf("Total test executions: %lu\n", (unsigned long)g_results.total_tests);
    printf(COLOR_GREEN "Passed:  %lu" COLOR_RESET " (%.1f%%)\n",
           (unsigned long)g_results.passed,
           g_results.total_tests > 0 ? (g_results.passed * 100.0 / g_results.total_tests) : 0.0);
    printf(COLOR_RED "Failed:  %lu" COLOR_RESET " (%.1f%%)\n",
           (unsigned long)g_results.failed,
           g_results.total_tests > 0 ? (g_results.failed * 100.0 / g_results.total_tests) : 0.0);
    printf(COLOR_YELLOW "Errors:  %lu" COLOR_RESET "\n", (unsigned long)g_results.errors);

    printf("\n");
    printf("Test Breakdown:\n");
    printf("  IT-2 (Initialize):      %d executions\n", TEST_INIT_EXECUTIONS);
    printf("  UT-5 (Block I/O):       %d executions\n", TEST_BLOCK_IO_EXECUTIONS);
    printf("  UT-5 (Byte I/O RMW):    %d executions\n", TEST_BYTE_IO_EXECUTIONS);
    printf("  IT-2 (File operations): %d executions\n", TEST_FILE_OPS_EXECUTIONS);
    printf("  ────────────────────────────────────\n");
    printf("  Total:                  %d executions\n",
           TEST_INIT_EXECUTIONS + TEST_BLOCK_IO_EXECUTIONS +
               TEST_BYTE_IO_EXECUTIONS + TEST_FILE_OPS_EXECUTIONS);

    printf("\n");
    if (g_results.failed == 0 && g_results.errors == 0) {
        printf(COLOR_GREEN "✓ ALL TESTS PASSED!" COLOR_RESET "\n");
    } else {
        printf(COLOR_RED "✗ SOME TESTS FAILED" COLOR_RESET "\n");
        printf("Review the output above for details.\n");
    }
    printf("\n");
    printf("================================================================================\n");
}

/*!
 * @brief Main test entry point
 */
int main(void) {
    stdio_init_all();

    // Wait for USB serial connection
    sleep_ms(6000);

    printf("STARTING TESTS IN 3 SECONDS\n");
    sleep_ms(3000);

    printf("\n\n");
    printf("================================================================================\n");
    printf("              AUTOMATED MICROSD DRIVER UNIT TESTS\n");
    printf("================================================================================\n");
    printf("Test Suite: MicroSD Driver Verification\n");
    printf("Based on: Core Traceability Matrix Requirements\n");
    printf("Date: 3 November 2025\n");
    printf("================================================================================\n");

    // Run all test suites
    test_microsd_init();
    test_block_io();
    test_byte_io();
    test_file_operations();

    // Print summary
    print_summary();

    // Keep running for observation
    printf("\nTests complete. System will remain active for monitoring.\n");
    printf("Press Ctrl+C to exit.\n");

    while (true) {
        sleep_ms(1000);
    }

    return 0;
}
