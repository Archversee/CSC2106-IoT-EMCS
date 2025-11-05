/*!
 * @file    chunk_transfer.c
 * @brief   High-level chunk-based file transfer management implementation
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 *
 * CHUNKED WRITE IMPLEMENTATION with FatFS:
 * ========================================
 * This implementation handles out-of-order chunk reception for MQTT file transfers.
 * Uses FatFS library for file I/O operations.
 *
 * Key features:
 * 1. Out-of-order chunk reception with bitmap tracking
 * 2. Duplicate chunk detection and handling
 * 3. Session-based transfer management
 * 4. Metadata chunk (chunk 0) stored separately
 * 5. Data chunks written to temporary file, finalized when complete
 *
 * Implementation details:
 * - Metadata is stored as chunk 0 in a separate .meta file
 * - Data chunks are written to a .tmp file with sparse writes
 * - When all chunks received, .tmp is renamed to final filename
 * - Bitmap tracks received chunks (max 256 chunks with 32-byte bitmap)
 */

#include "chunk_transfer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../drivers/microsd_driver.h"
#include "ff.h"
#include "hw_config.h"

bool chunk_transfer_init_session(const struct Metadata* metadata, transfer_session_t* session,
                                 bool use_new_filename) {
    if (!metadata || !session) {
        printf("ERROR: NULL parameter in chunk_transfer_init_session\n");
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
        const char* original_filename = metadata->filename;
        const char* dot = strrchr(original_filename, '.');

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

    /* Initialize chunk metadata structure */
    session->chunk_meta.total_chunks = session->total_chunks;
    session->chunk_meta.chunk_size = PAYLOAD_DATA_SIZE;
    session->chunk_meta.chunks_received = 0;
    session->chunk_meta.total_file_size = metadata->total_size;

    /* Calculate bitmap size needed (1 bit per chunk, rounded up to bytes) */
    session->chunk_meta.bitmap_size = (session->total_chunks + 7) / 8;

    /* Dynamically allocate bitmap */
    session->chunk_meta.chunk_bitmap = (uint8_t*)malloc(session->chunk_meta.bitmap_size);
    if (!session->chunk_meta.chunk_bitmap) {
        printf("ERROR: Failed to allocate bitmap (%lu bytes for %lu chunks)\n",
               (unsigned long)session->chunk_meta.bitmap_size,
               (unsigned long)session->total_chunks);
        return false;
    }

    /* Initialize bitmap to zeros */
    memset(session->chunk_meta.chunk_bitmap, 0, session->chunk_meta.bitmap_size);

    strncpy(session->chunk_meta.filename, session->filename, 63);
    session->chunk_meta.filename[63] = '\0';

    /* Create temporary filename for chunk writes
       Use shorter format to avoid 8.3 filename limit issues on FAT32 without LFN */
    char tmp_filename[METADATA_FILENAME_SIZE + 5];

    /* Extract base name without extension for shorter tmp name */
    const char* dot = strrchr(session->filename, '.');
    if (dot && dot != session->filename) {
        /* Use format: "basename~.tmp" to stay within 8.3 limits */
        size_t base_len = dot - session->filename;
        if (base_len > 7) base_len = 7;  // Leave room for "~.tmp"
        snprintf(tmp_filename, sizeof(tmp_filename), "%.*s~.tmp", (int)base_len, session->filename);
    } else {
        /* No extension, use simple format */
        snprintf(tmp_filename, sizeof(tmp_filename), "%.8s.tmp", session->filename);
    }

    /* Create and open temporary file (keep it open for entire session) */
    session->tmp_file_open = false;
    FRESULT fr = f_open(&session->tmp_file, tmp_filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (fr != FR_OK) {
        printf("ERROR: Failed to create temporary file %s (error %d)\n", tmp_filename, fr);
        free(session->chunk_meta.chunk_bitmap);
        session->chunk_meta.chunk_bitmap = NULL;
        return false;
    }
    session->tmp_file_open = true;

    /* Pre-allocate file size by seeking to end and writing a byte */
    /* This ensures the file has enough space for all chunks */
    uint32_t total_data_size = metadata->chunk_count * PAYLOAD_DATA_SIZE;
    if (total_data_size > 0) {
        fr = f_lseek(&session->tmp_file, total_data_size - 1);
        if (fr == FR_OK) {
            uint8_t dummy = 0;
            UINT bw;
            f_write(&session->tmp_file, &dummy, 1, &bw);
        }
    }
    /* Don't close - keep file open for chunk writes */

    /* Save metadata to .meta file (separate operation) */
    char meta_filename[METADATA_FILENAME_SIZE + 6];
    snprintf(meta_filename, sizeof(meta_filename), "%s.meta", session->filename);

    FIL meta_file;
    fr = f_open(&meta_file, meta_filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (fr != FR_OK) {
        printf("ERROR: Failed to create metadata file %s (error %d)\n", meta_filename, fr);
        f_close(&session->tmp_file); /* Close temp file */
        session->tmp_file_open = false;
        f_unlink(tmp_filename); /* Clean up temp file */
        free(session->chunk_meta.chunk_bitmap);
        session->chunk_meta.chunk_bitmap = NULL;
        return false;
    }

    UINT bw;
    fr = f_write(&meta_file, metadata, sizeof(struct Metadata), &bw);
    f_close(&meta_file);

    if (fr != FR_OK || bw != sizeof(struct Metadata)) {
        printf("ERROR: Failed to write metadata chunk\n");
        f_close(&session->tmp_file); /* Close temp file */
        session->tmp_file_open = false;
        f_unlink(tmp_filename);
        f_unlink(meta_filename);
        free(session->chunk_meta.chunk_bitmap);
        session->chunk_meta.chunk_bitmap = NULL;
        return false;
    }

    /* Mark metadata chunk (chunk 0) as received */
    session->chunk_meta.chunk_bitmap[0] |= 1;
    session->chunk_meta.chunks_received = 1;

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

bool chunk_transfer_write_payload(transfer_session_t* session, const struct Payload* payload) {
    if (!session || !payload) {
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
    if (byte_idx >= session->chunk_meta.bitmap_size) {
        printf("ERROR: Bitmap index out of bounds (seq=%lu, byte_idx=%lu, bitmap_size=%lu)\n",
               (unsigned long)payload->sequence, (unsigned long)byte_idx,
               (unsigned long)session->chunk_meta.bitmap_size);
        return false;
    }

    if (session->chunk_meta.chunk_bitmap[byte_idx] & (1 << bit_idx)) {
        printf("WARNING: Chunk %lu already received, skipping duplicate\n",
               (unsigned long)payload->sequence);
        return true; /* Not an error, just skip */
    }

    /* Verify temp file is still open */
    if (!session->tmp_file_open) {
        printf("ERROR: Temporary file is not open\n");
        return false;
    }

    /* Calculate offset for this chunk (chunk 1 goes at offset 0) */
    uint32_t offset = (payload->sequence - 1) * PAYLOAD_DATA_SIZE;

    /* Seek to the appropriate position */
    FRESULT fr = f_lseek(&session->tmp_file, offset);
    if (fr != FR_OK) {
        printf("ERROR: Failed to seek to offset %lu (error %d)\n", (unsigned long)offset, fr);
        return false;
    }

    /* Calculate actual chunk size (last chunk may be smaller) */
    uint32_t chunk_size = PAYLOAD_DATA_SIZE;
    if (payload->sequence == session->metadata.chunk_count - 1) {
        /* Last chunk - calculate remaining bytes */
        uint32_t remaining = session->metadata.total_size % PAYLOAD_DATA_SIZE;
        if (remaining > 0) {
            chunk_size = remaining;
        }
    }

    /* Write the chunk data */
    UINT bw;
    fr = f_write(&session->tmp_file, payload->data, chunk_size, &bw);

    if (fr != FR_OK || bw != chunk_size) {
        printf("ERROR: Failed to write chunk %lu (error %d, wrote %u/%lu bytes)\n",
               (unsigned long)payload->sequence, fr, bw, (unsigned long)chunk_size);
        return false;
    }

    /* Mark chunk as received in bitmap */
    session->chunk_meta.chunk_bitmap[byte_idx] |= (1 << bit_idx);
    session->chunk_meta.chunks_received++;

    printf("✓ Chunk %lu/%lu written (%u bytes)\n", (unsigned long)payload->sequence,
           (unsigned long)session->metadata.chunk_count, chunk_size);

    return true;
}

bool chunk_transfer_sync_window(transfer_session_t* session) {
    if (!session) {
        printf("ERROR: NULL parameter in chunk_transfer_sync_window\n");
        return false;
    }

    if (!session->active) {
        printf("ERROR: Session is not active\n");
        return false;
    }

    if (!session->tmp_file_open) {
        printf("ERROR: Temporary file is not open\n");
        return false;
    }

    /* Flush buffered writes to SD card */
    FRESULT fr = f_sync(&session->tmp_file);
    if (fr != FR_OK) {
        printf("ERROR: Failed to sync file (error %d)\n", fr);
        return false;
    }

    printf("✓ Window synced to SD card (%lu/%lu chunks received)\n",
           (unsigned long)session->chunk_meta.chunks_received,
           (unsigned long)session->chunk_meta.total_chunks);

    return true;
}

bool chunk_transfer_is_complete(const transfer_session_t* session) {
    if (!session || !session->active) {
        return false;
    }

    /* Check if all chunks have been received */
    if (session->chunk_meta.chunks_received != session->chunk_meta.total_chunks) {
        return false;
    }

    /* Verify all bits in bitmap are set */
    for (uint32_t i = 0; i < session->chunk_meta.total_chunks; i++) {
        uint32_t byte_idx = i / 8;
        uint32_t bit_idx = i % 8;
        if (!(session->chunk_meta.chunk_bitmap[byte_idx] & (1 << bit_idx))) {
            return false;
        }
    }

    return true;
}

bool chunk_transfer_finalize(transfer_session_t* session) {
    if (!session) {
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

    /* Close the temporary file handle before renaming */
    if (session->tmp_file_open) {
        f_sync(&session->tmp_file); /* Final sync to ensure all data is written */
        f_close(&session->tmp_file);
        session->tmp_file_open = false;
    }

    /* Generate temporary filename (must match the format used in init) */
    char tmp_filename[METADATA_FILENAME_SIZE + 5];
    const char* dot = strrchr(session->filename, '.');
    if (dot && dot != session->filename) {
        /* Use same short format as init: "basename~.tmp" */
        size_t base_len = dot - session->filename;
        if (base_len > 7) base_len = 7;
        snprintf(tmp_filename, sizeof(tmp_filename), "%.*s~.tmp", (int)base_len, session->filename);
    } else {
        snprintf(tmp_filename, sizeof(tmp_filename), "%.8s.tmp", session->filename);
    }

    /* Check if final file exists and delete it */
    FIL file;
    FRESULT fr = f_open(&file, session->filename, FA_READ);
    if (fr == FR_OK) {
        f_close(&file);
        f_unlink(session->filename);
    }

    /* Rename temp file to final filename */
    fr = f_rename(tmp_filename, session->filename);
    if (fr != FR_OK) {
        printf("ERROR: Failed to rename %s to %s (error %d)\n", tmp_filename, session->filename,
               fr);
        return false;
    }

    /* Truncate file to actual size (remove any padding from pre-allocation) */
    fr = f_open(&file, session->filename, FA_WRITE);
    if (fr == FR_OK) {
        f_lseek(&file, session->metadata.total_size);
        f_truncate(&file);
        f_close(&file);
    }

    /* Delete metadata file */
    char meta_filename[METADATA_FILENAME_SIZE + 6];
    snprintf(meta_filename, sizeof(meta_filename), "%s.meta", session->filename);
    f_unlink(meta_filename);

    printf("✓ Transfer session finalized:\n");
    printf("  Session ID: %s\n", session->session_id);
    printf("  File: %s\n", session->filename);
    printf("  Total size: %lu bytes\n", (unsigned long)session->metadata.total_size);

    /* Free the dynamically allocated bitmap */
    if (session->chunk_meta.chunk_bitmap) {
        free(session->chunk_meta.chunk_bitmap);
        session->chunk_meta.chunk_bitmap = NULL;
    }

    session->active = false;
    return true;
}

void chunk_transfer_get_progress(const transfer_session_t* session, uint32_t* chunks_received,
                                 uint32_t* total_chunks) {
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

void chunk_transfer_print_session_info(const transfer_session_t* session) {
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
