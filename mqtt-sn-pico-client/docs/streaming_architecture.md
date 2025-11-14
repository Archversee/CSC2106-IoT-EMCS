# Streaming File Transfer Architecture

## Overview

This document explains the buffered streaming read and write implementation for MQTT-SN file transfers on the Raspberry Pi Pico W. The system is designed to handle large files (multi-MB) with minimal RAM usage by streaming data in chunks rather than loading entire files into memory.

---

## Memory Constraints

**Raspberry Pi Pico W RAM**: 264 KB total
- System overhead: ~50-100 KB
- Available for application: ~150-200 KB
- Our streaming implementation: ~33-34 KB peak usage

**Goal**: Transfer files of any size without loading entire file into RAM

---

## Stream Read Architecture (Sender Side)

### Components

1. **Read Buffer**: 32 KB dynamically allocated buffer
2. **Chunk Buffer**: 237 bytes for current chunk being transmitted
3. **File Handle**: Persistent FatFS file handle (kept open during session)

### Read Flow Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                    SD Card File (e.g., 1.15 MB)                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │ [Sector 0] [Sector 1] [Sector 2] ... [Sector N]         │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                              │
                              │ FatFS f_read() - 32KB at a time
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                    RAM Read Buffer (32 KB)                      │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │ [32768 bytes of file data]                               │   │
│  │  Contains ~138 chunks (32KB ÷ 237 bytes per chunk)      │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                  │
│  buffer_file_offset: 0         (tracks position in file)        │
│  buffer_valid_bytes: 32768     (how much is currently valid)    │
└─────────────────────────────────────────────────────────────────┘
                              │
                              │ memcpy() - Extract 237 bytes
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                   Chunk Buffer (237 bytes)                      │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │ [237 bytes] + sequence + size + CRC                      │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                              │
                              │ MQTT-SN Publish (QoS 1)
                              ▼
                      Network Transmission
```

### Read Algorithm

```
For each chunk request (chunk_index):
  
  1. Calculate file_offset = chunk_index × 237
  
  2. Check if data is in read buffer:
     ┌─ Is buffer empty? ──────────────────────► Refill needed
     ├─ Is offset before buffer start? ────────► Refill needed
     ├─ Is offset after buffer end? ───────────► Refill needed
     └─ Does chunk span beyond buffer? ────────► Refill needed
  
  3. If refill needed:
     ├─ f_lseek() to file_offset
     ├─ f_read() up to 32KB into buffer
     └─ Update buffer_file_offset and buffer_valid_bytes
  
  4. Copy chunk from buffer:
     ├─ buffer_offset = file_offset - buffer_file_offset
     └─ memcpy(chunk, buffer + buffer_offset, 237)
  
  5. Set chunk metadata:
     ├─ sequence = chunk_index + 1
     ├─ size = 237 (or less for last chunk)
     └─ crc = CRC16(data)
  
  6. Return chunk ready for transmission
```

### Performance for 1.15 MB File (4982 chunks)

**Without buffering**:
- SD reads: 4,982 (one per chunk)
- Seeks: 4,982
- Bytes per read: 237

**With 32KB buffering**:
- SD reads: ~36-39 (1,180,650 ÷ 32,768)
- Seeks: ~36-39
- Bytes per read: 32,768
- **128x reduction in I/O operations!**

---

## Stream Write Architecture (Receiver Side)

### Components

1. **Temporary File Handle**: Persistent FatFS file handle (kept open during session)
2. **Bitmap Tracker**: Dynamically allocated bitmap for chunk tracking
3. **Sparse File**: Pre-allocated file with gaps filled as chunks arrive

### Write Flow Diagram

```
                    MQTT-SN Messages (arrive out-of-order)
                              │
                              │ QoS 1 (at-least-once)
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│              Chunk Receiver (chunk_transfer module)             │
│                                                                  │
│  Bitmap Tracker (dynamically allocated):                        │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │ [0][1][1][0][1][1][1][0] ... (1 bit per chunk)          │   │
│  │  ↑  ↑  ↑  ↑                                             │   │
│  │  │  │  │  └─ Chunk 3: NOT received                      │   │
│  │  │  │  └──── Chunk 2: Received ✓                        │   │
│  │  │  └─────── Chunk 1: Received ✓                        │   │
│  │  └────────── Chunk 0: Metadata (always set)             │   │
│  │                                                          │   │
│  │  Size: (total_chunks + 7) ÷ 8 bytes                     │   │
│  │  Example: 4983 chunks → 623 bytes                       │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                              │
                              │ Validate & track in bitmap
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                Temporary File (filename.tmp)                    │
│              File Handle kept OPEN during session               │
│                                                                  │
│  Pre-allocated size: chunk_count × 237 bytes                    │
│                                                                  │
│  ┌─────┬─────┬─────┬─────┬─────┬─────────────────────────┐     │
│  │ CH1 │ CH2 │ ??? │ CH4 │ CH5 │ ... sparse ...          │     │
│  └─────┴─────┴─────┴─────┴─────┴─────────────────────────┘     │
│    ↑     ↑     ↑     ↑     ↑                                    │
│    │     │     │     │     └─ f_lseek() to offset, f_write()   │
│    │     │     │     └─────── Chunk 4 written                   │
│    │     │     └───────────── Chunk 3 missing (gap)             │
│    │     └─────────────────── Chunk 2 written                   │
│    └───────────────────────── Chunk 1 written                   │
│                                                                  │
│  Every 100 chunks: f_sync() to commit data to SD card          │
└─────────────────────────────────────────────────────────────────┘
                              │
                              │ When all chunks received
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                    Finalization Process                         │
│                                                                  │
│  1. f_sync() - Final flush to SD card                          │
│  2. f_close() - Close temporary file handle                    │
│  3. f_rename() - Rename .tmp to final filename                 │
│  4. f_truncate() - Trim to actual file size                    │
│  5. f_unlink() - Delete .meta file                             │
│  6. free() - Release bitmap memory                             │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
                    Final File on SD Card
```

### Write Algorithm

```
On chunk received (out-of-order):

  1. Verify sequence number:
     ├─ Must be 1 to chunk_count (0 is metadata)
     └─ If invalid, reject
  
  2. Check bitmap for duplicate:
     ├─ byte_idx = sequence ÷ 8
     ├─ bit_idx = sequence % 8
     ├─ If bit already set → Skip (duplicate, QoS 1 allows this)
     └─ If not set → Continue
  
  3. Verify bitmap bounds:
     └─ If byte_idx ≥ bitmap_size → ERROR (memory protection)
  
  4. Write chunk to file (handle is already open):
     ├─ offset = (sequence - 1) × 237
     ├─ f_lseek(file_handle, offset)
     ├─ f_write(file_handle, chunk_data, chunk_size)
     └─ No f_close() - keep handle open!
  
  5. Every 100 chunks:
     └─ f_sync(file_handle) - Periodic commit to SD
  
  6. Update bitmap:
     ├─ chunk_bitmap[byte_idx] |= (1 << bit_idx)
     └─ chunks_received++
  
  7. Check if complete:
     └─ If chunks_received == total_chunks → Ready to finalize
```

### Write Performance vs. SD Card Health

**Old Approach (open/close per chunk)**:
```
For each chunk:
  ├─ f_open() ───┐
  ├─ f_lseek()   │
  ├─ f_write()   │ 4,982 iterations
  └─ f_close() ──┘

Result: 4,982 FAT updates, high SD wear
```

**New Approach (persistent handle)**:
```
Session init:
  └─ f_open() (once)

For each chunk:
  ├─ f_lseek()   │
  └─ f_write()   │ 4,982 iterations

Every 100 chunks:
  └─ f_sync()    │ ~50 syncs

Session finalize:
  ├─ f_sync()
  └─ f_close() (once)

Result: 1 open, 1 close, ~50 syncs
        Much healthier for SD card!
```

---

## Complete Transfer Flow

### Initialization Phase

```
SENDER (Pico W #1)                    RECEIVER (Pico W #2)
─────────────────                     ─────────────────────

1. User requests transfer
   └─ init_streaming_read("file.bin")
      ├─ f_stat() get file size
      ├─ f_open() keep handle
      ├─ malloc(32KB) read buffer
      └─ Generate metadata
   
2. Send metadata (QoS 2)  ──────────►  3. Receive metadata
   Chunk 0:                               └─ chunk_transfer_init_session()
   - filename                                ├─ malloc() bitmap
   - total_size                              ├─ f_open() temp file
   - chunk_count                             ├─ f_lseek() pre-allocate
   - session_id                              └─ Mark chunk 0 received
```

### Data Transfer Phase (Out-of-Order, QoS 1)

```
SENDER                                RECEIVER
──────                                ────────

Loop for i = 0 to chunk_count-1:

  read_chunk_streaming(i)
  ├─ Check if in 32KB buffer
  ├─ If not: f_read(32KB)
  └─ memcpy(237 bytes)
  
  Publish chunk ─────────────────────►  Receive chunk
  (QoS 1, may duplicate)                 └─ chunk_transfer_write_payload()
                                            ├─ Check bitmap (skip if dup)
                                            ├─ f_lseek() to position
                                            ├─ f_write() chunk data
                                            ├─ Update bitmap
                                            └─ if (count % 100 == 0) f_sync()

  [Chunks may arrive out of order]
  [Network may retransmit duplicates]
```

### Completion Phase

```
SENDER                                RECEIVER
──────                                ────────

All chunks sent:                      All chunks received:
  └─ cleanup_streaming_read()           └─ chunk_transfer_finalize()
     ├─ f_close()                          ├─ f_sync()
     └─ free(32KB buffer)                  ├─ f_close()
                                           ├─ f_rename(.tmp → final)
                                           ├─ f_truncate() to actual size
                                           ├─ f_unlink(.meta)
                                           └─ free(bitmap)
                                     
                                        File ready on SD card!
```

---

## Memory Usage Breakdown

### Sender (Read Side)

```
┌─────────────────────────────────────┐
│ Read Buffer         : 32,768 bytes  │  32.0 KB
│ Chunk Buffer        :    250 bytes  │   0.24 KB
│ Streaming Context   :     64 bytes  │   0.06 KB
│ FIL Handle          :    ~32 bytes  │   0.03 KB
├─────────────────────────────────────┤
│ TOTAL (sender)      :         ~33 KB │
└─────────────────────────────────────┘
```

### Receiver (Write Side)

```
┌─────────────────────────────────────┐
│ Bitmap (4983 chunks):    623 bytes  │   0.61 KB
│ Session Metadata    :    300 bytes  │   0.29 KB
│ FIL Handle          :     32 bytes  │   0.03 KB
│ Chunk Buffer        :    250 bytes  │   0.24 KB
├─────────────────────────────────────┤
│ TOTAL (receiver)    :        ~1.2 KB │
└─────────────────────────────────────┘

Note: Temporary file on SD card (not in RAM)
      Pre-allocated: 4982 × 237 = 1,180,734 bytes
```

---

## Key Optimizations

### 1. Buffered Reading (32KB)
- **Problem**: Reading 237 bytes at a time causes excessive SD seeks
- **Solution**: Read 32KB chunks, serve 138+ chunks from RAM buffer
- **Benefit**: 128x reduction in SD I/O operations

### 2. Persistent File Handles
- **Problem**: Opening/closing files thousands of times wears SD card FAT
- **Solution**: Open once, keep handle during entire session, close once
- **Benefit**: Minimal FAT updates, better SD longevity

### 3. Dynamic Bitmap
- **Problem**: Fixed-size bitmap wastes memory or limits file size
- **Solution**: Allocate exactly `(chunk_count + 7) ÷ 8` bytes
- **Benefit**: Scales to any file size, no memory waste

### 4. Periodic Syncing
- **Problem**: Buffered writes risk data loss on power failure
- **Solution**: Call `f_sync()` every 100 chunks (~23KB)
- **Benefit**: Limits potential data loss to ~100 chunks max

### 5. Out-of-Order Write Support
- **Problem**: MQTT-SN doesn't guarantee chunk order
- **Solution**: Sparse file with bitmap tracking + f_lseek() per chunk
- **Benefit**: Accepts chunks in any order, handles duplicates gracefully

---

## Scalability Analysis

### Current Implementation Limits

| File Size | Chunks  | Bitmap Size  | Read Buffer | Total RAM (Sender) | Total RAM (Receiver) |
| --------- | ------- | ------------ | ----------- | ------------------ | -------------------- |
| 1 MB      | 4,430   | 554 bytes    | 32 KB       | ~33 KB             | ~1.2 KB              |
| 5 MB      | 22,151  | 2,769 bytes  | 32 KB       | ~33 KB             | ~3.4 KB              |
| 10 MB     | 44,303  | 5,538 bytes  | 32 KB       | ~33 KB             | ~6.1 KB              |
| 50 MB     | 221,519 | 27,690 bytes | 32 KB       | ~33 KB             | ~28 KB               |

**Maximum file size**: Limited only by SD card capacity
- Bitmap scales linearly: ~6.25 bytes per MB
- Read buffer constant: 32 KB regardless of file size
- Sender RAM: ~33 KB constant
- Receiver RAM: ~1 KB + (file_size_MB × 6.25 bytes)

---

## Error Handling

### Read Errors
```
Buffer refill fails:
  ├─ f_lseek() error → Return error, log FatFS code
  ├─ f_read() error → Return error, log bytes read vs expected
  └─ Session cleanup: f_close(), free(buffer)
```

### Write Errors
```
Chunk write fails:
  ├─ Invalid sequence → Reject, log warning
  ├─ Duplicate chunk → Skip (expected with QoS 1)
  ├─ f_lseek() error → Return error, session remains active
  ├─ f_write() error → Return error, session remains active
  └─ Bitmap overflow → ERROR, reject (memory protection)

Session cleanup on error:
  ├─ f_close(tmp_file)
  ├─ f_unlink(tmp_file)
  ├─ f_unlink(meta_file)
  └─ free(bitmap)
```

---

## Future Enhancements

### Potential Improvements

1. **Adaptive Buffer Size**
   - Start with 8KB, increase to 32KB if file > 1MB
   - Reduces RAM for small files

2. **Compression**
   - Add zlib/lz4 compression layer
   - Reduce network bandwidth
   - Trade CPU for transmission time

3. **Resume Support**
   - Save bitmap to SD periodically
   - Resume incomplete transfers after reboot

4. **Multi-File Transfers**
   - Queue multiple files
   - Interleave chunks from different files
   - Better network utilization

5. **Chunk Priority**
   - Send first/last chunks with higher priority
   - Enable faster previews (e.g., for images)

---

## Conclusion

The streaming architecture achieves:
- ✅ **Scalability**: No file size limits (tested up to 1.15 MB)
- ✅ **Memory Efficiency**: ~33 KB sender, ~1-6 KB receiver
- ✅ **SD Card Health**: 128x fewer I/O operations
- ✅ **Reliability**: Handles out-of-order delivery and duplicates
- ✅ **Performance**: ~36-39 reads instead of 4,982 for 1.15 MB file

This design is production-ready for MQTT-SN file transfers on resource-constrained IoT devices.
