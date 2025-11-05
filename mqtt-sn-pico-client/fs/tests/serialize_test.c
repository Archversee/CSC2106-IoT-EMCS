/*!
 * @file    serialize_test.c
 * @brief   Test serialization/deserialization for MQTT QoS1 data transfer
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 *
 * This test demonstrates and validates the serialize/deserialize functions
 * for Payload and Metadata structs used in MQTT-SN file transfer.
 */

#include <stdio.h>
#include <string.h>

#include "data_frame.h"
#include "pico/stdlib.h"

/*!
 * @brief Test Payload serialization and deserialization
 */
static bool test_payload_serialization(void) {
    printf("\n=== Testing Payload Serialization ===\n");

    // Create test payload
    struct Payload original_payload;
    original_payload.sequence = 42;

    // Fill data with test pattern
    for (int i = 0; i < PAYLOAD_DATA_SIZE; i++) {
        original_payload.data[i] = (uint8_t)(i & 0xFF);
    }

    // Calculate CRC
    original_payload.crc = crc16((const char*)original_payload.data, PAYLOAD_DATA_SIZE);

    printf("Original Payload:\n");
    printf("  Sequence: %lu\n", (unsigned long)original_payload.sequence);
    printf("  Size: %lu bytes\n", (unsigned long)PAYLOAD_DATA_SIZE);
    printf("  CRC: 0x%04X\n", original_payload.crc);

    // Serialize
    uint8_t buffer[PAYLOAD_SIZE];
    int serialized_bytes = serialize_payload(&original_payload, buffer);

    // print the buffer to show serialized data
    // Space out the bytes to show each section
    printf("Serialized Buffer:\n");

    for (int i = 0; i < PAYLOAD_SIZE; i++) {
        printf("%02X ", buffer[i]);
        // Add spacing for readability
        if (i == 3 || i == 240 || i == 244) {
            printf("\n");
        }
    }

    printf("\n");

    if (serialized_bytes != PAYLOAD_SIZE) {
        printf("✗ FAILED: Serialization returned %d bytes (expected %d)\n",
               serialized_bytes, PAYLOAD_SIZE);
        return false;
    }

    printf("✓ Serialized to %d bytes\n", serialized_bytes);

    // Deserialize
    struct Payload deserialized_payload;
    int result = deserialize_payload(buffer, &deserialized_payload);

    if (result != 0) {
        printf("✗ FAILED: Deserialization failed\n");
        return false;
    }

    printf("Deserialized Payload:\n");
    printf("  Sequence: %lu\n", (unsigned long)deserialized_payload.sequence);
    printf("  Size: %lu bytes\n", (unsigned long)PAYLOAD_DATA_SIZE);
    printf("  CRC: 0x%04X\n", deserialized_payload.crc);

    // Verify fields match
    bool fields_match = true;

    if (original_payload.sequence != deserialized_payload.sequence) {
        printf("✗ FAILED: Sequence mismatch (%lu != %lu)\n",
               (unsigned long)original_payload.sequence,
               (unsigned long)deserialized_payload.sequence);
        fields_match = false;
    }

    if (original_payload.crc != deserialized_payload.crc) {
        printf("✗ FAILED: CRC mismatch (0x%04X != 0x%04X)\n",
               original_payload.crc, deserialized_payload.crc);
        fields_match = false;
    }

    // Verify data matches
    bool data_match = true;
    for (int i = 0; i < PAYLOAD_DATA_SIZE; i++) {
        if (original_payload.data[i] != deserialized_payload.data[i]) {
            printf("✗ FAILED: Data mismatch at byte %d (0x%02X != 0x%02X)\n",
                   i, original_payload.data[i], deserialized_payload.data[i]);
            data_match = false;
            break;
        }
    }

    if (fields_match && data_match) {
        printf("✓ PASSED: All fields and data match!\n");

        // Verify CRC integrity
        if (verify_chunk(&deserialized_payload)) {
            printf("✓ PASSED: CRC verification successful\n");
            return true;
        } else {
            printf("✗ FAILED: CRC verification failed\n");
            return false;
        }
    }

    return false;
}

/*!
 * @brief Test Metadata serialization and deserialization
 */
static bool test_metadata_serialization(void) {
    printf("\n=== Testing Metadata Serialization ===\n");

    // Create test metadata
    struct Metadata original_meta;
    strncpy(original_meta.session_id, "TEST_SESSION_123456789ABCDEF", SESSION_ID_SIZE - 1);
    original_meta.session_id[SESSION_ID_SIZE - 1] = '\0';
    strncpy(original_meta.filename, "test_file.bin", METADATA_FILENAME_SIZE - 1);
    original_meta.filename[METADATA_FILENAME_SIZE - 1] = '\0';
    original_meta.total_size = 123456;
    original_meta.chunk_count = 520;
    original_meta.last_modified = 1698754321;
    original_meta.file_crc = 0xABCD;

    printf("Original Metadata:\n");
    printf("  Session ID: %s\n", original_meta.session_id);
    printf("  Filename: %s\n", original_meta.filename);
    printf("  Total size: %lu bytes\n", (unsigned long)original_meta.total_size);
    printf("  Chunk count: %lu\n", (unsigned long)original_meta.chunk_count);
    printf("  Last modified: %lu\n", (unsigned long)original_meta.last_modified);
    printf("  File CRC: 0x%04X\n", original_meta.file_crc);

    // Serialize
    uint8_t buffer[PAYLOAD_SIZE];
    int serialized_bytes = serialize_metadata(&original_meta, buffer);

    // print the buffer to show serialized data
    // show the padding bytes as well
    printf("Serialized Buffer:\n");

    for (int i = 0; i < PAYLOAD_SIZE; i++) {
        printf("%02X ", buffer[i]);
        // Add spacing for readability
        if (i == 31 || i == 95 || i == 99 || i == 103 || i == 107 || i == 109) {
            printf("\n");
        }
    }

    if (serialized_bytes != PAYLOAD_SIZE) {
        printf("✗ FAILED: Serialization returned %d bytes (expected %d)\n",
               serialized_bytes, PAYLOAD_SIZE);
        return false;
    }

    printf("✓ Serialized to %d bytes (110 bytes data + padding)\n", serialized_bytes);

    // Deserialize
    struct Metadata deserialized_meta;
    int result = deserialize_metadata(buffer, &deserialized_meta);

    if (result != 0) {
        printf("✗ FAILED: Deserialization failed\n");
        return false;
    }

    printf("Deserialized Metadata:\n");
    printf("  Session ID: %s\n", deserialized_meta.session_id);
    printf("  Filename: %s\n", deserialized_meta.filename);
    printf("  Total size: %lu bytes\n", (unsigned long)deserialized_meta.total_size);
    printf("  Chunk count: %lu\n", (unsigned long)deserialized_meta.chunk_count);
    printf("  Last modified: %lu\n", (unsigned long)deserialized_meta.last_modified);
    printf("  File CRC: 0x%04X\n", deserialized_meta.file_crc);

    // Verify fields match
    bool all_match = true;

    if (strcmp(original_meta.session_id, deserialized_meta.session_id) != 0) {
        printf("✗ FAILED: Session ID mismatch\n");
        all_match = false;
    }

    if (strcmp(original_meta.filename, deserialized_meta.filename) != 0) {
        printf("✗ FAILED: Filename mismatch\n");
        all_match = false;
    }

    if (original_meta.total_size != deserialized_meta.total_size) {
        printf("✗ FAILED: Total size mismatch\n");
        all_match = false;
    }

    if (original_meta.chunk_count != deserialized_meta.chunk_count) {
        printf("✗ FAILED: Chunk count mismatch\n");
        all_match = false;
    }

    if (original_meta.last_modified != deserialized_meta.last_modified) {
        printf("✗ FAILED: Last modified mismatch\n");
        all_match = false;
    }

    if (original_meta.file_crc != deserialized_meta.file_crc) {
        printf("✗ FAILED: File CRC mismatch\n");
        all_match = false;
    }

    if (all_match) {
        printf("✓ PASSED: All fields match!\n");
        return true;
    }

    return false;
}

/*!
 * @brief Test simulating MQTT message transfer
 */
static bool test_mqtt_simulation(void) {
    printf("\n=== Simulating MQTT Message Transfer ===\n");

    // Sender side: Create and serialize a payload
    printf("\n--- Sender Side ---\n");
    struct Payload tx_payload;
    tx_payload.sequence = 5;

    // Fill with test data (using 100 bytes for this test)
    uint32_t test_data_size = 100;
    for (uint32_t i = 0; i < test_data_size; i++) {
        tx_payload.data[i] = (uint8_t)((i * 7) & 0xFF);
    }
    // Fill rest with zeros
    for (uint32_t i = test_data_size; i < PAYLOAD_DATA_SIZE; i++) {
        tx_payload.data[i] = 0;
    }

    tx_payload.crc = crc16((const char*)tx_payload.data, PAYLOAD_DATA_SIZE);

    printf("Transmitting Payload:\n");
    printf("  Sequence: %lu\n", (unsigned long)tx_payload.sequence);
    printf("  Size: %lu bytes\n", (unsigned long)PAYLOAD_DATA_SIZE);
    printf("  CRC: 0x%04X\n", tx_payload.crc);

    // Serialize for MQTT transmission
    uint8_t mqtt_buffer[PAYLOAD_SIZE];
    serialize_payload(&tx_payload, mqtt_buffer);

    printf("✓ Serialized to MQTT buffer (247 bytes)\n");

    // Simulate MQTT transmission
    printf("\n--- MQTT Transmission (simulated) ---\n");
    printf("  Buffer ready for mqtt_publish(topic, (char*)mqtt_buffer, %d)\n", PAYLOAD_SIZE);

    // Receiver side: Receive and deserialize
    printf("\n--- Receiver Side ---\n");
    struct Payload rx_payload;
    deserialize_payload(mqtt_buffer, &rx_payload);

    printf("Received Payload:\n");
    printf("  Sequence: %lu\n", (unsigned long)rx_payload.sequence);
    printf("  Size: %lu bytes\n", (unsigned long)PAYLOAD_DATA_SIZE);
    printf("  CRC: 0x%04X\n", rx_payload.crc);

    // Verify integrity
    if (verify_chunk(&rx_payload)) {
        printf("✓ PASSED: CRC verification successful\n");
    } else {
        printf("✗ FAILED: CRC verification failed\n");
        return false;
    }

    // Verify data matches
    bool data_match = true;
    for (uint32_t i = 0; i < PAYLOAD_DATA_SIZE; i++) {
        if (tx_payload.data[i] != rx_payload.data[i]) {
            printf("✗ FAILED: Data mismatch at byte %lu\n", (unsigned long)i);
            data_match = false;
            break;
        }
    }

    if (data_match) {
        printf("✓ PASSED: All transmitted data received correctly\n");
        return true;
    }

    return false;
}

/*!
 * @brief Test edge cases
 */
static bool test_edge_cases(void) {
    printf("\n=== Testing Edge Cases ===\n");

    // Test NULL pointer handling
    printf("Testing NULL pointer handling...\n");
    struct Payload payload;
    uint8_t buffer[PAYLOAD_SIZE];

    if (serialize_payload(NULL, buffer) != -1) {
        printf("✗ FAILED: NULL payload should return -1\n");
        return false;
    }

    if (serialize_payload(&payload, NULL) != -1) {
        printf("✗ FAILED: NULL buffer should return -1\n");
        return false;
    }

    if (deserialize_payload(NULL, &payload) != -1) {
        printf("✗ FAILED: NULL buffer should return -1\n");
        return false;
    }

    if (deserialize_payload(buffer, NULL) != -1) {
        printf("✗ FAILED: NULL payload should return -1\n");
        return false;
    }

    printf("✓ PASSED: NULL pointer handling\n");

    // Test minimum data size
    printf("\nTesting minimum data size (1 byte)...\n");
    payload.sequence = 0;
    payload.data[0] = 0x42;
    // Fill rest with zeros
    for (int i = 1; i < PAYLOAD_DATA_SIZE; i++) {
        payload.data[i] = 0;
    }
    payload.crc = crc16((const char*)payload.data, PAYLOAD_DATA_SIZE);

    serialize_payload(&payload, buffer);
    struct Payload rx_payload;
    deserialize_payload(buffer, &rx_payload);

    if (rx_payload.data[0] == 0x42) {
        printf("✓ PASSED: Minimum data size\n");
    } else {
        printf("✗ FAILED: Minimum data size\n");
        return false;
    }

    // Test maximum data size
    printf("\nTesting maximum data size (%d bytes)...\n", PAYLOAD_DATA_SIZE);
    payload.sequence = 999;
    for (int i = 0; i < PAYLOAD_DATA_SIZE; i++) {
        payload.data[i] = 0xFF;
    }
    payload.crc = crc16((const char*)payload.data, PAYLOAD_DATA_SIZE);

    serialize_payload(&payload, buffer);
    deserialize_payload(buffer, &rx_payload);

    if (rx_payload.sequence == 999) {
        printf("✓ PASSED: Maximum data size\n");
        return true;
    } else {
        printf("✗ FAILED: Maximum data size\n");
        return false;
    }
}

/*!
 * @brief Main test function
 */
int main(void) {
    int test_count = 1;

    stdio_init_all();

    printf("MQTT Serialization/Deserialization Test (Pico W)\n");
    printf("=================================================\n");
    printf("Running tests every 10 seconds...\n");

    // Initial delay to allow USB serial to enumerate
    for (int i = 0; i < 5; i++) {
        printf("Waiting... %d\n", 5 - i);
        sleep_ms(1000);
    }

    while (true) {
        printf("\n");
        printf("═══════════════════════════════════════════════════════\n");
        printf("              Test Run #%d\n", test_count);
        printf("═══════════════════════════════════════════════════════\n");
        printf("  Testing serialize/deserialize for MQTT QoS1\n");
        printf("  Payload size: %d bytes\n", PAYLOAD_SIZE);
        printf("  Data capacity: %d bytes\n", PAYLOAD_DATA_SIZE);
        printf("═══════════════════════════════════════════════════════\n");

        int passed = 0;
        int failed = 0;

        // Run tests
        if (test_payload_serialization()) {
            passed++;
        } else {
            failed++;
        }

        if (test_metadata_serialization()) {
            passed++;
        } else {
            failed++;
        }

        if (test_mqtt_simulation()) {
            passed++;
        } else {
            failed++;
        }

        if (test_edge_cases()) {
            passed++;
        } else {
            failed++;
        }

        // Summary
        printf("\n");
        printf("═══════════════════════════════════════════════════════\n");
        printf("              Test Summary\n");
        printf("═══════════════════════════════════════════════════════\n");
        printf("  Total tests: %d\n", passed + failed);
        printf("  ✓ Passed: %d\n", passed);
        printf("  ✗ Failed: %d\n", failed);

        if (failed == 0) {
            printf("\n  🎯 ALL TESTS PASSED!\n");
        } else {
            printf("\n  ⚠️  SOME TESTS FAILED\n");
        }
        printf("═══════════════════════════════════════════════════════\n");

        printf("\n--- Waiting 10 seconds for next test run ---\n\n");
        sleep_ms(10000);
        test_count++;
    }

    return 0;
}
