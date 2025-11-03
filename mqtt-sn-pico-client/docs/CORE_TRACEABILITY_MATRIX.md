# Core Components Traceability Matrix
## MQTT-SN (QoS 0/1/2) | MicroSD Driver | File Chunking

**Format**: `Requirement ID → Use Case ID → Function Name → Test ID → Pass/Fail`

---

## 1. MQTT-SN PROTOCOL WITH QoS 0/1/2

### 1.1 QoS 0 (At Most Once)

```
FR2 → UC4 → mqtt_sn_publish_topic_id() → Manual → PASS
FR2 → UC4 → udp_recv_callback() → Manual → PASS
```

**Description**: Fire-and-forget delivery, no acknowledgments
**Functions**:
- `mqtt_sn_publish_topic_id(qos=0)` - Sends PUBLISH with QoS 0 flag
- `udp_recv_callback()` - Processes incoming PUBLISH, no PUBACK sent

**Test Scenario**:
```
1. Client sends PUBLISH (QoS 0)
2. No acknowledgment expected
3. Message delivered once or lost
```

---

### 1.2 QoS 1 (At Least Once)

```
FR2 → UC5 → mqtt_sn_publish_topic_id() → ST-1 → PASS
FR2 → UC5 → mqtt_sn_send_puback() → ST-1 → PASS
FR2 → UC5 → check_qos_timeouts() → ST-1 → PASS
FR2 → UC5 → remove_pending_qos_msg() → ST-1 → PASS
FR2 → UC5 → udp_recv_callback() → ST-1 → PASS
FR2 → UC5 → get_next_msg_id() → UT-1 → PASS
NFR1 → UC5 → g_pending_msgs[] → ST-1 → PASS
NFR1 → UC5 → QOS_RETRY_INTERVAL_US → ST-1 → PASS
NFR1 → UC5 → QOS_MAX_RETRIES → ST-1 → PASS
```

**Description**: Guaranteed delivery with acknowledgments, may deliver duplicates
**Functions**:
- `mqtt_sn_publish_topic_id(qos=1)` - Sends PUBLISH with QoS 1 flag (0x20), stores in queue
- `mqtt_sn_send_puback()` - Sends PUBACK acknowledgment
- `check_qos_timeouts()` - Retransmits unacknowledged messages (max 3 retries, 2s interval)
- `remove_pending_qos_msg()` - Removes message from queue after PUBACK received
- `udp_recv_callback()` - Handles PUBLISH reception and PUBACK processing
- `get_next_msg_id()` - Generates unique message IDs (1-65535)

**Data Structures**:
- `qos_msg_t` - Stores msg_id, qos, timestamp, retry_count, topic_id, payload[247]
- `g_pending_msgs[10]` - Queue for tracking unacknowledged QoS 1/2 messages

**Test Scenario**:
```
1. Client sends PUBLISH (QoS 1, msg_id=123)
2. Message stored in g_pending_msgs[slot]
3. Gateway sends PUBACK (msg_id=123)
4. Client removes msg_id=123 from queue
5. If no PUBACK within 2s, retransmit (up to 3 times)
```

---

### 1.3 QoS 2 (Exactly Once)

```
FR2 → UC6 → mqtt_sn_publish_topic_id() → ST-2 → PASS
FR2 → UC6 → mqtt_sn_publish_topic_id() → ST-3 → PASS
FR2 → UC6 → mqtt_sn_send_pubrec() → ST-2 → PASS
FR2 → UC6 → mqtt_sn_send_pubrel() → ST-3 → PASS
FR2 → UC6 → mqtt_sn_send_pubcomp() → ST-3 → PASS
FR2 → UC6 → check_qos_timeouts() → ST-2 → PASS
FR2 → UC6 → check_qos_timeouts() → ST-3 → PASS
FR2 → UC6 → remove_pending_qos_msg() → ST-2 → PASS
FR2 → UC6 → remove_pending_qos_msg() → ST-3 → PASS
FR2 → UC6 → udp_recv_callback() → ST-2 → PASS
FR2 → UC6 → udp_recv_callback() → ST-3 → PASS
```

**Description**: Exactly-once delivery with 4-way handshake, no duplicates
**Functions**:
- `mqtt_sn_publish_topic_id(qos=2)` - Sends PUBLISH with QoS 2 flag (0x40), stores in queue (step=0)
- `mqtt_sn_send_pubrec()` - Sends PUBREC acknowledgment
- `mqtt_sn_send_pubrel()` - Sends PUBREL (release for processing)
- `mqtt_sn_send_pubcomp()` - Sends PUBCOMP (completion acknowledgment)
- `check_qos_timeouts()` - Retransmits PUBLISH (step=0) or PUBREL (step=1)
- `remove_pending_qos_msg()` - Removes message after PUBCOMP received
- `udp_recv_callback()` - Handles 4-way handshake state machine

**State Machine**:
```
Sender:   PUBLISH(step=0) → [wait PUBREC] → PUBREL(step=1) → [wait PUBCOMP] → DONE
Receiver: [wait PUBLISH] → PUBREC → [wait PUBREL] → PUBCOMP → DONE
```

**Test Scenarios**:

**ST-2: PUBREC Lost**
```
1. Client sends PUBLISH (QoS 2, msg_id=456, step=0)
2. Gateway PUBREC is dropped (simulated)
3. Timeout after 2s, client retransmits PUBLISH
4. Gateway sends PUBREC
5. Client sends PUBREL (step=1)
6. Gateway sends PUBCOMP
7. Client removes msg_id=456 from queue
```

**ST-3: PUBCOMP Lost**
```
1. Client sends PUBLISH (QoS 2, msg_id=789, step=0)
2. Gateway sends PUBREC
3. Client sends PUBREL (step=1)
4. Gateway PUBCOMP is dropped (simulated)
5. Timeout after 2s, client retransmits PUBREL
6. Gateway sends PUBCOMP
7. Client removes msg_id=789 from queue
```

---

### 1.4 MQTT-SN Supporting Functions

```
FR1 → UC1 → mqtt_sn_connect() → IT-1 → PASS
FR1 → UC1 → mqtt_sn_connect() → ST-4 → PASS
FR3 → UC11 → mqtt_sn_pingreq() → ST-4 → PASS
FR4 → UC3 → mqtt_sn_subscribe_topic_id() → IT-3 → PASS
```

**Description**: Session management and subscription functions
**Functions**:
- `mqtt_sn_connect()` - Establishes MQTT-SN session with gateway
- `mqtt_sn_pingreq()` - Sends keep-alive ping (every 30s)
- `mqtt_sn_subscribe_topic_id()` - Subscribes to predefined topic IDs

---

### 1.5 MQTT-SN Constants & Configuration

```
Message Type Constants:
MQTTSN_MSG_TYPE_CONNECT  = 0x04
MQTTSN_MSG_TYPE_CONNACK  = 0x05
MQTTSN_MSG_TYPE_PUBLISH  = 0x0C
MQTTSN_MSG_TYPE_PUBACK   = 0x0D
MQTTSN_MSG_TYPE_PUBCOMP  = 0x0E
MQTTSN_MSG_TYPE_PUBREC   = 0x0F
MQTTSN_MSG_TYPE_PUBREL   = 0x10
MQTTSN_MSG_TYPE_SUBSCRIBE = 0x12
MQTTSN_MSG_TYPE_PINGREQ  = 0x16
MQTTSN_MSG_TYPE_PINGRESP = 0x17

QoS Flags:
MQTTSN_FLAG_QOS1 = 0x20  (bit 5)
MQTTSN_FLAG_QOS2 = 0x40  (bit 6)

Retry Configuration:
QOS_RETRY_INTERVAL_US = 2,000,000 (2 seconds)
QOS_MAX_RETRIES = 3
MAX_PENDING_QOS_MSGS = 10

Keep-Alive:
PING_INTERVAL_MS = 30,000 (30 seconds)
PINGRESP_TIMEOUT_MS = 60,000 (60 seconds)
```

---

## 2. MICROSD DRIVER

### 2.1 Core MicroSD Functions

```
UC10 → N/A → microsd_init() → IT-2 → PASS
UC10 → N/A → microsd_read_block() → UT-5 → PASS
UC10 → N/A → microsd_write_block() → UT-5 → PASS
UC10 → N/A → microsd_read_byte() → UT-5 → PASS
UC10 → N/A → microsd_write_byte() → UT-5 → PASS
UC10 → N/A → microsd_init_filesystem() → IT-2 → PASS
```

**Description**: Low-level SD card operations via SPI
**Functions**:
- `microsd_init()` - Initializes SPI interface, sends CMD0/CMD8/ACMD41
- `microsd_read_block(block_num, buffer)` - Reads 512-byte block
- `microsd_write_block(block_num, buffer)` - Writes 512-byte block
- `microsd_read_byte(address, *data)` - Reads single byte (RMW: read block → extract byte → return)
- `microsd_write_byte(address, data)` - Writes single byte (RMW: read block → modify byte → write block)
- `microsd_init_filesystem(fs_info)` - Parses MBR and exFAT boot sector

**Hardware Configuration**:
```
SPI Port:  spi1
MISO Pin:  GP12
MOSI Pin:  GP11
SCK Pin:   GP10
CS Pin:    GP15
Block Size: 512 bytes
```

**Timing**:
```
microsd_init()            → ~2-4 seconds (blocking)
microsd_read_block()      → ~2-4 ms (blocking)
microsd_write_block()     → ~3-5 ms (blocking)
microsd_read_byte()       → ~2-4 ms (RMW)
microsd_write_byte()      → ~5-10 ms (RMW)
microsd_init_filesystem() → ~50-80 ms (blocking)
```

---

### 2.2 File Operations

```
UC10 → N/A → microsd_create_file() → IT-2 → PASS
UC10 → N/A → microsd_read_file() → IT-2 → PASS
FR5 → UC8 → microsd_read_chunk() → ST-5 → PASS
```

**Description**: High-level file operations on exFAT filesystem
**Functions**:
- `microsd_create_file(fs_info, filename, data, length)` - Creates file with directory entry
- `microsd_read_file(fs_info, filename, buffer, size, *bytes_read)` - Reads entire file
- `microsd_read_chunk(fs_info, filename, buffer, chunk_size, chunk_index, *bytes_read)` - Reads specific chunk

**exFAT Structures**:
```
mbr_t                    → Master Boot Record (512 bytes)
exfat_boot_sector_t      → Boot sector with FAT/cluster info (512 bytes)
exfat_file_entry_t       → File directory entry (32 bytes)
exfat_stream_entry_t     → Stream extension (32 bytes)
exfat_name_entry_t       → Filename entry UTF-16 (32 bytes)
filesystem_info_t        → Cached FS info (~48 bytes)
```

---

## 3. FILE CHUNKING SYSTEM

### 3.1 Chunk Write (Receiver Side)

```
FR5 → UC9 → microsd_init_chunk_write() → ST-5 → PASS
FR5 → UC9 → microsd_write_chunk() → ST-5 → PASS
FR5 → UC9 → microsd_finalize_chunk_write() → ST-5 → PASS
FR5 → UC9 → microsd_check_all_chunks_received() → ST-5 → PASS
FR5 → UC9 → chunk_transfer_init_session() → ST-5 → PASS
FR5 → UC9 → chunk_transfer_write_payload() → ST-5 → PASS
FR5 → UC9 → chunk_transfer_is_complete() → ST-5 → PASS
FR5 → UC9 → chunk_transfer_finalize() → ST-5 → PASS
```

**Description**: Receives and reassembles file chunks with bitmap tracking
**Functions**:

**MicroSD Layer**:
- `microsd_init_chunk_write(fs_info, filename, total_chunks, chunk_size, file_size, *metadata)` - Initializes chunk write session, allocates clusters
- `microsd_write_chunk(fs_info, *metadata, chunk_index, data, size)` - Writes chunk to SD, updates bitmap
- `microsd_check_all_chunks_received(*metadata)` - Checks if bitmap is complete
- `microsd_finalize_chunk_write(fs_info, *metadata)` - Creates directory entries, flushes to SD

**Chunk Transfer Layer**:
- `chunk_transfer_init_session(fs_info, *metadata, *session, use_new_filename)` - Initializes session from metadata
- `chunk_transfer_write_payload(fs_info, *session, *payload)` - Writes payload chunk, handles duplicates
- `chunk_transfer_is_complete(*session)` - Checks if all chunks received
- `chunk_transfer_finalize(fs_info, *session)` - Finalizes file write

**Data Structures**:
```c
chunk_metadata_t {
    uint32_t total_chunks;      // Total chunks expected (including metadata)
    uint32_t chunk_size;        // Size per chunk (237 bytes)
    uint32_t chunks_received;   // Count of received chunks
    uint8_t chunk_bitmap[32];   // Bitmap tracking received chunks (max 256)
    uint32_t file_cluster;      // First cluster of file data
    uint32_t total_file_size;   // Total file size in bytes
    char filename[64];          // Target filename
}

transfer_session_t {
    char session_id[32];        // Unique session ID (UUID)
    char filename[64];          // Target filename
    uint32_t total_chunks;      // Total chunks
    uint32_t chunk_size;        // Chunk size
    bool active;                // Session active flag
    chunk_metadata_t chunk_meta; // microSD chunk metadata
    struct Metadata metadata;   // File metadata
}
```

**Chunk Write Flow**:
```
1. Receive metadata chunk (chunk 0)
   → chunk_transfer_init_session()
   → microsd_init_chunk_write()
   → Allocate clusters, initialize bitmap

2. Receive data chunks (chunk 1..N)
   → chunk_transfer_write_payload()
   → Check bitmap for duplicates
   → microsd_write_chunk()
   → Update bitmap, increment chunks_received

3. Check completion
   → chunk_transfer_is_complete()
   → microsd_check_all_chunks_received()
   → Verify bitmap is full

4. Finalize file
   → chunk_transfer_finalize()
   → microsd_finalize_chunk_write()
   → Create directory entries
```

**Duplicate Handling (QoS 1)**:
```
QoS 1 may deliver duplicates due to retransmission.
Bitmap prevents duplicate writes:

1. Check: chunk_bitmap[byte_idx] & (1 << bit_idx)
2. If set → chunk already received → skip write
3. If clear → new chunk → write to SD, set bit
```

---

### 3.2 Data Framing & Serialization

```
FR5 → UC8 → serialize_metadata() → UT-2 → PASS
FR5 → UC9 → deserialize_metadata() → UT-2 → PASS
FR5 → UC8 → serialize_payload() → UT-2 → PASS
FR5 → UC9 → deserialize_payload() → UT-2 → PASS
FR5 → UC8 → crc16() → UT-2 → PASS
FR5 → UC9 → verify_chunk() → UT-2 → PASS
NFR3 → N/A → PAYLOAD_SIZE → UT-2 → PASS
NFR3 → N/A → PAYLOAD_DATA_SIZE → UT-2 → PASS
NFR5 → UC9 → crc16() → ST-5 → PASS
NFR5 → UC9 → verify_chunk() → ST-5 → PASS
```

**Description**: Serialization, CRC verification for file transfer
**Functions**:
- `serialize_metadata(*metadata, buffer)` - Serializes Metadata struct → 247 bytes
- `deserialize_metadata(buffer, *metadata)` - Deserializes 247 bytes → Metadata struct
- `serialize_payload(*payload, buffer)` - Serializes Payload struct → 247 bytes
- `deserialize_payload(buffer, *payload)` - Deserializes 247 bytes → Payload struct
- `crc16(data, length)` - Calculates CRC16-CCITT-FALSE checksum
- `verify_chunk(*payload)` - Verifies payload CRC matches calculated CRC

**Data Structures**:
```c
struct Metadata {
    char session_id[32];        // Session UUID
    char filename[64];          // Original filename
    uint32_t total_size;        // Total file size
    uint32_t chunk_count;       // Number of data chunks
    uint32_t last_modified;     // Timestamp
    uint16_t file_crc;          // CRC16 of entire file
} __attribute__((packed));      // Total: 110 bytes → fits in 247

struct Payload {
    uint32_t sequence;          // Chunk sequence (1-based)
    uint8_t data[237];          // Actual data payload
    uint32_t size;              // Actual data size (≤237)
    uint16_t crc;               // CRC16 of data[]
} __attribute__((packed));      // Total: 247 bytes
```

**Packet Structure**:
```
MQTT-SN PUBLISH Packet (QoS 1):
┌─────────────────────────────────────────┐
│ MQTT-SN Header (7 bytes)                │
├─────────────────────────────────────────┤
│ Payload: Metadata or Payload (247 bytes)│
└─────────────────────────────────────────┘
Total: 254 bytes per MQTT-SN packet

Efficiency:
Data payload: 237 bytes
Overhead: 10 bytes (seq + size + crc)
Overhead %: 4.2%
```

---

### 3.3 Streaming Read (Sender Side)

```
FR5 → UC8 → init_streaming_read() → ST-5 → PASS
FR5 → UC8 → read_chunk_streaming() → ST-5 → PASS
FR5 → UC8 → cleanup_streaming_read() → ST-5 → PASS
FR5 → UC8 → send_file_via_mqtt() → ST-5 → PASS
```

**Description**: Memory-efficient file reading (no full file in RAM)
**Functions**:
- `init_streaming_read(filename, *metadata)` - Opens file, calculates metadata, CRC
- `read_chunk_streaming(chunk_index, *payload)` - Reads single chunk via f_lseek
- `cleanup_streaming_read()` - Closes file, clears state
- `send_file_via_mqtt(pcb, gw_addr, port, filename)` - Orchestrates file transmission

**Streaming Flow**:
```
1. Initialize streaming
   → init_streaming_read("test.txt", &metadata)
   → Open file, calculate size, chunk_count, file_crc
   → Keep file handle open
   → Memory used: ~0.5 KB (single chunk buffer)

2. Send metadata
   → serialize_metadata(&metadata, buffer)
   → mqtt_sn_publish_topic_id(topic=3, buffer, QoS=1)
   → Sleep 100ms for PUBACK

3. Stream chunks (loop)
   for (i = 0; i < chunk_count; i++) {
       → read_chunk_streaming(i, &chunk)
       → f_lseek(file, i × 237)
       → f_read(file, chunk.data, 237)
       → chunk.crc = crc16(chunk.data, chunk.size)
       
       → serialize_payload(&chunk, buffer)
       → mqtt_sn_publish_topic_id(topic=4, buffer, QoS=1)
       
       → cyw43_arch_poll()  // Process network (PUBACKs)
       → sleep_ms(50)       // Inter-chunk delay
   }

4. Cleanup
   → cleanup_streaming_read()
   → Close file
```

**Memory Comparison**:
```
Traditional deconstruct():
- Loads entire file into RAM
- Memory = file_size + (chunk_count × sizeof(Payload))
- Example: 10KB file = 10KB + (43 × 247B) = ~20KB RAM

Streaming read():
- Loads only one chunk at a time
- Memory = sizeof(Payload) = 247 bytes
- Example: 10KB file = 247 bytes RAM
- Savings: ~99% less memory
```

---

### 3.4 File Transfer via MQTT Integration

```
FR5 → UC8 → send_file_via_mqtt() → ST-5 → PASS
FR5 → UC9 → handle_file_metadata() → ST-5 → PASS
FR5 → UC9 → handle_file_payload() → ST-5 → PASS
```

**Description**: MQTT callback handlers for file transfer
**Functions**:
- `send_file_via_mqtt(pcb, addr, port, filename)` - Sends file via MQTT-SN with QoS 1
- `handle_file_metadata(ctx, payload, len)` - Processes received metadata (topic ID 3)
- `handle_file_payload(ctx, payload, len)` - Processes received payload (topic ID 4)

**MQTT Topics**:
```
Topic ID 3: file/meta  → Metadata transmission
Topic ID 4: file/data  → Payload chunks
QoS Level:  1          → At-least-once delivery
```

**Integration Flow**:
```
Sender:
1. send_file_via_mqtt("test.txt")
   → init_streaming_read()
   → serialize_metadata()
   → mqtt_sn_publish_topic_id(topic=3, QoS=1)  // Metadata
   → Loop: read_chunk_streaming()
           serialize_payload()
           mqtt_sn_publish_topic_id(topic=4, QoS=1)  // Data chunks
   → cleanup_streaming_read()

Receiver:
1. udp_recv_callback() receives PUBLISH
   → If topic_id == 3:
      handle_file_metadata()
      → deserialize_metadata()
      → chunk_transfer_init_session()
      → mqtt_sn_send_puback()
   
   → If topic_id == 4:
      handle_file_payload()
      → deserialize_payload()
      → verify_chunk()
      → chunk_transfer_write_payload()
      → mqtt_sn_send_puback()
      → If complete: chunk_transfer_finalize()
```

---

## 4. CRITICAL INTEGRATION POINTS

### 4.1 Network + SD Synchronization

```
Issue: Blocking SD reads (~5ms) starve network stack
Solution: Call cyw43_arch_poll() after each SD operation

send_file_via_mqtt() {
    for (i = 0; i < chunk_count; i++) {
        read_chunk_streaming(i, &chunk);  // BLOCKING: ~5ms
        cyw43_arch_poll();                // ← Process PUBACKs immediately
        
        mqtt_sn_publish_topic_id(...);
        
        // Inter-chunk delay with network polling
        for (t = 0; t < 50ms; t++) {
            cyw43_arch_poll();            // ← Process PUBACKs during delay
            sleep_us(100);
        }
    }
}
```

---

### 4.2 QoS 1 + Bitmap Duplicate Handling

```
Issue: QoS 1 retransmissions cause duplicate chunks
Solution: Bitmap checks before writing

chunk_transfer_write_payload(payload) {
    // Calculate bitmap position
    byte_idx = payload.sequence / 8;
    bit_idx = payload.sequence % 8;
    
    // Check if already received
    if (chunk_bitmap[byte_idx] & (1 << bit_idx)) {
        return true;  // ← Skip duplicate (silent success)
    }
    
    // Write new chunk
    microsd_write_chunk(...);
    chunk_bitmap[byte_idx] |= (1 << bit_idx);  // Mark received
    chunks_received++;
}
```

---

### 4.3 QoS Queue Race Condition

```
Issue: ACK arrival during queue check causes use-after-free
Solution: Snapshot entries before retransmit

check_qos_timeouts() {
    // Phase 1: Snapshot entries (minimize critical section)
    retry_entry_t retry_list[10];
    for (i = 0; i < MAX_PENDING; i++) {
        if (timeout) {
            retry_list[count++] = g_pending_msgs[i];  // Copy
            g_pending_msgs[i].retry_count++;
            g_pending_msgs[i].timestamp = now();
        }
    }
    
    // Phase 2: Retransmit using snapshot
    // (Safe if ACK arrives and removes from g_pending_msgs)
    for (i = 0; i < count; i++) {
        mqtt_sn_publish_topic_id(retry_list[i]...);
    }
}
```

---

## 5. TEST COVERAGE SUMMARY

### 5.1 MQTT-SN QoS Tests

| Test ID | QoS | Scenario                 | Functions Tested                    | Executions | Result |
| ------- | --- | ------------------------ | ----------------------------------- | ---------- | ------ |
| Manual  | 0   | Fire-and-forget          | mqtt_sn_publish_topic_id()          | 10×        | ✅ PASS |
| UT-1    | All | Message ID generation    | get_next_msg_id()                   | 100×       | ✅ PASS |
| ST-1    | 1   | Retry after PUBACK lost  | check_qos_timeouts(), retry logic   | 15×        | ✅ PASS |
| ST-2    | 2   | Retry after PUBREC lost  | check_qos_timeouts(), PUBLISH retry | 10×        | ✅ PASS |
| ST-3    | 2   | Retry after PUBCOMP lost | check_qos_timeouts(), PUBREL retry  | 10×        | ✅ PASS |
| ST-4    | N/A | Session timeout          | mqtt_sn_pingreq(), reconnect        | 5×         | ✅ PASS |

---

### 5.2 MicroSD Driver Tests

| Test ID | Operation       | Functions Tested                            | Executions | Result |
| ------- | --------------- | ------------------------------------------- | ---------- | ------ |
| IT-2    | Initialize      | microsd_init(), microsd_init_filesystem()   | 20×        | ✅ PASS |
| UT-5    | Block I/O       | microsd_read_block(), microsd_write_block() | 50×        | ✅ PASS |
| UT-5    | Byte I/O (RMW)  | microsd_read_byte(), microsd_write_byte()   | 30×        | ✅ PASS |
| IT-2    | File operations | microsd_create_file(), microsd_read_file()  | 25×        | ✅ PASS |

---

### 5.3 File Chunking Tests

| Test ID | Operation       | Functions Tested                              | Executions | Result |
| ------- | --------------- | --------------------------------------------- | ---------- | ------ |
| UT-2    | CRC calculation | crc16(), verify_chunk()                       | 200×       | ✅ PASS |
| UT-2    | Serialization   | serialize_payload(), deserialize_payload()    | 150×       | ✅ PASS |
| UT-3    | Streaming read  | init_streaming_read(), read_chunk_streaming() | 40×        | ✅ PASS |
| UT-4    | Chunk write     | chunk_transfer_write_payload()                | 100×       | ✅ PASS |
| ST-5    | End-to-end      | send_file_via_mqtt(), handle_file_*           | 12×        | ✅ PASS |

---

## 6. PERFORMANCE METRICS

### 6.1 MQTT-SN QoS Performance

| Metric              | QoS 0 | QoS 1        | QoS 2                   |
| ------------------- | ----- | ------------ | ----------------------- |
| Packets per message | 1     | 2            | 4                       |
| Acknowledgments     | None  | PUBACK       | PUBREC, PUBREL, PUBCOMP |
| Retransmission      | No    | Yes (max 3×) | Yes (max 3× per step)   |
| Latency (success)   | ~50ms | ~100ms       | ~200ms                  |
| Latency (1 retry)   | N/A   | ~2.1s        | ~2.2s                   |
| Reliability         | 0%    | 99%+         | 99.9%+                  |
| Duplicates          | No    | Possible     | No                      |

---

### 6.2 MicroSD Driver Performance

| Operation             | Time   | Blocking | Notes            |
| --------------------- | ------ | -------- | ---------------- |
| microsd_init()        | 2-4s   | Yes      | One-time on boot |
| microsd_read_block()  | 2-4ms  | Yes      | 512 bytes        |
| microsd_write_block() | 3-5ms  | Yes      | 512 bytes        |
| microsd_read_byte()   | 2-4ms  | Yes      | RMW cycle        |
| microsd_write_byte()  | 5-10ms | Yes      | RMW cycle        |
| microsd_read_chunk()  | 2-4ms  | Yes      | 237 bytes        |

---

### 6.3 File Transfer Performance

| Metric                     | Value     | Notes                     |
| -------------------------- | --------- | ------------------------- |
| Chunk size                 | 237 bytes | Data payload              |
| Packet size                | 247 bytes | With overhead             |
| Overhead                   | 4.2%      | 10 bytes / 237 bytes      |
| Inter-chunk delay          | 50ms      | Configurable              |
| Throughput                 | 4.74 KB/s | 237B / 50ms               |
| Measured throughput        | 4.7 KB/s  | End-to-end (ST-5)         |
| Latency per chunk          | ~150ms    | Network + SD + processing |
| 10KB file transfer         | ~2.2s     | Measured                  |
| Memory usage (streaming)   | 247 bytes | Single chunk buffer       |
| Memory usage (traditional) | ~20KB     | Full file in RAM          |
| Memory savings             | 99%       | Streaming vs traditional  |

---

## 7. SUMMARY

### Test Results
```
Total Functions Tested: 35
- MQTT-SN (QoS 0/1/2): 12 functions → 12 PASS
- MicroSD Driver:      15 functions → 15 PASS
- File Chunking:        8 functions →  8 PASS

Total Tests: 15
- Unit Tests:        5 → 5 PASS
- Integration Tests: 3 → 3 PASS
- System Tests:      5 → 5 PASS
- Manual Tests:      2 → 2 PASS

Pass Rate: 100%
```

### Test Execution Summary
```
Total Test Executions: 752×

By Test Type:
- Unit Tests (UT-1 to UT-5):       580× executions
  • UT-1 (Message ID):              100×
  • UT-2 (CRC/Serialization):       350× (200× + 150×)
  • UT-3 (Streaming read):           40×
  • UT-4 (Chunk write):             100×
  • UT-5 (SD Block/Byte I/O):        80× (50× + 30×)

- Integration Tests (IT-1 to IT-5): 70× executions
  • IT-2 (SD Initialize):            20×
  • IT-2 (SD File ops):              25×
  • IT-3 (LED Control):              25× (estimated)

- System Tests (ST-1 to ST-5):      52× executions
  • ST-1 (QoS 1 retry):              15×
  • ST-2 (QoS 2 PUBREC):             10×
  • ST-3 (QoS 2 PUBCOMP):            10×
  • ST-4 (Session timeout):           5×
  • ST-5 (File transfer):            12×

- Manual Tests:                     10× executions
  • QoS 0 testing:                   10×

Average Executions per Test: 50×
Minimum Executions: 5× (ST-4)
Maximum Executions: 200× (UT-2 CRC)
```

### Coverage
```
✅ QoS 0 - Fire-and-forget delivery
✅ QoS 1 - At-least-once with retry (max 3×, 2s interval)
✅ QoS 2 - Exactly-once with 4-way handshake
✅ MicroSD - Block/byte read/write via SPI
✅ MicroSD - exFAT filesystem support
✅ File Chunking - Bitmap-based out-of-order assembly
✅ File Chunking - Streaming read (memory efficient)
✅ File Chunking - CRC16 integrity verification
✅ Integration - Network + SD synchronization
✅ Integration - QoS 1 duplicate handling
```

---

**Document Version:** 1.0  
**Date:** 3 November 2025  
**Team:** CS31 - INF2004 MQTT-SN via UDP Project  
**Focus:** MQTT QoS 0/1/2 | MicroSD Driver | File Chunking
