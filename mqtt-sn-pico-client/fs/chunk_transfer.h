/*!
 * @file    chunk_transfer.h
 * @brief   High-level chunk-based file transfer management for MQTT with RTOS safety
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 *
 * This module provides a higher-level abstraction over the microSD driver's
 * chunk write methods, handling session management and coordinating file
 * transfers with metadata tracking.
 *
 * TRANSFER POLICY:
 * - Metadata is sent via QoS 1 on file/data topic
 * - Receiver sends METADATA_RECV control message to confirm
 * - Data chunks use QoS 1 (at-least-once delivery)
 * - Data chunks are REFUSED if no metadata has been received
 * - Session must be active (metadata received) before data is accepted
 *
 * RTOS SAFETY:
 * - All file operations are protected by mutex
 * - Timeout-based locking prevents deadlocks
 * - Critical sections protect bitmap updates
 * - Automatic cleanup on errors
 * - Thread-safe progress tracking
 */

#ifndef CHUNK_TRANSFER_H
#define CHUNK_TRANSFER_H

#include <stdbool.h>
#include <stdint.h>

#include "data_frame.h"
#include "ff.h" // For FIL type

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Chunk metadata for tracking out-of-order writes with FatFS
 */
typedef struct {
    uint32_t total_chunks;    /*!< Total number of chunks expected */
    uint32_t chunk_size;      /*!< Size of each chunk in bytes */
    uint32_t chunks_received; /*!< Number of chunks received so far */
    uint8_t *chunk_bitmap; /*!< Dynamically allocated bitmap tracking which chunks are received */
    uint32_t bitmap_size;  /*!< Size of bitmap in bytes */
    uint32_t total_file_size; /*!< Total size of the file in bytes */
    char filename[64];        /*!< Filename for this chunked file */
} fs_chunk_metadata_t;

/*!
 * @brief Session information for active chunk transfers
 */
typedef struct {
    char session_id[SESSION_ID_SIZE];      /*!< Unique session identifier */
    char filename[METADATA_FILENAME_SIZE]; /*!< Target filename */
    uint32_t total_chunks;                 /*!< Total number of chunks (including metadata) */
    uint32_t chunk_size;                   /*!< Size of each data chunk */
    bool active;                           /*!< Whether this session is active */
    fs_chunk_metadata_t chunk_meta;        /*!< Chunk metadata for tracking */
    struct Metadata metadata;              /*!< File metadata */
    FIL tmp_file;                          /*!< Persistent file handle for temp file (kept open) */
    bool tmp_file_open;                    /*!< Whether temp file handle is currently open */
} transfer_session_t;

/*!
 * @brief Initialize the file transfer mutex (call once at startup)
 *
 * This function must be called before any other chunk transfer operations.
 * It creates the mutex used to protect file I/O operations.
 *
 * @return true on success, false on failure
 *
 * @note Thread-safe: Can be called multiple times (idempotent)
 */
bool chunk_transfer_init_mutex(void);

/*!
 * @brief Initialize a new chunk transfer session from metadata
 *
 * This function should be called when the metadata chunk (chunk 0) is received
 * via MQTT QoS 1 on the file/data topic. It establishes the session context and
 * prepares for receiving data chunks. Once initialized, the session becomes active
 * and data chunks can be accepted. After initialization, a METADATA_RECV control
 * message is sent to confirm receipt.
 *
 * @param metadata Pointer to the received metadata structure
 * @param session Pointer to session structure to initialize
 * @param use_new_filename If true, adds "_received" suffix to filename; if false, uses original
 * filename
 * @return true on success, false on failure
 *
 * @note Sets session->active to true on success
 * @note Caller should send METADATA_RECV control message after successful init
 * @note Thread-safe: Protected by internal mutex with timeout
 * @note On error, performs automatic cleanup
 */
bool chunk_transfer_init_session(const struct Metadata *metadata, transfer_session_t *session,
                                 bool use_new_filename);

/*!
 * @brief Write a data payload chunk to an active session
 *
 * This function handles writing a data chunk to the SD card for an active
 * transfer session. It verifies the session is active and validates the chunk
 * before writing.
 *
 * @param session Pointer to the active transfer session
 * @param payload Pointer to the payload chunk to write
 * @return true on success, false on failure
 *
 * @note REFUSES data chunks if session is not active (metadata not received)
 * @note Handles duplicate chunks gracefully using bitmap
 * @note Received via QoS 1, so duplicates may occur
 * @note Thread-safe: Protected by internal mutex with timeout
 * @note Bitmap updates use critical sections for atomicity
 */
bool chunk_transfer_write_payload(transfer_session_t *session, const struct Payload *payload);

/*!
 * @brief Sync/flush written chunks to SD card (called after each window)
 *
 * This function ensures all buffered writes are committed to the SD card.
 * Should be called after receiving a complete window of chunks to optimize
 * SD card write performance with the Go-Back-N sliding window protocol.
 *
 * @param session Pointer to the active transfer session
 * @return true on success, false on failure
 *
 * @note For 32KB windows (~138 chunks), call this after each ACK sent
 * @note Reduces SD card wear by batching writes per window instead of per chunk
 * @note Thread-safe: Protected by internal mutex with timeout
 */
bool chunk_transfer_sync_window(transfer_session_t *session);

/*!
 * @brief Check if all chunks have been received for a session
 *
 * @param session Pointer to the transfer session
 * @return true if all chunks received, false otherwise
 *
 * @note Thread-safe: Read-only operation, no locking required
 */
bool chunk_transfer_is_complete(const transfer_session_t *session);

/*!
 * @brief Finalize a chunk transfer session
 *
 * This function completes the file transfer by finalizing the write and cleaning
 * up the session. Should be called after all chunks are received.
 *
 * @param session Pointer to the transfer session to finalize
 * @return true on success, false on failure
 *
 * @note Thread-safe: Protected by internal mutex with timeout
 * @note Automatically cleans up resources on success
 */
bool chunk_transfer_finalize(transfer_session_t *session);

/*!
 * @brief Abort a chunk transfer session and cleanup resources
 *
 * This function forcefully aborts an active transfer session, closes files,
 * deletes temporary files, and frees allocated memory. Use this for error
 * recovery or when canceling a transfer.
 *
 * @param session Pointer to the transfer session to abort
 * @return true on success, false on failure
 *
 * @note Thread-safe: Protected by internal mutex with timeout
 * @note Safe to call even if session is partially initialized
 * @note Forces cleanup even if mutex acquisition fails (emergency recovery)
 */
bool chunk_transfer_abort(transfer_session_t *session);

/*!
 * @brief Get the progress of a transfer session
 *
 * @param session Pointer to the transfer session
 * @param chunks_received Output parameter for number of chunks received
 * @param total_chunks Output parameter for total number of chunks
 *
 * @note Thread-safe: Uses critical section for atomic read
 */
void chunk_transfer_get_progress(const transfer_session_t *session, uint32_t *chunks_received,
                                 uint32_t *total_chunks);

/*!
 * @brief Print session information for debugging
 *
 * @param session Pointer to the transfer session
 *
 * @note Thread-safe: Read-only operation
 */
void chunk_transfer_print_session_info(const transfer_session_t *session);

#ifdef __cplusplus
}
#endif

#endif /* CHUNK_TRANSFER_H */