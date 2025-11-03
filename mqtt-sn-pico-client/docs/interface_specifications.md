# Interface Specifications

## 6.4. (DI-4) File Transfer Protocol Interface (fs/data_frame.h)

### Overview
- **Module name:** `file_transfer_protocol`
- **Purpose:** Implements the application-level logic for file deconstruction (chunking) and reconstruction (reassembly) with both legacy and streaming modes.
- **Transport:** Logical layer. Prepares data payloads for DI-2 (MQTT-SN).
- **Blocking model:** 
  - **Legacy mode** (`deconstruct()`/`reconstruct()`): Blocking, synchronous functions that perform all disk I/O at once.
  - **Streaming mode** (`init_streaming_read()`/`read_chunk_streaming()`): Blocking per-chunk, but allows incremental processing.
- **Timing:** Highly variable, dependent on file size and DI-3 (SD Card) speed.
- **Units:** bytes, CRC16.

### Dependencies
- **MCU libraries:** C standard libraries (`stdio.h`, `stdlib.h`, `string.h`).
- **Peripheral interfaces:** DI-3 (MicroSD Driver) for all file read/write operations.
- **OS services:** None.

### Resources & Constraints

#### Memory Usage

**Legacy Mode (`deconstruct()`):**
- Dynamically allocates memory for an array of `Payload` structs.
- **Memory consumption:** `file_size + (chunk_count × sizeof(Payload))`
- **Example:** 50KB file = ~50KB + (211 chunks × 256 bytes) ≈ **104KB RAM**
- **⚠️ NOT RECOMMENDED** for files > 32KB on Pico W (264KB total RAM).

**Streaming Mode (`init_streaming_read()` + `read_chunk_streaming()`):**
- Progressive buffer detection: tries 4KB → 8KB → 16KB → 32KB → 64KB → 128KB
- Only allocates memory for **single chunk at a time** (~0.5KB)
- **Memory consumption:** `~512 bytes` (constant, regardless of file size)
- **Example:** 50KB file = **0.5KB RAM** (99% reduction!)
- **✅ RECOMMENDED** for all file sizes, especially > 5KB.

**Reconstruction Mode (`reconstruct()`):**
- Allocates buffer for complete file reconstruction
- Writes to SD card in single operation after verification
- **Memory consumption:** `file_size + (chunk_count × sizeof(Payload))`
- Suitable for receiver side where chunks arrive incrementally via MQTT-SN

### Initialisation & Configuration

#### Sender Side (File Transmission)

**Option 1: Legacy Mode (⚠️ Limited to small files)**
```c
struct Metadata meta;
struct Payload* chunks;

// Deconstruct entire file into RAM
int result = deconstruct("filename.txt", &meta, &chunks);
if (result != 0) {
    // Handle error
}

// Send chunks via MQTT-SN...
// [transmit metadata + all chunks]

free(chunks); // Clean up
```

**Option 2: Streaming Mode (✅ Recommended)**
```c
struct Metadata meta;

// Initialize streaming read
int result = init_streaming_read("filename.txt", &meta);
if (result != 0) {
    // Handle error
}

// Stream chunks one at a time
for (uint32_t i = 0; i < meta.chunk_count; i++) {
    struct Payload chunk;
    result = read_chunk_streaming(i, &chunk);
    if (result != 0) {
        // Handle error
    }
    
    // Send this chunk via MQTT-SN...
    // [transmit single chunk]
}

cleanup_streaming_read(); // Clean up
```

#### Receiver Side (File Reception)

```c
struct Metadata meta;
struct Payload* chunks;

// Receive metadata via MQTT-SN
// Receive all chunks via MQTT-SN...

// Reconstruct file from chunks
int result = reconstruct(&meta, &chunks, "output.txt");
if (result != 0) {
    // Handle error
}

free(chunks); // Clean up
```

### Error Handling

**Return codes:**
- `deconstruct()` / `reconstruct()` / `init_streaming_read()` / `read_chunk_streaming()`: 
  - `0` on success
  - `-1` on error (file not found, read error, memory allocation failure, CRC mismatch)
- `verify_chunk()`: 
  - `true` if CRC matches
  - `false` on CRC mismatch

**Error types:**
- **File not found:** File doesn't exist on SD card
- **Memory allocation failure:** Insufficient RAM (legacy mode only)
- **CRC mismatch:** Data corruption detected
- **Read error:** SD card communication failure
- **Hard limit reached:** File size exceeds 128KB initial read buffer (streaming mode)

**Timeouts:**
- None at this layer
- Relies on DI-2 (MQTT-SN QoS) for transport reliability and timeouts

### API Functions

#### Legacy API (Backward Compatible)

```c
// Deconstruct file into chunks (loads entire file into RAM)
int deconstruct(char* filename, struct Metadata* meta, struct Payload** chunks);

// Reconstruct file from chunks
int reconstruct(struct Metadata* meta, struct Payload** chunks, char* output_filename);

// Verify chunk CRC16
bool verify_chunk(struct Payload* payload);
```

#### Streaming API (Recommended)

```c
// Initialize streaming read session
int init_streaming_read(char* filename, struct Metadata* meta);

// Read a specific chunk by index
int read_chunk_streaming(uint32_t chunk_index, struct Payload* chunk);

// Clean up streaming session
void cleanup_streaming_read(void);

// Get current streaming session state
streaming_state_t get_streaming_state(void);
```

#### Serialization API

```c
// Serialize metadata to 247-byte buffer
int serialize_metadata(struct Metadata* meta, uint8_t* buffer);

// Deserialize metadata from 247-byte buffer
int deserialize_metadata(uint8_t* buffer, struct Metadata* meta);

// Serialize payload to 247-byte buffer
int serialize_payload(struct Payload* payload, uint8_t* buffer);

// Deserialize payload from 247-byte buffer
int deserialize_payload(uint8_t* buffer, struct Payload* payload);
```

### Test Notes

**Bench test:**
- Run `streaming_test.c` for comprehensive testing of both modes
- Run `data_frame_demo.c` for legacy mode testing
- Run `serialize_test.c` for serialization testing

**Test coverage:**
1. **UT-1:** Small file streaming (500 bytes) - verify memory efficiency
2. **UT-2:** CRC16 calculation with known test vectors
3. **UT-3:** Medium file streaming (5KB) - verify correctness
4. **UT-4:** Large file streaming (50KB) - verify scalability
5. **UT-5:** End-to-end reconstruction test - binary comparison
6. **UT-6:** Random access chunk reading - verify seek functionality
7. **UT-7:** Serialization round-trip - verify data integrity

**Known limitations:**
- **Legacy `deconstruct()`:** Memory allocation strategy is not scalable for large files on constrained devices (reads entire file into chunks in RAM). **Deprecated for files > 32KB.**
- **Streaming mode:** Initial file read uses progressive buffer detection up to 128KB. Files that cannot be read in 128KB chunks will fail with `HARD_LIMIT_REACHED` error.
- **Reconstruction:** Not restricted by memory limitations due to chunk-by-chunk file writing strategy.
- **File size:** Theoretical maximum file size limited only by SD card capacity and transfer time.

### Performance Characteristics

| Operation               | Legacy Mode            | Streaming Mode        |
| ----------------------- | ---------------------- | --------------------- |
| **Memory (500B file)**  | ~8 KB                  | 0.5 KB                |
| **Memory (5KB file)**   | ~8 KB                  | 0.5 KB                |
| **Memory (50KB file)**  | ~104 KB (❌ Too large!) | 0.5 KB                |
| **Max file size**       | 32 KB (hardcoded)      | Unlimited             |
| **Initialization time** | Fast (single read)     | Fast (metadata scan)  |
| **Per-chunk access**    | Instant (in RAM)       | ~1-5ms (SD card seek) |
| **Random access**       | ✅ Supported (O(1))     | ✅ Supported (O(1))    |
| **Sequential access**   | ✅ Optimal              | ✅ Optimal             |

---

## 6.5. (DI-5) Chunk Transfer Manager Interface (fs/chunk_transfer.h)

### Overview
- **Module name:** `chunk_transfer_manager`
- **Purpose:** High-level abstraction for managing chunk-based file transfers with session tracking, progress monitoring, and coordination with MQTT-SN QoS 1 transport.
- **Transport:** Interfaces with DI-4 (File Transfer Protocol) and DI-3 (MicroSD Driver).
- **Blocking model:** Blocking per-operation (init, write, finalize).
- **Timing:** Variable per operation (1-50ms typical per chunk write).
- **Units:** chunks, bytes, session IDs.

### Dependencies
- **MCU libraries:** C standard libraries (`stdio.h`, `stdlib.h`, `string.h`).
- **Peripheral interfaces:** 
  - DI-3 (MicroSD Driver) for chunk write operations
  - DI-4 (File Transfer Protocol) for metadata structures
- **OS services:** None.

### Resources & Constraints

#### Memory Usage
- **Session structure:** `sizeof(transfer_session_t)` ≈ 512 bytes
- **Chunk bitmap:** `(total_chunks / 8) + 1` bytes
  - 100 chunks = 13 bytes
  - 1000 chunks = 125 bytes
  - 10000 chunks = 1250 bytes
- **Per-chunk buffer:** 247 bytes (transient, freed after write)
- **Total typical:** < 1 KB per active session

#### Limitations
- **Concurrent sessions:** One active session at a time (single static context)
- **Chunk size:** Fixed at 237 bytes data + 10 bytes metadata = 247 bytes total
- **Chunk ordering:** Out-of-order delivery supported (QoS 1 may reorder packets)
- **Duplicate detection:** Automatic via chunk bitmap

### Initialisation & Configuration

#### Required Call Order (Receiver Side)

```c
filesystem_info_t fs_info;
transfer_session_t session;
struct Metadata metadata;

// 1. Initialize filesystem (once at startup)
microsd_init_filesystem(&fs_info);

// 2. Receive metadata via MQTT-SN (Topic ID 3)
// [metadata received and deserialized]

// 3. Initialize transfer session
bool success = chunk_transfer_init_session(&fs_info, &metadata, &session);
if (!success) {
    // Handle error
}

// 4. Receive and write chunks (Topic ID 4)
for (each received chunk) {
    struct Payload payload;
    // [payload received and deserialized]
    
    success = chunk_transfer_write_payload(&fs_info, &session, &payload);
    if (!success) {
        // Handle error (duplicate is not an error)
    }
    
    // Check progress
    uint32_t received, total;
    chunk_transfer_get_progress(&session, &received, &total);
    printf("Progress: %u/%u chunks\n", received, total);
}

// 5. Check completion
if (chunk_transfer_is_complete(&session)) {
    // All chunks received
}

// 6. Finalize transfer (writes file to SD card)
success = chunk_transfer_finalize(&fs_info, &session);
if (!success) {
    // Handle error
}
```

### Error Handling

**Return codes:**
- `chunk_transfer_init_session()`: `true` on success, `false` on error
- `chunk_transfer_write_payload()`: `true` on success, `false` on error (duplicate returns `true`)
- `chunk_transfer_finalize()`: `true` on success, `false` on error
- `chunk_transfer_is_complete()`: `true` if complete, `false` if pending chunks

**Error types:**
- **NULL parameter:** Invalid function arguments
- **Session not active:** Operation called on inactive session
- **Invalid sequence number:** Chunk sequence out of valid range
- **File system error:** SD card write failure
- **Incomplete transfer:** Attempted finalize with missing chunks
- **CRC mismatch:** Detected during final verification

**Duplicate handling:**
- **QoS 1 behavior:** May deliver chunks more than once (at-least-once semantics)
- **Detection:** Chunk bitmap tracks received chunks by sequence number
- **Action:** Duplicate chunks are silently skipped (returns success without writing)
- **Result:** No duplicate data written to SD card, no errors reported

### API Functions

#### Session Management

```c
// Initialize new transfer session from metadata
bool chunk_transfer_init_session(
    filesystem_info_t* fs_info,
    const struct Metadata* metadata,
    transfer_session_t* session
);

// Finalize transfer (complete file write)
bool chunk_transfer_finalize(
    filesystem_info_t* fs_info,
    transfer_session_t* session
);
```

#### Chunk Operations

```c
// Write a payload chunk to active session
bool chunk_transfer_write_payload(
    filesystem_info_t* fs_info,
    transfer_session_t* session,
    const struct Payload* payload
);

// Check if all chunks received
bool chunk_transfer_is_complete(
    const transfer_session_t* session
);
```

#### Progress Monitoring

```c
// Get transfer progress
void chunk_transfer_get_progress(
    const transfer_session_t* session,
    uint32_t* chunks_received,
    uint32_t* total_chunks
);

// Print session information for debugging
void chunk_transfer_print_session_info(
    const transfer_session_t* session
);
```

### Session Structure

```c
typedef struct {
    char session_id[SESSION_ID_SIZE];       // Unique session identifier
    char filename[METADATA_FILENAME_SIZE];  // Target filename
    uint32_t total_chunks;                  // Total chunks (including metadata)
    uint32_t chunk_size;                    // Size of each data chunk (237 bytes)
    bool active;                            // Session active flag
    chunk_metadata_t chunk_meta;            // MicroSD driver chunk metadata
    struct Metadata metadata;               // File metadata
} transfer_session_t;
```

### Integration with MQTT-SN QoS 1

#### Message Flow

**Metadata (Topic ID 3, QoS 1):**
```
Sender → PUBLISH(metadata) → Gateway → PUBLISH → Receiver
Sender ← PUBACK            ← Gateway ← PUBACK   ← Receiver
```
- Receiver calls `chunk_transfer_init_session()` upon receiving metadata

**Data Chunks (Topic ID 4, QoS 1):**
```
Sender → PUBLISH(chunk_N) → Gateway → PUBLISH → Receiver
Sender ← PUBACK           ← Gateway ← PUBACK   ← Receiver
```
- Receiver calls `chunk_transfer_write_payload()` for each chunk
- QoS 1 may cause retransmissions (duplicates automatically handled)

**Completion:**
```
Receiver checks: chunk_transfer_is_complete()
              ↓ true
Receiver calls: chunk_transfer_finalize()
              ↓ success
File written to SD card with CRC16 verification
```

### Test Notes

**Bench test:**
- Run `file_transfer_demo.c` for end-to-end transfer testing
- Use `streaming_test.c` reconstruction tests for validation

**Test coverage:**
1. **IT-1:** Session initialization with valid metadata
2. **IT-2:** Sequential chunk writing (in order)
3. **IT-3:** Out-of-order chunk writing (random order)
4. **IT-4:** Duplicate chunk detection and handling
5. **IT-5:** Progress monitoring accuracy
6. **IT-6:** Completion detection (all chunks received)
7. **IT-7:** Finalization with CRC16 verification
8. **IT-8:** Error recovery (missing chunks detected)
9. **IT-9:** Large file transfer (50KB+, 200+ chunks)
10. **IT-10:** Concurrent retransmission handling

**Known limitations:**
- **Single session:** Only one transfer session active at a time. Multiple concurrent transfers require additional session management.
- **No timeout handling:** This layer does not implement timeouts; relies on MQTT-SN QoS 1 layer for retransmission logic.
- **Memory overhead:** Chunk bitmap grows with file size (1 bit per chunk).
- **No resume capability:** If transfer is interrupted, must restart from beginning.

### Performance Characteristics

| Metric               | Typical Value | Notes                          |
| -------------------- | ------------- | ------------------------------ |
| **Session init**     | < 50ms        | Creates file, allocates bitmap |
| **Chunk write**      | 1-5ms         | Per 237-byte chunk             |
| **Duplicate check**  | < 0.1ms       | Bitmap lookup O(1)             |
| **Completion check** | < 1ms         | Bitmap scan O(n/8)             |
| **Finalization**     | 10-100ms      | Verifies CRC, finalizes file   |
| **Memory per chunk** | 0.125 bits    | Bitmap overhead                |
| **Total overhead**   | < 1KB         | Session + bitmap               |

### QoS 1 Reliability Features

**Guaranteed Delivery:**
- MQTT-SN QoS 1 ensures each chunk is acknowledged
- Unacknowledged chunks are retransmitted by sender
- Network packet loss handled automatically

**Duplicate Tolerance:**
- QoS 1 "at-least-once" semantics may deliver chunks multiple times
- Chunk bitmap prevents duplicate writes
- No data corruption from retransmissions

**Out-of-Order Support:**
- Chunks may arrive in any order
- Bitmap tracks which chunks received
- Completion requires all chunks, regardless of order

**Error Recovery:**
- Missing chunks detected before finalization
- CRC16 verification ensures data integrity
- Failed transfers can be retried from scratch

### Example Usage Scenarios

**Scenario 1: 5KB File (22 chunks)**
```
Init session: 30ms
Write 22 chunks: 22 × 2ms = 44ms
Check complete: 1ms
Finalize: 20ms
Total: ~95ms
Memory: ~512 bytes session + 3 bytes bitmap = 515 bytes
```

**Scenario 2: 50KB File (211 chunks)**
```
Init session: 40ms
Write 211 chunks: 211 × 3ms = 633ms
Check complete: 2ms
Finalize: 60ms
Total: ~735ms
Memory: ~512 bytes session + 27 bytes bitmap = 539 bytes
```

**Scenario 3: 500KB File (2111 chunks) with 5% duplicates**
```
Init session: 50ms
Write 2111 chunks: 2111 × 3ms = 6333ms
Write 106 duplicates: 106 × 0.1ms = 11ms (skipped)
Check complete: 10ms
Finalize: 200ms
Total: ~6604ms (6.6 seconds)
Memory: ~512 bytes session + 264 bytes bitmap = 776 bytes
```

---

## Summary Comparison

| Feature                | DI-4 (data_frame.h)            | DI-5 (chunk_transfer.h)           |
| ---------------------- | ------------------------------ | --------------------------------- |
| **Purpose**            | File chunking & reconstruction | Transfer session management       |
| **Level**              | Low-level file operations      | High-level transfer orchestration |
| **Memory**             | 0.5KB (streaming)              | < 1KB (session + bitmap)          |
| **File size limit**    | Unlimited (streaming)          | Unlimited                         |
| **Duplicate handling** | CRC verification               | Bitmap-based detection            |
| **Use case**           | Sender: chunk files            | Receiver: manage transfers        |
| **MQTT integration**   | Prepares payloads              | Coordinates with QoS 1            |
| **State management**   | Stateless (per-call)           | Stateful (session-based)          |
