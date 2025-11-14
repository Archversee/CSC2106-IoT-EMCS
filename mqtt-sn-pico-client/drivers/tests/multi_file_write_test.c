/*!
 * @file    multi_file_write_test.c
 * @brief   Automated test for multiple file write operations using FatFS drivers
 * @author  INF2004 Project Team
 * @date    6 November 2025
 *
 * Test Execution Counts:
 * - MT-1 (Sequential writes):         10 executions
 * - MT-2 (Concurrent open files):      5 executions
 * - MT-3 (Large file writes):          5 executions
 * - MT-4 (Small file writes):         20 executions
 * - MT-5 (File operations):            8 executions
 * - MT-6 (Directory operations):       5 executions
 *
 * Total: 53 test executions
 *
 * Purpose: Validate FatFS driver stability and reliability for multiple file operations
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../microsd_driver.h"
#include "ff.h"
#include "pico/stdlib.h"

/*! Test configuration */
#define TEST_SEQUENTIAL_WRITES 10
#define TEST_CONCURRENT_FILES 5
#define TEST_LARGE_FILES 5
#define TEST_SMALL_FILES 20
#define TEST_FILE_OPERATIONS 8
#define TEST_DIRECTORY_OPS 5

/*! Test file sizes */
#define TINY_FILE_SIZE 128      /* 128 bytes */
#define SMALL_FILE_SIZE 1024    /* 1KB */
#define MEDIUM_FILE_SIZE 8192   /* 8KB */
#define LARGE_FILE_SIZE 65536   /* 64KB */
#define XLARGE_FILE_SIZE 262144 /* 256KB */

/*! Test directory */
#define TEST_DIR "TEST_DIR"

/*! MicroSD initialization configuration */
#define MICROSD_INIT_MAX_RETRIES 3
#define MICROSD_INIT_RETRY_DELAY_MS 2000

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
static void print_test_header(const char *test_name, uint32_t iteration, uint32_t total) {
    if (iteration == 1 || iteration == total || iteration % 5 == 0) {
        printf("\r" COLOR_BLUE "[%s] Iteration %lu/%lu" COLOR_RESET, test_name,
               (unsigned long)iteration, (unsigned long)total);
        fflush(stdout);
    }
}

/*!
 * @brief Print test result
 */
static void print_result(bool passed, const char *message, bool newline) {
    g_results.total_tests++;

    if (passed) {
        g_results.passed++;
        printf("\r" COLOR_GREEN "✓ PASS" COLOR_RESET ": %s", message);
    } else {
        g_results.failed++;
        printf("\r" COLOR_RED "✗ FAIL" COLOR_RESET ": %s", message);
    }

    if (newline) {
        printf("\n");
    }
    fflush(stdout);
}

/*!
 * @brief Print error message
 */
static void print_error(const char *message) {
    g_results.errors++;
    printf(COLOR_RED "ERROR: %s" COLOR_RESET "\n", message);
}

/*!
 * @brief Generate test pattern data
 */
static void generate_pattern(uint8_t *buffer, size_t size, uint32_t seed) {
    for (size_t i = 0; i < size; i++) {
        buffer[i] = (uint8_t)((i * seed + 73) & 0xFF);
    }
}

/*!
 * @brief Verify test pattern data
 */
static bool verify_pattern(const uint8_t *buffer, size_t size, uint32_t seed) {
    for (size_t i = 0; i < size; i++) {
        uint8_t expected = (uint8_t)((i * seed + 73) & 0xFF);
        if (buffer[i] != expected) {
            return false;
        }
    }
    return true;
}

/*!
 * @brief Initialize MicroSD card with retries
 */
static bool init_microsd_with_retry(void) {
    printf("Initializing MicroSD card...\n");

    for (int attempt = 1; attempt <= MICROSD_INIT_MAX_RETRIES; attempt++) {
        printf("  Attempt %d/%d...\n", attempt, MICROSD_INIT_MAX_RETRIES);

        if (microsd_driver_init()) {
            printf(COLOR_GREEN "✓ MicroSD initialized successfully" COLOR_RESET "\n");
            microsd_driver_print_info();
            return true;
        }

        if (attempt < MICROSD_INIT_MAX_RETRIES) {
            printf("  Failed. Retrying in %d ms...\n", MICROSD_INIT_RETRY_DELAY_MS);
            sleep_ms(MICROSD_INIT_RETRY_DELAY_MS);
        }
    }

    print_error("Failed to initialize MicroSD card after all retries");
    return false;
}

/*!
 * @brief Clean up test files and directories
 */
static void cleanup_test_environment(void) {
    printf("\nCleaning up test environment...\n");

    uint32_t files_deleted = 0;
    uint32_t files_not_found = 0;

    // Delete test files (silently ignore files that don't exist)
    const char *test_files[] = {"SEQ_00.DAT",
                                "SEQ_01.DAT",
                                "SEQ_02.DAT",
                                "SEQ_03.DAT",
                                "SEQ_04.DAT",
                                "SEQ_05.DAT",
                                "SEQ_06.DAT",
                                "SEQ_07.DAT",
                                "SEQ_08.DAT",
                                "SEQ_09.DAT",
                                "CONC_0.TXT",
                                "CONC_1.TXT",
                                "CONC_2.TXT",
                                "CONC_3.TXT",
                                "CONC_4.TXT",
                                "LARGE_0.BIN",
                                "LARGE_1.BIN",
                                "LARGE_2.BIN",
                                "LARGE_3.BIN",
                                "LARGE_4.BIN",
                                "SMALL.TXT",
                                "RENAMED.TXT",
                                "COPY.TXT",
                                "TEST_DIR/FILE_A.TXT",
                                "TEST_DIR/FILE_B.DAT",
                                "TEST_DIR/SUBDIR/NESTED.TXT"};

    for (size_t i = 0; i < sizeof(test_files) / sizeof(test_files[0]); i++) {
        if (microsd_driver_file_exists(test_files[i])) {
            if (microsd_driver_unlink(test_files[i])) {
                files_deleted++;
            }
        } else {
            files_not_found++;
        }
    }

    // Remove directories (subdirs first) - also check existence first
    if (microsd_driver_file_exists("TEST_DIR/SUBDIR")) {
        microsd_driver_unlink("TEST_DIR/SUBDIR");
        files_deleted++;
    }
    if (microsd_driver_file_exists(TEST_DIR)) {
        microsd_driver_unlink(TEST_DIR);
        files_deleted++;
    }

    printf("✓ Cleanup complete (%lu files deleted, %lu already removed)\n",
           (unsigned long)files_deleted, (unsigned long)files_not_found);
}

/*!
 * @brief Test MT-1: Sequential file writes
 * Target: 10 executions
 */
static void test_sequential_writes(void) {
    printf("\n" COLOR_YELLOW "=== TEST MT-1: Sequential File Writes ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_SEQUENTIAL_WRITES);

    for (uint32_t i = 0; i < TEST_SEQUENTIAL_WRITES; i++) {
        print_test_header("MT-1 Sequential", i + 1, TEST_SEQUENTIAL_WRITES);

        char filename[32];
        snprintf(filename, sizeof(filename), "SEQ_%02lu.DAT", (unsigned long)i);

        // Generate test data
        uint8_t *write_buffer = (uint8_t *)malloc(SMALL_FILE_SIZE);
        if (!write_buffer) {
            print_result(false, "Failed to allocate write buffer", true);
            continue;
        }
        generate_pattern(write_buffer, SMALL_FILE_SIZE, i + 1);

        // Write file
        if (!microsd_driver_write_file(filename, write_buffer, SMALL_FILE_SIZE)) {
            free(write_buffer);
            print_result(false, "Failed to write file", true);
            continue;
        }

        // Read back and verify
        uint8_t *read_buffer = (uint8_t *)malloc(SMALL_FILE_SIZE);
        if (!read_buffer) {
            free(write_buffer);
            print_result(false, "Failed to allocate read buffer", true);
            continue;
        }

        size_t bytes_read = 0;
        if (!microsd_driver_read_file(filename, read_buffer, SMALL_FILE_SIZE, &bytes_read)) {
            free(write_buffer);
            free(read_buffer);
            print_result(false, "Failed to read file", true);
            continue;
        }

        // Verify size and content
        bool success =
            (bytes_read == SMALL_FILE_SIZE) && verify_pattern(read_buffer, SMALL_FILE_SIZE, i + 1);

        free(write_buffer);
        free(read_buffer);

        if (success) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Sequential write %lu/%lu verified", (unsigned long)(i + 1),
                     (unsigned long)TEST_SEQUENTIAL_WRITES);
            print_result(true, msg, i == TEST_SEQUENTIAL_WRITES - 1);
        } else {
            print_result(false, "Data verification failed", true);
        }
    }
    printf("\n");
}

/*!
 * @brief Test MT-2: Concurrent open file handles
 * Target: 5 executions
 */
static void test_concurrent_files(void) {
    printf("\n" COLOR_YELLOW "=== TEST MT-2: Concurrent Open Files ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_CONCURRENT_FILES);

    for (uint32_t i = 1; i <= TEST_CONCURRENT_FILES; i++) {
        print_test_header("MT-2 Concurrent", i, TEST_CONCURRENT_FILES);

        FIL files[5];
        bool all_opened = true;

        // Open multiple files simultaneously
        for (int j = 0; j < 5; j++) {
            char filename[32];
            snprintf(filename, sizeof(filename), "CONC_%d.TXT", j);

            if (!microsd_driver_open(&files[j], filename, FA_WRITE | FA_CREATE_ALWAYS)) {
                all_opened = false;
                // Close any already opened files
                for (int k = 0; k < j; k++) {
                    microsd_driver_close(&files[k]);
                }
                break;
            }
        }

        if (!all_opened) {
            print_result(false, "Failed to open all files concurrently", true);
            continue;
        }

        // Write to each file
        bool all_written = true;
        for (int j = 0; j < 5; j++) {
            char content[128];
            snprintf(content, sizeof(content), "Concurrent file %d, iteration %lu\n", j,
                     (unsigned long)i);

            UINT bytes_written = 0;
            if (!microsd_driver_write(&files[j], content, strlen(content), &bytes_written)) {
                all_written = false;
                break;
            }
        }

        // Close all files
        for (int j = 0; j < 5; j++) {
            microsd_driver_close(&files[j]);
        }

        if (all_written) {
            print_result(true, "5 concurrent files written successfully",
                         i == TEST_CONCURRENT_FILES);
        } else {
            print_result(false, "Failed to write to concurrent files", true);
        }
    }
    printf("\n");
}

/*!
 * @brief Test MT-3: Large file writes
 * Target: 5 executions
 */
static void test_large_file_writes(void) {
    printf("\n" COLOR_YELLOW "=== TEST MT-3: Large File Writes ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_LARGE_FILES);

    for (uint32_t i = 0; i < TEST_LARGE_FILES; i++) {
        print_test_header("MT-3 Large File", i + 1, TEST_LARGE_FILES);

        char filename[32];
        snprintf(filename, sizeof(filename), "LARGE_%lu.BIN", (unsigned long)i);

        // Allocate buffer for large file
        uint8_t *buffer = (uint8_t *)malloc(LARGE_FILE_SIZE);
        if (!buffer) {
            print_result(false, "Failed to allocate large buffer", true);
            continue;
        }

        // Generate pattern
        generate_pattern(buffer, LARGE_FILE_SIZE, (i + 1) * 7);

        // Write large file
        FIL file;
        if (!microsd_driver_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS)) {
            free(buffer);
            print_result(false, "Failed to open large file for writing", true);
            continue;
        }

        UINT bytes_written = 0;
        bool write_success = microsd_driver_write(&file, buffer, LARGE_FILE_SIZE, &bytes_written);
        microsd_driver_close(&file);

        if (!write_success || bytes_written != LARGE_FILE_SIZE) {
            free(buffer);
            print_result(false, "Large file write incomplete", true);
            continue;
        }

        // Verify file size
        FILINFO finfo;
        if (!microsd_driver_stat(filename, &finfo) || finfo.fsize != LARGE_FILE_SIZE) {
            free(buffer);
            print_result(false, "Large file size mismatch", true);
            continue;
        }

        free(buffer);

        char msg[128];
        snprintf(msg, sizeof(msg), "Large file %lu written (%lu bytes)", (unsigned long)(i + 1),
                 (unsigned long)LARGE_FILE_SIZE);
        print_result(true, msg, i == TEST_LARGE_FILES - 1);
    }
    printf("\n");
}

/*!
 * @brief Test MT-4: Many small file writes (stress test)
 * Target: 20 executions
 */
static void test_small_file_writes(void) {
    printf("\n" COLOR_YELLOW "=== TEST MT-4: Small File Writes (Stress) ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_SMALL_FILES);

    uint32_t success_count = 0;

    for (uint32_t i = 0; i < TEST_SMALL_FILES; i++) {
        print_test_header("MT-4 Small Files", i + 1, TEST_SMALL_FILES);

        char filename[32];
        snprintf(filename, sizeof(filename), "SMALL_%02lu.TXT", (unsigned long)i);

        char content[TINY_FILE_SIZE];
        snprintf(content, sizeof(content),
                 "Small file test #%lu - Quick brown fox jumps over the lazy dog",
                 (unsigned long)i);

        if (microsd_driver_write_file(filename, content, strlen(content))) {
            success_count++;

            // Clean up immediately
            microsd_driver_unlink(filename);
        }

        if ((i + 1) % 5 == 0 || i == TEST_SMALL_FILES - 1) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Small files: %lu/%lu successful",
                     (unsigned long)success_count, (unsigned long)(i + 1));
            print_result(success_count == (i + 1), msg, true);
        }
    }
    printf("\n");
}

/*!
 * @brief Test MT-5: File operations (rename, delete, stat)
 * Target: 8 executions
 */
static void test_file_operations(void) {
    printf("\n" COLOR_YELLOW "=== TEST MT-5: File Operations ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_FILE_OPERATIONS);

    for (uint32_t i = 1; i <= TEST_FILE_OPERATIONS; i++) {
        print_test_header("MT-5 Operations", i, TEST_FILE_OPERATIONS);

        const char *original = "SMALL.TXT";
        const char *renamed = "RENAMED.TXT";
        const char *content = "Test file for operations";

        // Create file
        if (!microsd_driver_write_file(original, content, strlen(content))) {
            print_result(false, "Failed to create test file", true);
            continue;
        }

        // Check file exists
        if (!microsd_driver_file_exists(original)) {
            print_result(false, "File existence check failed", true);
            microsd_driver_unlink(original);
            continue;
        }

        // Get file info
        FILINFO finfo;
        if (!microsd_driver_stat(original, &finfo)) {
            print_result(false, "Failed to get file info", true);
            microsd_driver_unlink(original);
            continue;
        }

        // Rename file
        if (!microsd_driver_rename(original, renamed)) {
            print_result(false, "Failed to rename file", true);
            microsd_driver_unlink(original);
            continue;
        }

        // Verify renamed file exists
        if (!microsd_driver_file_exists(renamed)) {
            print_result(false, "Renamed file not found", true);
            microsd_driver_unlink(renamed);
            continue;
        }

        // Delete renamed file
        if (!microsd_driver_unlink(renamed)) {
            print_result(false, "Failed to delete file", true);
            continue;
        }

        // Verify deletion
        if (microsd_driver_file_exists(renamed)) {
            print_result(false, "File still exists after deletion", true);
            continue;
        }

        print_result(true, "File operations successful", i == TEST_FILE_OPERATIONS);
    }
    printf("\n");
}

/*!
 * @brief Test MT-6: Directory operations
 * Target: 5 executions
 */
static void test_directory_operations(void) {
    printf("\n" COLOR_YELLOW "=== TEST MT-6: Directory Operations ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_DIRECTORY_OPS);

    for (uint32_t i = 1; i <= TEST_DIRECTORY_OPS; i++) {
        print_test_header("MT-6 Directories", i, TEST_DIRECTORY_OPS);

        // Create directory
        if (!microsd_driver_mkdir(TEST_DIR)) {
            // Directory might already exist, try to continue
            // print_result(false, "Failed to create directory", true);
            // continue;
        }

        // Create file in directory
        char filepath[64];
        snprintf(filepath, sizeof(filepath), "%s/FILE_A.TXT", TEST_DIR);

        const char *content = "File inside directory";
        if (!microsd_driver_write_file(filepath, content, strlen(content))) {
            print_result(false, "Failed to create file in directory", true);
            continue;
        }

        // Create subdirectory
        char subdir[64];
        snprintf(subdir, sizeof(subdir), "%s/SUBDIR", TEST_DIR);
        if (!microsd_driver_mkdir(subdir)) {
            // May already exist
        }

        // Create file in subdirectory
        char nested_file[96];
        snprintf(nested_file, sizeof(nested_file), "%s/NESTED.TXT", subdir);
        if (!microsd_driver_write_file(nested_file, "Nested file", 11)) {
            print_result(false, "Failed to create nested file", true);
            continue;
        }

        // List directory
        DIR dir;
        FILINFO finfo;
        if (!microsd_driver_opendir(&dir, TEST_DIR)) {
            print_result(false, "Failed to open directory for listing", true);
            continue;
        }

        int file_count = 0;
        while (microsd_driver_readdir(&dir, &finfo)) {
            if (finfo.fname[0] != 0) {
                file_count++;
            } else {
                break;
            }
        }
        microsd_driver_closedir(&dir);

        if (file_count >= 2) { // Should have at least FILE_A.TXT and SUBDIR
            char msg[128];
            snprintf(msg, sizeof(msg), "Directory operations successful (%d entries)", file_count);
            print_result(true, msg, i == TEST_DIRECTORY_OPS);
        } else {
            print_result(false, "Directory listing incomplete", true);
        }

        // Cleanup
        microsd_driver_unlink(nested_file);
        microsd_driver_unlink(subdir);
        microsd_driver_unlink(filepath);
    }
    printf("\n");
}

/*!
 * @brief Print final test summary
 */
static void print_summary(void) {
    printf("\n");
    printf("================================================================================\n");
    printf(COLOR_YELLOW "            MULTI-FILE WRITE TEST SUMMARY" COLOR_RESET "\n");
    printf("================================================================================\n");
    printf("\n");

    printf("Total test executions: %lu\n", (unsigned long)g_results.total_tests);
    printf(COLOR_GREEN "Passed:  %lu" COLOR_RESET " (%.1f%%)\n", (unsigned long)g_results.passed,
           g_results.total_tests > 0 ? (g_results.passed * 100.0 / g_results.total_tests) : 0.0);
    printf(COLOR_RED "Failed:  %lu" COLOR_RESET " (%.1f%%)\n", (unsigned long)g_results.failed,
           g_results.total_tests > 0 ? (g_results.failed * 100.0 / g_results.total_tests) : 0.0);
    printf(COLOR_YELLOW "Errors:  %lu" COLOR_RESET "\n", (unsigned long)g_results.errors);

    printf("\n");
    printf("Test Breakdown:\n");
    printf("  MT-1 (Sequential writes):      %d executions\n", TEST_SEQUENTIAL_WRITES);
    printf("  MT-2 (Concurrent files):       %d executions\n", TEST_CONCURRENT_FILES);
    printf("  MT-3 (Large file writes):      %d executions\n", TEST_LARGE_FILES);
    printf("  MT-4 (Small file writes):      %d executions\n", TEST_SMALL_FILES);
    printf("  MT-5 (File operations):        %d executions\n", TEST_FILE_OPERATIONS);
    printf("  MT-6 (Directory operations):   %d executions\n", TEST_DIRECTORY_OPS);
    printf("  ─────────────────────────────────────\n");
    printf("  Total:                         %d executions\n",
           TEST_SEQUENTIAL_WRITES + TEST_CONCURRENT_FILES + TEST_LARGE_FILES + TEST_SMALL_FILES +
               TEST_FILE_OPERATIONS + TEST_DIRECTORY_OPS);

    printf("\n");
    printf("Driver Statistics:\n");
    uint32_t total_reads = 0, total_writes = 0, last_error = 0;
    microsd_driver_get_stats(&total_reads, &total_writes, &last_error);
    printf("  Total reads:  %lu\n", (unsigned long)total_reads);
    printf("  Total writes: %lu\n", (unsigned long)total_writes);
    printf("  Last error:   %lu (%s)\n", (unsigned long)last_error,
           microsd_driver_get_error_string((FRESULT)last_error));

    printf("\n");
    printf("================================================================================\n");

    if (g_results.failed == 0 && g_results.errors == 0) {
        printf(COLOR_GREEN "ALL TESTS PASSED! ✓" COLOR_RESET "\n");
    } else {
        printf(COLOR_RED "SOME TESTS FAILED! ✗" COLOR_RESET "\n");
    }
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
    printf("          MULTI-FILE WRITE TEST - FatFS Driver Validation\n");
    printf("================================================================================\n");
    printf("Test Suite: Multiple File Write Operations\n");
    printf("Purpose: Validate FatFS driver reliability and performance\n");
    printf("Date: 6 November 2025\n");
    printf("================================================================================\n");

    // Initialize MicroSD
    if (!init_microsd_with_retry()) {
        printf("\n" COLOR_RED "Cannot proceed without MicroSD card." COLOR_RESET "\n");
        while (true) {
            sleep_ms(1000);
        }
    }

    printf("\n" COLOR_GREEN "MicroSD ready! Starting tests..." COLOR_RESET "\n");

    // Run all test suites
    test_sequential_writes();
    test_concurrent_files();
    test_large_file_writes();
    test_small_file_writes();
    test_file_operations();
    test_directory_operations();

    // Print summary
    print_summary();

    // Cleanup
    cleanup_test_environment();

    // Keep running for observation
    printf("\nTests complete. System will remain active for monitoring.\n");
    printf("Press Ctrl+C to exit.\n");

    while (true) {
        sleep_ms(1000);
    }

    return 0;
}
