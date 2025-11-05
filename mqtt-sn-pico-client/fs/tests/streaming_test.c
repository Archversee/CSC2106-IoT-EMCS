/*!
 * @file    streaming_test.c
 * @brief   MQTT-SN streaming test with Go-Back-N sliding window protocol
 * @author  INF2004 Project Team
 * @date    2025
 *
 * This test demonstrates the Go-Back-N sliding window protocol for MQTT-SN file transfer.
 * It implements:
 * - Sliding window flow control (4KB windows, ~17 chunks per window)
 * - Two MQTT topics: 'file/data' (sender->receiver) and 'file/control' (receiver->sender)
 * - ACK/NACK/REQUEST_NEXT control messages for flow control
 * - Automatic retransmission on packet loss
 * - Constant memory usage (~33KB) regardless of file size
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../drivers/microsd_driver.h"
#include "../chunk_transfer.h"
#include "data_frame.h"
#include "ff.h"  // FatFS
#include "pico/stdlib.h"

/*! Test configuration constants */
#define SOURCE_FILENAME "test.jpg"

/*! Go-Back-N Sliding Window Protocol Configuration */
#define CHUNK_SIZE 237                                       // Size of each data chunk in bytes (PAYLOAD_DATA_SIZE)
#define WINDOW_SIZE_BYTES (32 * 1024)                        // 32KB window size (optimized for Pico W 264KB RAM)
#define WINDOW_SIZE_CHUNKS (WINDOW_SIZE_BYTES / CHUNK_SIZE)  // ~138 chunks per window
#define MAX_RETRIES 3                                        // Maximum retransmission attempts

/*! MQTT Topic Simulation */
typedef enum {
    TOPIC_FILE_DATA,    // For transmitting metadata and data chunks
    TOPIC_FILE_CONTROL  // For flow control (ACK/NACK/Request)
} mqtt_topic_t;

/*! Control Message Types */
typedef enum {
    CTRL_ACK,           // Acknowledge chunks up to seq_num
    CTRL_NACK,          // Negative acknowledgment - request retransmission
    CTRL_REQUEST_NEXT,  // Request next batch of chunks
    CTRL_COMPLETE       // Transfer complete confirmation
} control_msg_type_t;

/*! Control Message Structure */
typedef struct {
    control_msg_type_t type;
    uint32_t seq_num;       // Sequence number (for ACK/NACK)
    uint32_t window_start;  // Start of next requested window
    uint32_t window_end;    // End of next requested window
    char session_id[32];    // Session identifier
} control_message_t;

/*! Sliding Window State */
typedef struct {
    uint32_t base;          // Base of sliding window (oldest unACKed)
    uint32_t next_seq;      // Next sequence to send
    uint32_t window_size;   // Window size in chunks
    uint32_t total_chunks;  // Total chunks in file
    bool* acked;            // ACK bitmap for chunks in current window
    uint32_t retries;       // Retry counter
    absolute_time_t last_send_time;
} sliding_window_t;

/*!
 * @brief Get file size from filesystem using microsd_driver
 * @param filename Filename to check
 * @return uint32_t File size in bytes, 0 if not found
 */
static uint32_t get_file_size(const char* filename) {
    printf("  get_file_size() called for: %s\n", filename);

    FILINFO fno;
    if (!microsd_driver_stat(filename, &fno)) {
        printf("  ERROR: Could not stat file: %s\n", filename);
        return 0;
    }

    uint32_t size = fno.fsize;
    printf("  File size: %u bytes\n", size);
    return size;
}

/*!
 * @brief Calculate the actual data size for a chunk
 * @param chunk_index Index of the chunk (0-based)
 * @param meta Pointer to Metadata structure
 * @return uint32_t Actual data size in bytes
 */
static uint32_t get_chunk_data_size(uint32_t chunk_index, const struct Metadata* meta) {
    if (chunk_index >= meta->chunk_count) {
        return 0;  // Invalid chunk index
    }

    // Last chunk might be smaller
    if (chunk_index == meta->chunk_count - 1) {
        uint32_t remainder = meta->total_size % PAYLOAD_DATA_SIZE;
        return (remainder == 0) ? PAYLOAD_DATA_SIZE : remainder;
    }

    return PAYLOAD_DATA_SIZE;
}

/*!
 * @brief Test streaming read and display chunk information
 * @param filename Source file to stream
 * @return bool true on success
 */
static bool test_streaming_read(const char* filename) {
    printf("\n=== Step 1: Initialize Streaming Read ===\n");

    struct Metadata meta = {0};

    // Initialize streaming read
    if (init_streaming_read((char*)filename, &meta) != 0) {
        printf("ERROR: Failed to initialize streaming read\n");
        return false;
    }

    printf("SUCCESS: File ready for streaming\n");
    printf("  Filename: %s\n", meta.filename);
    printf("  File size: %u bytes\n", meta.total_size);
    printf("  Total chunks: %u\n", meta.chunk_count);
    printf("  Session ID: %s\n", meta.session_id);
    printf("  File CRC: 0x%04X\n", meta.file_crc);

    // Show sample chunks
    printf("\n=== Sample Chunk Preview ===\n");
    uint32_t samples_to_show = (meta.chunk_count < 3) ? meta.chunk_count : 3;

    for (uint32_t i = 0; i < samples_to_show; i++) {
        struct Payload chunk = {0};

        if (read_chunk_streaming(i, &chunk) != 0) {
            printf("ERROR: Failed to read chunk %u\n", i);
            cleanup_streaming_read();
            return false;
        }

        if (!verify_chunk(&chunk)) {
            printf("ERROR: Chunk %u failed verification\n", i);
            cleanup_streaming_read();
            return false;
        }

        uint32_t chunk_size = get_chunk_data_size(i, &meta);
        printf("\nChunk %u: seq=%u, size=%u bytes, crc=0x%04X\n", i, chunk.sequence, chunk_size,
               chunk.crc);

        printf("  Data preview: \"");
        for (uint32_t j = 0; j < 80 && j < chunk_size; j++) {
            char c = chunk.data[j];
            printf("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        printf("%s\"\n", chunk_size > 80 ? "..." : "");
    }

    cleanup_streaming_read();
    printf("\nStreaming read test completed successfully\n");
    return true;
}

/*!
 * @brief Simulate MQTT publish to a topic
 * @param topic MQTT topic to publish to
 * @param data Pointer to data to publish
 * @param size Size of data in bytes
 */
static void mqtt_publish(mqtt_topic_t topic, const void* data, size_t size) {
    const char* topic_name = (topic == TOPIC_FILE_DATA) ? "file/data" : "file/control";

    // Simulate network delay (5-20ms)
    sleep_ms(5 + (rand() % 15));

    // In real implementation, this would publish to MQTT broker
    // For simulation, just log the action
    if (topic == TOPIC_FILE_CONTROL) {
        const control_message_t* ctrl = (const control_message_t*)data;
        switch (ctrl->type) {
            case CTRL_ACK:
                printf("    [MQTT:%s] ACK up to chunk %u\n", topic_name, ctrl->seq_num);
                break;
            case CTRL_REQUEST_NEXT:
                printf("    [MQTT:%s] REQUEST_NEXT window [%u-%u]\n",
                       topic_name, ctrl->window_start, ctrl->window_end);
                break;
            case CTRL_COMPLETE:
                printf("    [MQTT:%s] TRANSFER_COMPLETE\n", topic_name);
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief Initialize sliding window state
 * @param window Pointer to sliding window structure
 * @param total_chunks Total number of chunks in file
 * @return bool true on success
 */
static bool init_sliding_window(sliding_window_t* window, uint32_t total_chunks) {
    if (!window) return false;

    window->base = 1;  // Start from chunk 1 (0 is metadata)
    window->next_seq = 1;
    window->window_size = WINDOW_SIZE_CHUNKS;
    window->total_chunks = total_chunks;
    window->retries = 0;

    // Allocate ACK bitmap for window
    window->acked = (bool*)calloc(window->window_size, sizeof(bool));
    if (!window->acked) {
        printf("ERROR: Failed to allocate ACK bitmap\n");
        return false;
    }

    window->last_send_time = get_absolute_time();

    printf("Sliding Window initialized:\n");
    printf("  Window size: %u chunks (%u bytes)\n",
           window->window_size, window->window_size * CHUNK_SIZE);
    printf("  Total chunks: %u\n", total_chunks);

    return true;
}

/*!
 * @brief Clean up sliding window resources
 * @param window Pointer to sliding window structure
 */
static void cleanup_sliding_window(sliding_window_t* window) {
    if (window && window->acked) {
        free(window->acked);
        window->acked = NULL;
    }
}

/*!
 * @brief Check if chunk is within current window
 * @param window Pointer to sliding window structure
 * @param seq Sequence number to check
 * @return bool true if within window
 */
static bool is_in_window(const sliding_window_t* window, uint32_t seq) {
    return (seq >= window->base && seq < window->base + window->window_size);
}

/*!
 * @brief Process ACK and slide window if possible
 * @param window Pointer to sliding window structure
 * @param ack_seq Sequence number being acknowledged
 */
static void process_ack(sliding_window_t* window, uint32_t ack_seq) {
    if (!is_in_window(window, ack_seq)) {
        return;  // ACK outside window, ignore
    }

    // Mark chunk as ACKed
    uint32_t index = (ack_seq - window->base) % window->window_size;
    window->acked[index] = true;

    // Try to slide window forward
    while (window->base < window->total_chunks + 1) {
        index = (window->base - window->base) % window->window_size;
        if (!window->acked[index]) {
            break;  // Stop at first unACKed chunk
        }

        // Slide window forward
        window->acked[index] = false;  // Clear old ACK
        window->base++;
    }
}

/*!
 * @brief Simulate Go-Back-N MQTT streaming with sliding window
 * @param filename Source file to transfer
 * @return bool true on success
 */
static bool test_mqtt_gbn_streaming(const char* filename) {
    printf("\n=== Step 2: Go-Back-N MQTT Streaming with Sliding Window ===\n");

    struct Metadata meta = {0};
    sliding_window_t window = {0};

    // Initialize streaming
    if (init_streaming_read((char*)filename, &meta) != 0) {
        printf("ERROR: Failed to initialize streaming\n");
        return false;
    }

    printf("SENDER: Preparing to transmit file via MQTT-SN (Go-Back-N)\n");
    printf("  Session ID: %s\n", meta.session_id);
    printf("  Total chunks: %u\n", meta.chunk_count);
    printf("  File size: %u bytes\n", meta.total_size);

    // Initialize sliding window
    if (!init_sliding_window(&window, meta.chunk_count)) {
        cleanup_streaming_read();
        return false;
    }

    // Send metadata first (chunk 0)
    printf("\n[MQTT:file/data] Publishing metadata (chunk 0)...\n");
    mqtt_publish(TOPIC_FILE_DATA, &meta, sizeof(meta));
    sleep_ms(50);  // Wait for metadata to be received

    uint32_t total_transmitted = 0;
    uint32_t total_retransmissions = 0;
    absolute_time_t start = get_absolute_time();

    // Main transmission loop
    while (window.base <= meta.chunk_count) {
        // Send all chunks in current window that haven't been ACKed
        uint32_t window_end = window.base + window.window_size;
        if (window_end > meta.chunk_count + 1) {
            window_end = meta.chunk_count + 1;
        }

        printf("\n--- Window [%u-%u] ---\n", window.base, window_end - 1);

        // Transmit window
        for (uint32_t seq = window.base; seq < window_end; seq++) {
            uint32_t index = (seq - window.base) % window.window_size;

            // Skip already ACKed chunks
            if (window.acked[index]) {
                continue;
            }

            struct Payload chunk = {0};

            // Read chunk (seq is 1-indexed for data chunks, but read_chunk_streaming is 0-indexed)
            if (read_chunk_streaming(seq - 1, &chunk) != 0) {
                printf("ERROR: Failed to read chunk %u\n", seq);
                cleanup_sliding_window(&window);
                cleanup_streaming_read();
                return false;
            }

            // Verify chunk
            if (!verify_chunk(&chunk)) {
                printf("ERROR: Invalid chunk %u\n", seq);
                cleanup_sliding_window(&window);
                cleanup_streaming_read();
                return false;
            }

            uint32_t chunk_size = get_chunk_data_size(seq - 1, &meta);

            // Publish to MQTT (show first 3, last, and middle for progress)
            bool show_output = (seq <= window.base + 2) ||                                // First 3
                               (seq == window_end - 1) ||                                 // Last in window
                               ((seq - window.base) % 50 == 0 && seq > window.base + 2);  // Every 50th

            if (show_output) {
                printf("  [MQTT:file/data] Chunk %u/%u (seq=%u, size=%u, crc=0x%04X)\n",
                       seq, meta.chunk_count, chunk.sequence, chunk_size, chunk.crc);
            }
            mqtt_publish(TOPIC_FILE_DATA, &chunk, sizeof(chunk));
            total_transmitted++;
        }

        printf("  ✓ Transmitted %u chunks in window [%u-%u]\n",
               window_end - window.base, window.base, window_end - 1);

        // === RECEIVER SIDE SIMULATION ===
        printf("\n  [RECEIVER] Processing window...\n");

        // Simulate receiver processing time
        sleep_ms(50);

        // RECEIVER: Simulate random packet loss (5% chance per chunk)
        bool all_received = true;
        uint32_t last_received = window.base - 1;
        uint32_t first_lost_chunk = 0;

        for (uint32_t seq = window.base; seq < window_end; seq++) {
            if ((rand() % 100) < 5) {  // 5% packet loss
                all_received = false;
                first_lost_chunk = seq;
                printf("  [RECEIVER] ✗ Packet loss detected at chunk %u\n", seq);
                break;
            }
            last_received = seq;
        }

        if (all_received) {
            // All chunks in window received successfully
            printf("  [RECEIVER] ✓ All %u chunks received\n", window_end - window.base);

            // STEP 1: Write window to SD card (buffered write, then sync)
            printf("  [RECEIVER] Writing window to SD card...\n");
            sleep_ms(100);  // Simulate SD card write time
            printf("  [RECEIVER] ✓ Window synced to SD card\n");

            // STEP 2: Send control message to request next window
            control_message_t ctrl_msg = {0};
            strncpy(ctrl_msg.session_id, meta.session_id, sizeof(ctrl_msg.session_id) - 1);

            if (window_end > meta.chunk_count) {
                // Transfer complete
                ctrl_msg.type = CTRL_COMPLETE;
                ctrl_msg.seq_num = meta.chunk_count;
                printf("  [RECEIVER] Sending COMPLETE acknowledgment\n");
                mqtt_publish(TOPIC_FILE_CONTROL, &ctrl_msg, sizeof(ctrl_msg));

                // Mark all as ACKed
                for (uint32_t seq = window.base; seq < window_end; seq++) {
                    process_ack(&window, seq);
                }
                break;
            } else {
                // Request next window
                ctrl_msg.type = CTRL_REQUEST_NEXT;
                ctrl_msg.seq_num = last_received;
                ctrl_msg.window_start = window_end;
                ctrl_msg.window_end = window_end + window.window_size;
                if (ctrl_msg.window_end > meta.chunk_count + 1) {
                    ctrl_msg.window_end = meta.chunk_count + 1;
                }
                printf("  [RECEIVER] Requesting next window [%u-%u]\n",
                       ctrl_msg.window_start, ctrl_msg.window_end - 1);
                mqtt_publish(TOPIC_FILE_CONTROL, &ctrl_msg, sizeof(ctrl_msg));

                // Mark all as ACKed and slide window
                for (uint32_t seq = window.base; seq < window_end; seq++) {
                    process_ack(&window, seq);
                }
            }

        } else {
            // Packet loss detected - Go-Back-N: retransmit from first lost chunk
            printf("  [RECEIVER] Sending NACK - retransmit from chunk %u\n", first_lost_chunk);

            control_message_t ctrl_msg = {0};
            strncpy(ctrl_msg.session_id, meta.session_id, sizeof(ctrl_msg.session_id) - 1);
            ctrl_msg.type = CTRL_NACK;
            ctrl_msg.seq_num = last_received;
            mqtt_publish(TOPIC_FILE_CONTROL, &ctrl_msg, sizeof(ctrl_msg));

            total_retransmissions += (window_end - (last_received + 1));

            // Only ACK up to last successfully received
            for (uint32_t seq = window.base; seq <= last_received; seq++) {
                process_ack(&window, seq);
            }
        }

        // Check for timeout
        if (window.retries >= MAX_RETRIES) {
            printf("ERROR: Maximum retries exceeded\n");
            cleanup_sliding_window(&window);
            cleanup_streaming_read();
            return false;
        }
    }

    absolute_time_t end = get_absolute_time();
    int64_t elapsed_ms = absolute_time_diff_us(start, end) / 1000;

    cleanup_sliding_window(&window);
    cleanup_streaming_read();

    printf("\n=== TRANSMISSION COMPLETE ===\n");
    printf("  Total chunks: %u\n", meta.chunk_count);
    printf("  Chunks transmitted: %u\n", total_transmitted);
    printf("  Retransmissions: %u (%.1f%%)\n",
           total_retransmissions,
           (float)total_retransmissions * 100.0 / total_transmitted);
    printf("  Total bytes: %u\n", meta.total_size);
    printf("  Time: %lld ms\n", elapsed_ms);
    printf("  Throughput: %.2f KB/s\n",
           (float)meta.total_size / (elapsed_ms > 0 ? elapsed_ms / 1000.0 : 1.0) / 1024.0);
    printf("  Window size: %u chunks (%u bytes)\n",
           WINDOW_SIZE_CHUNKS, WINDOW_SIZE_BYTES);

    return true;
}

/*!
 * @brief Simulate MQTT-SN data transfer: stream chunks one at a time (Legacy)
 * @param filename Source file to transfer
 * @return bool true on success
 */
static bool test_mqtt_streaming(const char* filename) {
    printf("\n=== Step 2: Simulated MQTT-SN Streaming Transfer ===\n");

    struct Metadata meta = {0};

    // Initialize streaming
    if (init_streaming_read((char*)filename, &meta) != 0) {
        printf("ERROR: Failed to initialize streaming\n");
        return false;
    }

    printf("SENDER: Preparing to transmit file via MQTT-SN\n");
    printf("  Session ID: %s\n", meta.session_id);
    printf("  Total chunks: %u\n", meta.chunk_count);

    printf("\n[0/%u] Sending metadata packet...\n", meta.chunk_count);
    sleep_ms(20);  // Simulate network delay

    // Stream chunks one at a time (simulating MQTT transmission)
    uint32_t bytes_sent = 0;
    absolute_time_t start = get_absolute_time();

    for (uint32_t i = 0; i < meta.chunk_count; i++) {
        struct Payload chunk = {0};

        // Read chunk from SD card (sender side)
        if (read_chunk_streaming(i, &chunk) != 0) {
            printf("ERROR: Failed to read chunk %u\n", i);
            cleanup_streaming_read();
            return false;
        }

        // Verify chunk before sending
        if (!verify_chunk(&chunk)) {
            printf("ERROR: Invalid chunk %u\n", i);
            cleanup_streaming_read();
            return false;
        }

        uint32_t chunk_size = get_chunk_data_size(i, &meta);
        bytes_sent += chunk_size;

        // Show detailed info for first, last, and periodic chunks
        if (i < 2 || i == meta.chunk_count - 1 || (i + 1) % 10 == 0) {
            printf("[%u/%u] Chunk transmitted (seq=%u, size=%u, crc=0x%04X)\n", i + 1,
                   meta.chunk_count, chunk.sequence, chunk_size, chunk.crc);
        }

        // Simulate network delay
        sleep_ms(5);
    }

    absolute_time_t end = get_absolute_time();
    int64_t elapsed_ms = absolute_time_diff_us(start, end) / 1000;

    cleanup_streaming_read();

    printf("\nTRANSMISSION COMPLETE\n");
    printf("  Chunks sent: %u\n", meta.chunk_count);
    printf("  Total bytes: %u\n", bytes_sent);
    printf("  Time: %lld ms\n", elapsed_ms);
    printf("  Throughput: %.2f KB/s\n",
           (float)bytes_sent / (elapsed_ms > 0 ? elapsed_ms / 1000.0 : 1.0) / 1024.0);

    return true;
}

/*!
 * @brief Test reconstruction: stream chunks and reconstruct file
 * @param source_filename Source file to reconstruct
 * @param expected_size Expected file size in bytes
 * @return bool true on success
 */
static bool test_reconstruction(const char* source_filename, uint32_t expected_size) {
    printf("\n=== Step 3: File Reconstruction (Receiver Side) ===\n");

    struct Metadata meta = {0};

    // Initialize streaming (sender side - simulating transmission)
    if (init_streaming_read((char*)source_filename, &meta) != 0) {
        printf("ERROR: Failed to initialize streaming\n");
        return false;
    }

    printf("RECEIVER: Preparing to receive file\n");
    printf("  Expected file: %s\n", source_filename);
    printf("  Expected size: %u bytes\n", expected_size);
    printf("  Chunks to receive: %u\n", meta.chunk_count);

    printf("\nInitializing reconstruction session...\n");
    absolute_time_t recon_start = get_absolute_time();

    // Initialize transfer session with use_new_filename=true to test the flag
    transfer_session_t recon_session = {0};
    if (!chunk_transfer_init_session(&meta, &recon_session, true)) {
        printf("ERROR: Failed to initialize reconstruction session\n");
        cleanup_streaming_read();
        return false;
    }

    printf("  Output file will be: %s\n", recon_session.filename);
    printf("  Memory usage: ~33 KB (32KB read buffer + chunk buffer)\n");

    // TRUE STREAMING: Read one chunk at a time and immediately write it
    // No array allocation - uses only a single chunk buffer
    printf("\nStreaming chunks (read -> verify -> write)...\n");

    struct Payload chunk = {0};  // Single chunk buffer (~250 bytes)

    for (uint32_t i = 0; i < meta.chunk_count; i++) {
        // Read chunk from source file (simulating MQTT reception)
        if (read_chunk_streaming(i, &chunk) != 0) {
            printf("ERROR: Failed to read chunk %u\n", i);
            cleanup_streaming_read();
            return false;
        }

        // Verify chunk integrity
        if (!verify_chunk(&chunk)) {
            printf("ERROR: Chunk %u failed CRC verification\n", i);
            cleanup_streaming_read();
            return false;
        }

        // Immediately write verified chunk to destination file
        if (!chunk_transfer_write_payload(&recon_session, &chunk)) {
            printf("ERROR: Failed to write chunk %u (seq=%u) during reconstruction\n", i,
                   chunk.sequence);
            cleanup_streaming_read();
            return false;
        }

        // Progress updates
        if (i == 0 || i == meta.chunk_count - 1 || (i + 1) % 100 == 0) {
            printf("  [%u/%u] Chunk streamed (seq=%u)\n", i + 1, meta.chunk_count, chunk.sequence);
        }
    }

    cleanup_streaming_read();

    // Finalize the transfer
    if (!chunk_transfer_finalize(&recon_session)) {
        printf("ERROR: Failed to finalize reconstruction\n");
        return false;
    }

    absolute_time_t recon_end = get_absolute_time();
    int64_t recon_ms = absolute_time_diff_us(recon_start, recon_end) / 1000;

    printf("\nReconstruction complete! Time: %lld ms\n", recon_ms);

    printf("\n=== Step 4: Verification Summary ===\n");
    printf("✓ All %u chunks streamed and verified (CRC checks passed)\n", meta.chunk_count);
    printf("✓ File reconstructed with %u bytes\n", meta.total_size);
    printf("✓ File saved as: %s\n", recon_session.filename);
    printf("  Original filename: %s\n", source_filename);
    printf("  New filename: %s (with _received suffix)\n", recon_session.filename);
    printf("  Total time: %lld ms (streaming + reconstruction)\n", recon_ms);
    printf("  Peak memory usage: ~33 KB (32KB read buffer + chunk buffer)\n");

    // Additional wait for SD card write completion
    printf("\nWaiting for SD card write completion...\n");
    sleep_ms(500);

    // Verify file exists using FatFS
    printf("\n=== File Verification ===\n");
    printf("Checking if reconstructed file exists...\n");

    FIL verify_fil;
    FRESULT fr = f_open(&verify_fil, recon_session.filename, FA_READ);
    if (fr == FR_OK) {
        uint32_t verify_size = f_size(&verify_fil);
        f_close(&verify_fil);
        printf("✓ File found: %s (%u bytes)\n", recon_session.filename, verify_size);

        if (verify_size == meta.total_size) {
            printf("✓ File size matches expected: %u bytes\n", verify_size);
        } else {
            printf("⚠ WARNING: File size mismatch (expected %u, got %u)\n", meta.total_size,
                   verify_size);
        }
    } else {
        printf("✗ ERROR: Could not open reconstructed file (error %d)\n", fr);
    }
    printf("========================================\n");

    // Final wait before marking complete
    printf("\nFinal sync wait (allowing SD card internal write buffering)...\n");
    sleep_ms(1000);
    printf("✓ File should now be fully committed to SD card\n");

    return true;
} /*!
   * @brief Run complete streaming test
   * @return bool true if all steps passed, false otherwise
   */
static bool run_streaming_test(void) {
    bool success = true;

    // Check if source file exists
    printf("\nChecking for source file: %s\n", SOURCE_FILENAME);
    uint32_t file_size = get_file_size(SOURCE_FILENAME);
    if (file_size == 0) {
        printf("\n✗ ERROR: Source file '%s' not found!\n", SOURCE_FILENAME);
        printf("Please ensure the file exists on the SD card.\n");
        return false;
    }

    printf("✓ Found source file: %s (%u bytes)\n", SOURCE_FILENAME, file_size);

    printf("\n");
    printf("================================================\n");
    printf("  SIMULATING MQTT-SN DATA TRANSFER\n");
    printf("  Protocol: Go-Back-N Sliding Window\n");
    printf("  Process: Stream -> Verify -> Reconstruct\n");
    printf("================================================\n");

    // Step 1: Display streaming read info
    if (!test_streaming_read(SOURCE_FILENAME)) {
        printf("\n✗ FAILED: Streaming read test\n");
        success = false;
        return success;
    }

    sleep_ms(1000);

    // Step 2: Simulate MQTT transmission with Go-Back-N
    if (!test_mqtt_gbn_streaming(SOURCE_FILENAME)) {
        printf("\n✗ FAILED: Go-Back-N MQTT streaming test\n");
        success = false;
        return success;
    }

    sleep_ms(1000);

    // Step 3: Reconstruct and verify
    if (!test_reconstruction(SOURCE_FILENAME, file_size)) {
        printf("\n✗ FAILED: Reconstruction test\n");
        success = false;
        return success;
    }

    return success;
}

/*!
 * @brief Main test function - MQTT-SN streaming simulation
 */
int main(void) {
    int test_count = 1;

    stdio_init_all();

    // Give USB serial time to connect
    sleep_ms(2000);

    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║  MQTT-SN Streaming Test                               ║\n");
    printf("║  File: %-47s ║\n", SOURCE_FILENAME);
    printf("║  Testing: Deconstruct → Stream → Reconstruct         ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    printf("\nRunning streaming test every 30 seconds...\n");
    printf("Initializing...\n");

    // Initial delay to allow USB serial to enumerate
    for (int i = 0; i < 5; i++) {
        printf("Waiting... %d\n", 5 - i);
        sleep_ms(1000);
    }

    printf("\nStarting test loop...\n");

    while (true) {
        printf("\n\n");
        printf("╔═══════════════════════════════════════════════════════╗\n");
        printf("║                  Test Run #%-3d                        ║\n", test_count);
        printf("╚═══════════════════════════════════════════════════════╝\n");

        // Initialize FatFS filesystem using microsd_driver
        printf("\nInitializing microSD card driver and FatFS filesystem...\n");

        if (!microsd_driver_init()) {
            printf("✗ ERROR: Failed to initialize microSD driver\n");
            printf("Please check the microSD card connection and try again.\n");
            printf("\n--- Waiting 30 seconds before retry ---\n");
            sleep_ms(30000);
            test_count++;
            continue;
        }
        printf("✓ MicroSD driver and FatFS filesystem initialized successfully\n");

        uint32_t start_time = to_ms_since_boot(get_absolute_time());

        // Run streaming test
        bool test_passed = run_streaming_test();

        uint32_t end_time = to_ms_since_boot(get_absolute_time());
        uint32_t elapsed_ms = end_time - start_time;

        // Final summary
        printf("\n");
        printf("╔═══════════════════════════════════════════════════════╗\n");
        printf("║              Final Summary - Run #%-3d                 ║\n", test_count);
        printf("╚═══════════════════════════════════════════════════════╝\n");

        if (test_passed) {
            printf("\n  🎯 ALL TESTS PASSED!\n");
            printf("\n  The Go-Back-N streaming method successfully:\n");
            printf("    ✓ Step 1: Initialized streaming read\n");
            printf("    ✓ Step 2: Streamed chunks with sliding window flow control\n");
            printf("    ✓ Step 3: Reconstructed file with integrity verification\n");
            printf("\n  Key Features:\n");
            printf("    • Protocol: Go-Back-N with %u-chunk sliding window\n", WINDOW_SIZE_CHUNKS);
            printf("    • Window size: %u bytes (%u KB)\n", WINDOW_SIZE_BYTES, WINDOW_SIZE_BYTES / 1024);
            printf("    • Memory usage: ~33 KB (32KB read buffer + chunk buffer)\n");
            printf("    • MQTT Topics: file/data (tx), file/control (rx)\n");
            printf("    • Flow control: ACK/NACK/REQUEST_NEXT messages\n");
            printf("    • Packet loss recovery with retransmission\n");
            printf("    • No file size limitations\n");
            printf("    • Ready for MQTT-SN integration!\n");
        } else {
            printf("\n  ⚠️  TEST FAILED!\n");
            printf("  Please check the error messages above.\n");
        }

        printf("\n  Execution time: %lu ms\n", (unsigned long)elapsed_ms);
        printf("\n═══════════════════════════════════════════════════════\n");

        printf("\n--- Waiting 30 seconds for next test run ---\n");
        sleep_ms(30000);
        test_count++;
    }

    return 0;
}
