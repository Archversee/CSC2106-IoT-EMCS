/*!
 * @file    data_frame.h
 * @brief   Data frame structures and function prototypes for file transfer
 * @author  INF2004 Team
 * @date    2024
 */

#ifndef DATA_FRAME_H
#define DATA_FRAME_H

#include <stdint.h>
#include <stdio.h>

/*! Packet payload size in bytes */
#define PACKET_SIZE (1024U)

/*! CRC32 checksum size in bytes */
#define CHECKSUM_SIZE (4U)

/*! Maximum filename length */
#define FILENAME_MAX_LEN (64U)

/*! SHA-256 hash length as hex string */
#define HASH_LEN (32U)

/*! Maximum packets per file */
#define MAX_PACKETS_PER_FILE (100U)

/*!
 * @brief Data packet structure for file transfer
 */
typedef struct data_packet_t {
    uint32_t session_id;          /*!< Unique session identifier */
    uint32_t seq;                 /*!< Sequence number */
    uint16_t data_len;            /*!< Length of data in payload */
    uint32_t timestamp;           /*!< Packet timestamp */
    char checksum[CHECKSUM_SIZE]; /*!< MD5 checksum */
    char payload[PACKET_SIZE];    /*!< 1KB payload data */
} data_packet_t;

/*!
 * @brief Acknowledgment packet structure
 */
typedef struct ack_packet_t {
    uint32_t session_id; /*!< Session identifier */
    uint32_t seq;        /*!< Acknowledged sequence number */
} ack_packet_t;

/*!
 * @brief File metadata structure
 */
typedef struct file_metadata_t {
    uint32_t session_id;             /*!< Unique transfer ID */
    char filename[FILENAME_MAX_LEN]; /*!< Filename (e.g., "data.bin") */
    uint64_t file_size;              /*!< File length in bytes */
    uint32_t total_packets;          /*!< Total number of chunks */
    uint16_t packet_size;            /*!< Packet size (typically 1024) */
    char file_hash[HASH_LEN];        /*!< File hash as hex string */
} file_metadata_t;

/*!
 * @brief Send file to receiver
 * @param[in] p_filename    Pointer to filename string
 * @param[in] sock          Socket descriptor (unused in Pico implementation)
 * @param[in] p_receiver    Pointer to receiver data (unused in Pico implementation)
 * @return void
 */
void send_file(char const* const p_filename,
               int const sock,
               void* const p_receiver);

/*!
 * @brief Receive file from sender
 * @param[in] sock      Socket descriptor (unused in Pico implementation)
 * @param[in] p_sender  Pointer to sender data (unused in Pico implementation)
 * @return int          0 on success, negative on error
 */
int receive_file(int const sock,
                 void* const p_sender);

/*!
 * @brief Send acknowledgment packet
 * @param[in] sock          Socket descriptor (unused in Pico implementation)
 * @param[in] p_receiver    Pointer to receiver data (unused in Pico implementation)
 * @param[in] session_id    Session identifier
 * @param[in] seq           Sequence number
 * @return int              0 on success, negative on error
 */
int send_ack(int const sock,
             void* const p_receiver,
             uint32_t const session_id,
             uint32_t const seq);

/*!
 * @brief Receive acknowledgment packet
 * @param[in]  sock         Socket descriptor (unused in Pico implementation)
 * @param[in]  p_sender     Pointer to sender data (unused in Pico implementation)
 * @param[out] p_session_id Pointer to store session identifier
 * @param[out] p_seq        Pointer to store sequence number
 * @return int              0 on success, negative on error
 */
int receive_ack(int const sock,
                void* const p_sender,
                uint32_t* const p_session_id,
                uint32_t* const p_seq);

#endif /* DATA_FRAME_H */
