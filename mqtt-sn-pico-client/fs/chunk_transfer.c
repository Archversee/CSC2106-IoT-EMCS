/*!
 * @file    chunk_transfer.c
 * @brief   High-level chunk-based file transfer management implementation
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 */

#include "chunk_transfer.h"

#include <stdio.h>
#include <string.h>

bool chunk_transfer_init_session(filesystem_info_t *fs_info, const struct Metadata *metadata,
                                 transfer_session_t *session, bool use_new_filename) {
    if (!fs_info || !metadata || !session) {
        printf("ERROR: NULL parameter in chunk_transfer_init_session\n");
        return false;
    }

    /* Verify filesystem is initialized by checking critical fields */
    if (fs_info->bytes_per_sector == 0 || fs_info->cluster_count == 0) {
        printf("ERROR: Filesystem not properly initialized\n");
        return false;
    }

    /* Copy metadata to session */
    memcpy(&session->metadata, metadata, sizeof(struct Metadata));
    strncpy(session->session_id, metadata->session_id, SESSION_ID_SIZE - 1);
    session->session_id[SESSION_ID_SIZE - 1] = '\0';

    /* Determine filename based on flag */
    if (use_new_filename) {
        /* Modify filename to add "_received" suffix before extension */
        char new_filename[METADATA_FILENAME_SIZE];
        const char *original_filename = metadata->filename;
        const char *dot = strrchr(original_filename, '.');

        if (dot && dot != original_filename) {
            /* File has extension, insert "_received" before extension */
            size_t base_len = dot - original_filename;

            /* Ensure we don't overflow the buffer */
            if (base_len + strlen("_received") + strlen(dot) >= METADATA_FILENAME_SIZE) {
                /* Filename too long, truncate base name */
                base_len = METADATA_FILENAME_SIZE - strlen("_received") - strlen(dot) - 1;
            }

            snprintf(new_filename, METADATA_FILENAME_SIZE, "%.*s_received%s", (int)base_len,
                     original_filename, dot);
        } else {
            /* No extension, just append "_received" */
            snprintf(new_filename, METADATA_FILENAME_SIZE, "%s_received", original_filename);
        }

        strncpy(session->filename, new_filename, METADATA_FILENAME_SIZE - 1);
        session->filename[METADATA_FILENAME_SIZE - 1] = '\0';
    } else {
        /* Use original filename from metadata */
        strncpy(session->filename, metadata->filename, METADATA_FILENAME_SIZE - 1);
        session->filename[METADATA_FILENAME_SIZE - 1] = '\0';
    }

    /* Calculate total chunks: metadata (chunk 0) + data chunks */
    session->total_chunks = metadata->chunk_count + 1;
    session->chunk_size = PAYLOAD_DATA_SIZE;
    session->active = false;

    /* Initialize microSD chunk write with actual file size from metadata */
    if (!microsd_init_chunk_write(fs_info, session->filename, session->total_chunks,
                                  session->chunk_size, metadata->total_size,
                                  &session->chunk_meta)) {
        printf("ERROR: Failed to initialize microSD chunk write\n");
        return false;
    }

    /* Write metadata chunk (chunk 0) to microSD */
    if (!microsd_write_chunk(fs_info, &session->chunk_meta, 0, (uint8_t *)metadata,
                             sizeof(struct Metadata))) {
        printf("ERROR: Failed to write metadata chunk\n");
        return false;
    }

    session->active = true;
    printf("✓ Transfer session initialized:\n");
    printf("  Session ID: %s\n", session->session_id);
    if (use_new_filename) {
        printf("  Original filename: %s\n", metadata->filename);
        printf("  Saving as: %s\n", session->filename);
    } else {
        printf("  Filename: %s\n", session->filename);
    }
    printf("  Total chunks: %lu (1 metadata + %lu data)\n", (unsigned long)session->total_chunks,
           (unsigned long)metadata->chunk_count);

    return true;
}

bool chunk_transfer_write_payload(filesystem_info_t *fs_info, transfer_session_t *session,
                                  const struct Payload *payload) {
    if (!fs_info || !session || !payload) {
        printf("ERROR: NULL parameter in chunk_transfer_write_payload\n");
        return false;
    }

    if (!session->active) {
        printf("ERROR: Session is not active - metadata chunk must be received first\n");
        printf("       Refusing data payload chunk %lu (no active transfer session)\n",
               (unsigned long)payload->sequence);
        return false;
    }

    /* Verify chunk is within valid range */
    if (payload->sequence < 1 || payload->sequence > session->metadata.chunk_count) {
        printf("ERROR: Invalid sequence number %lu (valid: 1-%lu)\n",
               (unsigned long)payload->sequence, (unsigned long)session->metadata.chunk_count);
        return false;
    }

    /* Verify chunk hasn't already been written */
    uint32_t byte_idx = payload->sequence / 8;
    uint32_t bit_idx = payload->sequence % 8;

    /* Defensive check: ensure bitmap index is within allocated bounds */
    uint32_t bitmap_size_bytes = (session->chunk_meta.total_chunks + 7) / 8;
    if (byte_idx >= bitmap_size_bytes) {
        printf("ERROR: Bitmap index out of bounds (byte_idx=%lu, bitmap_size=%lu)\n",
               (unsigned long)byte_idx, (unsigned long)bitmap_size_bytes);
        return false;
    }
    if (session->chunk_meta.chunk_bitmap[byte_idx] & (1 << bit_idx)) {
        printf("WARNING: Chunk %lu already received, skipping duplicate\n",
               (unsigned long)payload->sequence);
        return true; /* Not an error, just skip */
    }

    /* Write chunk to microSD */
    if (!microsd_write_chunk(fs_info, &session->chunk_meta, payload->sequence, payload->data,
                             payload->size)) {
        printf("ERROR: Failed to write chunk %lu to microSD\n", (unsigned long)payload->sequence);
        return false;
    }

    return true;
}

bool chunk_transfer_is_complete(const transfer_session_t *session) {
    if (!session || !session->active) {
        return false;
    }

    return microsd_check_all_chunks_received(&session->chunk_meta);
}

bool chunk_transfer_finalize(filesystem_info_t *fs_info, transfer_session_t *session) {
    if (!fs_info || !session) {
        printf("ERROR: NULL parameter in chunk_transfer_finalize\n");
        return false;
    }

    if (!session->active) {
        printf("ERROR: Session is not active\n");
        return false;
    }

    /* Check if all chunks received */
    if (!chunk_transfer_is_complete(session)) {
        printf("ERROR: Cannot finalize - not all chunks received\n");
        printf("  Received: %lu/%lu chunks\n", (unsigned long)session->chunk_meta.chunks_received,
               (unsigned long)session->chunk_meta.total_chunks);
        return false;
    }

    /* Finalize microSD write */
    if (!microsd_finalize_chunk_write(fs_info, &session->chunk_meta)) {
        printf("ERROR: Failed to finalize microSD chunk write\n");
        return false;
    }

    printf("✓ Transfer session finalized:\n");
    printf("  Session ID: %s\n", session->session_id);
    printf("  File: %s\n", session->filename);
    printf("  Total size: %lu bytes\n", (unsigned long)session->metadata.total_size);

    session->active = false;
    return true;
}

void chunk_transfer_get_progress(const transfer_session_t *session, uint32_t *chunks_received,
                                 uint32_t *total_chunks) {
    if (!session) {
        if (chunks_received)
            *chunks_received = 0;
        if (total_chunks)
            *total_chunks = 0;
        return;
    }

    if (chunks_received) {
        *chunks_received = session->chunk_meta.chunks_received;
    }
    if (total_chunks) {
        *total_chunks = session->chunk_meta.total_chunks;
    }
}

void chunk_transfer_print_session_info(const transfer_session_t *session) {
    if (!session) {
        printf("Session: NULL\n");
        return;
    }

    printf("=== Transfer Session Info ===\n");
    printf("  Session ID: %s\n", session->session_id);
    printf("  Filename: %s\n", session->filename);
    printf("  Status: %s\n", session->active ? "ACTIVE" : "INACTIVE");
    printf("  Progress: %lu/%lu chunks\n", (unsigned long)session->chunk_meta.chunks_received,
           (unsigned long)session->chunk_meta.total_chunks);
    printf("  Chunk size: %lu bytes\n", (unsigned long)session->chunk_size);
    printf("  Total file size: %lu bytes\n", (unsigned long)session->metadata.total_size);

    /* Show which chunks have been received */
    printf("  Chunks received: ");
    for (uint32_t i = 0; i < session->total_chunks; i++) {
        uint32_t byte_idx = i / 8;
        uint32_t bit_idx = i % 8;
        if (session->chunk_meta.chunk_bitmap[byte_idx] & (1 << bit_idx)) {
            printf("%lu ", (unsigned long)i);
        }
    }
    printf("\n");

    /* Show missing chunks if any */
    bool has_missing = false;
    for (uint32_t i = 0; i < session->total_chunks; i++) {
        uint32_t byte_idx = i / 8;
        uint32_t bit_idx = i % 8;
        if (!(session->chunk_meta.chunk_bitmap[byte_idx] & (1 << bit_idx))) {
            if (!has_missing) {
                printf("  Missing chunks: ");
                has_missing = true;
            }
            printf("%lu ", (unsigned long)i);
        }
    }
    if (has_missing) {
        printf("\n");
    }
    printf("============================\n");
}
