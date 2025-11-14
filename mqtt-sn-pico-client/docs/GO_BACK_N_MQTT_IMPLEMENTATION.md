# Go-Back-N Protocol Implementation for MQTT-SN File Transfer

## Overview

This document describes the Go-Back-N sliding window protocol implementation for MQTT-SN file transfers on the Raspberry Pi Pico W.

## Protocol Design

### Automatic Transfer Method Selection

The implementation includes an intelligent auto-selection feature that chooses the optimal transfer method based on file size:

**Decision Logic**:
```
File Size ≤ 4KB  →  Normal Streaming Transfer
File Size > 4KB  →  Go-Back-N Sliding Window Protocol
```

**Rationale**:
- **Small files (≤4KB)**: Windowing overhead not justified, simple streaming is faster
- **Large files (>4KB)**: Significant benefits from:
  - 99% reduction in SD card writes (windowed writes vs per-chunk)
  - Flow control prevents sender overwhelming receiver
  - Automatic retransmission on packet loss
  - Memory-efficient constant usage regardless of file size

**Function**: `send_file_via_mqtt_auto()` automatically determines and applies the best method.

### Topics

The implementation uses three MQTT-SN topics:

1. **`file/data` (Topic ID: 4)** - Unidirectional sender → receiver
   - Carries metadata (chunk 0) with QoS 2
   - Carries data chunks (1...n) with QoS 1
   
2. **`file/control` (Topic ID: 5)** - Bidirectional flow control
   - Receiver → Sender: ACK, NACK, REQUEST_NEXT, COMPLETE
   - QoS 1 for all control messages

### Window Configuration

- **Window Size**: 32 KB (138 chunks × 237 bytes)
- **Optimized for**: Pico W with 264 KB RAM
- **Max Retries**: 3 attempts per window

## Architecture

### Sender (TX) Side

```
┌─────────────────────────────────────────────────────────┐
│ TX Pico (Sender)                                        │
├─────────────────────────────────────────────────────────┤
│ 1. Register topic: file/control                         │
│ 2. Subscribe to: file/control (for ACK/NACK/REQUEST)   │
│ 3. Call: send_file_via_mqtt_gbn()                      │
│                                                         │
│ Flow:                                                   │
│   - Initialize sliding_window_t (tx_window)            │
│   - Send metadata to file/data (QoS 2)                 │
│   - Wait for metadata confirmation                     │
│   - FOR each window:                                   │
│       • Transmit chunks [base...base+window_size]      │
│       • Wait for control message on file/control       │
│       • Process ACK/NACK/REQUEST_NEXT                  │
│       • Slide window or retransmit                     │
│   - Cleanup on COMPLETE                                │
└─────────────────────────────────────────────────────────┘
```

### Receiver (RX) Side

```
┌─────────────────────────────────────────────────────────┐
│ RX Pico (Receiver)                                      │
├─────────────────────────────────────────────────────────┤
│ 1. Subscribe to: file/data (QoS 2)                     │
│ 2. Register topic: file/control                         │
│ 3. Handlers:                                            │
│   - handle_file_metadata() - Initialize session        │
│   - handle_file_payload() - Receive chunks             │
│                                                         │
│ Flow:                                                   │
│   - Receive metadata from file/data                    │
│   - Initialize transfer_session_t                      │
│   - FOR each incoming chunk:                           │
│       • Write to temp file (buffered)                  │
│       • Track in bitmap                                │
│       • Check window completion                        │
│       • IF window complete:                            │
│           - Sync to SD card (chunk_transfer_sync_window)│
│           - Send REQUEST_NEXT to file/control          │
│       • IF missing chunks:                             │
│           - Send NACK to file/control                  │
│       • IF all chunks received:                        │
│           - Finalize transfer                          │
│           - Send COMPLETE to file/control              │
└─────────────────────────────────────────────────────────┘
```

## Key Data Structures

### Control Message (`control_message_t`)

```c
typedef struct __attribute__((packed)) {
    control_msg_type_t type;    // CTRL_ACK, CTRL_NACK, CTRL_REQUEST_NEXT, CTRL_COMPLETE
    uint32_t seq_num;           // Sequence number
    uint32_t window_start;      // Start of requested window
    uint32_t window_end;        // End of requested window
    char session_id[32];        // Session identifier
} control_message_t;
```

### Sliding Window State (`sliding_window_t`)

```c
typedef struct {
    uint32_t base;              // Base of sliding window (oldest unACKed)
    uint32_t next_seq;          // Next sequence to send
    uint32_t window_size;       // Window size in chunks (~138)
    uint32_t total_chunks;      // Total chunks in file
    bool* acked;                // ACK bitmap for current window
    uint32_t retries;           // Retry counter
    absolute_time_t last_send_time;
    char session_id[32];        // Session ID
    bool active;                // Window active flag
} sliding_window_t;
```

### MQTT Context Extension

```c
typedef struct {
    // Existing fields...
    transfer_session_t *file_session;
    bool transfer_in_progress;
    
    // Go-Back-N additions:
    sliding_window_t tx_window;     // Sender-side sliding window
    uint32_t last_acked_seq;        // Last ACKed sequence (receiver)
    char rx_session_id[32];         // Receiver session ID
    
    // UDP connection parameters (for sending control messages):
    struct udp_pcb* pcb;            // UDP PCB pointer
    ip_addr_t gw_addr;              // Gateway address
    u16_t gw_port;                  // Gateway port
} mqtt_sn_context_t;
```

## API Functions

### Sender Functions

#### `send_file_via_mqtt_auto()`

```c
void send_file_via_mqtt_auto(struct udp_pcb *pcb, const ip_addr_t *gw_addr, 
                             u16_t gw_port, const char *filename, 
                             mqtt_sn_context_t *ctx);
```

**Purpose**: Automatically select transfer method based on file size.

**Behavior**:
- Files **≤ 4KB**: Uses normal streaming transfer (`send_file_via_mqtt`)
- Files **> 4KB**: Uses Go-Back-N protocol (`send_file_via_mqtt_gbn`)

**Threshold**: 4KB chosen because:
- Small files don't benefit from windowing overhead
- Files > 4KB see significant SD write reduction with windowing
- One window (32KB) handles most medium-sized files efficiently

**Prerequisites**:
- For Go-Back-N transfers (>4KB), sender must have registered and subscribed to `file/control` topic
- Context parameter can be NULL for small files (≤4KB), but required for large files

#### `send_file_via_mqtt_gbn()`

```c
void send_file_via_mqtt_gbn(struct udp_pcb *pcb, const ip_addr_t *gw_addr, 
                            u16_t gw_port, const char *filename, 
                            mqtt_sn_context_t *ctx);
```

**Purpose**: Send file using Go-Back-N protocol with sliding windows (for files > 4KB).

**Prerequisites**:
- Sender must have registered and subscribed to `file/control` topic
- Topic ID must be obtained via REGACK

**Flow**:
1. Initialize streaming read
2. Send metadata (QoS 2) to `file/data`
3. Wait for metadata confirmation
4. Initialize sliding window
5. For each window:
   - Transmit all chunks in window
   - Wait for control message
   - Process ACK/NACK/REQUEST_NEXT
   - Slide window or retransmit

### Receiver Functions

#### `handle_file_metadata()`

```c
void handle_file_metadata(mqtt_sn_context_t *ctx, const uint8_t *payload, 
                          size_t len, struct udp_pcb *pcb, 
                          const ip_addr_t *addr, u16_t port);
```

**Purpose**: Initialize transfer session from received metadata.

**Actions**:
- Validate metadata
- Initialize `transfer_session_t`
- Create temp file
- Allocate bitmap
- Set `transfer_in_progress = true`
- Store session ID in `rx_session_id`

#### `handle_file_payload()`

```c
void handle_file_payload(mqtt_sn_context_t *ctx, const uint8_t *payload, size_t len,
                         struct udp_pcb *pcb, const ip_addr_t *addr, u16_t port);
```

**Purpose**: Receive and buffer chunks, manage windowed writes.

**Flow**:
1. Deserialize and verify chunk
2. Write to temp file (buffered)
3. Update bitmap
4. Check window completion:
   - **If window complete**: 
     - Sync to SD card (`chunk_transfer_sync_window()`)
     - Send `REQUEST_NEXT` via `send_control_message()`
   - **If missing chunks**: 
     - Send `NACK` via `send_control_message()`
   - **If all chunks received**: 
     - Finalize transfer
     - Send `COMPLETE` via `send_control_message()`

#### `handle_control_message()`

```c
void handle_control_message(mqtt_sn_context_t *ctx, const uint8_t *payload, 
                            size_t len, struct udp_pcb *pcb, 
                            const ip_addr_t *addr, u16_t port);
```

**Purpose**: Process flow control messages (sender side).

**Handles**:
- `CTRL_ACK`: Mark chunks ACKed, slide window
- `CTRL_NACK`: Retransmit from specified sequence
- `CTRL_REQUEST_NEXT`: Slide window to next batch
- `CTRL_COMPLETE`: Mark transfer complete

### Helper Functions

#### `init_sliding_window()`

```c
bool init_sliding_window(sliding_window_t* window, uint32_t total_chunks, 
                         const char* session_id);
```

Allocates ACK bitmap, initializes window state.

#### `cleanup_sliding_window()`

```c
void cleanup_sliding_window(sliding_window_t* window);
```

Frees ACK bitmap, deactivates window.

#### `send_control_message()`

```c
void send_control_message(struct udp_pcb *pcb, const ip_addr_t *gw_addr, 
                          u16_t gw_port, const control_message_t *ctrl_msg);
```

Publishes control message to `file/control` topic (QoS 1).

## Window-Based Writing Strategy

### Receiver Write Flow

```
┌────────────────────────────────────────────────────┐
│ Window [1-138] Received                            │
├────────────────────────────────────────────────────┤
│ 1. Chunks buffered in memory (chunk_transfer)     │
│ 2. Bitmap tracks received chunks                  │
│ 3. When window complete:                          │
│    → chunk_transfer_sync_window()                 │
│    → f_sync() writes to SD card                   │
│    → Send REQUEST_NEXT [139-276]                  │
│                                                    │
│ Benefit: ~4,364 writes → ~32 writes (1MB file)    │
└────────────────────────────────────────────────────┘
```

### Missing Chunk Detection

```c
// Check if all chunks in window received
for (uint32_t i = window_start; i <= window_end; i++) {
    uint32_t bitmap_index = i / 8;
    uint32_t bit_offset = i % 8;
    
    if (!(chunk_bitmap[bitmap_index] & (1 << bit_offset))) {
        // Missing chunk detected → Send NACK
        send_nack(i - 1);  // Last successful before gap
        break;
    }
}
```

## Control Message Flow

### Successful Window Transfer

```
Sender                          Receiver
  │                                │
  ├──── Window [1-138] ────────────>│
  │                                │ (Receives all 138 chunks)
  │                                │ (Writes to SD card)
  │                                │
  │<──── REQUEST_NEXT [139-276] ───┤
  │                                │
  ├──── Window [139-276] ──────────>│
  │                                │
  └────────────────────────────────┘
```

### Missing Chunks (NACK)

```
Sender                          Receiver
  │                                │
  ├──── Chunks [1-100] ────────────>│
  │     (Chunk 50 lost)            │
  │                                │ (Detects missing chunk 50)
  │<──── NACK (seq=49) ────────────┤
  │                                │
  ├──── Retransmit [50-100] ──────>│ (Go-Back-N)
  │                                │
  └────────────────────────────────┘
```

### Transfer Complete

```
Sender                          Receiver
  │                                │
  ├──── Last Window ──────────────>│
  │                                │ (All chunks received)
  │                                │ (Finalizes transfer)
  │                                │
  │<──── COMPLETE ─────────────────┤
  │                                │
  └────────────────────────────────┘
```

## Memory Optimization

### Sender (TX)

- **Streaming buffer**: 32 KB (read buffer)
- **Single chunk**: ~250 bytes
- **ACK bitmap**: 138 bits → 18 bytes per window
- **Total**: ~33 KB constant (file size independent)

### Receiver (RX)

- **Chunk bitmap**: `total_chunks / 8` bytes
- **Temp file handle**: FIL structure
- **Buffered writes**: Window-based (synced after each window)
- **Example (1MB file)**: ~550 bytes bitmap + FIL structure

## Usage Example

### Sender Setup (TX) - Recommended Method

```c
// TX Pico main loop
mqtt_sn_context_t tx_ctx = {0};

// 1. Register file/control topic (needed for Go-Back-N)
mqtt_sn_add_topic_for_registration(&tx_ctx, "file/control");

// 2. Wait for REGACK, then subscribe to receive flow control messages
mqtt_sn_add_topic_for_subscription(&tx_ctx, "file/control", QOS_LEVEL_1);
mqtt_sn_process_topic_registrations(&tx_ctx, pcb, &gateway_addr, gw_port);

// 3. Send file with automatic method selection (RECOMMENDED)
//    Automatically chooses Go-Back-N for files > 4KB, normal for ≤ 4KB
send_file_via_mqtt_auto(pcb, gw_addr, gw_port, "test.jpg", &tx_ctx);
```

### Sender Setup (TX) - Manual Method Selection

```c
// TX Pico main loop - if you want to force a specific method
mqtt_sn_context_t tx_ctx = {0};

// For Go-Back-N (large files):
// 1. Register and subscribe to file/control
mqtt_sn_add_topic_for_registration(&tx_ctx, "file/control");
mqtt_sn_add_topic_for_subscription(&tx_ctx, "file/control", QOS_LEVEL_1);
mqtt_sn_process_topic_registrations(&tx_ctx, pcb, &gateway_addr, gw_port);

// 2. Send using Go-Back-N explicitly
send_file_via_mqtt_gbn(pcb, gw_addr, gw_port, "large_file.jpg", &tx_ctx);

// For Normal Transfer (small files):
// No topic registration needed, just send
send_file_via_mqtt(pcb, gw_addr, gw_port, "small_file.txt");
```

### Receiver Setup (RX)

```c
// RX Pico main loop
mqtt_sn_context_t rx_ctx = {0};
transfer_session_t session = {0};
rx_ctx.file_session = &session;

// 1. Subscribe to file/data (receives metadata and chunks)
mqtt_sn_add_topic_for_subscription(&rx_ctx, "file/data", QOS_LEVEL_2);

// 2. Register file/control (for sending ACK/NACK)
mqtt_sn_add_topic_for_registration(&rx_ctx, "file/control");

// 3. Callbacks handle automatically:
//    - handle_file_metadata() on metadata
//    - handle_file_payload() on chunks
//    - Sends control messages automatically
```

## Performance Characteristics

### Throughput

- **Chunk size**: 237 bytes
- **Window size**: 32 KB (~138 chunks)
- **Network polling**: After each SD read, during delays
- **Expected**: ~20-50 KB/s (depends on network conditions)

### Retransmissions

- **Packet loss**: Handled by Go-Back-N
- **Example**: 5% packet loss → ~5-10% retransmissions
- **Max retries**: 3 per window before abort

### SD Card Writes

- **Without windowing**: ~4,364 writes (1MB file, 237-byte chunks)
- **With windowing**: ~32 writes (32KB windows)
- **Improvement**: **~99% reduction** in write operations

## Error Handling

### Sender Errors

1. **Metadata not confirmed**: Timeout after 5 seconds → abort
2. **Max retries exceeded**: After 3 failed windows → abort
3. **No control message**: Retry current window

### Receiver Errors

1. **Missing chunks**: Send NACK with first missing sequence
2. **CRC failure**: Drop chunk, wait for retransmission
3. **SD card full**: Abort transfer, notify sender
4. **Invalid metadata**: Reject transfer, notify sender

## Testing

### Simulation Test

See `fs/tests/streaming_test.c`:
- Simulates packet loss (5%)
- Validates Go-Back-N retransmission
- Tests window-based writes
- Verifies file integrity

### Integration Test

1. Flash TX firmware to Pico 1
2. Flash RX firmware to Pico 2
3. Connect both to MQTT-SN gateway
4. TX: Call `send_file_via_mqtt_gbn()`
5. RX: Receives automatically via callbacks
6. Verify file integrity on RX SD card

**Note**: The receiver automatically sends control messages (ACK/NACK/REQUEST_NEXT/COMPLETE) when handling incoming chunks. The UDP parameters are passed directly to `handle_file_payload()` from the receive callback.

## Limitations

1. **Single transfer at a time**: Only one active transfer per pico
2. **No resume**: Aborted transfers must restart from beginning
3. **Fixed window size**: 32KB hardcoded (can be changed via `WINDOW_SIZE_BYTES`)
4. **No compression**: Files sent as-is

## Future Enhancements

1. **Selective Repeat**: More efficient than Go-Back-N for high packet loss
2. **Dynamic window sizing**: Adapt to network conditions
3. **Resume capability**: Continue interrupted transfers
4. **Multi-file transfers**: Queue multiple files
5. **Compression**: Reduce bandwidth usage

## References

- MQTT-SN v1.2 Specification
- Go-Back-N ARQ Protocol
- FatFS R0.15 Documentation
- Pico W SDK Documentation
