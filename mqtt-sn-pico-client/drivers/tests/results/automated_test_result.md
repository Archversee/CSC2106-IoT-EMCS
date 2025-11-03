```
clear
Last login: Mon Nov  3 11:43:15 on ttys003

 • mqtt-sn-pico-client/build clear                                        cantabile ht
STARTING TESTS IN 3 SECONDS
STARTING TESTS IN 3 SECONDS
STARTING TESTS IN 3 SECONDS
STARTING TESTS IN 3 SECONDS


================================================================================
              AUTOMATED MICROSD DRIVER UNIT TESTS
================================================================================
Test Suite: MicroSD Driver Verification
Based on: Core Traceability Matrix Requirements
Date: 3 November 2025
================================================================================

=== TEST IT-2: MicroSD Initialization ===
Target executions: 20

[IT-2 Init] Iteration 1/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:ERROR] Card did not enter idle state (response: 0x1F)
ERROR: Initialization failed
  Waiting 2000 ms before retry...
  Initialization attempt 2/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
[MICROSD_DRV_v1.0.0:INFO] Volume dirty bit is set - clearing it
[MICROSD_DRV_v1.0.0:INFO] Volume dirty bit cleared successfully
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 2/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 3/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 4/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 5/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 6/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 7/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 8/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 9/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 10/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 11/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 12/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 13/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 14/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 15/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 16/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 17/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 18/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 19/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

[IT-2 Init] Iteration 20/20
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition
✓ PASS: MicroSD and filesystem initialized successfully

=== TEST UT-5: Block I/O ===
Target executions: 50
  Initialization attempt 1/3...
[MICROSD_DRV_v1.0.0:INFO] Initializing driver INF2004-CS31-MICROSD-2024
[MICROSD_DRV_v1.0.0:INFO] Card is in idle state. Sending CMD8...
[MICROSD_DRV_v1.0.0:INFO] Sending ACMD41 to initialize...
[MICROSD_DRV_v1.0.0:INFO] Card initialized successfully!
[MICROSD_DRV_v1.0.0:INFO] SPI speed increased to 10MHz
  ✓ Initialized in 68 ms

[UT-5 Block I/O] Iteration 1/50
✓ PASS: Block 1001 read/write verified

[UT-5 Block I/O] Iteration 2/50
✓ PASS: Block 1002 read/write verified

[UT-5 Block I/O] Iteration 3/50
✓ PASS: Block 1003 read/write verified

[UT-5 Block I/O] Iteration 4/50
✓ PASS: Block 1004 read/write verified

[UT-5 Block I/O] Iteration 5/50
✓ PASS: Block 1005 read/write verified

[UT-5 Block I/O] Iteration 6/50
✓ PASS: Block 1006 read/write verified

[UT-5 Block I/O] Iteration 7/50
✓ PASS: Block 1007 read/write verified

[UT-5 Block I/O] Iteration 8/50
✓ PASS: Block 1008 read/write verified

[UT-5 Block I/O] Iteration 9/50
✓ PASS: Block 1009 read/write verified

[UT-5 Block I/O] Iteration 10/50
✓ PASS: Block 1010 read/write verified

[UT-5 Block I/O] Iteration 11/50
✓ PASS: Block 1011 read/write verified

[UT-5 Block I/O] Iteration 12/50
✓ PASS: Block 1012 read/write verified

[UT-5 Block I/O] Iteration 13/50
✓ PASS: Block 1013 read/write verified

[UT-5 Block I/O] Iteration 14/50
✓ PASS: Block 1014 read/write verified

[UT-5 Block I/O] Iteration 15/50
✓ PASS: Block 1015 read/write verified

[UT-5 Block I/O] Iteration 16/50
✓ PASS: Block 1016 read/write verified

[UT-5 Block I/O] Iteration 17/50
✓ PASS: Block 1017 read/write verified

[UT-5 Block I/O] Iteration 18/50
✓ PASS: Block 1018 read/write verified

[UT-5 Block I/O] Iteration 19/50
✓ PASS: Block 1019 read/write verified

[UT-5 Block I/O] Iteration 20/50
✓ PASS: Block 1020 read/write verified

[UT-5 Block I/O] Iteration 21/50
✓ PASS: Block 1021 read/write verified

[UT-5 Block I/O] Iteration 22/50
✓ PASS: Block 1022 read/write verified

[UT-5 Block I/O] Iteration 23/50
✓ PASS: Block 1023 read/write verified

[UT-5 Block I/O] Iteration 24/50
✓ PASS: Block 1024 read/write verified

[UT-5 Block I/O] Iteration 25/50
✓ PASS: Block 1025 read/write verified

[UT-5 Block I/O] Iteration 26/50
✓ PASS: Block 1026 read/write verified

[UT-5 Block I/O] Iteration 27/50
✓ PASS: Block 1027 read/write verified

[UT-5 Block I/O] Iteration 28/50
✓ PASS: Block 1028 read/write verified

[UT-5 Block I/O] Iteration 29/50
✓ PASS: Block 1029 read/write verified

[UT-5 Block I/O] Iteration 30/50
✓ PASS: Block 1030 read/write verified

[UT-5 Block I/O] Iteration 31/50
✓ PASS: Block 1031 read/write verified

[UT-5 Block I/O] Iteration 32/50
✓ PASS: Block 1032 read/write verified

[UT-5 Block I/O] Iteration 33/50
✓ PASS: Block 1033 read/write verified

[UT-5 Block I/O] Iteration 34/50
✓ PASS: Block 1034 read/write verified

[UT-5 Block I/O] Iteration 35/50
✓ PASS: Block 1035 read/write verified

[UT-5 Block I/O] Iteration 36/50
✓ PASS: Block 1036 read/write verified

[UT-5 Block I/O] Iteration 37/50
✓ PASS: Block 1037 read/write verified

[UT-5 Block I/O] Iteration 38/50
✓ PASS: Block 1038 read/write verified

[UT-5 Block I/O] Iteration 39/50
✓ PASS: Block 1039 read/write verified

[UT-5 Block I/O] Iteration 40/50
✓ PASS: Block 1040 read/write verified

[UT-5 Block I/O] Iteration 41/50
✓ PASS: Block 1041 read/write verified

[UT-5 Block I/O] Iteration 42/50
✓ PASS: Block 1042 read/write verified

[UT-5 Block I/O] Iteration 43/50
✓ PASS: Block 1043 read/write verified

[UT-5 Block I/O] Iteration 44/50
✓ PASS: Block 1044 read/write verified

[UT-5 Block I/O] Iteration 45/50
✓ PASS: Block 1045 read/write verified

[UT-5 Block I/O] Iteration 46/50
✓ PASS: Block 1046 read/write verified

[UT-5 Block I/O] Iteration 47/50
✓ PASS: Block 1047 read/write verified

[UT-5 Block I/O] Iteration 48/50
✓ PASS: Block 1048 read/write verified

[UT-5 Block I/O] Iteration 49/50
✓ PASS: Block 1049 read/write verified

[UT-5 Block I/O] Iteration 50/50
✓ PASS: Block 1050 read/write verified

=== TEST UT-5: Byte I/O (RMW) ===
Target executions: 30

[UT-5 Byte I/O] Iteration 1/30
✓ PASS: Byte 0xA1 at 0x0007D00A verified

[UT-5 Byte I/O] Iteration 2/30
✓ PASS: Byte 0xA2 at 0x0007D014 verified

[UT-5 Byte I/O] Iteration 3/30
✓ PASS: Byte 0xA3 at 0x0007D01E verified

[UT-5 Byte I/O] Iteration 4/30
✓ PASS: Byte 0xA4 at 0x0007D028 verified

[UT-5 Byte I/O] Iteration 5/30
✓ PASS: Byte 0xA5 at 0x0007D032 verified

[UT-5 Byte I/O] Iteration 6/30
✓ PASS: Byte 0xA6 at 0x0007D03C verified

[UT-5 Byte I/O] Iteration 7/30
✓ PASS: Byte 0xA7 at 0x0007D046 verified

[UT-5 Byte I/O] Iteration 8/30
✓ PASS: Byte 0xA8 at 0x0007D050 verified

[UT-5 Byte I/O] Iteration 9/30
✓ PASS: Byte 0xA9 at 0x0007D05A verified

[UT-5 Byte I/O] Iteration 10/30
✓ PASS: Byte 0xAA at 0x0007D064 verified

[UT-5 Byte I/O] Iteration 11/30
✓ PASS: Byte 0xAB at 0x0007D06E verified

[UT-5 Byte I/O] Iteration 12/30
✓ PASS: Byte 0xAC at 0x0007D078 verified

[UT-5 Byte I/O] Iteration 13/30
✓ PASS: Byte 0xAD at 0x0007D082 verified

[UT-5 Byte I/O] Iteration 14/30
✓ PASS: Byte 0xAE at 0x0007D08C verified

[UT-5 Byte I/O] Iteration 15/30
✓ PASS: Byte 0xAF at 0x0007D096 verified

[UT-5 Byte I/O] Iteration 16/30
✓ PASS: Byte 0xA0 at 0x0007D0A0 verified

[UT-5 Byte I/O] Iteration 17/30
✓ PASS: Byte 0xA1 at 0x0007D0AA verified

[UT-5 Byte I/O] Iteration 18/30
✓ PASS: Byte 0xA2 at 0x0007D0B4 verified

[UT-5 Byte I/O] Iteration 19/30
✓ PASS: Byte 0xA3 at 0x0007D0BE verified

[UT-5 Byte I/O] Iteration 20/30
✓ PASS: Byte 0xA4 at 0x0007D0C8 verified

[UT-5 Byte I/O] Iteration 21/30
✓ PASS: Byte 0xA5 at 0x0007D0D2 verified

[UT-5 Byte I/O] Iteration 22/30
✓ PASS: Byte 0xA6 at 0x0007D0DC verified

[UT-5 Byte I/O] Iteration 23/30
✓ PASS: Byte 0xA7 at 0x0007D0E6 verified

[UT-5 Byte I/O] Iteration 24/30
✓ PASS: Byte 0xA8 at 0x0007D0F0 verified

[UT-5 Byte I/O] Iteration 25/30
✓ PASS: Byte 0xA9 at 0x0007D0FA verified

[UT-5 Byte I/O] Iteration 26/30
✓ PASS: Byte 0xAA at 0x0007D104 verified

[UT-5 Byte I/O] Iteration 27/30
✓ PASS: Byte 0xAB at 0x0007D10E verified

[UT-5 Byte I/O] Iteration 28/30
✓ PASS: Byte 0xAC at 0x0007D118 verified

[UT-5 Byte I/O] Iteration 29/30
✓ PASS: Byte 0xAD at 0x0007D122 verified

[UT-5 Byte I/O] Iteration 30/30
✓ PASS: Byte 0xAE at 0x0007D12C verified

=== TEST IT-2: File Operations ===
Target executions: 25
[MICROSD_DRV_v1.0.0:INFO] MBR with partition table detected
[MICROSD_DRV_v1.0.0:INFO] Found exFAT partition at sector 8192
[MICROSD_DRV_v1.0.0:INFO] exFAT filesystem confirmed in partition

[IT-2 File Ops] Iteration 1/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_001.TXT (154 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 154 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=3772794437, DataLength=224
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=3772795208, DataLength=224
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 8
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 154 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_001.TXT'
[MICROSD_DRV_v1.0.0:INFO] Cleaning corrupted file entry 3 (invalid cluster 3772794437)
[MICROSD_DRV_v1.0.0:INFO] Cleaning corrupted file entry 6 (invalid cluster 3772795208)
[MICROSD_DRV_v1.0.0:INFO] Cleaned 6 corrupted directory entries
[MICROSD_DRV_v1.0.0:INFO] Cleaned directory written successfully
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Volume Label
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Allocation Bitmap
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Up-case Table
[MICROSD_DRV_v1.0.0:INFO] Entry 3: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 5: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 6: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_001.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_001.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_001.TXT' at cluster 8, size 154 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 154 bytes from file 'TEST_001.TXT'
✓ PASS: File TEST_001.TXT created and verified (154 bytes)

[IT-2 File Ops] Iteration 2/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_002.TXT (154 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 154 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 12
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 154 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_002.TXT'
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Volume Label
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Allocation Bitmap
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Up-case Table
[MICROSD_DRV_v1.0.0:INFO] Entry 3: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 5: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 6: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 8: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 9: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_002.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_002.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_002.TXT' at cluster 12, size 154 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 154 bytes from file 'TEST_002.TXT'
✓ PASS: File TEST_002.TXT created and verified (154 bytes)

[IT-2 File Ops] Iteration 3/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_003.TXT (154 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 154 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 47
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 154 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_003.TXT'
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Volume Label
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Allocation Bitmap
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Up-case Table
[MICROSD_DRV_v1.0.0:INFO] Entry 3: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 5: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 6: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 8: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 9: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 11: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 12: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_003.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_003.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_003.TXT' at cluster 47, size 154 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 154 bytes from file 'TEST_003.TXT'
✓ PASS: File TEST_003.TXT created and verified (154 bytes)

[IT-2 File Ops] Iteration 4/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_004.TXT (154 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 154 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 52
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 154 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_004.TXT'
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Volume Label
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Allocation Bitmap
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Up-case Table
[MICROSD_DRV_v1.0.0:INFO] Entry 3: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 5: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 6: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 8: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 9: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 11: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 12: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 14: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_004.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_004.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_004.TXT' at cluster 52, size 154 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 154 bytes from file 'TEST_004.TXT'
✓ PASS: File TEST_004.TXT created and verified (154 bytes)

[IT-2 File Ops] Iteration 5/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_005.TXT (154 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 154 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 57
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 154 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_005.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 5 -> 62
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_005.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_005.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_005.TXT' at cluster 57, size 154 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 154 bytes from file 'TEST_005.TXT'
✓ PASS: File TEST_005.TXT created and verified (154 bytes)

[IT-2 File Ops] Iteration 6/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_006.TXT (154 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 154 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 67
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 154 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_006.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 62 -> 71
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_006.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_006.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_006.TXT' at cluster 67, size 154 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 154 bytes from file 'TEST_006.TXT'
✓ PASS: File TEST_006.TXT created and verified (154 bytes)

[IT-2 File Ops] Iteration 7/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_007.TXT (154 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 154 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 95
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 154 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_007.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 71 -> 96
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_007.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_007.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_007.TXT' at cluster 95, size 154 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 154 bytes from file 'TEST_007.TXT'
✓ PASS: File TEST_007.TXT created and verified (154 bytes)

[IT-2 File Ops] Iteration 8/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_008.TXT (154 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 154 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 99
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 154 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_008.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 96 -> 100
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_008.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_008.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_008.TXT' at cluster 99, size 154 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 154 bytes from file 'TEST_008.TXT'
✓ PASS: File TEST_008.TXT created and verified (154 bytes)

[IT-2 File Ops] Iteration 9/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_009.TXT (154 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 154 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 103
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 154 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_009.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 100 -> 104
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_009.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_009.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_009.TXT' at cluster 103, size 154 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 154 bytes from file 'TEST_009.TXT'
✓ PASS: File TEST_009.TXT created and verified (154 bytes)

[IT-2 File Ops] Iteration 10/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_010.TXT (155 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 155 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 107
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 155 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_010.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 104 -> 108
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_010.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_010.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 108, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_010.TXT' at cluster 107, size 155 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 155 bytes from file 'TEST_010.TXT'
✓ PASS: File TEST_010.TXT created and verified (155 bytes)

[IT-2 File Ops] Iteration 11/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_011.TXT (155 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 155 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 111
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 155 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_011.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 108 -> 112
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 112 (sector 21376)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_011.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_011.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 108, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 112 (sector 21376)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 112, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_011.TXT' at cluster 111, size 155 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 155 bytes from file 'TEST_011.TXT'
✓ PASS: File TEST_011.TXT created and verified (155 bytes)

[IT-2 File Ops] Iteration 12/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_012.TXT (155 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 155 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 115
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 155 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_012.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 112 -> 116
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 116 (sector 21632)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_012.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_012.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 108, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 112 (sector 21376)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 112, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 116 (sector 21632)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 116, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_012.TXT' at cluster 115, size 155 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 155 bytes from file 'TEST_012.TXT'
✓ PASS: File TEST_012.TXT created and verified (155 bytes)

[IT-2 File Ops] Iteration 13/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_013.TXT (155 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 155 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 119
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 155 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_013.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 116 -> 120
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 120 (sector 21888)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_013.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_013.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 108, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 112 (sector 21376)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 112, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 116 (sector 21632)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 116, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 120 (sector 21888)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 120, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_013.TXT' at cluster 119, size 155 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 155 bytes from file 'TEST_013.TXT'
✓ PASS: File TEST_013.TXT created and verified (155 bytes)

[IT-2 File Ops] Iteration 14/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_014.TXT (155 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 155 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 123
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 155 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_014.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 120 -> 124
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 124 (sector 22144)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_014.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_014.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 108, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 112 (sector 21376)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 112, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 116 (sector 21632)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 116, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 120 (sector 21888)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 120, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 124 (sector 22144)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 124, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_014.TXT' at cluster 123, size 155 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 155 bytes from file 'TEST_014.TXT'
✓ PASS: File TEST_014.TXT created and verified (155 bytes)

[IT-2 File Ops] Iteration 15/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_015.TXT (155 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 155 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 127
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 155 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_015.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 124 -> 128
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 128 (sector 22400)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_015.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_015.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 108, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 112 (sector 21376)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 112, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 116 (sector 21632)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 116, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 120 (sector 21888)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 120, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 124 (sector 22144)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 124, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 128 (sector 22400)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 128, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_015.TXT' at cluster 127, size 155 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 155 bytes from file 'TEST_015.TXT'
✓ PASS: File TEST_015.TXT created and verified (155 bytes)

[IT-2 File Ops] Iteration 16/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_016.TXT (155 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 155 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 131
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 155 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_016.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 128 -> 132
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 132 (sector 22656)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_016.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_016.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 108, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 112 (sector 21376)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 112, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 116 (sector 21632)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 116, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 120 (sector 21888)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 120, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 124 (sector 22144)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 124, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 128 (sector 22400)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 128, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 132 (sector 22656)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 132, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_016.TXT' at cluster 131, size 155 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 155 bytes from file 'TEST_016.TXT'
✓ PASS: File TEST_016.TXT created and verified (155 bytes)

[IT-2 File Ops] Iteration 17/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_017.TXT (155 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 155 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 135
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 155 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_017.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 132 -> 136
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 136 (sector 22912)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_017.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_017.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 108, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 112 (sector 21376)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 112, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 116 (sector 21632)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 116, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 120 (sector 21888)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 120, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 124 (sector 22144)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 124, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 128 (sector 22400)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 128, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 132 (sector 22656)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 132, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 136 (sector 22912)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 136, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_017.TXT' at cluster 135, size 155 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 155 bytes from file 'TEST_017.TXT'
✓ PASS: File TEST_017.TXT created and verified (155 bytes)

[IT-2 File Ops] Iteration 18/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_018.TXT (155 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 155 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 139
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 155 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_018.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 136 -> 140
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 140 (sector 23168)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_018.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_018.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 108, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 112 (sector 21376)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 112, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 116 (sector 21632)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 116, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 120 (sector 21888)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 120, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 124 (sector 22144)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 124, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 128 (sector 22400)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 128, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 132 (sector 22656)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 132, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 136 (sector 22912)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 136, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 140 (sector 23168)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 140, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_018.TXT' at cluster 139, size 155 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 155 bytes from file 'TEST_018.TXT'
✓ PASS: File TEST_018.TXT created and verified (155 bytes)

[IT-2 File Ops] Iteration 19/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_019.TXT (155 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 155 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 143
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 155 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_019.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 140 -> 144
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 144 (sector 23424)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_019.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_019.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 108, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 112 (sector 21376)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 112, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 116 (sector 21632)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 116, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 120 (sector 21888)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 120, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 124 (sector 22144)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 124, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 128 (sector 22400)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 128, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 132 (sector 22656)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 132, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 136 (sector 22912)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 136, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 140 (sector 23168)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 140, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 144 (sector 23424)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 144, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_019.TXT' at cluster 143, size 155 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 155 bytes from file 'TEST_019.TXT'
✓ PASS: File TEST_019.TXT created and verified (155 bytes)

[IT-2 File Ops] Iteration 20/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_020.TXT (155 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 155 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 147
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 155 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_020.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 144 -> 148
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 148 (sector 23680)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_020.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_020.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 108, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 112 (sector 21376)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 112, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 116 (sector 21632)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 116, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 120 (sector 21888)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 120, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 124 (sector 22144)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 124, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 128 (sector 22400)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 128, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 132 (sector 22656)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 132, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 136 (sector 22912)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 136, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 140 (sector 23168)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 140, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 144 (sector 23424)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 144, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 148 (sector 23680)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 148, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_020.TXT' at cluster 147, size 155 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 155 bytes from file 'TEST_020.TXT'
✓ PASS: File TEST_020.TXT created and verified (155 bytes)

[IT-2 File Ops] Iteration 21/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_021.TXT (155 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 155 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 151
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 155 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_021.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 148 -> 152
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 152 (sector 23936)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_021.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_021.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 108, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 112 (sector 21376)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 112, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 116 (sector 21632)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 116, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 120 (sector 21888)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 120, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 124 (sector 22144)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 124, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 128 (sector 22400)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 128, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 132 (sector 22656)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 132, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 136 (sector 22912)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 136, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 140 (sector 23168)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 140, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 144 (sector 23424)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 144, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 148 (sector 23680)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 148, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 152 (sector 23936)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 152, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_021.TXT' at cluster 151, size 155 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 155 bytes from file 'TEST_021.TXT'
✓ PASS: File TEST_021.TXT created and verified (155 bytes)

[IT-2 File Ops] Iteration 22/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_022.TXT (155 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 155 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 155
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 155 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_022.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 152 -> 156
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 156 (sector 24192)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_022.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_022.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 108, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 112 (sector 21376)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 112, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 116 (sector 21632)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 116, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 120 (sector 21888)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 120, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 124 (sector 22144)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 124, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 128 (sector 22400)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 128, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 132 (sector 22656)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 132, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 136 (sector 22912)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 136, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 140 (sector 23168)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 140, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 144 (sector 23424)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 144, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 148 (sector 23680)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 148, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 152 (sector 23936)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 152, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 156 (sector 24192)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 156, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_022.TXT' at cluster 155, size 155 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 155 bytes from file 'TEST_022.TXT'
✓ PASS: File TEST_022.TXT created and verified (155 bytes)

[IT-2 File Ops] Iteration 23/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_023.TXT (155 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 155 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 159
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 155 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_023.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 156 -> 160
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 160 (sector 24448)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_023.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_023.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 108, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 112 (sector 21376)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 112, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 116 (sector 21632)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 116, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 120 (sector 21888)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 120, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 124 (sector 22144)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 124, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 128 (sector 22400)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 128, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 132 (sector 22656)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 132, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 136 (sector 22912)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 136, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 140 (sector 23168)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 140, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 144 (sector 23424)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 144, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 148 (sector 23680)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 148, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 152 (sector 23936)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 152, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 156 (sector 24192)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 156, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 160 (sector 24448)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 160, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_023.TXT' at cluster 159, size 155 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 155 bytes from file 'TEST_023.TXT'
✓ PASS: File TEST_023.TXT created and verified (155 bytes)

[IT-2 File Ops] Iteration 24/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_024.TXT (155 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 155 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 163
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 155 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_024.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 160 -> 164
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 164 (sector 24704)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_024.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_024.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 108, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 112 (sector 21376)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 112, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 116 (sector 21632)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 116, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 120 (sector 21888)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 120, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 124 (sector 22144)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 124, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 128 (sector 22400)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 128, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 132 (sector 22656)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 132, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 136 (sector 22912)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 136, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 140 (sector 23168)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 140, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 144 (sector 23424)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 144, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 148 (sector 23680)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 148, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 152 (sector 23936)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 152, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 156 (sector 24192)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 156, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 160 (sector 24448)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 160, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 164 (sector 24704)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 164, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_024.TXT' at cluster 163, size 155 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 155 bytes from file 'TEST_024.TXT'
✓ PASS: File TEST_024.TXT created and verified (155 bytes)

[IT-2 File Ops] Iteration 25/25
[MICROSD_DRV_v1.0.0:INFO] Creating file: TEST_025.TXT (155 bytes)
[MICROSD_DRV_v1.0.0:INFO] Allocating cluster chain for 155 bytes (1 clusters)
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully allocated cluster chain: 1 clusters starting at 167
[MICROSD_DRV_v1.0.0:INFO] Successfully wrote 155 bytes across cluster chain
[MICROSD_DRV_v1.0.0:INFO] Using unique filename: 'TEST_025.TXT'
[MICROSD_DRV_v1.0.0:WARN] Root directory cluster full (need 3 entries, only 1 available)
[MICROSD_DRV_v1.0.0:INFO] Expanding directory cluster 5
[MICROSD_DRV_v1.0.0:INFO] === Root Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: Type=0x83[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=3, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Type=0x81[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=2, DataLength=60864
[MICROSD_DRV_v1.0.0:INFO] Entry 2: Type=0x82[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=2, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=4, DataLength=5836
[MICROSD_DRV_v1.0.0:INFO] Entry 3: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 4: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 5: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 6: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 7: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 8: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 9: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 10: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 11: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 12: Type=0x85[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=5, Importance=0, Category=0)
[MICROSD_DRV_v1.0.0:INFO]         FirstCluster=0, DataLength=0
[MICROSD_DRV_v1.0.0:INFO] Entry 13: Type=0xC0[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=0, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 14: Type=0xC1[MICROSD_DRV_v1.0.0:INFO]  (TypeCode=1, Importance=0, Category=1)
[MICROSD_DRV_v1.0.0:INFO] Entry 15: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Directory Entries ===
[MICROSD_DRV_v1.0.0:INFO] Found allocation bitmap at cluster 2
[MICROSD_DRV_v1.0.0:INFO] Successfully expanded directory: linked cluster 164 -> 168
[MICROSD_DRV_v1.0.0:INFO] Directory expanded: now writing to cluster 168 (sector 24960)
[MICROSD_DRV_v1.0.0:INFO] Directory expanded successfully, now have 16 available entries in new cluster
[MICROSD_DRV_v1.0.0:INFO] Directory write verification successful
[MICROSD_DRV_v1.0.0:INFO] === Final Root Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] Entry 0: File (secondary_count=2)
[MICROSD_DRV_v1.0.0:INFO] Entry 1: Stream Extension
[MICROSD_DRV_v1.0.0:INFO] Entry 2: File Name
[MICROSD_DRV_v1.0.0:INFO] Entry 3: End-of-directory marker
[MICROSD_DRV_v1.0.0:INFO] === End Final Directory Structure ===
[MICROSD_DRV_v1.0.0:INFO] File 'TEST_025.TXT' created successfully
[MICROSD_DRV_v1.0.0:INFO] Reading file: TEST_025.TXT
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 5 (sector 14528)
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x83 at cluster 5, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x81 at cluster 5, sector offset 0, entry 1
[MICROSD_DRV_v1.0.0:INFO] Found entry type 0x82 at cluster 5, sector offset 0, entry 2
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 3
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 6
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 9
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 5, sector offset 0, entry 12
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 62 (sector 18176)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 62, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 71 (sector 18752)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 71, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 96 (sector 20352)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 96, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 100 (sector 20608)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 100, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 104 (sector 20864)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 104, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 108 (sector 21120)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 108, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 112 (sector 21376)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 112, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 116 (sector 21632)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 116, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 120 (sector 21888)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 120, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 124 (sector 22144)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 124, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 128 (sector 22400)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 128, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 132 (sector 22656)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 132, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 136 (sector 22912)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 136, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 140 (sector 23168)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 140, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 144 (sector 23424)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 144, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 148 (sector 23680)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 148, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 152 (sector 23936)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 152, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 156 (sector 24192)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 156, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 160 (sector 24448)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 160, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 164 (sector 24704)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 164, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Searching directory cluster 168 (sector 24960)
[MICROSD_DRV_v1.0.0:INFO] Found file entry (0x85) at cluster 168, sector offset 0, entry 0
[MICROSD_DRV_v1.0.0:INFO] Found file 'TEST_025.TXT' at cluster 167, size 155 bytes
[MICROSD_DRV_v1.0.0:INFO] Successfully read 155 bytes from file 'TEST_025.TXT'
✓ PASS: File TEST_025.TXT created and verified (155 bytes)

================================================================================
                    AUTOMATED MICROSD TEST SUMMARY
================================================================================

Total test executions: 125
Passed:  125 (100.0%)
Failed:  0 (0.0%)
Errors:  1

Test Breakdown:
  IT-2 (Initialize):      20 executions
  UT-5 (Block I/O):       50 executions
  UT-5 (Byte I/O RMW):    30 executions
  IT-2 (File operations): 25 executions
  ────────────────────────────────────
  Total:                  125 executions

✗ SOME TESTS FAILED
Review the output above for details.

================================================================================

Tests complete. System will remain active for monitoring.
Press Ctrl+C to exit.

```