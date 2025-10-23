#ifndef DATA_FRAME_H
#define DATA_FRAME_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// MQTT QoS1 payload size (247 bytes for data + 9 bytes overhead = 256 total)
#define PAYLOAD_SIZE 247
#define CRC16_SIZE 2
#define SEQUENCE_SIZE 4
#define SIZE_FIELD 1

// Maximum data size in Payload to fit within 247 bytes when serialized
// struct Payload serialized size: 4 (sequence) + 4 (size) + 2 (crc) + data = 247 bytes
// Therefore: data size = 247 - 10 = 237 bytes
#define PAYLOAD_DATA_SIZE 237

// datagram packet struct for MQTT QoS1 data transfer (fits in 247 bytes when serialized)
struct Payload {
    uint32_t sequence;                // Chunk sequence number (4 bytes)
    uint8_t data[PAYLOAD_DATA_SIZE];  // Actual data payload (237 bytes)
    uint32_t size;                    // Actual data size in this chunk (4 bytes)
    uint16_t crc;                     // CRC16 checksum of data (2 bytes)
} __attribute__((packed));            // Total: 247 bytes

// Metadata for file transfer session (fits in 247 bytes when serialized)
// struct Metadata serialized size: 32 (session_id) + 64 (filename) + 4 (total_size) +
//                                  4 (chunk_count) + 4 (last_modified) + 2 (file_crc) = 110 bytes
#define SESSION_ID_SIZE 32
#define METADATA_FILENAME_SIZE 64

struct Metadata {
    char session_id[SESSION_ID_SIZE];       // Session ID to track transfer type (32 bytes)
    char filename[METADATA_FILENAME_SIZE];  // Filename (64 bytes)
    uint32_t total_size;                    // Total file size in bytes (4 bytes)
    uint32_t chunk_count;                   // Number of data chunks (4 bytes)
    uint32_t last_modified;                 // Unix timestamp (exFAT) (4 bytes)
    uint16_t file_crc;                      // CRC16 of entire file (2 bytes)
} __attribute__((packed));                  // Total: 110 bytes (fits in 247 bytes)

/**
 * @brief Read a file and break it into chunks for MQTT QoS1 transmission
 * @param filename Name of file to read from SD card
 * @param meta Pointer to Metadata structure (output)
 * @param chunks Pointer to array of Payload pointers (output - will be allocated)
 * @return int 0 on success, -1 on failure
 */
int deconstruct(char* filename, struct Metadata* meta, struct Payload** chunks);

/**
 * @brief Reconstruct a file from chunks with integrity verification
 * @param meta Pointer to Metadata structure
 * @param chunks Pointer to array of Payload pointers
 * @param output_filename Name of file to write to SD card
 * @return int 0 on success, -1 on failure
 */
int reconstruct(struct Metadata* meta, struct Payload** chunks, char* output_filename);

/**
 * @brief Verify integrity of a single chunk using CRC16
 * @param chunk Pointer to Payload structure
 * @return int 1 if valid, 0 if invalid
 */
int verify_chunk(struct Payload* chunk);

/**
 * @brief Calculate CRC16 checksum for data buffer (CCITT-FALSE)
 * @param data Pointer to data buffer
 * @param length Length of data in bytes
 * @return uint16_t CRC16 checksum value
 */
uint16_t crc16(unsigned char* data, size_t length);

/**
 * @brief Serialize Payload struct into a 247-byte buffer for MQTT transmission
 * @param payload Pointer to Payload structure to serialize
 * @param buffer Pointer to output buffer (must be at least PAYLOAD_SIZE bytes)
 * @return int Number of bytes written, or -1 on error
 */
int serialize_payload(struct Payload* payload, uint8_t* buffer);

/**
 * @brief Deserialize 247-byte buffer into Payload struct after MQTT reception
 * @param buffer Pointer to input buffer containing serialized data
 * @param payload Pointer to Payload structure to populate
 * @return int 0 on success, -1 on error
 */
int deserialize_payload(uint8_t* buffer, struct Payload* payload);

/**
 * @brief Serialize Metadata struct into a 247-byte buffer for MQTT transmission
 * @param metadata Pointer to Metadata structure to serialize
 * @param buffer Pointer to output buffer (must be at least PAYLOAD_SIZE bytes)
 * @return int Number of bytes written, or -1 on error
 */
int serialize_metadata(struct Metadata* metadata, uint8_t* buffer);

/**
 * @brief Deserialize 247-byte buffer into Metadata struct after MQTT reception
 * @param buffer Pointer to input buffer containing serialized data
 * @param metadata Pointer to Metadata structure to populate
 * @return int 0 on success, -1 on error
 */
int deserialize_metadata(uint8_t* buffer, struct Metadata* metadata);

#endif  // DATA_FRAME_H
