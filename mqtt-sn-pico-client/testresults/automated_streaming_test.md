## logged from automated_steaming_test.uf2

The purpose of this test is to validate the automated streaming functionality of the MQTT-SN Pico client. The test checks the client's ability to send and receive large data streams in chunks, ensuring data integrity and proper sequencing.

``` log

Streaming read test completed successfully

=== Step 2: Go-Back-N MQTT Streaming with Sliding Window ===
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: test.jpg
  Size: 1060364 bytes (1035.51 KB)
  Chunks: 4437
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
SENDER: Preparing to transmit file via MQTT-SN (Go-Back-N)
  Session ID: stream_442583
  Total chunks: 4437
  File size: 1060364 bytes
Sliding Window initialized:
  Window size: 138 chunks (32706 bytes)
  Total chunks: 4437

[MQTT:file/data] Publishing metadata (chunk 0)...

--- Window [1-138] ---
  [MQTT:file/data] Chunk 1/4437 (seq=1, size=239, crc32=0xA17BB47D)
  [MQTT:file/data] Chunk 2/4437 (seq=2, size=239, crc32=0xBADEF91F)
  [MQTT:file/data] Chunk 3/4437 (seq=3, size=239, crc32=0x6BAC12B6)
  [MQTT:file/data] Chunk 51/4437 (seq=51, size=239, crc32=0x2D6EDFA0)
  [MQTT:file/data] Chunk 101/4437 (seq=101, size=239, crc32=0x34FABA2A)
  [MQTT:file/data] Chunk 138/4437 (seq=138, size=239, crc32=0xC48074F2)
  ✓ Transmitted 138 chunks in window [1-138]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 36
  [RECEIVER] Sending NACK - retransmit from chunk 36

--- Window [36-173] ---
  [MQTT:file/data] Chunk 36/4437 (seq=36, size=239, crc32=0xEFAA224C)
  [MQTT:file/data] Chunk 37/4437 (seq=37, size=239, crc32=0xB611C52C)
  [MQTT:file/data] Chunk 38/4437 (seq=38, size=239, crc32=0xD43D4071)
  [MQTT:file/data] Chunk 86/4437 (seq=86, size=239, crc32=0x99BBF85E)
  [MQTT:file/data] Chunk 136/4437 (seq=136, size=239, crc32=0xEEEEB3C0)
  [MQTT:file/data] Chunk 173/4437 (seq=173, size=239, crc32=0x0C6AECAC)
  ✓ Transmitted 138 chunks in window [36-173]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 38
  [RECEIVER] Sending NACK - retransmit from chunk 38

--- Window [38-175] ---
  [MQTT:file/data] Chunk 38/4437 (seq=38, size=239, crc32=0xD43D4071)
  [MQTT:file/data] Chunk 39/4437 (seq=39, size=239, crc32=0xA19EE24B)
  [MQTT:file/data] Chunk 40/4437 (seq=40, size=239, crc32=0xC02645B4)

 •⟳ 7m 24s / clear                                                                         cantabile


STARTING TESTS IN 3 SECONDS


================================================================================
              AUTOMATED STREAMING READ UNIT TESTS
================================================================================
Test Suite: Memory-Efficient Streaming File Read
Based on: Core Traceability Matrix Requirements
Date: 3 November 2025
================================================================================

Creating test files on SD card...
Initializing MicroSD with retry logic...
  Attempt 1/3: Initializing SD card driver...
✓ SD card hardware initialized
✓ Filesystem mounted successfully
SUCCESS (took 162 ms)
✓ Created STREAM_SMALL.TXT (512 bytes)
✓ Created STREAM_MEDIUM.DAT (2048 bytes)
✓ Created STREAM_LARGE.BIN (8192 bytes)
Creating extra large test file (64KB)...
✓ Created STREAM_XLARGE.BIN (65536 bytes, 64.00 KB)
Creating largest test file (4MB) - this may take a while...
  Progress: 100.0% (4096/4096 KB)
✓ Created STREAM_XXLARGE.BIN (4194304 bytes, 4.00 MB)

Test files created successfully!

=== TEST ST-1: Initialize Streaming Read ===
Target executions: 15
[ST-1 Init] Iteration 1/15TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_LARGE.BIN
  Size: 8192 bytes (8.00 KB)
  Chunks: 35
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
[ST-1 Init] Iteration 5/15TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_LARGE.BIN
  Size: 8192 bytes (8.00 KB)
  Chunks: 35
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_LARGE.BIN
  Size: 8192 bytes (8.00 KB)
  Chunks: 35
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
[ST-1 Init] Iteration 10/15TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_LARGE.BIN
  Size: 8192 bytes (8.00 KB)
  Chunks: 35
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
[ST-1 Init] Iteration 15/15TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_LARGE.BIN
  Size: 8192 bytes (8.00 KB)
  Chunks: 35
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)

✓ PASS: Initialized streaming for STREAM_LARGE.BIN (8192 bytes, 35 chunks)


=== TEST ST-2: Stream Single Chunk ===
Target executions: 10
[ST-2 Single] Iteration 1/10TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
[ST-2 Single] Iteration 5/10TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
[ST-2 Single] Iteration 10/10TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)


=== TEST ST-3: Stream Full File ===
Target executions: 10
[ST-3 Full] Iteration 1/10TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x1C613576
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x1C613576
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x1C613576
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x1C613576
[ST-3 Full] Iteration 5/10TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x1C613576
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x1C613576
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x1C613576
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x1C613576
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x1C613576
[ST-3 Full] Iteration 10/10TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x1C613576

✓ PASS: Streamed full file (3 chunks, 512 bytes, CRC32: 0x1C613576)


=== TEST ST-4: Stream Large File ===
Target executions: 5
[ST-4 Large] Iteration 1/5TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_LARGE.BIN
  Size: 8192 bytes (8.00 KB)
  Chunks: 35
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x0EE89DEA
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_LARGE.BIN
  Size: 8192 bytes (8.00 KB)
  Chunks: 35
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x0EE89DEA
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_LARGE.BIN
  Size: 8192 bytes (8.00 KB)
  Chunks: 35
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x0EE89DEA
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_LARGE.BIN
  Size: 8192 bytes (8.00 KB)
  Chunks: 35
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x0EE89DEA
[ST-4 Large] Iteration 5/5TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_LARGE.BIN
  Size: 8192 bytes (8.00 KB)
  Chunks: 35
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x0EE89DEA

✓ PASS: Streamed large file (35 chunks verified)


=== TEST ST-6: Stream XLARGE File (64KB) ===
Target executions: 3
[ST-6 XLARGE] Iteration 1/3TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_XLARGE.BIN
  Size: 65536 bytes (64.00 KB)
  Chunks: 275
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x2FC81E84
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_XLARGE.BIN
  Size: 65536 bytes (64.00 KB)
  Chunks: 275
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x2FC81E84
[ST-6 XLARGE] Iteration 3/3TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_XLARGE.BIN
  Size: 65536 bytes (64.00 KB)
  Chunks: 275
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  File CRC32 calculated: 0x2FC81E84

✓ PASS: Streamed XLARGE file (275 chunks, CRC32: 0x2FC81E84)


=== TEST ST-7: Stream XXLARGE File (4MB) ===
Target executions: 2
[ST-7 XXLARGE] Iteration 1/2TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_XXLARGE.BIN
  Size: 4194304 bytes (4096.00 KB)
  Chunks: 17550
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  Streaming 4MB file (17550 chunks)...
  Progress: 1000/17550 chunks (5.7%)
  Progress: 2000/17550 chunks (11.4%)
  Progress: 3000/17550 chunks (17.1%)
  Progress: 4000/17550 chunks (22.8%)
  Progress: 5000/17550 chunks (28.5%)
  Progress: 6000/17550 chunks (34.2%)
  Progress: 7000/17550 chunks (39.9%)
  Progress: 8000/17550 chunks (45.6%)
  Progress: 9000/17550 chunks (51.3%)
  Progress: 10000/17550 chunks (57.0%)
  Progress: 11000/17550 chunks (62.7%)
  Progress: 12000/17550 chunks (68.4%)
  Progress: 13000/17550 chunks (74.1%)
  Progress: 14000/17550 chunks (79.8%)
  Progress: 15000/17550 chunks (85.5%)
  Progress: 16000/17550 chunks (91.2%)
  Progress: 17000/17550 chunks (96.9%)
  File CRC32 calculated: 0x35646A72
[ST-7 XXLARGE] Iteration 2/2TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_XXLARGE.BIN
  Size: 4194304 bytes (4096.00 KB)
  Chunks: 17550
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
  Streaming 4MB file (17550 chunks)...
  Progress: 1000/17550 chunks (5.7%)
  Progress: 2000/17550 chunks (11.4%)
  Progress: 3000/17550 chunks (17.1%)
  Progress: 4000/17550 chunks (22.8%)
  Progress: 5000/17550 chunks (28.5%)
  Progress: 6000/17550 chunks (34.2%)
  Progress: 7000/17550 chunks (39.9%)
  Progress: 8000/17550 chunks (45.6%)
  Progress: 9000/17550 chunks (51.3%)
  Progress: 10000/17550 chunks (57.0%)
  Progress: 11000/17550 chunks (62.7%)
  Progress: 12000/17550 chunks (68.4%)
  Progress: 13000/17550 chunks (74.1%)
  Progress: 14000/17550 chunks (79.8%)
  Progress: 15000/17550 chunks (85.5%)
  Progress: 16000/17550 chunks (91.2%)
  Progress: 17000/17550 chunks (96.9%)
  File CRC32 calculated: 0x35646A72

✓ PASS: Streamed XXLARGE file (17550 chunks, CRC32: 0x35646A72)


=== TEST ST-5: Cleanup and Re-initialization ===
Target executions: 12
[ST-5 Cleanup] Iteration 1/12TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
[ST-5 Cleanup] Iteration 5/12TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
[ST-5 Cleanup] Iteration 10/12TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
[ST-5 Cleanup] Iteration 12/12TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_SMALL.TXT
  Size: 512 bytes (0.50 KB)
  Chunks: 3
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes (2.00 KB)
  Chunks: 9
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)


================================================================================
                AUTOMATED STREAMING TEST SUMMARY
================================================================================

Total test executions: 57
Passed:  57 (100.0%)
Failed:  0 (0.0%)
Errors:  0

Test Breakdown:
  ST-1 (Init streaming):       15 executions
  ST-2 (Stream single chunk):  10 executions
  ST-3 (Stream full file):     10 executions
  ST-4 (Stream large file):    5 executions
  ST-6 (Stream XLARGE file):   3 executions
  ST-7 (Stream XXLARGE file):  2 executions
  ST-5 (Cleanup):              12 executions
  ─────────────────────────────────────
  Total:                       52 executions

✓ ALL TESTS PASSED!

================================================================================

Tests complete. System will remain active for monitoring.
Press Ctrl+C to exit.
```