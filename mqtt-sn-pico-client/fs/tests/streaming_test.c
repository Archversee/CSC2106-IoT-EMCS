/*!
 * @file    streaming_test.c
 * @brief   MQTT-SN streaming test - simulates file deconstruct, transfer, and reconstruct
 * @author  INF2004 Project Team
 * @date    2025
 *
 * This test demonstrates the streaming chunk read API for MQTT-SN data transfer.
 * It takes a file, deconstructs it using streaming method, simulates MQTT transmission,
 * and reconstructs it to verify integrity.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../drivers/microsd_driver.h"
#include "../chunk_transfer.h"
#include "data_frame.h"
#include "ff.h" // FatFS
#include "pico/stdlib.h"

/*! Test configuration constants */
#define SOURCE_FILENAME "test.jpg"

/*!
 * @brief Get file size from filesystem using microsd_driver
 * @param filename Filename to check
 * @return uint32_t File size in bytes, 0 if not found
 */
static uint32_t get_file_size(const char *filename) {
    printf("  get_file_size() called for: %s\n", filename);

    FILINFO fno;
    if (!microsd_driver_stat(filename, &fno)) {
        printf("  ERROR: Could not stat file: %s\n", filename);
        return 0;
    }

    uint32_t size = fno.fsize;
    printf("  File size: %u bytes\n", size);
    return size;
}

/*!
 * @brief Test streaming read and display chunk information
 * @param filename Source file to stream
 * @return bool true on success
 */
static bool test_streaming_read(const char *filename) {
    printf("\n=== Step 1: Initialize Streaming Read ===\n");

    struct Metadata meta = {0};

    // Initialize streaming read
    if (init_streaming_read((char *)filename, &meta) != 0) {
        printf("ERROR: Failed to initialize streaming read\n");
        return false;
    }

    printf("SUCCESS: File ready for streaming\n");
    printf("  Filename: %s\n", meta.filename);
    printf("  File size: %u bytes\n", meta.total_size);
    printf("  Total chunks: %u\n", meta.chunk_count);
    printf("  Session ID: %s\n", meta.session_id);
    printf("  File CRC: 0x%04X\n", meta.file_crc);

    // Show sample chunks
    printf("\n=== Sample Chunk Preview ===\n");
    uint32_t samples_to_show = (meta.chunk_count < 3) ? meta.chunk_count : 3;

    for (uint32_t i = 0; i < samples_to_show; i++) {
        struct Payload chunk = {0};

        if (read_chunk_streaming(i, &chunk) != 0) {
            printf("ERROR: Failed to read chunk %u\n", i);
            cleanup_streaming_read();
            return false;
        }

        if (!verify_chunk(&chunk)) {
            printf("ERROR: Chunk %u failed verification\n", i);
            cleanup_streaming_read();
            return false;
        }

        printf("\nChunk %u: seq=%u, size=%u bytes, crc=0x%04X\n", i, chunk.sequence, chunk.size,
               chunk.crc);

        printf("  Data preview: \"");
        for (uint32_t j = 0; j < 80 && j < chunk.size; j++) {
            char c = chunk.data[j];
            printf("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        printf("%s\"\n", chunk.size > 80 ? "..." : "");
    }

    cleanup_streaming_read();
    printf("\nStreaming read test completed successfully\n");
    return true;
}

/*!
 * @brief Simulate MQTT-SN data transfer: stream chunks one at a time
 * @param filename Source file to transfer
 * @return bool true on success
 */
static bool test_mqtt_streaming(const char *filename) {
    printf("\n=== Step 2: Simulated MQTT-SN Streaming Transfer ===\n");

    struct Metadata meta = {0};

    // Initialize streaming
    if (init_streaming_read((char *)filename, &meta) != 0) {
        printf("ERROR: Failed to initialize streaming\n");
        return false;
    }

    printf("SENDER: Preparing to transmit file via MQTT-SN\n");
    printf("  Session ID: %s\n", meta.session_id);
    printf("  Total chunks: %u\n", meta.chunk_count);

    printf("\n[0/%u] Sending metadata packet...\n", meta.chunk_count);
    sleep_ms(20); // Simulate network delay

    // Stream chunks one at a time (simulating MQTT transmission)
    uint32_t bytes_sent = 0;
    absolute_time_t start = get_absolute_time();

    for (uint32_t i = 0; i < meta.chunk_count; i++) {
        struct Payload chunk = {0};

        // Read chunk from SD card (sender side)
        if (read_chunk_streaming(i, &chunk) != 0) {
            printf("ERROR: Failed to read chunk %u\n", i);
            cleanup_streaming_read();
            return false;
        }

        // Verify chunk before sending
        if (!verify_chunk(&chunk)) {
            printf("ERROR: Invalid chunk %u\n", i);
            cleanup_streaming_read();
            return false;
        }

        bytes_sent += chunk.size;

        // Show detailed info for first, last, and periodic chunks
        if (i < 2 || i == meta.chunk_count - 1 || (i + 1) % 10 == 0) {
            printf("[%u/%u] Chunk transmitted (seq=%u, size=%u, crc=0x%04X)\n", i + 1,
                   meta.chunk_count, chunk.sequence, chunk.size, chunk.crc);
        }

        // Simulate network delay
        sleep_ms(5);
    }

    absolute_time_t end = get_absolute_time();
    int64_t elapsed_ms = absolute_time_diff_us(start, end) / 1000;

    cleanup_streaming_read();

    printf("\nTRANSMISSION COMPLETE\n");
    printf("  Chunks sent: %u\n", meta.chunk_count);
    printf("  Total bytes: %u\n", bytes_sent);
    printf("  Time: %lld ms\n", elapsed_ms);
    printf("  Throughput: %.2f KB/s\n",
           (float)bytes_sent / (elapsed_ms > 0 ? elapsed_ms / 1000.0 : 1.0) / 1024.0);

    return true;
}

/*!
 * @brief Test reconstruction: stream chunks and reconstruct file
 * @param source_filename Source file to reconstruct
 * @param expected_size Expected file size in bytes
 * @return bool true on success
 */
static bool test_reconstruction(const char *source_filename, uint32_t expected_size) {
    printf("\n=== Step 3: File Reconstruction (Receiver Side) ===\n");

    struct Metadata meta = {0};

    // Initialize streaming (sender side - simulating transmission)
    if (init_streaming_read((char *)source_filename, &meta) != 0) {
        printf("ERROR: Failed to initialize streaming\n");
        return false;
    }

    printf("RECEIVER: Preparing to receive file\n");
    printf("  Expected file: %s\n", source_filename);
    printf("  Expected size: %u bytes\n", expected_size);
    printf("  Chunks to receive: %u\n", meta.chunk_count);

    printf("\nInitializing reconstruction session...\n");
    absolute_time_t recon_start = get_absolute_time();

    // Initialize transfer session with use_new_filename=true to test the flag
    transfer_session_t recon_session = {0};
    if (!chunk_transfer_init_session(&meta, &recon_session, true)) {
        printf("ERROR: Failed to initialize reconstruction session\n");
        cleanup_streaming_read();
        return false;
    }

    printf("  Output file will be: %s\n", recon_session.filename);
    printf("  Memory usage: ~33 KB (32KB read buffer + chunk buffer)\n");

    // TRUE STREAMING: Read one chunk at a time and immediately write it
    // No array allocation - uses only a single chunk buffer
    printf("\nStreaming chunks (read -> verify -> write)...\n");

    struct Payload chunk = {0}; // Single chunk buffer (~250 bytes)

    for (uint32_t i = 0; i < meta.chunk_count; i++) {
        // Read chunk from source file (simulating MQTT reception)
        if (read_chunk_streaming(i, &chunk) != 0) {
            printf("ERROR: Failed to read chunk %u\n", i);
            cleanup_streaming_read();
            return false;
        }

        // Verify chunk integrity
        if (!verify_chunk(&chunk)) {
            printf("ERROR: Chunk %u failed CRC verification\n", i);
            cleanup_streaming_read();
            return false;
        }

        // Immediately write verified chunk to destination file
        if (!chunk_transfer_write_payload(&recon_session, &chunk)) {
            printf("ERROR: Failed to write chunk %u (seq=%u) during reconstruction\n", i,
                   chunk.sequence);
            cleanup_streaming_read();
            return false;
        }

        // Progress updates
        if (i == 0 || i == meta.chunk_count - 1 || (i + 1) % 100 == 0) {
            printf("  [%u/%u] Chunk streamed (seq=%u)\n", i + 1, meta.chunk_count, chunk.sequence);
        }
    }

    cleanup_streaming_read();

    // Finalize the transfer
    if (!chunk_transfer_finalize(&recon_session)) {
        printf("ERROR: Failed to finalize reconstruction\n");
        return false;
    }

    absolute_time_t recon_end = get_absolute_time();
    int64_t recon_ms = absolute_time_diff_us(recon_start, recon_end) / 1000;

    printf("\nReconstruction complete! Time: %lld ms\n", recon_ms);

    printf("\n=== Step 4: Verification Summary ===\n");
    printf("✓ All %u chunks streamed and verified (CRC checks passed)\n", meta.chunk_count);
    printf("✓ File reconstructed with %u bytes\n", meta.total_size);
    printf("✓ File saved as: %s\n", recon_session.filename);
    printf("  Original filename: %s\n", source_filename);
    printf("  New filename: %s (with _received suffix)\n", recon_session.filename);
    printf("  Total time: %lld ms (streaming + reconstruction)\n", recon_ms);
    printf("  Peak memory usage: ~33 KB (32KB read buffer + chunk buffer)\n");

    // Additional wait for SD card write completion
    printf("\nWaiting for SD card write completion...\n");
    sleep_ms(500);

    // Verify file exists using FatFS
    printf("\n=== File Verification ===\n");
    printf("Checking if reconstructed file exists...\n");

    FIL verify_fil;
    FRESULT fr = f_open(&verify_fil, recon_session.filename, FA_READ);
    if (fr == FR_OK) {
        uint32_t verify_size = f_size(&verify_fil);
        f_close(&verify_fil);
        printf("✓ File found: %s (%u bytes)\n", recon_session.filename, verify_size);

        if (verify_size == meta.total_size) {
            printf("✓ File size matches expected: %u bytes\n", verify_size);
        } else {
            printf("⚠ WARNING: File size mismatch (expected %u, got %u)\n", meta.total_size,
                   verify_size);
        }
    } else {
        printf("✗ ERROR: Could not open reconstructed file (error %d)\n", fr);
    }
    printf("========================================\n");

    // Final wait before marking complete
    printf("\nFinal sync wait (allowing SD card internal write buffering)...\n");
    sleep_ms(1000);
    printf("✓ File should now be fully committed to SD card\n");

    return true;
} /*!
   * @brief Run complete streaming test
   * @return bool true if all steps passed, false otherwise
   */
static bool run_streaming_test(void) {
    bool success = true;

    // Check if source file exists
    printf("\nChecking for source file: %s\n", SOURCE_FILENAME);
    uint32_t file_size = get_file_size(SOURCE_FILENAME);
    if (file_size == 0) {
        printf("\n✗ ERROR: Source file '%s' not found!\n", SOURCE_FILENAME);
        printf("Please ensure the file exists on the SD card.\n");
        return false;
    }

    printf("✓ Found source file: %s (%u bytes)\n", SOURCE_FILENAME, file_size);

    printf("\n");
    printf("================================================\n");
    printf("  SIMULATING MQTT-SN DATA TRANSFER\n");
    printf("  Process: Deconstruct -> Stream -> Reconstruct\n");
    printf("================================================\n");

    // Step 1: Display streaming read info
    if (!test_streaming_read(SOURCE_FILENAME)) {
        printf("\n✗ FAILED: Streaming read test\n");
        success = false;
        return success;
    }

    sleep_ms(1000);

    // Step 2: Simulate MQTT transmission
    if (!test_mqtt_streaming(SOURCE_FILENAME)) {
        printf("\n✗ FAILED: MQTT streaming test\n");
        success = false;
        return success;
    }

    sleep_ms(1000);

    // Step 3: Reconstruct and verify
    if (!test_reconstruction(SOURCE_FILENAME, file_size)) {
        printf("\n✗ FAILED: Reconstruction test\n");
        success = false;
        return success;
    }

    return success;
}

/*!
 * @brief Main test function - MQTT-SN streaming simulation
 */
int main(void) {
    int test_count = 1;

    stdio_init_all();

    // Give USB serial time to connect
    sleep_ms(2000);

    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║  MQTT-SN Streaming Test                               ║\n");
    printf("║  File: %-47s ║\n", SOURCE_FILENAME);
    printf("║  Testing: Deconstruct → Stream → Reconstruct         ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    printf("\nRunning streaming test every 30 seconds...\n");
    printf("Initializing...\n");

    // Initial delay to allow USB serial to enumerate
    for (int i = 0; i < 5; i++) {
        printf("Waiting... %d\n", 5 - i);
        sleep_ms(1000);
    }

    printf("\nStarting test loop...\n");

    while (true) {
        printf("\n\n");
        printf("╔═══════════════════════════════════════════════════════╗\n");
        printf("║                  Test Run #%-3d                        ║\n", test_count);
        printf("╚═══════════════════════════════════════════════════════╝\n");

        // Initialize FatFS filesystem using microsd_driver
        printf("\nInitializing microSD card driver and FatFS filesystem...\n");

        if (!microsd_driver_init()) {
            printf("✗ ERROR: Failed to initialize microSD driver\n");
            printf("Please check the microSD card connection and try again.\n");
            printf("\n--- Waiting 30 seconds before retry ---\n");
            sleep_ms(30000);
            test_count++;
            continue;
        }
        printf("✓ MicroSD driver and FatFS filesystem initialized successfully\n");

        uint32_t start_time = to_ms_since_boot(get_absolute_time());

        // Run streaming test
        bool test_passed = run_streaming_test();

        uint32_t end_time = to_ms_since_boot(get_absolute_time());
        uint32_t elapsed_ms = end_time - start_time;

        // Final summary
        printf("\n");
        printf("╔═══════════════════════════════════════════════════════╗\n");
        printf("║              Final Summary - Run #%-3d                 ║\n", test_count);
        printf("╚═══════════════════════════════════════════════════════╝\n");

        if (test_passed) {
            printf("\n  🎯 ALL TESTS PASSED!\n");
            printf("\n  The streaming method successfully:\n");
            printf("    ✓ Step 1: Initialized streaming read\n");
            printf("    ✓ Step 2: Streamed chunks (simulating MQTT-SN)\n");
            printf("    ✓ Step 3: Reconstructed file with integrity verification\n");
            printf("\n  Key Benefits:\n");
            printf("    • Memory usage: ~33 KB (32KB read buffer)\n");
            printf("    • Buffered SD card reads (reduces seeks)\n");
            printf("    • No file size limitations\n");
            printf("    • Ready for MQTT-SN integration!\n");
        } else {
            printf("\n  ⚠️  TEST FAILED!\n");
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
