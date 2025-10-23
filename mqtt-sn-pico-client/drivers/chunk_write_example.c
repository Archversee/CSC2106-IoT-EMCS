/*!
 * @file    chunk_write_example.c
 * @brief   Example usage of chunk-based write/read for out-of-order MQTT data
 * @author  INF2004 Team
 * @date    2024
 *
 * This example demonstrates how to use the chunk-based write and read methods
 * to handle out-of-order MQTT data packets.
 *
 * Scenario: File with 5 chunks (247 bytes each), where chunk 0 is metadata
 * Chunks may arrive out of order: 3, 2, 1, 5, 4
 * The system should sequentially write them to microSD as they arrive.
 */

#include <stdio.h>
#include <string.h>

#include "microsd_driver.h"
#include "pico/stdlib.h"

/* MQTT QoS1 payload size (247 bytes for data + 9 bytes overhead = 256 total) */
#define PAYLOAD_SIZE 247

/* Datagram packet struct for MQTT QoS1 data transfer */
struct Payload {
    uint32_t sequence;           /* Chunk sequence number */
    uint8_t data[PAYLOAD_SIZE];  /* Actual data payload */
    size_t size;                 /* Actual data size in this chunk */
    uint16_t crc;                /* CRC16 checksum of data */
};

/* Metadata for file tracking overall */
struct Metadata {
    char filename[256];
    uint32_t total_size;
    uint32_t chunk_count;
    uint32_t last_modified;  /* Unix timestamp (exFAT) */
    uint16_t file_crc;       /* CRC16 of entire file */
};

/*!
 * @brief Example of writing chunks out of order using Payload structs
 */
void example_out_of_order_chunk_write(void) {
    filesystem_info_t fs_info;
    chunk_metadata_t metadata;
    
    printf("\n=== Chunk-Based Write Example ===\n");

    /* Step 1: Initialize microSD card */
    if (!microsd_init()) {
        printf("Failed to initialize microSD card\n");
        return;
    }

    /* Step 2: Initialize filesystem */
    if (!microsd_init_filesystem(&fs_info)) {
        printf("Failed to initialize filesystem\n");
        return;
    }

    /* Step 3: Prepare metadata */
    struct Metadata file_meta;
    strncpy(file_meta.filename, "mqtt_data.bin", sizeof(file_meta.filename) - 1);
    file_meta.chunk_count = 5;  /* 5 data chunks (chunk 0 is metadata) */
    file_meta.total_size = file_meta.chunk_count * PAYLOAD_SIZE;
    file_meta.last_modified = get_current_time();
    file_meta.file_crc = 0x0000;  /* Calculate later if needed */
    
    uint32_t total_chunks = file_meta.chunk_count + 1; /* +1 for metadata chunk */
    
    printf("File metadata:\n");
    printf("  Filename: %s\n", file_meta.filename);
    printf("  Total chunks: %lu (including metadata)\n", (unsigned long)total_chunks);
    printf("  Chunk size: %d bytes\n", PAYLOAD_SIZE);
    printf("  Total file size: %lu bytes\n", (unsigned long)file_meta.total_size);

    /* Step 4: Initialize chunk write for the file */
    if (!microsd_init_chunk_write(&fs_info, file_meta.filename, total_chunks, PAYLOAD_SIZE, &metadata)) {
        printf("Failed to initialize chunk write\n");
        return;
    }

    /* Step 5: Simulate receiving chunks out of order */
    /* In a real MQTT scenario, these would come from MQTT callbacks */
    
    /* Simulate chunk order: 3, 2, 1, 5, 4 (plus metadata chunk 0) */
    uint32_t chunk_order[] = {0, 3, 2, 1, 5, 4};
    
    for (int i = 0; i < 6; i++) {
        struct Payload payload;
        payload.sequence = chunk_order[i];
        payload.size = PAYLOAD_SIZE;
        payload.crc = 0x0000;  /* Would calculate CRC in real implementation */

        /* Fill payload data with test pattern (chunk number repeated) */
        memset(payload.data, (uint8_t)payload.sequence, PAYLOAD_SIZE);

        printf("\nReceiving Payload[sequence=%lu, size=%lu]...\n", 
               (unsigned long)payload.sequence,
               (unsigned long)payload.size);

        /* Write the chunk */
        if (!microsd_write_chunk(&fs_info, &metadata, payload.sequence, 
                                 payload.data, payload.size)) {
            printf("Failed to write chunk %lu\n", (unsigned long)payload.sequence);
            continue;
        }

        printf("Successfully wrote chunk %lu\n", (unsigned long)payload.sequence);
        printf("Progress: %lu/%lu chunks received\n",
               (unsigned long)metadata.chunks_received,
               (unsigned long)metadata.total_chunks);

        /* Show current state after each chunk */
        printf("Chunks received so far: ");
        for (uint32_t j = 0; j < total_chunks; j++) {
            uint32_t byte_idx = j / 8;
            uint32_t bit_idx = j % 8;
            if (metadata.chunk_bitmap[byte_idx] & (1 << bit_idx)) {
                printf("%lu ", (unsigned long)j);
            }
        }
        printf("\n");
    }

    /* Step 6: Check if all chunks received */
    if (microsd_check_all_chunks_received(&metadata)) {
        printf("\n✓ All chunks received!\n");
    } else {
        printf("\n✗ Not all chunks received yet\n");
        return;
    }

    /* Step 7: Finalize the chunk write (create directory entry) */
    if (!microsd_finalize_chunk_write(&fs_info, &metadata)) {
        printf("Failed to finalize chunk write\n");
        return;
    }

    printf("\n✓ File '%s' successfully created on microSD!\n", file_meta.filename);
    printf("Total size: %lu bytes\n", (unsigned long)file_meta.total_size);
}

/*!
 * @brief Example of reading chunks from a file
 */
void example_chunk_read(void) {
    filesystem_info_t fs_info;
    uint8_t chunk_data[247];
    uint32_t bytes_read;
    const char* filename = "mqtt_data.bin";
    uint32_t chunk_size = 247;

    printf("\n=== Chunk-Based Read Example ===\n");

    /* Step 1: Initialize microSD card and filesystem */
    if (!microsd_init()) {
        printf("Failed to initialize microSD card\n");
        return;
    }

    if (!microsd_init_filesystem(&fs_info)) {
        printf("Failed to initialize filesystem\n");
        return;
    }

    /* Step 2: Read chunks from the file */
    printf("Reading chunks from '%s'...\n", filename);

    for (uint32_t chunk_idx = 1; chunk_idx <= 5; chunk_idx++) {
        if (!microsd_read_chunk(&fs_info, filename, chunk_data, chunk_size, chunk_idx, &bytes_read)) {
            printf("Failed to read chunk %lu\n", (unsigned long)chunk_idx);
            continue;
        }

        printf("\nChunk %lu: read %lu bytes\n",
               (unsigned long)chunk_idx,
               (unsigned long)bytes_read);

        /* Verify chunk data (should be filled with chunk number) */
        bool valid = true;
        for (uint32_t i = 0; i < bytes_read; i++) {
            if (chunk_data[i] != (uint8_t)chunk_idx) {
                valid = false;
                break;
            }
        }

        if (valid) {
            printf("✓ Chunk %lu data verified correctly\n", (unsigned long)chunk_idx);
        } else {
            printf("✗ Chunk %lu data verification failed\n", (unsigned long)chunk_idx);
        }
    }
}

/*!
 * @brief Main demonstration function
 * @return int Program exit status
 */
int main(void) {
    bool all_tests_passed = true;
    int test_count = 1;

    stdio_init_all();

    printf("MicroSD Chunk-Based Write/Read Demo\n");
    printf("====================================\n");
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
        printf("\n=== Test Run #%d ===\n", test_count);
        printf("╔════════════════════════════════════════════════════════╗\n");
        printf("║   MicroSD Chunk-Based Write/Read Example              ║\n");
        printf("║   Out-of-Order MQTT Data Handling                     ║\n");
        printf("╚════════════════════════════════════════════════════════╝\n");

        all_tests_passed = true;

        /* Initialize microSD card */
        printf("\nInitializing microSD card...\n");
        if (microsd_init()) {
            printf("MicroSD card initialized successfully\n");

            /* Example 1: Write chunks out of order */
            example_out_of_order_chunk_write();

            /* Wait a bit */
            sleep_ms(1000);

            /* Example 2: Read chunks back */
            example_chunk_read();

            printf("\n=== Test Cycle Complete ===\n");
        } else {
            printf("ERROR: Failed to initialize microSD card\n");
            printf("Please check the microSD card connection and try again.\n");
            all_tests_passed = false;
        }

        printf("\n--- Waiting 10 seconds for next test run ---\n\n");
        sleep_ms(10000);
        test_count++;
    }

    return 0;
}
