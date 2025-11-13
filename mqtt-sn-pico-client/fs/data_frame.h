#ifndef DATA_FRAME_H
#define DATA_FRAME_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// MQTT QoS1 payload size (247 bytes total: 4 bytes sequence + 239 bytes data + 4 bytes CRC32)
#define PAYLOAD_SIZE 247
#define SEQUENCE_SIZE 4
#define CRC32_SIZE 4

// Maximum data size in Payload to fit within 247 bytes when serialized
// struct Payload serialized size: 4 (sequence) + 239 (data) + 4 (crc32) = 247 bytes
// 239 bytes optimizes for 32KB SD card allocation (32768 / 239 ≈ 137 chunks with minimal waste)
#define PAYLOAD_DATA_SIZE 239

// Special sequence number to indicate metadata packet
#define SEQUENCE_METADATA 0xFFFFFFFFU

// datagram packet struct for MQTT QoS1 data transfer (fits in 247 bytes when serialized)
struct Payload {
    uint32_t sequence;               // Chunk sequence number (4 bytes) - 0xFFFFFFFF = metadata
    uint8_t data[PAYLOAD_DATA_SIZE]; // Actual data payload (239 bytes)
    uint32_t crc32;        // CRC32 checksum of data (4 bytes) - transmitted for verification
} __attribute__((packed)); // Total: 247 bytes

// Metadata for file transfer session (embedded in Payload with sequence = 0xFFFFFFFF)
// struct Metadata serialized size: 32 (session_id) + 64 (filename) + 4 (total_size) +
//                                  4 (chunk_count) + 4 (last_modified) + 4 (file_crc32) = 112 bytes
// This fits within the 239-byte data field of a Payload structure
#define SESSION_ID_SIZE 32
#define METADATA_FILENAME_SIZE 64

struct Metadata {
    char session_id[SESSION_ID_SIZE];      // Session ID to track transfer type (32 bytes)
    char filename[METADATA_FILENAME_SIZE]; // Filename (64 bytes)
    uint32_t total_size;                   // Total file size in bytes (4 bytes)
    uint32_t chunk_count;                  // Number of data chunks (4 bytes)
    uint32_t last_modified;                // Unix timestamp (exFAT) (4 bytes)
    uint32_t file_crc32;                   // CRC32 of entire file (4 bytes)
} __attribute__((packed));                 // Total: 112 bytes (fits in 239-byte Payload.data)

/**
 * @brief Read a file and break it into chunks for MQTT QoS1 transmission
 * @param filename Name of file to read from SD card
 * @param meta Pointer to Metadata structure (output)
 * @param chunks Pointer to array of Payload pointers (output - will be allocated)
 * @return int 0 on success, -1 on failure
 */
int deconstruct(char *filename, struct Metadata *meta, struct Payload **chunks);

/**
 * @brief Reconstruct a file from chunks with integrity verification
 * @param meta Pointer to Metadata structure
 * @param chunks Pointer to array of Payload pointers
 * @param output_filename Name of file to write to SD card
 * @return int 0 on success, -1 on failure
 */
int reconstruct(struct Metadata *meta, struct Payload **chunks, char *output_filename);

/**
 * @brief Verify integrity of a single chunk using CRC32
 * @param chunk Pointer to Payload structure
 * @return int 1 if valid, 0 if invalid
 */
int verify_chunk(struct Payload *chunk);

/**
 * @brief Calculate CRC16 checksum for data buffer (CCITT-FALSE)
 * @param data Pointer to data buffer
 * @param length Length of data in bytes
 * @return uint16_t CRC16 checksum value
 */
unsigned short crc16(const char *data, int length);

/**
 * @brief Calculate CRC32 checksum for data buffer
 * @param data Pointer to data buffer
 * @param length Length of data in bytes
 * @return uint32_t CRC32 checksum value
 */
uint32_t crc32(const uint8_t *data, size_t length);

/**
 * @brief Serialize Payload struct into a 247-byte buffer for MQTT transmission
 * Format: [4 bytes sequence][239 bytes data][4 bytes CRC32]
 * @param payload Pointer to Payload structure to serialize
 * @param buffer Pointer to output buffer (must be at least PAYLOAD_SIZE bytes)
 * @return int Number of bytes written, or -1 on error
 */
int serialize_payload(struct Payload *payload, uint8_t *buffer);

/**
 * @brief Deserialize 247-byte buffer into Payload struct after MQTT reception
 * Format: [4 bytes sequence][239 bytes data][4 bytes CRC32]
 * @param buffer Pointer to input buffer containing serialized data
 * @param payload Pointer to Payload structure to populate
 * @return int 0 on success, -1 on error
 */
int deserialize_payload(uint8_t *buffer, struct Payload *payload);

/**
 * @brief Serialize Metadata struct into Payload data field (embedded format)
 * Metadata is sent as a Payload with sequence = 0xFFFFFFFF
 * @param metadata Pointer to Metadata structure to serialize
 * @param buffer Pointer to output buffer (must be at least PAYLOAD_SIZE bytes)
 * @return int Number of bytes written, or -1 on error
 */
int serialize_metadata(struct Metadata *metadata, uint8_t *buffer);

/**
 * @brief Deserialize Payload data field into Metadata struct (embedded format)
 * Extracts metadata from a Payload with sequence = 0xFFFFFFFF
 * @param buffer Pointer to input buffer containing serialized data
 * @param metadata Pointer to Metadata structure to populate
 * @return int 0 on success, -1 on error
 */
int deserialize_metadata(uint8_t *buffer, struct Metadata *metadata);

/**
 * @brief Initialize streaming file read for chunked transmission (memory efficient)
 *
 * This function prepares for streaming chunk reads without loading the entire
 * file into memory. It only reads file metadata and caches cluster information.
 * Use this instead of deconstruct() for better memory efficiency.
 *
 * @param filename Name of file to prepare for streaming
 * @param meta Pointer to Metadata structure to populate
 * @return int 0 on success, -1 on failure
 */
int init_streaming_read(char *filename, struct Metadata *meta);

/**
 * @brief Read a single chunk for streaming transmission (memory efficient)
 *
 * This function reads only the requested chunk from the file without loading
 * the entire file into memory. Much more memory efficient than deconstruct().
 * Call init_streaming_read() first.
 *
 * @param chunk_index Index of chunk to read (0-based)
 * @param chunk Pointer to Payload structure to populate
 * @return int 0 on success, -1 on failure
 */
int read_chunk_streaming(uint32_t chunk_index, struct Payload *chunk);

/**
 * @brief Get the finalized file CRC32 after all chunks have been read
 * @return uint32_t File CRC32 checksum, or 0 if not finalized
 */
uint32_t get_streaming_file_crc(void);

/**
 * @brief Clean up streaming context
 */
void cleanup_streaming_read(void);

#endif // DATA_FRAME_H
