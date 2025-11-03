/*!
 * @file    chunk_transfer.h
 * @brief   High-level chunk-based file transfer management for MQTT
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 *
 * This module provides a higher-level abstraction over the microSD driver's
 * chunk write methods, handling session management and coordinating file
 * transfers with metadata tracking.
 *
 * TRANSFER POLICY:
 * - Metadata chunks use QoS 2 (exactly-once delivery)
 * - Data chunks use QoS 1 (at-least-once delivery)
 * - Data chunks are REFUSED if no metadata has been received
 * - Session must be active (metadata received) before data is accepted
 */

#ifndef CHUNK_TRANSFER_H
#define CHUNK_TRANSFER_H

#include <stdbool.h>
#include <stdint.h>

#include "data_frame.h"
#include "microsd_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Session information for active chunk transfers
 */
typedef struct {
    char session_id[SESSION_ID_SIZE];      /*!< Unique session identifier */
    char filename[METADATA_FILENAME_SIZE]; /*!< Target filename */
    uint32_t total_chunks;                 /*!< Total number of chunks (including metadata) */
    uint32_t chunk_size;                   /*!< Size of each data chunk */
    bool active;                           /*!< Whether this session is active */
    chunk_metadata_t chunk_meta;           /*!< microSD driver chunk metadata */
    struct Metadata metadata;              /*!< File metadata */
} transfer_session_t;

/*!
 * @brief Initialize a new chunk transfer session from metadata
 *
 * This function should be called when the metadata chunk (chunk 0) is received
 * via MQTT QoS 2. It establishes the session context and prepares the microSD
 * for receiving data chunks. Once initialized, the session becomes active and
 * data chunks can be accepted.
 *
 * @param fs_info Pointer to filesystem information
 * @param metadata Pointer to the received metadata structure
 * @param session Pointer to session structure to initialize
 * @param use_new_filename If true, adds "_received" suffix to filename; if false, uses original filename
 * @return true on success, false on failure
 *
 * @note Sets session->active to true on success
 * @note QoS 2 ensures this is called exactly once per transfer
 */
bool chunk_transfer_init_session(filesystem_info_t* fs_info,
                                 const struct Metadata* metadata,
                                 transfer_session_t* session,
                                 bool use_new_filename);

/*!
 * @brief Write a data payload chunk to an active session
 *
 * This function handles writing a data chunk to the microSD card for an
 * active transfer session. It verifies the session is active and validates
 * the chunk before writing.
 *
 * @param fs_info Pointer to filesystem information
 * @param session Pointer to the active transfer session
 * @param payload Pointer to the payload chunk to write
 * @return true on success, false on failure
 *
 * @note REFUSES data chunks if session is not active (metadata not received)
 * @note Handles duplicate chunks gracefully using bitmap
 * @note Received via QoS 1, so duplicates may occur
 */
bool chunk_transfer_write_payload(filesystem_info_t* fs_info,
                                  transfer_session_t* session,
                                  const struct Payload* payload);

/*!
 * @brief Check if all chunks have been received for a session
 *
 * @param session Pointer to the transfer session
 * @return true if all chunks received, false otherwise
 */
bool chunk_transfer_is_complete(const transfer_session_t* session);

/*!
 * @brief Finalize a chunk transfer session
 *
 * This function completes the file transfer by finalizing the microSD write
 * and cleaning up the session. Should be called after all chunks are received.
 *
 * @param fs_info Pointer to filesystem information
 * @param session Pointer to the transfer session to finalize
 * @return true on success, false on failure
 */
bool chunk_transfer_finalize(filesystem_info_t* fs_info,
                             transfer_session_t* session);

/*!
 * @brief Get the progress of a transfer session
 *
 * @param session Pointer to the transfer session
 * @param chunks_received Output parameter for number of chunks received
 * @param total_chunks Output parameter for total number of chunks
 */
void chunk_transfer_get_progress(const transfer_session_t* session,
                                 uint32_t* chunks_received,
                                 uint32_t* total_chunks);

/*!
 * @brief Print session information for debugging
 *
 * @param session Pointer to the transfer session
 */
void chunk_transfer_print_session_info(const transfer_session_t* session);

#ifdef __cplusplus
}
#endif

#endif /* CHUNK_TRANSFER_H */
