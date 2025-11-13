/*!
 * @file    data_frame.c
 * @brief   Data framing and TRUE STREAMING file transfer implementation
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 *
 * TRUE STREAMING IMPLEMENTATION (No Memory Buffer Limits):
 * =========================================================
 * Uses FatFS library for efficient file operations with true streaming support.
 * Previous version: Loaded entire file (up to 128KB) into RAM for CRC calculation
 * Current version: TRUE STREAMING with constant memory usage using FatFS
 *
 * Key improvements:
 * - Uses FatFS f_stat() to get file size without reading file contents
 * - Calculates CRC incrementally as chunks are read (via read_chunk_streaming)
 * - Memory usage: ~0.5 KB constant (single chunk buffer)
 * - No file size limit (can transfer multi-MB files)
 * - Suitable for embedded systems with limited RAM (Pico W has 264KB total)
 *
 * Technical approach:
 * 1. init_streaming_read(): Uses f_stat() to get file size (no data read)
 * 2. read_chunk_streaming(): Uses f_lseek() + f_read() for chunk, updates incremental CRC
 * 3. get_streaming_file_crc(): Returns finalized CRC after all chunks read
 * 4. File is read twice (once for CRC, once for transmission) to maintain low memory
 *
 * This is a proper streaming implementation suitable for constrained embedded systems.
 */

#include "data_frame.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../drivers/microsd_driver.h"
#include "ff.h"        // FatFS library
#include "hw_config.h" // For sd_get_by_num()

/*! Static file context for streaming operations */
#define READ_BUFFER_SIZE (32 * 1024) // 32KB read buffer

typedef struct {
    uint32_t file_size;
    uint32_t file_crc32;       // CRC32 of entire file (for Metadata)
    uint32_t crc_accumulator;  // For incremental CRC32 calculation
    uint32_t chunks_processed; // Number of chunks processed for CRC
    char filename[METADATA_FILENAME_SIZE];
    bool initialized;
    bool crc_finalized; // True when CRC calculation is complete
    FIL file_handle;    // Keep file open during streaming session
    bool file_is_open;  // Track if file handle is valid

    // Read buffering for efficiency
    uint8_t *read_buffer;        // Dynamically allocated 32KB buffer
    uint32_t buffer_file_offset; // File offset of first byte in buffer
    uint32_t buffer_valid_bytes; // Number of valid bytes currently in buffer
} streaming_context_t;

static streaming_context_t g_stream_ctx = {0};

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

    // Use microsd_driver to get file information WITHOUT reading file contents
    FILINFO fno;
    if (!microsd_driver_stat(filename, &fno)) {
        printf("ERROR: Failed to stat file '%s'\n", filename);
        return -1;
    }

    uint32_t file_size = fno.fsize;

    if (file_size == 0) {
        printf("ERROR: File '%s' is empty\n", filename);
        return -1;
    }

    // Store in streaming context
    g_stream_ctx.file_size = file_size;
    g_stream_ctx.file_crc32 = 0xFFFFFFFF; // Will be calculated incrementally as chunks are read
    g_stream_ctx.crc_accumulator = 0xFFFFFFFF; // Initialize CRC32 state
    g_stream_ctx.chunks_processed = 0;
    g_stream_ctx.crc_finalized = false;
    strncpy(g_stream_ctx.filename, filename, METADATA_FILENAME_SIZE - 1);
    g_stream_ctx.filename[METADATA_FILENAME_SIZE - 1] = '\0';

    // Open file once and keep it open for all chunk reads
    if (!microsd_driver_open(&g_stream_ctx.file_handle, filename, FA_READ)) {
        printf("ERROR: Failed to open file '%s' for streaming\n", filename);
        return -1;
    }

    g_stream_ctx.file_is_open = true;
    g_stream_ctx.initialized = true;

    // Allocate 32KB read buffer for efficient SD card access
    g_stream_ctx.read_buffer = (uint8_t *)malloc(READ_BUFFER_SIZE);
    if (!g_stream_ctx.read_buffer) {
        printf("ERROR: Failed to allocate %d byte read buffer\n", READ_BUFFER_SIZE);
        microsd_driver_close(&g_stream_ctx.file_handle);
        g_stream_ctx.file_is_open = false;
        return -1;
    }
    g_stream_ctx.buffer_file_offset = 0;
    g_stream_ctx.buffer_valid_bytes = 0;

    // Fill metadata
    strncpy(meta->filename, filename, METADATA_FILENAME_SIZE - 1);
    meta->filename[METADATA_FILENAME_SIZE - 1] = '\0';
    meta->total_size = file_size;
    meta->chunk_count = (file_size + PAYLOAD_DATA_SIZE - 1) / PAYLOAD_DATA_SIZE;
    meta->last_modified = 0;
    meta->file_crc32 = 0; // Will be set after all chunks are read

    // Generate session ID
    snprintf(meta->session_id, SESSION_ID_SIZE, "stream_%lu",
             (unsigned long)to_ms_since_boot(get_absolute_time()));

    printf("TRUE STREAMING read initialized (FatFS, buffered reading):\n");
    printf("  File: %s\n", filename);
    printf("  Size: %u bytes (%.2f KB)\n", file_size, (float)file_size / 1024.0f);
    printf("  Chunks: %u\n", meta->chunk_count);
    printf("  CRC: Will be calculated incrementally\n");
    printf("  Memory usage: ~32.5 KB (32KB read buffer + chunk)\n");
    printf("  Read buffer: %d KB (reduces SD card seeks)\n", READ_BUFFER_SIZE / 1024);
    printf("  Max file size: UNLIMITED (stream-based)\n");

    return 0;
}

/**
 * @brief Read a single chunk for streaming transmission (TRUE STREAMING)
 *
 * This function reads only the requested chunk from the file without loading
 * the entire file into memory. Much more memory efficient than deconstruct().
 *
 * Additionally, it updates the incremental CRC calculation for the entire file.
 * The file CRC is calculated progressively as each chunk is read in sequence.
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

    // Verify file is still open
    if (!g_stream_ctx.file_is_open) {
        printf("Error: File is not open for streaming\n");
        return -1;
    }

    // Check if requested data is in buffer
    bool need_refill = false;

    if (g_stream_ctx.buffer_valid_bytes == 0) {
        // Buffer is empty, need initial fill
        need_refill = true;
    } else if (file_offset < g_stream_ctx.buffer_file_offset) {
        // Requested offset is before buffer (backward seek - not optimal but handle it)
        need_refill = true;
    } else if (file_offset >= g_stream_ctx.buffer_file_offset + g_stream_ctx.buffer_valid_bytes) {
        // Requested offset is after buffer
        need_refill = true;
    } else if (file_offset + chunk_size >
               g_stream_ctx.buffer_file_offset + g_stream_ctx.buffer_valid_bytes) {
        // Chunk spans beyond buffer end
        need_refill = true;
    }

    // Refill buffer if needed
    if (need_refill) {
        // Seek to chunk offset
        if (!microsd_driver_seek(&g_stream_ctx.file_handle, file_offset)) {
            printf("Error: Failed to seek to offset %lu, attempting file reopen...\n",
                   (unsigned long)file_offset);

            // Try to recover by reopening the file
            microsd_driver_close(&g_stream_ctx.file_handle);
            if (!microsd_driver_open(&g_stream_ctx.file_handle, g_stream_ctx.filename, FA_READ)) {
                printf("Error: Failed to reopen file for recovery\n");
                g_stream_ctx.file_is_open = false;
                return -1;
            }

            // Try seek again
            if (!microsd_driver_seek(&g_stream_ctx.file_handle, file_offset)) {
                printf("Error: Failed to seek after reopen\n");
                return -1;
            }
            printf("  File reopened and seek successful\n");
        }

        // Read up to 32KB into buffer
        UINT bytes_read = 0;
        uint32_t bytes_to_read = READ_BUFFER_SIZE;
        if (file_offset + bytes_to_read > g_stream_ctx.file_size) {
            bytes_to_read = g_stream_ctx.file_size - file_offset;
        }

        if (!microsd_driver_read(&g_stream_ctx.file_handle, g_stream_ctx.read_buffer, bytes_to_read,
                                 &bytes_read)) {
            printf("Error: Failed to read buffer from file, attempting file reopen...\n");

            // Try to recover by reopening the file
            microsd_driver_close(&g_stream_ctx.file_handle);
            if (!microsd_driver_open(&g_stream_ctx.file_handle, g_stream_ctx.filename, FA_READ)) {
                printf("Error: Failed to reopen file for recovery\n");
                g_stream_ctx.file_is_open = false;
                return -1;
            }

            // Seek and read again
            if (!microsd_driver_seek(&g_stream_ctx.file_handle, file_offset)) {
                printf("Error: Failed to seek after reopen\n");
                return -1;
            }

            if (!microsd_driver_read(&g_stream_ctx.file_handle, g_stream_ctx.read_buffer,
                                     bytes_to_read, &bytes_read)) {
                printf("Error: Failed to read buffer even after file reopen\n");
                return -1;
            }
            printf("  File reopened, seek and read successful\n");
        }

        g_stream_ctx.buffer_file_offset = file_offset;
        g_stream_ctx.buffer_valid_bytes = bytes_read;
    }

    // Copy chunk data from buffer
    uint32_t buffer_offset = file_offset - g_stream_ctx.buffer_file_offset;
    memcpy(chunk->data, g_stream_ctx.read_buffer + buffer_offset, chunk_size);

    // Zero-pad the remaining bytes if chunk is smaller than PAYLOAD_DATA_SIZE
    if (chunk_size < PAYLOAD_DATA_SIZE) {
        memset(chunk->data + chunk_size, 0, PAYLOAD_DATA_SIZE - chunk_size);
    }

    // Set chunk sequence number (data chunks start from sequence 1, metadata uses 0xFFFFFFFF)
    chunk->sequence = chunk_index + 1;

    // Calculate CRC32 for the entire data field (239 bytes, including padding)
    chunk->crc32 = crc32(chunk->data, PAYLOAD_DATA_SIZE);

    // Update incremental CRC32 for entire file (if reading chunks sequentially)
    if (chunk_index == g_stream_ctx.chunks_processed && !g_stream_ctx.crc_finalized) {
        // Continue CRC32 calculation on actual file data (not padding)
        uint32_t crc = g_stream_ctx.crc_accumulator;

        for (uint32_t i = 0; i < chunk_size; i++) {
            crc ^= chunk->data[i];
            for (int j = 0; j < 8; j++) {
                if (crc & 1) {
                    crc = (crc >> 1) ^ 0xEDB88320; // CRC32 polynomial
                } else {
                    crc = crc >> 1;
                }
            }
        }

        g_stream_ctx.crc_accumulator = crc;
        g_stream_ctx.chunks_processed++;

        // If this was the last chunk, finalize the CRC32
        if (chunk_index == chunk_count - 1) {
            g_stream_ctx.file_crc32 = ~crc; // Final XOR
            g_stream_ctx.crc_finalized = true;
            printf("  File CRC32 calculated: 0x%08X\n", g_stream_ctx.file_crc32);
        }
    }

    return 0;
}

/**
 * @brief Get the finalized file CRC32 after all chunks have been read
 * @return uint32_t File CRC32 checksum, or 0 if not finalized
 */
uint32_t get_streaming_file_crc(void) {
    if (!g_stream_ctx.initialized || !g_stream_ctx.crc_finalized) {
        printf("Warning: File CRC32 not yet finalized\n");
        return 0;
    }
    return g_stream_ctx.file_crc32;
}

/**
 * @brief Clean up streaming context
 */
void cleanup_streaming_read(void) {
    // Close the file if it's still open
    if (g_stream_ctx.file_is_open) {
        microsd_driver_close(&g_stream_ctx.file_handle);
        g_stream_ctx.file_is_open = false;
    }

    // Free the read buffer
    if (g_stream_ctx.read_buffer) {
        free(g_stream_ctx.read_buffer);
        g_stream_ctx.read_buffer = NULL;
    }

    g_stream_ctx.initialized = false;
    g_stream_ctx.crc_finalized = false;
    g_stream_ctx.chunks_processed = 0;
    g_stream_ctx.buffer_valid_bytes = 0;
}

/**
 * @brief Verify integrity of a chunk using CRC32
 * @param chunk Pointer to Payload structure
 * @return int 1 if valid, 0 if invalid
 */
int verify_chunk(struct Payload *chunk) {
    if (chunk == NULL) {
        return 0;
    }

    // Calculate CRC32 of the entire data field (239 bytes)
    uint32_t calculated_crc = crc32(chunk->data, PAYLOAD_DATA_SIZE);

    // Compare with stored CRC
    return (calculated_crc == chunk->crc32) ? 1 : 0;
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

    // Get file size using microsd_driver
    FILINFO fno;
    if (!microsd_driver_stat(filename, &fno)) {
        printf("Error: Failed to stat file\n");
        return -1;
    }

    uint32_t file_size = fno.fsize;
    if (file_size == 0) {
        printf("Error: File is empty\n");
        return -1;
    }

// Use a reasonably sized buffer (32KB should be enough for most files on Pico)
#define MAX_FILE_BUFFER_SIZE (32 * 1024)

    // Check if file size exceeds buffer capacity (prevent silent truncation)
    if (file_size > MAX_FILE_BUFFER_SIZE) {
        printf("Error: File too large (%u bytes) for legacy deconstruct (max %u bytes)\n",
               file_size, MAX_FILE_BUFFER_SIZE);
        printf("       Use init_streaming_read() and read_chunk_streaming() instead\n");
        return -1;
    }

    uint8_t *file_buffer = (uint8_t *)malloc(file_size);
    if (file_buffer == NULL) {
        printf("Error: Memory allocation failed for file buffer (%u bytes)\n", file_size);
        return -1;
    }

    // Read the file using microsd_driver
    size_t bytes_read = 0;
    if (!microsd_driver_read_file(filename, file_buffer, file_size, &bytes_read)) {
        printf("Error: Failed to read file\n");
        free(file_buffer);
        return -1;
    }

    if (bytes_read != file_size) {
        printf("Error: Failed to read file (read %zu/%u bytes)\n", bytes_read, file_size);
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

    // Calculate file CRC32
    uint32_t file_crc = crc32(file_buffer, file_size);

    // Fill metadata
    strncpy(meta->filename, filename, METADATA_FILENAME_SIZE - 1);
    meta->filename[METADATA_FILENAME_SIZE - 1] = '\0';
    meta->total_size = file_size;
    meta->chunk_count = chunk_count;
    meta->last_modified = 0; // Would need RTC for actual timestamp
    meta->file_crc32 = file_crc;

    printf("Deconstructing file: %s\n", filename);
    printf("Total size: %u bytes\n", file_size);
    printf("Chunks: %u (%d bytes data per chunk)\n", chunk_count, PAYLOAD_DATA_SIZE);
    printf("File CRC32: 0x%08X\n", meta->file_crc32);

    // Create chunks
    for (uint32_t i = 0; i < chunk_count; i++) {
        uint32_t offset = i * PAYLOAD_DATA_SIZE;
        uint32_t remaining = file_size - offset;
        uint32_t chunk_size = (remaining < PAYLOAD_DATA_SIZE) ? remaining : PAYLOAD_DATA_SIZE;

        // Set sequence number (data chunks start from 1)
        (*chunks)[i].sequence = i + 1;

        // Copy data
        memcpy((*chunks)[i].data, file_buffer + offset, chunk_size);

        // Zero-pad the remaining bytes if chunk is smaller than PAYLOAD_DATA_SIZE
        if (chunk_size < PAYLOAD_DATA_SIZE) {
            memset((*chunks)[i].data + chunk_size, 0, PAYLOAD_DATA_SIZE - chunk_size);
        }

        // Calculate CRC32 for the entire data field (239 bytes, including padding)
        (*chunks)[i].crc32 = crc32((*chunks)[i].data, PAYLOAD_DATA_SIZE);

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
        // Verify sequence number (data chunks start from 1)
        if ((*chunks)[i].sequence != (i + 1)) {
            printf("Error: Chunk %u has wrong sequence number %u\n", i, (*chunks)[i].sequence);
            free(file_buffer);
            return -1;
        }

        // Calculate actual data size for this chunk
        uint32_t remaining = meta->total_size - offset;
        uint32_t chunk_data_size = (remaining < PAYLOAD_DATA_SIZE) ? remaining : PAYLOAD_DATA_SIZE;

        // Copy chunk data to buffer
        memcpy(file_buffer + offset, (*chunks)[i].data, chunk_data_size);
        offset += chunk_data_size;

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

    // Verify file CRC32
    uint32_t calculated_file_crc = crc32(file_buffer, meta->total_size);
    if (calculated_file_crc != meta->file_crc32) {
        printf("Error: File CRC32 mismatch - Expected: 0x%08X, Got: 0x%08X\n", meta->file_crc32,
               calculated_file_crc);
        free(file_buffer);
        return -1;
    }

    printf("File CRC32 verification passed: 0x%08X\n", calculated_file_crc);

    // Write reconstructed file to SD card using microsd_driver
    if (!microsd_driver_write_file(output_filename, file_buffer, meta->total_size)) {
        printf("Error: Failed to write file\n");
        free(file_buffer);
        return -1;
    }

    free(file_buffer);
    printf("Reconstruction complete! File saved as: %s\n", output_filename);
    return 0;
}

/**
 * @brief Calculate CRC32 checksum for data buffer
 * @param data Pointer to data buffer
 * @param length Length of data in bytes
 * @return uint32_t CRC32 checksum value
 */
uint32_t crc32(const uint8_t *data, size_t length) {
    uint32_t crc = 0xFFFFFFFF; // Initial value

    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320; // CRC32 polynomial
            } else {
                crc = crc >> 1;
            }
        }
    }

    return ~crc; // Final XOR
}

/**
 * @brief Serialize Payload struct into a 247-byte buffer for MQTT transmission
 * Format: [4 bytes sequence][239 bytes data][4 bytes CRC32]
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

    // Write data (239 bytes)
    memcpy(ptr, payload->data, PAYLOAD_DATA_SIZE);
    ptr += PAYLOAD_DATA_SIZE;

    // Write CRC32 (4 bytes, little-endian)
    *ptr++ = (uint8_t)(payload->crc32 & 0xFF);
    *ptr++ = (uint8_t)((payload->crc32 >> 8) & 0xFF);
    *ptr++ = (uint8_t)((payload->crc32 >> 16) & 0xFF);
    *ptr++ = (uint8_t)((payload->crc32 >> 24) & 0xFF);

    return (int)(ptr - buffer); // Should be 247 bytes
}

/**
 * @brief Deserialize 247-byte buffer into Payload struct after MQTT reception
 * Format: [4 bytes sequence][239 bytes data][4 bytes CRC32]
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

    // Read data (239 bytes)
    memcpy(payload->data, ptr, PAYLOAD_DATA_SIZE);
    ptr += PAYLOAD_DATA_SIZE;

    // Read CRC32 (4 bytes, little-endian)
    payload->crc32 = (uint32_t)ptr[0] | ((uint32_t)ptr[1] << 8) | ((uint32_t)ptr[2] << 16) |
                     ((uint32_t)ptr[3] << 24);
    ptr += 4;

    return 0;
}

/**
 * @brief Serialize Metadata struct into a Payload-formatted 247-byte buffer for MQTT transmission
 * Format: [4 bytes sequence=0xFFFFFFFF][metadata in data field][padding to 247 bytes]
 * @param metadata Pointer to Metadata structure to serialize
 * @param buffer Pointer to output buffer (must be at least PAYLOAD_SIZE bytes)
 * @return int Number of bytes written, or -1 on error
 */
int serialize_metadata(struct Metadata *metadata, uint8_t *buffer) {
    if (!metadata || !buffer) {
        return -1;
    }

    uint8_t *ptr = buffer;

    // Write sequence = 0xFFFFFFFF to indicate metadata packet (4 bytes, little-endian)
    *ptr++ = 0xFF;
    *ptr++ = 0xFF;
    *ptr++ = 0xFF;
    *ptr++ = 0xFF;

    // Write metadata into the data field (110 bytes total)
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

    // Write file_crc32 (4 bytes, little-endian)
    *ptr++ = (uint8_t)(metadata->file_crc32 & 0xFF);
    *ptr++ = (uint8_t)((metadata->file_crc32 >> 8) & 0xFF);
    *ptr++ = (uint8_t)((metadata->file_crc32 >> 16) & 0xFF);
    *ptr++ = (uint8_t)((metadata->file_crc32 >> 24) & 0xFF);

    // Fill remaining bytes with zeros to make it 247 bytes total
    size_t bytes_written = ptr - buffer; // Should be 4 + 112 = 116 bytes
    if (bytes_written < PAYLOAD_SIZE) {
        memset(ptr, 0, PAYLOAD_SIZE - bytes_written);
    }

    return PAYLOAD_SIZE; // Always return 247 bytes for consistency
}

/**
 * @brief Deserialize 247-byte Payload-formatted buffer into Metadata struct after MQTT reception
 * Format: [4 bytes sequence=0xFFFFFFFF][metadata in data field][padding]
 * @param buffer Pointer to input buffer containing serialized data
 * @param metadata Pointer to Metadata structure to populate
 * @return int 0 on success, -1 on error
 */
int deserialize_metadata(uint8_t *buffer, struct Metadata *metadata) {
    if (!buffer || !metadata) {
        return -1;
    }

    uint8_t *ptr = buffer;

    // Skip sequence field (4 bytes) - should be 0xFFFFFFFF but we don't validate here
    ptr += 4;

    // Read metadata from data field (110 bytes total)
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

    // Read file_crc32 (4 bytes, little-endian)
    metadata->file_crc32 = (uint32_t)ptr[0] | ((uint32_t)ptr[1] << 8) | ((uint32_t)ptr[2] << 16) |
                           ((uint32_t)ptr[3] << 24);
    ptr += 4;

    return 0;
}