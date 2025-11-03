```
                                                                                                     drwxr-xr-x@    - cantabile  3 Nov 12:40  fs
                  drwxr-xr-x@    - cantabile  2 Nov 11:27  generated
                                                                     drwxr-xr-x@    - cantabile  3 Nov 12:40  pico-sdk
                                                                                                                       .rw-r--r--@ 2.6k cantabile  3 Nov 12:40  cmake_install.cmake
                                                     .rw-r--r--@  29k cantabile  2 Nov 11:27  CMakeCache.txt
                                                                                                             .rw-r--r--@ 8.5M cantabile  3 Nov 12:40  compile_commands.json
                                             .rw-r--r--@ 388k cantabile  3 Nov 12:40  Makefile
                                                                                               .rwxr-xr-x@ 381k cantabile  3 Nov 12:00  mqtt-sn-pico-client.bin
                                 .rw-r--r--@ 2.5M cantabile  3 Nov 12:00  mqtt-sn-pico-client.dis
                                                                                                  .rwxr-xr-x@ 2.0M cantabile  3 Nov 12:00  mqtt-sn-pico-client.elf
                                    .rw-r--r--@ 841k cantabile  3 Nov 12:00  mqtt-sn-pico-client.elf.map
                                                                                                         .rw-r--r--@ 1.1M cantabile  3 Nov 12:00 󱊧 mqtt-sn-pico-client.hex
                                           .rw-r--r--@ 763k cantabile  3 Nov 12:00  mqtt-sn-pico-client.uf2
                                                                                                            .rw-r--r--@   60 cantabile  2 Nov 11:27  pico_flash_region.ld

 • mqtt-sn-pico-client/build make -j12                                                                      cantabile ht
[  0%] Built target bs2_default
                               [  0%] Built target cyw43_driver_picow_cyw43_bus_pio_spi_pio_h
                                                                                             [  0%] Built target bs2_default_library
     [  6%] Built target microsd_driver
                                       [ 11%] Built target fs_lib
                                                                 [ 19%] Built target automated_microsd_test
                                                                                                           [ 25%] Built target large_file_test
               [ 32%] Built target microsd_demo
                                               [ 41%] Built target chunk_write_example
                                                                                      [ 60%] Built target file_transfer_demo
                                                                                                                            [ 62%] Built target automated_chunk_test
                                     [ 62%] Built target streaming_test
                                                                       [ 69%] Built target data_frame_demo
                                                                                                          [ 76%] Built target serialize_test
             [ 76%] Building C object fs/CMakeFiles/automated_streaming_test.dir/tests/automated_streaming_test.c.o
                                                                                                                   [ 92%] Built target mqtt-sn-pico-client
                           [ 92%] Linking CXX executable automated_streaming_test.elf
                                                                                     [100%] Built target automated_streaming_test

 • mqtt-sn-pico-client/build clear                                                                          cantabile ht


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
  Attempt 1/3: [MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:ERROR] Card did not enter idle state (response: 0x1F)
FAILED (took 62 ms)
  Waiting 2000 ms before retry...
  Attempt 2/3: [MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
SUCCESS (took 68 ms)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Volume dirty bit is set - clearing it
[MICROSD_DRV_v1.0.0:INFO] Volume dirty bit cleared successfully
[MICROSD_DRV_v1.0.0:INFO] Creating file: STREAM_SMALL.TXT (512 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 512 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=2048
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=3, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=3772826305, DataLength=224
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=3772827076, DataLength=224
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 7
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 512 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'STREAM_SMALL.TXT'
[MICROSD_DRV_v1.0.0:INFO] Cleaning corrupted file entry 3 (invalid cluster 3772826305)
[MICROSD_DRV_v1.0.0:INFO] Cleaning corrupted file entry 6 (invalid cluster 3772827076)
[MICROSD_DRV_v1.0.0:INFO] Cleaned 6 corrupted directory entries
[MICROSD_DRV_v1.0.0:INFO] Cleaned directory written successfully
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Volume Label
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Allocation Bitmap
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Up-case Table
[MICROSD_DRV_v1.0.0:INFO] Entry 3: File (secondary_count=3)
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 5: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 6: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 7: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'STREAM_SMALL.TXT' created successfully
✓ Created STREAM_SMALL.TXT (512 bytes)
[MICROSD_DRV_v1.0.0:INFO] Creating file: STREAM_MEDIUM.DAT (2048 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 2048 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=2048
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=3, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 7: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 11
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 2048 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'STREAM_MEDIUM.DAT'
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Volume Label
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Allocation Bitmap
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Up-case Table
[MICROSD_DRV_v1.0.0:INFO] Entry 3: File (secondary_count=3)
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 5: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 6: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 7: File (secondary_count=3)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 9: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 10: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 11: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'STREAM_MEDIUM.DAT' created successfully
✓ Created STREAM_MEDIUM.DAT (2048 bytes)
[MICROSD_DRV_v1.0.0:INFO] Creating file: STREAM_LARGE.BIN (8192 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 8192 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=2048
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=3, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 48
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 8192 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Volume Label
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Allocation Bitmap
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Up-case Table
[MICROSD_DRV_v1.0.0:INFO] Entry 3: File (secondary_count=3)
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 5: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 6: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 7: File (secondary_count=3)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 9: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 10: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 11: File (secondary_count=3)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 13: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 14: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'STREAM_LARGE.BIN' created successfully
✓ Created STREAM_LARGE.BIN (8192 bytes)

Test files created successfully!

=== TEST ST-1: Initialize Streaming Read ===
Target executions: 15
[ST-1 Init] Iteration 1/15[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 4096 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
Streaming read initialized:
  File: STREAM_LARGE.BIN
  Size: 8192 bytes
  Chunks: 35
  CRC: 0x4629
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[ST-1 Init] Iteration 5/15[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 4096 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
Streaming read initialized:
  File: STREAM_LARGE.BIN
  Size: 8192 bytes
  Chunks: 35
  CRC: 0x4629
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 4096 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
Streaming read initialized:
  File: STREAM_LARGE.BIN
  Size: 8192 bytes
  Chunks: 35
  CRC: 0x4629
[ST-1 Init] Iteration 10/15[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 4096 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
Streaming read initialized:
  File: STREAM_LARGE.BIN
  Size: 8192 bytes
  Chunks: 35
  CRC: 0x4629
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[ST-1 Init] Iteration 15/15[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 4096 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
Streaming read initialized:
  File: STREAM_LARGE.BIN
  Size: 8192 bytes
  Chunks: 35
  CRC: 0x4629

✓ PASS: Initialized streaming for STREAM_LARGE.BIN (8192 bytes, 35 chunks)


=== TEST ST-2: Stream Single Chunk ===
Target executions: 10
[ST-2 Single] Iteration 1/10[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_MEDIUM.DAT' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 3 from file 'STREAM_MEDIUM.DAT' (offset 474, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 3 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 4 from file 'STREAM_MEDIUM.DAT' (offset 711, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 4 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 5 from file 'STREAM_MEDIUM.DAT' (offset 948, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 5 (237 bytes)
[ST-2 Single] Iteration 5/10[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 6 from file 'STREAM_MEDIUM.DAT' (offset 1185, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 6 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 7 from file 'STREAM_MEDIUM.DAT' (offset 1422, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 7 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 8 from file 'STREAM_MEDIUM.DAT' (offset 1659, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 8 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 9 from file 'STREAM_MEDIUM.DAT' (offset 1896, size 152)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 9 (152 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_MEDIUM.DAT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[ST-2 Single] Iteration 10/10[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_MEDIUM.DAT' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)


=== TEST ST-3: Stream Full File ===
Target executions: 10
[ST-3 Full] Iteration 1/10[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_SMALL.TXT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_SMALL.TXT' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 3 from file 'STREAM_SMALL.TXT' (offset 474, size 38)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 3 (38 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_SMALL.TXT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_SMALL.TXT' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 3 from file 'STREAM_SMALL.TXT' (offset 474, size 38)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 3 (38 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_SMALL.TXT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_SMALL.TXT' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 3 from file 'STREAM_SMALL.TXT' (offset 474, size 38)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 3 (38 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_SMALL.TXT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_SMALL.TXT' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 3 from file 'STREAM_SMALL.TXT' (offset 474, size 38)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 3 (38 bytes)
[ST-3 Full] Iteration 5/10[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_SMALL.TXT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_SMALL.TXT' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 3 from file 'STREAM_SMALL.TXT' (offset 474, size 38)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 3 (38 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_SMALL.TXT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_SMALL.TXT' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 3 from file 'STREAM_SMALL.TXT' (offset 474, size 38)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 3 (38 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_SMALL.TXT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_SMALL.TXT' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 3 from file 'STREAM_SMALL.TXT' (offset 474, size 38)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 3 (38 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_SMALL.TXT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_SMALL.TXT' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 3 from file 'STREAM_SMALL.TXT' (offset 474, size 38)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 3 (38 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_SMALL.TXT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_SMALL.TXT' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 3 from file 'STREAM_SMALL.TXT' (offset 474, size 38)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 3 (38 bytes)
[ST-3 Full] Iteration 10/10[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_SMALL.TXT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_SMALL.TXT' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 3 from file 'STREAM_SMALL.TXT' (offset 474, size 38)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 3 (38 bytes)

✓ PASS: Streamed full file (3 chunks, 512 bytes)


=== TEST ST-4: Stream Large File ===
Target executions: 5
[ST-4 Large] Iteration 1/5[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 4096 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
Streaming read initialized:
  File: STREAM_LARGE.BIN
  Size: 8192 bytes
  Chunks: 35
  CRC: 0x4629
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_LARGE.BIN' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_LARGE.BIN' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 3 from file 'STREAM_LARGE.BIN' (offset 474, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 3 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 4 from file 'STREAM_LARGE.BIN' (offset 711, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 4 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 5 from file 'STREAM_LARGE.BIN' (offset 948, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 5 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 6 from file 'STREAM_LARGE.BIN' (offset 1185, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 6 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 7 from file 'STREAM_LARGE.BIN' (offset 1422, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 7 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 8 from file 'STREAM_LARGE.BIN' (offset 1659, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 8 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 9 from file 'STREAM_LARGE.BIN' (offset 1896, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 9 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 10 from file 'STREAM_LARGE.BIN' (offset 2133, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 10 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 11 from file 'STREAM_LARGE.BIN' (offset 2370, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 11 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 12 from file 'STREAM_LARGE.BIN' (offset 2607, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 12 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 13 from file 'STREAM_LARGE.BIN' (offset 2844, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 13 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 14 from file 'STREAM_LARGE.BIN' (offset 3081, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 14 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 15 from file 'STREAM_LARGE.BIN' (offset 3318, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 15 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 16 from file 'STREAM_LARGE.BIN' (offset 3555, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 16 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 17 from file 'STREAM_LARGE.BIN' (offset 3792, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 17 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 18 from file 'STREAM_LARGE.BIN' (offset 4029, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 18 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 19 from file 'STREAM_LARGE.BIN' (offset 4266, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 19 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 20 from file 'STREAM_LARGE.BIN' (offset 4503, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 20 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 21 from file 'STREAM_LARGE.BIN' (offset 4740, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 21 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 22 from file 'STREAM_LARGE.BIN' (offset 4977, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 22 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 23 from file 'STREAM_LARGE.BIN' (offset 5214, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 23 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 24 from file 'STREAM_LARGE.BIN' (offset 5451, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 24 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 25 from file 'STREAM_LARGE.BIN' (offset 5688, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 25 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 26 from file 'STREAM_LARGE.BIN' (offset 5925, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 26 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 27 from file 'STREAM_LARGE.BIN' (offset 6162, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 27 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 28 from file 'STREAM_LARGE.BIN' (offset 6399, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 28 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 29 from file 'STREAM_LARGE.BIN' (offset 6636, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 29 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 30 from file 'STREAM_LARGE.BIN' (offset 6873, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 30 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 31 from file 'STREAM_LARGE.BIN' (offset 7110, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 31 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 32 from file 'STREAM_LARGE.BIN' (offset 7347, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 32 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 33 from file 'STREAM_LARGE.BIN' (offset 7584, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 33 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 34 from file 'STREAM_LARGE.BIN' (offset 7821, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 34 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 35 from file 'STREAM_LARGE.BIN' (offset 8058, size 134)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 35 (134 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 4096 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
Streaming read initialized:
  File: STREAM_LARGE.BIN
  Size: 8192 bytes
  Chunks: 35
  CRC: 0x4629
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_LARGE.BIN' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_LARGE.BIN' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 3 from file 'STREAM_LARGE.BIN' (offset 474, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 3 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 4 from file 'STREAM_LARGE.BIN' (offset 711, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 4 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 5 from file 'STREAM_LARGE.BIN' (offset 948, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 5 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 6 from file 'STREAM_LARGE.BIN' (offset 1185, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 6 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 7 from file 'STREAM_LARGE.BIN' (offset 1422, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 7 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 8 from file 'STREAM_LARGE.BIN' (offset 1659, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 8 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 9 from file 'STREAM_LARGE.BIN' (offset 1896, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 9 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 10 from file 'STREAM_LARGE.BIN' (offset 2133, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 10 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 11 from file 'STREAM_LARGE.BIN' (offset 2370, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 11 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 12 from file 'STREAM_LARGE.BIN' (offset 2607, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 12 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 13 from file 'STREAM_LARGE.BIN' (offset 2844, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 13 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 14 from file 'STREAM_LARGE.BIN' (offset 3081, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 14 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 15 from file 'STREAM_LARGE.BIN' (offset 3318, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 15 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 16 from file 'STREAM_LARGE.BIN' (offset 3555, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 16 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 17 from file 'STREAM_LARGE.BIN' (offset 3792, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 17 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 18 from file 'STREAM_LARGE.BIN' (offset 4029, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 18 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 19 from file 'STREAM_LARGE.BIN' (offset 4266, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 19 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 20 from file 'STREAM_LARGE.BIN' (offset 4503, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 20 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 21 from file 'STREAM_LARGE.BIN' (offset 4740, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 21 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 22 from file 'STREAM_LARGE.BIN' (offset 4977, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 22 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 23 from file 'STREAM_LARGE.BIN' (offset 5214, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 23 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 24 from file 'STREAM_LARGE.BIN' (offset 5451, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 24 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 25 from file 'STREAM_LARGE.BIN' (offset 5688, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 25 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 26 from file 'STREAM_LARGE.BIN' (offset 5925, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 26 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 27 from file 'STREAM_LARGE.BIN' (offset 6162, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 27 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 28 from file 'STREAM_LARGE.BIN' (offset 6399, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 28 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 29 from file 'STREAM_LARGE.BIN' (offset 6636, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 29 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 30 from file 'STREAM_LARGE.BIN' (offset 6873, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 30 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 31 from file 'STREAM_LARGE.BIN' (offset 7110, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 31 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 32 from file 'STREAM_LARGE.BIN' (offset 7347, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 32 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 33 from file 'STREAM_LARGE.BIN' (offset 7584, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 33 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 34 from file 'STREAM_LARGE.BIN' (offset 7821, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 34 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 35 from file 'STREAM_LARGE.BIN' (offset 8058, size 134)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 35 (134 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 4096 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
Streaming read initialized:
  File: STREAM_LARGE.BIN
  Size: 8192 bytes
  Chunks: 35
  CRC: 0x4629
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_LARGE.BIN' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_LARGE.BIN' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 3 from file 'STREAM_LARGE.BIN' (offset 474, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 3 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 4 from file 'STREAM_LARGE.BIN' (offset 711, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 4 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 5 from file 'STREAM_LARGE.BIN' (offset 948, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 5 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 6 from file 'STREAM_LARGE.BIN' (offset 1185, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 6 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 7 from file 'STREAM_LARGE.BIN' (offset 1422, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 7 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 8 from file 'STREAM_LARGE.BIN' (offset 1659, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 8 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 9 from file 'STREAM_LARGE.BIN' (offset 1896, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 9 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 10 from file 'STREAM_LARGE.BIN' (offset 2133, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 10 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 11 from file 'STREAM_LARGE.BIN' (offset 2370, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 11 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 12 from file 'STREAM_LARGE.BIN' (offset 2607, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 12 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 13 from file 'STREAM_LARGE.BIN' (offset 2844, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 13 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 14 from file 'STREAM_LARGE.BIN' (offset 3081, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 14 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 15 from file 'STREAM_LARGE.BIN' (offset 3318, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 15 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 16 from file 'STREAM_LARGE.BIN' (offset 3555, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 16 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 17 from file 'STREAM_LARGE.BIN' (offset 3792, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 17 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 18 from file 'STREAM_LARGE.BIN' (offset 4029, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 18 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 19 from file 'STREAM_LARGE.BIN' (offset 4266, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 19 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 20 from file 'STREAM_LARGE.BIN' (offset 4503, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 20 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 21 from file 'STREAM_LARGE.BIN' (offset 4740, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 21 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 22 from file 'STREAM_LARGE.BIN' (offset 4977, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 22 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 23 from file 'STREAM_LARGE.BIN' (offset 5214, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 23 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 24 from file 'STREAM_LARGE.BIN' (offset 5451, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 24 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 25 from file 'STREAM_LARGE.BIN' (offset 5688, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 25 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 26 from file 'STREAM_LARGE.BIN' (offset 5925, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 26 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 27 from file 'STREAM_LARGE.BIN' (offset 6162, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 27 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 28 from file 'STREAM_LARGE.BIN' (offset 6399, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 28 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 29 from file 'STREAM_LARGE.BIN' (offset 6636, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 29 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 30 from file 'STREAM_LARGE.BIN' (offset 6873, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 30 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 31 from file 'STREAM_LARGE.BIN' (offset 7110, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 31 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 32 from file 'STREAM_LARGE.BIN' (offset 7347, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 32 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 33 from file 'STREAM_LARGE.BIN' (offset 7584, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 33 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 34 from file 'STREAM_LARGE.BIN' (offset 7821, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 34 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 35 from file 'STREAM_LARGE.BIN' (offset 8058, size 134)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 35 (134 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 4096 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
Streaming read initialized:
  File: STREAM_LARGE.BIN
  Size: 8192 bytes
  Chunks: 35
  CRC: 0x4629
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_LARGE.BIN' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_LARGE.BIN' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 3 from file 'STREAM_LARGE.BIN' (offset 474, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 3 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 4 from file 'STREAM_LARGE.BIN' (offset 711, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 4 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 5 from file 'STREAM_LARGE.BIN' (offset 948, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 5 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 6 from file 'STREAM_LARGE.BIN' (offset 1185, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 6 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 7 from file 'STREAM_LARGE.BIN' (offset 1422, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 7 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 8 from file 'STREAM_LARGE.BIN' (offset 1659, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 8 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 9 from file 'STREAM_LARGE.BIN' (offset 1896, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 9 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 10 from file 'STREAM_LARGE.BIN' (offset 2133, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 10 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 11 from file 'STREAM_LARGE.BIN' (offset 2370, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 11 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 12 from file 'STREAM_LARGE.BIN' (offset 2607, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 12 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 13 from file 'STREAM_LARGE.BIN' (offset 2844, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 13 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 14 from file 'STREAM_LARGE.BIN' (offset 3081, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 14 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 15 from file 'STREAM_LARGE.BIN' (offset 3318, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 15 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 16 from file 'STREAM_LARGE.BIN' (offset 3555, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 16 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 17 from file 'STREAM_LARGE.BIN' (offset 3792, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 17 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 18 from file 'STREAM_LARGE.BIN' (offset 4029, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 18 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 19 from file 'STREAM_LARGE.BIN' (offset 4266, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 19 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 20 from file 'STREAM_LARGE.BIN' (offset 4503, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 20 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 21 from file 'STREAM_LARGE.BIN' (offset 4740, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 21 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 22 from file 'STREAM_LARGE.BIN' (offset 4977, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 22 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 23 from file 'STREAM_LARGE.BIN' (offset 5214, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 23 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 24 from file 'STREAM_LARGE.BIN' (offset 5451, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 24 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 25 from file 'STREAM_LARGE.BIN' (offset 5688, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 25 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 26 from file 'STREAM_LARGE.BIN' (offset 5925, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 26 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 27 from file 'STREAM_LARGE.BIN' (offset 6162, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 27 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 28 from file 'STREAM_LARGE.BIN' (offset 6399, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 28 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 29 from file 'STREAM_LARGE.BIN' (offset 6636, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 29 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 30 from file 'STREAM_LARGE.BIN' (offset 6873, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 30 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 31 from file 'STREAM_LARGE.BIN' (offset 7110, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 31 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 32 from file 'STREAM_LARGE.BIN' (offset 7347, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 32 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 33 from file 'STREAM_LARGE.BIN' (offset 7584, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 33 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 34 from file 'STREAM_LARGE.BIN' (offset 7821, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 34 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 35 from file 'STREAM_LARGE.BIN' (offset 8058, size 134)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 35 (134 bytes)
[ST-4 Large] Iteration 5/5[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 4096 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_LARGE.BIN
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 11
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_LARGE.BIN' at cluster 48, size 8192 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 8192 bytes from file 'STREAM_LARGE.BIN'
Streaming read initialized:
  File: STREAM_LARGE.BIN
  Size: 8192 bytes
  Chunks: 35
  CRC: 0x4629
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_LARGE.BIN' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 2 from file 'STREAM_LARGE.BIN' (offset 237, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 2 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 3 from file 'STREAM_LARGE.BIN' (offset 474, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 3 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 4 from file 'STREAM_LARGE.BIN' (offset 711, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 4 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 5 from file 'STREAM_LARGE.BIN' (offset 948, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 5 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 6 from file 'STREAM_LARGE.BIN' (offset 1185, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 6 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 7 from file 'STREAM_LARGE.BIN' (offset 1422, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 7 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 8 from file 'STREAM_LARGE.BIN' (offset 1659, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 8 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 9 from file 'STREAM_LARGE.BIN' (offset 1896, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 9 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 10 from file 'STREAM_LARGE.BIN' (offset 2133, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 10 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 11 from file 'STREAM_LARGE.BIN' (offset 2370, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 11 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 12 from file 'STREAM_LARGE.BIN' (offset 2607, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 12 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 13 from file 'STREAM_LARGE.BIN' (offset 2844, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 13 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 14 from file 'STREAM_LARGE.BIN' (offset 3081, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 14 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 15 from file 'STREAM_LARGE.BIN' (offset 3318, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 15 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 16 from file 'STREAM_LARGE.BIN' (offset 3555, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 16 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 17 from file 'STREAM_LARGE.BIN' (offset 3792, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 17 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 18 from file 'STREAM_LARGE.BIN' (offset 4029, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 18 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 19 from file 'STREAM_LARGE.BIN' (offset 4266, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 19 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 20 from file 'STREAM_LARGE.BIN' (offset 4503, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 20 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 21 from file 'STREAM_LARGE.BIN' (offset 4740, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 21 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 22 from file 'STREAM_LARGE.BIN' (offset 4977, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 22 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 23 from file 'STREAM_LARGE.BIN' (offset 5214, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 23 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 24 from file 'STREAM_LARGE.BIN' (offset 5451, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 24 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 25 from file 'STREAM_LARGE.BIN' (offset 5688, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 25 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 26 from file 'STREAM_LARGE.BIN' (offset 5925, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 26 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 27 from file 'STREAM_LARGE.BIN' (offset 6162, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 27 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 28 from file 'STREAM_LARGE.BIN' (offset 6399, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 28 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 29 from file 'STREAM_LARGE.BIN' (offset 6636, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 29 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 30 from file 'STREAM_LARGE.BIN' (offset 6873, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 30 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 31 from file 'STREAM_LARGE.BIN' (offset 7110, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 31 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 32 from file 'STREAM_LARGE.BIN' (offset 7347, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 32 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 33 from file 'STREAM_LARGE.BIN' (offset 7584, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 33 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 34 from file 'STREAM_LARGE.BIN' (offset 7821, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 34 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 35 from file 'STREAM_LARGE.BIN' (offset 8058, size 134)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 35 (134 bytes)

✓ PASS: Streamed large file (35 chunks verified)


=== TEST ST-5: Cleanup and Re-initialization ===
Target executions: 12
[ST-5 Cleanup] Iteration 1/12[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_MEDIUM.DAT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_MEDIUM.DAT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_MEDIUM.DAT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_MEDIUM.DAT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[ST-5 Cleanup] Iteration 5/12[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_MEDIUM.DAT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_MEDIUM.DAT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_MEDIUM.DAT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_MEDIUM.DAT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_MEDIUM.DAT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[ST-5 Cleanup] Iteration 10/12[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_MEDIUM.DAT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_MEDIUM.DAT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)
[ST-5 Cleanup] Iteration 12/12[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_SMALL.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_SMALL.TXT' at cluster 7, size 512 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 512 bytes from file 'STREAM_SMALL.TXT'
Streaming read initialized:
  File: STREAM_SMALL.TXT
  Size: 512 bytes
  Chunks: 3
  CRC: 0x56EE
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 16384
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Reading file: STREAM_MEDIUM.DAT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 4 (sector 16768)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 4, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 4, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 4, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 4, sector offset 0, entry 7
[MICROSD_DRV_v1.0.0:INFO] Found file 'STREAM_MEDIUM.DAT' at cluster 11, size 2048 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 2048 bytes from file 'STREAM_MEDIUM.DAT'
Streaming read initialized:
  File: STREAM_MEDIUM.DAT
  Size: 2048 bytes
  Chunks: 9
  CRC: 0xA48C
[MICROSD_DRV_v1.0.0:INFO] Reading chunk 1 from file 'STREAM_MEDIUM.DAT' (offset 0, size 237)
[MICROSD_DRV_v1.0.0:INFO] Successfully read chunk 1 (237 bytes)


================================================================================
                AUTOMATED STREAMING TEST SUMMARY
================================================================================

Total test executions: 52
Passed:  52 (100.0%)
Failed:  0 (0.0%)
Errors:  0

Test Breakdown:
  ST-1 (Init streaming):       15 executions
  ST-2 (Stream single chunk):  10 executions
  ST-3 (Stream full file):     10 executions
  ST-4 (Stream large file):    5 executions
  ST-5 (Cleanup):              12 executions
  ─────────────────────────────────────
  Total:                       52 executions

✓ ALL TESTS PASSED!

================================================================================

Tests complete. System will remain active for monitoring.
Press Ctrl+C to exit.

```