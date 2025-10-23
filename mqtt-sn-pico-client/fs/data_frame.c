#include "data_frame.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../drivers/microsd_driver.h"

/**
 * @brief Calculate CRC16 checksum for data buffer (CCITT-FALSE)
 * @param data Pointer to data buffer
 * @param length Length of data in bytes
 * @return uint16_t CRC16 checksum value
 */
uint16_t crc16(unsigned char* data, size_t length) {
    uint16_t crc = 0xFFFF;  // Initial value for CCITT-FALSE

    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;

        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;  // CCITT polynomial
            } else {
                crc = crc << 1;
            }
        }
    }

    return crc;
}

/**
 * @brief Verify integrity of a chunk using CRC16
 * @param chunk Pointer to Payload structure
 * @return int 1 if valid, 0 if invalid
 */
int verify_chunk(struct Payload* chunk) {
    if (chunk == NULL) {
        return 0;
    }

    // Calculate CRC16 of the data portion
    uint16_t calculated_crc = crc16(chunk->data, chunk->size);

    // Compare with stored CRC
    return (calculated_crc == chunk->crc) ? 1 : 0;
}

/**
 * @brief Deconstruct a file into chunks for MQTT transmission
 * @param filename Name of file to read from SD card
 * @param meta Pointer to Metadata structure (output)
 * @param chunks Pointer to array of Payload pointers (output)
 * @return int 0 on success, -1 on failure
 */
int deconstruct(char* filename, struct Metadata* meta, struct Payload** chunks) {
    if (filename == NULL || meta == NULL || chunks == NULL) {
        printf("Error: NULL parameters\n");
        return -1;
    }

    // Initialize filesystem
    filesystem_info_t fs_info;
    if (!microsd_init_filesystem(&fs_info)) {
        printf("Error: Failed to initialize filesystem\n");
        return -1;
    }

// Use a reasonably sized buffer (32KB should be enough for most files on Pico)
#define MAX_FILE_BUFFER_SIZE (32 * 1024)
    uint8_t* file_buffer = (uint8_t*)malloc(MAX_FILE_BUFFER_SIZE);
    if (file_buffer == NULL) {
        printf("Error: Memory allocation failed for file buffer\n");
        return -1;
    }

    // Read the file
    uint32_t file_size = 0;
    if (!microsd_read_file(&fs_info, filename, file_buffer, MAX_FILE_BUFFER_SIZE, &file_size)) {
        printf("Error: Failed to read file %s\n", filename);
        free(file_buffer);
        return -1;
    }

    if (file_size == 0) {
        printf("Error: File is empty or read failed\n");
        free(file_buffer);
        return -1;
    }

    printf("File read successfully: %u bytes\n", file_size);

    // Calculate number of chunks needed
    uint32_t chunk_count = (file_size + PAYLOAD_SIZE - 1) / PAYLOAD_SIZE;

    // Allocate memory for chunks array
    *chunks = (struct Payload*)malloc(sizeof(struct Payload) * chunk_count);
    if (*chunks == NULL) {
        printf("Error: Failed to allocate memory for %u chunks (%u bytes)\n",
               chunk_count, sizeof(struct Payload) * chunk_count);
        free(file_buffer);
        return -1;
    }

    printf("Allocated %u chunks (%u bytes)\n", chunk_count, sizeof(struct Payload) * chunk_count);

    // Calculate file CRC16
    uint16_t file_crc = crc16(file_buffer, file_size);

    // Fill metadata
    strncpy(meta->filename, filename, METADATA_FILENAME_SIZE - 1);
    meta->filename[METADATA_FILENAME_SIZE - 1] = '\0';
    meta->total_size = file_size;
    meta->chunk_count = chunk_count;
    meta->last_modified = 0;  // Would need RTC for actual timestamp
    meta->file_crc = file_crc;

    printf("Deconstructing file: %s\n", filename);
    printf("Total size: %u bytes\n", file_size);
    printf("Chunks: %u (247 bytes each)\n", chunk_count);
    printf("File CRC16: 0x%04X\n", meta->file_crc);

    // Create chunks
    for (uint32_t i = 0; i < chunk_count; i++) {
        uint32_t offset = i * PAYLOAD_SIZE;
        uint32_t remaining = file_size - offset;
        uint32_t chunk_size = (remaining < PAYLOAD_SIZE) ? remaining : PAYLOAD_SIZE;

        // Set sequence number
        (*chunks)[i].sequence = i;

        // Copy data
        memcpy((*chunks)[i].data, file_buffer + offset, chunk_size);
        (*chunks)[i].size = chunk_size;

        // Calculate CRC16 for this chunk
        (*chunks)[i].crc = crc16((*chunks)[i].data, chunk_size);

        if ((i + 1) % 10 == 0 || i == chunk_count - 1) {
            printf("Processed chunk %u/%u\n", i + 1, chunk_count);
        }
    }

    free(file_buffer);
    printf("Deconstruction complete!\n");
    return 0;
}

/**
 * @brief Reconstruct a file from chunks
 * @param meta Pointer to Metadata structure
 * @param chunks Pointer to array of Payload pointers
 * @param output_filename Name of file to write to SD card
 * @return int 0 on success, -1 on failure
 */
int reconstruct(struct Metadata* meta, struct Payload** chunks, char* output_filename) {
    if (meta == NULL || chunks == NULL || *chunks == NULL || output_filename == NULL) {
        printf("Error: NULL parameters\n");
        return -1;
    }

    printf("Reconstructing file: %s\n", output_filename);
    printf("Expected size: %u bytes\n", meta->total_size);
    printf("Expected chunks: %u\n", meta->chunk_count);

    // Verify all chunks first
    uint32_t verified_chunks = 0;
    uint32_t failed_chunks = 0;

    for (uint32_t i = 0; i < meta->chunk_count; i++) {
        if (verify_chunk(&(*chunks)[i])) {
            verified_chunks++;
        } else {
            printf("Warning: Chunk %u failed verification (CRC mismatch)\n", i);
            failed_chunks++;
        }
    }

    printf("Verified: %u/%u chunks\n", verified_chunks, meta->chunk_count);

    if (failed_chunks > 0) {
        printf("Error: %u chunks failed verification - reconstruction aborted\n", failed_chunks);
        return -1;
    }

    // Allocate buffer for complete file
    uint8_t* file_buffer = (uint8_t*)malloc(meta->total_size);
    if (file_buffer == NULL) {
        printf("Error: Memory allocation failed\n");
        return -1;
    }

    // Reconstruct file from chunks
    uint32_t offset = 0;
    for (uint32_t i = 0; i < meta->chunk_count; i++) {
        // Verify sequence number
        if ((*chunks)[i].sequence != i) {
            printf("Error: Chunk %u has wrong sequence number %u\n", i, (*chunks)[i].sequence);
            free(file_buffer);
            return -1;
        }

        // Copy chunk data to buffer
        memcpy(file_buffer + offset, (*chunks)[i].data, (*chunks)[i].size);
        offset += (*chunks)[i].size;

        if ((i + 1) % 100 == 0 || i == meta->chunk_count - 1) {
            printf("Reconstructed chunk %u/%u\n", i + 1, meta->chunk_count);
        }
    }

    // Verify total file size
    if (offset != meta->total_size) {
        printf("Error: Reconstructed size (%u) doesn't match expected size (%u)\n",
               offset, meta->total_size);
        free(file_buffer);
        return -1;
    }

    // Verify file CRC16
    uint16_t calculated_file_crc = crc16(file_buffer, meta->total_size);
    if (calculated_file_crc != meta->file_crc) {
        printf("Error: File CRC16 mismatch - Expected: 0x%04X, Got: 0x%04X\n",
               meta->file_crc, calculated_file_crc);
        free(file_buffer);
        return -1;
    }

    printf("File CRC16 verification passed: 0x%04X\n", calculated_file_crc);

    // Initialize filesystem
    filesystem_info_t fs_info;
    if (!microsd_init_filesystem(&fs_info)) {
        printf("Error: Failed to initialize filesystem\n");
        free(file_buffer);
        return -1;
    }

    // Write reconstructed file to SD card
    if (!microsd_create_file(&fs_info, output_filename, file_buffer, meta->total_size)) {
        printf("Error: Failed to write file to SD card\n");
        free(file_buffer);
        return -1;
    }

    free(file_buffer);
    printf("Reconstruction complete! File saved as: %s\n", output_filename);
    return 0;
}
