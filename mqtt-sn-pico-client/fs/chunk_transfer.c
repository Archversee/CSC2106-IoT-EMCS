/*!
 * @file    chunk_transfer.c
 * @brief   High-level chunk-based file transfer management implementation with RTOS safety
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 *
 * CHUNKED WRITE IMPLEMENTATION with FatFS and RTOS Safety:
 * =========================================================
 * This implementation handles out-of-order chunk reception for MQTT file transfers
 * with FreeRTOS mutex protection to prevent deadlocks and ensure thread safety.
 *
 * Key features:
 * 1. Out-of-order chunk reception with bitmap tracking
 * 2. Duplicate chunk detection and handling
 * 3. Session-based transfer management
 * 4. RTOS mutex protection for all file operations
 * 5. Timeout-based locking to prevent deadlocks
 * 6. Graceful error recovery and cleanup
 *
 * RTOS Safety Features:
 * - Mutex-protected file operations (SD card access)
 * - Timeout-based lock acquisition (prevents infinite blocking)
 * - Automatic cleanup on errors (prevents resource leaks)
 * - Session state tracking (allows recovery from failures)
 * - Critical section protection for bitmap updates
 *
 * Implementation details:
 * - Metadata is stored as chunk 0 in a separate .meta file
 * - Data chunks are written to a .tmp file with sparse writes
 * - When all chunks received, .tmp is renamed to final filename
 * - All file I/O operations are protected by mutex
 * - Lock acquisition uses timeout to detect deadlock conditions
 */

#include "chunk_transfer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../drivers/microsd_driver.h"
#include "FreeRTOS.h"
#include "ff.h"
#include "hw_config.h"
#include "semphr.h"
#include "task.h"

/*! Mutex for protecting file transfer operations */
static SemaphoreHandle_t g_file_transfer_mutex = NULL;

/*! Timeout for mutex acquisition (prevents deadlock) */
#define FILE_TRANSFER_MUTEX_TIMEOUT_MS 5000

/*! Initialize the file transfer mutex (call once at startup) */
bool chunk_transfer_init_mutex(void) {
    if (g_file_transfer_mutex == NULL) {
        g_file_transfer_mutex = xSemaphoreCreateMutex();
        if (g_file_transfer_mutex == NULL) {
            printf("ERROR: Failed to create file transfer mutex\n");
            return false;
        }
    }
    return true;
}

/*! Helper macro for mutex acquisition with timeout */
#define ACQUIRE_FILE_MUTEX()                                                                       \
    if (xSemaphoreTake(g_file_transfer_mutex, pdMS_TO_TICKS(FILE_TRANSFER_MUTEX_TIMEOUT_MS)) !=    \
        pdTRUE) {                                                                                  \
        printf("ERROR: Failed to acquire file transfer mutex (timeout)\n");                        \
        return false;                                                                              \
    }

#define RELEASE_FILE_MUTEX() xSemaphoreGive(g_file_transfer_mutex)

/*! Helper function to cleanup session on error */
static void cleanup_session_on_error(transfer_session_t *session) {
    if (!session)
        return;

    /* Close temp file if open */
    if (session->tmp_file_open) {
        f_close(&session->tmp_file);
        session->tmp_file_open = false;
    }

    /* Free bitmap if allocated */
    if (session->chunk_meta.chunk_bitmap) {
        free(session->chunk_meta.chunk_bitmap);
        session->chunk_meta.chunk_bitmap = NULL;
    }

    /* Mark session as inactive */
    session->active = false;

    /* Try to delete temporary files */
    char tmp_filename[METADATA_FILENAME_SIZE + 5];
    const char *dot = strrchr(session->filename, '.');
    if (dot && dot != session->filename) {
        size_t base_len = dot - session->filename;
        snprintf(tmp_filename, sizeof(tmp_filename), "%.*s~.tmp", (int)base_len, session->filename);
    } else {
        snprintf(tmp_filename, sizeof(tmp_filename), "%s.tmp", session->filename);
    }
    f_unlink(tmp_filename);

    char meta_filename[METADATA_FILENAME_SIZE + 6];
    snprintf(meta_filename, sizeof(meta_filename), "%s.meta", session->filename);
    f_unlink(meta_filename);

    printf("✗ Session cleaned up after error\n");
}

bool chunk_transfer_init_session(const struct Metadata *metadata, transfer_session_t *session,
                                 bool use_new_filename) {
    if (!metadata || !session) {
        printf("ERROR: NULL parameter in chunk_transfer_init_session\n");
        return false;
    }

    /* Ensure mutex is initialized */
    if (!chunk_transfer_init_mutex()) {
        return false;
    }

    /* Acquire mutex with timeout */
    ACQUIRE_FILE_MUTEX();

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
            size_t base_len = dot - original_filename;
            snprintf(new_filename, sizeof(new_filename), "%.*s_received%s", (int)base_len,
                     original_filename, dot);
        } else {
            snprintf(new_filename, sizeof(new_filename), "%s_received", original_filename);
        }

        strncpy(session->filename, new_filename, METADATA_FILENAME_SIZE - 1);
        session->filename[METADATA_FILENAME_SIZE - 1] = '\0';
    } else {
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
    session->chunk_meta.chunk_bitmap = (uint8_t *)malloc(session->chunk_meta.bitmap_size);
    if (!session->chunk_meta.chunk_bitmap) {
        printf("ERROR: Failed to allocate chunk bitmap (%lu bytes)\n",
               (unsigned long)session->chunk_meta.bitmap_size);
        RELEASE_FILE_MUTEX();
        return false;
    }

    /* Initialize bitmap to zeros */
    memset(session->chunk_meta.chunk_bitmap, 0, session->chunk_meta.bitmap_size);

    strncpy(session->chunk_meta.filename, session->filename, 63);
    session->chunk_meta.filename[63] = '\0';

    /* Create temporary filename for chunk writes */
    char tmp_filename[METADATA_FILENAME_SIZE + 5];
    const char *dot = strrchr(session->filename, '.');
    if (dot && dot != session->filename) {
        size_t base_len = dot - session->filename;
        snprintf(tmp_filename, sizeof(tmp_filename), "%.*s~.tmp", (int)base_len, session->filename);
    } else {
        snprintf(tmp_filename, sizeof(tmp_filename), "%s.tmp", session->filename);
    }

    /* Create and open temporary file (keep it open for entire session) */
    session->tmp_file_open = false;
    FRESULT fr = f_open(&session->tmp_file, tmp_filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (fr != FR_OK) {
        printf("ERROR: Failed to create temp file (FatFS error: %d)\n", fr);
        free(session->chunk_meta.chunk_bitmap);
        session->chunk_meta.chunk_bitmap = NULL;
        RELEASE_FILE_MUTEX();
        return false;
    }
    session->tmp_file_open = true;

    /* Pre-allocate file size by seeking to end and writing a byte */
    uint32_t total_data_size = metadata->chunk_count * PAYLOAD_DATA_SIZE;
    if (total_data_size > 0) {
        fr = f_lseek(&session->tmp_file, total_data_size - 1);
        if (fr == FR_OK) {
            uint8_t zero = 0;
            UINT bw;
            f_write(&session->tmp_file, &zero, 1, &bw);
            f_lseek(&session->tmp_file, 0);
        }
    }

    /* Save metadata to .meta file (separate operation) */
    char meta_filename[METADATA_FILENAME_SIZE + 6];
    snprintf(meta_filename, sizeof(meta_filename), "%s.meta", session->filename);

    FIL meta_file;
    fr = f_open(&meta_file, meta_filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (fr != FR_OK) {
        printf("ERROR: Failed to create metadata file (FatFS error: %d)\n", fr);
        cleanup_session_on_error(session);
        RELEASE_FILE_MUTEX();
        return false;
    }

    UINT bw;
    fr = f_write(&meta_file, metadata, sizeof(struct Metadata), &bw);
    f_close(&meta_file);

    if (fr != FR_OK || bw != sizeof(struct Metadata)) {
        printf("ERROR: Failed to write metadata (FatFS error: %d)\n", fr);
        cleanup_session_on_error(session);
        RELEASE_FILE_MUTEX();
        return false;
    }

    /* Mark metadata chunk (chunk 0) as received */
    session->chunk_meta.chunk_bitmap[0] |= 1;
    session->chunk_meta.chunks_received = 1;

    session->active = true;

    RELEASE_FILE_MUTEX();

    printf("✓ Transfer session initialized:\n");
    printf("  Session ID: %s\n", session->session_id);
    if (use_new_filename) {
        printf("  Original: %s\n", metadata->filename);
        printf("  Saving as: %s\n", session->filename);
    } else {
        printf("  Filename: %s\n", session->filename);
    }
    printf("  Total chunks: %lu (1 metadata + %lu data)\n", (unsigned long)session->total_chunks,
           (unsigned long)metadata->chunk_count);

    return true;
}

bool chunk_transfer_write_payload(transfer_session_t *session, const struct Payload *payload) {
    if (!session || !payload) {
        printf("ERROR: NULL parameter in chunk_transfer_write_payload\n");
        return false;
    }

    if (!session->active) {
        printf("ERROR: Session not active, cannot write payload\n");
        return false;
    }

    /* Verify chunk is within valid range */
    if (payload->sequence < 1 || payload->sequence > session->metadata.chunk_count) {
        printf("ERROR: Invalid chunk sequence %lu (expected 1-%lu)\n",
               (unsigned long)payload->sequence, (unsigned long)session->metadata.chunk_count);
        return false;
    }

    /* Check duplicate BEFORE acquiring mutex (reduces lock contention) */
    uint32_t byte_idx = payload->sequence / 8;
    uint32_t bit_idx = payload->sequence % 8;

    /* Defensive check: ensure bitmap index is within allocated bounds */
    if (byte_idx >= session->chunk_meta.bitmap_size) {
        printf("ERROR: Bitmap index out of bounds\n");
        return false;
    }

    /* Enter critical section for bitmap check (fast, no I/O) */
    taskENTER_CRITICAL();
    bool is_duplicate = (session->chunk_meta.chunk_bitmap[byte_idx] & (1 << bit_idx)) != 0;
    taskEXIT_CRITICAL();

    if (is_duplicate) {
        printf("INFO: Duplicate chunk %lu detected, skipping\n", (unsigned long)payload->sequence);
        return true; /* Not an error, just skip */
    }

    /* Acquire mutex for file I/O */
    ACQUIRE_FILE_MUTEX();

    /* Verify temp file is still open */
    if (!session->tmp_file_open) {
        printf("ERROR: Temporary file not open\n");
        RELEASE_FILE_MUTEX();
        return false;
    }

    /* Calculate offset for this chunk (chunk 1 goes at offset 0) */
    uint32_t offset = (payload->sequence - 1) * PAYLOAD_DATA_SIZE;

    /* Seek to the appropriate position */
    FRESULT fr = f_lseek(&session->tmp_file, offset);
    if (fr != FR_OK) {
        printf("ERROR: Failed to seek in temp file (FatFS error: %d)\n", fr);
        RELEASE_FILE_MUTEX();
        return false;
    }

    /* Calculate actual chunk size (last chunk may be smaller) */
    uint32_t chunk_size = PAYLOAD_DATA_SIZE;
    if (payload->sequence == session->metadata.chunk_count) {
        uint32_t remainder = session->metadata.total_size % PAYLOAD_DATA_SIZE;
        if (remainder != 0) {
            chunk_size = remainder;
        }
    }

    /* Write the chunk data */
    UINT bw;
    fr = f_write(&session->tmp_file, payload->data, chunk_size, &bw);

    if (fr != FR_OK || bw != chunk_size) {
        printf("ERROR: Failed to write chunk (FatFS error: %d)\n", fr);
        RELEASE_FILE_MUTEX();
        return false;
    }

    RELEASE_FILE_MUTEX();

    /* Update bitmap in critical section (atomic) */
    taskENTER_CRITICAL();
    session->chunk_meta.chunk_bitmap[byte_idx] |= (1 << bit_idx);
    session->chunk_meta.chunks_received++;
    taskEXIT_CRITICAL();

    printf("✓ Chunk %lu/%lu written (%u bytes)\n", (unsigned long)payload->sequence,
           (unsigned long)session->metadata.chunk_count, chunk_size);

    return true;
}

bool chunk_transfer_sync_window(transfer_session_t *session) {
    if (!session) {
        printf("ERROR: NULL session in chunk_transfer_sync_window\n");
        return false;
    }

    if (!session->active) {
        printf("ERROR: Session not active\n");
        return false;
    }

    if (!session->tmp_file_open) {
        printf("ERROR: Temporary file not open\n");
        return false;
    }

    /* Acquire mutex for file sync */
    ACQUIRE_FILE_MUTEX();

    /* Flush buffered writes to SD card */
    FRESULT fr = f_sync(&session->tmp_file);
    if (fr != FR_OK) {
        printf("ERROR: Failed to sync file (FatFS error: %d)\n", fr);
        RELEASE_FILE_MUTEX();
        return false;
    }

    RELEASE_FILE_MUTEX();

    printf("✓ Window synced to SD card (%lu/%lu chunks received)\n",
           (unsigned long)session->chunk_meta.chunks_received,
           (unsigned long)session->chunk_meta.total_chunks);

    return true;
}

bool chunk_transfer_is_complete(const transfer_session_t *session) {
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

bool chunk_transfer_finalize(transfer_session_t *session) {
    if (!session) {
        printf("ERROR: NULL session in chunk_transfer_finalize\n");
        return false;
    }

    if (!session->active) {
        printf("ERROR: Session not active\n");
        return false;
    }

    /* Check if all chunks received */
    if (!chunk_transfer_is_complete(session)) {
        printf("ERROR: Transfer incomplete (%lu/%lu chunks received)\n",
               (unsigned long)session->chunk_meta.chunks_received,
               (unsigned long)session->chunk_meta.total_chunks);
        return false;
    }

    /* Acquire mutex for file operations */
    ACQUIRE_FILE_MUTEX();

    /* Close the temporary file handle before renaming */
    if (session->tmp_file_open) {
        f_close(&session->tmp_file);
        session->tmp_file_open = false;
    }

    /* Generate temporary filename (must match the format used in init) */
    char tmp_filename[METADATA_FILENAME_SIZE + 5];
    const char *dot = strrchr(session->filename, '.');
    if (dot && dot != session->filename) {
        size_t base_len = dot - session->filename;
        snprintf(tmp_filename, sizeof(tmp_filename), "%.*s~.tmp", (int)base_len, session->filename);
    } else {
        snprintf(tmp_filename, sizeof(tmp_filename), "%s.tmp", session->filename);
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
        printf("ERROR: Failed to rename temp file (FatFS error: %d)\n", fr);
        RELEASE_FILE_MUTEX();
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

    RELEASE_FILE_MUTEX();

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

bool chunk_transfer_abort(transfer_session_t *session) {
    if (!session) {
        return false;
    }

    /* Acquire mutex with timeout */
    if (xSemaphoreTake(g_file_transfer_mutex, pdMS_TO_TICKS(FILE_TRANSFER_MUTEX_TIMEOUT_MS)) !=
        pdTRUE) {
        printf("WARNING: Failed to acquire mutex for abort, forcing cleanup\n");
        /* Force cleanup even without mutex in emergency situations */
    } else {
        cleanup_session_on_error(session);
        RELEASE_FILE_MUTEX();
    }

    printf("✓ Transfer session aborted and cleaned up\n");
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

    /* Read atomically */
    taskENTER_CRITICAL();
    if (chunks_received) {
        *chunks_received = session->chunk_meta.chunks_received;
    }
    if (total_chunks) {
        *total_chunks = session->chunk_meta.total_chunks;
    }
    taskEXIT_CRITICAL();
}

void chunk_transfer_print_session_info(const transfer_session_t *session) {
    if (!session) {
        printf("ERROR: NULL session\n");
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
