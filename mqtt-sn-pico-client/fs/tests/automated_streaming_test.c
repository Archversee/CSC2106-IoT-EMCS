/*!
 * @file    automated_streaming_test.c
 * @brief   Automated unit tests for streaming file read functionality
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 *
 * Test Execution Counts:
 * - ST-1 (Init streaming):       15 executions
 * - ST-2 (Stream single chunk):  10 executions
 * - ST-3 (Stream full file):     10 executions
 * - ST-4 (Stream large file):     5 executions
 * - ST-6 (Stream XLARGE file):    3 executions
 * - ST-7 (Stream XXLARGE file):   2 executions
 * - ST-5 (Cleanup):              12 executions
 *
 * Total: 57 test executions
 *
 * Note: This tests the memory-efficient streaming read implementation
 * that reads chunks on-demand without loading the entire file into RAM.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../../drivers/microsd_driver.h"
#include "../data_frame.h"
#include "pico/stdlib.h"

/*! Test configuration */
#define TEST_INIT_STREAMING_EXECUTIONS 15
#define TEST_SINGLE_CHUNK_EXECUTIONS 10
#define TEST_FULL_FILE_EXECUTIONS 10
#define TEST_LARGE_FILE_EXECUTIONS 5
#define TEST_XLARGE_FILE_EXECUTIONS 3
#define TEST_XXLARGE_FILE_EXECUTIONS 2
#define TEST_CLEANUP_EXECUTIONS 12

/*! Test file configuration */
#define TEST_FILE_SMALL "STREAM_SMALL.TXT"
#define TEST_FILE_MEDIUM "STREAM_MEDIUM.DAT"
#define TEST_FILE_LARGE "STREAM_LARGE.BIN"
#define TEST_FILE_XLARGE "STREAM_XLARGE.BIN"
#define TEST_FILE_XXLARGE "STREAM_XXLARGE.BIN"

#define SMALL_FILE_SIZE 512       /* 512 bytes - 3 chunks */
#define MEDIUM_FILE_SIZE 2048     /* 2KB - 9 chunks */
#define LARGE_FILE_SIZE 8192      /* 8KB - 35 chunks */
#define XLARGE_FILE_SIZE 65536    /* 64KB - 277 chunks */
#define XXLARGE_FILE_SIZE 4194304 /* 4MB - 16807 chunks */

/*! MicroSD initialization retry configuration */
#define MICROSD_INIT_MAX_RETRIES 3
#define MICROSD_INIT_RETRY_DELAY_MS 2000
#define MICROSD_INIT_TIMEOUT_MS 10000

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
static void print_result(bool passed, const char *message, bool verbose) {
    if (!passed || verbose) {
        if (passed) {
            printf("\n" COLOR_GREEN "✓ PASS" COLOR_RESET ": %s\n", message);
        } else {
            printf("\n" COLOR_RED "✗ FAIL" COLOR_RESET ": %s\n", message);
        }
    }

    if (passed) {
        g_results.passed++;
    } else {
        g_results.failed++;
    }
    g_results.total_tests++;
}

/*!
 * @brief Print error message
 */
static void print_error(const char *message) {
    printf("\n" COLOR_RED "ERROR" COLOR_RESET ": %s\n", message);
    g_results.errors++;
}

/*!
 * @brief Initialize MicroSD with retry logic and timeout recovery
 * @return true if initialization successful, false otherwise
 */
static bool init_microsd_with_recovery(void) {
    printf("Initializing MicroSD with retry logic...\n");

    for (int attempt = 1; attempt <= MICROSD_INIT_MAX_RETRIES; attempt++) {
        printf("  Attempt %d/%d: ", attempt, MICROSD_INIT_MAX_RETRIES);
        fflush(stdout);

        absolute_time_t start_time = get_absolute_time();
        bool init_success = microsd_driver_init();
        int64_t elapsed_ms = absolute_time_diff_us(start_time, get_absolute_time()) / 1000;

        if (init_success) {
            printf(COLOR_GREEN "SUCCESS" COLOR_RESET " (took %lld ms)\n", (long long)elapsed_ms);
            return true;
        }

        printf(COLOR_YELLOW "FAILED" COLOR_RESET " (took %lld ms)\n", (long long)elapsed_ms);

        // If this wasn't the last attempt, wait before retrying
        if (attempt < MICROSD_INIT_MAX_RETRIES) {
            printf("  Waiting %d ms before retry...\n", MICROSD_INIT_RETRY_DELAY_MS);
            sleep_ms(MICROSD_INIT_RETRY_DELAY_MS);
        }
    }

    printf(COLOR_RED "  All initialization attempts failed!" COLOR_RESET "\n");
    return false;
}

/*!
 * @brief Create test files on SD card
 */
static bool create_test_files(void) {
    printf("\nCreating test files on SD card...\n");

    // Initialize filesystem with retry logic
    if (!init_microsd_with_recovery()) {
        print_error("Failed to initialize MicroSD after retries");
        return false;
    }

    // Create small test file
    static uint8_t small_buffer[SMALL_FILE_SIZE];
    for (size_t i = 0; i < SMALL_FILE_SIZE; i++) {
        small_buffer[i] = (uint8_t)(i & 0xFF);
    }

    if (!microsd_driver_write_file(TEST_FILE_SMALL, small_buffer, SMALL_FILE_SIZE)) {
        print_error("Failed to create small test file");
        return false;
    }
    printf("✓ Created %s (%d bytes)\n", TEST_FILE_SMALL, SMALL_FILE_SIZE);

    // Create medium test file
    static uint8_t medium_buffer[MEDIUM_FILE_SIZE];
    for (size_t i = 0; i < MEDIUM_FILE_SIZE; i++) {
        medium_buffer[i] = (uint8_t)((i * 3) & 0xFF);
    }

    if (!microsd_driver_write_file(TEST_FILE_MEDIUM, medium_buffer, MEDIUM_FILE_SIZE)) {
        print_error("Failed to create medium test file");
        return false;
    }
    printf("✓ Created %s (%d bytes)\n", TEST_FILE_MEDIUM, MEDIUM_FILE_SIZE);

    // Create large test file
    static uint8_t large_buffer[LARGE_FILE_SIZE];
    for (size_t i = 0; i < LARGE_FILE_SIZE; i++) {
        large_buffer[i] = (uint8_t)((i * 7 + 42) & 0xFF);
    }

    if (!microsd_driver_write_file(TEST_FILE_LARGE, large_buffer, LARGE_FILE_SIZE)) {
        print_error("Failed to create large test file");
        return false;
    }
    printf("✓ Created %s (%d bytes)\n", TEST_FILE_LARGE, LARGE_FILE_SIZE);

    // Create extra large test file (64KB) - test buffer limitations
    printf("Creating extra large test file (64KB)...\n");
    uint8_t *xlarge_buffer = (uint8_t *)malloc(XLARGE_FILE_SIZE);
    if (!xlarge_buffer) {
        print_error("Failed to allocate memory for extra large test file");
        return false;
    }

    for (size_t i = 0; i < XLARGE_FILE_SIZE; i++) {
        xlarge_buffer[i] = (uint8_t)((i * 13 + 37) & 0xFF);
    }

    if (!microsd_driver_write_file(TEST_FILE_XLARGE, xlarge_buffer, XLARGE_FILE_SIZE)) {
        print_error("Failed to create extra large test file");
        free(xlarge_buffer);
        return false;
    }
    free(xlarge_buffer);
    printf("✓ Created %s (%d bytes, %.2f KB)\n", TEST_FILE_XLARGE, XLARGE_FILE_SIZE,
           XLARGE_FILE_SIZE / 1024.0f);

    // Create largest test file (4MB) - stress test for true streaming
    printf("Creating largest test file (4MB) - this may take a while...\n");

    // Write in chunks to avoid memory issues
    FIL file;
    if (!microsd_driver_open(&file, TEST_FILE_XXLARGE, FA_WRITE | FA_CREATE_ALWAYS)) {
        print_error("Failed to open largest test file for writing");
        return false;
    }

    const uint32_t write_chunk_size = 32768; // 32KB chunks
    uint8_t *write_buffer = (uint8_t *)malloc(write_chunk_size);
    if (!write_buffer) {
        print_error("Failed to allocate write buffer for largest file");
        microsd_driver_close(&file);
        return false;
    }

    uint32_t bytes_written_total = 0;
    uint32_t chunk_count = XXLARGE_FILE_SIZE / write_chunk_size;

    for (uint32_t chunk = 0; chunk < chunk_count; chunk++) {
        // Fill buffer with pattern
        for (uint32_t i = 0; i < write_chunk_size; i++) {
            uint32_t file_offset = chunk * write_chunk_size + i;
            write_buffer[i] = (uint8_t)((file_offset * 17 + 89) & 0xFF);
        }

        UINT bytes_written = 0;
        if (!microsd_driver_write(&file, write_buffer, write_chunk_size, &bytes_written)) {
            print_error("Failed to write chunk to largest file");
            free(write_buffer);
            microsd_driver_close(&file);
            return false;
        }
        bytes_written_total += bytes_written;

        // Progress indicator
        if ((chunk + 1) % 32 == 0) {
            printf("  Progress: %.1f%% (%lu/%lu KB)\r",
                   (bytes_written_total * 100.0f) / XXLARGE_FILE_SIZE,
                   (unsigned long)(bytes_written_total / 1024),
                   (unsigned long)(XXLARGE_FILE_SIZE / 1024));
            fflush(stdout);
        }
    }

    free(write_buffer);
    microsd_driver_close(&file);
    printf("\n✓ Created %s (%lu bytes, %.2f MB)\n", TEST_FILE_XXLARGE,
           (unsigned long)XXLARGE_FILE_SIZE, XXLARGE_FILE_SIZE / (1024.0f * 1024.0f));

    return true;
}

/*!
 * @brief Test ST-1: Initialize Streaming Read
 * Target: 15 executions
 */
static void test_init_streaming(void) {
    printf("\n" COLOR_YELLOW "=== TEST ST-1: Initialize Streaming Read ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_INIT_STREAMING_EXECUTIONS);

    for (uint32_t i = 1; i <= TEST_INIT_STREAMING_EXECUTIONS; i++) {
        print_test_header("ST-1 Init", i, TEST_INIT_STREAMING_EXECUTIONS);

        // Alternate between test files
        const char *test_file = (i % 3 == 0)   ? TEST_FILE_LARGE
                                : (i % 2 == 0) ? TEST_FILE_MEDIUM
                                               : TEST_FILE_SMALL;

        struct Metadata metadata;
        memset(&metadata, 0, sizeof(metadata));

        // Initialize streaming read
        int result = init_streaming_read((char *)test_file, &metadata);

        if (result != 0) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Failed to initialize streaming for %s", test_file);
            print_result(false, msg, true);
            cleanup_streaming_read();
            continue;
        }

        // Verify metadata
        if (strlen(metadata.filename) == 0) {
            print_result(false, "Metadata filename is empty", true);
            cleanup_streaming_read();
            continue;
        }

        if (metadata.total_size == 0) {
            print_result(false, "Metadata total_size is zero", true);
            cleanup_streaming_read();
            continue;
        }

        if (metadata.chunk_count == 0) {
            print_result(false, "Metadata chunk_count is zero", true);
            cleanup_streaming_read();
            continue;
        }

        // Verify chunk count calculation
        uint32_t expected_chunks =
            (metadata.total_size + PAYLOAD_DATA_SIZE - 1) / PAYLOAD_DATA_SIZE;
        if (metadata.chunk_count != expected_chunks) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Chunk count mismatch: expected %lu, got %lu",
                     (unsigned long)expected_chunks, (unsigned long)metadata.chunk_count);
            print_result(false, msg, true);
            cleanup_streaming_read();
            continue;
        }

        cleanup_streaming_read();

        char msg[128];
        snprintf(msg, sizeof(msg), "Initialized streaming for %s (%lu bytes, %lu chunks)",
                 metadata.filename, (unsigned long)metadata.total_size,
                 (unsigned long)metadata.chunk_count);
        print_result(true, msg, i == TEST_INIT_STREAMING_EXECUTIONS);
    }
    printf("\n");
}

/*!
 * @brief Test ST-2: Stream Single Chunk
 * Target: 10 executions
 */
static void test_stream_single_chunk(void) {
    printf("\n" COLOR_YELLOW "=== TEST ST-2: Stream Single Chunk ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_SINGLE_CHUNK_EXECUTIONS);

    for (uint32_t i = 1; i <= TEST_SINGLE_CHUNK_EXECUTIONS; i++) {
        print_test_header("ST-2 Single", i, TEST_SINGLE_CHUNK_EXECUTIONS);

        // Initialize streaming
        struct Metadata metadata;
        if (init_streaming_read(TEST_FILE_MEDIUM, &metadata) != 0) {
            print_result(false, "Failed to initialize streaming", true);
            continue;
        }

        // Read different chunk each iteration
        uint32_t chunk_index = i % metadata.chunk_count;
        struct Payload chunk;
        memset(&chunk, 0, sizeof(chunk));

        int result = read_chunk_streaming(chunk_index, &chunk);

        if (result != 0) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Failed to read chunk %lu", (unsigned long)chunk_index);
            print_result(false, msg, true);
            cleanup_streaming_read();
            continue;
        }

        // Verify chunk metadata (sequence uses 1-based indexing)
        if (chunk.sequence != chunk_index + 1) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Sequence mismatch: expected %lu, got %lu",
                     (unsigned long)(chunk_index + 1), (unsigned long)chunk.sequence);
            print_result(false, msg, true);
            cleanup_streaming_read();
            continue;
        }

        // Verify CRC (all payloads are now 239 bytes)
        uint32_t calculated_crc = crc32(chunk.data, PAYLOAD_DATA_SIZE);
        if (calculated_crc != chunk.crc32) {
            char msg[128];
            snprintf(msg, sizeof(msg), "CRC mismatch: expected 0x%08X, got 0x%08X", calculated_crc,
                     chunk.crc32);
            print_result(false, msg, true);
            cleanup_streaming_read();
            continue;
        }

        cleanup_streaming_read();

        char msg[128];
        snprintf(msg, sizeof(msg), "Read chunk %lu (%d bytes, CRC 0x%08X)",
                 (unsigned long)chunk_index, PAYLOAD_DATA_SIZE, chunk.crc32);
        print_result(true, msg, false);
    }
    printf("\n");
}

/*!
 * @brief Test ST-3: Stream Full File
 * Target: 10 executions
 */
static void test_stream_full_file(void) {
    printf("\n" COLOR_YELLOW "=== TEST ST-3: Stream Full File ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_FULL_FILE_EXECUTIONS);

    for (uint32_t i = 1; i <= TEST_FULL_FILE_EXECUTIONS; i++) {
        print_test_header("ST-3 Full", i, TEST_FULL_FILE_EXECUTIONS);

        // Initialize streaming
        struct Metadata metadata;
        if (init_streaming_read(TEST_FILE_SMALL, &metadata) != 0) {
            print_result(false, "Failed to initialize streaming", true);
            cleanup_streaming_read();
            continue;
        }

        // Stream all chunks
        bool all_chunks_ok = true;
        uint32_t total_bytes = 0;

        for (uint32_t chunk_idx = 0; chunk_idx < metadata.chunk_count; chunk_idx++) {
            struct Payload chunk;

            if (read_chunk_streaming(chunk_idx, &chunk) != 0) {
                char msg[128];
                snprintf(msg, sizeof(msg), "Failed to read chunk %lu", (unsigned long)chunk_idx);
                print_result(false, msg, true);
                all_chunks_ok = false;
                break;
            }

            // Verify sequence (data chunks use 1-based indexing, 0 is reserved for metadata)
            if (chunk.sequence != chunk_idx + 1) {
                char msg[128];
                snprintf(msg, sizeof(msg), "Sequence mismatch at chunk %lu: expected %lu, got %lu",
                         (unsigned long)chunk_idx, (unsigned long)(chunk_idx + 1),
                         (unsigned long)chunk.sequence);
                print_result(false, msg, true);
                all_chunks_ok = false;
                break;
            }

            // Verify CRC
            if (verify_chunk(&chunk) != 1) {
                all_chunks_ok = false;
                break;
            }

            // Each chunk is PAYLOAD_DATA_SIZE bytes (except possibly the last one)
            uint32_t chunk_bytes = PAYLOAD_DATA_SIZE;
            if (chunk_idx == metadata.chunk_count - 1) {
                // Last chunk might be smaller
                chunk_bytes = metadata.total_size - (chunk_idx * PAYLOAD_DATA_SIZE);
                if (chunk_bytes > PAYLOAD_DATA_SIZE) {
                    chunk_bytes = PAYLOAD_DATA_SIZE;
                }
            }

            total_bytes += chunk_bytes;
        }

        // Get the file CRC32 calculated during streaming
        uint32_t file_crc32 = get_streaming_file_crc();

        cleanup_streaming_read();

        if (!all_chunks_ok) {
            print_result(false, "Failed to stream all chunks", true);
            continue;
        }

        // Verify total size
        if (total_bytes != metadata.total_size) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Size mismatch: expected %lu, got %lu",
                     (unsigned long)metadata.total_size, (unsigned long)total_bytes);
            print_result(false, msg, true);
            continue;
        }

        // Verify file CRC32 was calculated
        if (file_crc32 == 0) {
            print_result(false, "File CRC32 not calculated", true);
            continue;
        }

        char msg[128];
        snprintf(msg, sizeof(msg), "Streamed full file (%lu chunks, %lu bytes, CRC32: 0x%08X)",
                 (unsigned long)metadata.chunk_count, (unsigned long)total_bytes, file_crc32);
        print_result(true, msg, i == TEST_FULL_FILE_EXECUTIONS);
    }
    printf("\n");
}

/*!
 * @brief Test ST-4: Stream Large File
 * Target: 5 executions
 */
static void test_stream_large_file(void) {
    printf("\n" COLOR_YELLOW "=== TEST ST-4: Stream Large File ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_LARGE_FILE_EXECUTIONS);

    for (uint32_t i = 1; i <= TEST_LARGE_FILE_EXECUTIONS; i++) {
        print_test_header("ST-4 Large", i, TEST_LARGE_FILE_EXECUTIONS);

        // Initialize streaming for large file
        struct Metadata metadata;
        if (init_streaming_read(TEST_FILE_LARGE, &metadata) != 0) {
            print_result(false, "Failed to initialize streaming for large file", true);
            cleanup_streaming_read();
            continue;
        }

        // Stream all chunks with CRC verification
        bool all_chunks_ok = true;
        uint32_t chunks_verified = 0;

        for (uint32_t chunk_idx = 0; chunk_idx < metadata.chunk_count; chunk_idx++) {
            struct Payload chunk;

            if (read_chunk_streaming(chunk_idx, &chunk) != 0) {
                all_chunks_ok = false;
                break;
            }

            if (verify_chunk(&chunk) != 1) {
                all_chunks_ok = false;
                break;
            }

            chunks_verified++;
        }

        cleanup_streaming_read();

        if (!all_chunks_ok) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Failed at chunk %lu/%lu", (unsigned long)chunks_verified,
                     (unsigned long)metadata.chunk_count);
            print_result(false, msg, true);
            continue;
        }

        char msg[128];
        snprintf(msg, sizeof(msg), "Streamed large file (%lu chunks verified)",
                 (unsigned long)chunks_verified);
        print_result(true, msg, i == TEST_LARGE_FILE_EXECUTIONS);
    }
    printf("\n");
}

/*!
 * @brief Test ST-6: Stream XLARGE file (64KB)
 * Target: 3 executions
 */
static void test_stream_xlarge_file(void) {
    printf("\n" COLOR_YELLOW "=== TEST ST-6: Stream XLARGE File (64KB) ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_XLARGE_FILE_EXECUTIONS);

    for (uint32_t i = 1; i <= TEST_XLARGE_FILE_EXECUTIONS; i++) {
        print_test_header("ST-6 XLARGE", i, TEST_XLARGE_FILE_EXECUTIONS);

        // Initialize streaming
        struct Metadata metadata;
        if (init_streaming_read(TEST_FILE_XLARGE, &metadata) != 0) {
            print_result(false, "Failed to initialize streaming for XLARGE file", true);
            continue;
        }

        // Verify metadata
        if (metadata.total_size != 65536) {
            print_result(false, "Incorrect file size in metadata", true);
            cleanup_streaming_read();
            continue;
        }

        // Stream all chunks with CRC verification
        bool all_chunks_ok = true;
        uint32_t chunks_verified = 0;

        for (uint32_t chunk_num = 0; chunk_num < metadata.chunk_count; chunk_num++) {
            struct Payload chunk;

            if (read_chunk_streaming(chunk_num, &chunk) != 0) {
                all_chunks_ok = false;
                break;
            }
            if (verify_chunk(&chunk) != 1) {
                all_chunks_ok = false;
                break;
            }
            chunks_verified++;
        }

        // Get the file CRC32 calculated during streaming
        uint32_t file_crc32 = get_streaming_file_crc();

        cleanup_streaming_read();

        if (!all_chunks_ok) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Failed at chunk %lu/%lu", (unsigned long)chunks_verified,
                     (unsigned long)metadata.chunk_count);
            print_result(false, msg, true);
            continue;
        }

        // Verify file CRC32 was calculated
        if (file_crc32 == 0) {
            print_result(false, "File CRC32 not calculated", true);
            continue;
        }

        char msg[128];
        snprintf(msg, sizeof(msg), "Streamed XLARGE file (%lu chunks, CRC32: 0x%08X)",
                 (unsigned long)chunks_verified, file_crc32);
        print_result(true, msg, i == TEST_XLARGE_FILE_EXECUTIONS);
    }
    printf("\n");
}

/*!
 * @brief Test ST-7: Stream XXLARGE file (4MB)
 * Target: 2 executions
 */
static void test_stream_xxlarge_file(void) {
    printf("\n" COLOR_YELLOW "=== TEST ST-7: Stream XXLARGE File (4MB) ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_XXLARGE_FILE_EXECUTIONS);

    for (uint32_t i = 1; i <= TEST_XXLARGE_FILE_EXECUTIONS; i++) {
        print_test_header("ST-7 XXLARGE", i, TEST_XXLARGE_FILE_EXECUTIONS);

        // Initialize streaming
        struct Metadata metadata;
        if (init_streaming_read(TEST_FILE_XXLARGE, &metadata) != 0) {
            print_result(false, "Failed to initialize streaming for XXLARGE file", true);
            continue;
        }

        // Verify metadata
        if (metadata.total_size != 4194304) {
            print_result(false, "Incorrect file size in metadata", true);
            cleanup_streaming_read();
            continue;
        }

        printf("  Streaming 4MB file (%lu chunks)...\n", (unsigned long)metadata.chunk_count);

        // Stream all chunks with CRC verification
        bool all_chunks_ok = true;
        uint32_t chunks_verified = 0;

        for (uint32_t chunk_num = 0; chunk_num < metadata.chunk_count; chunk_num++) {
            // Progress indicator every 1000 chunks
            if (chunk_num % 1000 == 0 && chunk_num > 0) {
                printf("  Progress: %lu/%lu chunks (%.1f%%)\n", (unsigned long)chunk_num,
                       (unsigned long)metadata.chunk_count,
                       (chunk_num * 100.0 / metadata.chunk_count));
            }

            struct Payload chunk;

            if (read_chunk_streaming(chunk_num, &chunk) != 0) {
                all_chunks_ok = false;
                break;
            }

            if (verify_chunk(&chunk) != 1) {
                all_chunks_ok = false;
                break;
            }

            chunks_verified++;
        }

        // Get the file CRC32 calculated during streaming
        uint32_t file_crc32 = get_streaming_file_crc();

        cleanup_streaming_read();

        if (!all_chunks_ok) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Failed at chunk %lu/%lu", (unsigned long)chunks_verified,
                     (unsigned long)metadata.chunk_count);
            print_result(false, msg, true);
            continue;
        }

        // Verify file CRC32 was calculated
        if (file_crc32 == 0) {
            print_result(false, "File CRC32 not calculated", true);
            continue;
        }

        char msg[128];
        snprintf(msg, sizeof(msg), "Streamed XXLARGE file (%lu chunks, CRC32: 0x%08X)",
                 (unsigned long)chunks_verified, file_crc32);
        print_result(true, msg, i == TEST_XXLARGE_FILE_EXECUTIONS);
    }
    printf("\n");
}

/*!
 * @brief Test ST-5: Cleanup and Re-initialization
 * Target: 12 executions
 */
static void test_cleanup_and_reinit(void) {
    printf("\n" COLOR_YELLOW "=== TEST ST-5: Cleanup and Re-initialization ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_CLEANUP_EXECUTIONS);

    for (uint32_t i = 1; i <= TEST_CLEANUP_EXECUTIONS; i++) {
        print_test_header("ST-5 Cleanup", i, TEST_CLEANUP_EXECUTIONS);

        // Initialize
        struct Metadata metadata1;
        if (init_streaming_read(TEST_FILE_SMALL, &metadata1) != 0) {
            print_result(false, "Failed to initialize first streaming", true);
            continue;
        }

        // Cleanup
        cleanup_streaming_read();

        // Re-initialize with different file
        struct Metadata metadata2;
        if (init_streaming_read(TEST_FILE_MEDIUM, &metadata2) != 0) {
            print_result(false, "Failed to re-initialize streaming", true);
            cleanup_streaming_read();
            continue;
        }

        // Verify we can read from new file
        struct Payload chunk;
        int result = read_chunk_streaming(0, &chunk);

        cleanup_streaming_read();

        if (result != 0) {
            print_result(false, "Failed to read after re-initialization", true);
            continue;
        }

        print_result(true, "Cleanup and re-initialization successful", false);
    }
    printf("\n");
}

/*!
 * @brief Print final test summary
 */
static void print_summary(void) {
    printf("\n");
    printf("================================================================================\n");
    printf(COLOR_YELLOW "                AUTOMATED STREAMING TEST SUMMARY" COLOR_RESET "\n");
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
    printf("  ST-1 (Init streaming):       %d executions\n", TEST_INIT_STREAMING_EXECUTIONS);
    printf("  ST-2 (Stream single chunk):  %d executions\n", TEST_SINGLE_CHUNK_EXECUTIONS);
    printf("  ST-3 (Stream full file):     %d executions\n", TEST_FULL_FILE_EXECUTIONS);
    printf("  ST-4 (Stream large file):    %d executions\n", TEST_LARGE_FILE_EXECUTIONS);
    printf("  ST-6 (Stream XLARGE file):   %d executions\n", TEST_XLARGE_FILE_EXECUTIONS);
    printf("  ST-7 (Stream XXLARGE file):  %d executions\n", TEST_XXLARGE_FILE_EXECUTIONS);
    printf("  ST-5 (Cleanup):              %d executions\n", TEST_CLEANUP_EXECUTIONS);
    printf("  ─────────────────────────────────────\n");
    printf("  Total:                       %d executions\n",
           TEST_INIT_STREAMING_EXECUTIONS + TEST_SINGLE_CHUNK_EXECUTIONS +
               TEST_FULL_FILE_EXECUTIONS + TEST_LARGE_FILE_EXECUTIONS + TEST_CLEANUP_EXECUTIONS);

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
    printf("              AUTOMATED STREAMING READ UNIT TESTS\n");
    printf("================================================================================\n");
    printf("Test Suite: Memory-Efficient Streaming File Read\n");
    printf("Based on: Core Traceability Matrix Requirements\n");
    printf("Date: 3 November 2025\n");
    printf("================================================================================\n");

    // Create test files
    if (!create_test_files()) {
        printf("\n" COLOR_RED "Failed to create test files. Aborting tests." COLOR_RESET "\n");
        while (true) {
            sleep_ms(1000);
        }
    }

    printf("\n" COLOR_GREEN "Test files created successfully!" COLOR_RESET "\n");

    // Run all test suites
    test_init_streaming();
    test_stream_single_chunk();
    test_stream_full_file();
    test_stream_large_file();
    test_stream_xlarge_file();
    test_stream_xxlarge_file();
    test_cleanup_and_reinit();

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
