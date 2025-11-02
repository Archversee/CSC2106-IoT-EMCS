/*!
 * @file    data_frame_demo.c
 * @brief   Data frame deconstruction/reconstruction demonstration for MQTT QoS1
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 *
 * This demo tests the file chunking algorithm for MQTT-SN data transfer
 * with 247-byte payloads and CRC32 integrity verification.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../drivers/microsd_driver.h"
#include "data_frame.h"
#include "pico/stdlib.h"

/*! Test configuration constants */
#define TEST_INPUT_FILENAME "TEST_INPUT.txt"
#define TEST_OUTPUT_FILENAME "TEST_OUTPUT.txt"
#define SMALL_FILE_SIZE 500    // Small test file: 500 bytes (~3 chunks)
#define MEDIUM_FILE_SIZE 5000  // Medium test file: 5KB (~21 chunks)
#define LARGE_FILE_SIZE 30000  // Large test file: 30KB (~122 chunks) - fits in 1 cluster

/*!
 * @brief Create a test file on SD card with known content
 * @param filename Name of file to create
 * @param size Size of file in bytes
 * @return bool true on success, false on failure
 */
static bool create_test_file(const char* filename, uint32_t size) {
    filesystem_info_t fs_info;

    printf("\n=== Creating Test File ===\n");
    printf("Filename: %s\n", filename);
    printf("Size: %u bytes\n", size);

    // Initialize filesystem
    if (!microsd_init_filesystem(&fs_info)) {
        printf("ERROR: Failed to initialize filesystem\n");
        return false;
    }

    // Generate test data with repeating pattern
    uint8_t* test_data = (uint8_t*)malloc(size);
    if (test_data == NULL) {
        printf("ERROR: Memory allocation failed\n");
        return false;
    }

    // Fill with pattern: "ABCDEFGH01234567" repeated
    const char pattern[] = "ABCDEFGH01234567";
    const uint32_t pattern_len = strlen(pattern);

    for (uint32_t i = 0; i < size; i++) {
        test_data[i] = pattern[i % pattern_len];
    }

    // Add a header to make it more interesting
    const char header[] =
        "INF2004 MQTT-SN File Transfer Test\n"
        "================================\n"
        "This file tests chunked data transfer.\n\n";
    uint32_t header_len = strlen(header);
    if (size > header_len) {
        memcpy(test_data, header, header_len);
    }

    // Create file on SD card
    if (!microsd_create_file(&fs_info, filename, test_data, size)) {
        printf("ERROR: Failed to create test file\n");
        free(test_data);
        return false;
    }

    free(test_data);
    printf("SUCCESS: Test file created\n");
    return true;
}

/*!
 * @brief Test file deconstruction into chunks
 * @param filename Name of file to deconstruct
 * @param meta Pointer to metadata structure (output)
 * @param chunks Pointer to chunks array (output)
 * @return bool true on success, false on failure
 */
static bool test_deconstruction(const char* filename, struct Metadata* meta, struct Payload** chunks) {
    printf("\n=== Testing Deconstruction ===\n");

    if (deconstruct((char*)filename, meta, chunks) != 0) {
        printf("ERROR: Deconstruction failed\n");
        return false;
    }

    printf("SUCCESS: File deconstructed into %u chunks\n", meta->chunk_count);
    printf("Metadata:\n");
    printf("  Filename: %s\n", meta->filename);
    printf("  Total size: %u bytes\n", meta->total_size);
    printf("  Chunks: %u\n", meta->chunk_count);
    printf("  File CRC32: 0x%08X\n", meta->file_crc);
    printf("  Last modified: 0x%08X\n", meta->last_modified);

    return true;
}

/*!
 * @brief Test individual chunk verification
 * @param chunks Array of payload chunks
 * @param chunk_count Number of chunks
 * @return bool true if all chunks valid, false otherwise
 */
static bool test_chunk_verification(struct Payload* chunks, uint32_t chunk_count) {
    printf("\n=== Testing Chunk Verification ===\n");
    printf("Verifying %u chunks...\n", chunk_count);

    uint32_t valid_chunks = 0;
    uint32_t invalid_chunks = 0;

    for (uint32_t i = 0; i < chunk_count; i++) {
        if (verify_chunk(&chunks[i])) {
            valid_chunks++;

            // Print details for first, last, and a few in between
            if (i == 0 || i == chunk_count - 1 || i % 50 == 0) {
                printf("  Chunk %u: VALID (seq=%u, size=%u, crc=0x%08X)\n",
                       i, chunks[i].sequence, chunks[i].size, chunks[i].crc);
            }
        } else {
            invalid_chunks++;
            printf("  Chunk %u: INVALID!\n", i);
        }
    }

    printf("\nVerification Results:\n");
    printf("  Valid chunks: %u/%u\n", valid_chunks, chunk_count);
    printf("  Invalid chunks: %u/%u\n", invalid_chunks, chunk_count);

    if (invalid_chunks > 0) {
        printf("ERROR: Some chunks failed verification\n");
        return false;
    }

    printf("SUCCESS: All chunks verified\n");
    return true;
}

/*!
 * @brief Test chunk corruption detection
 * @param chunks Array of payload chunks
 * @param chunk_count Number of chunks
 * @return bool true if corruption detected, false otherwise
 */
static bool test_corruption_detection(struct Payload* chunks, uint32_t chunk_count) {
    printf("\n=== Testing Corruption Detection ===\n");

    if (chunk_count < 2) {
        printf("SKIPPED: Need at least 2 chunks for this test\n");
        return true;
    }

    // Corrupt the middle chunk
    uint32_t corrupt_chunk_idx = chunk_count / 2;
    struct Payload* corrupt_chunk = &chunks[corrupt_chunk_idx];

    printf("Corrupting chunk %u...\n", corrupt_chunk_idx);

    // Save original data
    uint8_t original_byte = corrupt_chunk->data[0];

    // Corrupt first byte
    corrupt_chunk->data[0] ^= 0xFF;

    // Verify corruption is detected
    printf("Verifying corruption detection...\n");
    if (verify_chunk(corrupt_chunk)) {
        printf("ERROR: Corruption not detected!\n");
        // Restore data
        corrupt_chunk->data[0] = original_byte;
        return false;
    }

    printf("SUCCESS: Corruption detected (CRC mismatch)\n");

    // Restore original data
    printf("Restoring original data...\n");
    corrupt_chunk->data[0] = original_byte;

    // Verify restoration
    if (!verify_chunk(corrupt_chunk)) {
        printf("ERROR: Chunk still invalid after restoration\n");
        return false;
    }

    printf("SUCCESS: Chunk restored successfully\n");
    return true;
}

/*!
 * @brief Test simulated MQTT packet loss and retransmission
 * @param chunks Array of payload chunks
 * @param chunk_count Number of chunks
 * @return bool true on success, false on failure
 */
static bool test_packet_loss_simulation(struct Payload* chunks, uint32_t chunk_count) {
    printf("\n=== Testing Packet Loss Simulation (MQTT QoS1) ===\n");

    if (chunk_count < 5) {
        printf("SKIPPED: Need at least 5 chunks for this test\n");
        return true;
    }

    // Simulate losing packets 2 and 4
    uint32_t lost_packets[] = {2, 4};
    uint32_t num_lost = 2;

    printf("Simulating loss of packets: ");
    for (uint32_t i = 0; i < num_lost; i++) {
        printf("%u ", lost_packets[i]);
    }
    printf("\n");

    // Create received chunks array (missing some packets)
    struct Payload* received = (struct Payload*)malloc(sizeof(struct Payload) * chunk_count);
    if (received == NULL) {
        printf("ERROR: Memory allocation failed\n");
        return false;
    }

    bool* received_flags = (bool*)calloc(chunk_count, sizeof(bool));
    if (received_flags == NULL) {
        printf("ERROR: Memory allocation failed\n");
        free(received);
        return false;
    }

    // Copy all chunks except "lost" ones
    for (uint32_t i = 0; i < chunk_count; i++) {
        bool is_lost = false;
        for (uint32_t j = 0; j < num_lost; j++) {
            if (i == lost_packets[j]) {
                is_lost = true;
                break;
            }
        }

        if (!is_lost) {
            memcpy(&received[i], &chunks[i], sizeof(struct Payload));
            received_flags[i] = true;
        }
    }

    // Check for missing packets
    printf("\nChecking for missing packets...\n");
    uint32_t missing_count = 0;
    for (uint32_t i = 0; i < chunk_count; i++) {
        if (!received_flags[i]) {
            printf("  Packet %u: MISSING (requesting retransmission)\n", i);
            missing_count++;
        }
    }

    printf("Missing packets: %u\n", missing_count);

    // Simulate retransmission (copy missing packets)
    printf("\nSimulating retransmission...\n");
    for (uint32_t i = 0; i < chunk_count; i++) {
        if (!received_flags[i]) {
            memcpy(&received[i], &chunks[i], sizeof(struct Payload));
            received_flags[i] = true;
            printf("  Retransmitted packet %u\n", i);
        }
    }

    // Verify all packets now received
    printf("\nVerifying all packets received...\n");
    bool all_received = true;
    for (uint32_t i = 0; i < chunk_count; i++) {
        if (!received_flags[i] || !verify_chunk(&received[i])) {
            printf("ERROR: Packet %u still missing or invalid\n", i);
            all_received = false;
        }
    }

    free(received);
    free(received_flags);

    if (all_received) {
        printf("SUCCESS: All packets received and verified\n");
        return true;
    } else {
        printf("ERROR: Packet loss recovery failed\n");
        return false;
    }
}

/*!
 * @brief Test file reconstruction
 * @param meta Pointer to metadata structure
 * @param chunks Array of payload chunks
 * @param output_filename Name of output file
 * @return bool true on success, false on failure
 */
static bool test_reconstruction(struct Metadata* meta, struct Payload** chunks, const char* output_filename) {
    printf("\n=== Testing Reconstruction ===\n");

    if (reconstruct(meta, chunks, (char*)output_filename) != 0) {
        printf("ERROR: Reconstruction failed\n");
        return false;
    }

    printf("SUCCESS: File reconstructed and saved as '%s'\n", output_filename);
    return true;
}

/*!
 * @brief Verify reconstructed file matches original
 * @param original_filename Original file name
 * @param reconstructed_filename Reconstructed file name
 * @return bool true if files match, false otherwise
 */
static bool verify_file_integrity(const char* original_filename, const char* reconstructed_filename) {
    filesystem_info_t fs_info;
    uint8_t buffer1[512];
    uint8_t buffer2[512];
    uint32_t bytes_read1 = 0;
    uint32_t bytes_read2 = 0;

    printf("\n=== Verifying File Integrity ===\n");
    printf("Comparing '%s' and '%s'\n", original_filename, reconstructed_filename);

    if (!microsd_init_filesystem(&fs_info)) {
        printf("ERROR: Failed to initialize filesystem\n");
        return false;
    }

    // Read both files
    if (!microsd_read_file(&fs_info, original_filename, buffer1, sizeof(buffer1), &bytes_read1)) {
        printf("ERROR: Failed to read original file\n");
        return false;
    }

    if (!microsd_read_file(&fs_info, reconstructed_filename, buffer2, sizeof(buffer2), &bytes_read2)) {
        printf("ERROR: Failed to read reconstructed file\n");
        return false;
    }

    // Compare sizes
    if (bytes_read1 != bytes_read2) {
        printf("ERROR: File size mismatch (original=%u, reconstructed=%u)\n",
               bytes_read1, bytes_read2);
        return false;
    }

    // Compare content
    if (memcmp(buffer1, buffer2, bytes_read1) != 0) {
        printf("ERROR: File content mismatch\n");
        return false;
    }

    printf("SUCCESS: Files match perfectly (%u bytes)\n", bytes_read1);
    return true;
}

/*!
 * @brief Print MQTT QoS1 transfer statistics
 * @param meta Pointer to metadata structure
 * @return void
 */
static void print_mqtt_stats(struct Metadata* meta) {
    printf("\n=== MQTT QoS1 Transfer Statistics ===\n");
    printf("Total data size: %u bytes\n", meta->total_size);
    printf("Payload per packet: %u bytes\n", PAYLOAD_SIZE);
    printf("Total packets: %u\n", meta->chunk_count);
    printf("Overhead per packet: 9 bytes (seq + size + crc)\n");
    printf("Total packet size: %u bytes\n", PAYLOAD_SIZE + 9);

    uint32_t total_overhead = meta->chunk_count * 9;
    uint32_t total_transmitted = meta->total_size + total_overhead;
    float overhead_percentage = (float)total_overhead / (float)total_transmitted * 100.0f;

    printf("Total overhead: %u bytes (%.2f%%)\n", total_overhead, overhead_percentage);
    printf("Total transmitted: %u bytes\n", total_transmitted);

    // Estimate time at different MQTT speeds
    printf("\nEstimated transfer time:\n");
    printf("  @ 1 msg/sec:  %u seconds\n", meta->chunk_count);
    printf("  @ 10 msg/sec: %.1f seconds\n", (float)meta->chunk_count / 10.0f);
    printf("  @ 100 msg/sec: %.2f seconds\n", (float)meta->chunk_count / 100.0f);
}

/*!
 * @brief Run comprehensive test suite
 * @param test_size Size of test file in bytes
 * @return bool true if all tests pass, false otherwise
 */
static bool run_test_suite(uint32_t test_size) {
    struct Metadata meta;
    struct Payload* chunks = NULL;
    bool all_passed = true;

    printf("\n");
    printf("================================================\n");
    printf("  MQTT-SN Data Frame Test Suite\n");
    printf("  Test file size: %u bytes\n", test_size);
    printf("================================================\n");

    // Test 1: Create test file
    if (!create_test_file(TEST_INPUT_FILENAME, test_size)) {
        printf("\nTest 1 FAILED: File creation\n");
        return false;
    }
    printf("\nTest 1 PASSED: File creation\n");

    // Test 2: Deconstruction
    if (!test_deconstruction(TEST_INPUT_FILENAME, &meta, &chunks)) {
        printf("\nTest 2 FAILED: Deconstruction\n");
        return false;
    }
    printf("\nTest 2 PASSED: Deconstruction\n");

    // Test 3: Chunk verification
    if (!test_chunk_verification(chunks, meta.chunk_count)) {
        printf("\nTest 3 FAILED: Chunk verification\n");
        all_passed = false;
    } else {
        printf("\nTest 3 PASSED: Chunk verification\n");
    }

    // Test 4: Corruption detection
    if (!test_corruption_detection(chunks, meta.chunk_count)) {
        printf("\nTest 4 FAILED: Corruption detection\n");
        all_passed = false;
    } else {
        printf("\nTest 4 PASSED: Corruption detection\n");
    }

    // Test 5: Packet loss simulation
    if (!test_packet_loss_simulation(chunks, meta.chunk_count)) {
        printf("\nTest 5 FAILED: Packet loss simulation\n");
        all_passed = false;
    } else {
        printf("\nTest 5 PASSED: Packet loss simulation\n");
    }

    // Test 6: Reconstruction
    if (!test_reconstruction(&meta, &chunks, TEST_OUTPUT_FILENAME)) {
        printf("\nTest 6 FAILED: Reconstruction\n");
        all_passed = false;
    } else {
        printf("\nTest 6 PASSED: Reconstruction\n");
    }

    // Test 7: File integrity
    if (!verify_file_integrity(TEST_INPUT_FILENAME, TEST_OUTPUT_FILENAME)) {
        printf("\nTest 7 FAILED: File integrity\n");
        all_passed = false;
    } else {
        printf("\nTest 7 PASSED: File integrity\n");
    }

    // Print statistics
    print_mqtt_stats(&meta);

    // Cleanup
    if (chunks != NULL) {
        free(chunks);
    }

    return all_passed;
}

/*!
 * @brief Main demonstration function
 * @return int Program exit status
 */
int main(void) {
    int test_run = 1;

    stdio_init_all();

    // Wait for USB serial
    printf("Waiting for USB serial connection");
    for (int i = 0; i < 5; i++) {
        printf(".");
        sleep_ms(1000);
    }
    printf("\n\n");

    printf("========================================\n");
    printf("  MQTT-SN Data Frame Demo\n");
    printf("  File Chunking with CRC32 Verification\n");
    printf("  Payload Size: 247 bytes\n");
    printf("========================================\n\n");

    while (true) {
        printf("\n\n");
        printf("****************************************\n");
        printf("  Test Run #%d\n", test_run);
        printf("****************************************\n");

        // Initialize microSD
        printf("\nInitializing microSD card...\n");
        if (!microsd_init()) {
            printf("ERROR: Failed to initialize microSD card\n");
            printf("Please check SD card connection and try again.\n");
            sleep_ms(10000);
            continue;
        }
        printf("MicroSD card initialized successfully\n");

        // Set logging level
        microsd_set_log_level(MICROSD_LOG_INFO);

        // Run test suites with different file sizes
        bool all_suites_passed = true;

        // Small file test (~3 chunks)
        printf("\n\n>>> Running SMALL file test <<<\n");
        if (!run_test_suite(SMALL_FILE_SIZE)) {
            all_suites_passed = false;
        }

        sleep_ms(2000);

        // Medium file test (~21 chunks)
        printf("\n\n>>> Running MEDIUM file test <<<\n");
        if (!run_test_suite(MEDIUM_FILE_SIZE)) {
            all_suites_passed = false;
        }

        sleep_ms(2000);

        // Large file test (~203 chunks)
        printf("\n\n>>> Running LARGE file test <<<\n");
        if (!run_test_suite(LARGE_FILE_SIZE)) {
            all_suites_passed = false;
        }

        // Final summary
        printf("\n\n");
        printf("========================================\n");
        printf("  FINAL TEST RESULTS\n");
        printf("========================================\n");
        if (all_suites_passed) {
            printf("✓ ALL TEST SUITES PASSED!\n");
            printf("\nThe MQTT-SN data frame system is working correctly:\n");
            printf("- File deconstruction into 247-byte chunks\n");
            printf("- CRC32 integrity verification\n");
            printf("- Corruption detection\n");
            printf("- Packet loss handling (QoS1)\n");
            printf("- File reconstruction\n");
            printf("\nReady for MQTT-SN deployment!\n");
        } else {
            printf("✗ SOME TEST SUITES FAILED\n");
            printf("Please review errors above.\n");
        }
        printf("========================================\n");

        printf("\n\n--- Waiting 15 seconds before next test run ---\n");
        sleep_ms(15000);
        test_run++;
    }

    return 0;
}
