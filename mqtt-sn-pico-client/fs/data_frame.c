#include "data_frame.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../drivers/microsd_driver.h"

/*! Static file context for streaming operations */
typedef struct {
    filesystem_info_t fs_info;
    uint32_t file_cluster;
    uint32_t file_size;
    uint16_t file_crc;
    char filename[METADATA_FILENAME_SIZE];
    bool initialized;
} streaming_context_t;

static streaming_context_t g_stream_ctx = {0};

/**
 * @brief Calculate CRC16 checksum for data buffer (CCITT-FALSE)
 * @param data Pointer to data buffer
 * @param length Length of data in bytes
 * @return uint16_t CRC16 checksum value
 */
uint16_t crc16(unsigned char *data, size_t length) {
    uint16_t crc = 0xFFFF; // Initial value for CCITT-FALSE

    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;

        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021; // CCITT polynomial
            } else {
                crc = crc << 1;
            }
        }
    }

    return crc; // Return calculated CRC value
}

/**
 * @brief Initialize streaming file read for chunked transmission
 *
 * This function prepares for streaming chunk reads without loading the entire
 * file into memory. It only reads file metadata and caches cluster information.
 *
 * @param filename Name of file to prepare for streaming
 * @param meta Pointer to Metadata structure to populate
 * @return int 0 on success, -1 on failure
 */
int init_streaming_read(char *filename, struct Metadata *meta) {
    if (filename == NULL || meta == NULL) {
        printf("Error: NULL parameters\n");
        return -1;
    }

    // Initialize filesystem if needed
    if (!g_stream_ctx.initialized) {
        if (!microsd_init_filesystem(&g_stream_ctx.fs_info)) {
            printf("Error: Failed to initialize filesystem\n");
            return -1;
        }
    }

// Read file to get metadata (we need size and CRC)
// Get the actual file size from directory entry by using a large enough buffer size
// The read function will return the actual bytes read, which will be min(file_size, buffer_size)
// To get the real file size, we need to try reading with increasingly larger buffers until
// bytes_read < buffer_size, indicating we've read the entire file

// HARD LIMIT: Raspberry Pi Pico W has 264KB total SRAM
// Reserve space for stack, heap, and other operations
#define STREAMING_FILE_SIZE_HARD_LIMIT (131072U) // 128KB maximum file size

    uint32_t file_size = 0;
    uint8_t *full_buffer = NULL;

    // Progressively calculate buffer sizes at runtime, doubling each time
    // Start at 4KB, double until we reach the hard limit
    uint32_t test_buffer_size = 4096; // Start with 4KB

    while (test_buffer_size <= STREAMING_FILE_SIZE_HARD_LIMIT) {
        uint8_t *test_buffer = (uint8_t *)malloc(test_buffer_size);
        if (test_buffer == NULL) {
            printf("ERROR: Memory allocation failed for %u bytes\n", test_buffer_size);
            printf("       Available RAM insufficient for file operation\n");
            if (full_buffer)
                free(full_buffer);
            return -1;
        }

        uint32_t bytes_read = 0;
        if (!microsd_read_file(&g_stream_ctx.fs_info, filename, test_buffer, test_buffer_size,
                               &bytes_read)) {
            printf("ERROR: Failed to read file '%s'\n", filename);
            free(test_buffer);
            if (full_buffer)
                free(full_buffer);
            return -1;
        }

        if (bytes_read == 0) {
            printf("ERROR: File '%s' is empty\n", filename);
            free(test_buffer);
            if (full_buffer)
                free(full_buffer);
            return -1;
        }

        // If we read less than buffer size, we have the entire file
        if (bytes_read < test_buffer_size) {
            file_size = bytes_read;
            full_buffer = test_buffer; // Keep this buffer
            break;
        }

        // If we filled the buffer, need to try a larger size
        free(test_buffer);

        // Check if we've hit the hard limit
        if (test_buffer_size == STREAMING_FILE_SIZE_HARD_LIMIT) {
            printf("ERROR: HARD_LIMIT_REACHED\n");
            printf("       File '%s' exceeds maximum supported size (%u bytes / %u KB)\n", filename,
                   STREAMING_FILE_SIZE_HARD_LIMIT, STREAMING_FILE_SIZE_HARD_LIMIT / 1024);
            printf("       This limit is imposed by Raspberry Pi Pico W's 264KB SRAM\n");
            printf(
                "       For larger files, use chunk-based streaming without full CRC validation\n");
            if (full_buffer)
                free(full_buffer);
            return -1;
        }

        // Double the buffer size for next attempt, but cap at hard limit
        test_buffer_size *= 2;
        if (test_buffer_size > STREAMING_FILE_SIZE_HARD_LIMIT) {
            test_buffer_size = STREAMING_FILE_SIZE_HARD_LIMIT;
        }
    }
    if (full_buffer == NULL || file_size == 0) {
        printf("Error: Failed to read file\n");
        return -1;
    }

    // Calculate file CRC from full file content
    uint16_t file_crc = crc16(full_buffer, file_size);

    free(full_buffer);

    // Store in streaming context
    g_stream_ctx.file_size = file_size;
    g_stream_ctx.file_crc = file_crc;
    strncpy(g_stream_ctx.filename, filename, METADATA_FILENAME_SIZE - 1);
    g_stream_ctx.filename[METADATA_FILENAME_SIZE - 1] = '\0';
    g_stream_ctx.initialized = true;

    // Fill metadata
    strncpy(meta->filename, filename, METADATA_FILENAME_SIZE - 1);
    meta->filename[METADATA_FILENAME_SIZE - 1] = '\0';
    meta->total_size = file_size;
    meta->chunk_count = (file_size + PAYLOAD_DATA_SIZE - 1) / PAYLOAD_DATA_SIZE;
    meta->last_modified = 0;
    meta->file_crc = file_crc;

    // Generate session ID
    snprintf(meta->session_id, SESSION_ID_SIZE, "stream_%lu",
             (unsigned long)to_ms_since_boot(get_absolute_time()));

    printf("Streaming read initialized:\n");
    printf("  File: %s\n", filename);
    printf("  Size: %u bytes\n", file_size);
    printf("  Chunks: %u\n", meta->chunk_count);
    printf("  CRC: 0x%04X\n", file_crc);

    return 0;
}

/**
 * @brief Read a single chunk for streaming transmission
 *
 * This function reads only the requested chunk from the file without loading
 * the entire file into memory. Much more memory efficient than deconstruct().
 *
 * @param chunk_index Index of chunk to read (0-based)
 * @param chunk Pointer to Payload structure to populate
 * @return int 0 on success, -1 on failure
 */
int read_chunk_streaming(uint32_t chunk_index, struct Payload *chunk) {
    if (chunk == NULL) {
        printf("Error: NULL chunk pointer\n");
        return -1;
    }

    if (!g_stream_ctx.initialized) {
        printf("Error: Streaming context not initialized\n");
        return -1;
    }

    uint32_t chunk_count = (g_stream_ctx.file_size + PAYLOAD_DATA_SIZE - 1) / PAYLOAD_DATA_SIZE;

    if (chunk_index >= chunk_count) {
        printf("Error: Chunk index %lu out of range (max %lu)\n", (unsigned long)chunk_index,
               (unsigned long)chunk_count);
        return -1;
    }

    // Calculate file offset and size for this chunk
    uint32_t file_offset = chunk_index * PAYLOAD_DATA_SIZE;
    uint32_t remaining = g_stream_ctx.file_size - file_offset;
    uint32_t chunk_size = (remaining < PAYLOAD_DATA_SIZE) ? remaining : PAYLOAD_DATA_SIZE;

    // Read chunk directly from file using microsd_read_chunk
    uint32_t bytes_read = 0;
    if (!microsd_read_chunk(&g_stream_ctx.fs_info, g_stream_ctx.filename, chunk->data,
                            PAYLOAD_DATA_SIZE,
                            chunk_index + 1, // microsd_read_chunk uses 1-based indexing
                            &bytes_read)) {
        printf("Error: Failed to read chunk %lu from file\n", (unsigned long)chunk_index);
        return -1;
    }

    // Set chunk metadata
    chunk->sequence = chunk_index + 1; // Data chunks start from sequence 1 (0 is metadata)
    chunk->size = bytes_read;
    chunk->crc = crc16(chunk->data, bytes_read);

    return 0;
}

/**
 * @brief Clean up streaming context
 */
void cleanup_streaming_read(void) { g_stream_ctx.initialized = false; }

/**
 * @brief Verify integrity of a chunk using CRC16
 * @param chunk Pointer to Payload structure
 * @return int 1 if valid, 0 if invalid
 */
int verify_chunk(struct Payload *chunk) {
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
int deconstruct(char *filename, struct Metadata *meta, struct Payload **chunks) {
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
    uint8_t *file_buffer = (uint8_t *)malloc(MAX_FILE_BUFFER_SIZE);
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

    // Check if file size exceeds buffer capacity (prevent silent truncation)
    if (file_size >= MAX_FILE_BUFFER_SIZE) {
        printf("Error: File too large (%u bytes) for legacy deconstruct (max %u bytes)\n",
               file_size, MAX_FILE_BUFFER_SIZE);
        printf("       Use init_streaming_read() and read_chunk_streaming() instead\n");
        free(file_buffer);
        return -1;
    }

    printf("File read successfully: %u bytes\n", file_size);

    // Calculate number of chunks needed (based on actual data capacity)
    uint32_t chunk_count = (file_size + PAYLOAD_DATA_SIZE - 1) / PAYLOAD_DATA_SIZE;

    // Allocate memory for chunks array
    *chunks = (struct Payload *)malloc(sizeof(struct Payload) * chunk_count);
    if (*chunks == NULL) {
        printf("Error: Failed to allocate memory for %u chunks (%u bytes)\n", chunk_count,
               sizeof(struct Payload) * chunk_count);
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
    meta->last_modified = 0; // Would need RTC for actual timestamp
    meta->file_crc = file_crc;

    printf("Deconstructing file: %s\n", filename);
    printf("Total size: %u bytes\n", file_size);
    printf("Chunks: %u (%d bytes data per chunk)\n", chunk_count, PAYLOAD_DATA_SIZE);
    printf("File CRC16: 0x%04X\n", meta->file_crc);

    // Create chunks
    for (uint32_t i = 0; i < chunk_count; i++) {
        uint32_t offset = i * PAYLOAD_DATA_SIZE;
        uint32_t remaining = file_size - offset;
        uint32_t chunk_size = (remaining < PAYLOAD_DATA_SIZE) ? remaining : PAYLOAD_DATA_SIZE;

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
int reconstruct(struct Metadata *meta, struct Payload **chunks, char *output_filename) {
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
    uint8_t *file_buffer = (uint8_t *)malloc(meta->total_size);
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
        printf("Error: Reconstructed size (%u) doesn't match expected size (%u)\n", offset,
               meta->total_size);
        free(file_buffer);
        return -1;
    }

    // Verify file CRC16
    uint16_t calculated_file_crc = crc16(file_buffer, meta->total_size);
    if (calculated_file_crc != meta->file_crc) {
        printf("Error: File CRC16 mismatch - Expected: 0x%04X, Got: 0x%04X\n", meta->file_crc,
               calculated_file_crc);
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

/**
 * @brief Serialize Payload struct into a 247-byte buffer for MQTT transmission
 * @param payload Pointer to Payload structure to serialize
 * @param buffer Pointer to output buffer (must be at least PAYLOAD_SIZE bytes)
 * @return int Number of bytes written, or -1 on error
 */
int serialize_payload(struct Payload *payload, uint8_t *buffer) {
    if (!payload || !buffer) {
        return -1;
    }

    uint8_t *ptr = buffer;

    // Write sequence (4 bytes, little-endian)
    *ptr++ = (uint8_t)(payload->sequence & 0xFF);
    *ptr++ = (uint8_t)((payload->sequence >> 8) & 0xFF);
    *ptr++ = (uint8_t)((payload->sequence >> 16) & 0xFF);
    *ptr++ = (uint8_t)((payload->sequence >> 24) & 0xFF);

    // Write data (237 bytes)
    memcpy(ptr, payload->data, PAYLOAD_DATA_SIZE);
    ptr += PAYLOAD_DATA_SIZE;

    // Write size (4 bytes, little-endian)
    *ptr++ = (uint8_t)(payload->size & 0xFF);
    *ptr++ = (uint8_t)((payload->size >> 8) & 0xFF);
    *ptr++ = (uint8_t)((payload->size >> 16) & 0xFF);
    *ptr++ = (uint8_t)((payload->size >> 24) & 0xFF);

    // Write CRC (2 bytes, little-endian)
    *ptr++ = (uint8_t)(payload->crc & 0xFF);
    *ptr++ = (uint8_t)((payload->crc >> 8) & 0xFF);

    return (int)(ptr - buffer); // Should be 247 bytes
}

/**
 * @brief Deserialize 247-byte buffer into Payload struct after MQTT reception
 * @param buffer Pointer to input buffer containing serialized data
 * @param payload Pointer to Payload structure to populate
 * @return int 0 on success, -1 on error
 */
int deserialize_payload(uint8_t *buffer, struct Payload *payload) {
    if (!buffer || !payload) {
        return -1;
    }

    uint8_t *ptr = buffer;

    // Read sequence (4 bytes, little-endian)
    payload->sequence = (uint32_t)ptr[0] | ((uint32_t)ptr[1] << 8) | ((uint32_t)ptr[2] << 16) |
                        ((uint32_t)ptr[3] << 24);
    ptr += 4;

    // Read data (237 bytes)
    memcpy(payload->data, ptr, PAYLOAD_DATA_SIZE);
    ptr += PAYLOAD_DATA_SIZE;

    // Read size (4 bytes, little-endian)
    payload->size = (uint32_t)ptr[0] | ((uint32_t)ptr[1] << 8) | ((uint32_t)ptr[2] << 16) |
                    ((uint32_t)ptr[3] << 24);
    ptr += 4;

    // Read CRC (2 bytes, little-endian)
    payload->crc = (uint16_t)ptr[0] | ((uint16_t)ptr[1] << 8);
    ptr += 2;

    return 0;
}

/**
 * @brief Serialize Metadata struct into a 247-byte buffer for MQTT transmission
 * @param metadata Pointer to Metadata structure to serialize
 * @param buffer Pointer to output buffer (must be at least PAYLOAD_SIZE bytes)
 * @return int Number of bytes written, or -1 on error
 */
int serialize_metadata(struct Metadata *metadata, uint8_t *buffer) {
    if (!metadata || !buffer) {
        return -1;
    }

    uint8_t *ptr = buffer;

    // Write session_id (32 bytes)
    memcpy(ptr, metadata->session_id, SESSION_ID_SIZE);
    ptr += SESSION_ID_SIZE;

    // Write filename (64 bytes)
    memcpy(ptr, metadata->filename, METADATA_FILENAME_SIZE);
    ptr += METADATA_FILENAME_SIZE;

    // Write total_size (4 bytes, little-endian)
    *ptr++ = (uint8_t)(metadata->total_size & 0xFF);
    *ptr++ = (uint8_t)((metadata->total_size >> 8) & 0xFF);
    *ptr++ = (uint8_t)((metadata->total_size >> 16) & 0xFF);
    *ptr++ = (uint8_t)((metadata->total_size >> 24) & 0xFF);

    // Write chunk_count (4 bytes, little-endian)
    *ptr++ = (uint8_t)(metadata->chunk_count & 0xFF);
    *ptr++ = (uint8_t)((metadata->chunk_count >> 8) & 0xFF);
    *ptr++ = (uint8_t)((metadata->chunk_count >> 16) & 0xFF);
    *ptr++ = (uint8_t)((metadata->chunk_count >> 24) & 0xFF);

    // Write last_modified (4 bytes, little-endian)
    *ptr++ = (uint8_t)(metadata->last_modified & 0xFF);
    *ptr++ = (uint8_t)((metadata->last_modified >> 8) & 0xFF);
    *ptr++ = (uint8_t)((metadata->last_modified >> 16) & 0xFF);
    *ptr++ = (uint8_t)((metadata->last_modified >> 24) & 0xFF);

    // Write file_crc (2 bytes, little-endian)
    *ptr++ = (uint8_t)(metadata->file_crc & 0xFF);
    *ptr++ = (uint8_t)((metadata->file_crc >> 8) & 0xFF);

    // Fill remaining bytes with zeros to make it 247 bytes total
    size_t bytes_written = ptr - buffer; // Should be 110 bytes
    if (bytes_written < PAYLOAD_SIZE) {
        memset(ptr, 0, PAYLOAD_SIZE - bytes_written);
    }

    return PAYLOAD_SIZE; // Always return 247 bytes for consistency
}

/**
 * @brief Deserialize 247-byte buffer into Metadata struct after MQTT reception
 * @param buffer Pointer to input buffer containing serialized data
 * @param metadata Pointer to Metadata structure to populate
 * @return int 0 on success, -1 on error
 */
int deserialize_metadata(uint8_t *buffer, struct Metadata *metadata) {
    if (!buffer || !metadata) {
        return -1;
    }

    uint8_t *ptr = buffer;

    // Read session_id (32 bytes)
    memcpy(metadata->session_id, ptr, SESSION_ID_SIZE);
    metadata->session_id[SESSION_ID_SIZE - 1] = '\0'; // Ensure null termination
    ptr += SESSION_ID_SIZE;

    // Read filename (64 bytes)
    memcpy(metadata->filename, ptr, METADATA_FILENAME_SIZE);
    metadata->filename[METADATA_FILENAME_SIZE - 1] = '\0'; // Ensure null termination
    ptr += METADATA_FILENAME_SIZE;

    // Read total_size (4 bytes, little-endian)
    metadata->total_size = (uint32_t)ptr[0] | ((uint32_t)ptr[1] << 8) | ((uint32_t)ptr[2] << 16) |
                           ((uint32_t)ptr[3] << 24);
    ptr += 4;

    // Read chunk_count (4 bytes, little-endian)
    metadata->chunk_count = (uint32_t)ptr[0] | ((uint32_t)ptr[1] << 8) | ((uint32_t)ptr[2] << 16) |
                            ((uint32_t)ptr[3] << 24);
    ptr += 4;

    // Read last_modified (4 bytes, little-endian)
    metadata->last_modified = (uint32_t)ptr[0] | ((uint32_t)ptr[1] << 8) |
                              ((uint32_t)ptr[2] << 16) | ((uint32_t)ptr[3] << 24);
    ptr += 4;

    // Read file_crc (2 bytes, little-endian)
    metadata->file_crc = (uint16_t)ptr[0] | ((uint16_t)ptr[1] << 8);
    ptr += 2;

    return 0;
}