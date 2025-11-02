/*!
 * @file    streaming_test.c
 * @brief   Streaming chunk read test - memory efficient file transfer
 * @author  INF2004 Project Team
 * @date    2025
 *
 * This test demonstrates the streaming chunk read API which is much more
 * memory efficient than loading entire files into RAM. It compares the
 * old deconstruct() method with the new streaming approach.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../drivers/microsd_driver.h"
#include "data_frame.h"
#include "pico/stdlib.h"

/*! Test configuration constants */
#define TEST_FILENAME "STREAM_TEST.txt"
#define SMALL_FILE_SIZE 500     // Small: 500 bytes (~3 chunks)
#define MEDIUM_FILE_SIZE 5000   // Medium: 5KB (~21 chunks)
#define LARGE_FILE_SIZE 30000   // Large: 30KB (~127 chunks)
#define XLARGE_FILE_SIZE 50000  // XLarge: 50KB (~211 chunks) - exceeds 32KB limit of old method

/*!
 * @brief Create a test file with known pattern
 * @param filename Base filename to create
 * @param size File size in bytes
 * @param actual_filename Buffer to store the actual created filename (with version suffix)
 * @return bool true on success, false on failure
 */
static bool create_test_file(const char* filename, uint32_t size, char* actual_filename) {
    filesystem_info_t fs_info;

    printf("\n=== Creating Test File ===\n");
    printf("Requested filename: %s\n", filename);
    printf("Size: %u bytes\n", size);

    if (!microsd_init_filesystem(&fs_info)) {
        printf("ERROR: Failed to initialize filesystem\n");
        return false;
    }

    // Note: If file exists, microsd_create_file will create a versioned filename
    // (e.g., STREAM_TEST_1.txt, STREAM_TEST_2.txt, etc.)
    // We need to find out what filename was actually created by checking the directory

    // Generate test data with pattern
    uint8_t* test_data = (uint8_t*)malloc(size);
    if (test_data == NULL) {
        printf("ERROR: Memory allocation failed\n");
        return false;
    }

    // Fill with test content for integrity verification
    const char content[] =
        "Hololive Production is, without a doubt, one of the most incredible and revolutionary phenomena in the history of modern entertainment. "
        "What Cover Corporation has managed to build is nothing short of extraordinary, a global ecosystem of talent, creativity, and community that has brought joy to millions. "
        "It's not just a company; it's a cultural movement. The sheer greatness of Hololive stems from its core: the talents themselves. "
        "Each VTuber, whether from Hololive JP, EN, ID, or the amazing talents in Holostars, is a unique star shining with their own brilliant light.\n\n"
        "The diversity in talent is breathtaking. You have idols with angelic singing voices who produce original music and hold spectacular concerts, "
        "captivating audiences with their sheer vocal power and emotional delivery. You have elite gamers who can master any genre, "
        "from fast-paced FPS games to grueling endurance challenges, all while keeping their audience entertained with hilarious commentary and genuine reactions. "
        "You have artists who create stunning illustrations live on stream, comedians with impeccable timing, and storytellers who can weave intricate lore that keeps the community hooked. "
        "This is the magic of Hololive: it's an entire universe of entertainment packed into one agency.\n\n"
        "One of the greatest aspects of Hololive is the genuine sense of camaraderie, not just among the talents but with the entire fanbase. "
        "The talents frequently collaborate, creating some of the most memorable and chaotic moments in streaming history. "
        "These interactions feel authentic, like watching a group of close friends just having fun, and that positive energy is infectious. "
        "They support each other's milestones, celebrate victories together, and comfort each other during difficult times. "
        "This creates a \"one big family\" atmosphere that makes fans feel like they are part of something truly special and supportive.\n\n"
        "The community that has formed around Hololive is a testament to this positive environment. "
        "Fans are incredibly passionate, creative, and supportive. They create stunning fan art, hilarious video clips and memes (which the talents often react to, closing the loop of interaction), "
        "and detailed translations that bridge language barriers, allowing Hololive's influence to spread globally. "
        "Cover Corp. itself fosters this positivity with fan guidelines that encourage respect and support, helping to maintain one of the most welcoming communities online.\n\n"
        "Let's talk about the innovation. Hololive is at the forefront of virtual entertainment technology. "
        "Their 3D live concerts are industry-defining spectacles, blending high-energy performances, stunning augmented reality effects, "
        "and real-time audience interaction on a scale that rivals major pop concerts. "
        "The company's continuous investment in its proprietary streaming and motion capture technology ensures that the talents' avatars are expressive, dynamic, "
        "and able to convey a full range of emotion, which is crucial for forming a genuine connection with viewers.\n\n"
        "The global expansion of Hololive has been a massive success. Hololive English (EN) and Hololive Indonesia (ID) have brought the magic to new audiences, "
        "creating international superstars who have topped charts and broken streaming records. Talents from Hololive English -Myth-, for example, had one of the most explosive debuts in YouTube history. "
        "This global reach has proven that the appeal of Hololive's entertainment model is universal. It's not just about the anime aesthetic; it's about the genuine personalities and the connections they build.\n\n"
        "The male branch, Holostars, is equally fantastic, filled with incredibly talented and charismatic individuals. "
        "From Holostars JP to the global reach of Holostars EN, these \"stars\" showcase a different but equally compelling brand of entertainment. "
        "They are fantastic singers, hilarious streamers, and have a unique group dynamic that has earned them a dedicated and growing fanbase. "
        "Their success proves that Cover's formula for nurturing talent and community works across the board.\n\n"
        "The impact of Hololive is undeniable. They were a primary catalyst for the \"VTuber Boom\" of 2020, introducing millions of people around the world to the concept of virtual entertainers. "
        "They have won numerous awards, including \"Best VTuber Organization,\" and have collaborated with major brands in gaming, music, and even sports. "
        "Seeing Hololive talents appear in mainstream advertisements or at major events like the LA Dodgers' \"Japan Night\" is a clear sign of just how significant their cultural footprint has become.\n\n"
        "In summary, Hololive Production is a beacon of positivity and creativity in the digital age. "
        "It has set a new standard for what entertainment can be, proving that virtual avatars can be vessels for some of the most genuine, hilarious, and heartwarming personalities online. "
        "The talents are incredibly hardworking and passionate, the community is vibrant and supportive, and the company behind it all is constantly innovating. "
        "Hololive is more than just \"great\"; it's a source of daily happiness for a global audience, a true revolution in media, and a phenomenon that will be celebrated for years to come. "
        "The talent, the music, the streams, the community—every facet of Hololive is a shining example of excellence.\n";

    uint32_t content_len = strlen(content);

    // Repeat content to fill the requested size
    uint32_t offset = 0;
    while (offset < size) {
        uint32_t bytes_to_copy = (size - offset < content_len) ? (size - offset) : content_len;
        memcpy(test_data + offset, content, bytes_to_copy);
        offset += bytes_to_copy;
    }

    if (!microsd_create_file(&fs_info, filename, test_data, size)) {
        printf("ERROR: Failed to create test file\n");
        free(test_data);
        return false;
    }

    free(test_data);

    // Find the actual created filename by reading directory and looking for our size
    // The driver creates versioned files, so we need to find which one was just created
    // We'll search for the LAST file (most recently added) with our exact size and base name
    uint8_t dir_buffer[512];
    uint32_t root_sector = fs_info.partition_offset + fs_info.cluster_heap_offset +
                           ((fs_info.root_cluster - 2) * fs_info.sectors_per_cluster);

    strcpy(actual_filename, filename);  // Default to requested filename

    // Extract base filename without extension for matching
    char base_name[256];
    strncpy(base_name, filename, sizeof(base_name) - 1);
    base_name[sizeof(base_name) - 1] = '\0';
    char* dot = strrchr(base_name, '.');
    if (dot) *dot = '\0';

    if (microsd_read_block(root_sector, dir_buffer)) {
        // Scan directory for ALL files with matching size and base name
        // Keep the LAST one found (most recently created)
        for (int i = 0; i < 512; i += 32) {
            uint8_t entry_type = dir_buffer[i];
            if (entry_type == 0x85) {  // File entry
                uint8_t secondary_count = dir_buffer[i + 1];

                // Check if stream extension follows
                if ((i + 32) < 512 && dir_buffer[i + 32] == 0xC0) {
                    uint64_t* file_size_ptr = (uint64_t*)&dir_buffer[i + 32 + 8];
                    uint64_t file_size = *file_size_ptr;

                    if (file_size == size) {
                        // Found a file with matching size! Extract filename
                        char found_name[256] = "";
                        uint32_t char_idx = 0;

                        for (uint32_t j = 2; j <= secondary_count && (i + j * 32) < 512; j++) {
                            if (dir_buffer[(i + j * 32)] == 0xC1) {  // Name entry
                                uint16_t* utf16_chars = (uint16_t*)&dir_buffer[(i + j * 32) + 2];
                                for (int k = 0; k < 15 && char_idx < 255; k++) {
                                    if (utf16_chars[k] == 0) break;
                                    if (utf16_chars[k] < 128) {
                                        found_name[char_idx++] = (char)utf16_chars[k];
                                    }
                                }
                            }
                        }
                        found_name[char_idx] = '\0';

                        // Check if this matches our base filename pattern
                        // Compare against base_name (e.g., "STREAM_TEST" matches "STREAM_TEST_1.txt")
                        if (strncmp(found_name, base_name, strlen(base_name)) == 0) {
                            // This matches! Save it (last match wins = most recently created)
                            strcpy(actual_filename, found_name);
                        }
                    }
                }
            }
        }
    }

    printf("SUCCESS: Test file created as: %s\n", actual_filename);
    return true;
}

/*!
 * @brief Test OLD method: deconstruct (loads entire file)
 */
static bool test_old_deconstruct(const char* filename) {
    printf("\n=== Testing OLD Method: deconstruct() ===\n");

    struct Metadata meta = {0};
    struct Payload* chunks = NULL;

    uint32_t free_heap_before = 0;  // Would use getFreeHeap() on real system
    printf("Memory before: (tracking not available)\n");

    absolute_time_t start = get_absolute_time();

    if (deconstruct((char*)filename, &meta, &chunks) != 0) {
        printf("ERROR: Deconstruction failed\n");
        return false;
    }

    absolute_time_t end = get_absolute_time();
    int64_t elapsed_us = absolute_time_diff_us(start, end);

    printf("SUCCESS: File deconstructed\n");
    printf("  Chunks: %u\n", meta.chunk_count);
    printf("  File size: %u bytes\n", meta.total_size);
    printf("  Time: %lld us (%.2f ms)\n", elapsed_us, elapsed_us / 1000.0);
    printf("  Memory allocated: ~%u bytes (chunks array)\n",
           meta.chunk_count * sizeof(struct Payload));

    // Verify a few random chunks
    printf("\nVerifying chunks:\n");
    for (uint32_t i = 0; i < meta.chunk_count && i < 5; i++) {
        printf("\n--- Verifying Chunk %u ---\n", i);

        if (!verify_chunk(&chunks[i])) {
            printf("  Chunk %u: INVALID!\n", i);
            free(chunks);
            return false;
        }
        printf("  Chunk %u: VALID (seq=%u, size=%u, crc=0x%04X)\n",
               i, chunks[i].sequence, chunks[i].size, chunks[i].crc);

        // Show first 48 bytes as hex
        printf("  First 48 bytes (hex):\n  ");
        for (uint32_t j = 0; j < 48 && j < chunks[i].size; j++) {
            if (j > 0 && j % 16 == 0) printf("\n  ");
            printf("%02X ", chunks[i].data[j]);
        }
        printf("\n");

        // Show ASCII preview
        printf("  ASCII: \"");
        for (uint32_t j = 0; j < 70 && j < chunks[i].size; j++) {
            char c = chunks[i].data[j];
            printf("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        printf("%s\"\n", chunks[i].size > 70 ? "..." : "");
    }

    free(chunks);
    printf("\nOLD method completed successfully\n");
    return true;
}

/*!
 * @brief Test NEW method: streaming read
 */
static bool test_new_streaming(const char* filename) {
    printf("\n=== Testing NEW Method: Streaming ===\n");

    struct Metadata meta = {0};

    printf("Memory before: (tracking not available)\n");

    absolute_time_t start = get_absolute_time();

    // Initialize streaming read
    if (init_streaming_read((char*)filename, &meta) != 0) {
        printf("ERROR: Failed to initialize streaming read\n");
        return false;
    }

    absolute_time_t init_end = get_absolute_time();
    int64_t init_us = absolute_time_diff_us(start, init_end);

    printf("SUCCESS: Streaming initialized\n");
    printf("  Chunks: %u\n", meta.chunk_count);
    printf("  File size: %u bytes\n", meta.total_size);
    printf("  Session ID: %s\n", meta.session_id);
    printf("  Init time: %lld us (%.2f ms)\n", init_us, init_us / 1000.0);
    printf("  Memory allocated: ~%u bytes (single chunk buffer)\n",
           sizeof(struct Payload));

    // Read and verify chunks one at a time
    printf("\nReading chunks sequentially:\n");
    uint32_t chunks_to_verify = (meta.chunk_count < 5) ? meta.chunk_count : 5;

    for (uint32_t i = 0; i < chunks_to_verify; i++) {
        struct Payload chunk = {0};

        printf("\n--- Reading Chunk %u ---\n", i);

        if (read_chunk_streaming(i, &chunk) != 0) {
            printf("  Chunk %u: READ FAILED!\n", i);
            cleanup_streaming_read();
            return false;
        }

        if (!verify_chunk(&chunk)) {
            printf("  Chunk %u: INVALID!\n", i);
            cleanup_streaming_read();
            return false;
        }

        printf("  Chunk %u: VALID (seq=%u, size=%u, crc=0x%04X)\n",
               i, chunk.sequence, chunk.size, chunk.crc);

        // Show first 64 bytes of chunk data
        printf("  First 64 bytes of data:\n");
        printf("  ");
        for (uint32_t j = 0; j < 64 && j < chunk.size; j++) {
            if (j > 0 && j % 16 == 0) printf("\n  ");
            printf("%02X ", chunk.data[j]);
        }
        printf("\n");

        // Show as ASCII (printable characters only)
        printf("  ASCII preview: \"");
        for (uint32_t j = 0; j < 80 && j < chunk.size; j++) {
            char c = chunk.data[j];
            if (c >= 32 && c <= 126) {
                printf("%c", c);
            } else if (c == '\n') {
                printf("\\n");
            } else {
                printf(".");
            }
        }
        printf("%s\"\n", chunk.size > 80 ? "..." : "");
    }

    // Test random access
    printf("\n=== Testing Random Access ===\n");
    if (meta.chunk_count > 10) {
        uint32_t random_indices[] = {
            meta.chunk_count / 4,
            meta.chunk_count / 2,
            meta.chunk_count - 1};

        for (uint32_t i = 0; i < 3; i++) {
            uint32_t idx = random_indices[i];
            struct Payload chunk = {0};

            printf("\n--- Random Access: Chunk %u ---\n", idx);

            if (read_chunk_streaming(idx, &chunk) != 0) {
                printf("  Chunk %u: READ FAILED!\n", idx);
                cleanup_streaming_read();
                return false;
            }

            if (!verify_chunk(&chunk)) {
                printf("  Chunk %u: INVALID!\n", idx);
                cleanup_streaming_read();
                return false;
            }

            printf("  Chunk %u: VALID (seq=%u, size=%u, crc=0x%04X)\n",
                   idx, chunk.sequence, chunk.size, chunk.crc);

            // Show snippet of data
            printf("  Data snippet: \"");
            for (uint32_t j = 0; j < 60 && j < chunk.size; j++) {
                char c = chunk.data[j];
                if (c >= 32 && c <= 126) {
                    printf("%c", c);
                } else if (c == '\n') {
                    printf("\\n");
                } else {
                    printf(".");
                }
            }
            printf("%s\"\n", chunk.size > 60 ? "..." : "");
        }
    } else {
        printf("(Skipped - need >10 chunks for random access test)\n");
    }

    absolute_time_t end = get_absolute_time();
    int64_t total_us = absolute_time_diff_us(start, end);

    cleanup_streaming_read();

    printf("\nNEW method completed successfully\n");
    printf("Total time: %lld us (%.2f ms)\n", total_us, total_us / 1000.0);
    return true;
}

/*!
 * @brief Compare streaming vs deconstruct performance
 */
static bool test_performance_comparison(const char* filename, uint32_t file_size) {
    printf("\n");
    printf("================================================\n");
    printf("  PERFORMANCE COMPARISON\n");
    printf("  File: %s (%u bytes)\n", filename, file_size);
    printf("================================================\n");

    bool old_success = true;
    int64_t old_time = 0;

    // Only test OLD method if file is within 32KB limit
    if (file_size <= 32768) {
        // Test OLD method
        absolute_time_t old_start = get_absolute_time();
        old_success = test_old_deconstruct(filename);
        absolute_time_t old_end = get_absolute_time();
        old_time = absolute_time_diff_us(old_start, old_end);

        if (!old_success) {
            printf("\nOLD method failed!\n");
        }
    } else {
        printf("\n=== OLD Method: SKIPPED ===\n");
        printf("File size (%u bytes) exceeds 32KB limit of old method\n", file_size);
        old_success = false;  // Mark as failed since it can't handle this size
    }

    sleep_ms(1000);  // Let system settle

    // Test NEW method
    absolute_time_t new_start = get_absolute_time();
    bool new_success = test_new_streaming(filename);
    absolute_time_t new_end = get_absolute_time();
    int64_t new_time = absolute_time_diff_us(new_start, new_end);

    if (!new_success) {
        printf("\nNEW method failed!\n");
    }

    // Compare results
    printf("\n");
    printf("================================================\n");
    printf("  COMPARISON RESULTS\n");
    printf("================================================\n");

    if (file_size <= 32768) {
        printf("OLD Method (deconstruct):\n");
        printf("  Status: %s\n", old_success ? "SUCCESS" : "FAILED");
        printf("  Time: %.2f ms\n", old_time / 1000.0);
        printf("  Memory: ~%u KB (entire file + chunks array)\n",
               (file_size + (file_size / PAYLOAD_DATA_SIZE) * sizeof(struct Payload)) / 1024);
    } else {
        printf("OLD Method (deconstruct):\n");
        printf("  Status: NOT SUPPORTED (file > 32KB)\n");
        printf("  Limitation: Hard-coded 32KB buffer limit\n");
    }

    printf("\nNEW Method (streaming):\n");
    printf("  Status: %s\n", new_success ? "SUCCESS" : "FAILED");
    printf("  Time: %.2f ms\n", new_time / 1000.0);
    printf("  Memory: ~0.5 KB (single chunk + metadata)\n");

    if (old_success && new_success && file_size <= 32768) {
        float speedup = (float)old_time / (float)new_time;
        uint32_t memory_saved = file_size + (file_size / PAYLOAD_DATA_SIZE) * sizeof(struct Payload) - 512;

        printf("\nCOMPARISON:\n");
        printf("  Speed: %.2fx %s\n", speedup > 1.0 ? speedup : 1.0 / speedup,
               speedup > 1.0 ? "faster" : "slower");
        printf("  Memory saved: ~%u KB (%.1f%% reduction)\n",
               memory_saved / 1024,
               (float)memory_saved / (file_size + (file_size / PAYLOAD_DATA_SIZE) * sizeof(struct Payload)) * 100.0);
    } else if (new_success && file_size > 32768) {
        printf("\nKEY ADVANTAGES:\n");
        printf("  ✓ Streaming method handles files of ANY size\n");
        printf("  ✓ Old method limited to 32KB maximum\n");
        printf("  ✓ Memory usage: 99%% reduction compared to theoretical old method\n");
    }
    printf("================================================\n");

    // For success, new method must work
    return new_success;
}

/*!
 * @brief Test streaming with simulated MQTT transmission
 */
static bool test_streaming_mqtt_simulation(const char* filename) {
    printf("\n");
    printf("================================================\n");
    printf("  SIMULATED MQTT-SN STREAMING TRANSFER\n");
    printf("================================================\n");

    struct Metadata meta = {0};

    // Initialize streaming
    if (init_streaming_read((char*)filename, &meta) != 0) {
        printf("ERROR: Failed to initialize streaming\n");
        return false;
    }

    printf("Metadata prepared:\n");
    printf("  Session: %s\n", meta.session_id);
    printf("  File: %s\n", meta.filename);
    printf("  Size: %u bytes\n", meta.total_size);
    printf("  Chunks: %u\n", meta.chunk_count);
    printf("  CRC: 0x%04X\n", meta.file_crc);

    printf("\nSimulating MQTT-SN transmission:\n");
    printf("[0/0] Sending metadata...\n");
    sleep_ms(50);  // Simulate network delay

    // Stream chunks one at a time (like MQTT would do)
    uint32_t bytes_sent = 0;
    absolute_time_t start = get_absolute_time();

    for (uint32_t i = 0; i < meta.chunk_count; i++) {
        struct Payload chunk = {0};

        // Read chunk from SD card
        if (read_chunk_streaming(i, &chunk) != 0) {
            printf("ERROR: Failed to read chunk %u\n", i);
            cleanup_streaming_read();
            return false;
        }

        // Verify chunk
        if (!verify_chunk(&chunk)) {
            printf("ERROR: Invalid chunk %u\n", i);
            cleanup_streaming_read();
            return false;
        }

        // Simulate sending via MQTT
        bytes_sent += chunk.size;

        // Detailed output for first few chunks
        if (i < 3 || i == meta.chunk_count - 1) {
            printf("\n[Chunk %u/%u] Transmitting...\n", i + 1, meta.chunk_count);
            printf("  Sequence: %u\n", chunk.sequence);
            printf("  Size: %u bytes\n", chunk.size);
            printf("  CRC: 0x%04X\n", chunk.crc);
            printf("  Data preview: \"");
            for (uint32_t j = 0; j < 50 && j < chunk.size; j++) {
                char c = chunk.data[j];
                printf("%c", (c >= 32 && c <= 126) ? c : '.');
            }
            printf("%s\"\n", chunk.size > 50 ? "..." : "");
        }

        // Progress update every 25 chunks
        if ((i + 1) % 25 == 0 && i >= 3 && i != meta.chunk_count - 1) {
            float progress = ((float)(i + 1) / meta.chunk_count) * 100.0;
            printf("[%u/%u] %.1f%% - %u bytes sent\n",
                   i + 1, meta.chunk_count, progress, bytes_sent);
        }

        // Simulate network delay
        sleep_ms(10);
    }
    absolute_time_t end = get_absolute_time();
    int64_t elapsed_ms = absolute_time_diff_us(start, end) / 1000;

    cleanup_streaming_read();

    printf("\nTransfer complete!\n");
    printf("  Total bytes: %u\n", bytes_sent);
    printf("  Total time: %lld ms\n", elapsed_ms);
    printf("  Throughput: %.2f KB/s\n",
           (float)bytes_sent / (elapsed_ms / 1000.0) / 1024.0);
    printf("  Peak memory: ~0.5 KB (streaming buffer)\n");

    return true;
}

/*!
 * @brief Test reconstruction: stream chunks and reconstruct to verify integrity
 */
static bool test_reconstruction(const char* source_filename, uint32_t expected_size) {
    printf("\n=== Reconstruction Test (End-to-End Integrity) ===\n");
    printf("This test simulates: sender streams -> receiver reconstructs\n\n");

    struct Metadata meta = {0};
    struct Payload* chunks = NULL;

    // Step 1: Initialize streaming (sender side)
    printf("SENDER: Initializing streaming read...\n");
    if (init_streaming_read((char*)source_filename, &meta) != 0) {
        printf("ERROR: Failed to initialize streaming\n");
        return false;
    }

    printf("  Source file: %s\n", source_filename);
    printf("  File size: %u bytes\n", meta.total_size);
    printf("  Chunks: %u\n", meta.chunk_count);
    printf("  File CRC: 0x%04X\n", meta.file_crc);

    // Verify expected size
    if (meta.total_size != expected_size) {
        printf("WARNING: File size mismatch (expected %u, got %u)\n",
               expected_size, meta.total_size);
    }

    // Step 2: Allocate chunks array (receiver side)
    printf("\nRECEIVER: Allocating %u chunks...\n", meta.chunk_count);
    chunks = (struct Payload*)malloc(meta.chunk_count * sizeof(struct Payload));
    if (chunks == NULL) {
        printf("ERROR: Failed to allocate chunks array\n");
        cleanup_streaming_read();
        return false;
    }

    // Step 3: Stream all chunks (simulating network transfer)
    printf("\nTRANSFER: Streaming chunks...\n");
    absolute_time_t transfer_start = get_absolute_time();

    for (uint32_t i = 0; i < meta.chunk_count; i++) {
        // Sender: Read chunk
        if (read_chunk_streaming(i, &chunks[i]) != 0) {
            printf("ERROR: Failed to read chunk %u\n", i);
            free(chunks);
            cleanup_streaming_read();
            return false;
        }

        // Receiver: Verify chunk integrity
        if (!verify_chunk(&chunks[i])) {
            printf("ERROR: Chunk %u failed verification (CRC mismatch)\n", i);
            free(chunks);
            cleanup_streaming_read();
            return false;
        }

        // Progress update
        if ((i + 1) % 25 == 0 || i == 0 || i == meta.chunk_count - 1) {
            float progress = ((float)(i + 1) / meta.chunk_count) * 100.0;
            printf("  [%u/%u] %.1f%% complete\n", i + 1, meta.chunk_count, progress);
        }
    }

    absolute_time_t transfer_end = get_absolute_time();
    int64_t transfer_ms = absolute_time_diff_us(transfer_start, transfer_end) / 1000;

    cleanup_streaming_read();

    printf("\nTransfer complete!\n");
    printf("  Time: %lld ms\n", transfer_ms);
    printf("  All %u chunks verified\n", meta.chunk_count);

    // Step 4: Reconstruct file (receiver side)
    char output_filename[80];
    snprintf(output_filename, sizeof(output_filename), "RECON_%s", source_filename);

    printf("\nRECEIVER: Reconstructing file...\n");
    printf("  Output: %s\n", output_filename);

    absolute_time_t recon_start = get_absolute_time();

    if (reconstruct(&meta, &chunks, output_filename) != 0) {
        printf("ERROR: File reconstruction failed\n");
        free(chunks);
        return false;
    }

    absolute_time_t recon_end = get_absolute_time();
    int64_t recon_ms = absolute_time_diff_us(recon_start, recon_end) / 1000;

    printf("Reconstruction complete!\n");
    printf("  Time: %lld ms\n", recon_ms);

    // Step 5: Verify reconstructed file matches original
    printf("\nVERIFICATION: Comparing original vs reconstructed...\n");

    filesystem_info_t fs_info;
    if (!microsd_init_filesystem(&fs_info)) {
        printf("ERROR: Failed to initialize filesystem for verification\n");
        free(chunks);
        return false;
    }

    // Read both files and compare
    uint8_t* original_buffer = (uint8_t*)malloc(expected_size);
    uint8_t* reconstructed_buffer = (uint8_t*)malloc(expected_size);

    if (original_buffer == NULL || reconstructed_buffer == NULL) {
        printf("ERROR: Failed to allocate comparison buffers\n");
        free(chunks);
        if (original_buffer) free(original_buffer);
        if (reconstructed_buffer) free(reconstructed_buffer);
        return false;
    }

    uint32_t original_bytes_read = 0;
    uint32_t reconstructed_bytes_read = 0;

    bool read_success = microsd_read_file(&fs_info, source_filename, original_buffer,
                                          expected_size, &original_bytes_read);
    read_success &= microsd_read_file(&fs_info, output_filename, reconstructed_buffer,
                                      expected_size, &reconstructed_bytes_read);

    if (!read_success) {
        printf("ERROR: Failed to read files for comparison\n");
        free(chunks);
        free(original_buffer);
        free(reconstructed_buffer);
        return false;
    }

    printf("  Original size: %u bytes\n", original_bytes_read);
    printf("  Reconstructed size: %u bytes\n", reconstructed_bytes_read);

    bool files_match = true;

    if (original_bytes_read != reconstructed_bytes_read) {
        printf("ERROR: File size mismatch!\n");
        files_match = false;
    } else {
        // Compare byte-by-byte
        for (uint32_t i = 0; i < original_bytes_read; i++) {
            if (original_buffer[i] != reconstructed_buffer[i]) {
                printf("ERROR: Byte mismatch at offset %u (original=0x%02X, recon=0x%02X)\n",
                       i, original_buffer[i], reconstructed_buffer[i]);
                files_match = false;
                break;
            }
        }
    }

    free(original_buffer);
    free(reconstructed_buffer);
    free(chunks);

    if (files_match) {
        printf("✓ Files match perfectly! Reconstruction successful.\n");
        printf("\nSummary:\n");
        printf("  Original: %s (%u bytes)\n", source_filename, expected_size);
        printf("  Reconstructed: %s (%u bytes)\n", output_filename, expected_size);
        printf("  Chunks transferred: %u\n", meta.chunk_count);
        printf("  Total time: %lld ms (transfer + reconstruction)\n",
               transfer_ms + recon_ms);
        return true;
    } else {
        printf("✗ Files do NOT match! Reconstruction FAILED.\n");
        return false;
    }
}

/*!
 * @brief Run complete streaming test suite
 */
static bool run_streaming_test_suite(uint32_t file_size) {
    char actual_filename[256];

    printf("\n");
    printf("========================================\n");
    printf("  STREAMING TEST SUITE\n");
    printf("  File size: %u bytes\n", file_size);
    printf("========================================\n");

    // Create test file and get the actual created filename
    if (!create_test_file(TEST_FILENAME, file_size, actual_filename)) {
        printf("\nFAILED: File creation\n");
        return false;
    }

    printf("\n** Using file: %s for all tests **\n", actual_filename);
    sleep_ms(500);

    // Run performance comparison
    if (!test_performance_comparison(actual_filename, file_size)) {
        printf("\nFAILED: Performance comparison\n");
        return false;
    }

    sleep_ms(500);

    // Run MQTT simulation
    if (!test_streaming_mqtt_simulation(actual_filename)) {
        printf("\nFAILED: MQTT simulation\n");
        return false;
    }

    sleep_ms(500);

    // Run reconstruction test (end-to-end integrity check)
    if (!test_reconstruction(actual_filename, file_size)) {
        printf("\nFAILED: Reconstruction test\n");
        return false;
    }

    printf("\n✓ All streaming tests PASSED for %u byte file\n", file_size);
    return true;
}

/*!
 * @brief Main test function
 */
int main(void) {
    int test_run = 1;

    stdio_init_all();

    // Wait for USB serial
    printf("Waiting for USB serial");
    for (int i = 0; i < 3; i++) {
        printf(".");
        sleep_ms(1000);
    }
    printf("\n\n");

    printf("========================================\n");
    printf("  STREAMING CHUNK READ TEST\n");
    printf("  Memory Efficient File Transfer\n");
    printf("========================================\n\n");

    while (true) {
        printf("\n");
        printf("****************************************\n");
        printf("  Test Run #%d\n", test_run);
        printf("****************************************\n");

        // Initialize microSD
        printf("\nInitializing microSD card...\n");
        if (!microsd_init()) {
            printf("ERROR: Failed to initialize microSD\n");
            printf("Retrying in 5 seconds...\n");
            sleep_ms(5000);
            continue;
        }
        printf("MicroSD initialized successfully\n");

        microsd_set_log_level(MICROSD_LOG_INFO);

        bool all_passed = true;

        // Test 1: Small file
        printf("\n>>> TEST 1: Small file (%u bytes) <<<\n", SMALL_FILE_SIZE);
        if (!run_streaming_test_suite(SMALL_FILE_SIZE)) {
            all_passed = false;
        }
        sleep_ms(2000);

        // Test 2: Medium file
        printf("\n>>> TEST 2: Medium file (%u bytes) <<<\n", MEDIUM_FILE_SIZE);
        if (!run_streaming_test_suite(MEDIUM_FILE_SIZE)) {
            all_passed = false;
        }
        sleep_ms(2000);

        // Test 3: Large file
        printf("\n>>> TEST 3: Large file (%u bytes) <<<\n", LARGE_FILE_SIZE);
        if (!run_streaming_test_suite(LARGE_FILE_SIZE)) {
            all_passed = false;
        }
        sleep_ms(2000);

        // Test 4: Extra large file (would fail with old method!)
        printf("\n>>> TEST 4: Extra large file (%u bytes) <<<\n", XLARGE_FILE_SIZE);
        printf("NOTE: This file size exceeds 32KB limit of old method\n");
        printf("Testing with streaming method ONLY (old method cannot handle this)\n\n");

        // Test streaming method for XLarge with full suite
        char xlarge_filename[256];
        if (!create_test_file(TEST_FILENAME, XLARGE_FILE_SIZE, xlarge_filename)) {
            printf("FAILED: Could not create XLarge file\n");
            all_passed = false;
        } else {
            printf("\n** Using file: %s for XLarge tests **\n", xlarge_filename);

            if (!test_new_streaming(xlarge_filename)) {
                printf("FAILED: Streaming test\n");
                all_passed = false;
            }
            sleep_ms(500);

            if (!test_streaming_mqtt_simulation(xlarge_filename)) {
                printf("FAILED: MQTT simulation\n");
                all_passed = false;
            }
            sleep_ms(500);

            // Add reconstruction test for XLarge
            if (!test_reconstruction(xlarge_filename, XLARGE_FILE_SIZE)) {
                printf("FAILED: Reconstruction test\n");
                all_passed = false;
            } else {
                printf("\n✓ All streaming tests PASSED for %u byte XLarge file\n", XLARGE_FILE_SIZE);
            }
        }

        // Final summary
        printf("\n\n");
        printf("========================================\n");
        printf("  FINAL RESULTS\n");
        printf("========================================\n");
        if (all_passed) {
            printf("✓ ALL TESTS PASSED!\n\n");
            printf("The streaming API demonstrates:\n");
            printf("- Memory efficient chunk reading\n");
            printf("- No file size limitations\n");
            printf("- Random chunk access support\n");
            printf("- Ready for MQTT-SN integration\n");
        } else {
            printf("✗ SOME TESTS FAILED\n");
            printf("Review errors above\n");
        }
        printf("========================================\n");

        printf("\n--- Waiting 20 seconds before next run ---\n");
        sleep_ms(20000);
        test_run++;
    }

    return 0;
}
