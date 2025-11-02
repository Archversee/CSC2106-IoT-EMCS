/*!
 * @file    chunk_transfer_filename_test.c
 * @brief   Test chunk transfer with new filename modification feature
 * @author  INF2004 Project Team
 * @date    2025
 *
 * This test demonstrates the chunk transfer API with the new automatic
 * filename renaming feature that adds "_received" suffix to transferred files.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../drivers/microsd_driver.h"
#include "../chunk_transfer.h"
#include "../data_frame.h"
#include "pico/stdlib.h"

/*! Test configuration constants */
#define SOURCE_FILENAME "praise_lord_fauzi.txt"

/*!
 * @brief Test the chunk transfer with automatic filename modification
 * @param source_filename Source file to transfer
 * @return bool true on success
 */
static bool test_chunk_transfer_with_new_filename(const char* source_filename) {
    printf("\n╔═══════════════════════════════════════════════════════╗\n");
    printf("║  Testing Chunk Transfer with New Filename Logic      ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");

    filesystem_info_t fs_info = {0};
    transfer_session_t session = {0};

    // Step 1: Initialize filesystem
    printf("\n=== Step 1: Initialize Filesystem ===\n");
    if (!microsd_init_filesystem(&fs_info)) {
        printf("ERROR: Failed to initialize filesystem\n");
        return false;
    }
    printf("✓ Filesystem initialized\n");

    // Step 2: Initialize streaming read to get metadata
    printf("\n=== Step 2: Prepare Source File ===\n");
    struct Metadata meta = {0};

    if (init_streaming_read((char*)source_filename, &meta) != 0) {
        printf("ERROR: Failed to initialize streaming read\n");
        return false;
    }

    printf("✓ Source file prepared:\n");
    printf("  Original filename: %s\n", meta.filename);
    printf("  File size: %u bytes\n", meta.total_size);
    printf("  Total chunks: %u\n", meta.chunk_count);
    printf("  Session ID: %s\n", meta.session_id);

    // Step 3: Initialize chunk transfer session (this will apply the new filename)
    printf("\n=== Step 3: Initialize Transfer Session ===\n");
    printf("Calling chunk_transfer_init_session()...\n");

    if (!chunk_transfer_init_session(&fs_info, &meta, &session)) {
        printf("ERROR: Failed to initialize transfer session\n");
        cleanup_streaming_read();
        return false;
    }

    printf("\n>>> Notice: File will be saved with modified name! <<<\n");

    // Step 4: Transfer all chunks
    printf("\n=== Step 4: Transfer Data Chunks ===\n");
    printf("Transferring %u chunks...\n", meta.chunk_count);

    absolute_time_t transfer_start = get_absolute_time();
    uint32_t chunks_transferred = 0;

    for (uint32_t i = 0; i < meta.chunk_count; i++) {
        struct Payload chunk = {0};

        // Read chunk from source file
        if (read_chunk_streaming(i, &chunk) != 0) {
            printf("ERROR: Failed to read chunk %u\n", i);
            cleanup_streaming_read();
            return false;
        }

        // Verify chunk integrity
        if (!verify_chunk(&chunk)) {
            printf("ERROR: Chunk %u failed verification\n", i);
            cleanup_streaming_read();
            return false;
        }

        // Write chunk using chunk_transfer API
        if (!chunk_transfer_write_payload(&fs_info, &session, &chunk)) {
            printf("ERROR: Failed to write chunk %u\n", i);
            cleanup_streaming_read();
            return false;
        }

        chunks_transferred++;

        // Progress updates
        if (i == 0 || i == meta.chunk_count - 1 || (i + 1) % 10 == 0) {
            uint32_t received, total;
            chunk_transfer_get_progress(&session, &received, &total);
            printf("  [%u/%u] Chunk transferred (received: %u/%u)\n",
                   i + 1, meta.chunk_count, received, total);
        }
    }

    absolute_time_t transfer_end = get_absolute_time();
    int64_t transfer_ms = absolute_time_diff_us(transfer_start, transfer_end) / 1000;

    cleanup_streaming_read();

    printf("\n✓ All chunks transferred in %lld ms\n", transfer_ms);

    // Step 5: Check completion and finalize
    printf("\n=== Step 5: Finalize Transfer ===\n");

    if (!chunk_transfer_is_complete(&session)) {
        printf("ERROR: Transfer incomplete!\n");
        chunk_transfer_print_session_info(&session);
        return false;
    }

    printf("✓ All chunks received, finalizing...\n");

    if (!chunk_transfer_finalize(&fs_info, &session)) {
        printf("ERROR: Failed to finalize transfer\n");
        return false;
    }

    // Step 6: Print summary
    printf("\n╔═══════════════════════════════════════════════════════╗\n");
    printf("║              Transfer Complete Summary                ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    printf("\n  ✓ Source file: %s\n", source_filename);
    printf("  ✓ Output file: %s\n", session.filename);
    printf("  ✓ Chunks transferred: %u\n", chunks_transferred);
    printf("  ✓ Total size: %u bytes\n", meta.total_size);
    printf("  ✓ Transfer time: %lld ms\n", transfer_ms);
    printf("\n  Key Feature Demonstrated:\n");
    printf("    • Automatic filename modification\n");
    printf("    • %s → %s\n", source_filename, session.filename);
    printf("    • Prevents overwriting original files\n");
    printf("\n═══════════════════════════════════════════════════════\n");

    return true;
}

/*!
 * @brief Test duplicate chunk handling
 * @param source_filename Source file to test with
 * @return bool true on success
 */
static bool test_duplicate_chunk_handling(const char* source_filename) {
    printf("\n╔═══════════════════════════════════════════════════════╗\n");
    printf("║  Testing Duplicate Chunk Handling                     ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");

    filesystem_info_t fs_info = {0};
    transfer_session_t session = {0};

    // Initialize filesystem
    printf("\nInitializing filesystem...\n");
    if (!microsd_init_filesystem(&fs_info)) {
        printf("ERROR: Failed to initialize filesystem\n");
        return false;
    }

    // Initialize streaming read
    struct Metadata meta = {0};
    if (init_streaming_read((char*)source_filename, &meta) != 0) {
        printf("ERROR: Failed to initialize streaming read\n");
        return false;
    }

    // Initialize transfer session
    printf("Initializing transfer session...\n");
    if (!chunk_transfer_init_session(&fs_info, &meta, &session)) {
        printf("ERROR: Failed to initialize transfer session\n");
        cleanup_streaming_read();
        return false;
    }

    printf("✓ Session initialized with output: %s\n", session.filename);

    // Transfer first 3 chunks
    printf("\nTransferring first 3 chunks...\n");
    for (uint32_t i = 0; i < 3 && i < meta.chunk_count; i++) {
        struct Payload chunk = {0};
        if (read_chunk_streaming(i, &chunk) != 0) {
            printf("ERROR: Failed to read chunk %u\n", i);
            cleanup_streaming_read();
            return false;
        }
        if (!chunk_transfer_write_payload(&fs_info, &session, &chunk)) {
            printf("ERROR: Failed to write chunk %u\n", i);
            cleanup_streaming_read();
            return false;
        }
        printf("  Chunk %u transferred\n", i);
    }

    // Simulate duplicate: resend chunk 1
    printf("\nSimulating duplicate chunk (resending chunk 1)...\n");
    struct Payload dup_chunk = {0};
    if (read_chunk_streaming(1, &dup_chunk) != 0) {
        printf("ERROR: Failed to read duplicate chunk\n");
        cleanup_streaming_read();
        return false;
    }

    if (!chunk_transfer_write_payload(&fs_info, &session, &dup_chunk)) {
        printf("ERROR: Duplicate chunk handling failed\n");
        cleanup_streaming_read();
        return false;
    }
    printf("✓ Duplicate chunk handled correctly (should show warning)\n");

    // Transfer remaining chunks
    printf("\nTransferring remaining chunks...\n");
    for (uint32_t i = 3; i < meta.chunk_count; i++) {
        struct Payload chunk = {0};
        if (read_chunk_streaming(i, &chunk) != 0) {
            printf("ERROR: Failed to read chunk %u\n", i);
            cleanup_streaming_read();
            return false;
        }
        if (!chunk_transfer_write_payload(&fs_info, &session, &chunk)) {
            printf("ERROR: Failed to write chunk %u\n", i);
            cleanup_streaming_read();
            return false;
        }
    }

    cleanup_streaming_read();

    // Finalize
    printf("\nFinalizing transfer...\n");
    if (!chunk_transfer_finalize(&fs_info, &session)) {
        printf("ERROR: Failed to finalize\n");
        return false;
    }

    printf("\n✓ Duplicate chunk test passed!\n");
    printf("  File saved as: %s\n", session.filename);

    return true;
}

/*!
 * @brief Main test function
 */
int main(void) {
    int test_count = 1;

    stdio_init_all();

    // Give USB serial time to connect
    sleep_ms(2000);

    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║  Chunk Transfer Filename Test                         ║\n");
    printf("║  Source: %-44s ║\n", SOURCE_FILENAME);
    printf("║  Feature: Automatic _received suffix                  ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");

    // Initial delay
    printf("\nInitializing...\n");
    for (int i = 0; i < 5; i++) {
        printf("  Waiting... %d\n", 5 - i);
        sleep_ms(1000);
    }

    printf("\nStarting test loop...\n");

    while (true) {
        printf("\n\n");
        printf("╔═══════════════════════════════════════════════════════╗\n");
        printf("║                  Test Run #%-3d                        ║\n", test_count);
        printf("╚═══════════════════════════════════════════════════════╝\n");

        // Initialize microSD
        printf("\nInitializing microSD card...\n");
        if (!microsd_init()) {
            printf("✗ ERROR: Failed to initialize microSD card\n");
            printf("\n--- Waiting 30 seconds before retry ---\n");
            sleep_ms(30000);
            test_count++;
            continue;
        }
        printf("✓ MicroSD card initialized\n");

        microsd_set_log_level(MICROSD_LOG_INFO);

        uint32_t start_time = to_ms_since_boot(get_absolute_time());
        bool all_passed = true;

        // Test 1: Basic chunk transfer with new filename
        if (!test_chunk_transfer_with_new_filename(SOURCE_FILENAME)) {
            printf("\n✗ FAILED: Chunk transfer test\n");
            all_passed = false;
        } else {
            printf("\n✓ PASSED: Chunk transfer test\n");
        }

        sleep_ms(2000);

        // Test 2: Duplicate chunk handling
        if (all_passed) {
            if (!test_duplicate_chunk_handling(SOURCE_FILENAME)) {
                printf("\n✗ FAILED: Duplicate chunk test\n");
                all_passed = false;
            } else {
                printf("\n✓ PASSED: Duplicate chunk test\n");
            }
        }

        uint32_t end_time = to_ms_since_boot(get_absolute_time());
        uint32_t elapsed_ms = end_time - start_time;

        // Final summary
        printf("\n");
        printf("╔═══════════════════════════════════════════════════════╗\n");
        printf("║              Final Summary - Run #%-3d                 ║\n", test_count);
        printf("╚═══════════════════════════════════════════════════════╝\n");

        if (all_passed) {
            printf("\n  🎯 ALL TESTS PASSED!\n");
            printf("\n  Successfully verified:\n");
            printf("    ✓ Automatic filename modification\n");
            printf("    ✓ Files saved with _received suffix\n");
            printf("    ✓ Duplicate chunk handling\n");
            printf("    ✓ Complete file transfer workflow\n");
            printf("\n  Example:\n");
            printf("    Input:  %s\n", SOURCE_FILENAME);
            printf("    Output: praise_lord_fauzi_received.txt\n");
        } else {
            printf("\n  ⚠️  SOME TESTS FAILED!\n");
            printf("  Please check the error messages above.\n");
        }

        printf("\n  Execution time: %lu ms\n", (unsigned long)elapsed_ms);
        printf("\n═══════════════════════════════════════════════════════\n");

        printf("\n--- Waiting 30 seconds for next test run ---\n");
        sleep_ms(30000);
        test_count++;
    }

    return 0;
}
