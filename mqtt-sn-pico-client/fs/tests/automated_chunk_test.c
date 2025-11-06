/*!
 * @file    automated_chunk_test.c
 * @brief   Automated unit tests for data_frame serialization module
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 *
 * Test Execution Counts (from traceability matrix):
 * - UT-1 (Serialize):         100 executions
 * - UT-2 (Deserialize):       350 executions
 * - UT-3 (CRC verification):   40 executions
 * - UT-4 (Metadata serialize):100 executions
 * - UT-5 (Metadata deser):     80 executions
 * - IT-2 (Seq validation):     45 executions
 *
 * Total: 715 test executions
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../data_frame.h"
#include "pico/stdlib.h"

/*! Test configuration */
#define TEST_SERIALIZE_EXECUTIONS 100
#define TEST_DESERIALIZE_EXECUTIONS 350
#define TEST_CRC_EXECUTIONS 40
#define TEST_META_SERIALIZE_EXECUTIONS 100
#define TEST_META_DESERIALIZE_EXECUTIONS 80
#define TEST_SEQUENCE_VALIDATION_EXECUTIONS 45

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
    // Only print every 10th iteration for high-count tests to reduce output
    if (iteration == 1 || iteration == total || iteration % 10 == 0) {
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
 * @brief Test UT-1: Serialize Payload
 * Target: 100 executions
 */
static void test_serialize_payload(void) {
    printf("\n" COLOR_YELLOW "=== TEST UT-1: Serialize Payload ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_SERIALIZE_EXECUTIONS);

    for (uint32_t i = 1; i <= TEST_SERIALIZE_EXECUTIONS; i++) {
        print_test_header("UT-1 Serialize", i, TEST_SERIALIZE_EXECUTIONS);

        // Create test payload
        struct Payload payload;
        payload.sequence = i;

        // Fill with test pattern (all 239 bytes)
        for (size_t j = 0; j < PAYLOAD_DATA_SIZE; j++) {
            payload.data[j] = (uint8_t)((i + j) & 0xFF);
        }

        // Serialize
        static uint8_t buffer[PAYLOAD_SIZE];
        memset(buffer, 0, sizeof(buffer));

        int result = serialize_payload(&payload, buffer);
        if (result < 0) {
            print_result(false, "Serialization failed", true);
            continue;
        }

        // Verify buffer has data
        bool has_data = false;
        for (size_t j = 0; j < PAYLOAD_SIZE; j++) {
            if (buffer[j] != 0) {
                has_data = true;
                break;
            }
        }

        if (!has_data) {
            print_result(false, "Serialized buffer is empty", true);
            continue;
        }

        // Verify sequence number in buffer (little-endian, first 4 bytes)
        uint32_t serialized_seq =
            buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);

        if (serialized_seq != i) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Sequence mismatch: expected %lu, got %lu", (unsigned long)i,
                     (unsigned long)serialized_seq);
            print_result(false, msg, true);
            continue;
        }

        print_result(true, "Payload serialized successfully", false);
    }
    printf("\n");
}

/*!
 * @brief Test UT-2: Deserialize Payload
 * Target: 350 executions
 */
static void test_deserialize_payload(void) {
    printf("\n" COLOR_YELLOW "=== TEST UT-2: Deserialize Payload ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_DESERIALIZE_EXECUTIONS);

    for (uint32_t i = 1; i <= TEST_DESERIALIZE_EXECUTIONS; i++) {
        print_test_header("UT-2 Deserialize", i, TEST_DESERIALIZE_EXECUTIONS);

        // Create original payload
        struct Payload original;
        original.sequence = i;

        // Fill with test pattern (all 239 bytes)
        for (size_t j = 0; j < PAYLOAD_DATA_SIZE; j++) {
            original.data[j] = (uint8_t)((i * 3 + j) & 0xFF);
        }

        // Serialize
        static uint8_t buffer[PAYLOAD_SIZE];
        if (serialize_payload(&original, buffer) < 0) {
            print_result(false, "Serialization step failed", true);
            continue;
        }

        // Deserialize
        struct Payload deserialized;
        memset(&deserialized, 0, sizeof(deserialized));

        if (deserialize_payload(buffer, &deserialized) < 0) {
            print_result(false, "Deserialization failed", true);
            continue;
        }

        // Verify sequence number
        if (deserialized.sequence != original.sequence) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Sequence mismatch: expected %lu, got %lu",
                     (unsigned long)original.sequence, (unsigned long)deserialized.sequence);
            print_result(false, msg, true);
            continue;
        }

        // Verify data (all 239 bytes)
        bool data_match = (memcmp(original.data, deserialized.data, PAYLOAD_DATA_SIZE) == 0);
        if (!data_match) {
            print_result(false, "Data mismatch after deserialization", true);
            continue;
        }

        print_result(true, "Payload deserialized successfully", false);
    }
    printf("\n");
}

/*!
 * @brief Test UT-3: CRC Verification
 * Target: 40 executions
 */
static void test_crc_verification(void) {
    printf("\n" COLOR_YELLOW "=== TEST UT-3: CRC Verification ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_CRC_EXECUTIONS);

    for (uint32_t i = 1; i <= TEST_CRC_EXECUTIONS; i++) {
        print_test_header("UT-3 CRC", i, TEST_CRC_EXECUTIONS);

        // Create payload with varying pattern
        struct Payload payload;
        payload.sequence = i * 100;

        // Fill with test pattern (all 239 bytes)
        for (size_t j = 0; j < PAYLOAD_DATA_SIZE; j++) {
            payload.data[j] = (uint8_t)((i * 5 + j * 3) & 0xFF);
        }

        // Calculate CRC for the payload data (THIS IS CRITICAL!)
        payload.crc32 = crc32(payload.data, PAYLOAD_DATA_SIZE);

        // Serialize (writes the calculated CRC to buffer)
        static uint8_t buffer[PAYLOAD_SIZE];
        static uint8_t buffer_corrupted[PAYLOAD_SIZE];
        if (serialize_payload(&payload, buffer) < 0) {
            print_result(false, "Serialization failed", true);
            continue;
        }

        // Deserialize the valid data
        struct Payload deserialized;
        if (deserialize_payload(buffer, &deserialized) < 0) {
            print_result(false, "Deserialization failed", true);
            continue;
        }

        // First test: verify good data passes CRC check using verify_chunk
        if (verify_chunk(&deserialized) != 1) {
            print_result(false, "Valid data failed CRC verification", true);
            continue;
        }

        // Second test: Create corrupted payload and verify it fails CRC
        struct Payload corrupted;
        memcpy(&corrupted, &deserialized, sizeof(struct Payload));

        // Corrupt the data (not the CRC field itself)
        corrupted.data[0] ^= 0xFF;
        corrupted.data[10] ^= 0xAA;
        corrupted.data[20] ^= 0x55;
        // Keep the original CRC unchanged so verify_chunk will detect mismatch

        // Verify corrupted data fails CRC check
        if (verify_chunk(&corrupted) != 0) {
            print_result(false, "CRC failed to detect data corruption", true);
            continue;
        }

        char msg[64];
        snprintf(msg, sizeof(msg), "CRC verified (seq=%lu, %d bytes)",
                 (unsigned long)payload.sequence, PAYLOAD_DATA_SIZE);
        print_result(true, msg, i == TEST_CRC_EXECUTIONS); // Verbose on last
    }
    printf("\n");
}

/*!
 * @brief Test UT-4: Serialize Metadata
 * Target: 100 executions
 */
static void test_serialize_metadata(void) {
    printf("\n" COLOR_YELLOW "=== TEST UT-4: Serialize Metadata ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_META_SERIALIZE_EXECUTIONS);

    for (uint32_t i = 1; i <= TEST_META_SERIALIZE_EXECUTIONS; i++) {
        print_test_header("UT-4 Meta Serialize", i, TEST_META_SERIALIZE_EXECUTIONS);

        // Create test metadata
        struct Metadata metadata;
        snprintf(metadata.session_id, sizeof(metadata.session_id), "SESSION_%03lu",
                 (unsigned long)i);
        snprintf(metadata.filename, sizeof(metadata.filename), "TEST_%03lu.DAT", (unsigned long)i);
        metadata.total_size = 1000 + (i * 100);
        metadata.chunk_count = (metadata.total_size + PAYLOAD_DATA_SIZE - 1) / PAYLOAD_DATA_SIZE;
        metadata.last_modified = 1730000000 + i;
        metadata.file_crc32 = (uint32_t)(i & 0xFFFFFFFF);

        // Serialize
        static uint8_t buffer[PAYLOAD_SIZE];
        memset(buffer, 0, sizeof(buffer));

        int result = serialize_metadata(&metadata, buffer);
        if (result < 0) {
            print_result(false, "Metadata serialization failed", true);
            continue;
        }

        // Verify buffer has data
        bool has_data = false;
        for (size_t j = 0; j < PAYLOAD_SIZE; j++) {
            if (buffer[j] != 0) {
                has_data = true;
                break;
            }
        }

        if (!has_data) {
            print_result(false, "Serialized metadata buffer is empty", true);
            continue;
        }

        print_result(true, "Metadata serialized successfully", false);
    }
    printf("\n");
}

/*!
 * @brief Test UT-5: Deserialize Metadata
 * Target: 80 executions
 */
static void test_deserialize_metadata(void) {
    printf("\n" COLOR_YELLOW "=== TEST UT-5: Deserialize Metadata ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_META_DESERIALIZE_EXECUTIONS);

    for (uint32_t i = 1; i <= TEST_META_DESERIALIZE_EXECUTIONS; i++) {
        print_test_header("UT-5 Meta Deserialize", i, TEST_META_DESERIALIZE_EXECUTIONS);

        // Create original metadata
        struct Metadata original;
        snprintf(original.session_id, sizeof(original.session_id), "SESS_%03lu", (unsigned long)i);
        snprintf(original.filename, sizeof(original.filename), "FILE_%03lu.BIN", (unsigned long)i);
        original.total_size = 5000 + (i * 50);
        original.chunk_count = ((original.total_size + PAYLOAD_DATA_SIZE - 1) / PAYLOAD_DATA_SIZE);
        original.last_modified = 1730500000 + i;
        original.file_crc32 = (uint32_t)((i * 3) & 0xFFFFFFFF);

        // Serialize
        static uint8_t buffer[PAYLOAD_SIZE];
        if (serialize_metadata(&original, buffer) < 0) {
            print_result(false, "Serialization step failed", true);
            continue;
        }

        // Deserialize
        struct Metadata deserialized;
        memset(&deserialized, 0, sizeof(deserialized));

        if (deserialize_metadata(buffer, &deserialized) < 0) {
            print_result(false, "Metadata deserialization failed", true);
            continue;
        }

        // Verify session_id
        if (strcmp(deserialized.session_id, original.session_id) != 0) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Session ID mismatch: expected %s, got %s",
                     original.session_id, deserialized.session_id);
            print_result(false, msg, true);
            continue;
        }

        // Verify filename
        if (strcmp(deserialized.filename, original.filename) != 0) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Filename mismatch: expected %s, got %s", original.filename,
                     deserialized.filename);
            print_result(false, msg, true);
            continue;
        }

        // Verify total_size
        if (deserialized.total_size != original.total_size) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Size mismatch: expected %lu, got %lu",
                     (unsigned long)original.total_size, (unsigned long)deserialized.total_size);
            print_result(false, msg, true);
            continue;
        }

        print_result(true, "Metadata deserialized successfully", false);
    }
    printf("\n");
}

/*!
 * @brief Test IT-2: Sequence Number Validation
 * Target: 45 executions
 */
static void test_sequence_validation(void) {
    printf("\n" COLOR_YELLOW "=== TEST IT-2: Sequence Validation ===" COLOR_RESET "\n");
    printf("Target executions: %d\n", TEST_SEQUENCE_VALIDATION_EXECUTIONS);

    uint32_t expected_seq = 0;

    for (uint32_t i = 1; i <= TEST_SEQUENCE_VALIDATION_EXECUTIONS; i++) {
        print_test_header("IT-2 Sequence", i, TEST_SEQUENCE_VALIDATION_EXECUTIONS);

        // Create payload with incrementing sequence
        struct Payload payload;
        payload.sequence = expected_seq;

        // Fill with test pattern (all 239 bytes)
        for (size_t j = 0; j < PAYLOAD_DATA_SIZE; j++) {
            payload.data[j] = (uint8_t)((payload.sequence + j) & 0xFF);
        }

        // Serialize and deserialize
        static uint8_t buffer[PAYLOAD_SIZE];
        if (serialize_payload(&payload, buffer) < 0) {
            print_result(false, "Serialization failed", true);
            continue;
        }

        struct Payload received;
        if (deserialize_payload(buffer, &received) < 0) {
            print_result(false, "Deserialization failed", true);
            continue;
        }

        // Validate sequence number matches expected
        if (received.sequence != expected_seq) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Out-of-order sequence: expected %lu, got %lu",
                     (unsigned long)expected_seq, (unsigned long)received.sequence);
            print_result(false, msg, true);
            continue;
        }

        // Increment expected sequence
        expected_seq++;

        print_result(true, "Sequence number validated", false);
    }
    printf("\n");
}

/*!
 * @brief Print final test summary
 */
static void print_summary(void) {
    printf("\n");
    printf("================================================================================\n");
    printf(COLOR_YELLOW "                 AUTOMATED DATA FRAME TEST SUMMARY" COLOR_RESET "\n");
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
    printf("  UT-1 (Serialize):         %d executions\n", TEST_SERIALIZE_EXECUTIONS);
    printf("  UT-2 (Deserialize):       %d executions\n", TEST_DESERIALIZE_EXECUTIONS);
    printf("  UT-3 (CRC verification):  %d executions\n", TEST_CRC_EXECUTIONS);
    printf("  UT-4 (Meta serialize):    %d executions\n", TEST_META_SERIALIZE_EXECUTIONS);
    printf("  UT-5 (Meta deserialize):  %d executions\n", TEST_META_DESERIALIZE_EXECUTIONS);
    printf("  IT-2 (Seq validation):    %d executions\n", TEST_SEQUENCE_VALIDATION_EXECUTIONS);
    printf("  ─────────────────────────────────────\n");
    printf("  Total:                    %d executions\n",
           TEST_SERIALIZE_EXECUTIONS + TEST_DESERIALIZE_EXECUTIONS + TEST_CRC_EXECUTIONS +
               TEST_META_SERIALIZE_EXECUTIONS + TEST_META_DESERIALIZE_EXECUTIONS +
               TEST_SEQUENCE_VALIDATION_EXECUTIONS);

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
    // Initialize stdio first
    stdio_init_all();

    // Wait longer for USB serial connection to establish
    sleep_ms(6000);

    // Send initial marker to verify USB is working
    printf("STARTING...\n");
    sleep_ms(100);

    printf("\n\n");
    printf("================================================================================\n");
    printf("              AUTOMATED DATA FRAME UNIT TESTS\n");
    printf("================================================================================\n");
    printf("Test Suite: Data Frame Serialization & CRC Verification\n");
    printf("Based on: Core Traceability Matrix Requirements\n");
    printf("Date: 3 November 2025\n");
    printf("================================================================================\n");

    // Quick sanity check
    printf("\nPerforming sanity check...\n");
    struct Payload test_payload;
    test_payload.sequence = 1;

    // Fill with simple pattern
    for (int i = 0; i < PAYLOAD_DATA_SIZE; i++) {
        test_payload.data[i] = (uint8_t)(i & 0xFF);
    }

    uint8_t test_buffer[PAYLOAD_SIZE];
    int result = serialize_payload(&test_payload, test_buffer);
    printf("Sanity check: serialize_payload returned %d\n", result);

    if (result < 0) {
        printf(COLOR_RED "ERROR: Sanity check failed! Cannot proceed.\n" COLOR_RESET);
        printf("serialize_payload is not working correctly.\n");
        while (true) {
            sleep_ms(1000);
        }
    }
    printf(COLOR_GREEN "Sanity check passed!\n" COLOR_RESET);
    printf("\n");

    // Run all test suites
    test_serialize_payload();
    test_deserialize_payload();
    test_crc_verification();
    test_serialize_metadata();
    test_deserialize_metadata();
    test_sequence_validation();

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
