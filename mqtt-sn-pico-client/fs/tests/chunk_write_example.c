/*!
 * @file    chunk_write_example.c
 * @brief   Example usage of chunk-based write/read for out-of-order MQTT QoS1 data
 * @author  INF2004 Team
 * @date    2024
 *
 * This example demonstrates how to use the chunk-based write and read methods
 * to handle out-of-order MQTT QoS1 data packets using data_frame.h structs.
 *
 * MQTT QoS1 Protocol:
 * 1. Metadata chunk (chunk 0) is ALWAYS sent first to establish session_id
 * 2. Data chunks (1-N) may arrive out of order: 3, 2, 1, 5, 4
 * 3. Each data chunk references the session_id to identify which file transfer
 * 4. The system sequentially writes chunks to microSD as they arrive
 */

#include <stdio.h>
#include <string.h>

#include "chunk_transfer.h"
#include "data_frame.h"
#include "microsd_driver.h"
#include "pico/stdlib.h"

/*!
 * @brief Simulate MQTT reception of metadata chunk (chunk 0)
 */
static void simulate_mqtt_receive_metadata(struct Metadata* meta) {
    /* Simulate receiving metadata over MQTT */
    strncpy(meta->session_id, "SESSION_12345678_MQTT_TRANSFER", SESSION_ID_SIZE - 1);
    strncpy(meta->filename, "mqtt_test.bin", METADATA_FILENAME_SIZE - 1);
    meta->chunk_count = 5; /* 5 data chunks */
    meta->total_size = meta->chunk_count * PAYLOAD_DATA_SIZE;
    meta->last_modified = get_current_time();
    meta->file_crc = 0x0000; /* Would be calculated in real implementation */

    printf("MQTT Received: Metadata chunk\n");
    printf("  Session ID: %s\n", meta->session_id);
    printf("  Filename: %s\n", meta->filename);
    printf("  Total size: %lu bytes\n", (unsigned long)meta->total_size);
    printf("  Chunk count: %lu\n", (unsigned long)meta->chunk_count);
}

/*!
 * @brief Simulate MQTT reception of a data payload chunk
 * @note In QoS1, each data chunk would reference the session_id from metadata
 *       to identify which file transfer it belongs to
 */
static void simulate_mqtt_receive_payload(struct Payload* payload, uint32_t sequence,
                                          const char* session_id) {
    /* Simulate receiving payload over MQTT */
    payload->sequence = sequence;
    payload->size = PAYLOAD_DATA_SIZE;

    /* Fill with test pattern (sequence number repeated) */
    memset(payload->data, (uint8_t)sequence, PAYLOAD_DATA_SIZE);

    /* Calculate CRC (simplified - just use sequence for demo) */
    payload->crc = crc16(payload->data, payload->size);

    printf("MQTT Received: Payload[session=%s, sequence=%lu, size=%lu, crc=0x%04X]\n",
           session_id,
           (unsigned long)payload->sequence,
           (unsigned long)payload->size,
           payload->crc);
}

/*!
 * @brief Example of writing chunks out of order using chunk_transfer API
 */
static void example_out_of_order_chunk_write(void) {
    filesystem_info_t fs_info;
    transfer_session_t session;
    struct Metadata file_meta;
    struct Payload payload;

    printf("\n=== Chunk-Based Write Example (Using chunk_transfer API) ===\n");

    /* Step 1: Initialize filesystem */
    if (!microsd_init_filesystem(&fs_info)) {
        printf("Failed to initialize filesystem\n");
        return;
    }

    /* Step 2: Simulate receiving metadata chunk FIRST (QoS1 requirement) */
    printf("\n--- Step 1: MQTT QoS1 Metadata Reception (ALWAYS FIRST) ---\n");
    simulate_mqtt_receive_metadata(&file_meta);
    printf("  ✓ Session will be established with ID: %s\n", file_meta.session_id);
    printf("  ✓ All subsequent data chunks will reference this session_id\n");

    /* Step 3: Initialize transfer session (this handles metadata chunk writing) */
    printf("\n--- Step 2: Initializing Transfer Session ---\n");
    if (!chunk_transfer_init_session(&fs_info, &file_meta, &session)) {
        printf("Failed to initialize transfer session\n");
        return;
    }

    /* Step 4: Simulate receiving data chunks OUT OF ORDER: 3, 2, 1, 5, 4 */
    printf("\n--- Step 3: Receiving Data Chunks OUT OF ORDER ---\n");
    printf("  (Each chunk references session_id: %s)\n", session.session_id);
    uint32_t chunk_order[] = {3, 2, 1, 5, 4};

    for (int i = 0; i < 5; i++) {
        uint32_t sequence = chunk_order[i];

        printf("\n");
        /* Simulate MQTT receiving this chunk with session_id reference */
        simulate_mqtt_receive_payload(&payload, sequence, session.session_id);

        /* Verify chunk integrity */
        if (verify_chunk(&payload)) {
            printf("  CRC verification: PASSED\n");
        } else {
            printf("  CRC verification: FAILED - skipping chunk\n");
            continue;
        }

        /* Write the chunk using high-level API */
        if (!chunk_transfer_write_payload(&fs_info, &session, &payload)) {
            printf("  Failed to write chunk %lu\n", (unsigned long)payload.sequence);
            continue;
        }

        printf("  ✓ Written to microSD at position %lu\n", (unsigned long)payload.sequence);

        /* Get and display progress */
        uint32_t chunks_received, total_chunks;
        chunk_transfer_get_progress(&session, &chunks_received, &total_chunks);
        printf("  Progress: %lu/%lu chunks received\n",
               (unsigned long)chunks_received,
               (unsigned long)total_chunks);
    }

    /* Step 5: Print session status */
    printf("\n--- Step 4: Checking Session Status ---\n");
    chunk_transfer_print_session_info(&session);

    /* Step 6: Check if all chunks received and finalize */
    if (chunk_transfer_is_complete(&session)) {
        printf("\n--- Step 5: Finalizing Transfer ---\n");
        if (!chunk_transfer_finalize(&fs_info, &session)) {
            printf("Failed to finalize transfer\n");
            return;
        }
        printf("\n✓ SUCCESS: File transfer complete!\n");
    } else {
        printf("\n✗ ERROR: Transfer incomplete - missing chunks\n");
    }
}

/*!
 * @brief Example of reading chunks from a file and reconstructing Payload structs
 */
static void example_chunk_read(void) {
    filesystem_info_t fs_info;
    struct Payload payload;
    uint32_t bytes_read;
    const char* filename = "mqtt_test.bin";

    printf("\n=== Chunk-Based Read Example ===\n");

    /* Step 1: Initialize filesystem */
    if (!microsd_init_filesystem(&fs_info)) {
        printf("Failed to initialize filesystem\n");
        return;
    }

    /* Step 2: Read chunks from the file and verify */
    printf("Reading chunks from '%s'...\n", filename);

    for (uint32_t chunk_idx = 1; chunk_idx <= 5; chunk_idx++) {
        printf("\n--- Reading Chunk %lu ---\n", (unsigned long)chunk_idx);

        if (!microsd_read_chunk(&fs_info, filename, payload.data,
                                PAYLOAD_DATA_SIZE, chunk_idx, &bytes_read)) {
            printf("Failed to read chunk %lu\n", (unsigned long)chunk_idx);
            continue;
        }

        /* Reconstruct Payload struct */
        payload.sequence = chunk_idx;
        payload.size = bytes_read;
        payload.crc = crc16(payload.data, payload.size);

        printf("Payload[sequence=%lu, size=%lu, crc=0x%04X]\n",
               (unsigned long)payload.sequence,
               (unsigned long)payload.size,
               payload.crc);

        /* Verify chunk data (should be filled with chunk number) */
        bool valid = true;
        for (uint32_t i = 0; i < bytes_read; i++) {
            if (payload.data[i] != (uint8_t)chunk_idx) {
                valid = false;
                break;
            }
        }

        if (valid) {
            printf("✓ Chunk %lu data verified correctly\n", (unsigned long)chunk_idx);
        } else {
            printf("✗ Chunk %lu data verification FAILED\n", (unsigned long)chunk_idx);
        }

        /* Verify using data_frame function */
        if (verify_chunk(&payload)) {
            printf("✓ CRC verification: PASSED\n");
        } else {
            printf("✗ CRC verification: FAILED\n");
        }
    }
}

/*!
 * @brief Main demonstration function
 * @return int Program exit status
 */
int main(void) {
    int test_count = 1;

    stdio_init_all();

    printf("MicroSD Chunk-Based Write/Read Demo (data_frame.h)\n");
    printf("===================================================\n");
    printf("Running tests every 10 seconds...\n");

    /* Initial delay to allow USB serial to enumerate */
    for (int i = 0; i < 5; i++) {
        printf("Waiting... %d\n", 5 - i);
        sleep_ms(1000);
    }

    /* Set debug logging */
    microsd_set_log_level(MICROSD_LOG_DEBUG);

    /* Print driver information banner */
    microsd_print_banner();

    while (true) {
        printf("\n");
        printf("═══════════════════════════════════════════════════════\n");
        printf("              Test Run #%d\n", test_count);
        printf("═══════════════════════════════════════════════════════\n");
        printf("  MQTT QoS1 File Transfer Simulation\n");
        printf("  Using chunk_transfer API (high-level)\n");
        printf("  Protocol: Metadata FIRST, then data chunks\n");
        printf("  Transfer order: 0 (metadata) → 3 → 2 → 1 → 5 → 4\n");
        printf("  Each data chunk references session_id from metadata\n");
        printf("═══════════════════════════════════════════════════════\n");

        /* Initialize microSD card */
        printf("\nInitializing microSD card...\n");
        if (microsd_init()) {
            printf("✓ MicroSD card initialized successfully\n");

            /* Example 1: Write chunks out of order */
            example_out_of_order_chunk_write();

            /* Wait a bit */
            sleep_ms(1000);

            /* Example 2: Read chunks back */
            example_chunk_read();

            printf("\n✓ Test Cycle Complete\n");
        } else {
            printf("✗ ERROR: Failed to initialize microSD card\n");
            printf("  Please check the microSD card connection and try again.\n");
        }

        printf("\n--- Waiting 10 seconds for next test run ---\n\n");
        sleep_ms(10000);
        test_count++;
    }

    return 0;
}
