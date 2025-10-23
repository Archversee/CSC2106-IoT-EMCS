/*!
 * @file    file_transfer_demo.c
 * @brief   Complete file transfer simulation demo with chunking, serialization and reconstruction
 * @author  INF2004 Team
 * @date    2024
 *
 * This demo demonstrates the complete workflow of:
 * 1. Creating a test file (HELLO_INF2004.txt)
 * 2. Chunking the file based on payload size
 * 3. Serializing metadata and payloads
 * 4. Simulating MQTT-SN file transfer with serialization/deserialization
 * 5. Reconstructing the file from received chunks
 * Runs every 30 seconds to show consistent behavior
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_frame.h"
#include "microsd_driver.h"
#include "pico/stdlib.h"

/*! Constants */
#define TEST_FILENAME "HELLO_INF2004.txt"
#define RECONSTRUCTED_FILENAME "RECON.txt"

/*! Test file content */
static const char* test_file_content =
    "HELLO INF2004 CS31 GROUP\n"
    "This text file was created by microsd_driver.c demo.\n"
    "Hello Prof Fauzi!\n"
    "Just for some fun, here are some funny dumps:\n"
    "\n"
    "Glasses are really versatile. For example, glasses can change a character's\n"
    "appearance or personality: someone wearing glasses can look more serious,\n"
    "or removing glasses can create a different effect. Characters stealing a\n"
    "protagonist's glasses and putting them on can be played for humor (\"Haha,\n"
    "got your glasses!\"), and glasses can also make a character look cool.\n"
    "You can switch styles and colors to match a mood: half-rim, thick frames,\n"
    "and more. It's fun to experiment with different styles.\n"
    "\n"
    "I hope Luna or Marine might try some on to replace an eyepatch — it would\n"
    "be a cute visual change.\n"
    "\n"
    "Don't you think we should officially give everyone glasses?\n";

/*!
 * @brief Step 1: Create the test file on microSD
 * @return bool true on success, false on failure
 */
static bool step1_create_test_file(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("  STEP 1: Create Test File\n");
    printf("═══════════════════════════════════════════════════════\n");

    filesystem_info_t fs_info;

    printf("Initializing filesystem...\n");
    if (!microsd_init_filesystem(&fs_info)) {
        printf("✗ ERROR: Failed to initialize filesystem!\n");
        return false;
    }

    if (!fs_info.is_exfat) {
        printf("✗ ERROR: SD card is not formatted with exFAT!\n");
        return false;
    }

    printf("✓ Filesystem initialized (exFAT detected)\n");

    size_t content_length = strlen(test_file_content);
    printf("\nCreating file: %s\n", TEST_FILENAME);
    printf("Content size: %zu bytes\n", content_length);

    if (!microsd_create_file(&fs_info, TEST_FILENAME, (uint8_t*)test_file_content, content_length)) {
        printf("✗ ERROR: Failed to create file!\n");
        return false;
    }

    printf("✓ File '%s' created successfully!\n", TEST_FILENAME);
    printf("\nFile preview (first 150 chars):\n");
    printf("─────────────────────────────────────────────────────\n");
    for (size_t i = 0; i < 150 && i < content_length; i++) {
        printf("%c", test_file_content[i]);
    }
    printf("...\n");
    printf("─────────────────────────────────────────────────────\n");

    return true;
}

/*!
 * @brief Step 2: Chunk the file into metadata and payload chunks
 * @param metadata Output metadata structure
 * @param chunks Output array of payload chunks (will be allocated)
 * @return bool true on success, false on failure
 */
static bool step2_chunk_file(struct Metadata* metadata, struct Payload** chunks) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("  STEP 2: Chunk File into Payloads\n");
    printf("═══════════════════════════════════════════════════════\n");

    printf("Reading file: %s\n", TEST_FILENAME);
    printf("Payload capacity: %d bytes per chunk\n", PAYLOAD_SIZE);
    printf("Data capacity: %d bytes per chunk\n", PAYLOAD_DATA_SIZE);

    // Use deconstruct function from data_frame.c
    if (deconstruct(TEST_FILENAME, metadata, chunks) != 0) {
        printf("✗ ERROR: Failed to deconstruct file!\n");
        return false;
    }

    printf("\n✓ File chunked successfully!\n");
    printf("\nMetadata Summary:\n");
    printf("─────────────────────────────────────────────────────\n");
    printf("  Filename:      %s\n", metadata->filename);
    printf("  Total size:    %lu bytes\n", (unsigned long)metadata->total_size);
    printf("  Chunk count:   %lu chunks\n", (unsigned long)metadata->chunk_count);
    printf("  File CRC16:    0x%04X\n", metadata->file_crc);
    printf("─────────────────────────────────────────────────────\n");

    // Show details of first few chunks
    printf("\nFirst 3 chunks preview:\n");
    for (uint32_t i = 0; i < 3 && i < metadata->chunk_count; i++) {
        printf("  Chunk %lu: sequence=%lu, size=%lu, crc=0x%04X\n",
               (unsigned long)i,
               (unsigned long)(*chunks)[i].sequence,
               (unsigned long)(*chunks)[i].size,
               (*chunks)[i].crc);
    }

    return true;
}

/*!
 * @brief Step 3: Serialize metadata and all payload chunks
 * @param metadata Metadata structure to serialize
 * @param chunks Array of payload chunks
 * @param chunk_count Number of chunks
 * @param serialized_metadata Output buffer for serialized metadata (must be PAYLOAD_SIZE bytes)
 * @param serialized_chunks Output array for serialized chunks (will be allocated)
 * @return bool true on success, false on failure
 */
static bool step3_serialize_data(struct Metadata* metadata,
                                 struct Payload* chunks,
                                 uint32_t chunk_count,
                                 uint8_t* serialized_metadata,
                                 uint8_t** serialized_chunks) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("  STEP 3: Serialize Metadata and Payloads\n");
    printf("═══════════════════════════════════════════════════════\n");

    // Serialize metadata
    printf("Serializing metadata...\n");
    int meta_bytes = serialize_metadata(metadata, serialized_metadata);
    if (meta_bytes != PAYLOAD_SIZE) {
        printf("✗ ERROR: Metadata serialization failed (got %d bytes, expected %d)\n",
               meta_bytes, PAYLOAD_SIZE);
        return false;
    }
    printf("✓ Metadata serialized: %d bytes\n", meta_bytes);

    // Show serialized metadata sample (first 32 bytes)
    printf("\nSerialized metadata (first 32 bytes):\n");
    for (int i = 0; i < 32; i++) {
        printf("%02X ", serialized_metadata[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }

    // Allocate memory for serialized chunks
    *serialized_chunks = (uint8_t*)malloc(PAYLOAD_SIZE * chunk_count);
    if (*serialized_chunks == NULL) {
        printf("✗ ERROR: Failed to allocate memory for serialized chunks\n");
        return false;
    }

    // Serialize all chunks
    printf("\nSerializing %lu payload chunks...\n", (unsigned long)chunk_count);
    for (uint32_t i = 0; i < chunk_count; i++) {
        uint8_t* chunk_buffer = *serialized_chunks + (i * PAYLOAD_SIZE);
        int bytes = serialize_payload(&chunks[i], chunk_buffer);

        if (bytes != PAYLOAD_SIZE) {
            printf("✗ ERROR: Failed to serialize chunk %lu\n", (unsigned long)i);
            free(*serialized_chunks);
            *serialized_chunks = NULL;
            return false;
        }

        if ((i + 1) % 10 == 0 || i == chunk_count - 1) {
            printf("  Serialized chunk %lu/%lu\n", (unsigned long)(i + 1), (unsigned long)chunk_count);
        }
    }

    printf("✓ All payloads serialized successfully!\n");
    printf("\nSerialization Summary:\n");
    printf("─────────────────────────────────────────────────────\n");
    printf("  Metadata:      %d bytes (1 packet)\n", PAYLOAD_SIZE);
    printf("  Payload data:  %lu bytes (%lu packets)\n",
           (unsigned long)(PAYLOAD_SIZE * chunk_count),
           (unsigned long)chunk_count);
    printf("  Total:         %lu bytes (%lu packets)\n",
           (unsigned long)(PAYLOAD_SIZE * (chunk_count + 1)),
           (unsigned long)(chunk_count + 1));
    printf("─────────────────────────────────────────────────────\n");

    return true;
}

/*!
 * @brief Step 4: Simulate MQTT-SN transfer with deserialization
 * @param serialized_metadata Serialized metadata buffer
 * @param serialized_chunks Serialized chunks buffer
 * @param received_metadata Output for deserialized metadata
 * @param received_chunks Output for deserialized chunks (will be allocated)
 * @param chunk_count Number of chunks to transfer
 * @return bool true on success, false on failure
 */
static bool step4_simulate_transfer(uint8_t* serialized_metadata,
                                    uint8_t* serialized_chunks,
                                    struct Metadata* received_metadata,
                                    struct Payload** received_chunks,
                                    uint32_t chunk_count) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("  STEP 4: Simulate MQTT-SN File Transfer\n");
    printf("═══════════════════════════════════════════════════════\n");

    // Simulate metadata transfer
    printf("─── Transfer Sequence ───\n");
    printf("\n[SENDER → MQTT BROKER → RECEIVER]\n");
    printf("\n1. Transmitting metadata packet (247 bytes)...\n");
    printf("   mqtt_publish(\"file/metadata\", buffer, %d)\n", PAYLOAD_SIZE);

    // Simulate network delay
    sleep_ms(100);

    printf("   ↓ MQTT QoS1 transmission...\n");
    sleep_ms(50);

    printf("   ✓ Metadata received by subscriber\n");

    // Deserialize metadata on receiver side
    if (deserialize_metadata(serialized_metadata, received_metadata) != 0) {
        printf("✗ ERROR: Failed to deserialize metadata!\n");
        return false;
    }

    printf("   ✓ Metadata deserialized successfully\n");
    printf("\n   Received Metadata:\n");
    printf("     Filename:    %s\n", received_metadata->filename);
    printf("     Total size:  %lu bytes\n", (unsigned long)received_metadata->total_size);
    printf("     Chunks:      %lu\n", (unsigned long)received_metadata->chunk_count);
    printf("     File CRC:    0x%04X\n", received_metadata->file_crc);

    // Allocate memory for received chunks
    *received_chunks = (struct Payload*)malloc(sizeof(struct Payload) * chunk_count);
    if (*received_chunks == NULL) {
        printf("✗ ERROR: Failed to allocate memory for received chunks\n");
        return false;
    }

    // Simulate payload chunk transfers
    printf("\n2. Transmitting %lu payload chunks...\n", (unsigned long)chunk_count);

    uint32_t successful_transfers = 0;
    uint32_t failed_transfers = 0;

    for (uint32_t i = 0; i < chunk_count; i++) {
        uint8_t* chunk_buffer = serialized_chunks + (i * PAYLOAD_SIZE);

        // Simulate transmission
        if ((i + 1) % 10 == 0 || i == 0 || i == chunk_count - 1) {
            printf("   Chunk %lu/%lu: mqtt_publish(\"file/chunk/%lu\", buffer, %d)\n",
                   (unsigned long)(i + 1),
                   (unsigned long)chunk_count,
                   (unsigned long)i,
                   PAYLOAD_SIZE);
        }

        // Simulate small network delay for some packets
        if (i % 5 == 0) {
            sleep_ms(20);
        }

        // Deserialize on receiver side
        if (deserialize_payload(chunk_buffer, &(*received_chunks)[i]) != 0) {
            printf("   ✗ ERROR: Failed to deserialize chunk %lu\n", (unsigned long)i);
            failed_transfers++;
            continue;
        }

        // Verify chunk integrity
        if (!verify_chunk(&(*received_chunks)[i])) {
            printf("   ✗ ERROR: Chunk %lu failed CRC verification!\n", (unsigned long)i);
            failed_transfers++;
            continue;
        }

        successful_transfers++;
    }

    printf("\n─── Transfer Complete ───\n");
    printf("\nTransfer Statistics:\n");
    printf("─────────────────────────────────────────────────────\n");
    printf("  Total packets:     %lu (1 metadata + %lu payloads)\n",
           (unsigned long)(chunk_count + 1),
           (unsigned long)chunk_count);
    printf("  Successful:        %lu\n", (unsigned long)successful_transfers);
    printf("  Failed:            %lu\n", (unsigned long)failed_transfers);
    printf("  Success rate:      %.2f%%\n",
           (successful_transfers * 100.0) / chunk_count);
    printf("─────────────────────────────────────────────────────\n");

    if (failed_transfers > 0) {
        printf("✗ ERROR: Transfer incomplete due to failures\n");
        free(*received_chunks);
        *received_chunks = NULL;
        return false;
    }

    printf("✓ All chunks transferred and verified successfully!\n");
    return true;
}

/*!
 * @brief Step 5: Reconstruct the file from received chunks
 * @param metadata Received metadata
 * @param chunks Received payload chunks
 * @return bool true on success, false on failure
 */
static bool step5_reconstruct_file(struct Metadata* metadata, struct Payload* chunks) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("  STEP 5: Reconstruct File from Chunks\n");
    printf("═══════════════════════════════════════════════════════\n");

    printf("Reconstructing file: %s\n", RECONSTRUCTED_FILENAME);
    printf("Expected size: %lu bytes\n", (unsigned long)metadata->total_size);
    printf("Expected chunks: %lu\n", (unsigned long)metadata->chunk_count);

    // Use reconstruct function from data_frame.c
    if (reconstruct(metadata, &chunks, RECONSTRUCTED_FILENAME) != 0) {
        printf("✗ ERROR: Failed to reconstruct file!\n");
        return false;
    }

    printf("\n✓ File reconstructed successfully!\n");
    printf("\nFile saved as: %s\n", RECONSTRUCTED_FILENAME);

    // Verify by reading back the reconstructed file
    printf("\nVerifying reconstructed file...\n");
    filesystem_info_t fs_info;
    if (!microsd_init_filesystem(&fs_info)) {
        printf("⚠ WARNING: Could not verify file (filesystem init failed)\n");
        return true;
    }

    uint8_t read_buffer[1024];
    uint32_t bytes_read = 0;

    if (microsd_read_file(&fs_info, RECONSTRUCTED_FILENAME, read_buffer, sizeof(read_buffer), &bytes_read)) {
        printf("✓ Read back %lu bytes from reconstructed file\n", (unsigned long)bytes_read);

        // Compare with original
        if (bytes_read == strlen(test_file_content)) {
            bool match = true;
            for (uint32_t i = 0; i < bytes_read; i++) {
                if (read_buffer[i] != (uint8_t)test_file_content[i]) {
                    match = false;
                    printf("✗ ERROR: Byte mismatch at position %lu\n", (unsigned long)i);
                    printf("   Expected: 0x%02X ('%c'), Got: 0x%02X ('%c')\n",
                           (uint8_t)test_file_content[i],
                           (test_file_content[i] >= 32 && test_file_content[i] < 127) ? test_file_content[i] : '.',
                           read_buffer[i],
                           (read_buffer[i] >= 32 && read_buffer[i] < 127) ? read_buffer[i] : '.');
                    // Show a few bytes around the mismatch
                    printf("   Context (10 bytes before): ");
                    for (uint32_t j = (i >= 10) ? i - 10 : 0; j < i; j++) {
                        printf("%02X ", read_buffer[j]);
                    }
                    printf("\n   Context (10 bytes after):  ");
                    for (uint32_t j = i; j < i + 10 && j < bytes_read; j++) {
                        printf("%02X ", read_buffer[j]);
                    }
                    printf("\n");
                    break;
                }
            }

            if (match) {
                printf("✓ Perfect match! File integrity verified.\n");

                printf("\nReconstructed file preview (first 150 chars):\n");
                printf("─────────────────────────────────────────────────────\n");
                for (uint32_t i = 0; i < 150 && i < bytes_read; i++) {
                    printf("%c", read_buffer[i]);
                }
                printf("...\n");
                printf("─────────────────────────────────────────────────────\n");
            } else {
                printf("✗ ERROR: File content mismatch!\n");
                return false;
            }
        } else {
            printf("✗ ERROR: Size mismatch (expected %zu, got %lu)\n",
                   strlen(test_file_content), (unsigned long)bytes_read);
            return false;
        }
    } else {
        printf("⚠ WARNING: Could not read back reconstructed file\n");
    }

    return true;
}

/*!
 * @brief Run complete file transfer demo
 * @return bool true if all steps passed, false otherwise
 */
static bool run_complete_demo(void) {
    struct Metadata metadata = {0};
    struct Payload* chunks = NULL;
    uint8_t serialized_metadata[PAYLOAD_SIZE];
    uint8_t* serialized_chunks = NULL;
    struct Metadata received_metadata = {0};
    struct Payload* received_chunks = NULL;

    bool success = true;

    // Step 1: Create test file
    if (!step1_create_test_file()) {
        success = false;
        goto cleanup;
    }

    // Step 2: Chunk the file
    if (!step2_chunk_file(&metadata, &chunks)) {
        success = false;
        goto cleanup;
    }

    // Step 3: Serialize data
    if (!step3_serialize_data(&metadata, chunks, metadata.chunk_count,
                              serialized_metadata, &serialized_chunks)) {
        success = false;
        goto cleanup;
    }

    // Step 4: Simulate transfer
    if (!step4_simulate_transfer(serialized_metadata, serialized_chunks,
                                 &received_metadata, &received_chunks,
                                 metadata.chunk_count)) {
        success = false;
        goto cleanup;
    }

    // Step 5: Reconstruct file
    if (!step5_reconstruct_file(&received_metadata, received_chunks)) {
        success = false;
        goto cleanup;
    }

cleanup:
    // Free allocated memory
    if (chunks != NULL) {
        free(chunks);
    }
    if (serialized_chunks != NULL) {
        free(serialized_chunks);
    }
    if (received_chunks != NULL) {
        free(received_chunks);
    }

    return success;
}

/*!
 * @brief Main demonstration function
 * @return int Program exit status
 */
int main(void) {
    int test_count = 1;

    stdio_init_all();

    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║  MQTT-SN File Transfer Complete Demo                 ║\n");
    printf("║  Testing: Chunking → Serialization → Transfer        ║\n");
    printf("║           → Deserialization → Reconstruction         ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    printf("\nRunning complete demo every 30 seconds...\n");

    // Initial delay to allow USB serial to enumerate
    for (int i = 0; i < 5; i++) {
        printf("Waiting... %d\n", 5 - i);
        sleep_ms(1000);
    }

    while (true) {
        printf("\n\n");
        printf("╔═══════════════════════════════════════════════════════╗\n");
        printf("║                  Test Run #%-3d                        ║\n", test_count);
        printf("╚═══════════════════════════════════════════════════════╝\n");

        // Initialize microSD card for this test run
        printf("\nInitializing microSD card...\n");
        if (!microsd_init()) {
            printf("✗ ERROR: Failed to initialize microSD card\n");
            printf("Please check the microSD card connection and try again.\n");
            printf("\n--- Waiting 30 seconds before retry ---\n");
            sleep_ms(30000);
            test_count++;
            continue;
        }
        printf("✓ MicroSD card initialized successfully\n");
        microsd_set_log_level(MICROSD_LOG_INFO);

        uint32_t start_time = to_ms_since_boot(get_absolute_time());

        // Run complete demo
        bool demo_passed = run_complete_demo();

        uint32_t end_time = to_ms_since_boot(get_absolute_time());
        uint32_t elapsed_ms = end_time - start_time;

        // Final summary
        printf("\n");
        printf("╔═══════════════════════════════════════════════════════╗\n");
        printf("║              Final Summary - Run #%-3d                 ║\n", test_count);
        printf("╚═══════════════════════════════════════════════════════╝\n");

        if (demo_passed) {
            printf("\n  🎯 ALL STEPS COMPLETED SUCCESSFULLY!\n");
            printf("\n  The complete workflow demonstrated:\n");
            printf("    ✓ Step 1: File creation\n");
            printf("    ✓ Step 2: File chunking (%d bytes per chunk)\n", PAYLOAD_DATA_SIZE);
            printf("    ✓ Step 3: Serialization for MQTT-SN\n");
            printf("    ✓ Step 4: Simulated network transfer\n");
            printf("    ✓ Step 5: File reconstruction with verification\n");
        } else {
            printf("\n  ⚠️  DEMO FAILED!\n");
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
