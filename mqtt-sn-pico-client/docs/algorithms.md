# 7.3. Algorithms

## 7.3.1. Main Control Loop (Bare-metal Scheduler)

**File:** `mqtt-sn-pico-client.c::main()`

### Algorithm:

```
Initialization:
  stdio_init_all()
  initialize_microsd(&fs_info, max_attempts=3)
  cyw43_arch_init(), cyw43_arch_enable_sta_mode()
  cyw43_arch_wifi_connect_timeout_ms(SSID, PASS, TIMEOUT)
  udp_new(), udp_bind(), udp_recv(callback)
  mqtt_sn_connect(gateway_addr)
  mqtt_sn_subscribe_topic_id(1, 3, 4)

Loop (while true):
  cyw43_arch_poll()                        // Critical: network processing
  
  if button_pressed(GP20): publish_message(qos_level)
  if button_pressed(GP21): toggle_qos_level()
  if button_pressed(GP22): toggle_drop_acks()
  if button_pressed(GP19): send_file_via_mqtt(filename)
  
  if time_for_ping: mqtt_sn_pingreq()
  if ping_timeout: reconnect_mqtt()
  
  check_qos_timeouts()                     // Retry unacked messages
  check_microsd_hotplug()                  // Detect card insertion/removal
  check_wifi_status()                      // Reconnect if needed
  
  sleep_ms(100)
```

**Time Complexity:** O(M) where M = max pending QoS messages (10)  
**Critical:** `cyw43_arch_poll()` must be called frequently; blocking >50ms degrades network

---

## 7.3.2. Legacy File Deconstruction (Memory-Intensive)

**File:** `fs/data_frame.c::deconstruct()`

### Algorithm:

```
Input: filename, Metadata* meta, Payload** chunks

microsd_init_filesystem(&fs_info)
file_buffer = malloc(32KB)
microsd_read_file(filename, file_buffer, &file_size)

chunk_count = ceil(file_size / 237)
chunks = malloc(sizeof(Payload) * chunk_count)
file_crc = crc16(file_buffer, file_size)

Fill metadata:
  meta->filename = filename
  meta->total_size = file_size
  meta->chunk_count = chunk_count
  meta->file_crc = file_crc

For i = 0 to chunk_count - 1:
  chunks[i].sequence = i
  chunks[i].data = copy from file_buffer[i * 237]
  chunks[i].size = min(remaining, 237)
  chunks[i].crc = crc16(chunks[i].data, size)

free(file_buffer)
Return 0 on success
```

**Time Complexity:** O(N) where N = file size  
**Space Complexity:** O(N) - 32KB buffer + chunk array  
**Deprecated:** Use streaming API for files >10KB

---

## 7.3.3. Stream-Based Deconstruction (Memory-Efficient)

**File:** `fs/data_frame.c::init_streaming_read()`, `read_chunk_streaming()`, `cleanup_streaming_read()`

### Algorithm:

```
// Phase 1: Initialization
init_streaming_read(filename, Metadata* meta):
  microsd_init_filesystem()
  file = microsd_open_file(filename, "r")
  file_size = get_file_size(file)
  chunk_count = ceil(file_size / 237)
  session_id = generate_uuid()
  file_crc = microsd_calculate_file_crc16(file)  // Blocking: ~2ms per KB
  
  meta->filename = filename
  meta->total_size = file_size
  meta->chunk_count = chunk_count
  meta->session_id = session_id
  meta->file_crc = file_crc
  
  s_streaming_state.file = file  // Keep file open
  Return 0

// Phase 2: Chunk Reading  
read_chunk_streaming(chunk_index, Payload* chunk):
  if chunk_index >= s_streaming_state.chunk_count: return -1
  
  offset = chunk_index * 237
  f_lseek(file, offset)
  bytes_read = f_read(file, buffer, 237)
  
  chunk->sequence = chunk_index + 1
  chunk->size = bytes_read
  chunk->data = buffer
  chunk->crc = crc16(buffer, bytes_read)
  Return 0

// Phase 3: Cleanup
cleanup_streaming_read():
  microsd_close_file(s_streaming_state.file)
  memset(&s_streaming_state, 0, sizeof(...))
```

**Time Complexity:** O(N) init, O(1) per chunk  
**Space Complexity:** O(1) - constant 0.5KB memory

---

## 7.3.4. File Reconstruction (Bitmap-based Chunking)

**File:** `fs/chunk_transfer.c`

### Algorithm:

```
// Step 1: Initialize Session
chunk_transfer_init_session(fs_info, Metadata metadata, Session* session):
  session->metadata = metadata
  session->total_chunks = metadata.chunk_count + 1  // Include metadata chunk
  session->chunk_size = 237
  
  microsd_init_chunk_write(filename, total_chunks):
    memset(chunk_bitmap, 0, 32)  // 256 chunks max
    chunks_received = 0
  
  microsd_write_chunk(fs_info, metadata, chunk_index=0)
  chunk_bitmap[0] |= 1  // Mark chunk 0 received
  chunks_received++
  
  session->active = true
  Return true

// Step 2: Receive Chunk
chunk_transfer_write_payload(fs_info, Session* session, Payload payload):
  if !session->active: return false
  if payload.sequence < 1 or payload.sequence > chunk_count: return false
  
  // Check duplicate
  byte_idx = payload.sequence / 8
  bit_idx = payload.sequence % 8
  if chunk_bitmap[byte_idx] & (1 << bit_idx):
    return true  // Skip duplicate (QoS 1)
  
  // Write chunk
  offset = (payload.sequence - 1) * 237
  f_lseek(file, offset)
  f_write(file, payload.data, payload.size)
  
  // Mark received
  chunk_bitmap[byte_idx] |= (1 << bit_idx)
  chunks_received++  // O(1) increment
  Return true

// Step 3: Check Completion
chunk_transfer_is_complete(Session* session):
  if !session or !session->active: return false
  Return chunks_received == total_chunks  // O(1) comparison

// Step 4: Finalize
chunk_transfer_finalize(fs_info, Session* session):
  if !chunk_transfer_is_complete(session): return false
  
  microsd_finalize_chunk_write(fs_info)  // Flush cache, close file
  session->active = false
  Return true
```

**Time Complexity:** O(C) total, O(1) per chunk, O(1) completion check  
**Space Complexity:** O(1) - 32-byte bitmap + single chunk buffer

---

## 7.3.5. File Transfer via MQTT-SN (Streaming with Network Polling)

**File:** `mqtt/mqtt-sn-udp.c::send_file_via_mqtt()`

### Algorithm:

```
send_file_via_mqtt(pcb, gw_addr, gw_port, filename):
  // Phase 1: Initialize
  init_streaming_read(filename, &metadata)  // Blocking: CRC ~2ms/KB
  
  // Phase 2: Send Metadata
  serialize_metadata(&metadata, meta_buffer)
  msg_id = get_next_msg_id()
  mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port, 
                           topic_id=3, meta_buffer, qos=1, msg_id)
  sleep_ms(100)
  
  // Phase 3: Stream Chunks
  For i = 0 to metadata.chunk_count - 1:
    read_chunk_streaming(i, &chunk)        // Blocking: ~5ms SD read
    cyw43_arch_poll()                      // Critical: process PUBACKs
    
    if !verify_chunk(&chunk): continue
    serialize_payload(&chunk, payload_buffer)
    
    msg_id = get_next_msg_id()
    mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port,
                             topic_id=4, payload_buffer, qos=1, msg_id)
    
    // Interleaved network polling (instead of sleep_ms(50))
    delay_start = get_absolute_time()
    while (get_absolute_time() - delay_start) < 50ms:
      cyw43_arch_poll()                    // Process PUBACKs during delay
      sleep_us(100)
  
  // Phase 4: Cleanup
  cleanup_streaming_read()
```

**Time Complexity:** O(N + C) where N = file size  
**Space Complexity:** O(1) - constant 0.5KB memory  
**Critical:** `cyw43_arch_poll()` after SD reads prevents network starvation

---

## 7.3.6. MQTT-SN QoS 0 (Fire and Forget)

**File:** `mqtt/mqtt-sn-udp.c::mqtt_sn_publish_topic_id()`

### Algorithm:

```
// Sender
mqtt_sn_publish_qos0(pcb, gw_addr, gw_port, topic_id, payload, payload_len):
  packet_len = 7 + payload_len
  p = pbuf_alloc(PBUF_TRANSPORT, packet_len, PBUF_RAM)
  
  data[0] = packet_len
  data[1] = 0x0C                    // PUBLISH
  data[2] = 0x01                    // QoS 0, predefined topic
  data[3:4] = topic_id
  data[5:6] = 0x0000                // No message ID
  data[7...] = payload
  
  udp_sendto(pcb, p, gw_addr, gw_port)
  pbuf_free(p)
  // No retry logic, no storage

// Receiver
udp_recv_callback():
  if msg_type == 0x0C and qos == 0:
    process_payload(payload)
    // No PUBACK sent
```

**Time Complexity:** O(1)  
**Network Overhead:** 1 packet (7 + payload_len bytes)  
**Use Case:** Telemetry, heartbeats (not file transfer)

---

## 7.3.7. MQTT-SN QoS 1 (At Least Once)

**File:** `mqtt/mqtt-sn-udp.c::mqtt_sn_publish_topic_id()` + `check_qos_timeouts()`

### Algorithm:

```
// Sender: Publish
mqtt_sn_publish_qos1(pcb, gw_addr, gw_port, topic_id, payload, payload_len, msg_id):
  packet_len = 7 + payload_len
  p = pbuf_alloc(PBUF_TRANSPORT, packet_len, PBUF_RAM)
  
  data[0] = packet_len
  data[1] = 0x0C                    // PUBLISH
  data[2] = 0x21                    // QoS 1 (bit 5 set)
  data[3:4] = topic_id
  data[5:6] = msg_id                // Required for QoS 1
  data[7...] = payload
  
  udp_sendto(pcb, p, gw_addr, gw_port)
  pbuf_free(p)
  
  // Store for retry
  g_pending_msgs[slot].in_use = true
  g_pending_msgs[slot].msg_id = msg_id
  g_pending_msgs[slot].qos = 1
  g_pending_msgs[slot].timestamp = now()
  g_pending_msgs[slot].retry_count = 0
  g_pending_msgs[slot].payload = payload

// Receiver: PUBACK
udp_recv_callback():
  if msg_type == 0x0C and qos == 1:
    process_payload(payload)
    mqtt_sn_send_puback(pcb, addr, port, topic_id, msg_id, 0x00)

// Sender: Timeout & Retry (in main loop)
check_qos_timeouts(pcb, gw_addr, gw_port):
  For each msg in g_pending_msgs:
    if !msg.in_use: continue
    if (now() - msg.timestamp) > 3s:
      if msg.retry_count >= 3:
        msg.in_use = false        // Give up
      else:
        mqtt_sn_publish_qos1(...)  // Retransmit
        msg.retry_count++
        msg.timestamp = now()

// Sender: PUBACK Received
udp_recv_callback():
  if msg_type == 0x0D:             // PUBACK
    remove_pending_qos_msg(msg_id)
```

**Time Complexity:** O(1) send, O(M) retry check  
**Network Overhead:** 2 packets (best case), 8 packets (worst case with 3 retries)  
**Use Case:** File transfer (bitmap handles duplicates)

---

## 7.3.8. MQTT-SN QoS 2 (Exactly Once)

**File:** `mqtt/mqtt-sn-udp.c` - Multiple functions

### Algorithm:

```
// Step 1: Sender PUBLISH
mqtt_sn_publish_qos2(pcb, gw_addr, gw_port, topic_id, payload, msg_id):
  data[2] = 0x41                    // QoS 2 (bit 6 set)
  data[5:6] = msg_id
  udp_sendto(pcb, ...)
  
  g_pending_msgs[slot].qos = 2
  g_pending_msgs[slot].step = 0     // Waiting for PUBREC

// Step 2: Receiver PUBREC
udp_recv_callback():
  if msg_type == 0x0C and qos == 2:
    buffer_msg_id(msg_id)           // Store to detect duplicates
    mqtt_sn_send_pubrec(pcb, addr, port, msg_id)

// Step 3: Sender PUBREL
udp_recv_callback():
  if msg_type == 0x0F:              // PUBREC
    g_pending_msgs[slot].step = 1   // Now waiting for PUBCOMP
    g_pending_msgs[slot].timestamp = now()
    mqtt_sn_send_pubrel(pcb, addr, port, msg_id)

// Step 4: Receiver PUBCOMP (process payload NOW)
udp_recv_callback():
  if msg_type == 0x10:              // PUBREL
    process_payload(payload)        // Exactly-once guarantee
    mqtt_sn_send_pubcomp(pcb, addr, port, msg_id)
    clear_buffered_msg_id(msg_id)

// Step 5: Sender Complete
udp_recv_callback():
  if msg_type == 0x0E:              // PUBCOMP
    remove_pending_qos_msg(msg_id)

// Retry Logic
check_qos_timeouts():
  For each msg in g_pending_msgs where qos == 2:
    if msg.step == 0 and timeout:   // Waiting PUBREC
      retransmit PUBLISH
    if msg.step == 1 and timeout:   // Waiting PUBCOMP
      retransmit PUBREL
```

**State Machine:**
```
Sender:   PUBLISH → [wait PUBREC] → PUBREL → [wait PUBCOMP] → DONE
Receiver: [wait PUBLISH] → PUBREC → [wait PUBREL] → PUBCOMP → DONE
```

**Time Complexity:** O(1) per step  
**Network Overhead:** 4 packets (best case), 16 packets (worst case)  
**Use Case:** Payment, critical actuators (not file transfer)

---

## Summary Table

| Algorithm              | File                  | Time Complexity            | Space Complexity | Notes                         |
| ---------------------- | --------------------- | -------------------------- | ---------------- | ----------------------------- |
| **Main Loop**          | mqtt-sn-pico-client.c | O(M) per iteration         | O(1)             | M = max pending QoS msgs      |
| **Legacy Deconstruct** | data_frame.c          | O(N)                       | O(N)             | Loads entire file, deprecated |
| **Stream Deconstruct** | data_frame.c          | O(N) init, O(1) per chunk  | O(1)             | Memory efficient, preferred   |
| **Reconstruction**     | chunk_transfer.c      | O(C) total, O(1) per chunk | O(1)             | C = chunk count               |
| **File Transfer**      | mqtt-sn-udp.c         | O(N + C)                   | O(1)             | With network polling          |
| **QoS 0 Publish**      | mqtt-sn-udp.c         | O(1)                       | O(1)             | Fire-and-forget               |
| **QoS 1 Publish**      | mqtt-sn-udp.c         | O(1) send, O(M) retry      | O(M)             | At-least-once delivery        |
| **QoS 2 Publish**      | mqtt-sn-udp.c         | O(1) per step              | O(M)             | Exactly-once delivery         |

### Performance Characteristics

| Metric                     | Legacy API   | Streaming API     | Improvement         |
| -------------------------- | ------------ | ----------------- | ------------------- |
| **Memory (50KB file)**     | ~85KB        | ~0.5KB            | **99.4% reduction** |
| **File size limit**        | 32KB         | 4GB (theoretical) | **128× larger**     |
| **Init time (50KB)**       | 150ms        | 100ms             | 33% faster          |
| **Network responsiveness** | Blocked 55ms | <200μs            | **275× better**     |

---

## Best Practices

1. **File Transfer:** Always use streaming API (`init_streaming_read()`, not `deconstruct()`)
2. **QoS Selection:**
   - QoS 0: Telemetry, sensor data, non-critical events
   - **QoS 1: File transfer** (bitmap handles duplicates)
   - QoS 2: Payment, critical actuator control (rarely needed)
3. **Network Polling:**
   - Call `cyw43_arch_poll()` after any operation >1ms
   - Use polling loops instead of `sleep_ms()` for delays >10ms
4. **Memory Management:**
   - Avoid `deconstruct()` for files >10KB
   - Pre-allocate session buffers (don't malloc during transfer)
5. **Error Handling:**
   - Always check return values from SD card operations
   - Implement retry logic for filesystem initialization
   - Verify CRC after file transfer completion
