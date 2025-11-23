## logged from streaming_test.uf2

The purpose of this test is to verify the correct implementation of the Go-Back-N ARQ protocol in the MQTT-SN Pico client. The test involves sending a series of messages with intentional packet loss to ensure that the client correctly retransmits lost packets and maintains data integrity.

``` log
Waiting... 4
Waiting... 3
Waiting... 2
Waiting... 1

Initializing microSD card driver and FatFS filesystem...
Initializing SD card driver...
✓ SD card hardware initialized
✓ Filesystem mounted successfully
✓ MicroSD driver and FatFS filesystem initialized successfully

Checking for source file: test.jpg
  get_file_size() called for: test.jpg
  File size: 1060364 bytes
✓ Found source file: test.jpg (1060364 bytes)
  Large file detected (> 500KB): 20 iterations

╔═══════════════════════════════════════════════════════╗
║  Starting  20 test iterations                         ║
╚═══════════════════════════════════════════════════════╝


╔═══════════════════════════════════════════════════════╗
║              Test Run #1                              ║
╚═══════════════════════════════════════════════════════╝

Checking for source file: test.jpg
  get_file_size() called for: test.jpg
  File size: 1060364 bytes
✓ Found source file: test.jpg (1060364 bytes)

================================================
  SIMULATING MQTT-SN DATA TRANSFER
  Protocol: Go-Back-N Sliding Window
  Process: Stream -> Verify -> Reconstruct
================================================

=== Step 1: Initialize Streaming Read ===
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: test.jpg
  Size: 1060364 bytes (1035.51 KB)
  Chunks: 4437
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
SUCCESS: File ready for streaming
  Filename: test.jpg
  File size: 1060364 bytes
  Total chunks: 4437
  Session ID: stream_7149
  File CRC32: 0x00000000

=== Sample Chunk Preview ===

Chunk 0: seq=1, size=239 bytes, crc32=0xA17BB47D
  Data preview: "......JFIF........................................................................."

Chunk 1: seq=2, size=239 bytes, crc32=0xBADEF91F
  Data preview: "....A......=...((.D..Oc**.E......t..........={[p,!.....".......z<..Q#..m.M&........"

Chunk 2: seq=3, size=239 bytes, crc32=0x6BAC12B6
  Data preview: ".....Z...OM5.\.*g:.O\.}.QU3....Qg/.."U....${.."..k.5..k.+..|.Z.\....zX....M=PEZe..."

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
  Session ID: stream_8189
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
  [RECEIVER] ✗ Packet loss detected at chunk 17
  [RECEIVER] Sending NACK - retransmit from chunk 17

--- Window [17-154] ---
  [MQTT:file/data] Chunk 17/4437 (seq=17, size=239, crc32=0x3FE9C5AC)
  [MQTT:file/data] Chunk 18/4437 (seq=18, size=239, crc32=0x798DCB74)
  [MQTT:file/data] Chunk 19/4437 (seq=19, size=239, crc32=0x89F908F3)
  [MQTT:file/data] Chunk 67/4437 (seq=67, size=239, crc32=0x5DFF0BD8)
  [MQTT:file/data] Chunk 117/4437 (seq=117, size=239, crc32=0x2C4FE70C)
  [MQTT:file/data] Chunk 154/4437 (seq=154, size=239, crc32=0x6CDF3585)
  ✓ Transmitted 138 chunks in window [17-154]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 18
  [RECEIVER] Sending NACK - retransmit from chunk 18

--- Window [18-155] ---
  [MQTT:file/data] Chunk 18/4437 (seq=18, size=239, crc32=0x798DCB74)
  [MQTT:file/data] Chunk 19/4437 (seq=19, size=239, crc32=0x89F908F3)
  [MQTT:file/data] Chunk 20/4437 (seq=20, size=239, crc32=0x915160F4)
  [MQTT:file/data] Chunk 68/4437 (seq=68, size=239, crc32=0x6C5FE283)
  [MQTT:file/data] Chunk 118/4437 (seq=118, size=239, crc32=0x38F11FAE)
  [MQTT:file/data] Chunk 155/4437 (seq=155, size=239, crc32=0x71114D2D)
  ✓ Transmitted 138 chunks in window [18-155]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 31
  [RECEIVER] Sending NACK - retransmit from chunk 31

--- Window [31-168] ---
  [MQTT:file/data] Chunk 31/4437 (seq=31, size=239, crc32=0x41F86E77)
  [MQTT:file/data] Chunk 32/4437 (seq=32, size=239, crc32=0x70C79C31)
  [MQTT:file/data] Chunk 33/4437 (seq=33, size=239, crc32=0x5A70571C)
  [MQTT:file/data] Chunk 81/4437 (seq=81, size=239, crc32=0x7C67A953)
  [MQTT:file/data] Chunk 131/4437 (seq=131, size=239, crc32=0x42083A3F)
  [MQTT:file/data] Chunk 168/4437 (seq=168, size=239, crc32=0xB3F7580C)
  ✓ Transmitted 138 chunks in window [31-168]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 107
  [RECEIVER] Sending NACK - retransmit from chunk 107

--- Window [107-244] ---
  [MQTT:file/data] Chunk 107/4437 (seq=107, size=239, crc32=0xB2B8B05B)
  [MQTT:file/data] Chunk 108/4437 (seq=108, size=239, crc32=0x47B7A863)
  [MQTT:file/data] Chunk 109/4437 (seq=109, size=239, crc32=0x02BCF17E)
  [MQTT:file/data] Chunk 157/4437 (seq=157, size=239, crc32=0x2727BE06)
  [MQTT:file/data] Chunk 207/4437 (seq=207, size=239, crc32=0x5286FB54)
  [MQTT:file/data] Chunk 244/4437 (seq=244, size=239, crc32=0xFABE1A2D)
  ✓ Transmitted 138 chunks in window [107-244]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 122
  [RECEIVER] Sending NACK - retransmit from chunk 122

--- Window [122-259] ---
  [MQTT:file/data] Chunk 122/4437 (seq=122, size=239, crc32=0xA375C14A)
  [MQTT:file/data] Chunk 123/4437 (seq=123, size=239, crc32=0x29ED757C)
  [MQTT:file/data] Chunk 124/4437 (seq=124, size=239, crc32=0x0584ADD2)
  [MQTT:file/data] Chunk 172/4437 (seq=172, size=239, crc32=0x1503F3DA)
  [MQTT:file/data] Chunk 222/4437 (seq=222, size=239, crc32=0x43A55CC8)
  [MQTT:file/data] Chunk 259/4437 (seq=259, size=239, crc32=0x92465154)
  ✓ Transmitted 138 chunks in window [122-259]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 124
  [RECEIVER] Sending NACK - retransmit from chunk 124

--- Window [124-261] ---
  [MQTT:file/data] Chunk 124/4437 (seq=124, size=239, crc32=0x0584ADD2)
  [MQTT:file/data] Chunk 125/4437 (seq=125, size=239, crc32=0x009E5CE5)
  [MQTT:file/data] Chunk 126/4437 (seq=126, size=239, crc32=0xA1F1DF1C)
  [MQTT:file/data] Chunk 174/4437 (seq=174, size=239, crc32=0xCB1084DC)
  [MQTT:file/data] Chunk 224/4437 (seq=224, size=239, crc32=0xBAF2C21D)
  [MQTT:file/data] Chunk 261/4437 (seq=261, size=239, crc32=0x84CB421F)
  ✓ Transmitted 138 chunks in window [124-261]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 129
  [RECEIVER] Sending NACK - retransmit from chunk 129

--- Window [129-266] ---
  [MQTT:file/data] Chunk 129/4437 (seq=129, size=239, crc32=0x537ADB44)
  [MQTT:file/data] Chunk 130/4437 (seq=130, size=239, crc32=0x61258F58)
  [MQTT:file/data] Chunk 131/4437 (seq=131, size=239, crc32=0x42083A3F)
  [MQTT:file/data] Chunk 179/4437 (seq=179, size=239, crc32=0x8C550392)
  [MQTT:file/data] Chunk 229/4437 (seq=229, size=239, crc32=0xB7323BF5)
  [MQTT:file/data] Chunk 266/4437 (seq=266, size=239, crc32=0x728D7132)
  ✓ Transmitted 138 chunks in window [129-266]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 185
  [RECEIVER] Sending NACK - retransmit from chunk 185

--- Window [185-322] ---
  [MQTT:file/data] Chunk 185/4437 (seq=185, size=239, crc32=0xC675B466)
  [MQTT:file/data] Chunk 186/4437 (seq=186, size=239, crc32=0x04D625E9)
  [MQTT:file/data] Chunk 187/4437 (seq=187, size=239, crc32=0x26F784FF)
  [MQTT:file/data] Chunk 235/4437 (seq=235, size=239, crc32=0xBE3F38D2)
  [MQTT:file/data] Chunk 285/4437 (seq=285, size=239, crc32=0x0ECCFC1F)
  [MQTT:file/data] Chunk 322/4437 (seq=322, size=239, crc32=0xA9AF4E9F)
  ✓ Transmitted 138 chunks in window [185-322]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 199
  [RECEIVER] Sending NACK - retransmit from chunk 199

--- Window [199-336] ---
  [MQTT:file/data] Chunk 199/4437 (seq=199, size=239, crc32=0xB8AFE488)
  [MQTT:file/data] Chunk 200/4437 (seq=200, size=239, crc32=0x29256513)
  [MQTT:file/data] Chunk 201/4437 (seq=201, size=239, crc32=0xCB241B35)
  [MQTT:file/data] Chunk 249/4437 (seq=249, size=239, crc32=0xF8DF0AE2)
  [MQTT:file/data] Chunk 299/4437 (seq=299, size=239, crc32=0xE91B8ED6)
  [MQTT:file/data] Chunk 336/4437 (seq=336, size=239, crc32=0xD8DB3996)
  ✓ Transmitted 138 chunks in window [199-336]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 207
  [RECEIVER] Sending NACK - retransmit from chunk 207

--- Window [207-344] ---
  [MQTT:file/data] Chunk 207/4437 (seq=207, size=239, crc32=0x5286FB54)
  [MQTT:file/data] Chunk 208/4437 (seq=208, size=239, crc32=0x1FCD0376)
  [MQTT:file/data] Chunk 209/4437 (seq=209, size=239, crc32=0x28CA2D1A)
  [MQTT:file/data] Chunk 257/4437 (seq=257, size=239, crc32=0xDD8526C7)
  [MQTT:file/data] Chunk 307/4437 (seq=307, size=239, crc32=0xD401C063)
  [MQTT:file/data] Chunk 344/4437 (seq=344, size=239, crc32=0x24A685A3)
  ✓ Transmitted 138 chunks in window [207-344]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 210
  [RECEIVER] Sending NACK - retransmit from chunk 210

--- Window [210-347] ---
  [MQTT:file/data] Chunk 210/4437 (seq=210, size=239, crc32=0x85F2EEF4)
  [MQTT:file/data] Chunk 211/4437 (seq=211, size=239, crc32=0x4744CDFC)
  [MQTT:file/data] Chunk 212/4437 (seq=212, size=239, crc32=0x875A225D)
  [MQTT:file/data] Chunk 260/4437 (seq=260, size=239, crc32=0x2DBB28E9)
  [MQTT:file/data] Chunk 310/4437 (seq=310, size=239, crc32=0xBA846E64)
  [MQTT:file/data] Chunk 347/4437 (seq=347, size=239, crc32=0x99349BF6)
  ✓ Transmitted 138 chunks in window [210-347]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 215
  [RECEIVER] Sending NACK - retransmit from chunk 215

--- Window [215-352] ---
  [MQTT:file/data] Chunk 215/4437 (seq=215, size=239, crc32=0xFABF5FB3)
  [MQTT:file/data] Chunk 216/4437 (seq=216, size=239, crc32=0xAF26818E)
  [MQTT:file/data] Chunk 217/4437 (seq=217, size=239, crc32=0x0F211649)
  [MQTT:file/data] Chunk 265/4437 (seq=265, size=239, crc32=0xA2393F34)
  [MQTT:file/data] Chunk 315/4437 (seq=315, size=239, crc32=0xA602651D)
  [MQTT:file/data] Chunk 352/4437 (seq=352, size=239, crc32=0x34BDAA56)
  ✓ Transmitted 138 chunks in window [215-352]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 259
  [RECEIVER] Sending NACK - retransmit from chunk 259

--- Window [259-396] ---
  [MQTT:file/data] Chunk 259/4437 (seq=259, size=239, crc32=0x92465154)
  [MQTT:file/data] Chunk 260/4437 (seq=260, size=239, crc32=0x2DBB28E9)
  [MQTT:file/data] Chunk 261/4437 (seq=261, size=239, crc32=0x84CB421F)
  [MQTT:file/data] Chunk 309/4437 (seq=309, size=239, crc32=0x7FAF9621)
  [MQTT:file/data] Chunk 359/4437 (seq=359, size=239, crc32=0x5FE50A53)
  [MQTT:file/data] Chunk 396/4437 (seq=396, size=239, crc32=0xA1A54F73)
  ✓ Transmitted 138 chunks in window [259-396]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 281
  [RECEIVER] Sending NACK - retransmit from chunk 281

--- Window [281-418] ---
  [MQTT:file/data] Chunk 281/4437 (seq=281, size=239, crc32=0x54ABFF9D)
  [MQTT:file/data] Chunk 282/4437 (seq=282, size=239, crc32=0x4BC6CDD8)
  [MQTT:file/data] Chunk 283/4437 (seq=283, size=239, crc32=0x0C5CDCB3)
  [MQTT:file/data] Chunk 331/4437 (seq=331, size=239, crc32=0x52AAF759)
  [MQTT:file/data] Chunk 381/4437 (seq=381, size=239, crc32=0x44ECA0DF)
  [MQTT:file/data] Chunk 418/4437 (seq=418, size=239, crc32=0xCEA91D99)
  ✓ Transmitted 138 chunks in window [281-418]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 290
  [RECEIVER] Sending NACK - retransmit from chunk 290

--- Window [290-427] ---
  [MQTT:file/data] Chunk 290/4437 (seq=290, size=239, crc32=0xABCDB8E6)
  [MQTT:file/data] Chunk 291/4437 (seq=291, size=239, crc32=0xEF2A0DDA)
  [MQTT:file/data] Chunk 292/4437 (seq=292, size=239, crc32=0x222BE441)
  [MQTT:file/data] Chunk 340/4437 (seq=340, size=239, crc32=0xF73C0C89)
  [MQTT:file/data] Chunk 390/4437 (seq=390, size=239, crc32=0x7C290FA2)
  [MQTT:file/data] Chunk 427/4437 (seq=427, size=239, crc32=0xED9FCEFC)
  ✓ Transmitted 138 chunks in window [290-427]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 303
  [RECEIVER] Sending NACK - retransmit from chunk 303

--- Window [303-440] ---
  [MQTT:file/data] Chunk 303/4437 (seq=303, size=239, crc32=0x539F3DAB)
  [MQTT:file/data] Chunk 304/4437 (seq=304, size=239, crc32=0x10D1418E)
  [MQTT:file/data] Chunk 305/4437 (seq=305, size=239, crc32=0xB51528AB)
  [MQTT:file/data] Chunk 353/4437 (seq=353, size=239, crc32=0x54A0B6CA)
  [MQTT:file/data] Chunk 403/4437 (seq=403, size=239, crc32=0x9E368531)
  [MQTT:file/data] Chunk 440/4437 (seq=440, size=239, crc32=0x6D688A7E)
  ✓ Transmitted 138 chunks in window [303-440]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 316
  [RECEIVER] Sending NACK - retransmit from chunk 316

--- Window [316-453] ---
  [MQTT:file/data] Chunk 316/4437 (seq=316, size=239, crc32=0xFB536CE7)
  [MQTT:file/data] Chunk 317/4437 (seq=317, size=239, crc32=0x12257E0E)
  [MQTT:file/data] Chunk 318/4437 (seq=318, size=239, crc32=0xC716376E)
  [MQTT:file/data] Chunk 366/4437 (seq=366, size=239, crc32=0x05E3B688)
  [MQTT:file/data] Chunk 416/4437 (seq=416, size=239, crc32=0xF8EAB161)
  [MQTT:file/data] Chunk 453/4437 (seq=453, size=239, crc32=0x0A21DBD2)
  ✓ Transmitted 138 chunks in window [316-453]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 373
  [RECEIVER] Sending NACK - retransmit from chunk 373

--- Window [373-510] ---
  [MQTT:file/data] Chunk 373/4437 (seq=373, size=239, crc32=0xCB2E11F9)
  [MQTT:file/data] Chunk 374/4437 (seq=374, size=239, crc32=0x445FC848)
  [MQTT:file/data] Chunk 375/4437 (seq=375, size=239, crc32=0x15262ED7)
  [MQTT:file/data] Chunk 423/4437 (seq=423, size=239, crc32=0x807EAB12)
  [MQTT:file/data] Chunk 473/4437 (seq=473, size=239, crc32=0x8B0B71C0)
  [MQTT:file/data] Chunk 510/4437 (seq=510, size=239, crc32=0xCBB2059E)
  ✓ Transmitted 138 chunks in window [373-510]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 396
  [RECEIVER] Sending NACK - retransmit from chunk 396

--- Window [396-533] ---
  [MQTT:file/data] Chunk 396/4437 (seq=396, size=239, crc32=0xA1A54F73)
  [MQTT:file/data] Chunk 397/4437 (seq=397, size=239, crc32=0xE816F8A0)
  [MQTT:file/data] Chunk 398/4437 (seq=398, size=239, crc32=0x965DA9E5)
  [MQTT:file/data] Chunk 446/4437 (seq=446, size=239, crc32=0x4FF46C13)
  [MQTT:file/data] Chunk 496/4437 (seq=496, size=239, crc32=0x95CFE74A)
  [MQTT:file/data] Chunk 533/4437 (seq=533, size=239, crc32=0x5DCE4B50)
  ✓ Transmitted 138 chunks in window [396-533]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 410
  [RECEIVER] Sending NACK - retransmit from chunk 410

--- Window [410-547] ---
  [MQTT:file/data] Chunk 410/4437 (seq=410, size=239, crc32=0x421EEC10)
  [MQTT:file/data] Chunk 411/4437 (seq=411, size=239, crc32=0x3790B95E)
  [MQTT:file/data] Chunk 412/4437 (seq=412, size=239, crc32=0x38087AB6)
  [MQTT:file/data] Chunk 460/4437 (seq=460, size=239, crc32=0xC8406256)
  [MQTT:file/data] Chunk 510/4437 (seq=510, size=239, crc32=0xCBB2059E)
  [MQTT:file/data] Chunk 547/4437 (seq=547, size=239, crc32=0x53185054)
  ✓ Transmitted 138 chunks in window [410-547]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 426
  [RECEIVER] Sending NACK - retransmit from chunk 426

--- Window [426-563] ---
  [MQTT:file/data] Chunk 426/4437 (seq=426, size=239, crc32=0x545ACF7C)
  [MQTT:file/data] Chunk 427/4437 (seq=427, size=239, crc32=0xED9FCEFC)
  [MQTT:file/data] Chunk 428/4437 (seq=428, size=239, crc32=0x9FCF2C28)
  [MQTT:file/data] Chunk 476/4437 (seq=476, size=239, crc32=0xCF791E40)
  [MQTT:file/data] Chunk 526/4437 (seq=526, size=239, crc32=0x1D1C6719)
  [MQTT:file/data] Chunk 563/4437 (seq=563, size=239, crc32=0x26F8A287)
  ✓ Transmitted 138 chunks in window [426-563]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 431
  [RECEIVER] Sending NACK - retransmit from chunk 431

--- Window [431-568] ---
  [MQTT:file/data] Chunk 431/4437 (seq=431, size=239, crc32=0xD8B063DB)
  [MQTT:file/data] Chunk 432/4437 (seq=432, size=239, crc32=0x7AF516B6)
  [MQTT:file/data] Chunk 433/4437 (seq=433, size=239, crc32=0xBDD0E15A)
  [MQTT:file/data] Chunk 481/4437 (seq=481, size=239, crc32=0x1DE3BEB3)
  [MQTT:file/data] Chunk 531/4437 (seq=531, size=239, crc32=0x3F3CBC87)
  [MQTT:file/data] Chunk 568/4437 (seq=568, size=239, crc32=0x1D1263F4)
  ✓ Transmitted 138 chunks in window [431-568]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 440
  [RECEIVER] Sending NACK - retransmit from chunk 440

--- Window [440-577] ---
  [MQTT:file/data] Chunk 440/4437 (seq=440, size=239, crc32=0x6D688A7E)
  [MQTT:file/data] Chunk 441/4437 (seq=441, size=239, crc32=0x430E27DB)
  [MQTT:file/data] Chunk 442/4437 (seq=442, size=239, crc32=0xD4D43254)
  [MQTT:file/data] Chunk 490/4437 (seq=490, size=239, crc32=0x96F82D8D)
  [MQTT:file/data] Chunk 540/4437 (seq=540, size=239, crc32=0xA719A614)
  [MQTT:file/data] Chunk 577/4437 (seq=577, size=239, crc32=0x0C7B9C89)
  ✓ Transmitted 138 chunks in window [440-577]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 454
  [RECEIVER] Sending NACK - retransmit from chunk 454

--- Window [454-591] ---
  [MQTT:file/data] Chunk 454/4437 (seq=454, size=239, crc32=0xAA849FF9)
  [MQTT:file/data] Chunk 455/4437 (seq=455, size=239, crc32=0x0BD1F6B7)
  [MQTT:file/data] Chunk 456/4437 (seq=456, size=239, crc32=0xCFE7111A)
  [MQTT:file/data] Chunk 504/4437 (seq=504, size=239, crc32=0x0860FF1E)
  [MQTT:file/data] Chunk 554/4437 (seq=554, size=239, crc32=0x49948541)
  [MQTT:file/data] Chunk 591/4437 (seq=591, size=239, crc32=0x8466CC3E)
  ✓ Transmitted 138 chunks in window [454-591]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 464
  [RECEIVER] Sending NACK - retransmit from chunk 464

--- Window [464-601] ---
  [MQTT:file/data] Chunk 464/4437 (seq=464, size=239, crc32=0x4635E113)
  [MQTT:file/data] Chunk 465/4437 (seq=465, size=239, crc32=0xCE3EFC2F)
  [MQTT:file/data] Chunk 466/4437 (seq=466, size=239, crc32=0xFA9C2C14)
  [MQTT:file/data] Chunk 514/4437 (seq=514, size=239, crc32=0xBB970B94)
  [MQTT:file/data] Chunk 564/4437 (seq=564, size=239, crc32=0xE2DF2557)
  [MQTT:file/data] Chunk 601/4437 (seq=601, size=239, crc32=0x6BA125DD)
  ✓ Transmitted 138 chunks in window [464-601]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 489
  [RECEIVER] Sending NACK - retransmit from chunk 489

--- Window [489-626] ---
  [MQTT:file/data] Chunk 489/4437 (seq=489, size=239, crc32=0xBF150242)
  [MQTT:file/data] Chunk 490/4437 (seq=490, size=239, crc32=0x96F82D8D)
  [MQTT:file/data] Chunk 491/4437 (seq=491, size=239, crc32=0xB8F8C6BD)
  [MQTT:file/data] Chunk 539/4437 (seq=539, size=239, crc32=0x9E3DC0DE)
  [MQTT:file/data] Chunk 589/4437 (seq=589, size=239, crc32=0x0E167E47)
  [MQTT:file/data] Chunk 626/4437 (seq=626, size=239, crc32=0x5FA1F81D)
  ✓ Transmitted 138 chunks in window [489-626]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 489
  [RECEIVER] Sending NACK - retransmit from chunk 489

--- Window [489-626] ---
  [MQTT:file/data] Chunk 489/4437 (seq=489, size=239, crc32=0xBF150242)
  [MQTT:file/data] Chunk 490/4437 (seq=490, size=239, crc32=0x96F82D8D)
  [MQTT:file/data] Chunk 491/4437 (seq=491, size=239, crc32=0xB8F8C6BD)
  [MQTT:file/data] Chunk 539/4437 (seq=539, size=239, crc32=0x9E3DC0DE)
  [MQTT:file/data] Chunk 589/4437 (seq=589, size=239, crc32=0x0E167E47)
  [MQTT:file/data] Chunk 626/4437 (seq=626, size=239, crc32=0x5FA1F81D)
  ✓ Transmitted 138 chunks in window [489-626]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 534
  [RECEIVER] Sending NACK - retransmit from chunk 534

--- Window [534-671] ---
  [MQTT:file/data] Chunk 534/4437 (seq=534, size=239, crc32=0x20F3D5C5)
  [MQTT:file/data] Chunk 535/4437 (seq=535, size=239, crc32=0xA7E8C73D)
  [MQTT:file/data] Chunk 536/4437 (seq=536, size=239, crc32=0x1D722E87)
  [MQTT:file/data] Chunk 584/4437 (seq=584, size=239, crc32=0x1C98CC29)
  [MQTT:file/data] Chunk 634/4437 (seq=634, size=239, crc32=0x07EA8983)
  [MQTT:file/data] Chunk 671/4437 (seq=671, size=239, crc32=0xAECD073A)
  ✓ Transmitted 138 chunks in window [534-671]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 563
  [RECEIVER] Sending NACK - retransmit from chunk 563

--- Window [563-700] ---
  [MQTT:file/data] Chunk 563/4437 (seq=563, size=239, crc32=0x26F8A287)
  [MQTT:file/data] Chunk 564/4437 (seq=564, size=239, crc32=0xE2DF2557)
  [MQTT:file/data] Chunk 565/4437 (seq=565, size=239, crc32=0x67954021)
  [MQTT:file/data] Chunk 613/4437 (seq=613, size=239, crc32=0x97C3BB90)
  [MQTT:file/data] Chunk 663/4437 (seq=663, size=239, crc32=0xB6432322)
  [MQTT:file/data] Chunk 700/4437 (seq=700, size=239, crc32=0x228D5CA6)
  ✓ Transmitted 138 chunks in window [563-700]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 570
  [RECEIVER] Sending NACK - retransmit from chunk 570

--- Window [570-707] ---
  [MQTT:file/data] Chunk 570/4437 (seq=570, size=239, crc32=0x266B0FFA)
  [MQTT:file/data] Chunk 571/4437 (seq=571, size=239, crc32=0x5AA7C42F)
  [MQTT:file/data] Chunk 572/4437 (seq=572, size=239, crc32=0xFF6EEDBD)
  [MQTT:file/data] Chunk 620/4437 (seq=620, size=239, crc32=0xAC6B49A1)
  [MQTT:file/data] Chunk 670/4437 (seq=670, size=239, crc32=0x452C2B59)
  [MQTT:file/data] Chunk 707/4437 (seq=707, size=239, crc32=0x3ED5D653)
  ✓ Transmitted 138 chunks in window [570-707]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 608
  [RECEIVER] Sending NACK - retransmit from chunk 608

--- Window [608-745] ---
  [MQTT:file/data] Chunk 608/4437 (seq=608, size=239, crc32=0x3B5CCB49)
  [MQTT:file/data] Chunk 609/4437 (seq=609, size=239, crc32=0xD4EF5027)
  [MQTT:file/data] Chunk 610/4437 (seq=610, size=239, crc32=0x79B18ABD)
  [MQTT:file/data] Chunk 658/4437 (seq=658, size=239, crc32=0x58EBF8BB)
  [MQTT:file/data] Chunk 708/4437 (seq=708, size=239, crc32=0x510B4209)
  [MQTT:file/data] Chunk 745/4437 (seq=745, size=239, crc32=0x877C673B)
  ✓ Transmitted 138 chunks in window [608-745]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 619
  [RECEIVER] Sending NACK - retransmit from chunk 619

--- Window [619-756] ---
  [MQTT:file/data] Chunk 619/4437 (seq=619, size=239, crc32=0x6F309441)
  [MQTT:file/data] Chunk 620/4437 (seq=620, size=239, crc32=0xAC6B49A1)
  [MQTT:file/data] Chunk 621/4437 (seq=621, size=239, crc32=0xC9F80008)
  [MQTT:file/data] Chunk 669/4437 (seq=669, size=239, crc32=0x8EE54FB5)
  [MQTT:file/data] Chunk 719/4437 (seq=719, size=239, crc32=0xC9056C3A)
  [MQTT:file/data] Chunk 756/4437 (seq=756, size=239, crc32=0x76C4CD67)
  ✓ Transmitted 138 chunks in window [619-756]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 686
  [RECEIVER] Sending NACK - retransmit from chunk 686

--- Window [686-823] ---
  [MQTT:file/data] Chunk 686/4437 (seq=686, size=239, crc32=0xA64DD994)
  [MQTT:file/data] Chunk 687/4437 (seq=687, size=239, crc32=0xCB628BF4)
  [MQTT:file/data] Chunk 688/4437 (seq=688, size=239, crc32=0x381ADA44)
  [MQTT:file/data] Chunk 736/4437 (seq=736, size=239, crc32=0xEA688BE7)
  [MQTT:file/data] Chunk 786/4437 (seq=786, size=239, crc32=0x25B72026)
  [MQTT:file/data] Chunk 823/4437 (seq=823, size=239, crc32=0xCFB18179)
  ✓ Transmitted 138 chunks in window [686-823]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 687
  [RECEIVER] Sending NACK - retransmit from chunk 687

--- Window [687-824] ---
  [MQTT:file/data] Chunk 687/4437 (seq=687, size=239, crc32=0xCB628BF4)
  [MQTT:file/data] Chunk 688/4437 (seq=688, size=239, crc32=0x381ADA44)
  [MQTT:file/data] Chunk 689/4437 (seq=689, size=239, crc32=0xBE8B116D)
  [MQTT:file/data] Chunk 737/4437 (seq=737, size=239, crc32=0xD78F9281)
  [MQTT:file/data] Chunk 787/4437 (seq=787, size=239, crc32=0xE2AA8731)
  [MQTT:file/data] Chunk 824/4437 (seq=824, size=239, crc32=0x3B2AFACD)
  ✓ Transmitted 138 chunks in window [687-824]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 790
  [RECEIVER] Sending NACK - retransmit from chunk 790

--- Window [790-927] ---
  [MQTT:file/data] Chunk 790/4437 (seq=790, size=239, crc32=0xAAE3BF6B)
  [MQTT:file/data] Chunk 791/4437 (seq=791, size=239, crc32=0x8EA18362)
  [MQTT:file/data] Chunk 792/4437 (seq=792, size=239, crc32=0x677140D7)
  [MQTT:file/data] Chunk 840/4437 (seq=840, size=239, crc32=0x3BF27F0F)
  [MQTT:file/data] Chunk 890/4437 (seq=890, size=239, crc32=0xE164CCFE)
  [MQTT:file/data] Chunk 927/4437 (seq=927, size=239, crc32=0xFBB19BE5)
  ✓ Transmitted 138 chunks in window [790-927]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 815
  [RECEIVER] Sending NACK - retransmit from chunk 815

--- Window [815-952] ---
  [MQTT:file/data] Chunk 815/4437 (seq=815, size=239, crc32=0x6A6DC04D)
  [MQTT:file/data] Chunk 816/4437 (seq=816, size=239, crc32=0xE8A9588C)
  [MQTT:file/data] Chunk 817/4437 (seq=817, size=239, crc32=0x5C75728C)
  [MQTT:file/data] Chunk 865/4437 (seq=865, size=239, crc32=0xE0B92B87)
  [MQTT:file/data] Chunk 915/4437 (seq=915, size=239, crc32=0xA18F9BC3)
  [MQTT:file/data] Chunk 952/4437 (seq=952, size=239, crc32=0xA1C107AD)
  ✓ Transmitted 138 chunks in window [815-952]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 843
  [RECEIVER] Sending NACK - retransmit from chunk 843

--- Window [843-980] ---
  [MQTT:file/data] Chunk 843/4437 (seq=843, size=239, crc32=0xC5DD84C3)
  [MQTT:file/data] Chunk 844/4437 (seq=844, size=239, crc32=0x9E7C85A1)
  [MQTT:file/data] Chunk 845/4437 (seq=845, size=239, crc32=0x82D389FC)
  [MQTT:file/data] Chunk 893/4437 (seq=893, size=239, crc32=0x3DEDD109)
  [MQTT:file/data] Chunk 943/4437 (seq=943, size=239, crc32=0xE7556DA9)
  [MQTT:file/data] Chunk 980/4437 (seq=980, size=239, crc32=0xD2CB341B)
  ✓ Transmitted 138 chunks in window [843-980]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 882
  [RECEIVER] Sending NACK - retransmit from chunk 882

--- Window [882-1019] ---
  [MQTT:file/data] Chunk 882/4437 (seq=882, size=239, crc32=0x47A568CB)
  [MQTT:file/data] Chunk 883/4437 (seq=883, size=239, crc32=0xCD1034F6)
  [MQTT:file/data] Chunk 884/4437 (seq=884, size=239, crc32=0xAB79E264)
  [MQTT:file/data] Chunk 932/4437 (seq=932, size=239, crc32=0xC1CB9996)
  [MQTT:file/data] Chunk 982/4437 (seq=982, size=239, crc32=0x34318F25)
  [MQTT:file/data] Chunk 1019/4437 (seq=1019, size=239, crc32=0x3AA65C38)
  ✓ Transmitted 138 chunks in window [882-1019]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 890
  [RECEIVER] Sending NACK - retransmit from chunk 890

--- Window [890-1027] ---
  [MQTT:file/data] Chunk 890/4437 (seq=890, size=239, crc32=0xE164CCFE)
  [MQTT:file/data] Chunk 891/4437 (seq=891, size=239, crc32=0x92E7D2B4)
  [MQTT:file/data] Chunk 892/4437 (seq=892, size=239, crc32=0x2A4A89B3)
  [MQTT:file/data] Chunk 940/4437 (seq=940, size=239, crc32=0x985A6434)
  [MQTT:file/data] Chunk 990/4437 (seq=990, size=239, crc32=0x7496AB62)
  [MQTT:file/data] Chunk 1027/4437 (seq=1027, size=239, crc32=0x079174B2)
  ✓ Transmitted 138 chunks in window [890-1027]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 929
  [RECEIVER] Sending NACK - retransmit from chunk 929

--- Window [929-1066] ---
  [MQTT:file/data] Chunk 929/4437 (seq=929, size=239, crc32=0x82081781)
  [MQTT:file/data] Chunk 930/4437 (seq=930, size=239, crc32=0x5C1B53EE)
  [MQTT:file/data] Chunk 931/4437 (seq=931, size=239, crc32=0x58D1F57A)
  [MQTT:file/data] Chunk 979/4437 (seq=979, size=239, crc32=0x03B0E82E)
  [MQTT:file/data] Chunk 1029/4437 (seq=1029, size=239, crc32=0x1E920CAE)
  [MQTT:file/data] Chunk 1066/4437 (seq=1066, size=239, crc32=0xF793F9DE)
  ✓ Transmitted 138 chunks in window [929-1066]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 941
  [RECEIVER] Sending NACK - retransmit from chunk 941

--- Window [941-1078] ---
  [MQTT:file/data] Chunk 941/4437 (seq=941, size=239, crc32=0x288075AE)
  [MQTT:file/data] Chunk 942/4437 (seq=942, size=239, crc32=0x50A50CE1)
  [MQTT:file/data] Chunk 943/4437 (seq=943, size=239, crc32=0xE7556DA9)
  [MQTT:file/data] Chunk 991/4437 (seq=991, size=239, crc32=0xA0E90DF7)
  [MQTT:file/data] Chunk 1041/4437 (seq=1041, size=239, crc32=0xCFF7A1EF)
  [MQTT:file/data] Chunk 1078/4437 (seq=1078, size=239, crc32=0xBF57D7F1)
  ✓ Transmitted 138 chunks in window [941-1078]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 945
  [RECEIVER] Sending NACK - retransmit from chunk 945

--- Window [945-1082] ---
  [MQTT:file/data] Chunk 945/4437 (seq=945, size=239, crc32=0xC73A9198)
  [MQTT:file/data] Chunk 946/4437 (seq=946, size=239, crc32=0xA65248A1)
  [MQTT:file/data] Chunk 947/4437 (seq=947, size=239, crc32=0x4E8164CE)
  [MQTT:file/data] Chunk 995/4437 (seq=995, size=239, crc32=0x48C261E0)
  [MQTT:file/data] Chunk 1045/4437 (seq=1045, size=239, crc32=0x14D88FA0)
  [MQTT:file/data] Chunk 1082/4437 (seq=1082, size=239, crc32=0xEFF4476D)
  ✓ Transmitted 138 chunks in window [945-1082]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 964
  [RECEIVER] Sending NACK - retransmit from chunk 964

--- Window [964-1101] ---
  [MQTT:file/data] Chunk 964/4437 (seq=964, size=239, crc32=0x856EB42B)
  [MQTT:file/data] Chunk 965/4437 (seq=965, size=239, crc32=0x62A18177)
  [MQTT:file/data] Chunk 966/4437 (seq=966, size=239, crc32=0x379C23D4)
  [MQTT:file/data] Chunk 1014/4437 (seq=1014, size=239, crc32=0xDF485F96)
  [MQTT:file/data] Chunk 1064/4437 (seq=1064, size=239, crc32=0xE36364EB)
  [MQTT:file/data] Chunk 1101/4437 (seq=1101, size=239, crc32=0x16137ECB)
  ✓ Transmitted 138 chunks in window [964-1101]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 969
  [RECEIVER] Sending NACK - retransmit from chunk 969

--- Window [969-1106] ---
  [MQTT:file/data] Chunk 969/4437 (seq=969, size=239, crc32=0x3831C1B7)
  [MQTT:file/data] Chunk 970/4437 (seq=970, size=239, crc32=0xECE02386)
  [MQTT:file/data] Chunk 971/4437 (seq=971, size=239, crc32=0x54ED2009)
  [MQTT:file/data] Chunk 1019/4437 (seq=1019, size=239, crc32=0x3AA65C38)
  [MQTT:file/data] Chunk 1069/4437 (seq=1069, size=239, crc32=0xC04C77AB)
  [MQTT:file/data] Chunk 1106/4437 (seq=1106, size=239, crc32=0x8CEF8FFB)
  ✓ Transmitted 138 chunks in window [969-1106]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 975
  [RECEIVER] Sending NACK - retransmit from chunk 975

--- Window [975-1112] ---
  [MQTT:file/data] Chunk 975/4437 (seq=975, size=239, crc32=0xE01971D5)
  [MQTT:file/data] Chunk 976/4437 (seq=976, size=239, crc32=0x11C1EB1D)
  [MQTT:file/data] Chunk 977/4437 (seq=977, size=239, crc32=0x35F3C8C8)
  [MQTT:file/data] Chunk 1025/4437 (seq=1025, size=239, crc32=0x438FC607)
  [MQTT:file/data] Chunk 1075/4437 (seq=1075, size=239, crc32=0x402F39E7)
  [MQTT:file/data] Chunk 1112/4437 (seq=1112, size=239, crc32=0x37D26042)
  ✓ Transmitted 138 chunks in window [975-1112]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 979
  [RECEIVER] Sending NACK - retransmit from chunk 979

--- Window [979-1116] ---
  [MQTT:file/data] Chunk 979/4437 (seq=979, size=239, crc32=0x03B0E82E)
  [MQTT:file/data] Chunk 980/4437 (seq=980, size=239, crc32=0xD2CB341B)
  [MQTT:file/data] Chunk 981/4437 (seq=981, size=239, crc32=0x8AB6E1C8)
  [MQTT:file/data] Chunk 1029/4437 (seq=1029, size=239, crc32=0x1E920CAE)
  [MQTT:file/data] Chunk 1079/4437 (seq=1079, size=239, crc32=0x4778E0EC)
  [MQTT:file/data] Chunk 1116/4437 (seq=1116, size=239, crc32=0x0F9DEDCB)
  ✓ Transmitted 138 chunks in window [979-1116]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1011
  [RECEIVER] Sending NACK - retransmit from chunk 1011

--- Window [1011-1148] ---
  [MQTT:file/data] Chunk 1011/4437 (seq=1011, size=239, crc32=0x13D11337)
  [MQTT:file/data] Chunk 1012/4437 (seq=1012, size=239, crc32=0x1D2A05DA)
  [MQTT:file/data] Chunk 1013/4437 (seq=1013, size=239, crc32=0xEDDDFEEF)
  [MQTT:file/data] Chunk 1061/4437 (seq=1061, size=239, crc32=0xFF840639)
  [MQTT:file/data] Chunk 1111/4437 (seq=1111, size=239, crc32=0x6B7F9B0C)
  [MQTT:file/data] Chunk 1148/4437 (seq=1148, size=239, crc32=0x13BDB315)
  ✓ Transmitted 138 chunks in window [1011-1148]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1023
  [RECEIVER] Sending NACK - retransmit from chunk 1023

--- Window [1023-1160] ---
  [MQTT:file/data] Chunk 1023/4437 (seq=1023, size=239, crc32=0x2B4D0A04)
  [MQTT:file/data] Chunk 1024/4437 (seq=1024, size=239, crc32=0x723778F3)
  [MQTT:file/data] Chunk 1025/4437 (seq=1025, size=239, crc32=0x438FC607)
  [MQTT:file/data] Chunk 1073/4437 (seq=1073, size=239, crc32=0x373F0C92)
  [MQTT:file/data] Chunk 1123/4437 (seq=1123, size=239, crc32=0xAECEB64F)
  [MQTT:file/data] Chunk 1160/4437 (seq=1160, size=239, crc32=0xE09318CC)
  ✓ Transmitted 138 chunks in window [1023-1160]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1023
  [RECEIVER] Sending NACK - retransmit from chunk 1023

--- Window [1023-1160] ---
  [MQTT:file/data] Chunk 1023/4437 (seq=1023, size=239, crc32=0x2B4D0A04)
  [MQTT:file/data] Chunk 1024/4437 (seq=1024, size=239, crc32=0x723778F3)
  [MQTT:file/data] Chunk 1025/4437 (seq=1025, size=239, crc32=0x438FC607)
  [MQTT:file/data] Chunk 1073/4437 (seq=1073, size=239, crc32=0x373F0C92)
  [MQTT:file/data] Chunk 1123/4437 (seq=1123, size=239, crc32=0xAECEB64F)
  [MQTT:file/data] Chunk 1160/4437 (seq=1160, size=239, crc32=0xE09318CC)
  ✓ Transmitted 138 chunks in window [1023-1160]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1026
  [RECEIVER] Sending NACK - retransmit from chunk 1026

--- Window [1026-1163] ---
  [MQTT:file/data] Chunk 1026/4437 (seq=1026, size=239, crc32=0x2F7B96AD)
  [MQTT:file/data] Chunk 1027/4437 (seq=1027, size=239, crc32=0x079174B2)
  [MQTT:file/data] Chunk 1028/4437 (seq=1028, size=239, crc32=0x637D19D3)
  [MQTT:file/data] Chunk 1076/4437 (seq=1076, size=239, crc32=0xA6D4971E)
  [MQTT:file/data] Chunk 1126/4437 (seq=1126, size=239, crc32=0xF16F8DA8)
  [MQTT:file/data] Chunk 1163/4437 (seq=1163, size=239, crc32=0x867F5F52)
  ✓ Transmitted 138 chunks in window [1026-1163]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1054
  [RECEIVER] Sending NACK - retransmit from chunk 1054

--- Window [1054-1191] ---
  [MQTT:file/data] Chunk 1054/4437 (seq=1054, size=239, crc32=0xFE0B65B2)
  [MQTT:file/data] Chunk 1055/4437 (seq=1055, size=239, crc32=0x9D4E0E50)
  [MQTT:file/data] Chunk 1056/4437 (seq=1056, size=239, crc32=0x4920186A)
  [MQTT:file/data] Chunk 1104/4437 (seq=1104, size=239, crc32=0x4F87C90D)
  [MQTT:file/data] Chunk 1154/4437 (seq=1154, size=239, crc32=0x841888FF)
  [MQTT:file/data] Chunk 1191/4437 (seq=1191, size=239, crc32=0x60C29D84)
  ✓ Transmitted 138 chunks in window [1054-1191]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1070
  [RECEIVER] Sending NACK - retransmit from chunk 1070

--- Window [1070-1207] ---
  [MQTT:file/data] Chunk 1070/4437 (seq=1070, size=239, crc32=0x8D41D16D)
  [MQTT:file/data] Chunk 1071/4437 (seq=1071, size=239, crc32=0xB55C155F)
  [MQTT:file/data] Chunk 1072/4437 (seq=1072, size=239, crc32=0xADA5584F)
  [MQTT:file/data] Chunk 1120/4437 (seq=1120, size=239, crc32=0x81346490)
  [MQTT:file/data] Chunk 1170/4437 (seq=1170, size=239, crc32=0x412CBB46)
  [MQTT:file/data] Chunk 1207/4437 (seq=1207, size=239, crc32=0xD75C0B4E)
  ✓ Transmitted 138 chunks in window [1070-1207]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1075
  [RECEIVER] Sending NACK - retransmit from chunk 1075

--- Window [1075-1212] ---
  [MQTT:file/data] Chunk 1075/4437 (seq=1075, size=239, crc32=0x402F39E7)
  [MQTT:file/data] Chunk 1076/4437 (seq=1076, size=239, crc32=0xA6D4971E)
  [MQTT:file/data] Chunk 1077/4437 (seq=1077, size=239, crc32=0x5BE81FBD)
  [MQTT:file/data] Chunk 1125/4437 (seq=1125, size=239, crc32=0xE55F90CF)
  [MQTT:file/data] Chunk 1175/4437 (seq=1175, size=239, crc32=0x7048C189)
  [MQTT:file/data] Chunk 1212/4437 (seq=1212, size=239, crc32=0xB70EE3AA)
  ✓ Transmitted 138 chunks in window [1075-1212]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1078
  [RECEIVER] Sending NACK - retransmit from chunk 1078

--- Window [1078-1215] ---
  [MQTT:file/data] Chunk 1078/4437 (seq=1078, size=239, crc32=0xBF57D7F1)
  [MQTT:file/data] Chunk 1079/4437 (seq=1079, size=239, crc32=0x4778E0EC)
  [MQTT:file/data] Chunk 1080/4437 (seq=1080, size=239, crc32=0xB99F9EC3)
  [MQTT:file/data] Chunk 1128/4437 (seq=1128, size=239, crc32=0x91C66BBA)
  [MQTT:file/data] Chunk 1178/4437 (seq=1178, size=239, crc32=0x1BB1EE4B)
  [MQTT:file/data] Chunk 1215/4437 (seq=1215, size=239, crc32=0x66778920)
  ✓ Transmitted 138 chunks in window [1078-1215]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1094
  [RECEIVER] Sending NACK - retransmit from chunk 1094

--- Window [1094-1231] ---
  [MQTT:file/data] Chunk 1094/4437 (seq=1094, size=239, crc32=0xEA23A87D)
  [MQTT:file/data] Chunk 1095/4437 (seq=1095, size=239, crc32=0x142CB300)
  [MQTT:file/data] Chunk 1096/4437 (seq=1096, size=239, crc32=0xBE43EB8E)
  [MQTT:file/data] Chunk 1144/4437 (seq=1144, size=239, crc32=0x12899AC5)
  [MQTT:file/data] Chunk 1194/4437 (seq=1194, size=239, crc32=0xACF29AA4)
  [MQTT:file/data] Chunk 1231/4437 (seq=1231, size=239, crc32=0x70E21E22)
  ✓ Transmitted 138 chunks in window [1094-1231]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1134
  [RECEIVER] Sending NACK - retransmit from chunk 1134

--- Window [1134-1271] ---
  [MQTT:file/data] Chunk 1134/4437 (seq=1134, size=239, crc32=0xAE7DB14D)
  [MQTT:file/data] Chunk 1135/4437 (seq=1135, size=239, crc32=0x5F1755DF)
  [MQTT:file/data] Chunk 1136/4437 (seq=1136, size=239, crc32=0x9F7C93DB)
  [MQTT:file/data] Chunk 1184/4437 (seq=1184, size=239, crc32=0xF9C0972B)
  [MQTT:file/data] Chunk 1234/4437 (seq=1234, size=239, crc32=0x40B9B6CE)
  [MQTT:file/data] Chunk 1271/4437 (seq=1271, size=239, crc32=0x84D18431)
  ✓ Transmitted 138 chunks in window [1134-1271]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1153
  [RECEIVER] Sending NACK - retransmit from chunk 1153

--- Window [1153-1290] ---
  [MQTT:file/data] Chunk 1153/4437 (seq=1153, size=239, crc32=0xB0BC7450)
  [MQTT:file/data] Chunk 1154/4437 (seq=1154, size=239, crc32=0x841888FF)
  [MQTT:file/data] Chunk 1155/4437 (seq=1155, size=239, crc32=0x841FAC7F)
  [MQTT:file/data] Chunk 1203/4437 (seq=1203, size=239, crc32=0xE76112A9)
  [MQTT:file/data] Chunk 1253/4437 (seq=1253, size=239, crc32=0x7DCAB8B4)
  [MQTT:file/data] Chunk 1290/4437 (seq=1290, size=239, crc32=0x534B0CAD)
  ✓ Transmitted 138 chunks in window [1153-1290]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1175
  [RECEIVER] Sending NACK - retransmit from chunk 1175

--- Window [1175-1312] ---
  [MQTT:file/data] Chunk 1175/4437 (seq=1175, size=239, crc32=0x7048C189)
  [MQTT:file/data] Chunk 1176/4437 (seq=1176, size=239, crc32=0x2D65D768)
  [MQTT:file/data] Chunk 1177/4437 (seq=1177, size=239, crc32=0xA4104BB4)
  [MQTT:file/data] Chunk 1225/4437 (seq=1225, size=239, crc32=0xBE9DC9F1)
  [MQTT:file/data] Chunk 1275/4437 (seq=1275, size=239, crc32=0x19130852)
  [MQTT:file/data] Chunk 1312/4437 (seq=1312, size=239, crc32=0x68312BD3)
  ✓ Transmitted 138 chunks in window [1175-1312]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1196
  [RECEIVER] Sending NACK - retransmit from chunk 1196

--- Window [1196-1333] ---
  [MQTT:file/data] Chunk 1196/4437 (seq=1196, size=239, crc32=0x64CFD1A6)
  [MQTT:file/data] Chunk 1197/4437 (seq=1197, size=239, crc32=0x5145CD2C)
  [MQTT:file/data] Chunk 1198/4437 (seq=1198, size=239, crc32=0x89439BBF)
  [MQTT:file/data] Chunk 1246/4437 (seq=1246, size=239, crc32=0x736EB796)
  [MQTT:file/data] Chunk 1296/4437 (seq=1296, size=239, crc32=0xB02AD81E)
  [MQTT:file/data] Chunk 1333/4437 (seq=1333, size=239, crc32=0x66BA683E)
  ✓ Transmitted 138 chunks in window [1196-1333]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1219
  [RECEIVER] Sending NACK - retransmit from chunk 1219

--- Window [1219-1356] ---
  [MQTT:file/data] Chunk 1219/4437 (seq=1219, size=239, crc32=0x62A1DC09)
  [MQTT:file/data] Chunk 1220/4437 (seq=1220, size=239, crc32=0xC0102630)
  [MQTT:file/data] Chunk 1221/4437 (seq=1221, size=239, crc32=0x6684185E)
  [MQTT:file/data] Chunk 1269/4437 (seq=1269, size=239, crc32=0x2236782A)
  [MQTT:file/data] Chunk 1319/4437 (seq=1319, size=239, crc32=0xD283425A)
  [MQTT:file/data] Chunk 1356/4437 (seq=1356, size=239, crc32=0x7FA7B481)
  ✓ Transmitted 138 chunks in window [1219-1356]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1221
  [RECEIVER] Sending NACK - retransmit from chunk 1221

--- Window [1221-1358] ---
  [MQTT:file/data] Chunk 1221/4437 (seq=1221, size=239, crc32=0x6684185E)
  [MQTT:file/data] Chunk 1222/4437 (seq=1222, size=239, crc32=0x79DDC32B)
  [MQTT:file/data] Chunk 1223/4437 (seq=1223, size=239, crc32=0x41F83A8E)
  [MQTT:file/data] Chunk 1271/4437 (seq=1271, size=239, crc32=0x84D18431)
  [MQTT:file/data] Chunk 1321/4437 (seq=1321, size=239, crc32=0xE91EFCC5)
  [MQTT:file/data] Chunk 1358/4437 (seq=1358, size=239, crc32=0xC477B850)
  ✓ Transmitted 138 chunks in window [1221-1358]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1222
  [RECEIVER] Sending NACK - retransmit from chunk 1222

--- Window [1222-1359] ---
  [MQTT:file/data] Chunk 1222/4437 (seq=1222, size=239, crc32=0x79DDC32B)
  [MQTT:file/data] Chunk 1223/4437 (seq=1223, size=239, crc32=0x41F83A8E)
  [MQTT:file/data] Chunk 1224/4437 (seq=1224, size=239, crc32=0xAEAE42CF)
  [MQTT:file/data] Chunk 1272/4437 (seq=1272, size=239, crc32=0x0E0169C3)
  [MQTT:file/data] Chunk 1322/4437 (seq=1322, size=239, crc32=0x555AA323)
  [MQTT:file/data] Chunk 1359/4437 (seq=1359, size=239, crc32=0x8B664069)
  ✓ Transmitted 138 chunks in window [1222-1359]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1228
  [RECEIVER] Sending NACK - retransmit from chunk 1228

--- Window [1228-1365] ---
  [MQTT:file/data] Chunk 1228/4437 (seq=1228, size=239, crc32=0xF54DCD28)
  [MQTT:file/data] Chunk 1229/4437 (seq=1229, size=239, crc32=0x395E9FC4)
  [MQTT:file/data] Chunk 1230/4437 (seq=1230, size=239, crc32=0x861D0EEB)
  [MQTT:file/data] Chunk 1278/4437 (seq=1278, size=239, crc32=0x7AABD346)
  [MQTT:file/data] Chunk 1328/4437 (seq=1328, size=239, crc32=0x66942496)
  [MQTT:file/data] Chunk 1365/4437 (seq=1365, size=239, crc32=0xF151FBE7)
  ✓ Transmitted 138 chunks in window [1228-1365]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1234
  [RECEIVER] Sending NACK - retransmit from chunk 1234

--- Window [1234-1371] ---
  [MQTT:file/data] Chunk 1234/4437 (seq=1234, size=239, crc32=0x40B9B6CE)
  [MQTT:file/data] Chunk 1235/4437 (seq=1235, size=239, crc32=0x06A7DCED)
  [MQTT:file/data] Chunk 1236/4437 (seq=1236, size=239, crc32=0x9A0F3A55)
  [MQTT:file/data] Chunk 1284/4437 (seq=1284, size=239, crc32=0x6022A001)
  [MQTT:file/data] Chunk 1334/4437 (seq=1334, size=239, crc32=0x57E43145)
  [MQTT:file/data] Chunk 1371/4437 (seq=1371, size=239, crc32=0x54D3EA65)
  ✓ Transmitted 138 chunks in window [1234-1371]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1243
  [RECEIVER] Sending NACK - retransmit from chunk 1243

--- Window [1243-1380] ---
  [MQTT:file/data] Chunk 1243/4437 (seq=1243, size=239, crc32=0xE79CAC74)
  [MQTT:file/data] Chunk 1244/4437 (seq=1244, size=239, crc32=0xDE95CC9B)
  [MQTT:file/data] Chunk 1245/4437 (seq=1245, size=239, crc32=0x1AA8849C)
  [MQTT:file/data] Chunk 1293/4437 (seq=1293, size=239, crc32=0xEA7AFCF0)
  [MQTT:file/data] Chunk 1343/4437 (seq=1343, size=239, crc32=0xA8B0B8AC)
  [MQTT:file/data] Chunk 1380/4437 (seq=1380, size=239, crc32=0x03BFF281)
  ✓ Transmitted 138 chunks in window [1243-1380]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1246
  [RECEIVER] Sending NACK - retransmit from chunk 1246

--- Window [1246-1383] ---
  [MQTT:file/data] Chunk 1246/4437 (seq=1246, size=239, crc32=0x736EB796)
  [MQTT:file/data] Chunk 1247/4437 (seq=1247, size=239, crc32=0x53296D50)
  [MQTT:file/data] Chunk 1248/4437 (seq=1248, size=239, crc32=0x0F0A56D2)
  [MQTT:file/data] Chunk 1296/4437 (seq=1296, size=239, crc32=0xB02AD81E)
  [MQTT:file/data] Chunk 1346/4437 (seq=1346, size=239, crc32=0x764C6224)
  [MQTT:file/data] Chunk 1383/4437 (seq=1383, size=239, crc32=0x9B4D6D04)
  ✓ Transmitted 138 chunks in window [1246-1383]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1284
  [RECEIVER] Sending NACK - retransmit from chunk 1284

--- Window [1284-1421] ---
  [MQTT:file/data] Chunk 1284/4437 (seq=1284, size=239, crc32=0x6022A001)
  [MQTT:file/data] Chunk 1285/4437 (seq=1285, size=239, crc32=0xB5FF5526)
  [MQTT:file/data] Chunk 1286/4437 (seq=1286, size=239, crc32=0x52C32C05)
  [MQTT:file/data] Chunk 1334/4437 (seq=1334, size=239, crc32=0x57E43145)
  [MQTT:file/data] Chunk 1384/4437 (seq=1384, size=239, crc32=0xC8FD140B)
  [MQTT:file/data] Chunk 1421/4437 (seq=1421, size=239, crc32=0x63DBA3DB)
  ✓ Transmitted 138 chunks in window [1284-1421]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1303
  [RECEIVER] Sending NACK - retransmit from chunk 1303

--- Window [1303-1440] ---
  [MQTT:file/data] Chunk 1303/4437 (seq=1303, size=239, crc32=0x599D5E8D)
  [MQTT:file/data] Chunk 1304/4437 (seq=1304, size=239, crc32=0xB528AFD2)
  [MQTT:file/data] Chunk 1305/4437 (seq=1305, size=239, crc32=0xCA7CEEE6)
  [MQTT:file/data] Chunk 1353/4437 (seq=1353, size=239, crc32=0x1E50CFE8)
  [MQTT:file/data] Chunk 1403/4437 (seq=1403, size=239, crc32=0x22097C8B)
  [MQTT:file/data] Chunk 1440/4437 (seq=1440, size=239, crc32=0xA9EE37DE)
  ✓ Transmitted 138 chunks in window [1303-1440]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1307
  [RECEIVER] Sending NACK - retransmit from chunk 1307

--- Window [1307-1444] ---
  [MQTT:file/data] Chunk 1307/4437 (seq=1307, size=239, crc32=0xBAC80081)
  [MQTT:file/data] Chunk 1308/4437 (seq=1308, size=239, crc32=0x9E2E3FDF)
  [MQTT:file/data] Chunk 1309/4437 (seq=1309, size=239, crc32=0xF7F7E1B5)
  [MQTT:file/data] Chunk 1357/4437 (seq=1357, size=239, crc32=0x9F1663A0)
  [MQTT:file/data] Chunk 1407/4437 (seq=1407, size=239, crc32=0xB8230C39)
  [MQTT:file/data] Chunk 1444/4437 (seq=1444, size=239, crc32=0x1A380FA8)
  ✓ Transmitted 138 chunks in window [1307-1444]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1334
  [RECEIVER] Sending NACK - retransmit from chunk 1334

--- Window [1334-1471] ---
  [MQTT:file/data] Chunk 1334/4437 (seq=1334, size=239, crc32=0x57E43145)
  [MQTT:file/data] Chunk 1335/4437 (seq=1335, size=239, crc32=0x756D0EC8)
  [MQTT:file/data] Chunk 1336/4437 (seq=1336, size=239, crc32=0xBBDA59B3)
  [MQTT:file/data] Chunk 1384/4437 (seq=1384, size=239, crc32=0xC8FD140B)
  [MQTT:file/data] Chunk 1434/4437 (seq=1434, size=239, crc32=0xD7662752)
  [MQTT:file/data] Chunk 1471/4437 (seq=1471, size=239, crc32=0x76910C29)
  ✓ Transmitted 138 chunks in window [1334-1471]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1386
  [RECEIVER] Sending NACK - retransmit from chunk 1386

--- Window [1386-1523] ---
  [MQTT:file/data] Chunk 1386/4437 (seq=1386, size=239, crc32=0xBCC1D86A)
  [MQTT:file/data] Chunk 1387/4437 (seq=1387, size=239, crc32=0x775859B6)
  [MQTT:file/data] Chunk 1388/4437 (seq=1388, size=239, crc32=0x7EC95B8E)
  [MQTT:file/data] Chunk 1436/4437 (seq=1436, size=239, crc32=0x7AFBFA13)
  [MQTT:file/data] Chunk 1486/4437 (seq=1486, size=239, crc32=0x47EF82D0)
  [MQTT:file/data] Chunk 1523/4437 (seq=1523, size=239, crc32=0xA8C5623B)
  ✓ Transmitted 138 chunks in window [1386-1523]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1423
  [RECEIVER] Sending NACK - retransmit from chunk 1423

--- Window [1423-1560] ---
  [MQTT:file/data] Chunk 1423/4437 (seq=1423, size=239, crc32=0x666DE888)
  [MQTT:file/data] Chunk 1424/4437 (seq=1424, size=239, crc32=0x9A4DE245)
  [MQTT:file/data] Chunk 1425/4437 (seq=1425, size=239, crc32=0xE92C55FF)
  [MQTT:file/data] Chunk 1473/4437 (seq=1473, size=239, crc32=0x7F917621)
  [MQTT:file/data] Chunk 1523/4437 (seq=1523, size=239, crc32=0xA8C5623B)
  [MQTT:file/data] Chunk 1560/4437 (seq=1560, size=239, crc32=0x51A0D467)
  ✓ Transmitted 138 chunks in window [1423-1560]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1433
  [RECEIVER] Sending NACK - retransmit from chunk 1433

--- Window [1433-1570] ---
  [MQTT:file/data] Chunk 1433/4437 (seq=1433, size=239, crc32=0x74A0686D)
  [MQTT:file/data] Chunk 1434/4437 (seq=1434, size=239, crc32=0xD7662752)
  [MQTT:file/data] Chunk 1435/4437 (seq=1435, size=239, crc32=0xA52FDF63)
  [MQTT:file/data] Chunk 1483/4437 (seq=1483, size=239, crc32=0xD1CB70F7)
  [MQTT:file/data] Chunk 1533/4437 (seq=1533, size=239, crc32=0xBBE78A3D)
  [MQTT:file/data] Chunk 1570/4437 (seq=1570, size=239, crc32=0x6BF0AE77)
  ✓ Transmitted 138 chunks in window [1433-1570]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1445
  [RECEIVER] Sending NACK - retransmit from chunk 1445

--- Window [1445-1582] ---
  [MQTT:file/data] Chunk 1445/4437 (seq=1445, size=239, crc32=0x49672A65)
  [MQTT:file/data] Chunk 1446/4437 (seq=1446, size=239, crc32=0xE6E01AD3)
  [MQTT:file/data] Chunk 1447/4437 (seq=1447, size=239, crc32=0x87FA1B9A)
  [MQTT:file/data] Chunk 1495/4437 (seq=1495, size=239, crc32=0x0A5FC05A)
  [MQTT:file/data] Chunk 1545/4437 (seq=1545, size=239, crc32=0xFED9C53F)
  [MQTT:file/data] Chunk 1582/4437 (seq=1582, size=239, crc32=0x7FFF1EE4)
  ✓ Transmitted 138 chunks in window [1445-1582]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1494
  [RECEIVER] Sending NACK - retransmit from chunk 1494

--- Window [1494-1631] ---
  [MQTT:file/data] Chunk 1494/4437 (seq=1494, size=239, crc32=0xB373C550)
  [MQTT:file/data] Chunk 1495/4437 (seq=1495, size=239, crc32=0x0A5FC05A)
  [MQTT:file/data] Chunk 1496/4437 (seq=1496, size=239, crc32=0x6B67E7AC)
  [MQTT:file/data] Chunk 1544/4437 (seq=1544, size=239, crc32=0x11BFF8CA)
  [MQTT:file/data] Chunk 1594/4437 (seq=1594, size=239, crc32=0x0FE4B4EB)
  [MQTT:file/data] Chunk 1631/4437 (seq=1631, size=239, crc32=0x9C66BD0C)
  ✓ Transmitted 138 chunks in window [1494-1631]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1505
  [RECEIVER] Sending NACK - retransmit from chunk 1505

--- Window [1505-1642] ---
  [MQTT:file/data] Chunk 1505/4437 (seq=1505, size=239, crc32=0x1053297B)
  [MQTT:file/data] Chunk 1506/4437 (seq=1506, size=239, crc32=0x6E30E6D9)
  [MQTT:file/data] Chunk 1507/4437 (seq=1507, size=239, crc32=0x5AC079D7)
  [MQTT:file/data] Chunk 1555/4437 (seq=1555, size=239, crc32=0xEDF8F803)
  [MQTT:file/data] Chunk 1605/4437 (seq=1605, size=239, crc32=0x9FAE34B2)
  [MQTT:file/data] Chunk 1642/4437 (seq=1642, size=239, crc32=0xCCD568B2)
  ✓ Transmitted 138 chunks in window [1505-1642]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1509
  [RECEIVER] Sending NACK - retransmit from chunk 1509

--- Window [1509-1646] ---
  [MQTT:file/data] Chunk 1509/4437 (seq=1509, size=239, crc32=0x19FDEC20)
  [MQTT:file/data] Chunk 1510/4437 (seq=1510, size=239, crc32=0xEFE21A3A)
  [MQTT:file/data] Chunk 1511/4437 (seq=1511, size=239, crc32=0xDD0145AF)
  [MQTT:file/data] Chunk 1559/4437 (seq=1559, size=239, crc32=0x7459BB45)
  [MQTT:file/data] Chunk 1609/4437 (seq=1609, size=239, crc32=0xCAD16309)
  [MQTT:file/data] Chunk 1646/4437 (seq=1646, size=239, crc32=0x46D4D934)
  ✓ Transmitted 138 chunks in window [1509-1646]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1526
  [RECEIVER] Sending NACK - retransmit from chunk 1526

--- Window [1526-1663] ---
  [MQTT:file/data] Chunk 1526/4437 (seq=1526, size=239, crc32=0x4E7F9702)
  [MQTT:file/data] Chunk 1527/4437 (seq=1527, size=239, crc32=0x9C24B93B)
  [MQTT:file/data] Chunk 1528/4437 (seq=1528, size=239, crc32=0x947D33FE)
  [MQTT:file/data] Chunk 1576/4437 (seq=1576, size=239, crc32=0x9BFE5346)
  [MQTT:file/data] Chunk 1626/4437 (seq=1626, size=239, crc32=0x5D216421)
  [MQTT:file/data] Chunk 1663/4437 (seq=1663, size=239, crc32=0x1EC6EF3B)
  ✓ Transmitted 138 chunks in window [1526-1663]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1537
  [RECEIVER] Sending NACK - retransmit from chunk 1537

--- Window [1537-1674] ---
  [MQTT:file/data] Chunk 1537/4437 (seq=1537, size=239, crc32=0xBD75B8D5)
  [MQTT:file/data] Chunk 1538/4437 (seq=1538, size=239, crc32=0x56642F16)
  [MQTT:file/data] Chunk 1539/4437 (seq=1539, size=239, crc32=0x9A58BFC2)
  [MQTT:file/data] Chunk 1587/4437 (seq=1587, size=239, crc32=0xF3BC252F)
  [MQTT:file/data] Chunk 1637/4437 (seq=1637, size=239, crc32=0x10CBDF0C)
  [MQTT:file/data] Chunk 1674/4437 (seq=1674, size=239, crc32=0xE721BFCE)
  ✓ Transmitted 138 chunks in window [1537-1674]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1553
  [RECEIVER] Sending NACK - retransmit from chunk 1553

--- Window [1553-1690] ---
  [MQTT:file/data] Chunk 1553/4437 (seq=1553, size=239, crc32=0xD44DA441)
  [MQTT:file/data] Chunk 1554/4437 (seq=1554, size=239, crc32=0x8CF002DE)
  [MQTT:file/data] Chunk 1555/4437 (seq=1555, size=239, crc32=0xEDF8F803)
  [MQTT:file/data] Chunk 1603/4437 (seq=1603, size=239, crc32=0x50330919)
  [MQTT:file/data] Chunk 1653/4437 (seq=1653, size=239, crc32=0x42D14D8D)
  [MQTT:file/data] Chunk 1690/4437 (seq=1690, size=239, crc32=0x3D070023)
  ✓ Transmitted 138 chunks in window [1553-1690]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1559
  [RECEIVER] Sending NACK - retransmit from chunk 1559

--- Window [1559-1696] ---
  [MQTT:file/data] Chunk 1559/4437 (seq=1559, size=239, crc32=0x7459BB45)
  [MQTT:file/data] Chunk 1560/4437 (seq=1560, size=239, crc32=0x51A0D467)
  [MQTT:file/data] Chunk 1561/4437 (seq=1561, size=239, crc32=0x287B044C)
  [MQTT:file/data] Chunk 1609/4437 (seq=1609, size=239, crc32=0xCAD16309)
  [MQTT:file/data] Chunk 1659/4437 (seq=1659, size=239, crc32=0x5155F8D2)
  [MQTT:file/data] Chunk 1696/4437 (seq=1696, size=239, crc32=0xA08D3756)
  ✓ Transmitted 138 chunks in window [1559-1696]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1593
  [RECEIVER] Sending NACK - retransmit from chunk 1593

--- Window [1593-1730] ---
  [MQTT:file/data] Chunk 1593/4437 (seq=1593, size=239, crc32=0xBAEA8169)
  [MQTT:file/data] Chunk 1594/4437 (seq=1594, size=239, crc32=0x0FE4B4EB)
  [MQTT:file/data] Chunk 1595/4437 (seq=1595, size=239, crc32=0xA2811DC0)
  [MQTT:file/data] Chunk 1643/4437 (seq=1643, size=239, crc32=0x896935F2)
  [MQTT:file/data] Chunk 1693/4437 (seq=1693, size=239, crc32=0xE3050FC7)
  [MQTT:file/data] Chunk 1730/4437 (seq=1730, size=239, crc32=0x29E6C52A)
  ✓ Transmitted 138 chunks in window [1593-1730]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1642
  [RECEIVER] Sending NACK - retransmit from chunk 1642

--- Window [1642-1779] ---
  [MQTT:file/data] Chunk 1642/4437 (seq=1642, size=239, crc32=0xCCD568B2)
  [MQTT:file/data] Chunk 1643/4437 (seq=1643, size=239, crc32=0x896935F2)
  [MQTT:file/data] Chunk 1644/4437 (seq=1644, size=239, crc32=0x26A930DF)
  [MQTT:file/data] Chunk 1692/4437 (seq=1692, size=239, crc32=0x92F79C09)
  [MQTT:file/data] Chunk 1742/4437 (seq=1742, size=239, crc32=0x54192AC2)
  [MQTT:file/data] Chunk 1779/4437 (seq=1779, size=239, crc32=0x93403AD7)
  ✓ Transmitted 138 chunks in window [1642-1779]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1655
  [RECEIVER] Sending NACK - retransmit from chunk 1655

--- Window [1655-1792] ---
  [MQTT:file/data] Chunk 1655/4437 (seq=1655, size=239, crc32=0xE0958150)
  [MQTT:file/data] Chunk 1656/4437 (seq=1656, size=239, crc32=0x85E8BFBF)
  [MQTT:file/data] Chunk 1657/4437 (seq=1657, size=239, crc32=0x9BD89AF9)
  [MQTT:file/data] Chunk 1705/4437 (seq=1705, size=239, crc32=0x251E1679)
  [MQTT:file/data] Chunk 1755/4437 (seq=1755, size=239, crc32=0x469B5183)
  [MQTT:file/data] Chunk 1792/4437 (seq=1792, size=239, crc32=0x29BF6375)
  ✓ Transmitted 138 chunks in window [1655-1792]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1656
  [RECEIVER] Sending NACK - retransmit from chunk 1656

--- Window [1656-1793] ---
  [MQTT:file/data] Chunk 1656/4437 (seq=1656, size=239, crc32=0x85E8BFBF)
  [MQTT:file/data] Chunk 1657/4437 (seq=1657, size=239, crc32=0x9BD89AF9)
  [MQTT:file/data] Chunk 1658/4437 (seq=1658, size=239, crc32=0xF2D76642)
  [MQTT:file/data] Chunk 1706/4437 (seq=1706, size=239, crc32=0x8D3BE98F)
  [MQTT:file/data] Chunk 1756/4437 (seq=1756, size=239, crc32=0x7D59A427)
  [MQTT:file/data] Chunk 1793/4437 (seq=1793, size=239, crc32=0x876AFE9B)
  ✓ Transmitted 138 chunks in window [1656-1793]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1703
  [RECEIVER] Sending NACK - retransmit from chunk 1703

--- Window [1703-1840] ---
  [MQTT:file/data] Chunk 1703/4437 (seq=1703, size=239, crc32=0x53A79E69)
  [MQTT:file/data] Chunk 1704/4437 (seq=1704, size=239, crc32=0xC31F0D7A)
  [MQTT:file/data] Chunk 1705/4437 (seq=1705, size=239, crc32=0x251E1679)
  [MQTT:file/data] Chunk 1753/4437 (seq=1753, size=239, crc32=0xAABD23E7)
  [MQTT:file/data] Chunk 1803/4437 (seq=1803, size=239, crc32=0xEDEDF8F7)
  [MQTT:file/data] Chunk 1840/4437 (seq=1840, size=239, crc32=0x80302B0E)
  ✓ Transmitted 138 chunks in window [1703-1840]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1709
  [RECEIVER] Sending NACK - retransmit from chunk 1709

--- Window [1709-1846] ---
  [MQTT:file/data] Chunk 1709/4437 (seq=1709, size=239, crc32=0x8359252E)
  [MQTT:file/data] Chunk 1710/4437 (seq=1710, size=239, crc32=0x6622CC57)
  [MQTT:file/data] Chunk 1711/4437 (seq=1711, size=239, crc32=0xFA8D3AE7)
  [MQTT:file/data] Chunk 1759/4437 (seq=1759, size=239, crc32=0x4B3488CA)
  [MQTT:file/data] Chunk 1809/4437 (seq=1809, size=239, crc32=0x4AD1BB14)
  [MQTT:file/data] Chunk 1846/4437 (seq=1846, size=239, crc32=0x1DA1C722)
  ✓ Transmitted 138 chunks in window [1709-1846]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1731
  [RECEIVER] Sending NACK - retransmit from chunk 1731

--- Window [1731-1868] ---
  [MQTT:file/data] Chunk 1731/4437 (seq=1731, size=239, crc32=0x740D2B7C)
  [MQTT:file/data] Chunk 1732/4437 (seq=1732, size=239, crc32=0xEF175268)
  [MQTT:file/data] Chunk 1733/4437 (seq=1733, size=239, crc32=0x751BA1F5)
  [MQTT:file/data] Chunk 1781/4437 (seq=1781, size=239, crc32=0x840BDC02)
  [MQTT:file/data] Chunk 1831/4437 (seq=1831, size=239, crc32=0xE44AD192)
  [MQTT:file/data] Chunk 1868/4437 (seq=1868, size=239, crc32=0x6671F3DB)
  ✓ Transmitted 138 chunks in window [1731-1868]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1741
  [RECEIVER] Sending NACK - retransmit from chunk 1741

--- Window [1741-1878] ---
  [MQTT:file/data] Chunk 1741/4437 (seq=1741, size=239, crc32=0xC803BAD6)
  [MQTT:file/data] Chunk 1742/4437 (seq=1742, size=239, crc32=0x54192AC2)
  [MQTT:file/data] Chunk 1743/4437 (seq=1743, size=239, crc32=0xE54E3706)
  [MQTT:file/data] Chunk 1791/4437 (seq=1791, size=239, crc32=0x73FBCEF5)
  [MQTT:file/data] Chunk 1841/4437 (seq=1841, size=239, crc32=0x20E3DB74)
  [MQTT:file/data] Chunk 1878/4437 (seq=1878, size=239, crc32=0xE6027DD4)
  ✓ Transmitted 138 chunks in window [1741-1878]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1763
  [RECEIVER] Sending NACK - retransmit from chunk 1763

--- Window [1763-1900] ---
  [MQTT:file/data] Chunk 1763/4437 (seq=1763, size=239, crc32=0x05B65C40)
  [MQTT:file/data] Chunk 1764/4437 (seq=1764, size=239, crc32=0xF895F5C5)
  [MQTT:file/data] Chunk 1765/4437 (seq=1765, size=239, crc32=0xEA9A4971)
  [MQTT:file/data] Chunk 1813/4437 (seq=1813, size=239, crc32=0x7B103930)
  [MQTT:file/data] Chunk 1863/4437 (seq=1863, size=239, crc32=0x82AB13A7)
  [MQTT:file/data] Chunk 1900/4437 (seq=1900, size=239, crc32=0x9A85E716)
  ✓ Transmitted 138 chunks in window [1763-1900]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1783
  [RECEIVER] Sending NACK - retransmit from chunk 1783

--- Window [1783-1920] ---
  [MQTT:file/data] Chunk 1783/4437 (seq=1783, size=239, crc32=0x807CBEF1)
  [MQTT:file/data] Chunk 1784/4437 (seq=1784, size=239, crc32=0x22B2D615)
  [MQTT:file/data] Chunk 1785/4437 (seq=1785, size=239, crc32=0xFC965D11)
  [MQTT:file/data] Chunk 1833/4437 (seq=1833, size=239, crc32=0x7002F594)
  [MQTT:file/data] Chunk 1883/4437 (seq=1883, size=239, crc32=0x5348B734)
  [MQTT:file/data] Chunk 1920/4437 (seq=1920, size=239, crc32=0x80D48B1A)
  ✓ Transmitted 138 chunks in window [1783-1920]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1793
  [RECEIVER] Sending NACK - retransmit from chunk 1793

--- Window [1793-1930] ---
  [MQTT:file/data] Chunk 1793/4437 (seq=1793, size=239, crc32=0x876AFE9B)
  [MQTT:file/data] Chunk 1794/4437 (seq=1794, size=239, crc32=0xE297D2CF)
  [MQTT:file/data] Chunk 1795/4437 (seq=1795, size=239, crc32=0x91D4191F)
  [MQTT:file/data] Chunk 1843/4437 (seq=1843, size=239, crc32=0x92F20AD8)
  [MQTT:file/data] Chunk 1893/4437 (seq=1893, size=239, crc32=0x8A9D2EE1)
  [MQTT:file/data] Chunk 1930/4437 (seq=1930, size=239, crc32=0xA04BCA94)
  ✓ Transmitted 138 chunks in window [1793-1930]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1797
  [RECEIVER] Sending NACK - retransmit from chunk 1797

--- Window [1797-1934] ---
  [MQTT:file/data] Chunk 1797/4437 (seq=1797, size=239, crc32=0x5BAE5743)
  [MQTT:file/data] Chunk 1798/4437 (seq=1798, size=239, crc32=0x888E4733)
  [MQTT:file/data] Chunk 1799/4437 (seq=1799, size=239, crc32=0xC7798F7E)
  [MQTT:file/data] Chunk 1847/4437 (seq=1847, size=239, crc32=0x59A5B8F3)
  [MQTT:file/data] Chunk 1897/4437 (seq=1897, size=239, crc32=0xDEB0D7BD)
  [MQTT:file/data] Chunk 1934/4437 (seq=1934, size=239, crc32=0x6D715557)
  ✓ Transmitted 138 chunks in window [1797-1934]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1807
  [RECEIVER] Sending NACK - retransmit from chunk 1807

--- Window [1807-1944] ---
  [MQTT:file/data] Chunk 1807/4437 (seq=1807, size=239, crc32=0x95ECA260)
  [MQTT:file/data] Chunk 1808/4437 (seq=1808, size=239, crc32=0x150F48C2)
  [MQTT:file/data] Chunk 1809/4437 (seq=1809, size=239, crc32=0x4AD1BB14)
  [MQTT:file/data] Chunk 1857/4437 (seq=1857, size=239, crc32=0xF459EB12)
  [MQTT:file/data] Chunk 1907/4437 (seq=1907, size=239, crc32=0x5F47E1D8)
  [MQTT:file/data] Chunk 1944/4437 (seq=1944, size=239, crc32=0xEF26B2BC)
  ✓ Transmitted 138 chunks in window [1807-1944]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1856
  [RECEIVER] Sending NACK - retransmit from chunk 1856

--- Window [1856-1993] ---
  [MQTT:file/data] Chunk 1856/4437 (seq=1856, size=239, crc32=0xE896C4D2)
  [MQTT:file/data] Chunk 1857/4437 (seq=1857, size=239, crc32=0xF459EB12)
  [MQTT:file/data] Chunk 1858/4437 (seq=1858, size=239, crc32=0x65DE0F91)
  [MQTT:file/data] Chunk 1906/4437 (seq=1906, size=239, crc32=0xE13BB7B3)
  [MQTT:file/data] Chunk 1956/4437 (seq=1956, size=239, crc32=0x3ED23F57)
  [MQTT:file/data] Chunk 1993/4437 (seq=1993, size=239, crc32=0x9C568EBD)
  ✓ Transmitted 138 chunks in window [1856-1993]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1860
  [RECEIVER] Sending NACK - retransmit from chunk 1860

--- Window [1860-1997] ---
  [MQTT:file/data] Chunk 1860/4437 (seq=1860, size=239, crc32=0x07B63E73)
  [MQTT:file/data] Chunk 1861/4437 (seq=1861, size=239, crc32=0x6B30D1B3)
  [MQTT:file/data] Chunk 1862/4437 (seq=1862, size=239, crc32=0x45BEEFF7)
  [MQTT:file/data] Chunk 1910/4437 (seq=1910, size=239, crc32=0x4C17EAB1)
  [MQTT:file/data] Chunk 1960/4437 (seq=1960, size=239, crc32=0x89212379)
  [MQTT:file/data] Chunk 1997/4437 (seq=1997, size=239, crc32=0x3DD119AE)
  ✓ Transmitted 138 chunks in window [1860-1997]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1861
  [RECEIVER] Sending NACK - retransmit from chunk 1861

--- Window [1861-1998] ---
  [MQTT:file/data] Chunk 1861/4437 (seq=1861, size=239, crc32=0x6B30D1B3)
  [MQTT:file/data] Chunk 1862/4437 (seq=1862, size=239, crc32=0x45BEEFF7)
  [MQTT:file/data] Chunk 1863/4437 (seq=1863, size=239, crc32=0x82AB13A7)
  [MQTT:file/data] Chunk 1911/4437 (seq=1911, size=239, crc32=0xEE401053)
  [MQTT:file/data] Chunk 1961/4437 (seq=1961, size=239, crc32=0x87337841)
  [MQTT:file/data] Chunk 1998/4437 (seq=1998, size=239, crc32=0x28E3F037)
  ✓ Transmitted 138 chunks in window [1861-1998]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1930
  [RECEIVER] Sending NACK - retransmit from chunk 1930

--- Window [1930-2067] ---
  [MQTT:file/data] Chunk 1930/4437 (seq=1930, size=239, crc32=0xA04BCA94)
  [MQTT:file/data] Chunk 1931/4437 (seq=1931, size=239, crc32=0x19A94920)
  [MQTT:file/data] Chunk 1932/4437 (seq=1932, size=239, crc32=0x3D39B658)
  [MQTT:file/data] Chunk 1980/4437 (seq=1980, size=239, crc32=0x6D041F11)
  [MQTT:file/data] Chunk 2030/4437 (seq=2030, size=239, crc32=0x9F0DE12F)
  [MQTT:file/data] Chunk 2067/4437 (seq=2067, size=239, crc32=0xDD12A624)
  ✓ Transmitted 138 chunks in window [1930-2067]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1942
  [RECEIVER] Sending NACK - retransmit from chunk 1942

--- Window [1942-2079] ---
  [MQTT:file/data] Chunk 1942/4437 (seq=1942, size=239, crc32=0x3F2F6F61)
  [MQTT:file/data] Chunk 1943/4437 (seq=1943, size=239, crc32=0xAFD69172)
  [MQTT:file/data] Chunk 1944/4437 (seq=1944, size=239, crc32=0xEF26B2BC)
  [MQTT:file/data] Chunk 1992/4437 (seq=1992, size=239, crc32=0xDC7B6127)
  [MQTT:file/data] Chunk 2042/4437 (seq=2042, size=239, crc32=0x629FD404)
  [MQTT:file/data] Chunk 2079/4437 (seq=2079, size=239, crc32=0x0DF8B462)
  ✓ Transmitted 138 chunks in window [1942-2079]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 1953
  [RECEIVER] Sending NACK - retransmit from chunk 1953

--- Window [1953-2090] ---
  [MQTT:file/data] Chunk 1953/4437 (seq=1953, size=239, crc32=0xDB970CD2)
  [MQTT:file/data] Chunk 1954/4437 (seq=1954, size=239, crc32=0x3F142EA6)
  [MQTT:file/data] Chunk 1955/4437 (seq=1955, size=239, crc32=0xD11E58FC)
  [MQTT:file/data] Chunk 2003/4437 (seq=2003, size=239, crc32=0x01B848F6)
  [MQTT:file/data] Chunk 2053/4437 (seq=2053, size=239, crc32=0x9090848E)
  [MQTT:file/data] Chunk 2090/4437 (seq=2090, size=239, crc32=0x8FB95D73)
  ✓ Transmitted 138 chunks in window [1953-2090]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2016
  [RECEIVER] Sending NACK - retransmit from chunk 2016

--- Window [2016-2153] ---
  [MQTT:file/data] Chunk 2016/4437 (seq=2016, size=239, crc32=0xC3F65AA9)
  [MQTT:file/data] Chunk 2017/4437 (seq=2017, size=239, crc32=0x3EE49316)
  [MQTT:file/data] Chunk 2018/4437 (seq=2018, size=239, crc32=0x8EDB6AC5)
  [MQTT:file/data] Chunk 2066/4437 (seq=2066, size=239, crc32=0xCDC66454)
  [MQTT:file/data] Chunk 2116/4437 (seq=2116, size=239, crc32=0x8AA8C65C)
  [MQTT:file/data] Chunk 2153/4437 (seq=2153, size=239, crc32=0x277FA36C)
  ✓ Transmitted 138 chunks in window [2016-2153]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2024
  [RECEIVER] Sending NACK - retransmit from chunk 2024

--- Window [2024-2161] ---
  [MQTT:file/data] Chunk 2024/4437 (seq=2024, size=239, crc32=0x800E1198)
  [MQTT:file/data] Chunk 2025/4437 (seq=2025, size=239, crc32=0xFB3F7DD0)
  [MQTT:file/data] Chunk 2026/4437 (seq=2026, size=239, crc32=0x06A6F07E)
  [MQTT:file/data] Chunk 2074/4437 (seq=2074, size=239, crc32=0xF938AE40)
  [MQTT:file/data] Chunk 2124/4437 (seq=2124, size=239, crc32=0xAA21D7EB)
  [MQTT:file/data] Chunk 2161/4437 (seq=2161, size=239, crc32=0xE594A4ED)
  ✓ Transmitted 138 chunks in window [2024-2161]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2071
  [RECEIVER] Sending NACK - retransmit from chunk 2071

--- Window [2071-2208] ---
  [MQTT:file/data] Chunk 2071/4437 (seq=2071, size=239, crc32=0x561E6B7A)
  [MQTT:file/data] Chunk 2072/4437 (seq=2072, size=239, crc32=0x9617BBB1)
  [MQTT:file/data] Chunk 2073/4437 (seq=2073, size=239, crc32=0xAF8AAC86)
  [MQTT:file/data] Chunk 2121/4437 (seq=2121, size=239, crc32=0x269135D2)
  [MQTT:file/data] Chunk 2171/4437 (seq=2171, size=239, crc32=0x66D77C2E)
  [MQTT:file/data] Chunk 2208/4437 (seq=2208, size=239, crc32=0x6C418F69)
  ✓ Transmitted 138 chunks in window [2071-2208]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2085
  [RECEIVER] Sending NACK - retransmit from chunk 2085

--- Window [2085-2222] ---
  [MQTT:file/data] Chunk 2085/4437 (seq=2085, size=239, crc32=0xC5B7CAF8)
  [MQTT:file/data] Chunk 2086/4437 (seq=2086, size=239, crc32=0xBCAA938A)
  [MQTT:file/data] Chunk 2087/4437 (seq=2087, size=239, crc32=0xC3630EAF)
  [MQTT:file/data] Chunk 2135/4437 (seq=2135, size=239, crc32=0x6190E4E0)
  [MQTT:file/data] Chunk 2185/4437 (seq=2185, size=239, crc32=0x04F3EE01)
  [MQTT:file/data] Chunk 2222/4437 (seq=2222, size=239, crc32=0x07BBD49C)
  ✓ Transmitted 138 chunks in window [2085-2222]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2091
  [RECEIVER] Sending NACK - retransmit from chunk 2091

--- Window [2091-2228] ---
  [MQTT:file/data] Chunk 2091/4437 (seq=2091, size=239, crc32=0x396E4D2D)
  [MQTT:file/data] Chunk 2092/4437 (seq=2092, size=239, crc32=0x3DE7D233)
  [MQTT:file/data] Chunk 2093/4437 (seq=2093, size=239, crc32=0x783ADA77)
  [MQTT:file/data] Chunk 2141/4437 (seq=2141, size=239, crc32=0xF5D643C4)
  [MQTT:file/data] Chunk 2191/4437 (seq=2191, size=239, crc32=0xD2B643AF)
  [MQTT:file/data] Chunk 2228/4437 (seq=2228, size=239, crc32=0xDF47E943)
  ✓ Transmitted 138 chunks in window [2091-2228]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2096
  [RECEIVER] Sending NACK - retransmit from chunk 2096

--- Window [2096-2233] ---
  [MQTT:file/data] Chunk 2096/4437 (seq=2096, size=239, crc32=0x5DC2244F)
  [MQTT:file/data] Chunk 2097/4437 (seq=2097, size=239, crc32=0xE62ED412)
  [MQTT:file/data] Chunk 2098/4437 (seq=2098, size=239, crc32=0x953F26C5)
  [MQTT:file/data] Chunk 2146/4437 (seq=2146, size=239, crc32=0x6A40F139)
  [MQTT:file/data] Chunk 2196/4437 (seq=2196, size=239, crc32=0x64570219)
  [MQTT:file/data] Chunk 2233/4437 (seq=2233, size=239, crc32=0x91123E8D)
  ✓ Transmitted 138 chunks in window [2096-2233]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2097
  [RECEIVER] Sending NACK - retransmit from chunk 2097

--- Window [2097-2234] ---
  [MQTT:file/data] Chunk 2097/4437 (seq=2097, size=239, crc32=0xE62ED412)
  [MQTT:file/data] Chunk 2098/4437 (seq=2098, size=239, crc32=0x953F26C5)
  [MQTT:file/data] Chunk 2099/4437 (seq=2099, size=239, crc32=0x6C6C2350)
  [MQTT:file/data] Chunk 2147/4437 (seq=2147, size=239, crc32=0xF966D9D2)
  [MQTT:file/data] Chunk 2197/4437 (seq=2197, size=239, crc32=0x836476A0)
  [MQTT:file/data] Chunk 2234/4437 (seq=2234, size=239, crc32=0x6A8DFECB)
  ✓ Transmitted 138 chunks in window [2097-2234]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2103
  [RECEIVER] Sending NACK - retransmit from chunk 2103

--- Window [2103-2240] ---
  [MQTT:file/data] Chunk 2103/4437 (seq=2103, size=239, crc32=0xF97EBD1C)
  [MQTT:file/data] Chunk 2104/4437 (seq=2104, size=239, crc32=0x8F786942)
  [MQTT:file/data] Chunk 2105/4437 (seq=2105, size=239, crc32=0x8DEA25F7)
  [MQTT:file/data] Chunk 2153/4437 (seq=2153, size=239, crc32=0x277FA36C)
  [MQTT:file/data] Chunk 2203/4437 (seq=2203, size=239, crc32=0xF71202D5)
  [MQTT:file/data] Chunk 2240/4437 (seq=2240, size=239, crc32=0x95023628)
  ✓ Transmitted 138 chunks in window [2103-2240]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2103
  [RECEIVER] Sending NACK - retransmit from chunk 2103

--- Window [2103-2240] ---
  [MQTT:file/data] Chunk 2103/4437 (seq=2103, size=239, crc32=0xF97EBD1C)
  [MQTT:file/data] Chunk 2104/4437 (seq=2104, size=239, crc32=0x8F786942)
  [MQTT:file/data] Chunk 2105/4437 (seq=2105, size=239, crc32=0x8DEA25F7)
  [MQTT:file/data] Chunk 2153/4437 (seq=2153, size=239, crc32=0x277FA36C)
  [MQTT:file/data] Chunk 2203/4437 (seq=2203, size=239, crc32=0xF71202D5)
  [MQTT:file/data] Chunk 2240/4437 (seq=2240, size=239, crc32=0x95023628)
  ✓ Transmitted 138 chunks in window [2103-2240]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2103
  [RECEIVER] Sending NACK - retransmit from chunk 2103

--- Window [2103-2240] ---
  [MQTT:file/data] Chunk 2103/4437 (seq=2103, size=239, crc32=0xF97EBD1C)
  [MQTT:file/data] Chunk 2104/4437 (seq=2104, size=239, crc32=0x8F786942)
  [MQTT:file/data] Chunk 2105/4437 (seq=2105, size=239, crc32=0x8DEA25F7)
  [MQTT:file/data] Chunk 2153/4437 (seq=2153, size=239, crc32=0x277FA36C)
  [MQTT:file/data] Chunk 2203/4437 (seq=2203, size=239, crc32=0xF71202D5)
  [MQTT:file/data] Chunk 2240/4437 (seq=2240, size=239, crc32=0x95023628)
  ✓ Transmitted 138 chunks in window [2103-2240]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2112
  [RECEIVER] Sending NACK - retransmit from chunk 2112

--- Window [2112-2249] ---
  [MQTT:file/data] Chunk 2112/4437 (seq=2112, size=239, crc32=0x0DDF2B39)
  [MQTT:file/data] Chunk 2113/4437 (seq=2113, size=239, crc32=0xD27CB951)
  [MQTT:file/data] Chunk 2114/4437 (seq=2114, size=239, crc32=0xF77C2955)
  [MQTT:file/data] Chunk 2162/4437 (seq=2162, size=239, crc32=0x60C74D62)
  [MQTT:file/data] Chunk 2212/4437 (seq=2212, size=239, crc32=0x1ED53D99)
  [MQTT:file/data] Chunk 2249/4437 (seq=2249, size=239, crc32=0xCC0FAD95)
  ✓ Transmitted 138 chunks in window [2112-2249]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2142
  [RECEIVER] Sending NACK - retransmit from chunk 2142

--- Window [2142-2279] ---
  [MQTT:file/data] Chunk 2142/4437 (seq=2142, size=239, crc32=0xC5BAF45B)
  [MQTT:file/data] Chunk 2143/4437 (seq=2143, size=239, crc32=0x6D8EBE3F)
  [MQTT:file/data] Chunk 2144/4437 (seq=2144, size=239, crc32=0xA5CDAAEE)
  [MQTT:file/data] Chunk 2192/4437 (seq=2192, size=239, crc32=0x72064E80)
  [MQTT:file/data] Chunk 2242/4437 (seq=2242, size=239, crc32=0x99FD5C2B)
  [MQTT:file/data] Chunk 2279/4437 (seq=2279, size=239, crc32=0xA6E9ED4C)
  ✓ Transmitted 138 chunks in window [2142-2279]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2163
  [RECEIVER] Sending NACK - retransmit from chunk 2163

--- Window [2163-2300] ---
  [MQTT:file/data] Chunk 2163/4437 (seq=2163, size=239, crc32=0xA439C432)
  [MQTT:file/data] Chunk 2164/4437 (seq=2164, size=239, crc32=0x7F5CDA1E)
  [MQTT:file/data] Chunk 2165/4437 (seq=2165, size=239, crc32=0x7E910AF0)
  [MQTT:file/data] Chunk 2213/4437 (seq=2213, size=239, crc32=0x5C0BD4B1)
  [MQTT:file/data] Chunk 2263/4437 (seq=2263, size=239, crc32=0x2267F51E)
  [MQTT:file/data] Chunk 2300/4437 (seq=2300, size=239, crc32=0xCA2FA8CA)
  ✓ Transmitted 138 chunks in window [2163-2300]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2203
  [RECEIVER] Sending NACK - retransmit from chunk 2203

--- Window [2203-2340] ---
  [MQTT:file/data] Chunk 2203/4437 (seq=2203, size=239, crc32=0xF71202D5)
  [MQTT:file/data] Chunk 2204/4437 (seq=2204, size=239, crc32=0x6FEED2A1)
  [MQTT:file/data] Chunk 2205/4437 (seq=2205, size=239, crc32=0xCE0C15CC)
  [MQTT:file/data] Chunk 2253/4437 (seq=2253, size=239, crc32=0xC50102BE)
  [MQTT:file/data] Chunk 2303/4437 (seq=2303, size=239, crc32=0x89171308)
  [MQTT:file/data] Chunk 2340/4437 (seq=2340, size=239, crc32=0x49A82415)
  ✓ Transmitted 138 chunks in window [2203-2340]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2229
  [RECEIVER] Sending NACK - retransmit from chunk 2229

--- Window [2229-2366] ---
  [MQTT:file/data] Chunk 2229/4437 (seq=2229, size=239, crc32=0x0A65994D)
  [MQTT:file/data] Chunk 2230/4437 (seq=2230, size=239, crc32=0x2F03B383)
  [MQTT:file/data] Chunk 2231/4437 (seq=2231, size=239, crc32=0xE02D1E0F)
  [MQTT:file/data] Chunk 2279/4437 (seq=2279, size=239, crc32=0xA6E9ED4C)
  [MQTT:file/data] Chunk 2329/4437 (seq=2329, size=239, crc32=0xB7480C7C)
  [MQTT:file/data] Chunk 2366/4437 (seq=2366, size=239, crc32=0x6FE0B696)
  ✓ Transmitted 138 chunks in window [2229-2366]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2232
  [RECEIVER] Sending NACK - retransmit from chunk 2232

--- Window [2232-2369] ---
  [MQTT:file/data] Chunk 2232/4437 (seq=2232, size=239, crc32=0x68A11CF2)
  [MQTT:file/data] Chunk 2233/4437 (seq=2233, size=239, crc32=0x91123E8D)
  [MQTT:file/data] Chunk 2234/4437 (seq=2234, size=239, crc32=0x6A8DFECB)
  [MQTT:file/data] Chunk 2282/4437 (seq=2282, size=239, crc32=0x0EA53EDA)
  [MQTT:file/data] Chunk 2332/4437 (seq=2332, size=239, crc32=0xB05086A4)
  [MQTT:file/data] Chunk 2369/4437 (seq=2369, size=239, crc32=0xAF53CDC1)
  ✓ Transmitted 138 chunks in window [2232-2369]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2235
  [RECEIVER] Sending NACK - retransmit from chunk 2235

--- Window [2235-2372] ---
  [MQTT:file/data] Chunk 2235/4437 (seq=2235, size=239, crc32=0xE829F02C)
  [MQTT:file/data] Chunk 2236/4437 (seq=2236, size=239, crc32=0x8F733C70)
  [MQTT:file/data] Chunk 2237/4437 (seq=2237, size=239, crc32=0xCFDE3555)
  [MQTT:file/data] Chunk 2285/4437 (seq=2285, size=239, crc32=0xC51CE069)
  [MQTT:file/data] Chunk 2335/4437 (seq=2335, size=239, crc32=0x58C4582E)
  [MQTT:file/data] Chunk 2372/4437 (seq=2372, size=239, crc32=0x05BB47A4)
  ✓ Transmitted 138 chunks in window [2235-2372]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2244
  [RECEIVER] Sending NACK - retransmit from chunk 2244

--- Window [2244-2381] ---
  [MQTT:file/data] Chunk 2244/4437 (seq=2244, size=239, crc32=0xE92997BD)
  [MQTT:file/data] Chunk 2245/4437 (seq=2245, size=239, crc32=0xB9ACB901)
  [MQTT:file/data] Chunk 2246/4437 (seq=2246, size=239, crc32=0xC44FF27D)
  [MQTT:file/data] Chunk 2294/4437 (seq=2294, size=239, crc32=0x882C15F7)
  [MQTT:file/data] Chunk 2344/4437 (seq=2344, size=239, crc32=0x321885B9)
  [MQTT:file/data] Chunk 2381/4437 (seq=2381, size=239, crc32=0x8B587E8B)
  ✓ Transmitted 138 chunks in window [2244-2381]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2247
  [RECEIVER] Sending NACK - retransmit from chunk 2247

--- Window [2247-2384] ---
  [MQTT:file/data] Chunk 2247/4437 (seq=2247, size=239, crc32=0xC2E7FC03)
  [MQTT:file/data] Chunk 2248/4437 (seq=2248, size=239, crc32=0xC4ECDB7D)
  [MQTT:file/data] Chunk 2249/4437 (seq=2249, size=239, crc32=0xCC0FAD95)
  [MQTT:file/data] Chunk 2297/4437 (seq=2297, size=239, crc32=0x09F416BA)
  [MQTT:file/data] Chunk 2347/4437 (seq=2347, size=239, crc32=0x66964AAA)
  [MQTT:file/data] Chunk 2384/4437 (seq=2384, size=239, crc32=0x41F1F385)
  ✓ Transmitted 138 chunks in window [2247-2384]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2268
  [RECEIVER] Sending NACK - retransmit from chunk 2268

--- Window [2268-2405] ---
  [MQTT:file/data] Chunk 2268/4437 (seq=2268, size=239, crc32=0x082D43DD)
  [MQTT:file/data] Chunk 2269/4437 (seq=2269, size=239, crc32=0x5481CC8D)
  [MQTT:file/data] Chunk 2270/4437 (seq=2270, size=239, crc32=0xBBD205BA)
  [MQTT:file/data] Chunk 2318/4437 (seq=2318, size=239, crc32=0x07911D2F)
  [MQTT:file/data] Chunk 2368/4437 (seq=2368, size=239, crc32=0x45998B06)
  [MQTT:file/data] Chunk 2405/4437 (seq=2405, size=239, crc32=0xA0C221D2)
  ✓ Transmitted 138 chunks in window [2268-2405]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2318
  [RECEIVER] Sending NACK - retransmit from chunk 2318

--- Window [2318-2455] ---
  [MQTT:file/data] Chunk 2318/4437 (seq=2318, size=239, crc32=0x07911D2F)
  [MQTT:file/data] Chunk 2319/4437 (seq=2319, size=239, crc32=0xE0503B17)
  [MQTT:file/data] Chunk 2320/4437 (seq=2320, size=239, crc32=0x52FDCADE)
  [MQTT:file/data] Chunk 2368/4437 (seq=2368, size=239, crc32=0x45998B06)
  [MQTT:file/data] Chunk 2418/4437 (seq=2418, size=239, crc32=0x5254F9E7)
  [MQTT:file/data] Chunk 2455/4437 (seq=2455, size=239, crc32=0xC4AA18C4)
  ✓ Transmitted 138 chunks in window [2318-2455]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2331
  [RECEIVER] Sending NACK - retransmit from chunk 2331

--- Window [2331-2468] ---
  [MQTT:file/data] Chunk 2331/4437 (seq=2331, size=239, crc32=0x4749AC05)
  [MQTT:file/data] Chunk 2332/4437 (seq=2332, size=239, crc32=0xB05086A4)
  [MQTT:file/data] Chunk 2333/4437 (seq=2333, size=239, crc32=0x29AE2BA7)
  [MQTT:file/data] Chunk 2381/4437 (seq=2381, size=239, crc32=0x8B587E8B)
  [MQTT:file/data] Chunk 2431/4437 (seq=2431, size=239, crc32=0x2E311087)
  [MQTT:file/data] Chunk 2468/4437 (seq=2468, size=239, crc32=0x4E37A281)
  ✓ Transmitted 138 chunks in window [2331-2468]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2334
  [RECEIVER] Sending NACK - retransmit from chunk 2334

--- Window [2334-2471] ---
  [MQTT:file/data] Chunk 2334/4437 (seq=2334, size=239, crc32=0xCF1F3DFD)
  [MQTT:file/data] Chunk 2335/4437 (seq=2335, size=239, crc32=0x58C4582E)
  [MQTT:file/data] Chunk 2336/4437 (seq=2336, size=239, crc32=0xA10C9F02)
  [MQTT:file/data] Chunk 2384/4437 (seq=2384, size=239, crc32=0x41F1F385)
  [MQTT:file/data] Chunk 2434/4437 (seq=2434, size=239, crc32=0xEDEA9A81)
  [MQTT:file/data] Chunk 2471/4437 (seq=2471, size=239, crc32=0x44201708)
  ✓ Transmitted 138 chunks in window [2334-2471]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2357
  [RECEIVER] Sending NACK - retransmit from chunk 2357

--- Window [2357-2494] ---
  [MQTT:file/data] Chunk 2357/4437 (seq=2357, size=239, crc32=0xFB966994)
  [MQTT:file/data] Chunk 2358/4437 (seq=2358, size=239, crc32=0x1C0E4A44)
  [MQTT:file/data] Chunk 2359/4437 (seq=2359, size=239, crc32=0x58D2AD7B)
  [MQTT:file/data] Chunk 2407/4437 (seq=2407, size=239, crc32=0xF9A9751B)
  [MQTT:file/data] Chunk 2457/4437 (seq=2457, size=239, crc32=0x08ADC290)
  [MQTT:file/data] Chunk 2494/4437 (seq=2494, size=239, crc32=0x845E6518)
  ✓ Transmitted 138 chunks in window [2357-2494]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2391
  [RECEIVER] Sending NACK - retransmit from chunk 2391

--- Window [2391-2528] ---
  [MQTT:file/data] Chunk 2391/4437 (seq=2391, size=239, crc32=0x2E11F754)
  [MQTT:file/data] Chunk 2392/4437 (seq=2392, size=239, crc32=0xD1ED0DD0)
  [MQTT:file/data] Chunk 2393/4437 (seq=2393, size=239, crc32=0x52BE8E05)
  [MQTT:file/data] Chunk 2441/4437 (seq=2441, size=239, crc32=0x60C60A9D)
  [MQTT:file/data] Chunk 2491/4437 (seq=2491, size=239, crc32=0x7C645101)
  [MQTT:file/data] Chunk 2528/4437 (seq=2528, size=239, crc32=0x90561043)
  ✓ Transmitted 138 chunks in window [2391-2528]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2411
  [RECEIVER] Sending NACK - retransmit from chunk 2411

--- Window [2411-2548] ---
  [MQTT:file/data] Chunk 2411/4437 (seq=2411, size=239, crc32=0x77009E2C)
  [MQTT:file/data] Chunk 2412/4437 (seq=2412, size=239, crc32=0x119C88C7)
  [MQTT:file/data] Chunk 2413/4437 (seq=2413, size=239, crc32=0x42D38B37)
  [MQTT:file/data] Chunk 2461/4437 (seq=2461, size=239, crc32=0x7F6F4EC4)
  [MQTT:file/data] Chunk 2511/4437 (seq=2511, size=239, crc32=0x054AE324)
  [MQTT:file/data] Chunk 2548/4437 (seq=2548, size=239, crc32=0xE7DB4AEA)
  ✓ Transmitted 138 chunks in window [2411-2548]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2450
  [RECEIVER] Sending NACK - retransmit from chunk 2450

--- Window [2450-2587] ---
  [MQTT:file/data] Chunk 2450/4437 (seq=2450, size=239, crc32=0xDEDE1563)
  [MQTT:file/data] Chunk 2451/4437 (seq=2451, size=239, crc32=0xB87D4FE5)
  [MQTT:file/data] Chunk 2452/4437 (seq=2452, size=239, crc32=0x69AB5DF2)
  [MQTT:file/data] Chunk 2500/4437 (seq=2500, size=239, crc32=0xF8344A07)
  [MQTT:file/data] Chunk 2550/4437 (seq=2550, size=239, crc32=0x30EB6B94)
  [MQTT:file/data] Chunk 2587/4437 (seq=2587, size=239, crc32=0x8DB5CA30)
  ✓ Transmitted 138 chunks in window [2450-2587]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2454
  [RECEIVER] Sending NACK - retransmit from chunk 2454

--- Window [2454-2591] ---
  [MQTT:file/data] Chunk 2454/4437 (seq=2454, size=239, crc32=0x54C1F3B4)
  [MQTT:file/data] Chunk 2455/4437 (seq=2455, size=239, crc32=0xC4AA18C4)
  [MQTT:file/data] Chunk 2456/4437 (seq=2456, size=239, crc32=0xEA5FB1A7)
  [MQTT:file/data] Chunk 2504/4437 (seq=2504, size=239, crc32=0x44E2249C)
  [MQTT:file/data] Chunk 2554/4437 (seq=2554, size=239, crc32=0x109870C2)
  [MQTT:file/data] Chunk 2591/4437 (seq=2591, size=239, crc32=0x79D88553)
  ✓ Transmitted 138 chunks in window [2454-2591]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2482
  [RECEIVER] Sending NACK - retransmit from chunk 2482

--- Window [2482-2619] ---
  [MQTT:file/data] Chunk 2482/4437 (seq=2482, size=239, crc32=0xE3032975)
  [MQTT:file/data] Chunk 2483/4437 (seq=2483, size=239, crc32=0x5F19AA7C)
  [MQTT:file/data] Chunk 2484/4437 (seq=2484, size=239, crc32=0xEBF4295E)
  [MQTT:file/data] Chunk 2532/4437 (seq=2532, size=239, crc32=0xE987EFF6)
  [MQTT:file/data] Chunk 2582/4437 (seq=2582, size=239, crc32=0x48FDE864)
  [MQTT:file/data] Chunk 2619/4437 (seq=2619, size=239, crc32=0x424B7618)
  ✓ Transmitted 138 chunks in window [2482-2619]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2482
  [RECEIVER] Sending NACK - retransmit from chunk 2482

--- Window [2482-2619] ---
  [MQTT:file/data] Chunk 2482/4437 (seq=2482, size=239, crc32=0xE3032975)
  [MQTT:file/data] Chunk 2483/4437 (seq=2483, size=239, crc32=0x5F19AA7C)
  [MQTT:file/data] Chunk 2484/4437 (seq=2484, size=239, crc32=0xEBF4295E)
  [MQTT:file/data] Chunk 2532/4437 (seq=2532, size=239, crc32=0xE987EFF6)
  [MQTT:file/data] Chunk 2582/4437 (seq=2582, size=239, crc32=0x48FDE864)
  [MQTT:file/data] Chunk 2619/4437 (seq=2619, size=239, crc32=0x424B7618)
  ✓ Transmitted 138 chunks in window [2482-2619]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2484
  [RECEIVER] Sending NACK - retransmit from chunk 2484

--- Window [2484-2621] ---
  [MQTT:file/data] Chunk 2484/4437 (seq=2484, size=239, crc32=0xEBF4295E)
  [MQTT:file/data] Chunk 2485/4437 (seq=2485, size=239, crc32=0x52653FD1)
  [MQTT:file/data] Chunk 2486/4437 (seq=2486, size=239, crc32=0x9AEB2371)
  [MQTT:file/data] Chunk 2534/4437 (seq=2534, size=239, crc32=0xB8725A96)
  [MQTT:file/data] Chunk 2584/4437 (seq=2584, size=239, crc32=0xC8053633)
  [MQTT:file/data] Chunk 2621/4437 (seq=2621, size=239, crc32=0x88827717)
  ✓ Transmitted 138 chunks in window [2484-2621]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2486
  [RECEIVER] Sending NACK - retransmit from chunk 2486

--- Window [2486-2623] ---
  [MQTT:file/data] Chunk 2486/4437 (seq=2486, size=239, crc32=0x9AEB2371)
  [MQTT:file/data] Chunk 2487/4437 (seq=2487, size=239, crc32=0x3D416558)
  [MQTT:file/data] Chunk 2488/4437 (seq=2488, size=239, crc32=0x897E3E7C)
  [MQTT:file/data] Chunk 2536/4437 (seq=2536, size=239, crc32=0x0578D390)
  [MQTT:file/data] Chunk 2586/4437 (seq=2586, size=239, crc32=0x891EA60E)
  [MQTT:file/data] Chunk 2623/4437 (seq=2623, size=239, crc32=0x22EA4125)
  ✓ Transmitted 138 chunks in window [2486-2623]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2493
  [RECEIVER] Sending NACK - retransmit from chunk 2493

--- Window [2493-2630] ---
  [MQTT:file/data] Chunk 2493/4437 (seq=2493, size=239, crc32=0xB907057F)
  [MQTT:file/data] Chunk 2494/4437 (seq=2494, size=239, crc32=0x845E6518)
  [MQTT:file/data] Chunk 2495/4437 (seq=2495, size=239, crc32=0x68BDF2C1)
  [MQTT:file/data] Chunk 2543/4437 (seq=2543, size=239, crc32=0x03FA6B87)
  [MQTT:file/data] Chunk 2593/4437 (seq=2593, size=239, crc32=0xB4020359)
  [MQTT:file/data] Chunk 2630/4437 (seq=2630, size=239, crc32=0xAB8E1CC5)
  ✓ Transmitted 138 chunks in window [2493-2630]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2510
  [RECEIVER] Sending NACK - retransmit from chunk 2510

--- Window [2510-2647] ---
  [MQTT:file/data] Chunk 2510/4437 (seq=2510, size=239, crc32=0x7D745661)
  [MQTT:file/data] Chunk 2511/4437 (seq=2511, size=239, crc32=0x054AE324)
  [MQTT:file/data] Chunk 2512/4437 (seq=2512, size=239, crc32=0xFA798BE0)
  [MQTT:file/data] Chunk 2560/4437 (seq=2560, size=239, crc32=0xE23371BF)
  [MQTT:file/data] Chunk 2610/4437 (seq=2610, size=239, crc32=0x540F871E)
  [MQTT:file/data] Chunk 2647/4437 (seq=2647, size=239, crc32=0x5E4C2D16)
  ✓ Transmitted 138 chunks in window [2510-2647]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2566
  [RECEIVER] Sending NACK - retransmit from chunk 2566

--- Window [2566-2703] ---
  [MQTT:file/data] Chunk 2566/4437 (seq=2566, size=239, crc32=0x12900AB9)
  [MQTT:file/data] Chunk 2567/4437 (seq=2567, size=239, crc32=0x608D87F6)
  [MQTT:file/data] Chunk 2568/4437 (seq=2568, size=239, crc32=0x144E2D44)
  [MQTT:file/data] Chunk 2616/4437 (seq=2616, size=239, crc32=0x704A2AD0)
  [MQTT:file/data] Chunk 2666/4437 (seq=2666, size=239, crc32=0xF8286D46)
  [MQTT:file/data] Chunk 2703/4437 (seq=2703, size=239, crc32=0xE14A7384)
  ✓ Transmitted 138 chunks in window [2566-2703]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2567
  [RECEIVER] Sending NACK - retransmit from chunk 2567

--- Window [2567-2704] ---
  [MQTT:file/data] Chunk 2567/4437 (seq=2567, size=239, crc32=0x608D87F6)
  [MQTT:file/data] Chunk 2568/4437 (seq=2568, size=239, crc32=0x144E2D44)
  [MQTT:file/data] Chunk 2569/4437 (seq=2569, size=239, crc32=0xC6D2C8F2)
  [MQTT:file/data] Chunk 2617/4437 (seq=2617, size=239, crc32=0x4ADFFAB5)
  [MQTT:file/data] Chunk 2667/4437 (seq=2667, size=239, crc32=0xBEE45BC9)
  [MQTT:file/data] Chunk 2704/4437 (seq=2704, size=239, crc32=0x683E3C96)
  ✓ Transmitted 138 chunks in window [2567-2704]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2587
  [RECEIVER] Sending NACK - retransmit from chunk 2587

--- Window [2587-2724] ---
  [MQTT:file/data] Chunk 2587/4437 (seq=2587, size=239, crc32=0x8DB5CA30)
  [MQTT:file/data] Chunk 2588/4437 (seq=2588, size=239, crc32=0x06B18338)
  [MQTT:file/data] Chunk 2589/4437 (seq=2589, size=239, crc32=0xEF278690)
  [MQTT:file/data] Chunk 2637/4437 (seq=2637, size=239, crc32=0x82E6CC06)
  [MQTT:file/data] Chunk 2687/4437 (seq=2687, size=239, crc32=0x52CEAA10)
  [MQTT:file/data] Chunk 2724/4437 (seq=2724, size=239, crc32=0xBCB412D1)
  ✓ Transmitted 138 chunks in window [2587-2724]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2591
  [RECEIVER] Sending NACK - retransmit from chunk 2591

--- Window [2591-2728] ---
  [MQTT:file/data] Chunk 2591/4437 (seq=2591, size=239, crc32=0x79D88553)
  [MQTT:file/data] Chunk 2592/4437 (seq=2592, size=239, crc32=0x254B51B9)
  [MQTT:file/data] Chunk 2593/4437 (seq=2593, size=239, crc32=0xB4020359)
  [MQTT:file/data] Chunk 2641/4437 (seq=2641, size=239, crc32=0x34EEDC6F)
  [MQTT:file/data] Chunk 2691/4437 (seq=2691, size=239, crc32=0x4A7ADAEB)
  [MQTT:file/data] Chunk 2728/4437 (seq=2728, size=239, crc32=0xC4901FE6)
  ✓ Transmitted 138 chunks in window [2591-2728]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2593
  [RECEIVER] Sending NACK - retransmit from chunk 2593

--- Window [2593-2730] ---
  [MQTT:file/data] Chunk 2593/4437 (seq=2593, size=239, crc32=0xB4020359)
  [MQTT:file/data] Chunk 2594/4437 (seq=2594, size=239, crc32=0x5D61068C)
  [MQTT:file/data] Chunk 2595/4437 (seq=2595, size=239, crc32=0x3FF24DE3)
  [MQTT:file/data] Chunk 2643/4437 (seq=2643, size=239, crc32=0x76E02036)
  [MQTT:file/data] Chunk 2693/4437 (seq=2693, size=239, crc32=0x2968EAF8)
  [MQTT:file/data] Chunk 2730/4437 (seq=2730, size=239, crc32=0x37957A2B)
  ✓ Transmitted 138 chunks in window [2593-2730]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2618
  [RECEIVER] Sending NACK - retransmit from chunk 2618

--- Window [2618-2755] ---
  [MQTT:file/data] Chunk 2618/4437 (seq=2618, size=239, crc32=0x5D84341C)
  [MQTT:file/data] Chunk 2619/4437 (seq=2619, size=239, crc32=0x424B7618)
  [MQTT:file/data] Chunk 2620/4437 (seq=2620, size=239, crc32=0x292D53BC)
  [MQTT:file/data] Chunk 2668/4437 (seq=2668, size=239, crc32=0xCC9603B5)
  [MQTT:file/data] Chunk 2718/4437 (seq=2718, size=239, crc32=0x3D9077D9)
  [MQTT:file/data] Chunk 2755/4437 (seq=2755, size=239, crc32=0x5ACD0350)
  ✓ Transmitted 138 chunks in window [2618-2755]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2633
  [RECEIVER] Sending NACK - retransmit from chunk 2633

--- Window [2633-2770] ---
  [MQTT:file/data] Chunk 2633/4437 (seq=2633, size=239, crc32=0xF60C77E3)
  [MQTT:file/data] Chunk 2634/4437 (seq=2634, size=239, crc32=0x7051A6F3)
  [MQTT:file/data] Chunk 2635/4437 (seq=2635, size=239, crc32=0x5E7D9C4B)
  [MQTT:file/data] Chunk 2683/4437 (seq=2683, size=239, crc32=0xDAF121AF)
  [MQTT:file/data] Chunk 2733/4437 (seq=2733, size=239, crc32=0x66234809)
  [MQTT:file/data] Chunk 2770/4437 (seq=2770, size=239, crc32=0xDDFC047A)
  ✓ Transmitted 138 chunks in window [2633-2770]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2657
  [RECEIVER] Sending NACK - retransmit from chunk 2657

--- Window [2657-2794] ---
  [MQTT:file/data] Chunk 2657/4437 (seq=2657, size=239, crc32=0x74AEF694)
  [MQTT:file/data] Chunk 2658/4437 (seq=2658, size=239, crc32=0x331CE622)
  [MQTT:file/data] Chunk 2659/4437 (seq=2659, size=239, crc32=0x930E5DD4)
  [MQTT:file/data] Chunk 2707/4437 (seq=2707, size=239, crc32=0x15125CCE)
  [MQTT:file/data] Chunk 2757/4437 (seq=2757, size=239, crc32=0x8532B95E)
  [MQTT:file/data] Chunk 2794/4437 (seq=2794, size=239, crc32=0x8464432E)
  ✓ Transmitted 138 chunks in window [2657-2794]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2666
  [RECEIVER] Sending NACK - retransmit from chunk 2666

--- Window [2666-2803] ---
  [MQTT:file/data] Chunk 2666/4437 (seq=2666, size=239, crc32=0xF8286D46)
  [MQTT:file/data] Chunk 2667/4437 (seq=2667, size=239, crc32=0xBEE45BC9)
  [MQTT:file/data] Chunk 2668/4437 (seq=2668, size=239, crc32=0xCC9603B5)
  [MQTT:file/data] Chunk 2716/4437 (seq=2716, size=239, crc32=0xDFFFD026)
  [MQTT:file/data] Chunk 2766/4437 (seq=2766, size=239, crc32=0xD272DB90)
  [MQTT:file/data] Chunk 2803/4437 (seq=2803, size=239, crc32=0x734A7BFA)
  ✓ Transmitted 138 chunks in window [2666-2803]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2671
  [RECEIVER] Sending NACK - retransmit from chunk 2671

--- Window [2671-2808] ---
  [MQTT:file/data] Chunk 2671/4437 (seq=2671, size=239, crc32=0x5F5DFAAC)
  [MQTT:file/data] Chunk 2672/4437 (seq=2672, size=239, crc32=0x93BD13E4)
  [MQTT:file/data] Chunk 2673/4437 (seq=2673, size=239, crc32=0x95E57602)
  [MQTT:file/data] Chunk 2721/4437 (seq=2721, size=239, crc32=0x7BC2A667)
  [MQTT:file/data] Chunk 2771/4437 (seq=2771, size=239, crc32=0xAD234FB6)
  [MQTT:file/data] Chunk 2808/4437 (seq=2808, size=239, crc32=0xF564DA72)
  ✓ Transmitted 138 chunks in window [2671-2808]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2738
  [RECEIVER] Sending NACK - retransmit from chunk 2738

--- Window [2738-2875] ---
  [MQTT:file/data] Chunk 2738/4437 (seq=2738, size=239, crc32=0xC223B8A4)
  [MQTT:file/data] Chunk 2739/4437 (seq=2739, size=239, crc32=0x0C061766)
  [MQTT:file/data] Chunk 2740/4437 (seq=2740, size=239, crc32=0x99BEB401)
  [MQTT:file/data] Chunk 2788/4437 (seq=2788, size=239, crc32=0xEA51CEA8)
  [MQTT:file/data] Chunk 2838/4437 (seq=2838, size=239, crc32=0x0E87DEF9)
  [MQTT:file/data] Chunk 2875/4437 (seq=2875, size=239, crc32=0x5BF40F71)
  ✓ Transmitted 138 chunks in window [2738-2875]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2739
  [RECEIVER] Sending NACK - retransmit from chunk 2739

--- Window [2739-2876] ---
  [MQTT:file/data] Chunk 2739/4437 (seq=2739, size=239, crc32=0x0C061766)
  [MQTT:file/data] Chunk 2740/4437 (seq=2740, size=239, crc32=0x99BEB401)
  [MQTT:file/data] Chunk 2741/4437 (seq=2741, size=239, crc32=0x2339FE86)
  [MQTT:file/data] Chunk 2789/4437 (seq=2789, size=239, crc32=0xCD0B276F)
  [MQTT:file/data] Chunk 2839/4437 (seq=2839, size=239, crc32=0xFBD7D8D3)
  [MQTT:file/data] Chunk 2876/4437 (seq=2876, size=239, crc32=0xD571E4CD)
  ✓ Transmitted 138 chunks in window [2739-2876]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2757
  [RECEIVER] Sending NACK - retransmit from chunk 2757

--- Window [2757-2894] ---
  [MQTT:file/data] Chunk 2757/4437 (seq=2757, size=239, crc32=0x8532B95E)
  [MQTT:file/data] Chunk 2758/4437 (seq=2758, size=239, crc32=0x060B1206)
  [MQTT:file/data] Chunk 2759/4437 (seq=2759, size=239, crc32=0x341858BD)
  [MQTT:file/data] Chunk 2807/4437 (seq=2807, size=239, crc32=0xB521AEBF)
  [MQTT:file/data] Chunk 2857/4437 (seq=2857, size=239, crc32=0x143B6C3B)
  [MQTT:file/data] Chunk 2894/4437 (seq=2894, size=239, crc32=0x23AD7796)
  ✓ Transmitted 138 chunks in window [2757-2894]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2820
  [RECEIVER] Sending NACK - retransmit from chunk 2820

--- Window [2820-2957] ---
  [MQTT:file/data] Chunk 2820/4437 (seq=2820, size=239, crc32=0x7BA21366)
  [MQTT:file/data] Chunk 2821/4437 (seq=2821, size=239, crc32=0xD8C21EAA)
  [MQTT:file/data] Chunk 2822/4437 (seq=2822, size=239, crc32=0x01F8CA25)
  [MQTT:file/data] Chunk 2870/4437 (seq=2870, size=239, crc32=0x8D5ABBDF)
  [MQTT:file/data] Chunk 2920/4437 (seq=2920, size=239, crc32=0xE35ADC59)
  [MQTT:file/data] Chunk 2957/4437 (seq=2957, size=239, crc32=0xE921B553)
  ✓ Transmitted 138 chunks in window [2820-2957]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2850
  [RECEIVER] Sending NACK - retransmit from chunk 2850

--- Window [2850-2987] ---
  [MQTT:file/data] Chunk 2850/4437 (seq=2850, size=239, crc32=0x6AD88A2A)
  [MQTT:file/data] Chunk 2851/4437 (seq=2851, size=239, crc32=0x2829DDF6)
  [MQTT:file/data] Chunk 2852/4437 (seq=2852, size=239, crc32=0x07AE02B8)
  [MQTT:file/data] Chunk 2900/4437 (seq=2900, size=239, crc32=0xD71DBAF7)
  [MQTT:file/data] Chunk 2950/4437 (seq=2950, size=239, crc32=0x9AA7ED35)
  [MQTT:file/data] Chunk 2987/4437 (seq=2987, size=239, crc32=0x14F6A33E)
  ✓ Transmitted 138 chunks in window [2850-2987]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2888
  [RECEIVER] Sending NACK - retransmit from chunk 2888

--- Window [2888-3025] ---
  [MQTT:file/data] Chunk 2888/4437 (seq=2888, size=239, crc32=0x60E91DFB)
  [MQTT:file/data] Chunk 2889/4437 (seq=2889, size=239, crc32=0x309166E4)
  [MQTT:file/data] Chunk 2890/4437 (seq=2890, size=239, crc32=0xED2FBD22)
  [MQTT:file/data] Chunk 2938/4437 (seq=2938, size=239, crc32=0xB3FDF1AD)
  [MQTT:file/data] Chunk 2988/4437 (seq=2988, size=239, crc32=0xA165F9B7)
  [MQTT:file/data] Chunk 3025/4437 (seq=3025, size=239, crc32=0x2DAD3E07)
  ✓ Transmitted 138 chunks in window [2888-3025]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2891
  [RECEIVER] Sending NACK - retransmit from chunk 2891

--- Window [2891-3028] ---
  [MQTT:file/data] Chunk 2891/4437 (seq=2891, size=239, crc32=0x22B28459)
  [MQTT:file/data] Chunk 2892/4437 (seq=2892, size=239, crc32=0x12CBDFDB)
  [MQTT:file/data] Chunk 2893/4437 (seq=2893, size=239, crc32=0x209DED95)
  [MQTT:file/data] Chunk 2941/4437 (seq=2941, size=239, crc32=0xC1654554)
  [MQTT:file/data] Chunk 2991/4437 (seq=2991, size=239, crc32=0x3EC8E98C)
  [MQTT:file/data] Chunk 3028/4437 (seq=3028, size=239, crc32=0x524C3999)
  ✓ Transmitted 138 chunks in window [2891-3028]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2914
  [RECEIVER] Sending NACK - retransmit from chunk 2914

--- Window [2914-3051] ---
  [MQTT:file/data] Chunk 2914/4437 (seq=2914, size=239, crc32=0x7F240ED4)
  [MQTT:file/data] Chunk 2915/4437 (seq=2915, size=239, crc32=0x18E79B73)
  [MQTT:file/data] Chunk 2916/4437 (seq=2916, size=239, crc32=0x5E517916)
  [MQTT:file/data] Chunk 2964/4437 (seq=2964, size=239, crc32=0x3AB1BACB)
  [MQTT:file/data] Chunk 3014/4437 (seq=3014, size=239, crc32=0x03A629A2)
  [MQTT:file/data] Chunk 3051/4437 (seq=3051, size=239, crc32=0x4CD3B4B5)
  ✓ Transmitted 138 chunks in window [2914-3051]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2930
  [RECEIVER] Sending NACK - retransmit from chunk 2930

--- Window [2930-3067] ---
  [MQTT:file/data] Chunk 2930/4437 (seq=2930, size=239, crc32=0x1DE66619)
  [MQTT:file/data] Chunk 2931/4437 (seq=2931, size=239, crc32=0x3B780A4F)
  [MQTT:file/data] Chunk 2932/4437 (seq=2932, size=239, crc32=0xC081AC7F)
  [MQTT:file/data] Chunk 2980/4437 (seq=2980, size=239, crc32=0x673E45C8)
  [MQTT:file/data] Chunk 3030/4437 (seq=3030, size=239, crc32=0x412173C2)
  [MQTT:file/data] Chunk 3067/4437 (seq=3067, size=239, crc32=0x225E9892)
  ✓ Transmitted 138 chunks in window [2930-3067]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2932
  [RECEIVER] Sending NACK - retransmit from chunk 2932

--- Window [2932-3069] ---
  [MQTT:file/data] Chunk 2932/4437 (seq=2932, size=239, crc32=0xC081AC7F)
  [MQTT:file/data] Chunk 2933/4437 (seq=2933, size=239, crc32=0x1C361779)
  [MQTT:file/data] Chunk 2934/4437 (seq=2934, size=239, crc32=0x79690866)
  [MQTT:file/data] Chunk 2982/4437 (seq=2982, size=239, crc32=0xD31E5A79)
  [MQTT:file/data] Chunk 3032/4437 (seq=3032, size=239, crc32=0xF759C2C4)
  [MQTT:file/data] Chunk 3069/4437 (seq=3069, size=239, crc32=0xE28BB377)
  ✓ Transmitted 138 chunks in window [2932-3069]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2977
  [RECEIVER] Sending NACK - retransmit from chunk 2977

--- Window [2977-3114] ---
  [MQTT:file/data] Chunk 2977/4437 (seq=2977, size=239, crc32=0xE2DE5859)
  [MQTT:file/data] Chunk 2978/4437 (seq=2978, size=239, crc32=0xE4CA4C31)
  [MQTT:file/data] Chunk 2979/4437 (seq=2979, size=239, crc32=0x5E5D59E0)
  [MQTT:file/data] Chunk 3027/4437 (seq=3027, size=239, crc32=0x59B44918)
  [MQTT:file/data] Chunk 3077/4437 (seq=3077, size=239, crc32=0x056E0E30)
  [MQTT:file/data] Chunk 3114/4437 (seq=3114, size=239, crc32=0x790A8284)
  ✓ Transmitted 138 chunks in window [2977-3114]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2979
  [RECEIVER] Sending NACK - retransmit from chunk 2979

--- Window [2979-3116] ---
  [MQTT:file/data] Chunk 2979/4437 (seq=2979, size=239, crc32=0x5E5D59E0)
  [MQTT:file/data] Chunk 2980/4437 (seq=2980, size=239, crc32=0x673E45C8)
  [MQTT:file/data] Chunk 2981/4437 (seq=2981, size=239, crc32=0x4BF45120)
  [MQTT:file/data] Chunk 3029/4437 (seq=3029, size=239, crc32=0x109A60DF)
  [MQTT:file/data] Chunk 3079/4437 (seq=3079, size=239, crc32=0x2EC2DDB0)
  [MQTT:file/data] Chunk 3116/4437 (seq=3116, size=239, crc32=0x85962A98)
  ✓ Transmitted 138 chunks in window [2979-3116]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2987
  [RECEIVER] Sending NACK - retransmit from chunk 2987

--- Window [2987-3124] ---
  [MQTT:file/data] Chunk 2987/4437 (seq=2987, size=239, crc32=0x14F6A33E)
  [MQTT:file/data] Chunk 2988/4437 (seq=2988, size=239, crc32=0xA165F9B7)
  [MQTT:file/data] Chunk 2989/4437 (seq=2989, size=239, crc32=0x5A0A1C81)
  [MQTT:file/data] Chunk 3037/4437 (seq=3037, size=239, crc32=0x9E192196)
  [MQTT:file/data] Chunk 3087/4437 (seq=3087, size=239, crc32=0x855ED0B2)
  [MQTT:file/data] Chunk 3124/4437 (seq=3124, size=239, crc32=0x7E020CBD)
  ✓ Transmitted 138 chunks in window [2987-3124]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2987
  [RECEIVER] Sending NACK - retransmit from chunk 2987

--- Window [2987-3124] ---
  [MQTT:file/data] Chunk 2987/4437 (seq=2987, size=239, crc32=0x14F6A33E)
  [MQTT:file/data] Chunk 2988/4437 (seq=2988, size=239, crc32=0xA165F9B7)
  [MQTT:file/data] Chunk 2989/4437 (seq=2989, size=239, crc32=0x5A0A1C81)
  [MQTT:file/data] Chunk 3037/4437 (seq=3037, size=239, crc32=0x9E192196)
  [MQTT:file/data] Chunk 3087/4437 (seq=3087, size=239, crc32=0x855ED0B2)
  [MQTT:file/data] Chunk 3124/4437 (seq=3124, size=239, crc32=0x7E020CBD)
  ✓ Transmitted 138 chunks in window [2987-3124]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 2991
  [RECEIVER] Sending NACK - retransmit from chunk 2991

--- Window [2991-3128] ---
  [MQTT:file/data] Chunk 2991/4437 (seq=2991, size=239, crc32=0x3EC8E98C)
  [MQTT:file/data] Chunk 2992/4437 (seq=2992, size=239, crc32=0x97A93694)
  [MQTT:file/data] Chunk 2993/4437 (seq=2993, size=239, crc32=0x1EB886DF)
  [MQTT:file/data] Chunk 3041/4437 (seq=3041, size=239, crc32=0x6463B4AD)
  [MQTT:file/data] Chunk 3091/4437 (seq=3091, size=239, crc32=0xEFE7F6CB)
  [MQTT:file/data] Chunk 3128/4437 (seq=3128, size=239, crc32=0x9DFEC25F)
  ✓ Transmitted 138 chunks in window [2991-3128]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3001
  [RECEIVER] Sending NACK - retransmit from chunk 3001

--- Window [3001-3138] ---
  [MQTT:file/data] Chunk 3001/4437 (seq=3001, size=239, crc32=0x22BC9FFE)
  [MQTT:file/data] Chunk 3002/4437 (seq=3002, size=239, crc32=0xCF682FBE)
  [MQTT:file/data] Chunk 3003/4437 (seq=3003, size=239, crc32=0x6F7C3685)
  [MQTT:file/data] Chunk 3051/4437 (seq=3051, size=239, crc32=0x4CD3B4B5)
  [MQTT:file/data] Chunk 3101/4437 (seq=3101, size=239, crc32=0xC616B575)
  [MQTT:file/data] Chunk 3138/4437 (seq=3138, size=239, crc32=0x3B75DF8E)
  ✓ Transmitted 138 chunks in window [3001-3138]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3003
  [RECEIVER] Sending NACK - retransmit from chunk 3003

--- Window [3003-3140] ---
  [MQTT:file/data] Chunk 3003/4437 (seq=3003, size=239, crc32=0x6F7C3685)
  [MQTT:file/data] Chunk 3004/4437 (seq=3004, size=239, crc32=0x8BC36F6A)
  [MQTT:file/data] Chunk 3005/4437 (seq=3005, size=239, crc32=0x416C76D4)
  [MQTT:file/data] Chunk 3053/4437 (seq=3053, size=239, crc32=0x0A5179A8)
  [MQTT:file/data] Chunk 3103/4437 (seq=3103, size=239, crc32=0x8E988F7E)
  [MQTT:file/data] Chunk 3140/4437 (seq=3140, size=239, crc32=0x6B502FED)
  ✓ Transmitted 138 chunks in window [3003-3140]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3039
  [RECEIVER] Sending NACK - retransmit from chunk 3039

--- Window [3039-3176] ---
  [MQTT:file/data] Chunk 3039/4437 (seq=3039, size=239, crc32=0x61EB1734)
  [MQTT:file/data] Chunk 3040/4437 (seq=3040, size=239, crc32=0x8330427A)
  [MQTT:file/data] Chunk 3041/4437 (seq=3041, size=239, crc32=0x6463B4AD)
  [MQTT:file/data] Chunk 3089/4437 (seq=3089, size=239, crc32=0x4268DA15)
  [MQTT:file/data] Chunk 3139/4437 (seq=3139, size=239, crc32=0x1FBF9538)
  [MQTT:file/data] Chunk 3176/4437 (seq=3176, size=239, crc32=0xE0E8B1AC)
  ✓ Transmitted 138 chunks in window [3039-3176]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3053
  [RECEIVER] Sending NACK - retransmit from chunk 3053

--- Window [3053-3190] ---
  [MQTT:file/data] Chunk 3053/4437 (seq=3053, size=239, crc32=0x0A5179A8)
  [MQTT:file/data] Chunk 3054/4437 (seq=3054, size=239, crc32=0x90B267F4)
  [MQTT:file/data] Chunk 3055/4437 (seq=3055, size=239, crc32=0xEEDB4F57)
  [MQTT:file/data] Chunk 3103/4437 (seq=3103, size=239, crc32=0x8E988F7E)
  [MQTT:file/data] Chunk 3153/4437 (seq=3153, size=239, crc32=0x7275AECF)
  [MQTT:file/data] Chunk 3190/4437 (seq=3190, size=239, crc32=0x30BF8EBA)
  ✓ Transmitted 138 chunks in window [3053-3190]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3069
  [RECEIVER] Sending NACK - retransmit from chunk 3069

--- Window [3069-3206] ---
  [MQTT:file/data] Chunk 3069/4437 (seq=3069, size=239, crc32=0xE28BB377)
  [MQTT:file/data] Chunk 3070/4437 (seq=3070, size=239, crc32=0x4A908469)
  [MQTT:file/data] Chunk 3071/4437 (seq=3071, size=239, crc32=0xC3D1B81A)
  [MQTT:file/data] Chunk 3119/4437 (seq=3119, size=239, crc32=0xA17F3160)
  [MQTT:file/data] Chunk 3169/4437 (seq=3169, size=239, crc32=0xF336724A)
  [MQTT:file/data] Chunk 3206/4437 (seq=3206, size=239, crc32=0xAD0DFC0A)
  ✓ Transmitted 138 chunks in window [3069-3206]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3082
  [RECEIVER] Sending NACK - retransmit from chunk 3082

--- Window [3082-3219] ---
  [MQTT:file/data] Chunk 3082/4437 (seq=3082, size=239, crc32=0x0FE7387F)
  [MQTT:file/data] Chunk 3083/4437 (seq=3083, size=239, crc32=0xAC82C723)
  [MQTT:file/data] Chunk 3084/4437 (seq=3084, size=239, crc32=0xE879B3FC)
  [MQTT:file/data] Chunk 3132/4437 (seq=3132, size=239, crc32=0x4C8EF10E)
  [MQTT:file/data] Chunk 3182/4437 (seq=3182, size=239, crc32=0xB052CF62)
  [MQTT:file/data] Chunk 3219/4437 (seq=3219, size=239, crc32=0x3E2CFA72)
  ✓ Transmitted 138 chunks in window [3082-3219]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3098
  [RECEIVER] Sending NACK - retransmit from chunk 3098

--- Window [3098-3235] ---
  [MQTT:file/data] Chunk 3098/4437 (seq=3098, size=239, crc32=0x896CB10C)
  [MQTT:file/data] Chunk 3099/4437 (seq=3099, size=239, crc32=0xFB6EEF0F)
  [MQTT:file/data] Chunk 3100/4437 (seq=3100, size=239, crc32=0x296CA26A)
  [MQTT:file/data] Chunk 3148/4437 (seq=3148, size=239, crc32=0x7052E3CA)
  [MQTT:file/data] Chunk 3198/4437 (seq=3198, size=239, crc32=0x3AD391F5)
  [MQTT:file/data] Chunk 3235/4437 (seq=3235, size=239, crc32=0x89247223)
  ✓ Transmitted 138 chunks in window [3098-3235]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3158
  [RECEIVER] Sending NACK - retransmit from chunk 3158

--- Window [3158-3295] ---
  [MQTT:file/data] Chunk 3158/4437 (seq=3158, size=239, crc32=0x10682FA7)
  [MQTT:file/data] Chunk 3159/4437 (seq=3159, size=239, crc32=0xC9DF50B8)
  [MQTT:file/data] Chunk 3160/4437 (seq=3160, size=239, crc32=0x72C4162C)
  [MQTT:file/data] Chunk 3208/4437 (seq=3208, size=239, crc32=0x0DFFCDDB)
  [MQTT:file/data] Chunk 3258/4437 (seq=3258, size=239, crc32=0x92EA22DB)
  [MQTT:file/data] Chunk 3295/4437 (seq=3295, size=239, crc32=0x45AA86D2)
  ✓ Transmitted 138 chunks in window [3158-3295]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3162
  [RECEIVER] Sending NACK - retransmit from chunk 3162

--- Window [3162-3299] ---
  [MQTT:file/data] Chunk 3162/4437 (seq=3162, size=239, crc32=0x214485B1)
  [MQTT:file/data] Chunk 3163/4437 (seq=3163, size=239, crc32=0x4649218F)
  [MQTT:file/data] Chunk 3164/4437 (seq=3164, size=239, crc32=0x6CB619E5)
  [MQTT:file/data] Chunk 3212/4437 (seq=3212, size=239, crc32=0x6CE9CA0C)
  [MQTT:file/data] Chunk 3262/4437 (seq=3262, size=239, crc32=0xDAEDB1A2)
  [MQTT:file/data] Chunk 3299/4437 (seq=3299, size=239, crc32=0x15A4F2C9)
  ✓ Transmitted 138 chunks in window [3162-3299]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3185
  [RECEIVER] Sending NACK - retransmit from chunk 3185

--- Window [3185-3322] ---
  [MQTT:file/data] Chunk 3185/4437 (seq=3185, size=239, crc32=0xB9F4F04E)
  [MQTT:file/data] Chunk 3186/4437 (seq=3186, size=239, crc32=0x5DA8244E)
  [MQTT:file/data] Chunk 3187/4437 (seq=3187, size=239, crc32=0x9DEF6B89)
  [MQTT:file/data] Chunk 3235/4437 (seq=3235, size=239, crc32=0x89247223)
  [MQTT:file/data] Chunk 3285/4437 (seq=3285, size=239, crc32=0x8F7D89BD)
  [MQTT:file/data] Chunk 3322/4437 (seq=3322, size=239, crc32=0x668D5B64)
  ✓ Transmitted 138 chunks in window [3185-3322]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3223
  [RECEIVER] Sending NACK - retransmit from chunk 3223

--- Window [3223-3360] ---
  [MQTT:file/data] Chunk 3223/4437 (seq=3223, size=239, crc32=0x54AFE74D)
  [MQTT:file/data] Chunk 3224/4437 (seq=3224, size=239, crc32=0x028F0DF4)
  [MQTT:file/data] Chunk 3225/4437 (seq=3225, size=239, crc32=0x5A7CE3FB)
  [MQTT:file/data] Chunk 3273/4437 (seq=3273, size=239, crc32=0x3BDF8F19)
  [MQTT:file/data] Chunk 3323/4437 (seq=3323, size=239, crc32=0xC159D78C)
  [MQTT:file/data] Chunk 3360/4437 (seq=3360, size=239, crc32=0x60B6F7A8)
  ✓ Transmitted 138 chunks in window [3223-3360]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3260
  [RECEIVER] Sending NACK - retransmit from chunk 3260

--- Window [3260-3397] ---
  [MQTT:file/data] Chunk 3260/4437 (seq=3260, size=239, crc32=0xD2DDF6C5)
  [MQTT:file/data] Chunk 3261/4437 (seq=3261, size=239, crc32=0x23ABBFCD)
  [MQTT:file/data] Chunk 3262/4437 (seq=3262, size=239, crc32=0xDAEDB1A2)
  [MQTT:file/data] Chunk 3310/4437 (seq=3310, size=239, crc32=0xD9AC0AB5)
  [MQTT:file/data] Chunk 3360/4437 (seq=3360, size=239, crc32=0x60B6F7A8)
  [MQTT:file/data] Chunk 3397/4437 (seq=3397, size=239, crc32=0x7CAC3A99)
  ✓ Transmitted 138 chunks in window [3260-3397]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3292
  [RECEIVER] Sending NACK - retransmit from chunk 3292

--- Window [3292-3429] ---
  [MQTT:file/data] Chunk 3292/4437 (seq=3292, size=239, crc32=0xD144E645)
  [MQTT:file/data] Chunk 3293/4437 (seq=3293, size=239, crc32=0xC0C045B8)
  [MQTT:file/data] Chunk 3294/4437 (seq=3294, size=239, crc32=0xA4643E2C)
  [MQTT:file/data] Chunk 3342/4437 (seq=3342, size=239, crc32=0xE380974D)
  [MQTT:file/data] Chunk 3392/4437 (seq=3392, size=239, crc32=0xEAC2B68C)
  [MQTT:file/data] Chunk 3429/4437 (seq=3429, size=239, crc32=0xA6D07D59)
  ✓ Transmitted 138 chunks in window [3292-3429]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3297
  [RECEIVER] Sending NACK - retransmit from chunk 3297

--- Window [3297-3434] ---
  [MQTT:file/data] Chunk 3297/4437 (seq=3297, size=239, crc32=0x6B054BBA)
  [MQTT:file/data] Chunk 3298/4437 (seq=3298, size=239, crc32=0x8E74C641)
  [MQTT:file/data] Chunk 3299/4437 (seq=3299, size=239, crc32=0x15A4F2C9)
  [MQTT:file/data] Chunk 3347/4437 (seq=3347, size=239, crc32=0x01865BBC)
  [MQTT:file/data] Chunk 3397/4437 (seq=3397, size=239, crc32=0x7CAC3A99)
  [MQTT:file/data] Chunk 3434/4437 (seq=3434, size=239, crc32=0x613BE9BD)
  ✓ Transmitted 138 chunks in window [3297-3434]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3344
  [RECEIVER] Sending NACK - retransmit from chunk 3344

--- Window [3344-3481] ---
  [MQTT:file/data] Chunk 3344/4437 (seq=3344, size=239, crc32=0x445506BD)
  [MQTT:file/data] Chunk 3345/4437 (seq=3345, size=239, crc32=0xBB54AF45)
  [MQTT:file/data] Chunk 3346/4437 (seq=3346, size=239, crc32=0x7189F616)
  [MQTT:file/data] Chunk 3394/4437 (seq=3394, size=239, crc32=0xCEB0BE0C)
  [MQTT:file/data] Chunk 3444/4437 (seq=3444, size=239, crc32=0xF61AD4B7)
  [MQTT:file/data] Chunk 3481/4437 (seq=3481, size=239, crc32=0x46E712C0)
  ✓ Transmitted 138 chunks in window [3344-3481]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3367
  [RECEIVER] Sending NACK - retransmit from chunk 3367

--- Window [3367-3504] ---
  [MQTT:file/data] Chunk 3367/4437 (seq=3367, size=239, crc32=0x737E8F7B)
  [MQTT:file/data] Chunk 3368/4437 (seq=3368, size=239, crc32=0x44CDC5AC)
  [MQTT:file/data] Chunk 3369/4437 (seq=3369, size=239, crc32=0xC098A883)
  [MQTT:file/data] Chunk 3417/4437 (seq=3417, size=239, crc32=0x5D187BCD)
  [MQTT:file/data] Chunk 3467/4437 (seq=3467, size=239, crc32=0xB73DAC3C)
  [MQTT:file/data] Chunk 3504/4437 (seq=3504, size=239, crc32=0x783238BD)
  ✓ Transmitted 138 chunks in window [3367-3504]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3374
  [RECEIVER] Sending NACK - retransmit from chunk 3374

--- Window [3374-3511] ---
  [MQTT:file/data] Chunk 3374/4437 (seq=3374, size=239, crc32=0x8F2D8960)
  [MQTT:file/data] Chunk 3375/4437 (seq=3375, size=239, crc32=0xFD112DA9)
  [MQTT:file/data] Chunk 3376/4437 (seq=3376, size=239, crc32=0xB731BAD3)
  [MQTT:file/data] Chunk 3424/4437 (seq=3424, size=239, crc32=0xCF01CAD5)
  [MQTT:file/data] Chunk 3474/4437 (seq=3474, size=239, crc32=0x2D616149)
  [MQTT:file/data] Chunk 3511/4437 (seq=3511, size=239, crc32=0x76691D3D)
  ✓ Transmitted 138 chunks in window [3374-3511]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3405
  [RECEIVER] Sending NACK - retransmit from chunk 3405

--- Window [3405-3542] ---
  [MQTT:file/data] Chunk 3405/4437 (seq=3405, size=239, crc32=0x8DCBF272)
  [MQTT:file/data] Chunk 3406/4437 (seq=3406, size=239, crc32=0x075ACEA6)
  [MQTT:file/data] Chunk 3407/4437 (seq=3407, size=239, crc32=0x29207F5F)
  [MQTT:file/data] Chunk 3455/4437 (seq=3455, size=239, crc32=0x1C8F1F0A)
  [MQTT:file/data] Chunk 3505/4437 (seq=3505, size=239, crc32=0x98E03D27)
  [MQTT:file/data] Chunk 3542/4437 (seq=3542, size=239, crc32=0xD27E62E4)
  ✓ Transmitted 138 chunks in window [3405-3542]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3430
  [RECEIVER] Sending NACK - retransmit from chunk 3430

--- Window [3430-3567] ---
  [MQTT:file/data] Chunk 3430/4437 (seq=3430, size=239, crc32=0xE731501B)
  [MQTT:file/data] Chunk 3431/4437 (seq=3431, size=239, crc32=0xEFE7E38E)
  [MQTT:file/data] Chunk 3432/4437 (seq=3432, size=239, crc32=0xF8665350)
  [MQTT:file/data] Chunk 3480/4437 (seq=3480, size=239, crc32=0x9731D697)
  [MQTT:file/data] Chunk 3530/4437 (seq=3530, size=239, crc32=0x40B49852)
  [MQTT:file/data] Chunk 3567/4437 (seq=3567, size=239, crc32=0x3876337A)
  ✓ Transmitted 138 chunks in window [3430-3567]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3431
  [RECEIVER] Sending NACK - retransmit from chunk 3431

--- Window [3431-3568] ---
  [MQTT:file/data] Chunk 3431/4437 (seq=3431, size=239, crc32=0xEFE7E38E)
  [MQTT:file/data] Chunk 3432/4437 (seq=3432, size=239, crc32=0xF8665350)
  [MQTT:file/data] Chunk 3433/4437 (seq=3433, size=239, crc32=0x1360F289)
  [MQTT:file/data] Chunk 3481/4437 (seq=3481, size=239, crc32=0x46E712C0)
  [MQTT:file/data] Chunk 3531/4437 (seq=3531, size=239, crc32=0xCBBE25BC)
  [MQTT:file/data] Chunk 3568/4437 (seq=3568, size=239, crc32=0xE4FD9B3D)
  ✓ Transmitted 138 chunks in window [3431-3568]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3449
  [RECEIVER] Sending NACK - retransmit from chunk 3449

--- Window [3449-3586] ---
  [MQTT:file/data] Chunk 3449/4437 (seq=3449, size=239, crc32=0x1AB9AF28)
  [MQTT:file/data] Chunk 3450/4437 (seq=3450, size=239, crc32=0x6BE0E861)
  [MQTT:file/data] Chunk 3451/4437 (seq=3451, size=239, crc32=0xC3F08A09)
  [MQTT:file/data] Chunk 3499/4437 (seq=3499, size=239, crc32=0xEF0D6F41)
  [MQTT:file/data] Chunk 3549/4437 (seq=3549, size=239, crc32=0x0AB63C46)
  [MQTT:file/data] Chunk 3586/4437 (seq=3586, size=239, crc32=0xCEC3C0DC)
  ✓ Transmitted 138 chunks in window [3449-3586]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3452
  [RECEIVER] Sending NACK - retransmit from chunk 3452

--- Window [3452-3589] ---
  [MQTT:file/data] Chunk 3452/4437 (seq=3452, size=239, crc32=0x9AB824A9)
  [MQTT:file/data] Chunk 3453/4437 (seq=3453, size=239, crc32=0xFC1E30BB)
  [MQTT:file/data] Chunk 3454/4437 (seq=3454, size=239, crc32=0xC9EE8CE8)
  [MQTT:file/data] Chunk 3502/4437 (seq=3502, size=239, crc32=0x152C415F)
  [MQTT:file/data] Chunk 3552/4437 (seq=3552, size=239, crc32=0x7CD5D520)
  [MQTT:file/data] Chunk 3589/4437 (seq=3589, size=239, crc32=0x5871BFE9)
  ✓ Transmitted 138 chunks in window [3452-3589]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3485
  [RECEIVER] Sending NACK - retransmit from chunk 3485

--- Window [3485-3622] ---
  [MQTT:file/data] Chunk 3485/4437 (seq=3485, size=239, crc32=0x290A815B)
  [MQTT:file/data] Chunk 3486/4437 (seq=3486, size=239, crc32=0x44CF9482)
  [MQTT:file/data] Chunk 3487/4437 (seq=3487, size=239, crc32=0xA5EFB0C8)
  [MQTT:file/data] Chunk 3535/4437 (seq=3535, size=239, crc32=0x96858358)
  [MQTT:file/data] Chunk 3585/4437 (seq=3585, size=239, crc32=0xAFEAA309)
  [MQTT:file/data] Chunk 3622/4437 (seq=3622, size=239, crc32=0x37F5FEDA)
  ✓ Transmitted 138 chunks in window [3485-3622]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3494
  [RECEIVER] Sending NACK - retransmit from chunk 3494

--- Window [3494-3631] ---
  [MQTT:file/data] Chunk 3494/4437 (seq=3494, size=239, crc32=0xB0DEE9AF)
  [MQTT:file/data] Chunk 3495/4437 (seq=3495, size=239, crc32=0x16079954)
  [MQTT:file/data] Chunk 3496/4437 (seq=3496, size=239, crc32=0x6657A204)
  [MQTT:file/data] Chunk 3544/4437 (seq=3544, size=239, crc32=0x77BE4383)
  [MQTT:file/data] Chunk 3594/4437 (seq=3594, size=239, crc32=0x3E1BDDC5)
  [MQTT:file/data] Chunk 3631/4437 (seq=3631, size=239, crc32=0xC9E34C1C)
  ✓ Transmitted 138 chunks in window [3494-3631]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3526
  [RECEIVER] Sending NACK - retransmit from chunk 3526

--- Window [3526-3663] ---
  [MQTT:file/data] Chunk 3526/4437 (seq=3526, size=239, crc32=0xBC901536)
  [MQTT:file/data] Chunk 3527/4437 (seq=3527, size=239, crc32=0x1E6ADA18)
  [MQTT:file/data] Chunk 3528/4437 (seq=3528, size=239, crc32=0xDD0FF16E)
  [MQTT:file/data] Chunk 3576/4437 (seq=3576, size=239, crc32=0x7F37A6B3)
  [MQTT:file/data] Chunk 3626/4437 (seq=3626, size=239, crc32=0x0CDC3A8F)
  [MQTT:file/data] Chunk 3663/4437 (seq=3663, size=239, crc32=0x8B8A644C)
  ✓ Transmitted 138 chunks in window [3526-3663]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3542
  [RECEIVER] Sending NACK - retransmit from chunk 3542

--- Window [3542-3679] ---
  [MQTT:file/data] Chunk 3542/4437 (seq=3542, size=239, crc32=0xD27E62E4)
  [MQTT:file/data] Chunk 3543/4437 (seq=3543, size=239, crc32=0x37A6C1DF)
  [MQTT:file/data] Chunk 3544/4437 (seq=3544, size=239, crc32=0x77BE4383)
  [MQTT:file/data] Chunk 3592/4437 (seq=3592, size=239, crc32=0x603A86AC)
  [MQTT:file/data] Chunk 3642/4437 (seq=3642, size=239, crc32=0x17E0ADAE)
  [MQTT:file/data] Chunk 3679/4437 (seq=3679, size=239, crc32=0x08BBF3A4)
  ✓ Transmitted 138 chunks in window [3542-3679]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3554
  [RECEIVER] Sending NACK - retransmit from chunk 3554

--- Window [3554-3691] ---
  [MQTT:file/data] Chunk 3554/4437 (seq=3554, size=239, crc32=0x38429A64)
  [MQTT:file/data] Chunk 3555/4437 (seq=3555, size=239, crc32=0xFA7D9B19)
  [MQTT:file/data] Chunk 3556/4437 (seq=3556, size=239, crc32=0xF4DCF4F6)
  [MQTT:file/data] Chunk 3604/4437 (seq=3604, size=239, crc32=0xA4DF2D55)
  [MQTT:file/data] Chunk 3654/4437 (seq=3654, size=239, crc32=0x9F83E9EA)
  [MQTT:file/data] Chunk 3691/4437 (seq=3691, size=239, crc32=0x28E8C1CA)
  ✓ Transmitted 138 chunks in window [3554-3691]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3555
  [RECEIVER] Sending NACK - retransmit from chunk 3555

--- Window [3555-3692] ---
  [MQTT:file/data] Chunk 3555/4437 (seq=3555, size=239, crc32=0xFA7D9B19)
  [MQTT:file/data] Chunk 3556/4437 (seq=3556, size=239, crc32=0xF4DCF4F6)
  [MQTT:file/data] Chunk 3557/4437 (seq=3557, size=239, crc32=0x8E547A17)
  [MQTT:file/data] Chunk 3605/4437 (seq=3605, size=239, crc32=0xCB51FC6D)
  [MQTT:file/data] Chunk 3655/4437 (seq=3655, size=239, crc32=0x65497B1E)
  [MQTT:file/data] Chunk 3692/4437 (seq=3692, size=239, crc32=0xAAAD165F)
  ✓ Transmitted 138 chunks in window [3555-3692]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3656
  [RECEIVER] Sending NACK - retransmit from chunk 3656

--- Window [3656-3793] ---
  [MQTT:file/data] Chunk 3656/4437 (seq=3656, size=239, crc32=0x79D17958)
  [MQTT:file/data] Chunk 3657/4437 (seq=3657, size=239, crc32=0x50FD49C6)
  [MQTT:file/data] Chunk 3658/4437 (seq=3658, size=239, crc32=0x382BC2F8)
  [MQTT:file/data] Chunk 3706/4437 (seq=3706, size=239, crc32=0x0D666316)
  [MQTT:file/data] Chunk 3756/4437 (seq=3756, size=239, crc32=0xA22E54B6)
  [MQTT:file/data] Chunk 3793/4437 (seq=3793, size=239, crc32=0x425A5DA4)
  ✓ Transmitted 138 chunks in window [3656-3793]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3696
  [RECEIVER] Sending NACK - retransmit from chunk 3696

--- Window [3696-3833] ---
  [MQTT:file/data] Chunk 3696/4437 (seq=3696, size=239, crc32=0x7B85F858)
  [MQTT:file/data] Chunk 3697/4437 (seq=3697, size=239, crc32=0x28EACDBE)
  [MQTT:file/data] Chunk 3698/4437 (seq=3698, size=239, crc32=0xBA432DBA)
  [MQTT:file/data] Chunk 3746/4437 (seq=3746, size=239, crc32=0x12DD1846)
  [MQTT:file/data] Chunk 3796/4437 (seq=3796, size=239, crc32=0x169668E4)
  [MQTT:file/data] Chunk 3833/4437 (seq=3833, size=239, crc32=0x9B046093)
  ✓ Transmitted 138 chunks in window [3696-3833]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3697
  [RECEIVER] Sending NACK - retransmit from chunk 3697

--- Window [3697-3834] ---
  [MQTT:file/data] Chunk 3697/4437 (seq=3697, size=239, crc32=0x28EACDBE)
  [MQTT:file/data] Chunk 3698/4437 (seq=3698, size=239, crc32=0xBA432DBA)
  [MQTT:file/data] Chunk 3699/4437 (seq=3699, size=239, crc32=0x320C5DDC)
  [MQTT:file/data] Chunk 3747/4437 (seq=3747, size=239, crc32=0x9D11F155)
  [MQTT:file/data] Chunk 3797/4437 (seq=3797, size=239, crc32=0x41844A9E)
  [MQTT:file/data] Chunk 3834/4437 (seq=3834, size=239, crc32=0xDD14799B)
  ✓ Transmitted 138 chunks in window [3697-3834]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3701
  [RECEIVER] Sending NACK - retransmit from chunk 3701

--- Window [3701-3838] ---
  [MQTT:file/data] Chunk 3701/4437 (seq=3701, size=239, crc32=0x192E1B31)
  [MQTT:file/data] Chunk 3702/4437 (seq=3702, size=239, crc32=0xD337CDE2)
  [MQTT:file/data] Chunk 3703/4437 (seq=3703, size=239, crc32=0x68F2459C)
  [MQTT:file/data] Chunk 3751/4437 (seq=3751, size=239, crc32=0xB7E1D12A)
  [MQTT:file/data] Chunk 3801/4437 (seq=3801, size=239, crc32=0x7F07D934)
  [MQTT:file/data] Chunk 3838/4437 (seq=3838, size=239, crc32=0xFA22834B)
  ✓ Transmitted 138 chunks in window [3701-3838]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3723
  [RECEIVER] Sending NACK - retransmit from chunk 3723

--- Window [3723-3860] ---
  [MQTT:file/data] Chunk 3723/4437 (seq=3723, size=239, crc32=0x987BB3EA)
  [MQTT:file/data] Chunk 3724/4437 (seq=3724, size=239, crc32=0x022C3FCB)
  [MQTT:file/data] Chunk 3725/4437 (seq=3725, size=239, crc32=0x7A99C6BE)
  [MQTT:file/data] Chunk 3773/4437 (seq=3773, size=239, crc32=0x26770B60)
  [MQTT:file/data] Chunk 3823/4437 (seq=3823, size=239, crc32=0x03660629)
  [MQTT:file/data] Chunk 3860/4437 (seq=3860, size=239, crc32=0x02D7F88E)
  ✓ Transmitted 138 chunks in window [3723-3860]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3754
  [RECEIVER] Sending NACK - retransmit from chunk 3754

--- Window [3754-3891] ---
  [MQTT:file/data] Chunk 3754/4437 (seq=3754, size=239, crc32=0xA40EDE64)
  [MQTT:file/data] Chunk 3755/4437 (seq=3755, size=239, crc32=0x22BFCAB3)
  [MQTT:file/data] Chunk 3756/4437 (seq=3756, size=239, crc32=0xA22E54B6)
  [MQTT:file/data] Chunk 3804/4437 (seq=3804, size=239, crc32=0x075CCB99)
  [MQTT:file/data] Chunk 3854/4437 (seq=3854, size=239, crc32=0x58D9AD98)
  [MQTT:file/data] Chunk 3891/4437 (seq=3891, size=239, crc32=0x3D285B2E)
  ✓ Transmitted 138 chunks in window [3754-3891]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3784
  [RECEIVER] Sending NACK - retransmit from chunk 3784

--- Window [3784-3921] ---
  [MQTT:file/data] Chunk 3784/4437 (seq=3784, size=239, crc32=0x872D0E6B)
  [MQTT:file/data] Chunk 3785/4437 (seq=3785, size=239, crc32=0x6F8E8124)
  [MQTT:file/data] Chunk 3786/4437 (seq=3786, size=239, crc32=0x07A37ADB)
  [MQTT:file/data] Chunk 3834/4437 (seq=3834, size=239, crc32=0xDD14799B)
  [MQTT:file/data] Chunk 3884/4437 (seq=3884, size=239, crc32=0x63D1A387)
  [MQTT:file/data] Chunk 3921/4437 (seq=3921, size=239, crc32=0x40E4636E)
  ✓ Transmitted 138 chunks in window [3784-3921]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3786
  [RECEIVER] Sending NACK - retransmit from chunk 3786

--- Window [3786-3923] ---
  [MQTT:file/data] Chunk 3786/4437 (seq=3786, size=239, crc32=0x07A37ADB)
  [MQTT:file/data] Chunk 3787/4437 (seq=3787, size=239, crc32=0x1E32DF9A)
  [MQTT:file/data] Chunk 3788/4437 (seq=3788, size=239, crc32=0x634884EC)
  [MQTT:file/data] Chunk 3836/4437 (seq=3836, size=239, crc32=0x947794E9)
  [MQTT:file/data] Chunk 3886/4437 (seq=3886, size=239, crc32=0x46BCECEF)
  [MQTT:file/data] Chunk 3923/4437 (seq=3923, size=239, crc32=0x89527257)
  ✓ Transmitted 138 chunks in window [3786-3923]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3814
  [RECEIVER] Sending NACK - retransmit from chunk 3814

--- Window [3814-3951] ---
  [MQTT:file/data] Chunk 3814/4437 (seq=3814, size=239, crc32=0x8E00992F)
  [MQTT:file/data] Chunk 3815/4437 (seq=3815, size=239, crc32=0xCB0F4321)
  [MQTT:file/data] Chunk 3816/4437 (seq=3816, size=239, crc32=0x0DE868A0)
  [MQTT:file/data] Chunk 3864/4437 (seq=3864, size=239, crc32=0xB87589B5)
  [MQTT:file/data] Chunk 3914/4437 (seq=3914, size=239, crc32=0xB3DE578F)
  [MQTT:file/data] Chunk 3951/4437 (seq=3951, size=239, crc32=0x1E459103)
  ✓ Transmitted 138 chunks in window [3814-3951]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3827
  [RECEIVER] Sending NACK - retransmit from chunk 3827

--- Window [3827-3964] ---
  [MQTT:file/data] Chunk 3827/4437 (seq=3827, size=239, crc32=0x5CB6CF95)
  [MQTT:file/data] Chunk 3828/4437 (seq=3828, size=239, crc32=0x929B7D5C)
  [MQTT:file/data] Chunk 3829/4437 (seq=3829, size=239, crc32=0xCFF0740C)
  [MQTT:file/data] Chunk 3877/4437 (seq=3877, size=239, crc32=0xDAA2227B)
  [MQTT:file/data] Chunk 3927/4437 (seq=3927, size=239, crc32=0x1A7313F9)
  [MQTT:file/data] Chunk 3964/4437 (seq=3964, size=239, crc32=0xB530F9EE)
  ✓ Transmitted 138 chunks in window [3827-3964]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3827
  [RECEIVER] Sending NACK - retransmit from chunk 3827

--- Window [3827-3964] ---
  [MQTT:file/data] Chunk 3827/4437 (seq=3827, size=239, crc32=0x5CB6CF95)
  [MQTT:file/data] Chunk 3828/4437 (seq=3828, size=239, crc32=0x929B7D5C)
  [MQTT:file/data] Chunk 3829/4437 (seq=3829, size=239, crc32=0xCFF0740C)
  [MQTT:file/data] Chunk 3877/4437 (seq=3877, size=239, crc32=0xDAA2227B)
  [MQTT:file/data] Chunk 3927/4437 (seq=3927, size=239, crc32=0x1A7313F9)
  [MQTT:file/data] Chunk 3964/4437 (seq=3964, size=239, crc32=0xB530F9EE)
  ✓ Transmitted 138 chunks in window [3827-3964]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3827
  [RECEIVER] Sending NACK - retransmit from chunk 3827

--- Window [3827-3964] ---
  [MQTT:file/data] Chunk 3827/4437 (seq=3827, size=239, crc32=0x5CB6CF95)
  [MQTT:file/data] Chunk 3828/4437 (seq=3828, size=239, crc32=0x929B7D5C)
  [MQTT:file/data] Chunk 3829/4437 (seq=3829, size=239, crc32=0xCFF0740C)
  [MQTT:file/data] Chunk 3877/4437 (seq=3877, size=239, crc32=0xDAA2227B)
  [MQTT:file/data] Chunk 3927/4437 (seq=3927, size=239, crc32=0x1A7313F9)
  [MQTT:file/data] Chunk 3964/4437 (seq=3964, size=239, crc32=0xB530F9EE)
  ✓ Transmitted 138 chunks in window [3827-3964]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3836
  [RECEIVER] Sending NACK - retransmit from chunk 3836

--- Window [3836-3973] ---
  [MQTT:file/data] Chunk 3836/4437 (seq=3836, size=239, crc32=0x947794E9)
  [MQTT:file/data] Chunk 3837/4437 (seq=3837, size=239, crc32=0xA546B17A)
  [MQTT:file/data] Chunk 3838/4437 (seq=3838, size=239, crc32=0xFA22834B)
  [MQTT:file/data] Chunk 3886/4437 (seq=3886, size=239, crc32=0x46BCECEF)
  [MQTT:file/data] Chunk 3936/4437 (seq=3936, size=239, crc32=0xA33F7464)
  [MQTT:file/data] Chunk 3973/4437 (seq=3973, size=239, crc32=0x757AEA62)
  ✓ Transmitted 138 chunks in window [3836-3973]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3837
  [RECEIVER] Sending NACK - retransmit from chunk 3837

--- Window [3837-3974] ---
  [MQTT:file/data] Chunk 3837/4437 (seq=3837, size=239, crc32=0xA546B17A)
  [MQTT:file/data] Chunk 3838/4437 (seq=3838, size=239, crc32=0xFA22834B)
  [MQTT:file/data] Chunk 3839/4437 (seq=3839, size=239, crc32=0xB9F98B1F)
  [MQTT:file/data] Chunk 3887/4437 (seq=3887, size=239, crc32=0x45E23281)
  [MQTT:file/data] Chunk 3937/4437 (seq=3937, size=239, crc32=0x7EACD9FE)
  [MQTT:file/data] Chunk 3974/4437 (seq=3974, size=239, crc32=0x3B6EC80C)
  ✓ Transmitted 138 chunks in window [3837-3974]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3864
  [RECEIVER] Sending NACK - retransmit from chunk 3864

--- Window [3864-4001] ---
  [MQTT:file/data] Chunk 3864/4437 (seq=3864, size=239, crc32=0xB87589B5)
  [MQTT:file/data] Chunk 3865/4437 (seq=3865, size=239, crc32=0x685914C5)
  [MQTT:file/data] Chunk 3866/4437 (seq=3866, size=239, crc32=0x2CD171C6)
  [MQTT:file/data] Chunk 3914/4437 (seq=3914, size=239, crc32=0xB3DE578F)
  [MQTT:file/data] Chunk 3964/4437 (seq=3964, size=239, crc32=0xB530F9EE)
  [MQTT:file/data] Chunk 4001/4437 (seq=4001, size=239, crc32=0xFB468826)
  ✓ Transmitted 138 chunks in window [3864-4001]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3882
  [RECEIVER] Sending NACK - retransmit from chunk 3882

--- Window [3882-4019] ---
  [MQTT:file/data] Chunk 3882/4437 (seq=3882, size=239, crc32=0x53AD17CA)
  [MQTT:file/data] Chunk 3883/4437 (seq=3883, size=239, crc32=0xA7E6AB53)
  [MQTT:file/data] Chunk 3884/4437 (seq=3884, size=239, crc32=0x63D1A387)
  [MQTT:file/data] Chunk 3932/4437 (seq=3932, size=239, crc32=0x9B2BA643)
  [MQTT:file/data] Chunk 3982/4437 (seq=3982, size=239, crc32=0xEEED1920)
  [MQTT:file/data] Chunk 4019/4437 (seq=4019, size=239, crc32=0x83FA31C0)
  ✓ Transmitted 138 chunks in window [3882-4019]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3919
  [RECEIVER] Sending NACK - retransmit from chunk 3919

--- Window [3919-4056] ---
  [MQTT:file/data] Chunk 3919/4437 (seq=3919, size=239, crc32=0x35FA13A2)
  [MQTT:file/data] Chunk 3920/4437 (seq=3920, size=239, crc32=0x665FC9C3)
  [MQTT:file/data] Chunk 3921/4437 (seq=3921, size=239, crc32=0x40E4636E)
  [MQTT:file/data] Chunk 3969/4437 (seq=3969, size=239, crc32=0x957381E3)
  [MQTT:file/data] Chunk 4019/4437 (seq=4019, size=239, crc32=0x83FA31C0)
  [MQTT:file/data] Chunk 4056/4437 (seq=4056, size=239, crc32=0x3FD9F61C)
  ✓ Transmitted 138 chunks in window [3919-4056]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3936
  [RECEIVER] Sending NACK - retransmit from chunk 3936

--- Window [3936-4073] ---
  [MQTT:file/data] Chunk 3936/4437 (seq=3936, size=239, crc32=0xA33F7464)
  [MQTT:file/data] Chunk 3937/4437 (seq=3937, size=239, crc32=0x7EACD9FE)
  [MQTT:file/data] Chunk 3938/4437 (seq=3938, size=239, crc32=0x589644C0)
  [MQTT:file/data] Chunk 3986/4437 (seq=3986, size=239, crc32=0xDA0A3924)
  [MQTT:file/data] Chunk 4036/4437 (seq=4036, size=239, crc32=0x6C9B0F6A)
  [MQTT:file/data] Chunk 4073/4437 (seq=4073, size=239, crc32=0x0317814A)
  ✓ Transmitted 138 chunks in window [3936-4073]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 3954
  [RECEIVER] Sending NACK - retransmit from chunk 3954

--- Window [3954-4091] ---
  [MQTT:file/data] Chunk 3954/4437 (seq=3954, size=239, crc32=0x1E85B5ED)
  [MQTT:file/data] Chunk 3955/4437 (seq=3955, size=239, crc32=0x57444FA3)
  [MQTT:file/data] Chunk 3956/4437 (seq=3956, size=239, crc32=0xC0797D2A)
  [MQTT:file/data] Chunk 4004/4437 (seq=4004, size=239, crc32=0x8CE0F6DA)
  [MQTT:file/data] Chunk 4054/4437 (seq=4054, size=239, crc32=0xDBDF0F2F)
  [MQTT:file/data] Chunk 4091/4437 (seq=4091, size=239, crc32=0xEDBAE58F)
  ✓ Transmitted 138 chunks in window [3954-4091]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4010
  [RECEIVER] Sending NACK - retransmit from chunk 4010

--- Window [4010-4147] ---
  [MQTT:file/data] Chunk 4010/4437 (seq=4010, size=239, crc32=0x4D4D5418)
  [MQTT:file/data] Chunk 4011/4437 (seq=4011, size=239, crc32=0x02CA431B)
  [MQTT:file/data] Chunk 4012/4437 (seq=4012, size=239, crc32=0xAC94D733)
  [MQTT:file/data] Chunk 4060/4437 (seq=4060, size=239, crc32=0xAB1CB360)
  [MQTT:file/data] Chunk 4110/4437 (seq=4110, size=239, crc32=0xBEE33DDF)
  [MQTT:file/data] Chunk 4147/4437 (seq=4147, size=239, crc32=0xB688AD8A)
  ✓ Transmitted 138 chunks in window [4010-4147]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4013
  [RECEIVER] Sending NACK - retransmit from chunk 4013

--- Window [4013-4150] ---
  [MQTT:file/data] Chunk 4013/4437 (seq=4013, size=239, crc32=0x2CED9940)
  [MQTT:file/data] Chunk 4014/4437 (seq=4014, size=239, crc32=0xFCAB8543)
  [MQTT:file/data] Chunk 4015/4437 (seq=4015, size=239, crc32=0x50E91B95)
  [MQTT:file/data] Chunk 4063/4437 (seq=4063, size=239, crc32=0x0373EAD0)
  [MQTT:file/data] Chunk 4113/4437 (seq=4113, size=239, crc32=0xF10A2935)
  [MQTT:file/data] Chunk 4150/4437 (seq=4150, size=239, crc32=0x2FC791C1)
  ✓ Transmitted 138 chunks in window [4013-4150]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4026
  [RECEIVER] Sending NACK - retransmit from chunk 4026

--- Window [4026-4163] ---
  [MQTT:file/data] Chunk 4026/4437 (seq=4026, size=239, crc32=0xD0438C31)
  [MQTT:file/data] Chunk 4027/4437 (seq=4027, size=239, crc32=0x8DB28AE1)
  [MQTT:file/data] Chunk 4028/4437 (seq=4028, size=239, crc32=0x51B7024A)
  [MQTT:file/data] Chunk 4076/4437 (seq=4076, size=239, crc32=0x56F185EC)
  [MQTT:file/data] Chunk 4126/4437 (seq=4126, size=239, crc32=0x45F2A850)
  [MQTT:file/data] Chunk 4163/4437 (seq=4163, size=239, crc32=0x725A3315)
  ✓ Transmitted 138 chunks in window [4026-4163]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4026
  [RECEIVER] Sending NACK - retransmit from chunk 4026

--- Window [4026-4163] ---
  [MQTT:file/data] Chunk 4026/4437 (seq=4026, size=239, crc32=0xD0438C31)
  [MQTT:file/data] Chunk 4027/4437 (seq=4027, size=239, crc32=0x8DB28AE1)
  [MQTT:file/data] Chunk 4028/4437 (seq=4028, size=239, crc32=0x51B7024A)
  [MQTT:file/data] Chunk 4076/4437 (seq=4076, size=239, crc32=0x56F185EC)
  [MQTT:file/data] Chunk 4126/4437 (seq=4126, size=239, crc32=0x45F2A850)
  [MQTT:file/data] Chunk 4163/4437 (seq=4163, size=239, crc32=0x725A3315)
  ✓ Transmitted 138 chunks in window [4026-4163]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4074
  [RECEIVER] Sending NACK - retransmit from chunk 4074

--- Window [4074-4211] ---
  [MQTT:file/data] Chunk 4074/4437 (seq=4074, size=239, crc32=0xFCD877CD)
  [MQTT:file/data] Chunk 4075/4437 (seq=4075, size=239, crc32=0xC6AF7179)
  [MQTT:file/data] Chunk 4076/4437 (seq=4076, size=239, crc32=0x56F185EC)
  [MQTT:file/data] Chunk 4124/4437 (seq=4124, size=239, crc32=0x00C7E66D)
  [MQTT:file/data] Chunk 4174/4437 (seq=4174, size=239, crc32=0x95BCDA36)
  [MQTT:file/data] Chunk 4211/4437 (seq=4211, size=239, crc32=0x721091C1)
  ✓ Transmitted 138 chunks in window [4074-4211]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4091
  [RECEIVER] Sending NACK - retransmit from chunk 4091

--- Window [4091-4228] ---
  [MQTT:file/data] Chunk 4091/4437 (seq=4091, size=239, crc32=0xEDBAE58F)
  [MQTT:file/data] Chunk 4092/4437 (seq=4092, size=239, crc32=0x7963EFFA)
  [MQTT:file/data] Chunk 4093/4437 (seq=4093, size=239, crc32=0x555FA30B)
  [MQTT:file/data] Chunk 4141/4437 (seq=4141, size=239, crc32=0xA1B34EB7)
  [MQTT:file/data] Chunk 4191/4437 (seq=4191, size=239, crc32=0x0C52F0CE)
  [MQTT:file/data] Chunk 4228/4437 (seq=4228, size=239, crc32=0xA4607D8D)
  ✓ Transmitted 138 chunks in window [4091-4228]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4107
  [RECEIVER] Sending NACK - retransmit from chunk 4107

--- Window [4107-4244] ---
  [MQTT:file/data] Chunk 4107/4437 (seq=4107, size=239, crc32=0x56109F7C)
  [MQTT:file/data] Chunk 4108/4437 (seq=4108, size=239, crc32=0x30FD3C75)
  [MQTT:file/data] Chunk 4109/4437 (seq=4109, size=239, crc32=0x782E628E)
  [MQTT:file/data] Chunk 4157/4437 (seq=4157, size=239, crc32=0x7F27774F)
  [MQTT:file/data] Chunk 4207/4437 (seq=4207, size=239, crc32=0xEC0F1796)
  [MQTT:file/data] Chunk 4244/4437 (seq=4244, size=239, crc32=0xFFBD777F)
  ✓ Transmitted 138 chunks in window [4107-4244]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4120
  [RECEIVER] Sending NACK - retransmit from chunk 4120

--- Window [4120-4257] ---
  [MQTT:file/data] Chunk 4120/4437 (seq=4120, size=239, crc32=0x5BAF0971)
  [MQTT:file/data] Chunk 4121/4437 (seq=4121, size=239, crc32=0x547F7F77)
  [MQTT:file/data] Chunk 4122/4437 (seq=4122, size=239, crc32=0xA63BD744)
  [MQTT:file/data] Chunk 4170/4437 (seq=4170, size=239, crc32=0x3AAF9ECC)
  [MQTT:file/data] Chunk 4220/4437 (seq=4220, size=239, crc32=0xB2FF5134)
  [MQTT:file/data] Chunk 4257/4437 (seq=4257, size=239, crc32=0xB69E8423)
  ✓ Transmitted 138 chunks in window [4120-4257]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4133
  [RECEIVER] Sending NACK - retransmit from chunk 4133

--- Window [4133-4270] ---
  [MQTT:file/data] Chunk 4133/4437 (seq=4133, size=239, crc32=0xD19C0197)
  [MQTT:file/data] Chunk 4134/4437 (seq=4134, size=239, crc32=0x0DB7496F)
  [MQTT:file/data] Chunk 4135/4437 (seq=4135, size=239, crc32=0xD97E37F3)
  [MQTT:file/data] Chunk 4183/4437 (seq=4183, size=239, crc32=0x4DCCCDF2)
  [MQTT:file/data] Chunk 4233/4437 (seq=4233, size=239, crc32=0xF798991F)
  [MQTT:file/data] Chunk 4270/4437 (seq=4270, size=239, crc32=0x990398CF)
  ✓ Transmitted 138 chunks in window [4133-4270]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4151
  [RECEIVER] Sending NACK - retransmit from chunk 4151

--- Window [4151-4288] ---
  [MQTT:file/data] Chunk 4151/4437 (seq=4151, size=239, crc32=0x80DB64C3)
  [MQTT:file/data] Chunk 4152/4437 (seq=4152, size=239, crc32=0x30BFFBFD)
  [MQTT:file/data] Chunk 4153/4437 (seq=4153, size=239, crc32=0x1AE6CBFA)
  [MQTT:file/data] Chunk 4201/4437 (seq=4201, size=239, crc32=0xCA7A76E0)
  [MQTT:file/data] Chunk 4251/4437 (seq=4251, size=239, crc32=0x56040461)
  [MQTT:file/data] Chunk 4288/4437 (seq=4288, size=239, crc32=0x4EA3774A)
  ✓ Transmitted 138 chunks in window [4151-4288]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4153
  [RECEIVER] Sending NACK - retransmit from chunk 4153

--- Window [4153-4290] ---
  [MQTT:file/data] Chunk 4153/4437 (seq=4153, size=239, crc32=0x1AE6CBFA)
  [MQTT:file/data] Chunk 4154/4437 (seq=4154, size=239, crc32=0x75777095)
  [MQTT:file/data] Chunk 4155/4437 (seq=4155, size=239, crc32=0xE7780AA7)
  [MQTT:file/data] Chunk 4203/4437 (seq=4203, size=239, crc32=0x2F6C87B6)
  [MQTT:file/data] Chunk 4253/4437 (seq=4253, size=239, crc32=0x8702706A)
  [MQTT:file/data] Chunk 4290/4437 (seq=4290, size=239, crc32=0xDFA84304)
  ✓ Transmitted 138 chunks in window [4153-4290]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4156
  [RECEIVER] Sending NACK - retransmit from chunk 4156

--- Window [4156-4293] ---
  [MQTT:file/data] Chunk 4156/4437 (seq=4156, size=239, crc32=0x475DF28D)
  [MQTT:file/data] Chunk 4157/4437 (seq=4157, size=239, crc32=0x7F27774F)
  [MQTT:file/data] Chunk 4158/4437 (seq=4158, size=239, crc32=0x1688913F)
  [MQTT:file/data] Chunk 4206/4437 (seq=4206, size=239, crc32=0xB9F5B173)
  [MQTT:file/data] Chunk 4256/4437 (seq=4256, size=239, crc32=0xCCD65634)
  [MQTT:file/data] Chunk 4293/4437 (seq=4293, size=239, crc32=0xC09CF776)
  ✓ Transmitted 138 chunks in window [4156-4293]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4164
  [RECEIVER] Sending NACK - retransmit from chunk 4164

--- Window [4164-4301] ---
  [MQTT:file/data] Chunk 4164/4437 (seq=4164, size=239, crc32=0x6A829F6D)
  [MQTT:file/data] Chunk 4165/4437 (seq=4165, size=239, crc32=0x20B40A5D)
  [MQTT:file/data] Chunk 4166/4437 (seq=4166, size=239, crc32=0xF8946A0E)
  [MQTT:file/data] Chunk 4214/4437 (seq=4214, size=239, crc32=0x71C9450B)
  [MQTT:file/data] Chunk 4264/4437 (seq=4264, size=239, crc32=0xED2096F7)
  [MQTT:file/data] Chunk 4301/4437 (seq=4301, size=239, crc32=0x4F1B3B58)
  ✓ Transmitted 138 chunks in window [4164-4301]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4171
  [RECEIVER] Sending NACK - retransmit from chunk 4171

--- Window [4171-4308] ---
  [MQTT:file/data] Chunk 4171/4437 (seq=4171, size=239, crc32=0x2A2C0046)
  [MQTT:file/data] Chunk 4172/4437 (seq=4172, size=239, crc32=0xFDD7EBC3)
  [MQTT:file/data] Chunk 4173/4437 (seq=4173, size=239, crc32=0xDAA28870)
  [MQTT:file/data] Chunk 4221/4437 (seq=4221, size=239, crc32=0xDFDD3198)
  [MQTT:file/data] Chunk 4271/4437 (seq=4271, size=239, crc32=0x4EFEA614)
  [MQTT:file/data] Chunk 4308/4437 (seq=4308, size=239, crc32=0x120DEF1C)
  ✓ Transmitted 138 chunks in window [4171-4308]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4186
  [RECEIVER] Sending NACK - retransmit from chunk 4186

--- Window [4186-4323] ---
  [MQTT:file/data] Chunk 4186/4437 (seq=4186, size=239, crc32=0x6B5BE12A)
  [MQTT:file/data] Chunk 4187/4437 (seq=4187, size=239, crc32=0xC6C09CC2)
  [MQTT:file/data] Chunk 4188/4437 (seq=4188, size=239, crc32=0x97623C25)
  [MQTT:file/data] Chunk 4236/4437 (seq=4236, size=239, crc32=0xD70EDD11)
  [MQTT:file/data] Chunk 4286/4437 (seq=4286, size=239, crc32=0x425B6B08)
  [MQTT:file/data] Chunk 4323/4437 (seq=4323, size=239, crc32=0x6B6D226D)
  ✓ Transmitted 138 chunks in window [4186-4323]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4225
  [RECEIVER] Sending NACK - retransmit from chunk 4225

--- Window [4225-4362] ---
  [MQTT:file/data] Chunk 4225/4437 (seq=4225, size=239, crc32=0x2950AD49)
  [MQTT:file/data] Chunk 4226/4437 (seq=4226, size=239, crc32=0xF8335AB1)
  [MQTT:file/data] Chunk 4227/4437 (seq=4227, size=239, crc32=0x7660C61A)
  [MQTT:file/data] Chunk 4275/4437 (seq=4275, size=239, crc32=0xBF7F6980)
  [MQTT:file/data] Chunk 4325/4437 (seq=4325, size=239, crc32=0xC74C8405)
  [MQTT:file/data] Chunk 4362/4437 (seq=4362, size=239, crc32=0x1006DB49)
  ✓ Transmitted 138 chunks in window [4225-4362]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4243
  [RECEIVER] Sending NACK - retransmit from chunk 4243

--- Window [4243-4380] ---
  [MQTT:file/data] Chunk 4243/4437 (seq=4243, size=239, crc32=0x9DDCE701)
  [MQTT:file/data] Chunk 4244/4437 (seq=4244, size=239, crc32=0xFFBD777F)
  [MQTT:file/data] Chunk 4245/4437 (seq=4245, size=239, crc32=0xDC5FAAC7)
  [MQTT:file/data] Chunk 4293/4437 (seq=4293, size=239, crc32=0xC09CF776)
  [MQTT:file/data] Chunk 4343/4437 (seq=4343, size=239, crc32=0x0EF07F72)
  [MQTT:file/data] Chunk 4380/4437 (seq=4380, size=239, crc32=0xCEFED767)
  ✓ Transmitted 138 chunks in window [4243-4380]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4265
  [RECEIVER] Sending NACK - retransmit from chunk 4265

--- Window [4265-4402] ---
  [MQTT:file/data] Chunk 4265/4437 (seq=4265, size=239, crc32=0x774BC60F)
  [MQTT:file/data] Chunk 4266/4437 (seq=4266, size=239, crc32=0x9FE9B022)
  [MQTT:file/data] Chunk 4267/4437 (seq=4267, size=239, crc32=0x8A6DDBCC)
  [MQTT:file/data] Chunk 4315/4437 (seq=4315, size=239, crc32=0x75AEBF71)
  [MQTT:file/data] Chunk 4365/4437 (seq=4365, size=239, crc32=0x15637224)
  [MQTT:file/data] Chunk 4402/4437 (seq=4402, size=239, crc32=0x502BA7D8)
  ✓ Transmitted 138 chunks in window [4265-4402]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4266
  [RECEIVER] Sending NACK - retransmit from chunk 4266

--- Window [4266-4403] ---
  [MQTT:file/data] Chunk 4266/4437 (seq=4266, size=239, crc32=0x9FE9B022)
  [MQTT:file/data] Chunk 4267/4437 (seq=4267, size=239, crc32=0x8A6DDBCC)
  [MQTT:file/data] Chunk 4268/4437 (seq=4268, size=239, crc32=0x4E202B85)
  [MQTT:file/data] Chunk 4316/4437 (seq=4316, size=239, crc32=0xCCCCDB84)
  [MQTT:file/data] Chunk 4366/4437 (seq=4366, size=239, crc32=0x38E5C4E5)
  [MQTT:file/data] Chunk 4403/4437 (seq=4403, size=239, crc32=0x5E556E74)
  ✓ Transmitted 138 chunks in window [4266-4403]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4327
  [RECEIVER] Sending NACK - retransmit from chunk 4327

--- Window [4327-4437] ---
  [MQTT:file/data] Chunk 4327/4437 (seq=4327, size=239, crc32=0x216E56E9)
  [MQTT:file/data] Chunk 4328/4437 (seq=4328, size=239, crc32=0x8EAF31F7)
  [MQTT:file/data] Chunk 4329/4437 (seq=4329, size=239, crc32=0xE32C90F4)
  [MQTT:file/data] Chunk 4377/4437 (seq=4377, size=239, crc32=0xEBDE4EE7)
  [MQTT:file/data] Chunk 4427/4437 (seq=4427, size=239, crc32=0x2383D224)
  File CRC32 calculated: 0x83F995A0
  [MQTT:file/data] Chunk 4437/4437 (seq=4437, size=160, crc32=0xCE9AFA92)
  ✓ Transmitted 111 chunks in window [4327-4437]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4360
  [RECEIVER] Sending NACK - retransmit from chunk 4360

--- Window [4360-4437] ---
  [MQTT:file/data] Chunk 4360/4437 (seq=4360, size=239, crc32=0x7756F826)
  [MQTT:file/data] Chunk 4361/4437 (seq=4361, size=239, crc32=0xBBC96BB4)
  [MQTT:file/data] Chunk 4362/4437 (seq=4362, size=239, crc32=0x1006DB49)
  [MQTT:file/data] Chunk 4410/4437 (seq=4410, size=239, crc32=0x89C615C9)
  [MQTT:file/data] Chunk 4437/4437 (seq=4437, size=160, crc32=0xCE9AFA92)
  ✓ Transmitted 78 chunks in window [4360-4437]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4360
  [RECEIVER] Sending NACK - retransmit from chunk 4360

--- Window [4360-4437] ---
  [MQTT:file/data] Chunk 4360/4437 (seq=4360, size=239, crc32=0x7756F826)
  [MQTT:file/data] Chunk 4361/4437 (seq=4361, size=239, crc32=0xBBC96BB4)
  [MQTT:file/data] Chunk 4362/4437 (seq=4362, size=239, crc32=0x1006DB49)
  [MQTT:file/data] Chunk 4410/4437 (seq=4410, size=239, crc32=0x89C615C9)
  [MQTT:file/data] Chunk 4437/4437 (seq=4437, size=160, crc32=0xCE9AFA92)
  ✓ Transmitted 78 chunks in window [4360-4437]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4361
  [RECEIVER] Sending NACK - retransmit from chunk 4361

--- Window [4361-4437] ---
  [MQTT:file/data] Chunk 4361/4437 (seq=4361, size=239, crc32=0xBBC96BB4)
  [MQTT:file/data] Chunk 4362/4437 (seq=4362, size=239, crc32=0x1006DB49)
  [MQTT:file/data] Chunk 4363/4437 (seq=4363, size=239, crc32=0x79DEA57C)
  [MQTT:file/data] Chunk 4411/4437 (seq=4411, size=239, crc32=0xA51A8CAE)
  [MQTT:file/data] Chunk 4437/4437 (seq=4437, size=160, crc32=0xCE9AFA92)
  ✓ Transmitted 77 chunks in window [4361-4437]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4372
  [RECEIVER] Sending NACK - retransmit from chunk 4372

--- Window [4372-4437] ---
  [MQTT:file/data] Chunk 4372/4437 (seq=4372, size=239, crc32=0x90B4973C)
  [MQTT:file/data] Chunk 4373/4437 (seq=4373, size=239, crc32=0x7B80E6B5)
  [MQTT:file/data] Chunk 4374/4437 (seq=4374, size=239, crc32=0xD445A2CD)
  [MQTT:file/data] Chunk 4422/4437 (seq=4422, size=239, crc32=0x67543F7D)
  [MQTT:file/data] Chunk 4437/4437 (seq=4437, size=160, crc32=0xCE9AFA92)
  ✓ Transmitted 66 chunks in window [4372-4437]

  [RECEIVER] Processing window...
  [RECEIVER] ✗ Packet loss detected at chunk 4414
  [RECEIVER] Sending NACK - retransmit from chunk 4414

--- Window [4414-4437] ---
  [MQTT:file/data] Chunk 4414/4437 (seq=4414, size=239, crc32=0xE82EF4B8)
  [MQTT:file/data] Chunk 4415/4437 (seq=4415, size=239, crc32=0x050E7ED9)
  [MQTT:file/data] Chunk 4416/4437 (seq=4416, size=239, crc32=0x1A803FB1)
  [MQTT:file/data] Chunk 4437/4437 (seq=4437, size=160, crc32=0xCE9AFA92)
  ✓ Transmitted 24 chunks in window [4414-4437]

  [RECEIVER] Processing window...
  [RECEIVER] ✓ All 24 chunks received
  [RECEIVER] Writing window to SD card...
  [RECEIVER] ✓ Window synced to SD card
  [RECEIVER] Sending COMPLETE acknowledgment
    [MQTT:file/control] TRANSFER_COMPLETE

=== TRANSMISSION COMPLETE ===
  Total chunks: 4437
  Chunks transmitted: 31622
  Retransmissions: 27185 (86.0%)
  Total bytes: 1060364
  Time: 419444 ms
  Throughput: 2.47 KB/s
  Window size: 138 chunks (32768 bytes)

=== Step 3: File Reconstruction (Receiver Side) ===
TRUE STREAMING read initialized (FatFS, buffered reading):
  File: test.jpg
  Size: 1060364 bytes (1035.51 KB)
  Chunks: 4437
  CRC: Will be calculated incrementally
  Memory usage: ~32.5 KB (32KB read buffer + chunk)
  Read buffer: 32 KB (reduces SD card seeks)
  Max file size: UNLIMITED (stream-based)
RECEIVER: Preparing to receive file
  Expected file: test.jpg
  Expected size: 1060364 bytes
  Chunks to receive: 4437

Initializing reconstruction session...
✓ Transfer session initialized:
  Session ID: stream_428701
  Original filename: test.jpg
  Saving as: test_received.jpg
  Total chunks: 4438 (1 metadata + 4437 data)
  Output file will be: test_received.jpg
  Memory usage: ~33 KB (32KB read buffer + chunk buffer)

Streaming chunks (read -> verify -> write)...
✓ Chunk 1/4437 written (239 bytes)
  [1/4437] Chunk streamed (seq=1)
✓ Chunk 2/4437 written (239 bytes)
✓ Chunk 3/4437 written (239 bytes)
✓ Chunk 4/4437 written (239 bytes)
✓ Chunk 5/4437 written (239 bytes)
✓ Chunk 6/4437 written (239 bytes)
✓ Chunk 7/4437 written (239 bytes)
✓ Chunk 8/4437 written (239 bytes)
✓ Chunk 9/4437 written (239 bytes)
✓ Chunk 10/4437 written (239 bytes)
✓ Chunk 11/4437 written (239 bytes)
✓ Chunk 12/4437 written (239 bytes)
✓ Chunk 13/4437 written (239 bytes)
✓ Chunk 14/4437 written (239 bytes)
✓ Chunk 15/4437 written (239 bytes)
✓ Chunk 16/4437 written (239 bytes)
✓ Chunk 17/4437 written (239 bytes)
✓ Chunk 18/4437 written (239 bytes)
✓ Chunk 19/4437 written (239 bytes)
✓ Chunk 20/4437 written (239 bytes)
✓ Chunk 21/4437 written (239 bytes)
✓ Chunk 22/4437 written (239 bytes)
✓ Chunk 23/4437 written (239 bytes)
✓ Chunk 24/4437 written (239 bytes)
✓ Chunk 25/4437 written (239 bytes)
✓ Chunk 26/4437 written (239 bytes)
✓ Chunk 27/4437 written (239 bytes)
✓ Chunk 28/4437 written (239 bytes)
✓ Chunk 29/4437 written (239 bytes)
✓ Chunk 30/4437 written (239 bytes)
✓ Chunk 31/4437 written (239 bytes)
✓ Chunk 32/4437 written (239 bytes)
✓ Chunk 33/4437 written (239 bytes)
✓ Chunk 34/4437 written (239 bytes)
✓ Chunk 35/4437 written (239 bytes)
✓ Chunk 36/4437 written (239 bytes)
✓ Chunk 37/4437 written (239 bytes)
✓ Chunk 38/4437 written (239 bytes)
✓ Chunk 39/4437 written (239 bytes)
✓ Chunk 40/4437 written (239 bytes)
✓ Chunk 41/4437 written (239 bytes)
✓ Chunk 42/4437 written (239 bytes)
✓ Chunk 43/4437 written (239 bytes)
✓ Chunk 44/4437 written (239 bytes)
✓ Chunk 45/4437 written (239 bytes)
✓ Chunk 46/4437 written (239 bytes)
✓ Chunk 47/4437 written (239 bytes)
✓ Chunk 48/4437 written (239 bytes)
✓ Chunk 49/4437 written (239 bytes)
✓ Chunk 50/4437 written (239 bytes)
✓ Chunk 51/4437 written (239 bytes)
✓ Chunk 52/4437 written (239 bytes)
✓ Chunk 53/4437 written (239 bytes)
✓ Chunk 54/4437 written (239 bytes)
✓ Chunk 55/4437 written (239 bytes)
✓ Chunk 56/4437 written (239 bytes)
✓ Chunk 57/4437 written (239 bytes)
✓ Chunk 58/4437 written (239 bytes)
✓ Chunk 59/4437 written (239 bytes)
✓ Chunk 60/4437 written (239 bytes)
✓ Chunk 61/4437 written (239 bytes)
✓ Chunk 62/4437 written (239 bytes)
✓ Chunk 63/4437 written (239 bytes)
✓ Chunk 64/4437 written (239 bytes)
✓ Chunk 65/4437 written (239 bytes)
✓ Chunk 66/4437 written (239 bytes)
✓ Chunk 67/4437 written (239 bytes)
✓ Chunk 68/4437 written (239 bytes)
✓ Chunk 69/4437 written (239 bytes)
✓ Chunk 70/4437 written (239 bytes)
✓ Chunk 71/4437 written (239 bytes)
✓ Chunk 72/4437 written (239 bytes)
✓ Chunk 73/4437 written (239 bytes)
✓ Chunk 74/4437 written (239 bytes)
✓ Chunk 75/4437 written (239 bytes)
✓ Chunk 76/4437 written (239 bytes)
✓ Chunk 77/4437 written (239 bytes)
✓ Chunk 78/4437 written (239 bytes)
✓ Chunk 79/4437 written (239 bytes)
✓ Chunk 80/4437 written (239 bytes)
✓ Chunk 81/4437 written (239 bytes)
✓ Chunk 82/4437 written (239 bytes)
✓ Chunk 83/4437 written (239 bytes)
✓ Chunk 84/4437 written (239 bytes)
✓ Chunk 85/4437 written (239 bytes)
✓ Chunk 86/4437 written (239 bytes)
✓ Chunk 87/4437 written (239 bytes)
✓ Chunk 88/4437 written (239 bytes)
✓ Chunk 89/4437 written (239 bytes)
✓ Chunk 90/4437 written (239 bytes)
✓ Chunk 91/4437 written (239 bytes)
✓ Chunk 92/4437 written (239 bytes)
✓ Chunk 93/4437 written (239 bytes)
✓ Chunk 94/4437 written (239 bytes)
✓ Chunk 95/4437 written (239 bytes)
✓ Chunk 96/4437 written (239 bytes)
✓ Chunk 97/4437 written (239 bytes)
✓ Chunk 98/4437 written (239 bytes)
✓ Chunk 99/4437 written (239 bytes)
✓ Chunk 100/4437 written (239 bytes)
  [100/4437] Chunk streamed (seq=100)
✓ Chunk 101/4437 written (239 bytes)
✓ Chunk 102/4437 written (239 bytes)
✓ Chunk 103/4437 written (239 bytes)
✓ Chunk 104/4437 written (239 bytes)
✓ Chunk 105/4437 written (239 bytes)
✓ Chunk 106/4437 written (239 bytes)
✓ Chunk 107/4437 written (239 bytes)
✓ Chunk 108/4437 written (239 bytes)
✓ Chunk 109/4437 written (239 bytes)
✓ Chunk 110/4437 written (239 bytes)
✓ Chunk 111/4437 written (239 bytes)
✓ Chunk 112/4437 written (239 bytes)
✓ Chunk 113/4437 written (239 bytes)
✓ Chunk 114/4437 written (239 bytes)
✓ Chunk 115/4437 written (239 bytes)
✓ Chunk 116/4437 written (239 bytes)
✓ Chunk 117/4437 written (239 bytes)
✓ Chunk 118/4437 written (239 bytes)
✓ Chunk 119/4437 written (239 bytes)
✓ Chunk 120/4437 written (239 bytes)
✓ Chunk 121/4437 written (239 bytes)
✓ Chunk 122/4437 written (239 bytes)
✓ Chunk 123/4437 written (239 bytes)
✓ Chunk 124/4437 written (239 bytes)
✓ Chunk 125/4437 written (239 bytes)
✓ Chunk 126/4437 written (239 bytes)
✓ Chunk 127/4437 written (239 bytes)
✓ Chunk 128/4437 written (239 bytes)
✓ Chunk 129/4437 written (239 bytes)
✓ Chunk 130/4437 written (239 bytes)
✓ Chunk 131/4437 written (239 bytes)
✓ Chunk 132/4437 written (239 bytes)
✓ Chunk 133/4437 written (239 bytes)
✓ Chunk 134/4437 written (239 bytes)
✓ Chunk 135/4437 written (239 bytes)
✓ Chunk 136/4437 written (239 bytes)
✓ Chunk 137/4437 written (239 bytes)
✓ Chunk 138/4437 written (239 bytes)
✓ Chunk 139/4437 written (239 bytes)
✓ Chunk 140/4437 written (239 bytes)
✓ Chunk 141/4437 written (239 bytes)
✓ Chunk 142/4437 written (239 bytes)
✓ Chunk 143/4437 written (239 bytes)
✓ Chunk 144/4437 written (239 bytes)
✓ Chunk 145/4437 written (239 bytes)
✓ Chunk 146/4437 written (239 bytes)
✓ Chunk 147/4437 written (239 bytes)
✓ Chunk 148/4437 written (239 bytes)
✓ Chunk 149/4437 written (239 bytes)
✓ Chunk 150/4437 written (239 bytes)
✓ Chunk 151/4437 written (239 bytes)
✓ Chunk 152/4437 written (239 bytes)
✓ Chunk 153/4437 written (239 bytes)
✓ Chunk 154/4437 written (239 bytes)
✓ Chunk 155/4437 written (239 bytes)
✓ Chunk 156/4437 written (239 bytes)
✓ Chunk 157/4437 written (239 bytes)
✓ Chunk 158/4437 written (239 bytes)
✓ Chunk 159/4437 written (239 bytes)
✓ Chunk 160/4437 written (239 bytes)
✓ Chunk 161/4437 written (239 bytes)
✓ Chunk 162/4437 written (239 bytes)
✓ Chunk 163/4437 written (239 bytes)
✓ Chunk 164/4437 written (239 bytes)
✓ Chunk 165/4437 written (239 bytes)
✓ Chunk 166/4437 written (239 bytes)
✓ Chunk 167/4437 written (239 bytes)
✓ Chunk 168/4437 written (239 bytes)
✓ Chunk 169/4437 written (239 bytes)
✓ Chunk 170/4437 written (239 bytes)
✓ Chunk 171/4437 written (239 bytes)
✓ Chunk 172/4437 written (239 bytes)
✓ Chunk 173/4437 written (239 bytes)
✓ Chunk 174/4437 written (239 bytes)
✓ Chunk 175/4437 written (239 bytes)
✓ Chunk 176/4437 written (239 bytes)
✓ Chunk 177/4437 written (239 bytes)
✓ Chunk 178/4437 written (239 bytes)
✓ Chunk 179/4437 written (239 bytes)
✓ Chunk 180/4437 written (239 bytes)
✓ Chunk 181/4437 written (239 bytes)
✓ Chunk 182/4437 written (239 bytes)
✓ Chunk 183/4437 written (239 bytes)
✓ Chunk 184/4437 written (239 bytes)
✓ Chunk 185/4437 written (239 bytes)
✓ Chunk 186/4437 written (239 bytes)
✓ Chunk 187/4437 written (239 bytes)
✓ Chunk 188/4437 written (239 bytes)
✓ Chunk 189/4437 written (239 bytes)
✓ Chunk 190/4437 written (239 bytes)
✓ Chunk 191/4437 written (239 bytes)
✓ Chunk 192/4437 written (239 bytes)
✓ Chunk 193/4437 written (239 bytes)
✓ Chunk 194/4437 written (239 bytes)
✓ Chunk 195/4437 written (239 bytes)
✓ Chunk 196/4437 written (239 bytes)
✓ Chunk 197/4437 written (239 bytes)
✓ Chunk 198/4437 written (239 bytes)
✓ Chunk 199/4437 written (239 bytes)
✓ Chunk 200/4437 written (239 bytes)
  [200/4437] Chunk streamed (seq=200)
✓ Chunk 201/4437 written (239 bytes)
✓ Chunk 202/4437 written (239 bytes)
✓ Chunk 203/4437 written (239 bytes)
✓ Chunk 204/4437 written (239 bytes)
✓ Chunk 205/4437 written (239 bytes)
✓ Chunk 206/4437 written (239 bytes)
✓ Chunk 207/4437 written (239 bytes)
✓ Chunk 208/4437 written (239 bytes)
✓ Chunk 209/4437 written (239 bytes)
✓ Chunk 210/4437 written (239 bytes)
✓ Chunk 211/4437 written (239 bytes)
✓ Chunk 212/4437 written (239 bytes)
✓ Chunk 213/4437 written (239 bytes)
✓ Chunk 214/4437 written (239 bytes)
✓ Chunk 215/4437 written (239 bytes)
✓ Chunk 216/4437 written (239 bytes)
✓ Chunk 217/4437 written (239 bytes)
✓ Chunk 218/4437 written (239 bytes)
✓ Chunk 219/4437 written (239 bytes)
✓ Chunk 220/4437 written (239 bytes)
✓ Chunk 221/4437 written (239 bytes)
✓ Chunk 222/4437 written (239 bytes)
✓ Chunk 223/4437 written (239 bytes)
✓ Chunk 224/4437 written (239 bytes)
✓ Chunk 225/4437 written (239 bytes)
✓ Chunk 226/4437 written (239 bytes)
✓ Chunk 227/4437 written (239 bytes)
✓ Chunk 228/4437 written (239 bytes)
✓ Chunk 229/4437 written (239 bytes)
✓ Chunk 230/4437 written (239 bytes)
✓ Chunk 231/4437 written (239 bytes)
✓ Chunk 232/4437 written (239 bytes)
✓ Chunk 233/4437 written (239 bytes)
✓ Chunk 234/4437 written (239 bytes)
✓ Chunk 235/4437 written (239 bytes)
✓ Chunk 236/4437 written (239 bytes)
✓ Chunk 237/4437 written (239 bytes)
✓ Chunk 238/4437 written (239 bytes)
✓ Chunk 239/4437 written (239 bytes)
✓ Chunk 240/4437 written (239 bytes)
✓ Chunk 241/4437 written (239 bytes)
✓ Chunk 242/4437 written (239 bytes)
✓ Chunk 243/4437 written (239 bytes)
✓ Chunk 244/4437 written (239 bytes)
✓ Chunk 245/4437 written (239 bytes)
✓ Chunk 246/4437 written (239 bytes)
✓ Chunk 247/4437 written (239 bytes)
✓ Chunk 248/4437 written (239 bytes)
✓ Chunk 249/4437 written (239 bytes)
✓ Chunk 250/4437 written (239 bytes)
✓ Chunk 251/4437 written (239 bytes)
✓ Chunk 252/4437 written (239 bytes)
✓ Chunk 253/4437 written (239 bytes)
✓ Chunk 254/4437 written (239 bytes)
✓ Chunk 255/4437 written (239 bytes)
✓ Chunk 256/4437 written (239 bytes)
✓ Chunk 257/4437 written (239 bytes)
✓ Chunk 258/4437 written (239 bytes)
✓ Chunk 259/4437 written (239 bytes)
✓ Chunk 260/4437 written (239 bytes)
✓ Chunk 261/4437 written (239 bytes)
✓ Chunk 262/4437 written (239 bytes)
✓ Chunk 263/4437 written (239 bytes)
✓ Chunk 264/4437 written (239 bytes)
✓ Chunk 265/4437 written (239 bytes)
✓ Chunk 266/4437 written (239 bytes)
✓ Chunk 267/4437 written (239 bytes)
✓ Chunk 268/4437 written (239 bytes)
✓ Chunk 269/4437 written (239 bytes)
✓ Chunk 270/4437 written (239 bytes)
✓ Chunk 271/4437 written (239 bytes)
✓ Chunk 272/4437 written (239 bytes)
✓ Chunk 273/4437 written (239 bytes)
✓ Chunk 274/4437 written (239 bytes)
✓ Chunk 275/4437 written (239 bytes)
✓ Chunk 276/4437 written (239 bytes)
✓ Chunk 277/4437 written (239 bytes)
✓ Chunk 278/4437 written (239 bytes)
✓ Chunk 279/4437 written (239 bytes)
✓ Chunk 280/4437 written (239 bytes)
✓ Chunk 281/4437 written (239 bytes)
✓ Chunk 282/4437 written (239 bytes)
✓ Chunk 283/4437 written (239 bytes)
✓ Chunk 284/4437 written (239 bytes)
✓ Chunk 285/4437 written (239 bytes)
✓ Chunk 286/4437 written (239 bytes)
✓ Chunk 287/4437 written (239 bytes)
✓ Chunk 288/4437 written (239 bytes)
✓ Chunk 289/4437 written (239 bytes)
✓ Chunk 290/4437 written (239 bytes)
✓ Chunk 291/4437 written (239 bytes)
✓ Chunk 292/4437 written (239 bytes)
✓ Chunk 293/4437 written (239 bytes)
✓ Chunk 294/4437 written (239 bytes)
✓ Chunk 295/4437 written (239 bytes)
✓ Chunk 296/4437 written (239 bytes)
✓ Chunk 297/4437 written (239 bytes)
✓ Chunk 298/4437 written (239 bytes)
✓ Chunk 299/4437 written (239 bytes)
✓ Chunk 300/4437 written (239 bytes)
  [300/4437] Chunk streamed (seq=300)
✓ Chunk 301/4437 written (239 bytes)
✓ Chunk 302/4437 written (239 bytes)
✓ Chunk 303/4437 written (239 bytes)
✓ Chunk 304/4437 written (239 bytes)
✓ Chunk 305/4437 written (239 bytes)
✓ Chunk 306/4437 written (239 bytes)
✓ Chunk 307/4437 written (239 bytes)
✓ Chunk 308/4437 written (239 bytes)
✓ Chunk 309/4437 written (239 bytes)
✓ Chunk 310/4437 written (239 bytes)
✓ Chunk 311/4437 written (239 bytes)
✓ Chunk 312/4437 written (239 bytes)
✓ Chunk 313/4437 written (239 bytes)
✓ Chunk 314/4437 written (239 bytes)
✓ Chunk 315/4437 written (239 bytes)
✓ Chunk 316/4437 written (239 bytes)
✓ Chunk 317/4437 written (239 bytes)
✓ Chunk 318/4437 written (239 bytes)
✓ Chunk 319/4437 written (239 bytes)
✓ Chunk 320/4437 written (239 bytes)
✓ Chunk 321/4437 written (239 bytes)
✓ Chunk 322/4437 written (239 bytes)
✓ Chunk 323/4437 written (239 bytes)
✓ Chunk 324/4437 written (239 bytes)
✓ Chunk 325/4437 written (239 bytes)
✓ Chunk 326/4437 written (239 bytes)
✓ Chunk 327/4437 written (239 bytes)
✓ Chunk 328/4437 written (239 bytes)
✓ Chunk 329/4437 written (239 bytes)
✓ Chunk 330/4437 written (239 bytes)
✓ Chunk 331/4437 written (239 bytes)
✓ Chunk 332/4437 written (239 bytes)
✓ Chunk 333/4437 written (239 bytes)
✓ Chunk 334/4437 written (239 bytes)
✓ Chunk 335/4437 written (239 bytes)
✓ Chunk 336/4437 written (239 bytes)
✓ Chunk 337/4437 written (239 bytes)
✓ Chunk 338/4437 written (239 bytes)
✓ Chunk 339/4437 written (239 bytes)
✓ Chunk 340/4437 written (239 bytes)
✓ Chunk 341/4437 written (239 bytes)
✓ Chunk 342/4437 written (239 bytes)
✓ Chunk 343/4437 written (239 bytes)
✓ Chunk 344/4437 written (239 bytes)
✓ Chunk 345/4437 written (239 bytes)
✓ Chunk 346/4437 written (239 bytes)
✓ Chunk 347/4437 written (239 bytes)
✓ Chunk 348/4437 written (239 bytes)
✓ Chunk 349/4437 written (239 bytes)
✓ Chunk 350/4437 written (239 bytes)
✓ Chunk 351/4437 written (239 bytes)
✓ Chunk 352/4437 written (239 bytes)
✓ Chunk 353/4437 written (239 bytes)
✓ Chunk 354/4437 written (239 bytes)
✓ Chunk 355/4437 written (239 bytes)
✓ Chunk 356/4437 written (239 bytes)
✓ Chunk 357/4437 written (239 bytes)
✓ Chunk 358/4437 written (239 bytes)
✓ Chunk 359/4437 written (239 bytes)
✓ Chunk 360/4437 written (239 bytes)
✓ Chunk 361/4437 written (239 bytes)
✓ Chunk 362/4437 written (239 bytes)
✓ Chunk 363/4437 written (239 bytes)
✓ Chunk 364/4437 written (239 bytes)
✓ Chunk 365/4437 written (239 bytes)
✓ Chunk 366/4437 written (239 bytes)
✓ Chunk 367/4437 written (239 bytes)
✓ Chunk 368/4437 written (239 bytes)
✓ Chunk 369/4437 written (239 bytes)
✓ Chunk 370/4437 written (239 bytes)
✓ Chunk 371/4437 written (239 bytes)
✓ Chunk 372/4437 written (239 bytes)
✓ Chunk 373/4437 written (239 bytes)
✓ Chunk 374/4437 written (239 bytes)
✓ Chunk 375/4437 written (239 bytes)
✓ Chunk 376/4437 written (239 bytes)
✓ Chunk 377/4437 written (239 bytes)
✓ Chunk 378/4437 written (239 bytes)
✓ Chunk 379/4437 written (239 bytes)
✓ Chunk 380/4437 written (239 bytes)
✓ Chunk 381/4437 written (239 bytes)
✓ Chunk 382/4437 written (239 bytes)
✓ Chunk 383/4437 written (239 bytes)
✓ Chunk 384/4437 written (239 bytes)
✓ Chunk 385/4437 written (239 bytes)
✓ Chunk 386/4437 written (239 bytes)
✓ Chunk 387/4437 written (239 bytes)
✓ Chunk 388/4437 written (239 bytes)
✓ Chunk 389/4437 written (239 bytes)
✓ Chunk 390/4437 written (239 bytes)
✓ Chunk 391/4437 written (239 bytes)
✓ Chunk 392/4437 written (239 bytes)
✓ Chunk 393/4437 written (239 bytes)
✓ Chunk 394/4437 written (239 bytes)
✓ Chunk 395/4437 written (239 bytes)
✓ Chunk 396/4437 written (239 bytes)
✓ Chunk 397/4437 written (239 bytes)
✓ Chunk 398/4437 written (239 bytes)
✓ Chunk 399/4437 written (239 bytes)
✓ Chunk 400/4437 written (239 bytes)
  [400/4437] Chunk streamed (seq=400)
✓ Chunk 401/4437 written (239 bytes)
✓ Chunk 402/4437 written (239 bytes)
✓ Chunk 403/4437 written (239 bytes)
✓ Chunk 404/4437 written (239 bytes)
✓ Chunk 405/4437 written (239 bytes)
✓ Chunk 406/4437 written (239 bytes)
✓ Chunk 407/4437 written (239 bytes)
✓ Chunk 408/4437 written (239 bytes)
✓ Chunk 409/4437 written (239 bytes)
✓ Chunk 410/4437 written (239 bytes)
✓ Chunk 411/4437 written (239 bytes)
✓ Chunk 412/4437 written (239 bytes)
✓ Chunk 413/4437 written (239 bytes)
✓ Chunk 414/4437 written (239 bytes)
✓ Chunk 415/4437 written (239 bytes)
✓ Chunk 416/4437 written (239 bytes)
✓ Chunk 417/4437 written (239 bytes)
✓ Chunk 418/4437 written (239 bytes)
✓ Chunk 419/4437 written (239 bytes)
✓ Chunk 420/4437 written (239 bytes)
✓ Chunk 421/4437 written (239 bytes)
✓ Chunk 422/4437 written (239 bytes)
✓ Chunk 423/4437 written (239 bytes)
✓ Chunk 424/4437 written (239 bytes)
✓ Chunk 425/4437 written (239 bytes)
✓ Chunk 426/4437 written (239 bytes)
✓ Chunk 427/4437 written (239 bytes)
✓ Chunk 428/4437 written (239 bytes)
✓ Chunk 429/4437 written (239 bytes)
✓ Chunk 430/4437 written (239 bytes)
✓ Chunk 431/4437 written (239 bytes)
✓ Chunk 432/4437 written (239 bytes)
✓ Chunk 433/4437 written (239 bytes)
✓ Chunk 434/4437 written (239 bytes)
✓ Chunk 435/4437 written (239 bytes)
✓ Chunk 436/4437 written (239 bytes)
✓ Chunk 437/4437 written (239 bytes)
✓ Chunk 438/4437 written (239 bytes)
✓ Chunk 439/4437 written (239 bytes)
✓ Chunk 440/4437 written (239 bytes)
✓ Chunk 441/4437 written (239 bytes)
✓ Chunk 442/4437 written (239 bytes)
✓ Chunk 443/4437 written (239 bytes)
✓ Chunk 444/4437 written (239 bytes)
✓ Chunk 445/4437 written (239 bytes)
✓ Chunk 446/4437 written (239 bytes)
✓ Chunk 447/4437 written (239 bytes)
✓ Chunk 448/4437 written (239 bytes)
✓ Chunk 449/4437 written (239 bytes)
✓ Chunk 450/4437 written (239 bytes)
✓ Chunk 451/4437 written (239 bytes)
✓ Chunk 452/4437 written (239 bytes)
✓ Chunk 453/4437 written (239 bytes)
✓ Chunk 454/4437 written (239 bytes)
✓ Chunk 455/4437 written (239 bytes)
✓ Chunk 456/4437 written (239 bytes)
✓ Chunk 457/4437 written (239 bytes)
✓ Chunk 458/4437 written (239 bytes)
✓ Chunk 459/4437 written (239 bytes)
✓ Chunk 460/4437 written (239 bytes)
✓ Chunk 461/4437 written (239 bytes)
✓ Chunk 462/4437 written (239 bytes)
✓ Chunk 463/4437 written (239 bytes)
✓ Chunk 464/4437 written (239 bytes)
✓ Chunk 465/4437 written (239 bytes)
✓ Chunk 466/4437 written (239 bytes)
✓ Chunk 467/4437 written (239 bytes)
✓ Chunk 468/4437 written (239 bytes)
✓ Chunk 469/4437 written (239 bytes)
✓ Chunk 470/4437 written (239 bytes)
✓ Chunk 471/4437 written (239 bytes)
✓ Chunk 472/4437 written (239 bytes)
✓ Chunk 473/4437 written (239 bytes)
✓ Chunk 474/4437 written (239 bytes)
✓ Chunk 475/4437 written (239 bytes)
✓ Chunk 476/4437 written (239 bytes)
✓ Chunk 477/4437 written (239 bytes)
✓ Chunk 478/4437 written (239 bytes)
✓ Chunk 479/4437 written (239 bytes)
✓ Chunk 480/4437 written (239 bytes)
✓ Chunk 481/4437 written (239 bytes)
✓ Chunk 482/4437 written (239 bytes)
✓ Chunk 483/4437 written (239 bytes)
✓ Chunk 484/4437 written (239 bytes)
✓ Chunk 485/4437 written (239 bytes)
✓ Chunk 486/4437 written (239 bytes)
✓ Chunk 487/4437 written (239 bytes)
✓ Chunk 488/4437 written (239 bytes)
✓ Chunk 489/4437 written (239 bytes)
✓ Chunk 490/4437 written (239 bytes)
✓ Chunk 491/4437 written (239 bytes)
✓ Chunk 492/4437 written (239 bytes)
✓ Chunk 493/4437 written (239 bytes)
✓ Chunk 494/4437 written (239 bytes)
✓ Chunk 495/4437 written (239 bytes)
✓ Chunk 496/4437 written (239 bytes)
✓ Chunk 497/4437 written (239 bytes)
✓ Chunk 498/4437 written (239 bytes)
✓ Chunk 499/4437 written (239 bytes)
✓ Chunk 500/4437 written (239 bytes)
  [500/4437] Chunk streamed (seq=500)
✓ Chunk 501/4437 written (239 bytes)
✓ Chunk 502/4437 written (239 bytes)
✓ Chunk 503/4437 written (239 bytes)
✓ Chunk 504/4437 written (239 bytes)
✓ Chunk 505/4437 written (239 bytes)
✓ Chunk 506/4437 written (239 bytes)
✓ Chunk 507/4437 written (239 bytes)
✓ Chunk 508/4437 written (239 bytes)
✓ Chunk 509/4437 written (239 bytes)
✓ Chunk 510/4437 written (239 bytes)
✓ Chunk 511/4437 written (239 bytes)
✓ Chunk 512/4437 written (239 bytes)
✓ Chunk 513/4437 written (239 bytes)
✓ Chunk 514/4437 written (239 bytes)
✓ Chunk 515/4437 written (239 bytes)
✓ Chunk 516/4437 written (239 bytes)
✓ Chunk 517/4437 written (239 bytes)
✓ Chunk 518/4437 written (239 bytes)
✓ Chunk 519/4437 written (239 bytes)
✓ Chunk 520/4437 written (239 bytes)
✓ Chunk 521/4437 written (239 bytes)
✓ Chunk 522/4437 written (239 bytes)
✓ Chunk 523/4437 written (239 bytes)
✓ Chunk 524/4437 written (239 bytes)
✓ Chunk 525/4437 written (239 bytes)
✓ Chunk 526/4437 written (239 bytes)
✓ Chunk 527/4437 written (239 bytes)
✓ Chunk 528/4437 written (239 bytes)
✓ Chunk 529/4437 written (239 bytes)
✓ Chunk 530/4437 written (239 bytes)
✓ Chunk 531/4437 written (239 bytes)
✓ Chunk 532/4437 written (239 bytes)
✓ Chunk 533/4437 written (239 bytes)
✓ Chunk 534/4437 written (239 bytes)
✓ Chunk 535/4437 written (239 bytes)
✓ Chunk 536/4437 written (239 bytes)
✓ Chunk 537/4437 written (239 bytes)
✓ Chunk 538/4437 written (239 bytes)
✓ Chunk 539/4437 written (239 bytes)
✓ Chunk 540/4437 written (239 bytes)
✓ Chunk 541/4437 written (239 bytes)
✓ Chunk 542/4437 written (239 bytes)
✓ Chunk 543/4437 written (239 bytes)
✓ Chunk 544/4437 written (239 bytes)
✓ Chunk 545/4437 written (239 bytes)
✓ Chunk 546/4437 written (239 bytes)
✓ Chunk 547/4437 written (239 bytes)
✓ Chunk 548/4437 written (239 bytes)
✓ Chunk 549/4437 written (239 bytes)
✓ Chunk 550/4437 written (239 bytes)
✓ Chunk 551/4437 written (239 bytes)
✓ Chunk 552/4437 written (239 bytes)
✓ Chunk 553/4437 written (239 bytes)
✓ Chunk 554/4437 written (239 bytes)
✓ Chunk 555/4437 written (239 bytes)
✓ Chunk 556/4437 written (239 bytes)
✓ Chunk 557/4437 written (239 bytes)
✓ Chunk 558/4437 written (239 bytes)
✓ Chunk 559/4437 written (239 bytes)
✓ Chunk 560/4437 written (239 bytes)
✓ Chunk 561/4437 written (239 bytes)
✓ Chunk 562/4437 written (239 bytes)
✓ Chunk 563/4437 written (239 bytes)
✓ Chunk 564/4437 written (239 bytes)
✓ Chunk 565/4437 written (239 bytes)
✓ Chunk 566/4437 written (239 bytes)
✓ Chunk 567/4437 written (239 bytes)
✓ Chunk 568/4437 written (239 bytes)
✓ Chunk 569/4437 written (239 bytes)
✓ Chunk 570/4437 written (239 bytes)
✓ Chunk 571/4437 written (239 bytes)
✓ Chunk 572/4437 written (239 bytes)
✓ Chunk 573/4437 written (239 bytes)
✓ Chunk 574/4437 written (239 bytes)
✓ Chunk 575/4437 written (239 bytes)
✓ Chunk 576/4437 written (239 bytes)
✓ Chunk 577/4437 written (239 bytes)
✓ Chunk 578/4437 written (239 bytes)
✓ Chunk 579/4437 written (239 bytes)
✓ Chunk 580/4437 written (239 bytes)
✓ Chunk 581/4437 written (239 bytes)
✓ Chunk 582/4437 written (239 bytes)
✓ Chunk 583/4437 written (239 bytes)
✓ Chunk 584/4437 written (239 bytes)
✓ Chunk 585/4437 written (239 bytes)
✓ Chunk 586/4437 written (239 bytes)
✓ Chunk 587/4437 written (239 bytes)
✓ Chunk 588/4437 written (239 bytes)
✓ Chunk 589/4437 written (239 bytes)
✓ Chunk 590/4437 written (239 bytes)
✓ Chunk 591/4437 written (239 bytes)
✓ Chunk 592/4437 written (239 bytes)
✓ Chunk 593/4437 written (239 bytes)
✓ Chunk 594/4437 written (239 bytes)
✓ Chunk 595/4437 written (239 bytes)
✓ Chunk 596/4437 written (239 bytes)
✓ Chunk 597/4437 written (239 bytes)
✓ Chunk 598/4437 written (239 bytes)
✓ Chunk 599/4437 written (239 bytes)
✓ Chunk 600/4437 written (239 bytes)
  [600/4437] Chunk streamed (seq=600)
✓ Chunk 601/4437 written (239 bytes)
✓ Chunk 602/4437 written (239 bytes)
✓ Chunk 603/4437 written (239 bytes)
✓ Chunk 604/4437 written (239 bytes)
✓ Chunk 605/4437 written (239 bytes)
✓ Chunk 606/4437 written (239 bytes)
✓ Chunk 607/4437 written (239 bytes)
✓ Chunk 608/4437 written (239 bytes)
✓ Chunk 609/4437 written (239 bytes)
✓ Chunk 610/4437 written (239 bytes)
✓ Chunk 611/4437 written (239 bytes)
✓ Chunk 612/4437 written (239 bytes)
✓ Chunk 613/4437 written (239 bytes)
✓ Chunk 614/4437 written (239 bytes)
✓ Chunk 615/4437 written (239 bytes)
✓ Chunk 616/4437 written (239 bytes)
✓ Chunk 617/4437 written (239 bytes)
✓ Chunk 618/4437 written (239 bytes)
✓ Chunk 619/4437 written (239 bytes)
✓ Chunk 620/4437 written (239 bytes)
✓ Chunk 621/4437 written (239 bytes)
✓ Chunk 622/4437 written (239 bytes)
✓ Chunk 623/4437 written (239 bytes)
✓ Chunk 624/4437 written (239 bytes)
✓ Chunk 625/4437 written (239 bytes)
✓ Chunk 626/4437 written (239 bytes)
✓ Chunk 627/4437 written (239 bytes)
✓ Chunk 628/4437 written (239 bytes)
✓ Chunk 629/4437 written (239 bytes)
✓ Chunk 630/4437 written (239 bytes)
✓ Chunk 631/4437 written (239 bytes)
✓ Chunk 632/4437 written (239 bytes)
✓ Chunk 633/4437 written (239 bytes)
✓ Chunk 634/4437 written (239 bytes)
✓ Chunk 635/4437 written (239 bytes)
✓ Chunk 636/4437 written (239 bytes)
✓ Chunk 637/4437 written (239 bytes)
✓ Chunk 638/4437 written (239 bytes)
✓ Chunk 639/4437 written (239 bytes)
✓ Chunk 640/4437 written (239 bytes)
✓ Chunk 641/4437 written (239 bytes)
✓ Chunk 642/4437 written (239 bytes)
✓ Chunk 643/4437 written (239 bytes)
✓ Chunk 644/4437 written (239 bytes)
✓ Chunk 645/4437 written (239 bytes)
✓ Chunk 646/4437 written (239 bytes)
✓ Chunk 647/4437 written (239 bytes)
✓ Chunk 648/4437 written (239 bytes)
✓ Chunk 649/4437 written (239 bytes)
✓ Chunk 650/4437 written (239 bytes)
✓ Chunk 651/4437 written (239 bytes)
✓ Chunk 652/4437 written (239 bytes)
✓ Chunk 653/4437 written (239 bytes)
✓ Chunk 654/4437 written (239 bytes)
✓ Chunk 655/4437 written (239 bytes)
✓ Chunk 656/4437 written (239 bytes)
✓ Chunk 657/4437 written (239 bytes)
✓ Chunk 658/4437 written (239 bytes)
✓ Chunk 659/4437 written (239 bytes)
✓ Chunk 660/4437 written (239 bytes)
✓ Chunk 661/4437 written (239 bytes)
✓ Chunk 662/4437 written (239 bytes)
✓ Chunk 663/4437 written (239 bytes)
✓ Chunk 664/4437 written (239 bytes)
✓ Chunk 665/4437 written (239 bytes)
✓ Chunk 666/4437 written (239 bytes)
✓ Chunk 667/4437 written (239 bytes)
✓ Chunk 668/4437 written (239 bytes)
✓ Chunk 669/4437 written (239 bytes)
✓ Chunk 670/4437 written (239 bytes)
✓ Chunk 671/4437 written (239 bytes)
✓ Chunk 672/4437 written (239 bytes)
✓ Chunk 673/4437 written (239 bytes)
✓ Chunk 674/4437 written (239 bytes)
✓ Chunk 675/4437 written (239 bytes)
✓ Chunk 676/4437 written (239 bytes)
✓ Chunk 677/4437 written (239 bytes)
✓ Chunk 678/4437 written (239 bytes)
✓ Chunk 679/4437 written (239 bytes)
✓ Chunk 680/4437 written (239 bytes)
✓ Chunk 681/4437 written (239 bytes)
✓ Chunk 682/4437 written (239 bytes)
✓ Chunk 683/4437 written (239 bytes)
✓ Chunk 684/4437 written (239 bytes)
✓ Chunk 685/4437 written (239 bytes)
✓ Chunk 686/4437 written (239 bytes)
✓ Chunk 687/4437 written (239 bytes)
✓ Chunk 688/4437 written (239 bytes)
✓ Chunk 689/4437 written (239 bytes)
✓ Chunk 690/4437 written (239 bytes)
✓ Chunk 691/4437 written (239 bytes)
✓ Chunk 692/4437 written (239 bytes)
✓ Chunk 693/4437 written (239 bytes)
✓ Chunk 694/4437 written (239 bytes)
✓ Chunk 695/4437 written (239 bytes)
✓ Chunk 696/4437 written (239 bytes)
✓ Chunk 697/4437 written (239 bytes)
✓ Chunk 698/4437 written (239 bytes)
✓ Chunk 699/4437 written (239 bytes)
✓ Chunk 700/4437 written (239 bytes)
  [700/4437] Chunk streamed (seq=700)
✓ Chunk 701/4437 written (239 bytes)
✓ Chunk 702/4437 written (239 bytes)
✓ Chunk 703/4437 written (239 bytes)
✓ Chunk 704/4437 written (239 bytes)
✓ Chunk 705/4437 written (239 bytes)
✓ Chunk 706/4437 written (239 bytes)
✓ Chunk 707/4437 written (239 bytes)
✓ Chunk 708/4437 written (239 bytes)
✓ Chunk 709/4437 written (239 bytes)
✓ Chunk 710/4437 written (239 bytes)
✓ Chunk 711/4437 written (239 bytes)
✓ Chunk 712/4437 written (239 bytes)
✓ Chunk 713/4437 written (239 bytes)
✓ Chunk 714/4437 written (239 bytes)
✓ Chunk 715/4437 written (239 bytes)
✓ Chunk 716/4437 written (239 bytes)
✓ Chunk 717/4437 written (239 bytes)
✓ Chunk 718/4437 written (239 bytes)
✓ Chunk 719/4437 written (239 bytes)
✓ Chunk 720/4437 written (239 bytes)
✓ Chunk 721/4437 written (239 bytes)
✓ Chunk 722/4437 written (239 bytes)
✓ Chunk 723/4437 written (239 bytes)
✓ Chunk 724/4437 written (239 bytes)
✓ Chunk 725/4437 written (239 bytes)
✓ Chunk 726/4437 written (239 bytes)
✓ Chunk 727/4437 written (239 bytes)
✓ Chunk 728/4437 written (239 bytes)
✓ Chunk 729/4437 written (239 bytes)
✓ Chunk 730/4437 written (239 bytes)
✓ Chunk 731/4437 written (239 bytes)
✓ Chunk 732/4437 written (239 bytes)
✓ Chunk 733/4437 written (239 bytes)
✓ Chunk 734/4437 written (239 bytes)
✓ Chunk 735/4437 written (239 bytes)
✓ Chunk 736/4437 written (239 bytes)
✓ Chunk 737/4437 written (239 bytes)
✓ Chunk 738/4437 written (239 bytes)
✓ Chunk 739/4437 written (239 bytes)
✓ Chunk 740/4437 written (239 bytes)
✓ Chunk 741/4437 written (239 bytes)
✓ Chunk 742/4437 written (239 bytes)
✓ Chunk 743/4437 written (239 bytes)
✓ Chunk 744/4437 written (239 bytes)
✓ Chunk 745/4437 written (239 bytes)
✓ Chunk 746/4437 written (239 bytes)
✓ Chunk 747/4437 written (239 bytes)
✓ Chunk 748/4437 written (239 bytes)
✓ Chunk 749/4437 written (239 bytes)
✓ Chunk 750/4437 written (239 bytes)
✓ Chunk 751/4437 written (239 bytes)
✓ Chunk 752/4437 written (239 bytes)
✓ Chunk 753/4437 written (239 bytes)
✓ Chunk 754/4437 written (239 bytes)
✓ Chunk 755/4437 written (239 bytes)
✓ Chunk 756/4437 written (239 bytes)
✓ Chunk 757/4437 written (239 bytes)
✓ Chunk 758/4437 written (239 bytes)
✓ Chunk 759/4437 written (239 bytes)
✓ Chunk 760/4437 written (239 bytes)
✓ Chunk 761/4437 written (239 bytes)
✓ Chunk 762/4437 written (239 bytes)
✓ Chunk 763/4437 written (239 bytes)
✓ Chunk 764/4437 written (239 bytes)
✓ Chunk 765/4437 written (239 bytes)
✓ Chunk 766/4437 written (239 bytes)
✓ Chunk 767/4437 written (239 bytes)
✓ Chunk 768/4437 written (239 bytes)
✓ Chunk 769/4437 written (239 bytes)
✓ Chunk 770/4437 written (239 bytes)
✓ Chunk 771/4437 written (239 bytes)
✓ Chunk 772/4437 written (239 bytes)
✓ Chunk 773/4437 written (239 bytes)
✓ Chunk 774/4437 written (239 bytes)
✓ Chunk 775/4437 written (239 bytes)
✓ Chunk 776/4437 written (239 bytes)
✓ Chunk 777/4437 written (239 bytes)
✓ Chunk 778/4437 written (239 bytes)
✓ Chunk 779/4437 written (239 bytes)
✓ Chunk 780/4437 written (239 bytes)
✓ Chunk 781/4437 written (239 bytes)
✓ Chunk 782/4437 written (239 bytes)
✓ Chunk 783/4437 written (239 bytes)
✓ Chunk 784/4437 written (239 bytes)
✓ Chunk 785/4437 written (239 bytes)
✓ Chunk 786/4437 written (239 bytes)
✓ Chunk 787/4437 written (239 bytes)
✓ Chunk 788/4437 written (239 bytes)
✓ Chunk 789/4437 written (239 bytes)
✓ Chunk 790/4437 written (239 bytes)
✓ Chunk 791/4437 written (239 bytes)
✓ Chunk 792/4437 written (239 bytes)
✓ Chunk 793/4437 written (239 bytes)
✓ Chunk 794/4437 written (239 bytes)
✓ Chunk 795/4437 written (239 bytes)
✓ Chunk 796/4437 written (239 bytes)
✓ Chunk 797/4437 written (239 bytes)
✓ Chunk 798/4437 written (239 bytes)
✓ Chunk 799/4437 written (239 bytes)
✓ Chunk 800/4437 written (239 bytes)
  [800/4437] Chunk streamed (seq=800)
✓ Chunk 801/4437 written (239 bytes)
✓ Chunk 802/4437 written (239 bytes)
✓ Chunk 803/4437 written (239 bytes)
✓ Chunk 804/4437 written (239 bytes)
✓ Chunk 805/4437 written (239 bytes)
✓ Chunk 806/4437 written (239 bytes)
✓ Chunk 807/4437 written (239 bytes)
✓ Chunk 808/4437 written (239 bytes)
✓ Chunk 809/4437 written (239 bytes)
✓ Chunk 810/4437 written (239 bytes)
✓ Chunk 811/4437 written (239 bytes)
✓ Chunk 812/4437 written (239 bytes)
✓ Chunk 813/4437 written (239 bytes)
✓ Chunk 814/4437 written (239 bytes)
✓ Chunk 815/4437 written (239 bytes)
✓ Chunk 816/4437 written (239 bytes)
✓ Chunk 817/4437 written (239 bytes)
✓ Chunk 818/4437 written (239 bytes)
✓ Chunk 819/4437 written (239 bytes)
✓ Chunk 820/4437 written (239 bytes)
✓ Chunk 821/4437 written (239 bytes)
✓ Chunk 822/4437 written (239 bytes)
✓ Chunk 823/4437 written (239 bytes)
✓ Chunk 824/4437 written (239 bytes)
✓ Chunk 825/4437 written (239 bytes)
✓ Chunk 826/4437 written (239 bytes)
✓ Chunk 827/4437 written (239 bytes)
✓ Chunk 828/4437 written (239 bytes)
✓ Chunk 829/4437 written (239 bytes)
✓ Chunk 830/4437 written (239 bytes)
✓ Chunk 831/4437 written (239 bytes)
✓ Chunk 832/4437 written (239 bytes)
✓ Chunk 833/4437 written (239 bytes)
✓ Chunk 834/4437 written (239 bytes)
✓ Chunk 835/4437 written (239 bytes)
✓ Chunk 836/4437 written (239 bytes)
✓ Chunk 837/4437 written (239 bytes)
✓ Chunk 838/4437 written (239 bytes)
✓ Chunk 839/4437 written (239 bytes)
✓ Chunk 840/4437 written (239 bytes)
✓ Chunk 841/4437 written (239 bytes)
✓ Chunk 842/4437 written (239 bytes)
✓ Chunk 843/4437 written (239 bytes)
✓ Chunk 844/4437 written (239 bytes)
✓ Chunk 845/4437 written (239 bytes)
✓ Chunk 846/4437 written (239 bytes)
✓ Chunk 847/4437 written (239 bytes)
✓ Chunk 848/4437 written (239 bytes)
✓ Chunk 849/4437 written (239 bytes)
✓ Chunk 850/4437 written (239 bytes)
✓ Chunk 851/4437 written (239 bytes)
✓ Chunk 852/4437 written (239 bytes)
✓ Chunk 853/4437 written (239 bytes)
✓ Chunk 854/4437 written (239 bytes)
✓ Chunk 855/4437 written (239 bytes)
✓ Chunk 856/4437 written (239 bytes)
✓ Chunk 857/4437 written (239 bytes)
✓ Chunk 858/4437 written (239 bytes)
✓ Chunk 859/4437 written (239 bytes)
✓ Chunk 860/4437 written (239 bytes)
✓ Chunk 861/4437 written (239 bytes)
✓ Chunk 862/4437 written (239 bytes)
✓ Chunk 863/4437 written (239 bytes)
✓ Chunk 864/4437 written (239 bytes)
✓ Chunk 865/4437 written (239 bytes)
✓ Chunk 866/4437 written (239 bytes)
✓ Chunk 867/4437 written (239 bytes)
✓ Chunk 868/4437 written (239 bytes)
✓ Chunk 869/4437 written (239 bytes)
✓ Chunk 870/4437 written (239 bytes)
✓ Chunk 871/4437 written (239 bytes)
✓ Chunk 872/4437 written (239 bytes)
✓ Chunk 873/4437 written (239 bytes)
✓ Chunk 874/4437 written (239 bytes)
✓ Chunk 875/4437 written (239 bytes)
✓ Chunk 876/4437 written (239 bytes)
✓ Chunk 877/4437 written (239 bytes)
✓ Chunk 878/4437 written (239 bytes)
✓ Chunk 879/4437 written (239 bytes)
✓ Chunk 880/4437 written (239 bytes)
✓ Chunk 881/4437 written (239 bytes)
✓ Chunk 882/4437 written (239 bytes)
✓ Chunk 883/4437 written (239 bytes)
✓ Chunk 884/4437 written (239 bytes)
✓ Chunk 885/4437 written (239 bytes)
✓ Chunk 886/4437 written (239 bytes)
✓ Chunk 887/4437 written (239 bytes)
✓ Chunk 888/4437 written (239 bytes)
✓ Chunk 889/4437 written (239 bytes)
✓ Chunk 890/4437 written (239 bytes)
✓ Chunk 891/4437 written (239 bytes)
✓ Chunk 892/4437 written (239 bytes)
✓ Chunk 893/4437 written (239 bytes)
✓ Chunk 894/4437 written (239 bytes)
✓ Chunk 895/4437 written (239 bytes)
✓ Chunk 896/4437 written (239 bytes)
✓ Chunk 897/4437 written (239 bytes)
✓ Chunk 898/4437 written (239 bytes)
✓ Chunk 899/4437 written (239 bytes)
✓ Chunk 900/4437 written (239 bytes)
  [900/4437] Chunk streamed (seq=900)
✓ Chunk 901/4437 written (239 bytes)
✓ Chunk 902/4437 written (239 bytes)
✓ Chunk 903/4437 written (239 bytes)
✓ Chunk 904/4437 written (239 bytes)
✓ Chunk 905/4437 written (239 bytes)
✓ Chunk 906/4437 written (239 bytes)
✓ Chunk 907/4437 written (239 bytes)
✓ Chunk 908/4437 written (239 bytes)
✓ Chunk 909/4437 written (239 bytes)
✓ Chunk 910/4437 written (239 bytes)
✓ Chunk 911/4437 written (239 bytes)
✓ Chunk 912/4437 written (239 bytes)
✓ Chunk 913/4437 written (239 bytes)
✓ Chunk 914/4437 written (239 bytes)
✓ Chunk 915/4437 written (239 bytes)
✓ Chunk 916/4437 written (239 bytes)
✓ Chunk 917/4437 written (239 bytes)
✓ Chunk 918/4437 written (239 bytes)
✓ Chunk 919/4437 written (239 bytes)
✓ Chunk 920/4437 written (239 bytes)
✓ Chunk 921/4437 written (239 bytes)
✓ Chunk 922/4437 written (239 bytes)
✓ Chunk 923/4437 written (239 bytes)
✓ Chunk 924/4437 written (239 bytes)
✓ Chunk 925/4437 written (239 bytes)
✓ Chunk 926/4437 written (239 bytes)
✓ Chunk 927/4437 written (239 bytes)
✓ Chunk 928/4437 written (239 bytes)
✓ Chunk 929/4437 written (239 bytes)
✓ Chunk 930/4437 written (239 bytes)
✓ Chunk 931/4437 written (239 bytes)
✓ Chunk 932/4437 written (239 bytes)
✓ Chunk 933/4437 written (239 bytes)
✓ Chunk 934/4437 written (239 bytes)
✓ Chunk 935/4437 written (239 bytes)
✓ Chunk 936/4437 written (239 bytes)
✓ Chunk 937/4437 written (239 bytes)
✓ Chunk 938/4437 written (239 bytes)
✓ Chunk 939/4437 written (239 bytes)
✓ Chunk 940/4437 written (239 bytes)
✓ Chunk 941/4437 written (239 bytes)
✓ Chunk 942/4437 written (239 bytes)
✓ Chunk 943/4437 written (239 bytes)
✓ Chunk 944/4437 written (239 bytes)
✓ Chunk 945/4437 written (239 bytes)
✓ Chunk 946/4437 written (239 bytes)
✓ Chunk 947/4437 written (239 bytes)
✓ Chunk 948/4437 written (239 bytes)
✓ Chunk 949/4437 written (239 bytes)
✓ Chunk 950/4437 written (239 bytes)
✓ Chunk 951/4437 written (239 bytes)
✓ Chunk 952/4437 written (239 bytes)
✓ Chunk 953/4437 written (239 bytes)
✓ Chunk 954/4437 written (239 bytes)
✓ Chunk 955/4437 written (239 bytes)
✓ Chunk 956/4437 written (239 bytes)
✓ Chunk 957/4437 written (239 bytes)
✓ Chunk 958/4437 written (239 bytes)
✓ Chunk 959/4437 written (239 bytes)
✓ Chunk 960/4437 written (239 bytes)
✓ Chunk 961/4437 written (239 bytes)
✓ Chunk 962/4437 written (239 bytes)
✓ Chunk 963/4437 written (239 bytes)
✓ Chunk 964/4437 written (239 bytes)
✓ Chunk 965/4437 written (239 bytes)
✓ Chunk 966/4437 written (239 bytes)
✓ Chunk 967/4437 written (239 bytes)
✓ Chunk 968/4437 written (239 bytes)
✓ Chunk 969/4437 written (239 bytes)
✓ Chunk 970/4437 written (239 bytes)
✓ Chunk 971/4437 written (239 bytes)
✓ Chunk 972/4437 written (239 bytes)
✓ Chunk 973/4437 written (239 bytes)
✓ Chunk 974/4437 written (239 bytes)
✓ Chunk 975/4437 written (239 bytes)
✓ Chunk 976/4437 written (239 bytes)
✓ Chunk 977/4437 written (239 bytes)
✓ Chunk 978/4437 written (239 bytes)
✓ Chunk 979/4437 written (239 bytes)
✓ Chunk 980/4437 written (239 bytes)
✓ Chunk 981/4437 written (239 bytes)
✓ Chunk 982/4437 written (239 bytes)
✓ Chunk 983/4437 written (239 bytes)
✓ Chunk 984/4437 written (239 bytes)
✓ Chunk 985/4437 written (239 bytes)
✓ Chunk 986/4437 written (239 bytes)
✓ Chunk 987/4437 written (239 bytes)
✓ Chunk 988/4437 written (239 bytes)
✓ Chunk 989/4437 written (239 bytes)
✓ Chunk 990/4437 written (239 bytes)
✓ Chunk 991/4437 written (239 bytes)
✓ Chunk 992/4437 written (239 bytes)
✓ Chunk 993/4437 written (239 bytes)
✓ Chunk 994/4437 written (239 bytes)
✓ Chunk 995/4437 written (239 bytes)
✓ Chunk 996/4437 written (239 bytes)
✓ Chunk 997/4437 written (239 bytes)
✓ Chunk 998/4437 written (239 bytes)
✓ Chunk 999/4437 written (239 bytes)
✓ Chunk 1000/4437 written (239 bytes)
  [1000/4437] Chunk streamed (seq=1000)
✓ Chunk 1001/4437 written (239 bytes)
✓ Chunk 1002/4437 written (239 bytes)
✓ Chunk 1003/4437 written (239 bytes)
✓ Chunk 1004/4437 written (239 bytes)
✓ Chunk 1005/4437 written (239 bytes)
✓ Chunk 1006/4437 written (239 bytes)
✓ Chunk 1007/4437 written (239 bytes)
✓ Chunk 1008/4437 written (239 bytes)
✓ Chunk 1009/4437 written (239 bytes)
✓ Chunk 1010/4437 written (239 bytes)
✓ Chunk 1011/4437 written (239 bytes)
✓ Chunk 1012/4437 written (239 bytes)
✓ Chunk 1013/4437 written (239 bytes)
✓ Chunk 1014/4437 written (239 bytes)
✓ Chunk 1015/4437 written (239 bytes)
✓ Chunk 1016/4437 written (239 bytes)
✓ Chunk 1017/4437 written (239 bytes)
✓ Chunk 1018/4437 written (239 bytes)
✓ Chunk 1019/4437 written (239 bytes)
✓ Chunk 1020/4437 written (239 bytes)
✓ Chunk 1021/4437 written (239 bytes)
✓ Chunk 1022/4437 written (239 bytes)
✓ Chunk 1023/4437 written (239 bytes)
✓ Chunk 1024/4437 written (239 bytes)
✓ Chunk 1025/4437 written (239 bytes)
✓ Chunk 1026/4437 written (239 bytes)
✓ Chunk 1027/4437 written (239 bytes)
✓ Chunk 1028/4437 written (239 bytes)
✓ Chunk 1029/4437 written (239 bytes)
✓ Chunk 1030/4437 written (239 bytes)
✓ Chunk 1031/4437 written (239 bytes)
✓ Chunk 1032/4437 written (239 bytes)
✓ Chunk 1033/4437 written (239 bytes)
✓ Chunk 1034/4437 written (239 bytes)
✓ Chunk 1035/4437 written (239 bytes)
✓ Chunk 1036/4437 written (239 bytes)
✓ Chunk 1037/4437 written (239 bytes)
✓ Chunk 1038/4437 written (239 bytes)
✓ Chunk 1039/4437 written (239 bytes)
✓ Chunk 1040/4437 written (239 bytes)
✓ Chunk 1041/4437 written (239 bytes)
✓ Chunk 1042/4437 written (239 bytes)
✓ Chunk 1043/4437 written (239 bytes)
✓ Chunk 1044/4437 written (239 bytes)
✓ Chunk 1045/4437 written (239 bytes)
✓ Chunk 1046/4437 written (239 bytes)
✓ Chunk 1047/4437 written (239 bytes)
✓ Chunk 1048/4437 written (239 bytes)
✓ Chunk 1049/4437 written (239 bytes)
✓ Chunk 1050/4437 written (239 bytes)
✓ Chunk 1051/4437 written (239 bytes)
✓ Chunk 1052/4437 written (239 bytes)
✓ Chunk 1053/4437 written (239 bytes)
✓ Chunk 1054/4437 written (239 bytes)
✓ Chunk 1055/4437 written (239 bytes)
✓ Chunk 1056/4437 written (239 bytes)
✓ Chunk 1057/4437 written (239 bytes)
✓ Chunk 1058/4437 written (239 bytes)
✓ Chunk 1059/4437 written (239 bytes)
✓ Chunk 1060/4437 written (239 bytes)
✓ Chunk 1061/4437 written (239 bytes)
✓ Chunk 1062/4437 written (239 bytes)
✓ Chunk 1063/4437 written (239 bytes)
✓ Chunk 1064/4437 written (239 bytes)
✓ Chunk 1065/4437 written (239 bytes)
✓ Chunk 1066/4437 written (239 bytes)
✓ Chunk 1067/4437 written (239 bytes)
✓ Chunk 1068/4437 written (239 bytes)
✓ Chunk 1069/4437 written (239 bytes)
✓ Chunk 1070/4437 written (239 bytes)
✓ Chunk 1071/4437 written (239 bytes)
✓ Chunk 1072/4437 written (239 bytes)
✓ Chunk 1073/4437 written (239 bytes)
✓ Chunk 1074/4437 written (239 bytes)
✓ Chunk 1075/4437 written (239 bytes)
✓ Chunk 1076/4437 written (239 bytes)
✓ Chunk 1077/4437 written (239 bytes)
✓ Chunk 1078/4437 written (239 bytes)
✓ Chunk 1079/4437 written (239 bytes)
✓ Chunk 1080/4437 written (239 bytes)
✓ Chunk 1081/4437 written (239 bytes)
✓ Chunk 1082/4437 written (239 bytes)
✓ Chunk 1083/4437 written (239 bytes)
✓ Chunk 1084/4437 written (239 bytes)
✓ Chunk 1085/4437 written (239 bytes)
✓ Chunk 1086/4437 written (239 bytes)
✓ Chunk 1087/4437 written (239 bytes)
✓ Chunk 1088/4437 written (239 bytes)
✓ Chunk 1089/4437 written (239 bytes)
✓ Chunk 1090/4437 written (239 bytes)
✓ Chunk 1091/4437 written (239 bytes)
✓ Chunk 1092/4437 written (239 bytes)
✓ Chunk 1093/4437 written (239 bytes)
✓ Chunk 1094/4437 written (239 bytes)
✓ Chunk 1095/4437 written (239 bytes)
✓ Chunk 1096/4437 written (239 bytes)
✓ Chunk 1097/4437 written (239 bytes)
✓ Chunk 1098/4437 written (239 bytes)
✓ Chunk 1099/4437 written (239 bytes)
✓ Chunk 1100/4437 written (239 bytes)
  [1100/4437] Chunk streamed (seq=1100)
✓ Chunk 1101/4437 written (239 bytes)
✓ Chunk 1102/4437 written (239 bytes)
✓ Chunk 1103/4437 written (239 bytes)
✓ Chunk 1104/4437 written (239 bytes)
✓ Chunk 1105/4437 written (239 bytes)
✓ Chunk 1106/4437 written (239 bytes)
✓ Chunk 1107/4437 written (239 bytes)
✓ Chunk 1108/4437 written (239 bytes)
✓ Chunk 1109/4437 written (239 bytes)
✓ Chunk 1110/4437 written (239 bytes)
✓ Chunk 1111/4437 written (239 bytes)
✓ Chunk 1112/4437 written (239 bytes)
✓ Chunk 1113/4437 written (239 bytes)
✓ Chunk 1114/4437 written (239 bytes)
✓ Chunk 1115/4437 written (239 bytes)
✓ Chunk 1116/4437 written (239 bytes)
✓ Chunk 1117/4437 written (239 bytes)
✓ Chunk 1118/4437 written (239 bytes)
✓ Chunk 1119/4437 written (239 bytes)
✓ Chunk 1120/4437 written (239 bytes)
✓ Chunk 1121/4437 written (239 bytes)
✓ Chunk 1122/4437 written (239 bytes)
✓ Chunk 1123/4437 written (239 bytes)
✓ Chunk 1124/4437 written (239 bytes)
✓ Chunk 1125/4437 written (239 bytes)
✓ Chunk 1126/4437 written (239 bytes)
✓ Chunk 1127/4437 written (239 bytes)
✓ Chunk 1128/4437 written (239 bytes)
✓ Chunk 1129/4437 written (239 bytes)
✓ Chunk 1130/4437 written (239 bytes)
✓ Chunk 1131/4437 written (239 bytes)
✓ Chunk 1132/4437 written (239 bytes)
✓ Chunk 1133/4437 written (239 bytes)
✓ Chunk 1134/4437 written (239 bytes)
✓ Chunk 1135/4437 written (239 bytes)
✓ Chunk 1136/4437 written (239 bytes)
✓ Chunk 1137/4437 written (239 bytes)
✓ Chunk 1138/4437 written (239 bytes)
✓ Chunk 1139/4437 written (239 bytes)
✓ Chunk 1140/4437 written (239 bytes)
✓ Chunk 1141/4437 written (239 bytes)
✓ Chunk 1142/4437 written (239 bytes)
✓ Chunk 1143/4437 written (239 bytes)
✓ Chunk 1144/4437 written (239 bytes)
✓ Chunk 1145/4437 written (239 bytes)
✓ Chunk 1146/4437 written (239 bytes)
✓ Chunk 1147/4437 written (239 bytes)
✓ Chunk 1148/4437 written (239 bytes)
✓ Chunk 1149/4437 written (239 bytes)
✓ Chunk 1150/4437 written (239 bytes)
✓ Chunk 1151/4437 written (239 bytes)
✓ Chunk 1152/4437 written (239 bytes)
✓ Chunk 1153/4437 written (239 bytes)
✓ Chunk 1154/4437 written (239 bytes)
✓ Chunk 1155/4437 written (239 bytes)
✓ Chunk 1156/4437 written (239 bytes)
✓ Chunk 1157/4437 written (239 bytes)
✓ Chunk 1158/4437 written (239 bytes)
✓ Chunk 1159/4437 written (239 bytes)
✓ Chunk 1160/4437 written (239 bytes)
✓ Chunk 1161/4437 written (239 bytes)
✓ Chunk 1162/4437 written (239 bytes)
✓ Chunk 1163/4437 written (239 bytes)
✓ Chunk 1164/4437 written (239 bytes)
✓ Chunk 1165/4437 written (239 bytes)
✓ Chunk 1166/4437 written (239 bytes)
✓ Chunk 1167/4437 written (239 bytes)
✓ Chunk 1168/4437 written (239 bytes)
✓ Chunk 1169/4437 written (239 bytes)
✓ Chunk 1170/4437 written (239 bytes)
✓ Chunk 1171/4437 written (239 bytes)
✓ Chunk 1172/4437 written (239 bytes)
✓ Chunk 1173/4437 written (239 bytes)
✓ Chunk 1174/4437 written (239 bytes)
✓ Chunk 1175/4437 written (239 bytes)
✓ Chunk 1176/4437 written (239 bytes)
✓ Chunk 1177/4437 written (239 bytes)
✓ Chunk 1178/4437 written (239 bytes)
✓ Chunk 1179/4437 written (239 bytes)
✓ Chunk 1180/4437 written (239 bytes)
✓ Chunk 1181/4437 written (239 bytes)
✓ Chunk 1182/4437 written (239 bytes)
✓ Chunk 1183/4437 written (239 bytes)
✓ Chunk 1184/4437 written (239 bytes)
✓ Chunk 1185/4437 written (239 bytes)
✓ Chunk 1186/4437 written (239 bytes)
✓ Chunk 1187/4437 written (239 bytes)
✓ Chunk 1188/4437 written (239 bytes)
✓ Chunk 1189/4437 written (239 bytes)
✓ Chunk 1190/4437 written (239 bytes)
✓ Chunk 1191/4437 written (239 bytes)
✓ Chunk 1192/4437 written (239 bytes)
✓ Chunk 1193/4437 written (239 bytes)
✓ Chunk 1194/4437 written (239 bytes)
✓ Chunk 1195/4437 written (239 bytes)
✓ Chunk 1196/4437 written (239 bytes)
✓ Chunk 1197/4437 written (239 bytes)
✓ Chunk 1198/4437 written (239 bytes)
✓ Chunk 1199/4437 written (239 bytes)
✓ Chunk 1200/4437 written (239 bytes)
  [1200/4437] Chunk streamed (seq=1200)
✓ Chunk 1201/4437 written (239 bytes)
✓ Chunk 1202/4437 written (239 bytes)
✓ Chunk 1203/4437 written (239 bytes)
✓ Chunk 1204/4437 written (239 bytes)
✓ Chunk 1205/4437 written (239 bytes)
✓ Chunk 1206/4437 written (239 bytes)
✓ Chunk 1207/4437 written (239 bytes)
✓ Chunk 1208/4437 written (239 bytes)
✓ Chunk 1209/4437 written (239 bytes)
✓ Chunk 1210/4437 written (239 bytes)
✓ Chunk 1211/4437 written (239 bytes)
✓ Chunk 1212/4437 written (239 bytes)
✓ Chunk 1213/4437 written (239 bytes)
✓ Chunk 1214/4437 written (239 bytes)
✓ Chunk 1215/4437 written (239 bytes)
✓ Chunk 1216/4437 written (239 bytes)
✓ Chunk 1217/4437 written (239 bytes)
✓ Chunk 1218/4437 written (239 bytes)
✓ Chunk 1219/4437 written (239 bytes)
✓ Chunk 1220/4437 written (239 bytes)
✓ Chunk 1221/4437 written (239 bytes)
✓ Chunk 1222/4437 written (239 bytes)
✓ Chunk 1223/4437 written (239 bytes)
✓ Chunk 1224/4437 written (239 bytes)
✓ Chunk 1225/4437 written (239 bytes)
✓ Chunk 1226/4437 written (239 bytes)
✓ Chunk 1227/4437 written (239 bytes)
✓ Chunk 1228/4437 written (239 bytes)
✓ Chunk 1229/4437 written (239 bytes)
✓ Chunk 1230/4437 written (239 bytes)
✓ Chunk 1231/4437 written (239 bytes)
✓ Chunk 1232/4437 written (239 bytes)
✓ Chunk 1233/4437 written (239 bytes)
✓ Chunk 1234/4437 written (239 bytes)
✓ Chunk 1235/4437 written (239 bytes)
✓ Chunk 1236/4437 written (239 bytes)
✓ Chunk 1237/4437 written (239 bytes)
✓ Chunk 1238/4437 written (239 bytes)
✓ Chunk 1239/4437 written (239 bytes)
✓ Chunk 1240/4437 written (239 bytes)
✓ Chunk 1241/4437 written (239 bytes)
✓ Chunk 1242/4437 written (239 bytes)
✓ Chunk 1243/4437 written (239 bytes)
✓ Chunk 1244/4437 written (239 bytes)
✓ Chunk 1245/4437 written (239 bytes)
✓ Chunk 1246/4437 written (239 bytes)
✓ Chunk 1247/4437 written (239 bytes)
✓ Chunk 1248/4437 written (239 bytes)
✓ Chunk 1249/4437 written (239 bytes)
✓ Chunk 1250/4437 written (239 bytes)
✓ Chunk 1251/4437 written (239 bytes)
✓ Chunk 1252/4437 written (239 bytes)
✓ Chunk 1253/4437 written (239 bytes)
✓ Chunk 1254/4437 written (239 bytes)
✓ Chunk 1255/4437 written (239 bytes)
✓ Chunk 1256/4437 written (239 bytes)
✓ Chunk 1257/4437 written (239 bytes)
✓ Chunk 1258/4437 written (239 bytes)
✓ Chunk 1259/4437 written (239 bytes)
✓ Chunk 1260/4437 written (239 bytes)
✓ Chunk 1261/4437 written (239 bytes)
✓ Chunk 1262/4437 written (239 bytes)
✓ Chunk 1263/4437 written (239 bytes)
✓ Chunk 1264/4437 written (239 bytes)
✓ Chunk 1265/4437 written (239 bytes)
✓ Chunk 1266/4437 written (239 bytes)
✓ Chunk 1267/4437 written (239 bytes)
✓ Chunk 1268/4437 written (239 bytes)
✓ Chunk 1269/4437 written (239 bytes)
✓ Chunk 1270/4437 written (239 bytes)
✓ Chunk 1271/4437 written (239 bytes)
✓ Chunk 1272/4437 written (239 bytes)
✓ Chunk 1273/4437 written (239 bytes)
✓ Chunk 1274/4437 written (239 bytes)
✓ Chunk 1275/4437 written (239 bytes)
✓ Chunk 1276/4437 written (239 bytes)
✓ Chunk 1277/4437 written (239 bytes)
✓ Chunk 1278/4437 written (239 bytes)
✓ Chunk 1279/4437 written (239 bytes)
✓ Chunk 1280/4437 written (239 bytes)
✓ Chunk 1281/4437 written (239 bytes)
✓ Chunk 1282/4437 written (239 bytes)
✓ Chunk 1283/4437 written (239 bytes)
✓ Chunk 1284/4437 written (239 bytes)
✓ Chunk 1285/4437 written (239 bytes)
✓ Chunk 1286/4437 written (239 bytes)
✓ Chunk 1287/4437 written (239 bytes)
✓ Chunk 1288/4437 written (239 bytes)
✓ Chunk 1289/4437 written (239 bytes)
✓ Chunk 1290/4437 written (239 bytes)
✓ Chunk 1291/4437 written (239 bytes)
✓ Chunk 1292/4437 written (239 bytes)
✓ Chunk 1293/4437 written (239 bytes)
✓ Chunk 1294/4437 written (239 bytes)
✓ Chunk 1295/4437 written (239 bytes)
✓ Chunk 1296/4437 written (239 bytes)
✓ Chunk 1297/4437 written (239 bytes)
✓ Chunk 1298/4437 written (239 bytes)
✓ Chunk 1299/4437 written (239 bytes)
✓ Chunk 1300/4437 written (239 bytes)
  [1300/4437] Chunk streamed (seq=1300)
✓ Chunk 1301/4437 written (239 bytes)
✓ Chunk 1302/4437 written (239 bytes)
✓ Chunk 1303/4437 written (239 bytes)
✓ Chunk 1304/4437 written (239 bytes)
✓ Chunk 1305/4437 written (239 bytes)
✓ Chunk 1306/4437 written (239 bytes)
✓ Chunk 1307/4437 written (239 bytes)
✓ Chunk 1308/4437 written (239 bytes)
✓ Chunk 1309/4437 written (239 bytes)
✓ Chunk 1310/4437 written (239 bytes)
✓ Chunk 1311/4437 written (239 bytes)
✓ Chunk 1312/4437 written (239 bytes)
✓ Chunk 1313/4437 written (239 bytes)
✓ Chunk 1314/4437 written (239 bytes)
✓ Chunk 1315/4437 written (239 bytes)
✓ Chunk 1316/4437 written (239 bytes)
✓ Chunk 1317/4437 written (239 bytes)
✓ Chunk 1318/4437 written (239 bytes)
✓ Chunk 1319/4437 written (239 bytes)
✓ Chunk 1320/4437 written (239 bytes)
✓ Chunk 1321/4437 written (239 bytes)
✓ Chunk 1322/4437 written (239 bytes)
✓ Chunk 1323/4437 written (239 bytes)
✓ Chunk 1324/4437 written (239 bytes)
✓ Chunk 1325/4437 written (239 bytes)
✓ Chunk 1326/4437 written (239 bytes)
✓ Chunk 1327/4437 written (239 bytes)
✓ Chunk 1328/4437 written (239 bytes)
✓ Chunk 1329/4437 written (239 bytes)
✓ Chunk 1330/4437 written (239 bytes)
✓ Chunk 1331/4437 written (239 bytes)
✓ Chunk 1332/4437 written (239 bytes)
✓ Chunk 1333/4437 written (239 bytes)
✓ Chunk 1334/4437 written (239 bytes)
✓ Chunk 1335/4437 written (239 bytes)
✓ Chunk 1336/4437 written (239 bytes)
✓ Chunk 1337/4437 written (239 bytes)
✓ Chunk 1338/4437 written (239 bytes)
✓ Chunk 1339/4437 written (239 bytes)
✓ Chunk 1340/4437 written (239 bytes)
✓ Chunk 1341/4437 written (239 bytes)
✓ Chunk 1342/4437 written (239 bytes)
✓ Chunk 1343/4437 written (239 bytes)
✓ Chunk 1344/4437 written (239 bytes)
✓ Chunk 1345/4437 written (239 bytes)
✓ Chunk 1346/4437 written (239 bytes)
✓ Chunk 1347/4437 written (239 bytes)
✓ Chunk 1348/4437 written (239 bytes)
✓ Chunk 1349/4437 written (239 bytes)
✓ Chunk 1350/4437 written (239 bytes)
✓ Chunk 1351/4437 written (239 bytes)
✓ Chunk 1352/4437 written (239 bytes)
✓ Chunk 1353/4437 written (239 bytes)
✓ Chunk 1354/4437 written (239 bytes)
✓ Chunk 1355/4437 written (239 bytes)
✓ Chunk 1356/4437 written (239 bytes)
✓ Chunk 1357/4437 written (239 bytes)
✓ Chunk 1358/4437 written (239 bytes)
✓ Chunk 1359/4437 written (239 bytes)
✓ Chunk 1360/4437 written (239 bytes)
✓ Chunk 1361/4437 written (239 bytes)
✓ Chunk 1362/4437 written (239 bytes)
✓ Chunk 1363/4437 written (239 bytes)
✓ Chunk 1364/4437 written (239 bytes)
✓ Chunk 1365/4437 written (239 bytes)
✓ Chunk 1366/4437 written (239 bytes)
✓ Chunk 1367/4437 written (239 bytes)
✓ Chunk 1368/4437 written (239 bytes)
✓ Chunk 1369/4437 written (239 bytes)
✓ Chunk 1370/4437 written (239 bytes)
✓ Chunk 1371/4437 written (239 bytes)
✓ Chunk 1372/4437 written (239 bytes)
✓ Chunk 1373/4437 written (239 bytes)
✓ Chunk 1374/4437 written (239 bytes)
✓ Chunk 1375/4437 written (239 bytes)
✓ Chunk 1376/4437 written (239 bytes)
✓ Chunk 1377/4437 written (239 bytes)
✓ Chunk 1378/4437 written (239 bytes)
✓ Chunk 1379/4437 written (239 bytes)
✓ Chunk 1380/4437 written (239 bytes)
✓ Chunk 1381/4437 written (239 bytes)
✓ Chunk 1382/4437 written (239 bytes)
✓ Chunk 1383/4437 written (239 bytes)
✓ Chunk 1384/4437 written (239 bytes)
✓ Chunk 1385/4437 written (239 bytes)
✓ Chunk 1386/4437 written (239 bytes)
✓ Chunk 1387/4437 written (239 bytes)
✓ Chunk 1388/4437 written (239 bytes)
✓ Chunk 1389/4437 written (239 bytes)
✓ Chunk 1390/4437 written (239 bytes)
✓ Chunk 1391/4437 written (239 bytes)
✓ Chunk 1392/4437 written (239 bytes)
✓ Chunk 1393/4437 written (239 bytes)
✓ Chunk 1394/4437 written (239 bytes)
✓ Chunk 1395/4437 written (239 bytes)
✓ Chunk 1396/4437 written (239 bytes)
✓ Chunk 1397/4437 written (239 bytes)
✓ Chunk 1398/4437 written (239 bytes)
✓ Chunk 1399/4437 written (239 bytes)
✓ Chunk 1400/4437 written (239 bytes)
  [1400/4437] Chunk streamed (seq=1400)
✓ Chunk 1401/4437 written (239 bytes)
✓ Chunk 1402/4437 written (239 bytes)
✓ Chunk 1403/4437 written (239 bytes)
✓ Chunk 1404/4437 written (239 bytes)
✓ Chunk 1405/4437 written (239 bytes)
✓ Chunk 1406/4437 written (239 bytes)
✓ Chunk 1407/4437 written (239 bytes)
✓ Chunk 1408/4437 written (239 bytes)
✓ Chunk 1409/4437 written (239 bytes)
✓ Chunk 1410/4437 written (239 bytes)
✓ Chunk 1411/4437 written (239 bytes)
✓ Chunk 1412/4437 written (239 bytes)
✓ Chunk 1413/4437 written (239 bytes)
✓ Chunk 1414/4437 written (239 bytes)
✓ Chunk 1415/4437 written (239 bytes)
✓ Chunk 1416/4437 written (239 bytes)
✓ Chunk 1417/4437 written (239 bytes)
✓ Chunk 1418/4437 written (239 bytes)
✓ Chunk 1419/4437 written (239 bytes)
✓ Chunk 1420/4437 written (239 bytes)
✓ Chunk 1421/4437 written (239 bytes)
✓ Chunk 1422/4437 written (239 bytes)
✓ Chunk 1423/4437 written (239 bytes)
✓ Chunk 1424/4437 written (239 bytes)
✓ Chunk 1425/4437 written (239 bytes)
✓ Chunk 1426/4437 written (239 bytes)
✓ Chunk 1427/4437 written (239 bytes)
✓ Chunk 1428/4437 written (239 bytes)
✓ Chunk 1429/4437 written (239 bytes)
✓ Chunk 1430/4437 written (239 bytes)
✓ Chunk 1431/4437 written (239 bytes)
✓ Chunk 1432/4437 written (239 bytes)
✓ Chunk 1433/4437 written (239 bytes)
✓ Chunk 1434/4437 written (239 bytes)
✓ Chunk 1435/4437 written (239 bytes)
✓ Chunk 1436/4437 written (239 bytes)
✓ Chunk 1437/4437 written (239 bytes)
✓ Chunk 1438/4437 written (239 bytes)
✓ Chunk 1439/4437 written (239 bytes)
✓ Chunk 1440/4437 written (239 bytes)
✓ Chunk 1441/4437 written (239 bytes)
✓ Chunk 1442/4437 written (239 bytes)
✓ Chunk 1443/4437 written (239 bytes)
✓ Chunk 1444/4437 written (239 bytes)
✓ Chunk 1445/4437 written (239 bytes)
✓ Chunk 1446/4437 written (239 bytes)
✓ Chunk 1447/4437 written (239 bytes)
✓ Chunk 1448/4437 written (239 bytes)
✓ Chunk 1449/4437 written (239 bytes)
✓ Chunk 1450/4437 written (239 bytes)
✓ Chunk 1451/4437 written (239 bytes)
✓ Chunk 1452/4437 written (239 bytes)
✓ Chunk 1453/4437 written (239 bytes)
✓ Chunk 1454/4437 written (239 bytes)
✓ Chunk 1455/4437 written (239 bytes)
✓ Chunk 1456/4437 written (239 bytes)
✓ Chunk 1457/4437 written (239 bytes)
✓ Chunk 1458/4437 written (239 bytes)
✓ Chunk 1459/4437 written (239 bytes)
✓ Chunk 1460/4437 written (239 bytes)
✓ Chunk 1461/4437 written (239 bytes)
✓ Chunk 1462/4437 written (239 bytes)
✓ Chunk 1463/4437 written (239 bytes)
✓ Chunk 1464/4437 written (239 bytes)
✓ Chunk 1465/4437 written (239 bytes)
✓ Chunk 1466/4437 written (239 bytes)
✓ Chunk 1467/4437 written (239 bytes)
✓ Chunk 1468/4437 written (239 bytes)
✓ Chunk 1469/4437 written (239 bytes)
✓ Chunk 1470/4437 written (239 bytes)
✓ Chunk 1471/4437 written (239 bytes)
✓ Chunk 1472/4437 written (239 bytes)
✓ Chunk 1473/4437 written (239 bytes)
✓ Chunk 1474/4437 written (239 bytes)
✓ Chunk 1475/4437 written (239 bytes)
✓ Chunk 1476/4437 written (239 bytes)
✓ Chunk 1477/4437 written (239 bytes)
✓ Chunk 1478/4437 written (239 bytes)
✓ Chunk 1479/4437 written (239 bytes)
✓ Chunk 1480/4437 written (239 bytes)
✓ Chunk 1481/4437 written (239 bytes)
✓ Chunk 1482/4437 written (239 bytes)
✓ Chunk 1483/4437 written (239 bytes)
✓ Chunk 1484/4437 written (239 bytes)
✓ Chunk 1485/4437 written (239 bytes)
✓ Chunk 1486/4437 written (239 bytes)
✓ Chunk 1487/4437 written (239 bytes)
✓ Chunk 1488/4437 written (239 bytes)
✓ Chunk 1489/4437 written (239 bytes)
✓ Chunk 1490/4437 written (239 bytes)
✓ Chunk 1491/4437 written (239 bytes)
✓ Chunk 1492/4437 written (239 bytes)
✓ Chunk 1493/4437 written (239 bytes)
✓ Chunk 1494/4437 written (239 bytes)
✓ Chunk 1495/4437 written (239 bytes)
✓ Chunk 1496/4437 written (239 bytes)
✓ Chunk 1497/4437 written (239 bytes)
✓ Chunk 1498/4437 written (239 bytes)
✓ Chunk 1499/4437 written (239 bytes)
✓ Chunk 1500/4437 written (239 bytes)
  [1500/4437] Chunk streamed (seq=1500)
✓ Chunk 1501/4437 written (239 bytes)
✓ Chunk 1502/4437 written (239 bytes)
✓ Chunk 1503/4437 written (239 bytes)
✓ Chunk 1504/4437 written (239 bytes)
✓ Chunk 1505/4437 written (239 bytes)
✓ Chunk 1506/4437 written (239 bytes)
✓ Chunk 1507/4437 written (239 bytes)
✓ Chunk 1508/4437 written (239 bytes)
✓ Chunk 1509/4437 written (239 bytes)
✓ Chunk 1510/4437 written (239 bytes)
✓ Chunk 1511/4437 written (239 bytes)
✓ Chunk 1512/4437 written (239 bytes)
✓ Chunk 1513/4437 written (239 bytes)
✓ Chunk 1514/4437 written (239 bytes)
✓ Chunk 1515/4437 written (239 bytes)
✓ Chunk 1516/4437 written (239 bytes)
✓ Chunk 1517/4437 written (239 bytes)
✓ Chunk 1518/4437 written (239 bytes)
✓ Chunk 1519/4437 written (239 bytes)
✓ Chunk 1520/4437 written (239 bytes)
✓ Chunk 1521/4437 written (239 bytes)
✓ Chunk 1522/4437 written (239 bytes)
✓ Chunk 1523/4437 written (239 bytes)
✓ Chunk 1524/4437 written (239 bytes)
✓ Chunk 1525/4437 written (239 bytes)
✓ Chunk 1526/4437 written (239 bytes)
✓ Chunk 1527/4437 written (239 bytes)
✓ Chunk 1528/4437 written (239 bytes)
✓ Chunk 1529/4437 written (239 bytes)
✓ Chunk 1530/4437 written (239 bytes)
✓ Chunk 1531/4437 written (239 bytes)
✓ Chunk 1532/4437 written (239 bytes)
✓ Chunk 1533/4437 written (239 bytes)
✓ Chunk 1534/4437 written (239 bytes)
✓ Chunk 1535/4437 written (239 bytes)
✓ Chunk 1536/4437 written (239 bytes)
✓ Chunk 1537/4437 written (239 bytes)
✓ Chunk 1538/4437 written (239 bytes)
✓ Chunk 1539/4437 written (239 bytes)
✓ Chunk 1540/4437 written (239 bytes)
✓ Chunk 1541/4437 written (239 bytes)
✓ Chunk 1542/4437 written (239 bytes)
✓ Chunk 1543/4437 written (239 bytes)
✓ Chunk 1544/4437 written (239 bytes)
✓ Chunk 1545/4437 written (239 bytes)
✓ Chunk 1546/4437 written (239 bytes)
✓ Chunk 1547/4437 written (239 bytes)
✓ Chunk 1548/4437 written (239 bytes)
✓ Chunk 1549/4437 written (239 bytes)
✓ Chunk 1550/4437 written (239 bytes)
✓ Chunk 1551/4437 written (239 bytes)
✓ Chunk 1552/4437 written (239 bytes)
✓ Chunk 1553/4437 written (239 bytes)
✓ Chunk 1554/4437 written (239 bytes)
✓ Chunk 1555/4437 written (239 bytes)
✓ Chunk 1556/4437 written (239 bytes)
✓ Chunk 1557/4437 written (239 bytes)
✓ Chunk 1558/4437 written (239 bytes)
✓ Chunk 1559/4437 written (239 bytes)
✓ Chunk 1560/4437 written (239 bytes)
✓ Chunk 1561/4437 written (239 bytes)
✓ Chunk 1562/4437 written (239 bytes)
✓ Chunk 1563/4437 written (239 bytes)
✓ Chunk 1564/4437 written (239 bytes)
✓ Chunk 1565/4437 written (239 bytes)
✓ Chunk 1566/4437 written (239 bytes)
✓ Chunk 1567/4437 written (239 bytes)
✓ Chunk 1568/4437 written (239 bytes)
✓ Chunk 1569/4437 written (239 bytes)
✓ Chunk 1570/4437 written (239 bytes)
✓ Chunk 1571/4437 written (239 bytes)
✓ Chunk 1572/4437 written (239 bytes)
✓ Chunk 1573/4437 written (239 bytes)
✓ Chunk 1574/4437 written (239 bytes)
✓ Chunk 1575/4437 written (239 bytes)
✓ Chunk 1576/4437 written (239 bytes)
✓ Chunk 1577/4437 written (239 bytes)
✓ Chunk 1578/4437 written (239 bytes)
✓ Chunk 1579/4437 written (239 bytes)
✓ Chunk 1580/4437 written (239 bytes)
✓ Chunk 1581/4437 written (239 bytes)
✓ Chunk 1582/4437 written (239 bytes)
✓ Chunk 1583/4437 written (239 bytes)
✓ Chunk 1584/4437 written (239 bytes)
✓ Chunk 1585/4437 written (239 bytes)
✓ Chunk 1586/4437 written (239 bytes)
✓ Chunk 1587/4437 written (239 bytes)
✓ Chunk 1588/4437 written (239 bytes)
✓ Chunk 1589/4437 written (239 bytes)
✓ Chunk 1590/4437 written (239 bytes)
✓ Chunk 1591/4437 written (239 bytes)
✓ Chunk 1592/4437 written (239 bytes)
✓ Chunk 1593/4437 written (239 bytes)
✓ Chunk 1594/4437 written (239 bytes)
✓ Chunk 1595/4437 written (239 bytes)
✓ Chunk 1596/4437 written (239 bytes)
✓ Chunk 1597/4437 written (239 bytes)
✓ Chunk 1598/4437 written (239 bytes)
✓ Chunk 1599/4437 written (239 bytes)
✓ Chunk 1600/4437 written (239 bytes)
  [1600/4437] Chunk streamed (seq=1600)
✓ Chunk 1601/4437 written (239 bytes)
✓ Chunk 1602/4437 written (239 bytes)
✓ Chunk 1603/4437 written (239 bytes)
✓ Chunk 1604/4437 written (239 bytes)
✓ Chunk 1605/4437 written (239 bytes)
✓ Chunk 1606/4437 written (239 bytes)
✓ Chunk 1607/4437 written (239 bytes)
✓ Chunk 1608/4437 written (239 bytes)
✓ Chunk 1609/4437 written (239 bytes)
✓ Chunk 1610/4437 written (239 bytes)
✓ Chunk 1611/4437 written (239 bytes)
✓ Chunk 1612/4437 written (239 bytes)
✓ Chunk 1613/4437 written (239 bytes)
✓ Chunk 1614/4437 written (239 bytes)
✓ Chunk 1615/4437 written (239 bytes)
✓ Chunk 1616/4437 written (239 bytes)
✓ Chunk 1617/4437 written (239 bytes)
✓ Chunk 1618/4437 written (239 bytes)
✓ Chunk 1619/4437 written (239 bytes)
✓ Chunk 1620/4437 written (239 bytes)
✓ Chunk 1621/4437 written (239 bytes)
✓ Chunk 1622/4437 written (239 bytes)
✓ Chunk 1623/4437 written (239 bytes)
✓ Chunk 1624/4437 written (239 bytes)
✓ Chunk 1625/4437 written (239 bytes)
✓ Chunk 1626/4437 written (239 bytes)
✓ Chunk 1627/4437 written (239 bytes)
✓ Chunk 1628/4437 written (239 bytes)
✓ Chunk 1629/4437 written (239 bytes)
✓ Chunk 1630/4437 written (239 bytes)
✓ Chunk 1631/4437 written (239 bytes)
✓ Chunk 1632/4437 written (239 bytes)
✓ Chunk 1633/4437 written (239 bytes)
✓ Chunk 1634/4437 written (239 bytes)
✓ Chunk 1635/4437 written (239 bytes)
✓ Chunk 1636/4437 written (239 bytes)
✓ Chunk 1637/4437 written (239 bytes)
✓ Chunk 1638/4437 written (239 bytes)
✓ Chunk 1639/4437 written (239 bytes)
✓ Chunk 1640/4437 written (239 bytes)
✓ Chunk 1641/4437 written (239 bytes)
✓ Chunk 1642/4437 written (239 bytes)
✓ Chunk 1643/4437 written (239 bytes)
✓ Chunk 1644/4437 written (239 bytes)
✓ Chunk 1645/4437 written (239 bytes)
✓ Chunk 1646/4437 written (239 bytes)
✓ Chunk 1647/4437 written (239 bytes)
✓ Chunk 1648/4437 written (239 bytes)
✓ Chunk 1649/4437 written (239 bytes)
✓ Chunk 1650/4437 written (239 bytes)
✓ Chunk 1651/4437 written (239 bytes)
✓ Chunk 1652/4437 written (239 bytes)
✓ Chunk 1653/4437 written (239 bytes)
✓ Chunk 1654/4437 written (239 bytes)
✓ Chunk 1655/4437 written (239 bytes)
✓ Chunk 1656/4437 written (239 bytes)
✓ Chunk 1657/4437 written (239 bytes)
✓ Chunk 1658/4437 written (239 bytes)
✓ Chunk 1659/4437 written (239 bytes)
✓ Chunk 1660/4437 written (239 bytes)
✓ Chunk 1661/4437 written (239 bytes)
✓ Chunk 1662/4437 written (239 bytes)
✓ Chunk 1663/4437 written (239 bytes)
✓ Chunk 1664/4437 written (239 bytes)
✓ Chunk 1665/4437 written (239 bytes)
✓ Chunk 1666/4437 written (239 bytes)
✓ Chunk 1667/4437 written (239 bytes)
✓ Chunk 1668/4437 written (239 bytes)
✓ Chunk 1669/4437 written (239 bytes)
✓ Chunk 1670/4437 written (239 bytes)
✓ Chunk 1671/4437 written (239 bytes)
✓ Chunk 1672/4437 written (239 bytes)
✓ Chunk 1673/4437 written (239 bytes)
✓ Chunk 1674/4437 written (239 bytes)
✓ Chunk 1675/4437 written (239 bytes)
✓ Chunk 1676/4437 written (239 bytes)
✓ Chunk 1677/4437 written (239 bytes)
✓ Chunk 1678/4437 written (239 bytes)
✓ Chunk 1679/4437 written (239 bytes)
✓ Chunk 1680/4437 written (239 bytes)
✓ Chunk 1681/4437 written (239 bytes)
✓ Chunk 1682/4437 written (239 bytes)
✓ Chunk 1683/4437 written (239 bytes)
✓ Chunk 1684/4437 written (239 bytes)
✓ Chunk 1685/4437 written (239 bytes)
✓ Chunk 1686/4437 written (239 bytes)
✓ Chunk 1687/4437 written (239 bytes)
✓ Chunk 1688/4437 written (239 bytes)
✓ Chunk 1689/4437 written (239 bytes)
✓ Chunk 1690/4437 written (239 bytes)
✓ Chunk 1691/4437 written (239 bytes)
✓ Chunk 1692/4437 written (239 bytes)
✓ Chunk 1693/4437 written (239 bytes)
✓ Chunk 1694/4437 written (239 bytes)
✓ Chunk 1695/4437 written (239 bytes)
✓ Chunk 1696/4437 written (239 bytes)
✓ Chunk 1697/4437 written (239 bytes)
✓ Chunk 1698/4437 written (239 bytes)
✓ Chunk 1699/4437 written (239 bytes)
✓ Chunk 1700/4437 written (239 bytes)
  [1700/4437] Chunk streamed (seq=1700)
✓ Chunk 1701/4437 written (239 bytes)
✓ Chunk 1702/4437 written (239 bytes)
✓ Chunk 1703/4437 written (239 bytes)
✓ Chunk 1704/4437 written (239 bytes)
✓ Chunk 1705/4437 written (239 bytes)
✓ Chunk 1706/4437 written (239 bytes)
✓ Chunk 1707/4437 written (239 bytes)
✓ Chunk 1708/4437 written (239 bytes)
✓ Chunk 1709/4437 written (239 bytes)
✓ Chunk 1710/4437 written (239 bytes)
✓ Chunk 1711/4437 written (239 bytes)
✓ Chunk 1712/4437 written (239 bytes)
✓ Chunk 1713/4437 written (239 bytes)
✓ Chunk 1714/4437 written (239 bytes)
✓ Chunk 1715/4437 written (239 bytes)
✓ Chunk 1716/4437 written (239 bytes)
✓ Chunk 1717/4437 written (239 bytes)
✓ Chunk 1718/4437 written (239 bytes)
✓ Chunk 1719/4437 written (239 bytes)
✓ Chunk 1720/4437 written (239 bytes)
✓ Chunk 1721/4437 written (239 bytes)
✓ Chunk 1722/4437 written (239 bytes)
✓ Chunk 1723/4437 written (239 bytes)
✓ Chunk 1724/4437 written (239 bytes)
✓ Chunk 1725/4437 written (239 bytes)
✓ Chunk 1726/4437 written (239 bytes)
✓ Chunk 1727/4437 written (239 bytes)
✓ Chunk 1728/4437 written (239 bytes)
✓ Chunk 1729/4437 written (239 bytes)
✓ Chunk 1730/4437 written (239 bytes)
✓ Chunk 1731/4437 written (239 bytes)
✓ Chunk 1732/4437 written (239 bytes)
✓ Chunk 1733/4437 written (239 bytes)
✓ Chunk 1734/4437 written (239 bytes)
✓ Chunk 1735/4437 written (239 bytes)
✓ Chunk 1736/4437 written (239 bytes)
✓ Chunk 1737/4437 written (239 bytes)
✓ Chunk 1738/4437 written (239 bytes)
✓ Chunk 1739/4437 written (239 bytes)
✓ Chunk 1740/4437 written (239 bytes)
✓ Chunk 1741/4437 written (239 bytes)
✓ Chunk 1742/4437 written (239 bytes)
✓ Chunk 1743/4437 written (239 bytes)
✓ Chunk 1744/4437 written (239 bytes)
✓ Chunk 1745/4437 written (239 bytes)
✓ Chunk 1746/4437 written (239 bytes)
✓ Chunk 1747/4437 written (239 bytes)
✓ Chunk 1748/4437 written (239 bytes)
✓ Chunk 1749/4437 written (239 bytes)
✓ Chunk 1750/4437 written (239 bytes)
✓ Chunk 1751/4437 written (239 bytes)
✓ Chunk 1752/4437 written (239 bytes)
✓ Chunk 1753/4437 written (239 bytes)
✓ Chunk 1754/4437 written (239 bytes)
✓ Chunk 1755/4437 written (239 bytes)
✓ Chunk 1756/4437 written (239 bytes)
✓ Chunk 1757/4437 written (239 bytes)
✓ Chunk 1758/4437 written (239 bytes)
✓ Chunk 1759/4437 written (239 bytes)
✓ Chunk 1760/4437 written (239 bytes)
✓ Chunk 1761/4437 written (239 bytes)
✓ Chunk 1762/4437 written (239 bytes)
✓ Chunk 1763/4437 written (239 bytes)
✓ Chunk 1764/4437 written (239 bytes)
✓ Chunk 1765/4437 written (239 bytes)
✓ Chunk 1766/4437 written (239 bytes)
✓ Chunk 1767/4437 written (239 bytes)
✓ Chunk 1768/4437 written (239 bytes)
✓ Chunk 1769/4437 written (239 bytes)
✓ Chunk 1770/4437 written (239 bytes)
✓ Chunk 1771/4437 written (239 bytes)
✓ Chunk 1772/4437 written (239 bytes)
✓ Chunk 1773/4437 written (239 bytes)
✓ Chunk 1774/4437 written (239 bytes)
✓ Chunk 1775/4437 written (239 bytes)
✓ Chunk 1776/4437 written (239 bytes)
✓ Chunk 1777/4437 written (239 bytes)
✓ Chunk 1778/4437 written (239 bytes)
✓ Chunk 1779/4437 written (239 bytes)
✓ Chunk 1780/4437 written (239 bytes)
✓ Chunk 1781/4437 written (239 bytes)
✓ Chunk 1782/4437 written (239 bytes)
✓ Chunk 1783/4437 written (239 bytes)
✓ Chunk 1784/4437 written (239 bytes)
✓ Chunk 1785/4437 written (239 bytes)
✓ Chunk 1786/4437 written (239 bytes)
✓ Chunk 1787/4437 written (239 bytes)
✓ Chunk 1788/4437 written (239 bytes)
✓ Chunk 1789/4437 written (239 bytes)
✓ Chunk 1790/4437 written (239 bytes)
✓ Chunk 1791/4437 written (239 bytes)
✓ Chunk 1792/4437 written (239 bytes)
✓ Chunk 1793/4437 written (239 bytes)
✓ Chunk 1794/4437 written (239 bytes)
✓ Chunk 1795/4437 written (239 bytes)
✓ Chunk 1796/4437 written (239 bytes)
✓ Chunk 1797/4437 written (239 bytes)
✓ Chunk 1798/4437 written (239 bytes)
✓ Chunk 1799/4437 written (239 bytes)
✓ Chunk 1800/4437 written (239 bytes)
  [1800/4437] Chunk streamed (seq=1800)
✓ Chunk 1801/4437 written (239 bytes)
✓ Chunk 1802/4437 written (239 bytes)
✓ Chunk 1803/4437 written (239 bytes)
✓ Chunk 1804/4437 written (239 bytes)
✓ Chunk 1805/4437 written (239 bytes)
✓ Chunk 1806/4437 written (239 bytes)
✓ Chunk 1807/4437 written (239 bytes)
✓ Chunk 1808/4437 written (239 bytes)
✓ Chunk 1809/4437 written (239 bytes)
✓ Chunk 1810/4437 written (239 bytes)
✓ Chunk 1811/4437 written (239 bytes)
✓ Chunk 1812/4437 written (239 bytes)
✓ Chunk 1813/4437 written (239 bytes)
✓ Chunk 1814/4437 written (239 bytes)
✓ Chunk 1815/4437 written (239 bytes)
✓ Chunk 1816/4437 written (239 bytes)
✓ Chunk 1817/4437 written (239 bytes)
✓ Chunk 1818/4437 written (239 bytes)
✓ Chunk 1819/4437 written (239 bytes)
✓ Chunk 1820/4437 written (239 bytes)
✓ Chunk 1821/4437 written (239 bytes)
✓ Chunk 1822/4437 written (239 bytes)
✓ Chunk 1823/4437 written (239 bytes)
✓ Chunk 1824/4437 written (239 bytes)
✓ Chunk 1825/4437 written (239 bytes)
✓ Chunk 1826/4437 written (239 bytes)
✓ Chunk 1827/4437 written (239 bytes)
✓ Chunk 1828/4437 written (239 bytes)
✓ Chunk 1829/4437 written (239 bytes)
✓ Chunk 1830/4437 written (239 bytes)
✓ Chunk 1831/4437 written (239 bytes)
✓ Chunk 1832/4437 written (239 bytes)
✓ Chunk 1833/4437 written (239 bytes)
✓ Chunk 1834/4437 written (239 bytes)
✓ Chunk 1835/4437 written (239 bytes)
✓ Chunk 1836/4437 written (239 bytes)
✓ Chunk 1837/4437 written (239 bytes)
✓ Chunk 1838/4437 written (239 bytes)
✓ Chunk 1839/4437 written (239 bytes)
✓ Chunk 1840/4437 written (239 bytes)
✓ Chunk 1841/4437 written (239 bytes)
✓ Chunk 1842/4437 written (239 bytes)
✓ Chunk 1843/4437 written (239 bytes)
✓ Chunk 1844/4437 written (239 bytes)
✓ Chunk 1845/4437 written (239 bytes)
✓ Chunk 1846/4437 written (239 bytes)
✓ Chunk 1847/4437 written (239 bytes)
✓ Chunk 1848/4437 written (239 bytes)
✓ Chunk 1849/4437 written (239 bytes)
✓ Chunk 1850/4437 written (239 bytes)
✓ Chunk 1851/4437 written (239 bytes)
✓ Chunk 1852/4437 written (239 bytes)
✓ Chunk 1853/4437 written (239 bytes)
✓ Chunk 1854/4437 written (239 bytes)
✓ Chunk 1855/4437 written (239 bytes)
✓ Chunk 1856/4437 written (239 bytes)
✓ Chunk 1857/4437 written (239 bytes)
✓ Chunk 1858/4437 written (239 bytes)
✓ Chunk 1859/4437 written (239 bytes)
✓ Chunk 1860/4437 written (239 bytes)
✓ Chunk 1861/4437 written (239 bytes)
✓ Chunk 1862/4437 written (239 bytes)
✓ Chunk 1863/4437 written (239 bytes)
✓ Chunk 1864/4437 written (239 bytes)
✓ Chunk 1865/4437 written (239 bytes)
✓ Chunk 1866/4437 written (239 bytes)
✓ Chunk 1867/4437 written (239 bytes)
✓ Chunk 1868/4437 written (239 bytes)
✓ Chunk 1869/4437 written (239 bytes)
✓ Chunk 1870/4437 written (239 bytes)
✓ Chunk 1871/4437 written (239 bytes)
✓ Chunk 1872/4437 written (239 bytes)
✓ Chunk 1873/4437 written (239 bytes)
✓ Chunk 1874/4437 written (239 bytes)
✓ Chunk 1875/4437 written (239 bytes)
✓ Chunk 1876/4437 written (239 bytes)
✓ Chunk 1877/4437 written (239 bytes)
✓ Chunk 1878/4437 written (239 bytes)
✓ Chunk 1879/4437 written (239 bytes)
✓ Chunk 1880/4437 written (239 bytes)
✓ Chunk 1881/4437 written (239 bytes)
✓ Chunk 1882/4437 written (239 bytes)
✓ Chunk 1883/4437 written (239 bytes)
✓ Chunk 1884/4437 written (239 bytes)
✓ Chunk 1885/4437 written (239 bytes)
✓ Chunk 1886/4437 written (239 bytes)
✓ Chunk 1887/4437 written (239 bytes)
✓ Chunk 1888/4437 written (239 bytes)
✓ Chunk 1889/4437 written (239 bytes)
✓ Chunk 1890/4437 written (239 bytes)
✓ Chunk 1891/4437 written (239 bytes)
✓ Chunk 1892/4437 written (239 bytes)
✓ Chunk 1893/4437 written (239 bytes)
✓ Chunk 1894/4437 written (239 bytes)
✓ Chunk 1895/4437 written (239 bytes)
✓ Chunk 1896/4437 written (239 bytes)
✓ Chunk 1897/4437 written (239 bytes)
✓ Chunk 1898/4437 written (239 bytes)
✓ Chunk 1899/4437 written (239 bytes)
✓ Chunk 1900/4437 written (239 bytes)
  [1900/4437] Chunk streamed (seq=1900)
✓ Chunk 1901/4437 written (239 bytes)
✓ Chunk 1902/4437 written (239 bytes)
✓ Chunk 1903/4437 written (239 bytes)
✓ Chunk 1904/4437 written (239 bytes)
✓ Chunk 1905/4437 written (239 bytes)
✓ Chunk 1906/4437 written (239 bytes)
✓ Chunk 1907/4437 written (239 bytes)
✓ Chunk 1908/4437 written (239 bytes)
✓ Chunk 1909/4437 written (239 bytes)
✓ Chunk 1910/4437 written (239 bytes)
✓ Chunk 1911/4437 written (239 bytes)
✓ Chunk 1912/4437 written (239 bytes)
✓ Chunk 1913/4437 written (239 bytes)
✓ Chunk 1914/4437 written (239 bytes)
✓ Chunk 1915/4437 written (239 bytes)
✓ Chunk 1916/4437 written (239 bytes)
✓ Chunk 1917/4437 written (239 bytes)
✓ Chunk 1918/4437 written (239 bytes)
✓ Chunk 1919/4437 written (239 bytes)
✓ Chunk 1920/4437 written (239 bytes)
✓ Chunk 1921/4437 written (239 bytes)
✓ Chunk 1922/4437 written (239 bytes)
✓ Chunk 1923/4437 written (239 bytes)
✓ Chunk 1924/4437 written (239 bytes)
✓ Chunk 1925/4437 written (239 bytes)
✓ Chunk 1926/4437 written (239 bytes)
✓ Chunk 1927/4437 written (239 bytes)
✓ Chunk 1928/4437 written (239 bytes)
✓ Chunk 1929/4437 written (239 bytes)
✓ Chunk 1930/4437 written (239 bytes)
✓ Chunk 1931/4437 written (239 bytes)
✓ Chunk 1932/4437 written (239 bytes)
✓ Chunk 1933/4437 written (239 bytes)
✓ Chunk 1934/4437 written (239 bytes)
✓ Chunk 1935/4437 written (239 bytes)
✓ Chunk 1936/4437 written (239 bytes)
✓ Chunk 1937/4437 written (239 bytes)
✓ Chunk 1938/4437 written (239 bytes)
✓ Chunk 1939/4437 written (239 bytes)
✓ Chunk 1940/4437 written (239 bytes)
✓ Chunk 1941/4437 written (239 bytes)
✓ Chunk 1942/4437 written (239 bytes)
✓ Chunk 1943/4437 written (239 bytes)
✓ Chunk 1944/4437 written (239 bytes)
✓ Chunk 1945/4437 written (239 bytes)
✓ Chunk 1946/4437 written (239 bytes)
✓ Chunk 1947/4437 written (239 bytes)
✓ Chunk 1948/4437 written (239 bytes)
✓ Chunk 1949/4437 written (239 bytes)
✓ Chunk 1950/4437 written (239 bytes)
✓ Chunk 1951/4437 written (239 bytes)
✓ Chunk 1952/4437 written (239 bytes)
✓ Chunk 1953/4437 written (239 bytes)
✓ Chunk 1954/4437 written (239 bytes)
✓ Chunk 1955/4437 written (239 bytes)
✓ Chunk 1956/4437 written (239 bytes)
✓ Chunk 1957/4437 written (239 bytes)
✓ Chunk 1958/4437 written (239 bytes)
✓ Chunk 1959/4437 written (239 bytes)
✓ Chunk 1960/4437 written (239 bytes)
✓ Chunk 1961/4437 written (239 bytes)
✓ Chunk 1962/4437 written (239 bytes)
✓ Chunk 1963/4437 written (239 bytes)
✓ Chunk 1964/4437 written (239 bytes)
✓ Chunk 1965/4437 written (239 bytes)
✓ Chunk 1966/4437 written (239 bytes)
✓ Chunk 1967/4437 written (239 bytes)
✓ Chunk 1968/4437 written (239 bytes)
✓ Chunk 1969/4437 written (239 bytes)
✓ Chunk 1970/4437 written (239 bytes)
✓ Chunk 1971/4437 written (239 bytes)
✓ Chunk 1972/4437 written (239 bytes)
✓ Chunk 1973/4437 written (239 bytes)
✓ Chunk 1974/4437 written (239 bytes)
✓ Chunk 1975/4437 written (239 bytes)
✓ Chunk 1976/4437 written (239 bytes)
✓ Chunk 1977/4437 written (239 bytes)
✓ Chunk 1978/4437 written (239 bytes)
✓ Chunk 1979/4437 written (239 bytes)
✓ Chunk 1980/4437 written (239 bytes)
✓ Chunk 1981/4437 written (239 bytes)
✓ Chunk 1982/4437 written (239 bytes)
✓ Chunk 1983/4437 written (239 bytes)
✓ Chunk 1984/4437 written (239 bytes)
✓ Chunk 1985/4437 written (239 bytes)
✓ Chunk 1986/4437 written (239 bytes)
✓ Chunk 1987/4437 written (239 bytes)
✓ Chunk 1988/4437 written (239 bytes)
✓ Chunk 1989/4437 written (239 bytes)
✓ Chunk 1990/4437 written (239 bytes)
✓ Chunk 1991/4437 written (239 bytes)
✓ Chunk 1992/4437 written (239 bytes)
✓ Chunk 1993/4437 written (239 bytes)
✓ Chunk 1994/4437 written (239 bytes)
✓ Chunk 1995/4437 written (239 bytes)
✓ Chunk 1996/4437 written (239 bytes)
✓ Chunk 1997/4437 written (239 bytes)
✓ Chunk 1998/4437 written (239 bytes)
✓ Chunk 1999/4437 written (239 bytes)
✓ Chunk 2000/4437 written (239 bytes)
  [2000/4437] Chunk streamed (seq=2000)
✓ Chunk 2001/4437 written (239 bytes)
✓ Chunk 2002/4437 written (239 bytes)
✓ Chunk 2003/4437 written (239 bytes)
✓ Chunk 2004/4437 written (239 bytes)
✓ Chunk 2005/4437 written (239 bytes)
✓ Chunk 2006/4437 written (239 bytes)
✓ Chunk 2007/4437 written (239 bytes)
✓ Chunk 2008/4437 written (239 bytes)
✓ Chunk 2009/4437 written (239 bytes)
✓ Chunk 2010/4437 written (239 bytes)
✓ Chunk 2011/4437 written (239 bytes)
✓ Chunk 2012/4437 written (239 bytes)
✓ Chunk 2013/4437 written (239 bytes)
✓ Chunk 2014/4437 written (239 bytes)
✓ Chunk 2015/4437 written (239 bytes)
✓ Chunk 2016/4437 written (239 bytes)
✓ Chunk 2017/4437 written (239 bytes)
✓ Chunk 2018/4437 written (239 bytes)
✓ Chunk 2019/4437 written (239 bytes)
✓ Chunk 2020/4437 written (239 bytes)
✓ Chunk 2021/4437 written (239 bytes)
✓ Chunk 2022/4437 written (239 bytes)
✓ Chunk 2023/4437 written (239 bytes)
✓ Chunk 2024/4437 written (239 bytes)
✓ Chunk 2025/4437 written (239 bytes)
✓ Chunk 2026/4437 written (239 bytes)
✓ Chunk 2027/4437 written (239 bytes)
✓ Chunk 2028/4437 written (239 bytes)
✓ Chunk 2029/4437 written (239 bytes)
✓ Chunk 2030/4437 written (239 bytes)
✓ Chunk 2031/4437 written (239 bytes)
✓ Chunk 2032/4437 written (239 bytes)
✓ Chunk 2033/4437 written (239 bytes)
✓ Chunk 2034/4437 written (239 bytes)
✓ Chunk 2035/4437 written (239 bytes)
✓ Chunk 2036/4437 written (239 bytes)
✓ Chunk 2037/4437 written (239 bytes)
✓ Chunk 2038/4437 written (239 bytes)
✓ Chunk 2039/4437 written (239 bytes)
✓ Chunk 2040/4437 written (239 bytes)
✓ Chunk 2041/4437 written (239 bytes)
✓ Chunk 2042/4437 written (239 bytes)
✓ Chunk 2043/4437 written (239 bytes)
✓ Chunk 2044/4437 written (239 bytes)
✓ Chunk 2045/4437 written (239 bytes)
✓ Chunk 2046/4437 written (239 bytes)
✓ Chunk 2047/4437 written (239 bytes)
✓ Chunk 2048/4437 written (239 bytes)
✓ Chunk 2049/4437 written (239 bytes)
✓ Chunk 2050/4437 written (239 bytes)
✓ Chunk 2051/4437 written (239 bytes)
✓ Chunk 2052/4437 written (239 bytes)
✓ Chunk 2053/4437 written (239 bytes)
✓ Chunk 2054/4437 written (239 bytes)
✓ Chunk 2055/4437 written (239 bytes)
✓ Chunk 2056/4437 written (239 bytes)
✓ Chunk 2057/4437 written (239 bytes)
✓ Chunk 2058/4437 written (239 bytes)
✓ Chunk 2059/4437 written (239 bytes)
✓ Chunk 2060/4437 written (239 bytes)
✓ Chunk 2061/4437 written (239 bytes)
✓ Chunk 2062/4437 written (239 bytes)
✓ Chunk 2063/4437 written (239 bytes)
✓ Chunk 2064/4437 written (239 bytes)
✓ Chunk 2065/4437 written (239 bytes)
✓ Chunk 2066/4437 written (239 bytes)
✓ Chunk 2067/4437 written (239 bytes)
✓ Chunk 2068/4437 written (239 bytes)
✓ Chunk 2069/4437 written (239 bytes)
✓ Chunk 2070/4437 written (239 bytes)
✓ Chunk 2071/4437 written (239 bytes)
✓ Chunk 2072/4437 written (239 bytes)
✓ Chunk 2073/4437 written (239 bytes)
✓ Chunk 2074/4437 written (239 bytes)
✓ Chunk 2075/4437 written (239 bytes)
✓ Chunk 2076/4437 written (239 bytes)
✓ Chunk 2077/4437 written (239 bytes)
✓ Chunk 2078/4437 written (239 bytes)
✓ Chunk 2079/4437 written (239 bytes)
✓ Chunk 2080/4437 written (239 bytes)
✓ Chunk 2081/4437 written (239 bytes)
✓ Chunk 2082/4437 written (239 bytes)
✓ Chunk 2083/4437 written (239 bytes)
✓ Chunk 2084/4437 written (239 bytes)
✓ Chunk 2085/4437 written (239 bytes)
✓ Chunk 2086/4437 written (239 bytes)
✓ Chunk 2087/4437 written (239 bytes)
✓ Chunk 2088/4437 written (239 bytes)
✓ Chunk 2089/4437 written (239 bytes)
✓ Chunk 2090/4437 written (239 bytes)
✓ Chunk 2091/4437 written (239 bytes)
✓ Chunk 2092/4437 written (239 bytes)
✓ Chunk 2093/4437 written (239 bytes)
✓ Chunk 2094/4437 written (239 bytes)
✓ Chunk 2095/4437 written (239 bytes)
✓ Chunk 2096/4437 written (239 bytes)
✓ Chunk 2097/4437 written (239 bytes)
✓ Chunk 2098/4437 written (239 bytes)
✓ Chunk 2099/4437 written (239 bytes)
✓ Chunk 2100/4437 written (239 bytes)
  [2100/4437] Chunk streamed (seq=2100)
✓ Chunk 2101/4437 written (239 bytes)
✓ Chunk 2102/4437 written (239 bytes)
✓ Chunk 2103/4437 written (239 bytes)
✓ Chunk 2104/4437 written (239 bytes)
✓ Chunk 2105/4437 written (239 bytes)
✓ Chunk 2106/4437 written (239 bytes)
✓ Chunk 2107/4437 written (239 bytes)
✓ Chunk 2108/4437 written (239 bytes)
✓ Chunk 2109/4437 written (239 bytes)
✓ Chunk 2110/4437 written (239 bytes)
✓ Chunk 2111/4437 written (239 bytes)
✓ Chunk 2112/4437 written (239 bytes)
✓ Chunk 2113/4437 written (239 bytes)
✓ Chunk 2114/4437 written (239 bytes)
✓ Chunk 2115/4437 written (239 bytes)
✓ Chunk 2116/4437 written (239 bytes)
✓ Chunk 2117/4437 written (239 bytes)
✓ Chunk 2118/4437 written (239 bytes)
✓ Chunk 2119/4437 written (239 bytes)
✓ Chunk 2120/4437 written (239 bytes)
✓ Chunk 2121/4437 written (239 bytes)
✓ Chunk 2122/4437 written (239 bytes)
✓ Chunk 2123/4437 written (239 bytes)
✓ Chunk 2124/4437 written (239 bytes)
✓ Chunk 2125/4437 written (239 bytes)
✓ Chunk 2126/4437 written (239 bytes)
✓ Chunk 2127/4437 written (239 bytes)
✓ Chunk 2128/4437 written (239 bytes)
✓ Chunk 2129/4437 written (239 bytes)
✓ Chunk 2130/4437 written (239 bytes)
✓ Chunk 2131/4437 written (239 bytes)
✓ Chunk 2132/4437 written (239 bytes)
✓ Chunk 2133/4437 written (239 bytes)
✓ Chunk 2134/4437 written (239 bytes)
✓ Chunk 2135/4437 written (239 bytes)
✓ Chunk 2136/4437 written (239 bytes)
✓ Chunk 2137/4437 written (239 bytes)
✓ Chunk 2138/4437 written (239 bytes)
✓ Chunk 2139/4437 written (239 bytes)
✓ Chunk 2140/4437 written (239 bytes)
✓ Chunk 2141/4437 written (239 bytes)
✓ Chunk 2142/4437 written (239 bytes)
✓ Chunk 2143/4437 written (239 bytes)
✓ Chunk 2144/4437 written (239 bytes)
✓ Chunk 2145/4437 written (239 bytes)
✓ Chunk 2146/4437 written (239 bytes)
✓ Chunk 2147/4437 written (239 bytes)
✓ Chunk 2148/4437 written (239 bytes)
✓ Chunk 2149/4437 written (239 bytes)
✓ Chunk 2150/4437 written (239 bytes)
✓ Chunk 2151/4437 written (239 bytes)
✓ Chunk 2152/4437 written (239 bytes)
✓ Chunk 2153/4437 written (239 bytes)
✓ Chunk 2154/4437 written (239 bytes)
✓ Chunk 2155/4437 written (239 bytes)
✓ Chunk 2156/4437 written (239 bytes)
✓ Chunk 2157/4437 written (239 bytes)
✓ Chunk 2158/4437 written (239 bytes)
✓ Chunk 2159/4437 written (239 bytes)
✓ Chunk 2160/4437 written (239 bytes)
✓ Chunk 2161/4437 written (239 bytes)
✓ Chunk 2162/4437 written (239 bytes)
✓ Chunk 2163/4437 written (239 bytes)
✓ Chunk 2164/4437 written (239 bytes)
✓ Chunk 2165/4437 written (239 bytes)
✓ Chunk 2166/4437 written (239 bytes)
✓ Chunk 2167/4437 written (239 bytes)
✓ Chunk 2168/4437 written (239 bytes)
✓ Chunk 2169/4437 written (239 bytes)
✓ Chunk 2170/4437 written (239 bytes)
✓ Chunk 2171/4437 written (239 bytes)
✓ Chunk 2172/4437 written (239 bytes)
✓ Chunk 2173/4437 written (239 bytes)
✓ Chunk 2174/4437 written (239 bytes)
✓ Chunk 2175/4437 written (239 bytes)
✓ Chunk 2176/4437 written (239 bytes)
✓ Chunk 2177/4437 written (239 bytes)
✓ Chunk 2178/4437 written (239 bytes)
✓ Chunk 2179/4437 written (239 bytes)
✓ Chunk 2180/4437 written (239 bytes)
✓ Chunk 2181/4437 written (239 bytes)
✓ Chunk 2182/4437 written (239 bytes)
✓ Chunk 2183/4437 written (239 bytes)
✓ Chunk 2184/4437 written (239 bytes)
✓ Chunk 2185/4437 written (239 bytes)
✓ Chunk 2186/4437 written (239 bytes)
✓ Chunk 2187/4437 written (239 bytes)
✓ Chunk 2188/4437 written (239 bytes)
✓ Chunk 2189/4437 written (239 bytes)
✓ Chunk 2190/4437 written (239 bytes)
✓ Chunk 2191/4437 written (239 bytes)
✓ Chunk 2192/4437 written (239 bytes)
✓ Chunk 2193/4437 written (239 bytes)
✓ Chunk 2194/4437 written (239 bytes)
✓ Chunk 2195/4437 written (239 bytes)
✓ Chunk 2196/4437 written (239 bytes)
✓ Chunk 2197/4437 written (239 bytes)
✓ Chunk 2198/4437 written (239 bytes)
✓ Chunk 2199/4437 written (239 bytes)
✓ Chunk 2200/4437 written (239 bytes)
  [2200/4437] Chunk streamed (seq=2200)
✓ Chunk 2201/4437 written (239 bytes)
✓ Chunk 2202/4437 written (239 bytes)
✓ Chunk 2203/4437 written (239 bytes)
✓ Chunk 2204/4437 written (239 bytes)
✓ Chunk 2205/4437 written (239 bytes)
✓ Chunk 2206/4437 written (239 bytes)
✓ Chunk 2207/4437 written (239 bytes)
✓ Chunk 2208/4437 written (239 bytes)
✓ Chunk 2209/4437 written (239 bytes)
✓ Chunk 2210/4437 written (239 bytes)
✓ Chunk 2211/4437 written (239 bytes)
✓ Chunk 2212/4437 written (239 bytes)
✓ Chunk 2213/4437 written (239 bytes)
✓ Chunk 2214/4437 written (239 bytes)
✓ Chunk 2215/4437 written (239 bytes)
✓ Chunk 2216/4437 written (239 bytes)
✓ Chunk 2217/4437 written (239 bytes)
✓ Chunk 2218/4437 written (239 bytes)
✓ Chunk 2219/4437 written (239 bytes)
✓ Chunk 2220/4437 written (239 bytes)
✓ Chunk 2221/4437 written (239 bytes)
✓ Chunk 2222/4437 written (239 bytes)
✓ Chunk 2223/4437 written (239 bytes)
✓ Chunk 2224/4437 written (239 bytes)
✓ Chunk 2225/4437 written (239 bytes)
✓ Chunk 2226/4437 written (239 bytes)
✓ Chunk 2227/4437 written (239 bytes)
✓ Chunk 2228/4437 written (239 bytes)
✓ Chunk 2229/4437 written (239 bytes)
✓ Chunk 2230/4437 written (239 bytes)
✓ Chunk 2231/4437 written (239 bytes)
✓ Chunk 2232/4437 written (239 bytes)
✓ Chunk 2233/4437 written (239 bytes)
✓ Chunk 2234/4437 written (239 bytes)
✓ Chunk 2235/4437 written (239 bytes)
✓ Chunk 2236/4437 written (239 bytes)
✓ Chunk 2237/4437 written (239 bytes)
✓ Chunk 2238/4437 written (239 bytes)
✓ Chunk 2239/4437 written (239 bytes)
✓ Chunk 2240/4437 written (239 bytes)
✓ Chunk 2241/4437 written (239 bytes)
✓ Chunk 2242/4437 written (239 bytes)
✓ Chunk 2243/4437 written (239 bytes)
✓ Chunk 2244/4437 written (239 bytes)
✓ Chunk 2245/4437 written (239 bytes)
✓ Chunk 2246/4437 written (239 bytes)
✓ Chunk 2247/4437 written (239 bytes)
✓ Chunk 2248/4437 written (239 bytes)
✓ Chunk 2249/4437 written (239 bytes)
✓ Chunk 2250/4437 written (239 bytes)
✓ Chunk 2251/4437 written (239 bytes)
✓ Chunk 2252/4437 written (239 bytes)
✓ Chunk 2253/4437 written (239 bytes)
✓ Chunk 2254/4437 written (239 bytes)
✓ Chunk 2255/4437 written (239 bytes)
✓ Chunk 2256/4437 written (239 bytes)
✓ Chunk 2257/4437 written (239 bytes)
✓ Chunk 2258/4437 written (239 bytes)
✓ Chunk 2259/4437 written (239 bytes)
✓ Chunk 2260/4437 written (239 bytes)
✓ Chunk 2261/4437 written (239 bytes)
✓ Chunk 2262/4437 written (239 bytes)
✓ Chunk 2263/4437 written (239 bytes)
✓ Chunk 2264/4437 written (239 bytes)
✓ Chunk 2265/4437 written (239 bytes)
✓ Chunk 2266/4437 written (239 bytes)
✓ Chunk 2267/4437 written (239 bytes)
✓ Chunk 2268/4437 written (239 bytes)
✓ Chunk 2269/4437 written (239 bytes)
✓ Chunk 2270/4437 written (239 bytes)
✓ Chunk 2271/4437 written (239 bytes)
✓ Chunk 2272/4437 written (239 bytes)
✓ Chunk 2273/4437 written (239 bytes)
✓ Chunk 2274/4437 written (239 bytes)
✓ Chunk 2275/4437 written (239 bytes)
✓ Chunk 2276/4437 written (239 bytes)
✓ Chunk 2277/4437 written (239 bytes)
✓ Chunk 2278/4437 written (239 bytes)
✓ Chunk 2279/4437 written (239 bytes)
✓ Chunk 2280/4437 written (239 bytes)
✓ Chunk 2281/4437 written (239 bytes)
✓ Chunk 2282/4437 written (239 bytes)
✓ Chunk 2283/4437 written (239 bytes)
✓ Chunk 2284/4437 written (239 bytes)
✓ Chunk 2285/4437 written (239 bytes)
✓ Chunk 2286/4437 written (239 bytes)
✓ Chunk 2287/4437 written (239 bytes)
✓ Chunk 2288/4437 written (239 bytes)
✓ Chunk 2289/4437 written (239 bytes)
✓ Chunk 2290/4437 written (239 bytes)
✓ Chunk 2291/4437 written (239 bytes)
✓ Chunk 2292/4437 written (239 bytes)
✓ Chunk 2293/4437 written (239 bytes)
✓ Chunk 2294/4437 written (239 bytes)
✓ Chunk 2295/4437 written (239 bytes)
✓ Chunk 2296/4437 written (239 bytes)
✓ Chunk 2297/4437 written (239 bytes)
✓ Chunk 2298/4437 written (239 bytes)
✓ Chunk 2299/4437 written (239 bytes)
✓ Chunk 2300/4437 written (239 bytes)
  [2300/4437] Chunk streamed (seq=2300)
✓ Chunk 2301/4437 written (239 bytes)
✓ Chunk 2302/4437 written (239 bytes)
✓ Chunk 2303/4437 written (239 bytes)
✓ Chunk 2304/4437 written (239 bytes)
✓ Chunk 2305/4437 written (239 bytes)
✓ Chunk 2306/4437 written (239 bytes)
✓ Chunk 2307/4437 written (239 bytes)
✓ Chunk 2308/4437 written (239 bytes)
✓ Chunk 2309/4437 written (239 bytes)
✓ Chunk 2310/4437 written (239 bytes)
✓ Chunk 2311/4437 written (239 bytes)
✓ Chunk 2312/4437 written (239 bytes)
✓ Chunk 2313/4437 written (239 bytes)
✓ Chunk 2314/4437 written (239 bytes)
✓ Chunk 2315/4437 written (239 bytes)
✓ Chunk 2316/4437 written (239 bytes)
✓ Chunk 2317/4437 written (239 bytes)
✓ Chunk 2318/4437 written (239 bytes)
✓ Chunk 2319/4437 written (239 bytes)
✓ Chunk 2320/4437 written (239 bytes)
✓ Chunk 2321/4437 written (239 bytes)
✓ Chunk 2322/4437 written (239 bytes)
✓ Chunk 2323/4437 written (239 bytes)
✓ Chunk 2324/4437 written (239 bytes)
✓ Chunk 2325/4437 written (239 bytes)
✓ Chunk 2326/4437 written (239 bytes)
✓ Chunk 2327/4437 written (239 bytes)
✓ Chunk 2328/4437 written (239 bytes)
✓ Chunk 2329/4437 written (239 bytes)
✓ Chunk 2330/4437 written (239 bytes)
✓ Chunk 2331/4437 written (239 bytes)
✓ Chunk 2332/4437 written (239 bytes)
✓ Chunk 2333/4437 written (239 bytes)
✓ Chunk 2334/4437 written (239 bytes)
✓ Chunk 2335/4437 written (239 bytes)
✓ Chunk 2336/4437 written (239 bytes)
✓ Chunk 2337/4437 written (239 bytes)
✓ Chunk 2338/4437 written (239 bytes)
✓ Chunk 2339/4437 written (239 bytes)
✓ Chunk 2340/4437 written (239 bytes)
✓ Chunk 2341/4437 written (239 bytes)
✓ Chunk 2342/4437 written (239 bytes)
✓ Chunk 2343/4437 written (239 bytes)
✓ Chunk 2344/4437 written (239 bytes)
✓ Chunk 2345/4437 written (239 bytes)
✓ Chunk 2346/4437 written (239 bytes)
✓ Chunk 2347/4437 written (239 bytes)
✓ Chunk 2348/4437 written (239 bytes)
✓ Chunk 2349/4437 written (239 bytes)
✓ Chunk 2350/4437 written (239 bytes)
✓ Chunk 2351/4437 written (239 bytes)
✓ Chunk 2352/4437 written (239 bytes)
✓ Chunk 2353/4437 written (239 bytes)
✓ Chunk 2354/4437 written (239 bytes)
✓ Chunk 2355/4437 written (239 bytes)
✓ Chunk 2356/4437 written (239 bytes)
✓ Chunk 2357/4437 written (239 bytes)
✓ Chunk 2358/4437 written (239 bytes)
✓ Chunk 2359/4437 written (239 bytes)
✓ Chunk 2360/4437 written (239 bytes)
✓ Chunk 2361/4437 written (239 bytes)
✓ Chunk 2362/4437 written (239 bytes)
✓ Chunk 2363/4437 written (239 bytes)
✓ Chunk 2364/4437 written (239 bytes)
✓ Chunk 2365/4437 written (239 bytes)
✓ Chunk 2366/4437 written (239 bytes)
✓ Chunk 2367/4437 written (239 bytes)
✓ Chunk 2368/4437 written (239 bytes)
✓ Chunk 2369/4437 written (239 bytes)
✓ Chunk 2370/4437 written (239 bytes)
✓ Chunk 2371/4437 written (239 bytes)
✓ Chunk 2372/4437 written (239 bytes)
✓ Chunk 2373/4437 written (239 bytes)
✓ Chunk 2374/4437 written (239 bytes)
✓ Chunk 2375/4437 written (239 bytes)
✓ Chunk 2376/4437 written (239 bytes)
✓ Chunk 2377/4437 written (239 bytes)
✓ Chunk 2378/4437 written (239 bytes)
✓ Chunk 2379/4437 written (239 bytes)
✓ Chunk 2380/4437 written (239 bytes)
✓ Chunk 2381/4437 written (239 bytes)
✓ Chunk 2382/4437 written (239 bytes)
✓ Chunk 2383/4437 written (239 bytes)
✓ Chunk 2384/4437 written (239 bytes)
✓ Chunk 2385/4437 written (239 bytes)
✓ Chunk 2386/4437 written (239 bytes)
✓ Chunk 2387/4437 written (239 bytes)
✓ Chunk 2388/4437 written (239 bytes)
✓ Chunk 2389/4437 written (239 bytes)
✓ Chunk 2390/4437 written (239 bytes)
✓ Chunk 2391/4437 written (239 bytes)
✓ Chunk 2392/4437 written (239 bytes)
✓ Chunk 2393/4437 written (239 bytes)
✓ Chunk 2394/4437 written (239 bytes)
✓ Chunk 2395/4437 written (239 bytes)
✓ Chunk 2396/4437 written (239 bytes)
✓ Chunk 2397/4437 written (239 bytes)
✓ Chunk 2398/4437 written (239 bytes)
✓ Chunk 2399/4437 written (239 bytes)
✓ Chunk 2400/4437 written (239 bytes)
  [2400/4437] Chunk streamed (seq=2400)
✓ Chunk 2401/4437 written (239 bytes)
✓ Chunk 2402/4437 written (239 bytes)
✓ Chunk 2403/4437 written (239 bytes)
✓ Chunk 2404/4437 written (239 bytes)
✓ Chunk 2405/4437 written (239 bytes)
✓ Chunk 2406/4437 written (239 bytes)
✓ Chunk 2407/4437 written (239 bytes)
✓ Chunk 2408/4437 written (239 bytes)
✓ Chunk 2409/4437 written (239 bytes)
✓ Chunk 2410/4437 written (239 bytes)
✓ Chunk 2411/4437 written (239 bytes)
✓ Chunk 2412/4437 written (239 bytes)
✓ Chunk 2413/4437 written (239 bytes)
✓ Chunk 2414/4437 written (239 bytes)
✓ Chunk 2415/4437 written (239 bytes)
✓ Chunk 2416/4437 written (239 bytes)
✓ Chunk 2417/4437 written (239 bytes)
✓ Chunk 2418/4437 written (239 bytes)
✓ Chunk 2419/4437 written (239 bytes)
✓ Chunk 2420/4437 written (239 bytes)
✓ Chunk 2421/4437 written (239 bytes)
✓ Chunk 2422/4437 written (239 bytes)
✓ Chunk 2423/4437 written (239 bytes)
✓ Chunk 2424/4437 written (239 bytes)
✓ Chunk 2425/4437 written (239 bytes)
✓ Chunk 2426/4437 written (239 bytes)
✓ Chunk 2427/4437 written (239 bytes)
✓ Chunk 2428/4437 written (239 bytes)
✓ Chunk 2429/4437 written (239 bytes)
✓ Chunk 2430/4437 written (239 bytes)
✓ Chunk 2431/4437 written (239 bytes)
✓ Chunk 2432/4437 written (239 bytes)
✓ Chunk 2433/4437 written (239 bytes)
✓ Chunk 2434/4437 written (239 bytes)
✓ Chunk 2435/4437 written (239 bytes)
✓ Chunk 2436/4437 written (239 bytes)
✓ Chunk 2437/4437 written (239 bytes)
✓ Chunk 2438/4437 written (239 bytes)
✓ Chunk 2439/4437 written (239 bytes)
✓ Chunk 2440/4437 written (239 bytes)
✓ Chunk 2441/4437 written (239 bytes)
✓ Chunk 2442/4437 written (239 bytes)
✓ Chunk 2443/4437 written (239 bytes)
✓ Chunk 2444/4437 written (239 bytes)
✓ Chunk 2445/4437 written (239 bytes)
✓ Chunk 2446/4437 written (239 bytes)
✓ Chunk 2447/4437 written (239 bytes)
✓ Chunk 2448/4437 written (239 bytes)
✓ Chunk 2449/4437 written (239 bytes)
✓ Chunk 2450/4437 written (239 bytes)
✓ Chunk 2451/4437 written (239 bytes)
✓ Chunk 2452/4437 written (239 bytes)
✓ Chunk 2453/4437 written (239 bytes)
✓ Chunk 2454/4437 written (239 bytes)
✓ Chunk 2455/4437 written (239 bytes)
✓ Chunk 2456/4437 written (239 bytes)
✓ Chunk 2457/4437 written (239 bytes)
✓ Chunk 2458/4437 written (239 bytes)
✓ Chunk 2459/4437 written (239 bytes)
✓ Chunk 2460/4437 written (239 bytes)
✓ Chunk 2461/4437 written (239 bytes)
✓ Chunk 2462/4437 written (239 bytes)
✓ Chunk 2463/4437 written (239 bytes)
✓ Chunk 2464/4437 written (239 bytes)
✓ Chunk 2465/4437 written (239 bytes)
✓ Chunk 2466/4437 written (239 bytes)
✓ Chunk 2467/4437 written (239 bytes)
✓ Chunk 2468/4437 written (239 bytes)
✓ Chunk 2469/4437 written (239 bytes)
✓ Chunk 2470/4437 written (239 bytes)
✓ Chunk 2471/4437 written (239 bytes)
✓ Chunk 2472/4437 written (239 bytes)
✓ Chunk 2473/4437 written (239 bytes)
✓ Chunk 2474/4437 written (239 bytes)
✓ Chunk 2475/4437 written (239 bytes)
✓ Chunk 2476/4437 written (239 bytes)
✓ Chunk 2477/4437 written (239 bytes)
✓ Chunk 2478/4437 written (239 bytes)
✓ Chunk 2479/4437 written (239 bytes)
✓ Chunk 2480/4437 written (239 bytes)
✓ Chunk 2481/4437 written (239 bytes)
✓ Chunk 2482/4437 written (239 bytes)
✓ Chunk 2483/4437 written (239 bytes)
✓ Chunk 2484/4437 written (239 bytes)
✓ Chunk 2485/4437 written (239 bytes)
✓ Chunk 2486/4437 written (239 bytes)
✓ Chunk 2487/4437 written (239 bytes)
✓ Chunk 2488/4437 written (239 bytes)
✓ Chunk 2489/4437 written (239 bytes)
✓ Chunk 2490/4437 written (239 bytes)
✓ Chunk 2491/4437 written (239 bytes)
✓ Chunk 2492/4437 written (239 bytes)
✓ Chunk 2493/4437 written (239 bytes)
✓ Chunk 2494/4437 written (239 bytes)
✓ Chunk 2495/4437 written (239 bytes)
✓ Chunk 2496/4437 written (239 bytes)
✓ Chunk 2497/4437 written (239 bytes)
✓ Chunk 2498/4437 written (239 bytes)
✓ Chunk 2499/4437 written (239 bytes)
✓ Chunk 2500/4437 written (239 bytes)
  [2500/4437] Chunk streamed (seq=2500)
✓ Chunk 2501/4437 written (239 bytes)
✓ Chunk 2502/4437 written (239 bytes)
✓ Chunk 2503/4437 written (239 bytes)
✓ Chunk 2504/4437 written (239 bytes)
✓ Chunk 2505/4437 written (239 bytes)
✓ Chunk 2506/4437 written (239 bytes)
✓ Chunk 2507/4437 written (239 bytes)
✓ Chunk 2508/4437 written (239 bytes)
✓ Chunk 2509/4437 written (239 bytes)
✓ Chunk 2510/4437 written (239 bytes)
✓ Chunk 2511/4437 written (239 bytes)
✓ Chunk 2512/4437 written (239 bytes)
✓ Chunk 2513/4437 written (239 bytes)
✓ Chunk 2514/4437 written (239 bytes)
✓ Chunk 2515/4437 written (239 bytes)
✓ Chunk 2516/4437 written (239 bytes)
✓ Chunk 2517/4437 written (239 bytes)
✓ Chunk 2518/4437 written (239 bytes)
✓ Chunk 2519/4437 written (239 bytes)
✓ Chunk 2520/4437 written (239 bytes)
✓ Chunk 2521/4437 written (239 bytes)
✓ Chunk 2522/4437 written (239 bytes)
✓ Chunk 2523/4437 written (239 bytes)
✓ Chunk 2524/4437 written (239 bytes)
✓ Chunk 2525/4437 written (239 bytes)
✓ Chunk 2526/4437 written (239 bytes)
✓ Chunk 2527/4437 written (239 bytes)
✓ Chunk 2528/4437 written (239 bytes)
✓ Chunk 2529/4437 written (239 bytes)
✓ Chunk 2530/4437 written (239 bytes)
✓ Chunk 2531/4437 written (239 bytes)
✓ Chunk 2532/4437 written (239 bytes)
✓ Chunk 2533/4437 written (239 bytes)
✓ Chunk 2534/4437 written (239 bytes)
✓ Chunk 2535/4437 written (239 bytes)
✓ Chunk 2536/4437 written (239 bytes)
✓ Chunk 2537/4437 written (239 bytes)
✓ Chunk 2538/4437 written (239 bytes)
✓ Chunk 2539/4437 written (239 bytes)
✓ Chunk 2540/4437 written (239 bytes)
✓ Chunk 2541/4437 written (239 bytes)
✓ Chunk 2542/4437 written (239 bytes)
✓ Chunk 2543/4437 written (239 bytes)
✓ Chunk 2544/4437 written (239 bytes)
✓ Chunk 2545/4437 written (239 bytes)
✓ Chunk 2546/4437 written (239 bytes)
✓ Chunk 2547/4437 written (239 bytes)
✓ Chunk 2548/4437 written (239 bytes)
✓ Chunk 2549/4437 written (239 bytes)
✓ Chunk 2550/4437 written (239 bytes)
✓ Chunk 2551/4437 written (239 bytes)
✓ Chunk 2552/4437 written (239 bytes)
✓ Chunk 2553/4437 written (239 bytes)
✓ Chunk 2554/4437 written (239 bytes)
✓ Chunk 2555/4437 written (239 bytes)
✓ Chunk 2556/4437 written (239 bytes)
✓ Chunk 2557/4437 written (239 bytes)
✓ Chunk 2558/4437 written (239 bytes)
✓ Chunk 2559/4437 written (239 bytes)
✓ Chunk 2560/4437 written (239 bytes)
✓ Chunk 2561/4437 written (239 bytes)
✓ Chunk 2562/4437 written (239 bytes)
✓ Chunk 2563/4437 written (239 bytes)
✓ Chunk 2564/4437 written (239 bytes)
✓ Chunk 2565/4437 written (239 bytes)
✓ Chunk 2566/4437 written (239 bytes)
✓ Chunk 2567/4437 written (239 bytes)
✓ Chunk 2568/4437 written (239 bytes)
✓ Chunk 2569/4437 written (239 bytes)
✓ Chunk 2570/4437 written (239 bytes)
✓ Chunk 2571/4437 written (239 bytes)
✓ Chunk 2572/4437 written (239 bytes)
✓ Chunk 2573/4437 written (239 bytes)
✓ Chunk 2574/4437 written (239 bytes)
✓ Chunk 2575/4437 written (239 bytes)
✓ Chunk 2576/4437 written (239 bytes)
✓ Chunk 2577/4437 written (239 bytes)
✓ Chunk 2578/4437 written (239 bytes)
✓ Chunk 2579/4437 written (239 bytes)
✓ Chunk 2580/4437 written (239 bytes)
✓ Chunk 2581/4437 written (239 bytes)
✓ Chunk 2582/4437 written (239 bytes)
✓ Chunk 2583/4437 written (239 bytes)
✓ Chunk 2584/4437 written (239 bytes)
✓ Chunk 2585/4437 written (239 bytes)
✓ Chunk 2586/4437 written (239 bytes)
✓ Chunk 2587/4437 written (239 bytes)
✓ Chunk 2588/4437 written (239 bytes)
✓ Chunk 2589/4437 written (239 bytes)
✓ Chunk 2590/4437 written (239 bytes)
✓ Chunk 2591/4437 written (239 bytes)
✓ Chunk 2592/4437 written (239 bytes)
✓ Chunk 2593/4437 written (239 bytes)
✓ Chunk 2594/4437 written (239 bytes)
✓ Chunk 2595/4437 written (239 bytes)
✓ Chunk 2596/4437 written (239 bytes)
✓ Chunk 2597/4437 written (239 bytes)
✓ Chunk 2598/4437 written (239 bytes)
✓ Chunk 2599/4437 written (239 bytes)
✓ Chunk 2600/4437 written (239 bytes)
  [2600/4437] Chunk streamed (seq=2600)
✓ Chunk 2601/4437 written (239 bytes)
✓ Chunk 2602/4437 written (239 bytes)
✓ Chunk 2603/4437 written (239 bytes)
✓ Chunk 2604/4437 written (239 bytes)
✓ Chunk 2605/4437 written (239 bytes)
✓ Chunk 2606/4437 written (239 bytes)
✓ Chunk 2607/4437 written (239 bytes)
✓ Chunk 2608/4437 written (239 bytes)
✓ Chunk 2609/4437 written (239 bytes)
✓ Chunk 2610/4437 written (239 bytes)
✓ Chunk 2611/4437 written (239 bytes)
✓ Chunk 2612/4437 written (239 bytes)
✓ Chunk 2613/4437 written (239 bytes)
✓ Chunk 2614/4437 written (239 bytes)
✓ Chunk 2615/4437 written (239 bytes)
✓ Chunk 2616/4437 written (239 bytes)
✓ Chunk 2617/4437 written (239 bytes)
✓ Chunk 2618/4437 written (239 bytes)
✓ Chunk 2619/4437 written (239 bytes)
✓ Chunk 2620/4437 written (239 bytes)
✓ Chunk 2621/4437 written (239 bytes)
✓ Chunk 2622/4437 written (239 bytes)
✓ Chunk 2623/4437 written (239 bytes)
✓ Chunk 2624/4437 written (239 bytes)
✓ Chunk 2625/4437 written (239 bytes)
✓ Chunk 2626/4437 written (239 bytes)
✓ Chunk 2627/4437 written (239 bytes)
✓ Chunk 2628/4437 written (239 bytes)
✓ Chunk 2629/4437 written (239 bytes)
✓ Chunk 2630/4437 written (239 bytes)
✓ Chunk 2631/4437 written (239 bytes)
✓ Chunk 2632/4437 written (239 bytes)
✓ Chunk 2633/4437 written (239 bytes)
✓ Chunk 2634/4437 written (239 bytes)
✓ Chunk 2635/4437 written (239 bytes)
✓ Chunk 2636/4437 written (239 bytes)
✓ Chunk 2637/4437 written (239 bytes)
✓ Chunk 2638/4437 written (239 bytes)
✓ Chunk 2639/4437 written (239 bytes)
✓ Chunk 2640/4437 written (239 bytes)
✓ Chunk 2641/4437 written (239 bytes)
✓ Chunk 2642/4437 written (239 bytes)
✓ Chunk 2643/4437 written (239 bytes)
✓ Chunk 2644/4437 written (239 bytes)
✓ Chunk 2645/4437 written (239 bytes)
✓ Chunk 2646/4437 written (239 bytes)
✓ Chunk 2647/4437 written (239 bytes)
✓ Chunk 2648/4437 written (239 bytes)
✓ Chunk 2649/4437 written (239 bytes)
✓ Chunk 2650/4437 written (239 bytes)
✓ Chunk 2651/4437 written (239 bytes)
✓ Chunk 2652/4437 written (239 bytes)
✓ Chunk 2653/4437 written (239 bytes)
✓ Chunk 2654/4437 written (239 bytes)
✓ Chunk 2655/4437 written (239 bytes)
✓ Chunk 2656/4437 written (239 bytes)
✓ Chunk 2657/4437 written (239 bytes)
✓ Chunk 2658/4437 written (239 bytes)
✓ Chunk 2659/4437 written (239 bytes)
✓ Chunk 2660/4437 written (239 bytes)
✓ Chunk 2661/4437 written (239 bytes)
✓ Chunk 2662/4437 written (239 bytes)
✓ Chunk 2663/4437 written (239 bytes)
✓ Chunk 2664/4437 written (239 bytes)
✓ Chunk 2665/4437 written (239 bytes)
✓ Chunk 2666/4437 written (239 bytes)
✓ Chunk 2667/4437 written (239 bytes)
✓ Chunk 2668/4437 written (239 bytes)
✓ Chunk 2669/4437 written (239 bytes)
✓ Chunk 2670/4437 written (239 bytes)
✓ Chunk 2671/4437 written (239 bytes)
✓ Chunk 2672/4437 written (239 bytes)
✓ Chunk 2673/4437 written (239 bytes)
✓ Chunk 2674/4437 written (239 bytes)
✓ Chunk 2675/4437 written (239 bytes)
✓ Chunk 2676/4437 written (239 bytes)
✓ Chunk 2677/4437 written (239 bytes)
✓ Chunk 2678/4437 written (239 bytes)
✓ Chunk 2679/4437 written (239 bytes)
✓ Chunk 2680/4437 written (239 bytes)
✓ Chunk 2681/4437 written (239 bytes)
✓ Chunk 2682/4437 written (239 bytes)
✓ Chunk 2683/4437 written (239 bytes)
✓ Chunk 2684/4437 written (239 bytes)
✓ Chunk 2685/4437 written (239 bytes)
✓ Chunk 2686/4437 written (239 bytes)
✓ Chunk 2687/4437 written (239 bytes)
✓ Chunk 2688/4437 written (239 bytes)
✓ Chunk 2689/4437 written (239 bytes)
✓ Chunk 2690/4437 written (239 bytes)
✓ Chunk 2691/4437 written (239 bytes)
✓ Chunk 2692/4437 written (239 bytes)
✓ Chunk 2693/4437 written (239 bytes)
✓ Chunk 2694/4437 written (239 bytes)
✓ Chunk 2695/4437 written (239 bytes)
✓ Chunk 2696/4437 written (239 bytes)
✓ Chunk 2697/4437 written (239 bytes)
✓ Chunk 2698/4437 written (239 bytes)
✓ Chunk 2699/4437 written (239 bytes)
✓ Chunk 2700/4437 written (239 bytes)
  [2700/4437] Chunk streamed (seq=2700)
✓ Chunk 2701/4437 written (239 bytes)
✓ Chunk 2702/4437 written (239 bytes)
✓ Chunk 2703/4437 written (239 bytes)
✓ Chunk 2704/4437 written (239 bytes)
✓ Chunk 2705/4437 written (239 bytes)
✓ Chunk 2706/4437 written (239 bytes)
✓ Chunk 2707/4437 written (239 bytes)
✓ Chunk 2708/4437 written (239 bytes)
✓ Chunk 2709/4437 written (239 bytes)
✓ Chunk 2710/4437 written (239 bytes)
✓ Chunk 2711/4437 written (239 bytes)
✓ Chunk 2712/4437 written (239 bytes)
✓ Chunk 2713/4437 written (239 bytes)
✓ Chunk 2714/4437 written (239 bytes)
✓ Chunk 2715/4437 written (239 bytes)
✓ Chunk 2716/4437 written (239 bytes)
✓ Chunk 2717/4437 written (239 bytes)
✓ Chunk 2718/4437 written (239 bytes)
✓ Chunk 2719/4437 written (239 bytes)
✓ Chunk 2720/4437 written (239 bytes)
✓ Chunk 2721/4437 written (239 bytes)
✓ Chunk 2722/4437 written (239 bytes)
✓ Chunk 2723/4437 written (239 bytes)
✓ Chunk 2724/4437 written (239 bytes)
✓ Chunk 2725/4437 written (239 bytes)
✓ Chunk 2726/4437 written (239 bytes)
✓ Chunk 2727/4437 written (239 bytes)
✓ Chunk 2728/4437 written (239 bytes)
✓ Chunk 2729/4437 written (239 bytes)
✓ Chunk 2730/4437 written (239 bytes)
✓ Chunk 2731/4437 written (239 bytes)
✓ Chunk 2732/4437 written (239 bytes)
✓ Chunk 2733/4437 written (239 bytes)
✓ Chunk 2734/4437 written (239 bytes)
✓ Chunk 2735/4437 written (239 bytes)
✓ Chunk 2736/4437 written (239 bytes)
✓ Chunk 2737/4437 written (239 bytes)
✓ Chunk 2738/4437 written (239 bytes)
✓ Chunk 2739/4437 written (239 bytes)
✓ Chunk 2740/4437 written (239 bytes)
✓ Chunk 2741/4437 written (239 bytes)
✓ Chunk 2742/4437 written (239 bytes)
✓ Chunk 2743/4437 written (239 bytes)
✓ Chunk 2744/4437 written (239 bytes)
✓ Chunk 2745/4437 written (239 bytes)
✓ Chunk 2746/4437 written (239 bytes)
✓ Chunk 2747/4437 written (239 bytes)
✓ Chunk 2748/4437 written (239 bytes)
✓ Chunk 2749/4437 written (239 bytes)
✓ Chunk 2750/4437 written (239 bytes)
✓ Chunk 2751/4437 written (239 bytes)
✓ Chunk 2752/4437 written (239 bytes)
✓ Chunk 2753/4437 written (239 bytes)
✓ Chunk 2754/4437 written (239 bytes)
✓ Chunk 2755/4437 written (239 bytes)
✓ Chunk 2756/4437 written (239 bytes)
✓ Chunk 2757/4437 written (239 bytes)
✓ Chunk 2758/4437 written (239 bytes)
✓ Chunk 2759/4437 written (239 bytes)
✓ Chunk 2760/4437 written (239 bytes)
✓ Chunk 2761/4437 written (239 bytes)
✓ Chunk 2762/4437 written (239 bytes)
✓ Chunk 2763/4437 written (239 bytes)
✓ Chunk 2764/4437 written (239 bytes)
✓ Chunk 2765/4437 written (239 bytes)
✓ Chunk 2766/4437 written (239 bytes)
✓ Chunk 2767/4437 written (239 bytes)
✓ Chunk 2768/4437 written (239 bytes)
✓ Chunk 2769/4437 written (239 bytes)
✓ Chunk 2770/4437 written (239 bytes)
✓ Chunk 2771/4437 written (239 bytes)
✓ Chunk 2772/4437 written (239 bytes)
✓ Chunk 2773/4437 written (239 bytes)
✓ Chunk 2774/4437 written (239 bytes)
✓ Chunk 2775/4437 written (239 bytes)
✓ Chunk 2776/4437 written (239 bytes)
✓ Chunk 2777/4437 written (239 bytes)
✓ Chunk 2778/4437 written (239 bytes)
✓ Chunk 2779/4437 written (239 bytes)
✓ Chunk 2780/4437 written (239 bytes)
✓ Chunk 2781/4437 written (239 bytes)
✓ Chunk 2782/4437 written (239 bytes)
✓ Chunk 2783/4437 written (239 bytes)
✓ Chunk 2784/4437 written (239 bytes)
✓ Chunk 2785/4437 written (239 bytes)
✓ Chunk 2786/4437 written (239 bytes)
✓ Chunk 2787/4437 written (239 bytes)
✓ Chunk 2788/4437 written (239 bytes)
✓ Chunk 2789/4437 written (239 bytes)
✓ Chunk 2790/4437 written (239 bytes)
✓ Chunk 2791/4437 written (239 bytes)
✓ Chunk 2792/4437 written (239 bytes)
✓ Chunk 2793/4437 written (239 bytes)
✓ Chunk 2794/4437 written (239 bytes)
✓ Chunk 2795/4437 written (239 bytes)
✓ Chunk 2796/4437 written (239 bytes)
✓ Chunk 2797/4437 written (239 bytes)
✓ Chunk 2798/4437 written (239 bytes)
✓ Chunk 2799/4437 written (239 bytes)
✓ Chunk 2800/4437 written (239 bytes)
  [2800/4437] Chunk streamed (seq=2800)
✓ Chunk 2801/4437 written (239 bytes)
✓ Chunk 2802/4437 written (239 bytes)
✓ Chunk 2803/4437 written (239 bytes)
✓ Chunk 2804/4437 written (239 bytes)
✓ Chunk 2805/4437 written (239 bytes)
✓ Chunk 2806/4437 written (239 bytes)
✓ Chunk 2807/4437 written (239 bytes)
✓ Chunk 2808/4437 written (239 bytes)
✓ Chunk 2809/4437 written (239 bytes)
✓ Chunk 2810/4437 written (239 bytes)
✓ Chunk 2811/4437 written (239 bytes)
✓ Chunk 2812/4437 written (239 bytes)
✓ Chunk 2813/4437 written (239 bytes)
✓ Chunk 2814/4437 written (239 bytes)
✓ Chunk 2815/4437 written (239 bytes)
✓ Chunk 2816/4437 written (239 bytes)
✓ Chunk 2817/4437 written (239 bytes)
✓ Chunk 2818/4437 written (239 bytes)
✓ Chunk 2819/4437 written (239 bytes)
✓ Chunk 2820/4437 written (239 bytes)
✓ Chunk 2821/4437 written (239 bytes)
✓ Chunk 2822/4437 written (239 bytes)
✓ Chunk 2823/4437 written (239 bytes)
✓ Chunk 2824/4437 written (239 bytes)
✓ Chunk 2825/4437 written (239 bytes)
✓ Chunk 2826/4437 written (239 bytes)
✓ Chunk 2827/4437 written (239 bytes)
✓ Chunk 2828/4437 written (239 bytes)
✓ Chunk 2829/4437 written (239 bytes)
✓ Chunk 2830/4437 written (239 bytes)
✓ Chunk 2831/4437 written (239 bytes)
✓ Chunk 2832/4437 written (239 bytes)
✓ Chunk 2833/4437 written (239 bytes)
✓ Chunk 2834/4437 written (239 bytes)
✓ Chunk 2835/4437 written (239 bytes)
✓ Chunk 2836/4437 written (239 bytes)
✓ Chunk 2837/4437 written (239 bytes)
✓ Chunk 2838/4437 written (239 bytes)
✓ Chunk 2839/4437 written (239 bytes)
✓ Chunk 2840/4437 written (239 bytes)
✓ Chunk 2841/4437 written (239 bytes)
✓ Chunk 2842/4437 written (239 bytes)
✓ Chunk 2843/4437 written (239 bytes)
✓ Chunk 2844/4437 written (239 bytes)
✓ Chunk 2845/4437 written (239 bytes)
✓ Chunk 2846/4437 written (239 bytes)
✓ Chunk 2847/4437 written (239 bytes)
✓ Chunk 2848/4437 written (239 bytes)
✓ Chunk 2849/4437 written (239 bytes)
✓ Chunk 2850/4437 written (239 bytes)
✓ Chunk 2851/4437 written (239 bytes)
✓ Chunk 2852/4437 written (239 bytes)
✓ Chunk 2853/4437 written (239 bytes)
✓ Chunk 2854/4437 written (239 bytes)
✓ Chunk 2855/4437 written (239 bytes)
✓ Chunk 2856/4437 written (239 bytes)
✓ Chunk 2857/4437 written (239 bytes)
✓ Chunk 2858/4437 written (239 bytes)
✓ Chunk 2859/4437 written (239 bytes)
✓ Chunk 2860/4437 written (239 bytes)
✓ Chunk 2861/4437 written (239 bytes)
✓ Chunk 2862/4437 written (239 bytes)
✓ Chunk 2863/4437 written (239 bytes)
✓ Chunk 2864/4437 written (239 bytes)
✓ Chunk 2865/4437 written (239 bytes)
✓ Chunk 2866/4437 written (239 bytes)
✓ Chunk 2867/4437 written (239 bytes)
✓ Chunk 2868/4437 written (239 bytes)
✓ Chunk 2869/4437 written (239 bytes)
✓ Chunk 2870/4437 written (239 bytes)
✓ Chunk 2871/4437 written (239 bytes)
✓ Chunk 2872/4437 written (239 bytes)
✓ Chunk 2873/4437 written (239 bytes)
✓ Chunk 2874/4437 written (239 bytes)
✓ Chunk 2875/4437 written (239 bytes)
✓ Chunk 2876/4437 written (239 bytes)
✓ Chunk 2877/4437 written (239 bytes)
✓ Chunk 2878/4437 written (239 bytes)
✓ Chunk 2879/4437 written (239 bytes)
✓ Chunk 2880/4437 written (239 bytes)
✓ Chunk 2881/4437 written (239 bytes)
✓ Chunk 2882/4437 written (239 bytes)
✓ Chunk 2883/4437 written (239 bytes)
✓ Chunk 2884/4437 written (239 bytes)
✓ Chunk 2885/4437 written (239 bytes)
✓ Chunk 2886/4437 written (239 bytes)
✓ Chunk 2887/4437 written (239 bytes)
✓ Chunk 2888/4437 written (239 bytes)
✓ Chunk 2889/4437 written (239 bytes)
✓ Chunk 2890/4437 written (239 bytes)
✓ Chunk 2891/4437 written (239 bytes)
✓ Chunk 2892/4437 written (239 bytes)
✓ Chunk 2893/4437 written (239 bytes)
✓ Chunk 2894/4437 written (239 bytes)
✓ Chunk 2895/4437 written (239 bytes)
✓ Chunk 2896/4437 written (239 bytes)
✓ Chunk 2897/4437 written (239 bytes)
✓ Chunk 2898/4437 written (239 bytes)
✓ Chunk 2899/4437 written (239 bytes)
✓ Chunk 2900/4437 written (239 bytes)
  [2900/4437] Chunk streamed (seq=2900)
✓ Chunk 2901/4437 written (239 bytes)
✓ Chunk 2902/4437 written (239 bytes)
✓ Chunk 2903/4437 written (239 bytes)
✓ Chunk 2904/4437 written (239 bytes)
✓ Chunk 2905/4437 written (239 bytes)
✓ Chunk 2906/4437 written (239 bytes)
✓ Chunk 2907/4437 written (239 bytes)
✓ Chunk 2908/4437 written (239 bytes)
✓ Chunk 2909/4437 written (239 bytes)
✓ Chunk 2910/4437 written (239 bytes)
✓ Chunk 2911/4437 written (239 bytes)
✓ Chunk 2912/4437 written (239 bytes)
✓ Chunk 2913/4437 written (239 bytes)
✓ Chunk 2914/4437 written (239 bytes)
✓ Chunk 2915/4437 written (239 bytes)
✓ Chunk 2916/4437 written (239 bytes)
✓ Chunk 2917/4437 written (239 bytes)
✓ Chunk 2918/4437 written (239 bytes)
✓ Chunk 2919/4437 written (239 bytes)
✓ Chunk 2920/4437 written (239 bytes)
✓ Chunk 2921/4437 written (239 bytes)
✓ Chunk 2922/4437 written (239 bytes)
✓ Chunk 2923/4437 written (239 bytes)
✓ Chunk 2924/4437 written (239 bytes)
✓ Chunk 2925/4437 written (239 bytes)
✓ Chunk 2926/4437 written (239 bytes)
✓ Chunk 2927/4437 written (239 bytes)
✓ Chunk 2928/4437 written (239 bytes)
✓ Chunk 2929/4437 written (239 bytes)
✓ Chunk 2930/4437 written (239 bytes)
✓ Chunk 2931/4437 written (239 bytes)
✓ Chunk 2932/4437 written (239 bytes)
✓ Chunk 2933/4437 written (239 bytes)
✓ Chunk 2934/4437 written (239 bytes)
✓ Chunk 2935/4437 written (239 bytes)
✓ Chunk 2936/4437 written (239 bytes)
✓ Chunk 2937/4437 written (239 bytes)
✓ Chunk 2938/4437 written (239 bytes)
✓ Chunk 2939/4437 written (239 bytes)
✓ Chunk 2940/4437 written (239 bytes)
✓ Chunk 2941/4437 written (239 bytes)
✓ Chunk 2942/4437 written (239 bytes)
✓ Chunk 2943/4437 written (239 bytes)
✓ Chunk 2944/4437 written (239 bytes)
✓ Chunk 2945/4437 written (239 bytes)
✓ Chunk 2946/4437 written (239 bytes)
✓ Chunk 2947/4437 written (239 bytes)
✓ Chunk 2948/4437 written (239 bytes)
✓ Chunk 2949/4437 written (239 bytes)
✓ Chunk 2950/4437 written (239 bytes)
✓ Chunk 2951/4437 written (239 bytes)
✓ Chunk 2952/4437 written (239 bytes)
✓ Chunk 2953/4437 written (239 bytes)
✓ Chunk 2954/4437 written (239 bytes)
✓ Chunk 2955/4437 written (239 bytes)
✓ Chunk 2956/4437 written (239 bytes)
✓ Chunk 2957/4437 written (239 bytes)
✓ Chunk 2958/4437 written (239 bytes)
✓ Chunk 2959/4437 written (239 bytes)
✓ Chunk 2960/4437 written (239 bytes)
✓ Chunk 2961/4437 written (239 bytes)
✓ Chunk 2962/4437 written (239 bytes)
✓ Chunk 2963/4437 written (239 bytes)
✓ Chunk 2964/4437 written (239 bytes)
✓ Chunk 2965/4437 written (239 bytes)
✓ Chunk 2966/4437 written (239 bytes)
✓ Chunk 2967/4437 written (239 bytes)
✓ Chunk 2968/4437 written (239 bytes)
✓ Chunk 2969/4437 written (239 bytes)
✓ Chunk 2970/4437 written (239 bytes)
✓ Chunk 2971/4437 written (239 bytes)
✓ Chunk 2972/4437 written (239 bytes)
✓ Chunk 2973/4437 written (239 bytes)
✓ Chunk 2974/4437 written (239 bytes)
✓ Chunk 2975/4437 written (239 bytes)
✓ Chunk 2976/4437 written (239 bytes)
✓ Chunk 2977/4437 written (239 bytes)
✓ Chunk 2978/4437 written (239 bytes)
✓ Chunk 2979/4437 written (239 bytes)
✓ Chunk 2980/4437 written (239 bytes)
✓ Chunk 2981/4437 written (239 bytes)
✓ Chunk 2982/4437 written (239 bytes)
✓ Chunk 2983/4437 written (239 bytes)
✓ Chunk 2984/4437 written (239 bytes)
✓ Chunk 2985/4437 written (239 bytes)
✓ Chunk 2986/4437 written (239 bytes)
✓ Chunk 2987/4437 written (239 bytes)
✓ Chunk 2988/4437 written (239 bytes)
✓ Chunk 2989/4437 written (239 bytes)
✓ Chunk 2990/4437 written (239 bytes)
✓ Chunk 2991/4437 written (239 bytes)
✓ Chunk 2992/4437 written (239 bytes)
✓ Chunk 2993/4437 written (239 bytes)
✓ Chunk 2994/4437 written (239 bytes)
✓ Chunk 2995/4437 written (239 bytes)
✓ Chunk 2996/4437 written (239 bytes)
✓ Chunk 2997/4437 written (239 bytes)
✓ Chunk 2998/4437 written (239 bytes)
✓ Chunk 2999/4437 written (239 bytes)
✓ Chunk 3000/4437 written (239 bytes)
  [3000/4437] Chunk streamed (seq=3000)
✓ Chunk 3001/4437 written (239 bytes)
✓ Chunk 3002/4437 written (239 bytes)
✓ Chunk 3003/4437 written (239 bytes)
✓ Chunk 3004/4437 written (239 bytes)
✓ Chunk 3005/4437 written (239 bytes)
✓ Chunk 3006/4437 written (239 bytes)
✓ Chunk 3007/4437 written (239 bytes)
✓ Chunk 3008/4437 written (239 bytes)
✓ Chunk 3009/4437 written (239 bytes)
✓ Chunk 3010/4437 written (239 bytes)
✓ Chunk 3011/4437 written (239 bytes)
✓ Chunk 3012/4437 written (239 bytes)
✓ Chunk 3013/4437 written (239 bytes)
✓ Chunk 3014/4437 written (239 bytes)
✓ Chunk 3015/4437 written (239 bytes)
✓ Chunk 3016/4437 written (239 bytes)
✓ Chunk 3017/4437 written (239 bytes)
✓ Chunk 3018/4437 written (239 bytes)
✓ Chunk 3019/4437 written (239 bytes)
✓ Chunk 3020/4437 written (239 bytes)
✓ Chunk 3021/4437 written (239 bytes)
✓ Chunk 3022/4437 written (239 bytes)
✓ Chunk 3023/4437 written (239 bytes)
✓ Chunk 3024/4437 written (239 bytes)
✓ Chunk 3025/4437 written (239 bytes)
✓ Chunk 3026/4437 written (239 bytes)
✓ Chunk 3027/4437 written (239 bytes)
✓ Chunk 3028/4437 written (239 bytes)
✓ Chunk 3029/4437 written (239 bytes)
✓ Chunk 3030/4437 written (239 bytes)
✓ Chunk 3031/4437 written (239 bytes)
✓ Chunk 3032/4437 written (239 bytes)
✓ Chunk 3033/4437 written (239 bytes)
✓ Chunk 3034/4437 written (239 bytes)
✓ Chunk 3035/4437 written (239 bytes)
✓ Chunk 3036/4437 written (239 bytes)
✓ Chunk 3037/4437 written (239 bytes)
✓ Chunk 3038/4437 written (239 bytes)
✓ Chunk 3039/4437 written (239 bytes)
✓ Chunk 3040/4437 written (239 bytes)
✓ Chunk 3041/4437 written (239 bytes)
✓ Chunk 3042/4437 written (239 bytes)
✓ Chunk 3043/4437 written (239 bytes)
✓ Chunk 3044/4437 written (239 bytes)
✓ Chunk 3045/4437 written (239 bytes)
✓ Chunk 3046/4437 written (239 bytes)
✓ Chunk 3047/4437 written (239 bytes)
✓ Chunk 3048/4437 written (239 bytes)
✓ Chunk 3049/4437 written (239 bytes)
✓ Chunk 3050/4437 written (239 bytes)
✓ Chunk 3051/4437 written (239 bytes)
✓ Chunk 3052/4437 written (239 bytes)
✓ Chunk 3053/4437 written (239 bytes)
✓ Chunk 3054/4437 written (239 bytes)
✓ Chunk 3055/4437 written (239 bytes)
✓ Chunk 3056/4437 written (239 bytes)
✓ Chunk 3057/4437 written (239 bytes)
✓ Chunk 3058/4437 written (239 bytes)
✓ Chunk 3059/4437 written (239 bytes)
✓ Chunk 3060/4437 written (239 bytes)
✓ Chunk 3061/4437 written (239 bytes)
✓ Chunk 3062/4437 written (239 bytes)
✓ Chunk 3063/4437 written (239 bytes)
✓ Chunk 3064/4437 written (239 bytes)
✓ Chunk 3065/4437 written (239 bytes)
✓ Chunk 3066/4437 written (239 bytes)
✓ Chunk 3067/4437 written (239 bytes)
✓ Chunk 3068/4437 written (239 bytes)
✓ Chunk 3069/4437 written (239 bytes)
✓ Chunk 3070/4437 written (239 bytes)
✓ Chunk 3071/4437 written (239 bytes)
✓ Chunk 3072/4437 written (239 bytes)
✓ Chunk 3073/4437 written (239 bytes)
✓ Chunk 3074/4437 written (239 bytes)
✓ Chunk 3075/4437 written (239 bytes)
✓ Chunk 3076/4437 written (239 bytes)
✓ Chunk 3077/4437 written (239 bytes)
✓ Chunk 3078/4437 written (239 bytes)
✓ Chunk 3079/4437 written (239 bytes)
✓ Chunk 3080/4437 written (239 bytes)
✓ Chunk 3081/4437 written (239 bytes)
✓ Chunk 3082/4437 written (239 bytes)
✓ Chunk 3083/4437 written (239 bytes)
✓ Chunk 3084/4437 written (239 bytes)
✓ Chunk 3085/4437 written (239 bytes)
✓ Chunk 3086/4437 written (239 bytes)
✓ Chunk 3087/4437 written (239 bytes)
✓ Chunk 3088/4437 written (239 bytes)
✓ Chunk 3089/4437 written (239 bytes)
✓ Chunk 3090/4437 written (239 bytes)
✓ Chunk 3091/4437 written (239 bytes)
✓ Chunk 3092/4437 written (239 bytes)
✓ Chunk 3093/4437 written (239 bytes)
✓ Chunk 3094/4437 written (239 bytes)
✓ Chunk 3095/4437 written (239 bytes)
✓ Chunk 3096/4437 written (239 bytes)
✓ Chunk 3097/4437 written (239 bytes)
✓ Chunk 3098/4437 written (239 bytes)
✓ Chunk 3099/4437 written (239 bytes)
✓ Chunk 3100/4437 written (239 bytes)
  [3100/4437] Chunk streamed (seq=3100)
✓ Chunk 3101/4437 written (239 bytes)
✓ Chunk 3102/4437 written (239 bytes)
✓ Chunk 3103/4437 written (239 bytes)
✓ Chunk 3104/4437 written (239 bytes)
✓ Chunk 3105/4437 written (239 bytes)
✓ Chunk 3106/4437 written (239 bytes)
✓ Chunk 3107/4437 written (239 bytes)
✓ Chunk 3108/4437 written (239 bytes)
✓ Chunk 3109/4437 written (239 bytes)
✓ Chunk 3110/4437 written (239 bytes)
✓ Chunk 3111/4437 written (239 bytes)
✓ Chunk 3112/4437 written (239 bytes)
✓ Chunk 3113/4437 written (239 bytes)
✓ Chunk 3114/4437 written (239 bytes)
✓ Chunk 3115/4437 written (239 bytes)
✓ Chunk 3116/4437 written (239 bytes)
✓ Chunk 3117/4437 written (239 bytes)
✓ Chunk 3118/4437 written (239 bytes)
✓ Chunk 3119/4437 written (239 bytes)
✓ Chunk 3120/4437 written (239 bytes)
✓ Chunk 3121/4437 written (239 bytes)
✓ Chunk 3122/4437 written (239 bytes)
✓ Chunk 3123/4437 written (239 bytes)
✓ Chunk 3124/4437 written (239 bytes)
✓ Chunk 3125/4437 written (239 bytes)
✓ Chunk 3126/4437 written (239 bytes)
✓ Chunk 3127/4437 written (239 bytes)
✓ Chunk 3128/4437 written (239 bytes)
✓ Chunk 3129/4437 written (239 bytes)
✓ Chunk 3130/4437 written (239 bytes)
✓ Chunk 3131/4437 written (239 bytes)
✓ Chunk 3132/4437 written (239 bytes)
✓ Chunk 3133/4437 written (239 bytes)
✓ Chunk 3134/4437 written (239 bytes)
✓ Chunk 3135/4437 written (239 bytes)
✓ Chunk 3136/4437 written (239 bytes)
✓ Chunk 3137/4437 written (239 bytes)
✓ Chunk 3138/4437 written (239 bytes)
✓ Chunk 3139/4437 written (239 bytes)
✓ Chunk 3140/4437 written (239 bytes)
✓ Chunk 3141/4437 written (239 bytes)
✓ Chunk 3142/4437 written (239 bytes)
✓ Chunk 3143/4437 written (239 bytes)
✓ Chunk 3144/4437 written (239 bytes)
✓ Chunk 3145/4437 written (239 bytes)
✓ Chunk 3146/4437 written (239 bytes)
✓ Chunk 3147/4437 written (239 bytes)
✓ Chunk 3148/4437 written (239 bytes)
✓ Chunk 3149/4437 written (239 bytes)
✓ Chunk 3150/4437 written (239 bytes)
✓ Chunk 3151/4437 written (239 bytes)
✓ Chunk 3152/4437 written (239 bytes)
✓ Chunk 3153/4437 written (239 bytes)
✓ Chunk 3154/4437 written (239 bytes)
✓ Chunk 3155/4437 written (239 bytes)
✓ Chunk 3156/4437 written (239 bytes)
✓ Chunk 3157/4437 written (239 bytes)
✓ Chunk 3158/4437 written (239 bytes)
✓ Chunk 3159/4437 written (239 bytes)
✓ Chunk 3160/4437 written (239 bytes)
✓ Chunk 3161/4437 written (239 bytes)
✓ Chunk 3162/4437 written (239 bytes)
✓ Chunk 3163/4437 written (239 bytes)
✓ Chunk 3164/4437 written (239 bytes)
✓ Chunk 3165/4437 written (239 bytes)
✓ Chunk 3166/4437 written (239 bytes)
✓ Chunk 3167/4437 written (239 bytes)
✓ Chunk 3168/4437 written (239 bytes)
✓ Chunk 3169/4437 written (239 bytes)
✓ Chunk 3170/4437 written (239 bytes)
✓ Chunk 3171/4437 written (239 bytes)
✓ Chunk 3172/4437 written (239 bytes)
✓ Chunk 3173/4437 written (239 bytes)
✓ Chunk 3174/4437 written (239 bytes)
✓ Chunk 3175/4437 written (239 bytes)
✓ Chunk 3176/4437 written (239 bytes)
✓ Chunk 3177/4437 written (239 bytes)
✓ Chunk 3178/4437 written (239 bytes)
✓ Chunk 3179/4437 written (239 bytes)
✓ Chunk 3180/4437 written (239 bytes)
✓ Chunk 3181/4437 written (239 bytes)
✓ Chunk 3182/4437 written (239 bytes)
✓ Chunk 3183/4437 written (239 bytes)
✓ Chunk 3184/4437 written (239 bytes)
✓ Chunk 3185/4437 written (239 bytes)
✓ Chunk 3186/4437 written (239 bytes)
✓ Chunk 3187/4437 written (239 bytes)
✓ Chunk 3188/4437 written (239 bytes)
✓ Chunk 3189/4437 written (239 bytes)
✓ Chunk 3190/4437 written (239 bytes)
✓ Chunk 3191/4437 written (239 bytes)
✓ Chunk 3192/4437 written (239 bytes)
✓ Chunk 3193/4437 written (239 bytes)
✓ Chunk 3194/4437 written (239 bytes)
✓ Chunk 3195/4437 written (239 bytes)
✓ Chunk 3196/4437 written (239 bytes)
✓ Chunk 3197/4437 written (239 bytes)
✓ Chunk 3198/4437 written (239 bytes)
✓ Chunk 3199/4437 written (239 bytes)
✓ Chunk 3200/4437 written (239 bytes)
  [3200/4437] Chunk streamed (seq=3200)
✓ Chunk 3201/4437 written (239 bytes)
✓ Chunk 3202/4437 written (239 bytes)
✓ Chunk 3203/4437 written (239 bytes)
✓ Chunk 3204/4437 written (239 bytes)
✓ Chunk 3205/4437 written (239 bytes)
✓ Chunk 3206/4437 written (239 bytes)
✓ Chunk 3207/4437 written (239 bytes)
✓ Chunk 3208/4437 written (239 bytes)
✓ Chunk 3209/4437 written (239 bytes)
✓ Chunk 3210/4437 written (239 bytes)
✓ Chunk 3211/4437 written (239 bytes)
✓ Chunk 3212/4437 written (239 bytes)
✓ Chunk 3213/4437 written (239 bytes)
✓ Chunk 3214/4437 written (239 bytes)
✓ Chunk 3215/4437 written (239 bytes)
✓ Chunk 3216/4437 written (239 bytes)
✓ Chunk 3217/4437 written (239 bytes)
✓ Chunk 3218/4437 written (239 bytes)
✓ Chunk 3219/4437 written (239 bytes)
✓ Chunk 3220/4437 written (239 bytes)
✓ Chunk 3221/4437 written (239 bytes)
✓ Chunk 3222/4437 written (239 bytes)
✓ Chunk 3223/4437 written (239 bytes)
✓ Chunk 3224/4437 written (239 bytes)
✓ Chunk 3225/4437 written (239 bytes)
✓ Chunk 3226/4437 written (239 bytes)
✓ Chunk 3227/4437 written (239 bytes)
✓ Chunk 3228/4437 written (239 bytes)
✓ Chunk 3229/4437 written (239 bytes)
✓ Chunk 3230/4437 written (239 bytes)
✓ Chunk 3231/4437 written (239 bytes)
✓ Chunk 3232/4437 written (239 bytes)
✓ Chunk 3233/4437 written (239 bytes)
✓ Chunk 3234/4437 written (239 bytes)
✓ Chunk 3235/4437 written (239 bytes)
✓ Chunk 3236/4437 written (239 bytes)
✓ Chunk 3237/4437 written (239 bytes)
✓ Chunk 3238/4437 written (239 bytes)
✓ Chunk 3239/4437 written (239 bytes)
✓ Chunk 3240/4437 written (239 bytes)
✓ Chunk 3241/4437 written (239 bytes)
✓ Chunk 3242/4437 written (239 bytes)
✓ Chunk 3243/4437 written (239 bytes)
✓ Chunk 3244/4437 written (239 bytes)
✓ Chunk 3245/4437 written (239 bytes)
✓ Chunk 3246/4437 written (239 bytes)
✓ Chunk 3247/4437 written (239 bytes)
✓ Chunk 3248/4437 written (239 bytes)
✓ Chunk 3249/4437 written (239 bytes)
✓ Chunk 3250/4437 written (239 bytes)
✓ Chunk 3251/4437 written (239 bytes)
✓ Chunk 3252/4437 written (239 bytes)
✓ Chunk 3253/4437 written (239 bytes)
✓ Chunk 3254/4437 written (239 bytes)
✓ Chunk 3255/4437 written (239 bytes)
✓ Chunk 3256/4437 written (239 bytes)
✓ Chunk 3257/4437 written (239 bytes)
✓ Chunk 3258/4437 written (239 bytes)
✓ Chunk 3259/4437 written (239 bytes)
✓ Chunk 3260/4437 written (239 bytes)
✓ Chunk 3261/4437 written (239 bytes)
✓ Chunk 3262/4437 written (239 bytes)
✓ Chunk 3263/4437 written (239 bytes)
✓ Chunk 3264/4437 written (239 bytes)
✓ Chunk 3265/4437 written (239 bytes)
✓ Chunk 3266/4437 written (239 bytes)
✓ Chunk 3267/4437 written (239 bytes)
✓ Chunk 3268/4437 written (239 bytes)
✓ Chunk 3269/4437 written (239 bytes)
✓ Chunk 3270/4437 written (239 bytes)
✓ Chunk 3271/4437 written (239 bytes)
✓ Chunk 3272/4437 written (239 bytes)
✓ Chunk 3273/4437 written (239 bytes)
✓ Chunk 3274/4437 written (239 bytes)
✓ Chunk 3275/4437 written (239 bytes)
✓ Chunk 3276/4437 written (239 bytes)
✓ Chunk 3277/4437 written (239 bytes)
✓ Chunk 3278/4437 written (239 bytes)
✓ Chunk 3279/4437 written (239 bytes)
✓ Chunk 3280/4437 written (239 bytes)
✓ Chunk 3281/4437 written (239 bytes)
✓ Chunk 3282/4437 written (239 bytes)
✓ Chunk 3283/4437 written (239 bytes)
✓ Chunk 3284/4437 written (239 bytes)
✓ Chunk 3285/4437 written (239 bytes)
✓ Chunk 3286/4437 written (239 bytes)
✓ Chunk 3287/4437 written (239 bytes)
✓ Chunk 3288/4437 written (239 bytes)
✓ Chunk 3289/4437 written (239 bytes)
✓ Chunk 3290/4437 written (239 bytes)
✓ Chunk 3291/4437 written (239 bytes)
✓ Chunk 3292/4437 written (239 bytes)
✓ Chunk 3293/4437 written (239 bytes)
✓ Chunk 3294/4437 written (239 bytes)
✓ Chunk 3295/4437 written (239 bytes)
✓ Chunk 3296/4437 written (239 bytes)
✓ Chunk 3297/4437 written (239 bytes)
✓ Chunk 3298/4437 written (239 bytes)
✓ Chunk 3299/4437 written (239 bytes)
✓ Chunk 3300/4437 written (239 bytes)
  [3300/4437] Chunk streamed (seq=3300)
✓ Chunk 3301/4437 written (239 bytes)
✓ Chunk 3302/4437 written (239 bytes)
✓ Chunk 3303/4437 written (239 bytes)
✓ Chunk 3304/4437 written (239 bytes)
✓ Chunk 3305/4437 written (239 bytes)
✓ Chunk 3306/4437 written (239 bytes)
✓ Chunk 3307/4437 written (239 bytes)
✓ Chunk 3308/4437 written (239 bytes)
✓ Chunk 3309/4437 written (239 bytes)
✓ Chunk 3310/4437 written (239 bytes)
✓ Chunk 3311/4437 written (239 bytes)
✓ Chunk 3312/4437 written (239 bytes)
✓ Chunk 3313/4437 written (239 bytes)
✓ Chunk 3314/4437 written (239 bytes)
✓ Chunk 3315/4437 written (239 bytes)
✓ Chunk 3316/4437 written (239 bytes)
✓ Chunk 3317/4437 written (239 bytes)
✓ Chunk 3318/4437 written (239 bytes)
✓ Chunk 3319/4437 written (239 bytes)
✓ Chunk 3320/4437 written (239 bytes)
✓ Chunk 3321/4437 written (239 bytes)
✓ Chunk 3322/4437 written (239 bytes)
✓ Chunk 3323/4437 written (239 bytes)
✓ Chunk 3324/4437 written (239 bytes)
✓ Chunk 3325/4437 written (239 bytes)
✓ Chunk 3326/4437 written (239 bytes)
✓ Chunk 3327/4437 written (239 bytes)
✓ Chunk 3328/4437 written (239 bytes)
✓ Chunk 3329/4437 written (239 bytes)
✓ Chunk 3330/4437 written (239 bytes)
✓ Chunk 3331/4437 written (239 bytes)
✓ Chunk 3332/4437 written (239 bytes)
✓ Chunk 3333/4437 written (239 bytes)
✓ Chunk 3334/4437 written (239 bytes)
✓ Chunk 3335/4437 written (239 bytes)
✓ Chunk 3336/4437 written (239 bytes)
✓ Chunk 3337/4437 written (239 bytes)
✓ Chunk 3338/4437 written (239 bytes)
✓ Chunk 3339/4437 written (239 bytes)
✓ Chunk 3340/4437 written (239 bytes)
✓ Chunk 3341/4437 written (239 bytes)
✓ Chunk 3342/4437 written (239 bytes)
✓ Chunk 3343/4437 written (239 bytes)
✓ Chunk 3344/4437 written (239 bytes)
✓ Chunk 3345/4437 written (239 bytes)
✓ Chunk 3346/4437 written (239 bytes)
✓ Chunk 3347/4437 written (239 bytes)
✓ Chunk 3348/4437 written (239 bytes)
✓ Chunk 3349/4437 written (239 bytes)
✓ Chunk 3350/4437 written (239 bytes)
✓ Chunk 3351/4437 written (239 bytes)
✓ Chunk 3352/4437 written (239 bytes)
✓ Chunk 3353/4437 written (239 bytes)
✓ Chunk 3354/4437 written (239 bytes)
✓ Chunk 3355/4437 written (239 bytes)
✓ Chunk 3356/4437 written (239 bytes)
✓ Chunk 3357/4437 written (239 bytes)
✓ Chunk 3358/4437 written (239 bytes)
✓ Chunk 3359/4437 written (239 bytes)
✓ Chunk 3360/4437 written (239 bytes)
✓ Chunk 3361/4437 written (239 bytes)
✓ Chunk 3362/4437 written (239 bytes)
✓ Chunk 3363/4437 written (239 bytes)
✓ Chunk 3364/4437 written (239 bytes)
✓ Chunk 3365/4437 written (239 bytes)
✓ Chunk 3366/4437 written (239 bytes)
✓ Chunk 3367/4437 written (239 bytes)
✓ Chunk 3368/4437 written (239 bytes)
✓ Chunk 3369/4437 written (239 bytes)
✓ Chunk 3370/4437 written (239 bytes)
✓ Chunk 3371/4437 written (239 bytes)
✓ Chunk 3372/4437 written (239 bytes)
✓ Chunk 3373/4437 written (239 bytes)
✓ Chunk 3374/4437 written (239 bytes)
✓ Chunk 3375/4437 written (239 bytes)
✓ Chunk 3376/4437 written (239 bytes)
✓ Chunk 3377/4437 written (239 bytes)
✓ Chunk 3378/4437 written (239 bytes)
✓ Chunk 3379/4437 written (239 bytes)
✓ Chunk 3380/4437 written (239 bytes)
✓ Chunk 3381/4437 written (239 bytes)
✓ Chunk 3382/4437 written (239 bytes)
✓ Chunk 3383/4437 written (239 bytes)
✓ Chunk 3384/4437 written (239 bytes)
✓ Chunk 3385/4437 written (239 bytes)
✓ Chunk 3386/4437 written (239 bytes)
✓ Chunk 3387/4437 written (239 bytes)
✓ Chunk 3388/4437 written (239 bytes)
✓ Chunk 3389/4437 written (239 bytes)
✓ Chunk 3390/4437 written (239 bytes)
✓ Chunk 3391/4437 written (239 bytes)
✓ Chunk 3392/4437 written (239 bytes)
✓ Chunk 3393/4437 written (239 bytes)
✓ Chunk 3394/4437 written (239 bytes)
✓ Chunk 3395/4437 written (239 bytes)
✓ Chunk 3396/4437 written (239 bytes)
✓ Chunk 3397/4437 written (239 bytes)
✓ Chunk 3398/4437 written (239 bytes)
✓ Chunk 3399/4437 written (239 bytes)
✓ Chunk 3400/4437 written (239 bytes)
  [3400/4437] Chunk streamed (seq=3400)
✓ Chunk 3401/4437 written (239 bytes)
✓ Chunk 3402/4437 written (239 bytes)
✓ Chunk 3403/4437 written (239 bytes)
✓ Chunk 3404/4437 written (239 bytes)
✓ Chunk 3405/4437 written (239 bytes)
✓ Chunk 3406/4437 written (239 bytes)
✓ Chunk 3407/4437 written (239 bytes)
✓ Chunk 3408/4437 written (239 bytes)
✓ Chunk 3409/4437 written (239 bytes)
✓ Chunk 3410/4437 written (239 bytes)
✓ Chunk 3411/4437 written (239 bytes)
✓ Chunk 3412/4437 written (239 bytes)
✓ Chunk 3413/4437 written (239 bytes)
✓ Chunk 3414/4437 written (239 bytes)
✓ Chunk 3415/4437 written (239 bytes)
✓ Chunk 3416/4437 written (239 bytes)
✓ Chunk 3417/4437 written (239 bytes)
✓ Chunk 3418/4437 written (239 bytes)
✓ Chunk 3419/4437 written (239 bytes)
✓ Chunk 3420/4437 written (239 bytes)
✓ Chunk 3421/4437 written (239 bytes)
✓ Chunk 3422/4437 written (239 bytes)
✓ Chunk 3423/4437 written (239 bytes)
✓ Chunk 3424/4437 written (239 bytes)
✓ Chunk 3425/4437 written (239 bytes)
✓ Chunk 3426/4437 written (239 bytes)
✓ Chunk 3427/4437 written (239 bytes)
✓ Chunk 3428/4437 written (239 bytes)
✓ Chunk 3429/4437 written (239 bytes)
✓ Chunk 3430/4437 written (239 bytes)
✓ Chunk 3431/4437 written (239 bytes)
✓ Chunk 3432/4437 written (239 bytes)
✓ Chunk 3433/4437 written (239 bytes)
✓ Chunk 3434/4437 written (239 bytes)
✓ Chunk 3435/4437 written (239 bytes)
✓ Chunk 3436/4437 written (239 bytes)
✓ Chunk 3437/4437 written (239 bytes)
✓ Chunk 3438/4437 written (239 bytes)
✓ Chunk 3439/4437 written (239 bytes)
✓ Chunk 3440/4437 written (239 bytes)
✓ Chunk 3441/4437 written (239 bytes)
✓ Chunk 3442/4437 written (239 bytes)
✓ Chunk 3443/4437 written (239 bytes)
✓ Chunk 3444/4437 written (239 bytes)
✓ Chunk 3445/4437 written (239 bytes)
✓ Chunk 3446/4437 written (239 bytes)
✓ Chunk 3447/4437 written (239 bytes)
✓ Chunk 3448/4437 written (239 bytes)
✓ Chunk 3449/4437 written (239 bytes)
✓ Chunk 3450/4437 written (239 bytes)
✓ Chunk 3451/4437 written (239 bytes)
✓ Chunk 3452/4437 written (239 bytes)
✓ Chunk 3453/4437 written (239 bytes)
✓ Chunk 3454/4437 written (239 bytes)
✓ Chunk 3455/4437 written (239 bytes)
✓ Chunk 3456/4437 written (239 bytes)
✓ Chunk 3457/4437 written (239 bytes)
✓ Chunk 3458/4437 written (239 bytes)
✓ Chunk 3459/4437 written (239 bytes)
✓ Chunk 3460/4437 written (239 bytes)
✓ Chunk 3461/4437 written (239 bytes)
✓ Chunk 3462/4437 written (239 bytes)
✓ Chunk 3463/4437 written (239 bytes)
✓ Chunk 3464/4437 written (239 bytes)
✓ Chunk 3465/4437 written (239 bytes)
✓ Chunk 3466/4437 written (239 bytes)
✓ Chunk 3467/4437 written (239 bytes)
✓ Chunk 3468/4437 written (239 bytes)
✓ Chunk 3469/4437 written (239 bytes)
✓ Chunk 3470/4437 written (239 bytes)
✓ Chunk 3471/4437 written (239 bytes)
✓ Chunk 3472/4437 written (239 bytes)
✓ Chunk 3473/4437 written (239 bytes)
✓ Chunk 3474/4437 written (239 bytes)
✓ Chunk 3475/4437 written (239 bytes)
✓ Chunk 3476/4437 written (239 bytes)
✓ Chunk 3477/4437 written (239 bytes)
✓ Chunk 3478/4437 written (239 bytes)
✓ Chunk 3479/4437 written (239 bytes)
✓ Chunk 3480/4437 written (239 bytes)
✓ Chunk 3481/4437 written (239 bytes)
✓ Chunk 3482/4437 written (239 bytes)
✓ Chunk 3483/4437 written (239 bytes)
✓ Chunk 3484/4437 written (239 bytes)
✓ Chunk 3485/4437 written (239 bytes)
✓ Chunk 3486/4437 written (239 bytes)
✓ Chunk 3487/4437 written (239 bytes)
✓ Chunk 3488/4437 written (239 bytes)
✓ Chunk 3489/4437 written (239 bytes)
✓ Chunk 3490/4437 written (239 bytes)
✓ Chunk 3491/4437 written (239 bytes)
✓ Chunk 3492/4437 written (239 bytes)
✓ Chunk 3493/4437 written (239 bytes)
✓ Chunk 3494/4437 written (239 bytes)
✓ Chunk 3495/4437 written (239 bytes)
✓ Chunk 3496/4437 written (239 bytes)
✓ Chunk 3497/4437 written (239 bytes)
✓ Chunk 3498/4437 written (239 bytes)
✓ Chunk 3499/4437 written (239 bytes)
✓ Chunk 3500/4437 written (239 bytes)
  [3500/4437] Chunk streamed (seq=3500)
✓ Chunk 3501/4437 written (239 bytes)
✓ Chunk 3502/4437 written (239 bytes)
✓ Chunk 3503/4437 written (239 bytes)
✓ Chunk 3504/4437 written (239 bytes)
✓ Chunk 3505/4437 written (239 bytes)
✓ Chunk 3506/4437 written (239 bytes)
✓ Chunk 3507/4437 written (239 bytes)
✓ Chunk 3508/4437 written (239 bytes)
✓ Chunk 3509/4437 written (239 bytes)
✓ Chunk 3510/4437 written (239 bytes)
✓ Chunk 3511/4437 written (239 bytes)
✓ Chunk 3512/4437 written (239 bytes)
✓ Chunk 3513/4437 written (239 bytes)
✓ Chunk 3514/4437 written (239 bytes)
✓ Chunk 3515/4437 written (239 bytes)
✓ Chunk 3516/4437 written (239 bytes)
✓ Chunk 3517/4437 written (239 bytes)
✓ Chunk 3518/4437 written (239 bytes)
✓ Chunk 3519/4437 written (239 bytes)
✓ Chunk 3520/4437 written (239 bytes)
✓ Chunk 3521/4437 written (239 bytes)
✓ Chunk 3522/4437 written (239 bytes)
✓ Chunk 3523/4437 written (239 bytes)
✓ Chunk 3524/4437 written (239 bytes)
✓ Chunk 3525/4437 written (239 bytes)
✓ Chunk 3526/4437 written (239 bytes)
✓ Chunk 3527/4437 written (239 bytes)
✓ Chunk 3528/4437 written (239 bytes)
✓ Chunk 3529/4437 written (239 bytes)
✓ Chunk 3530/4437 written (239 bytes)
✓ Chunk 3531/4437 written (239 bytes)
✓ Chunk 3532/4437 written (239 bytes)
✓ Chunk 3533/4437 written (239 bytes)
✓ Chunk 3534/4437 written (239 bytes)
✓ Chunk 3535/4437 written (239 bytes)
✓ Chunk 3536/4437 written (239 bytes)
✓ Chunk 3537/4437 written (239 bytes)
✓ Chunk 3538/4437 written (239 bytes)
✓ Chunk 3539/4437 written (239 bytes)
✓ Chunk 3540/4437 written (239 bytes)
✓ Chunk 3541/4437 written (239 bytes)
✓ Chunk 3542/4437 written (239 bytes)
✓ Chunk 3543/4437 written (239 bytes)
✓ Chunk 3544/4437 written (239 bytes)
✓ Chunk 3545/4437 written (239 bytes)
✓ Chunk 3546/4437 written (239 bytes)
✓ Chunk 3547/4437 written (239 bytes)
✓ Chunk 3548/4437 written (239 bytes)
✓ Chunk 3549/4437 written (239 bytes)
✓ Chunk 3550/4437 written (239 bytes)
✓ Chunk 3551/4437 written (239 bytes)
✓ Chunk 3552/4437 written (239 bytes)
✓ Chunk 3553/4437 written (239 bytes)
✓ Chunk 3554/4437 written (239 bytes)
✓ Chunk 3555/4437 written (239 bytes)
✓ Chunk 3556/4437 written (239 bytes)
✓ Chunk 3557/4437 written (239 bytes)
✓ Chunk 3558/4437 written (239 bytes)
✓ Chunk 3559/4437 written (239 bytes)
✓ Chunk 3560/4437 written (239 bytes)
✓ Chunk 3561/4437 written (239 bytes)
✓ Chunk 3562/4437 written (239 bytes)
✓ Chunk 3563/4437 written (239 bytes)
✓ Chunk 3564/4437 written (239 bytes)
✓ Chunk 3565/4437 written (239 bytes)
✓ Chunk 3566/4437 written (239 bytes)
✓ Chunk 3567/4437 written (239 bytes)
✓ Chunk 3568/4437 written (239 bytes)
✓ Chunk 3569/4437 written (239 bytes)
✓ Chunk 3570/4437 written (239 bytes)
✓ Chunk 3571/4437 written (239 bytes)
✓ Chunk 3572/4437 written (239 bytes)
✓ Chunk 3573/4437 written (239 bytes)
✓ Chunk 3574/4437 written (239 bytes)
✓ Chunk 3575/4437 written (239 bytes)
✓ Chunk 3576/4437 written (239 bytes)
✓ Chunk 3577/4437 written (239 bytes)
✓ Chunk 3578/4437 written (239 bytes)
✓ Chunk 3579/4437 written (239 bytes)
✓ Chunk 3580/4437 written (239 bytes)
✓ Chunk 3581/4437 written (239 bytes)
✓ Chunk 3582/4437 written (239 bytes)
✓ Chunk 3583/4437 written (239 bytes)
✓ Chunk 3584/4437 written (239 bytes)
✓ Chunk 3585/4437 written (239 bytes)
✓ Chunk 3586/4437 written (239 bytes)
✓ Chunk 3587/4437 written (239 bytes)
✓ Chunk 3588/4437 written (239 bytes)
✓ Chunk 3589/4437 written (239 bytes)
✓ Chunk 3590/4437 written (239 bytes)
✓ Chunk 3591/4437 written (239 bytes)
✓ Chunk 3592/4437 written (239 bytes)
✓ Chunk 3593/4437 written (239 bytes)
✓ Chunk 3594/4437 written (239 bytes)
✓ Chunk 3595/4437 written (239 bytes)
✓ Chunk 3596/4437 written (239 bytes)
✓ Chunk 3597/4437 written (239 bytes)
✓ Chunk 3598/4437 written (239 bytes)
✓ Chunk 3599/4437 written (239 bytes)
✓ Chunk 3600/4437 written (239 bytes)
  [3600/4437] Chunk streamed (seq=3600)
✓ Chunk 3601/4437 written (239 bytes)
✓ Chunk 3602/4437 written (239 bytes)
✓ Chunk 3603/4437 written (239 bytes)
✓ Chunk 3604/4437 written (239 bytes)
✓ Chunk 3605/4437 written (239 bytes)
✓ Chunk 3606/4437 written (239 bytes)
✓ Chunk 3607/4437 written (239 bytes)
✓ Chunk 3608/4437 written (239 bytes)
✓ Chunk 3609/4437 written (239 bytes)
✓ Chunk 3610/4437 written (239 bytes)
✓ Chunk 3611/4437 written (239 bytes)
✓ Chunk 3612/4437 written (239 bytes)
✓ Chunk 3613/4437 written (239 bytes)
✓ Chunk 3614/4437 written (239 bytes)
✓ Chunk 3615/4437 written (239 bytes)
✓ Chunk 3616/4437 written (239 bytes)
✓ Chunk 3617/4437 written (239 bytes)
✓ Chunk 3618/4437 written (239 bytes)
✓ Chunk 3619/4437 written (239 bytes)
✓ Chunk 3620/4437 written (239 bytes)
✓ Chunk 3621/4437 written (239 bytes)
✓ Chunk 3622/4437 written (239 bytes)
✓ Chunk 3623/4437 written (239 bytes)
✓ Chunk 3624/4437 written (239 bytes)
✓ Chunk 3625/4437 written (239 bytes)
✓ Chunk 3626/4437 written (239 bytes)
✓ Chunk 3627/4437 written (239 bytes)
✓ Chunk 3628/4437 written (239 bytes)
✓ Chunk 3629/4437 written (239 bytes)
✓ Chunk 3630/4437 written (239 bytes)
✓ Chunk 3631/4437 written (239 bytes)
✓ Chunk 3632/4437 written (239 bytes)
✓ Chunk 3633/4437 written (239 bytes)
✓ Chunk 3634/4437 written (239 bytes)
✓ Chunk 3635/4437 written (239 bytes)
✓ Chunk 3636/4437 written (239 bytes)
✓ Chunk 3637/4437 written (239 bytes)
✓ Chunk 3638/4437 written (239 bytes)
✓ Chunk 3639/4437 written (239 bytes)
✓ Chunk 3640/4437 written (239 bytes)
✓ Chunk 3641/4437 written (239 bytes)
✓ Chunk 3642/4437 written (239 bytes)
✓ Chunk 3643/4437 written (239 bytes)
✓ Chunk 3644/4437 written (239 bytes)
✓ Chunk 3645/4437 written (239 bytes)
✓ Chunk 3646/4437 written (239 bytes)
✓ Chunk 3647/4437 written (239 bytes)
✓ Chunk 3648/4437 written (239 bytes)
✓ Chunk 3649/4437 written (239 bytes)
✓ Chunk 3650/4437 written (239 bytes)
✓ Chunk 3651/4437 written (239 bytes)
✓ Chunk 3652/4437 written (239 bytes)
✓ Chunk 3653/4437 written (239 bytes)
✓ Chunk 3654/4437 written (239 bytes)
✓ Chunk 3655/4437 written (239 bytes)
✓ Chunk 3656/4437 written (239 bytes)
✓ Chunk 3657/4437 written (239 bytes)
✓ Chunk 3658/4437 written (239 bytes)
✓ Chunk 3659/4437 written (239 bytes)
✓ Chunk 3660/4437 written (239 bytes)
✓ Chunk 3661/4437 written (239 bytes)
✓ Chunk 3662/4437 written (239 bytes)
✓ Chunk 3663/4437 written (239 bytes)
✓ Chunk 3664/4437 written (239 bytes)
✓ Chunk 3665/4437 written (239 bytes)
✓ Chunk 3666/4437 written (239 bytes)
✓ Chunk 3667/4437 written (239 bytes)
✓ Chunk 3668/4437 written (239 bytes)
✓ Chunk 3669/4437 written (239 bytes)
✓ Chunk 3670/4437 written (239 bytes)
✓ Chunk 3671/4437 written (239 bytes)
✓ Chunk 3672/4437 written (239 bytes)
✓ Chunk 3673/4437 written (239 bytes)
✓ Chunk 3674/4437 written (239 bytes)
✓ Chunk 3675/4437 written (239 bytes)
✓ Chunk 3676/4437 written (239 bytes)
✓ Chunk 3677/4437 written (239 bytes)
✓ Chunk 3678/4437 written (239 bytes)
✓ Chunk 3679/4437 written (239 bytes)
✓ Chunk 3680/4437 written (239 bytes)
✓ Chunk 3681/4437 written (239 bytes)
✓ Chunk 3682/4437 written (239 bytes)
✓ Chunk 3683/4437 written (239 bytes)
✓ Chunk 3684/4437 written (239 bytes)
✓ Chunk 3685/4437 written (239 bytes)
✓ Chunk 3686/4437 written (239 bytes)
✓ Chunk 3687/4437 written (239 bytes)
✓ Chunk 3688/4437 written (239 bytes)
✓ Chunk 3689/4437 written (239 bytes)
✓ Chunk 3690/4437 written (239 bytes)
✓ Chunk 3691/4437 written (239 bytes)
✓ Chunk 3692/4437 written (239 bytes)
✓ Chunk 3693/4437 written (239 bytes)
✓ Chunk 3694/4437 written (239 bytes)
✓ Chunk 3695/4437 written (239 bytes)
✓ Chunk 3696/4437 written (239 bytes)
✓ Chunk 3697/4437 written (239 bytes)
✓ Chunk 3698/4437 written (239 bytes)
✓ Chunk 3699/4437 written (239 bytes)
✓ Chunk 3700/4437 written (239 bytes)
  [3700/4437] Chunk streamed (seq=3700)
✓ Chunk 3701/4437 written (239 bytes)
✓ Chunk 3702/4437 written (239 bytes)
✓ Chunk 3703/4437 written (239 bytes)
✓ Chunk 3704/4437 written (239 bytes)
✓ Chunk 3705/4437 written (239 bytes)
✓ Chunk 3706/4437 written (239 bytes)
✓ Chunk 3707/4437 written (239 bytes)
✓ Chunk 3708/4437 written (239 bytes)
✓ Chunk 3709/4437 written (239 bytes)
✓ Chunk 3710/4437 written (239 bytes)
✓ Chunk 3711/4437 written (239 bytes)
✓ Chunk 3712/4437 written (239 bytes)
✓ Chunk 3713/4437 written (239 bytes)
✓ Chunk 3714/4437 written (239 bytes)
✓ Chunk 3715/4437 written (239 bytes)
✓ Chunk 3716/4437 written (239 bytes)
✓ Chunk 3717/4437 written (239 bytes)
✓ Chunk 3718/4437 written (239 bytes)
✓ Chunk 3719/4437 written (239 bytes)
✓ Chunk 3720/4437 written (239 bytes)
✓ Chunk 3721/4437 written (239 bytes)
✓ Chunk 3722/4437 written (239 bytes)
✓ Chunk 3723/4437 written (239 bytes)
✓ Chunk 3724/4437 written (239 bytes)
✓ Chunk 3725/4437 written (239 bytes)
✓ Chunk 3726/4437 written (239 bytes)
✓ Chunk 3727/4437 written (239 bytes)
✓ Chunk 3728/4437 written (239 bytes)
✓ Chunk 3729/4437 written (239 bytes)
✓ Chunk 3730/4437 written (239 bytes)
✓ Chunk 3731/4437 written (239 bytes)
✓ Chunk 3732/4437 written (239 bytes)
✓ Chunk 3733/4437 written (239 bytes)
✓ Chunk 3734/4437 written (239 bytes)
✓ Chunk 3735/4437 written (239 bytes)
✓ Chunk 3736/4437 written (239 bytes)
✓ Chunk 3737/4437 written (239 bytes)
✓ Chunk 3738/4437 written (239 bytes)
✓ Chunk 3739/4437 written (239 bytes)
✓ Chunk 3740/4437 written (239 bytes)
✓ Chunk 3741/4437 written (239 bytes)
✓ Chunk 3742/4437 written (239 bytes)
✓ Chunk 3743/4437 written (239 bytes)
✓ Chunk 3744/4437 written (239 bytes)
✓ Chunk 3745/4437 written (239 bytes)
✓ Chunk 3746/4437 written (239 bytes)
✓ Chunk 3747/4437 written (239 bytes)
✓ Chunk 3748/4437 written (239 bytes)
✓ Chunk 3749/4437 written (239 bytes)
✓ Chunk 3750/4437 written (239 bytes)
✓ Chunk 3751/4437 written (239 bytes)
✓ Chunk 3752/4437 written (239 bytes)
✓ Chunk 3753/4437 written (239 bytes)
✓ Chunk 3754/4437 written (239 bytes)
✓ Chunk 3755/4437 written (239 bytes)
✓ Chunk 3756/4437 written (239 bytes)
✓ Chunk 3757/4437 written (239 bytes)
✓ Chunk 3758/4437 written (239 bytes)
✓ Chunk 3759/4437 written (239 bytes)
✓ Chunk 3760/4437 written (239 bytes)
✓ Chunk 3761/4437 written (239 bytes)
✓ Chunk 3762/4437 written (239 bytes)
✓ Chunk 3763/4437 written (239 bytes)
✓ Chunk 3764/4437 written (239 bytes)
✓ Chunk 3765/4437 written (239 bytes)
✓ Chunk 3766/4437 written (239 bytes)
✓ Chunk 3767/4437 written (239 bytes)
✓ Chunk 3768/4437 written (239 bytes)
✓ Chunk 3769/4437 written (239 bytes)
✓ Chunk 3770/4437 written (239 bytes)
✓ Chunk 3771/4437 written (239 bytes)
✓ Chunk 3772/4437 written (239 bytes)
✓ Chunk 3773/4437 written (239 bytes)
✓ Chunk 3774/4437 written (239 bytes)
✓ Chunk 3775/4437 written (239 bytes)
✓ Chunk 3776/4437 written (239 bytes)
✓ Chunk 3777/4437 written (239 bytes)
✓ Chunk 3778/4437 written (239 bytes)
✓ Chunk 3779/4437 written (239 bytes)
✓ Chunk 3780/4437 written (239 bytes)
✓ Chunk 3781/4437 written (239 bytes)
✓ Chunk 3782/4437 written (239 bytes)
✓ Chunk 3783/4437 written (239 bytes)
✓ Chunk 3784/4437 written (239 bytes)
✓ Chunk 3785/4437 written (239 bytes)
✓ Chunk 3786/4437 written (239 bytes)
✓ Chunk 3787/4437 written (239 bytes)
✓ Chunk 3788/4437 written (239 bytes)
✓ Chunk 3789/4437 written (239 bytes)
✓ Chunk 3790/4437 written (239 bytes)
✓ Chunk 3791/4437 written (239 bytes)
✓ Chunk 3792/4437 written (239 bytes)
✓ Chunk 3793/4437 written (239 bytes)
✓ Chunk 3794/4437 written (239 bytes)
✓ Chunk 3795/4437 written (239 bytes)
✓ Chunk 3796/4437 written (239 bytes)
✓ Chunk 3797/4437 written (239 bytes)
✓ Chunk 3798/4437 written (239 bytes)
✓ Chunk 3799/4437 written (239 bytes)
✓ Chunk 3800/4437 written (239 bytes)
  [3800/4437] Chunk streamed (seq=3800)
✓ Chunk 3801/4437 written (239 bytes)
✓ Chunk 3802/4437 written (239 bytes)
✓ Chunk 3803/4437 written (239 bytes)
✓ Chunk 3804/4437 written (239 bytes)
✓ Chunk 3805/4437 written (239 bytes)
✓ Chunk 3806/4437 written (239 bytes)
✓ Chunk 3807/4437 written (239 bytes)
✓ Chunk 3808/4437 written (239 bytes)
✓ Chunk 3809/4437 written (239 bytes)
✓ Chunk 3810/4437 written (239 bytes)
✓ Chunk 3811/4437 written (239 bytes)
✓ Chunk 3812/4437 written (239 bytes)
✓ Chunk 3813/4437 written (239 bytes)
✓ Chunk 3814/4437 written (239 bytes)
✓ Chunk 3815/4437 written (239 bytes)
✓ Chunk 3816/4437 written (239 bytes)
✓ Chunk 3817/4437 written (239 bytes)
✓ Chunk 3818/4437 written (239 bytes)
✓ Chunk 3819/4437 written (239 bytes)
✓ Chunk 3820/4437 written (239 bytes)
✓ Chunk 3821/4437 written (239 bytes)
✓ Chunk 3822/4437 written (239 bytes)
✓ Chunk 3823/4437 written (239 bytes)
✓ Chunk 3824/4437 written (239 bytes)
✓ Chunk 3825/4437 written (239 bytes)
✓ Chunk 3826/4437 written (239 bytes)
✓ Chunk 3827/4437 written (239 bytes)
✓ Chunk 3828/4437 written (239 bytes)
✓ Chunk 3829/4437 written (239 bytes)
✓ Chunk 3830/4437 written (239 bytes)
✓ Chunk 3831/4437 written (239 bytes)
✓ Chunk 3832/4437 written (239 bytes)
✓ Chunk 3833/4437 written (239 bytes)
✓ Chunk 3834/4437 written (239 bytes)
✓ Chunk 3835/4437 written (239 bytes)
✓ Chunk 3836/4437 written (239 bytes)
✓ Chunk 3837/4437 written (239 bytes)
✓ Chunk 3838/4437 written (239 bytes)
✓ Chunk 3839/4437 written (239 bytes)
✓ Chunk 3840/4437 written (239 bytes)
✓ Chunk 3841/4437 written (239 bytes)
✓ Chunk 3842/4437 written (239 bytes)
✓ Chunk 3843/4437 written (239 bytes)
✓ Chunk 3844/4437 written (239 bytes)
✓ Chunk 3845/4437 written (239 bytes)
✓ Chunk 3846/4437 written (239 bytes)
✓ Chunk 3847/4437 written (239 bytes)
✓ Chunk 3848/4437 written (239 bytes)
✓ Chunk 3849/4437 written (239 bytes)
✓ Chunk 3850/4437 written (239 bytes)
✓ Chunk 3851/4437 written (239 bytes)
✓ Chunk 3852/4437 written (239 bytes)
✓ Chunk 3853/4437 written (239 bytes)
✓ Chunk 3854/4437 written (239 bytes)
✓ Chunk 3855/4437 written (239 bytes)
✓ Chunk 3856/4437 written (239 bytes)
✓ Chunk 3857/4437 written (239 bytes)
✓ Chunk 3858/4437 written (239 bytes)
✓ Chunk 3859/4437 written (239 bytes)
✓ Chunk 3860/4437 written (239 bytes)
✓ Chunk 3861/4437 written (239 bytes)
✓ Chunk 3862/4437 written (239 bytes)
✓ Chunk 3863/4437 written (239 bytes)
✓ Chunk 3864/4437 written (239 bytes)
✓ Chunk 3865/4437 written (239 bytes)
✓ Chunk 3866/4437 written (239 bytes)
✓ Chunk 3867/4437 written (239 bytes)
✓ Chunk 3868/4437 written (239 bytes)
✓ Chunk 3869/4437 written (239 bytes)
✓ Chunk 3870/4437 written (239 bytes)
✓ Chunk 3871/4437 written (239 bytes)
✓ Chunk 3872/4437 written (239 bytes)
✓ Chunk 3873/4437 written (239 bytes)
✓ Chunk 3874/4437 written (239 bytes)
✓ Chunk 3875/4437 written (239 bytes)
✓ Chunk 3876/4437 written (239 bytes)
✓ Chunk 3877/4437 written (239 bytes)
✓ Chunk 3878/4437 written (239 bytes)
✓ Chunk 3879/4437 written (239 bytes)
✓ Chunk 3880/4437 written (239 bytes)
✓ Chunk 3881/4437 written (239 bytes)
✓ Chunk 3882/4437 written (239 bytes)
✓ Chunk 3883/4437 written (239 bytes)
✓ Chunk 3884/4437 written (239 bytes)
✓ Chunk 3885/4437 written (239 bytes)
✓ Chunk 3886/4437 written (239 bytes)
✓ Chunk 3887/4437 written (239 bytes)
✓ Chunk 3888/4437 written (239 bytes)
✓ Chunk 3889/4437 written (239 bytes)
✓ Chunk 3890/4437 written (239 bytes)
✓ Chunk 3891/4437 written (239 bytes)
✓ Chunk 3892/4437 written (239 bytes)
✓ Chunk 3893/4437 written (239 bytes)
✓ Chunk 3894/4437 written (239 bytes)
✓ Chunk 3895/4437 written (239 bytes)
✓ Chunk 3896/4437 written (239 bytes)
✓ Chunk 3897/4437 written (239 bytes)
✓ Chunk 3898/4437 written (239 bytes)
✓ Chunk 3899/4437 written (239 bytes)
✓ Chunk 3900/4437 written (239 bytes)
  [3900/4437] Chunk streamed (seq=3900)
✓ Chunk 3901/4437 written (239 bytes)
✓ Chunk 3902/4437 written (239 bytes)
✓ Chunk 3903/4437 written (239 bytes)
✓ Chunk 3904/4437 written (239 bytes)
✓ Chunk 3905/4437 written (239 bytes)
✓ Chunk 3906/4437 written (239 bytes)
✓ Chunk 3907/4437 written (239 bytes)
✓ Chunk 3908/4437 written (239 bytes)
✓ Chunk 3909/4437 written (239 bytes)
✓ Chunk 3910/4437 written (239 bytes)
✓ Chunk 3911/4437 written (239 bytes)
✓ Chunk 3912/4437 written (239 bytes)
✓ Chunk 3913/4437 written (239 bytes)
✓ Chunk 3914/4437 written (239 bytes)
✓ Chunk 3915/4437 written (239 bytes)
✓ Chunk 3916/4437 written (239 bytes)
✓ Chunk 3917/4437 written (239 bytes)
✓ Chunk 3918/4437 written (239 bytes)
✓ Chunk 3919/4437 written (239 bytes)
✓ Chunk 3920/4437 written (239 bytes)
✓ Chunk 3921/4437 written (239 bytes)
✓ Chunk 3922/4437 written (239 bytes)
✓ Chunk 3923/4437 written (239 bytes)
✓ Chunk 3924/4437 written (239 bytes)
✓ Chunk 3925/4437 written (239 bytes)
✓ Chunk 3926/4437 written (239 bytes)
✓ Chunk 3927/4437 written (239 bytes)
✓ Chunk 3928/4437 written (239 bytes)
✓ Chunk 3929/4437 written (239 bytes)
✓ Chunk 3930/4437 written (239 bytes)
✓ Chunk 3931/4437 written (239 bytes)
✓ Chunk 3932/4437 written (239 bytes)
✓ Chunk 3933/4437 written (239 bytes)
✓ Chunk 3934/4437 written (239 bytes)
✓ Chunk 3935/4437 written (239 bytes)
✓ Chunk 3936/4437 written (239 bytes)
✓ Chunk 3937/4437 written (239 bytes)
✓ Chunk 3938/4437 written (239 bytes)
✓ Chunk 3939/4437 written (239 bytes)
✓ Chunk 3940/4437 written (239 bytes)
✓ Chunk 3941/4437 written (239 bytes)
✓ Chunk 3942/4437 written (239 bytes)
✓ Chunk 3943/4437 written (239 bytes)
✓ Chunk 3944/4437 written (239 bytes)
✓ Chunk 3945/4437 written (239 bytes)
✓ Chunk 3946/4437 written (239 bytes)
✓ Chunk 3947/4437 written (239 bytes)
✓ Chunk 3948/4437 written (239 bytes)
✓ Chunk 3949/4437 written (239 bytes)
✓ Chunk 3950/4437 written (239 bytes)
✓ Chunk 3951/4437 written (239 bytes)
✓ Chunk 3952/4437 written (239 bytes)
✓ Chunk 3953/4437 written (239 bytes)
✓ Chunk 3954/4437 written (239 bytes)
✓ Chunk 3955/4437 written (239 bytes)
✓ Chunk 3956/4437 written (239 bytes)
✓ Chunk 3957/4437 written (239 bytes)
✓ Chunk 3958/4437 written (239 bytes)
✓ Chunk 3959/4437 written (239 bytes)
✓ Chunk 3960/4437 written (239 bytes)
✓ Chunk 3961/4437 written (239 bytes)
✓ Chunk 3962/4437 written (239 bytes)
✓ Chunk 3963/4437 written (239 bytes)
✓ Chunk 3964/4437 written (239 bytes)
✓ Chunk 3965/4437 written (239 bytes)
✓ Chunk 3966/4437 written (239 bytes)
✓ Chunk 3967/4437 written (239 bytes)
✓ Chunk 3968/4437 written (239 bytes)
✓ Chunk 3969/4437 written (239 bytes)
✓ Chunk 3970/4437 written (239 bytes)
✓ Chunk 3971/4437 written (239 bytes)
✓ Chunk 3972/4437 written (239 bytes)
✓ Chunk 3973/4437 written (239 bytes)
✓ Chunk 3974/4437 written (239 bytes)
✓ Chunk 3975/4437 written (239 bytes)
✓ Chunk 3976/4437 written (239 bytes)
✓ Chunk 3977/4437 written (239 bytes)
✓ Chunk 3978/4437 written (239 bytes)
✓ Chunk 3979/4437 written (239 bytes)
✓ Chunk 3980/4437 written (239 bytes)
✓ Chunk 3981/4437 written (239 bytes)
✓ Chunk 3982/4437 written (239 bytes)
✓ Chunk 3983/4437 written (239 bytes)
✓ Chunk 3984/4437 written (239 bytes)
✓ Chunk 3985/4437 written (239 bytes)
✓ Chunk 3986/4437 written (239 bytes)
✓ Chunk 3987/4437 written (239 bytes)
✓ Chunk 3988/4437 written (239 bytes)
✓ Chunk 3989/4437 written (239 bytes)
✓ Chunk 3990/4437 written (239 bytes)
✓ Chunk 3991/4437 written (239 bytes)
✓ Chunk 3992/4437 written (239 bytes)
✓ Chunk 3993/4437 written (239 bytes)
✓ Chunk 3994/4437 written (239 bytes)
✓ Chunk 3995/4437 written (239 bytes)
✓ Chunk 3996/4437 written (239 bytes)
✓ Chunk 3997/4437 written (239 bytes)
✓ Chunk 3998/4437 written (239 bytes)
✓ Chunk 3999/4437 written (239 bytes)
✓ Chunk 4000/4437 written (239 bytes)
  [4000/4437] Chunk streamed (seq=4000)
✓ Chunk 4001/4437 written (239 bytes)
✓ Chunk 4002/4437 written (239 bytes)
✓ Chunk 4003/4437 written (239 bytes)
✓ Chunk 4004/4437 written (239 bytes)
✓ Chunk 4005/4437 written (239 bytes)
✓ Chunk 4006/4437 written (239 bytes)
✓ Chunk 4007/4437 written (239 bytes)
✓ Chunk 4008/4437 written (239 bytes)
✓ Chunk 4009/4437 written (239 bytes)
✓ Chunk 4010/4437 written (239 bytes)
✓ Chunk 4011/4437 written (239 bytes)
✓ Chunk 4012/4437 written (239 bytes)
✓ Chunk 4013/4437 written (239 bytes)
✓ Chunk 4014/4437 written (239 bytes)
✓ Chunk 4015/4437 written (239 bytes)
✓ Chunk 4016/4437 written (239 bytes)
✓ Chunk 4017/4437 written (239 bytes)
✓ Chunk 4018/4437 written (239 bytes)
✓ Chunk 4019/4437 written (239 bytes)
✓ Chunk 4020/4437 written (239 bytes)
✓ Chunk 4021/4437 written (239 bytes)
✓ Chunk 4022/4437 written (239 bytes)
✓ Chunk 4023/4437 written (239 bytes)
✓ Chunk 4024/4437 written (239 bytes)
✓ Chunk 4025/4437 written (239 bytes)
✓ Chunk 4026/4437 written (239 bytes)
✓ Chunk 4027/4437 written (239 bytes)
✓ Chunk 4028/4437 written (239 bytes)
✓ Chunk 4029/4437 written (239 bytes)
✓ Chunk 4030/4437 written (239 bytes)
✓ Chunk 4031/4437 written (239 bytes)
✓ Chunk 4032/4437 written (239 bytes)
✓ Chunk 4033/4437 written (239 bytes)
✓ Chunk 4034/4437 written (239 bytes)
✓ Chunk 4035/4437 written (239 bytes)
✓ Chunk 4036/4437 written (239 bytes)
✓ Chunk 4037/4437 written (239 bytes)
✓ Chunk 4038/4437 written (239 bytes)
✓ Chunk 4039/4437 written (239 bytes)
✓ Chunk 4040/4437 written (239 bytes)
✓ Chunk 4041/4437 written (239 bytes)
✓ Chunk 4042/4437 written (239 bytes)
✓ Chunk 4043/4437 written (239 bytes)
✓ Chunk 4044/4437 written (239 bytes)
✓ Chunk 4045/4437 written (239 bytes)
✓ Chunk 4046/4437 written (239 bytes)
✓ Chunk 4047/4437 written (239 bytes)
✓ Chunk 4048/4437 written (239 bytes)
✓ Chunk 4049/4437 written (239 bytes)
✓ Chunk 4050/4437 written (239 bytes)
✓ Chunk 4051/4437 written (239 bytes)
✓ Chunk 4052/4437 written (239 bytes)
✓ Chunk 4053/4437 written (239 bytes)
✓ Chunk 4054/4437 written (239 bytes)
✓ Chunk 4055/4437 written (239 bytes)
✓ Chunk 4056/4437 written (239 bytes)
✓ Chunk 4057/4437 written (239 bytes)
✓ Chunk 4058/4437 written (239 bytes)
✓ Chunk 4059/4437 written (239 bytes)
✓ Chunk 4060/4437 written (239 bytes)
✓ Chunk 4061/4437 written (239 bytes)
✓ Chunk 4062/4437 written (239 bytes)
✓ Chunk 4063/4437 written (239 bytes)
✓ Chunk 4064/4437 written (239 bytes)
✓ Chunk 4065/4437 written (239 bytes)
✓ Chunk 4066/4437 written (239 bytes)
✓ Chunk 4067/4437 written (239 bytes)
✓ Chunk 4068/4437 written (239 bytes)
✓ Chunk 4069/4437 written (239 bytes)
✓ Chunk 4070/4437 written (239 bytes)
✓ Chunk 4071/4437 written (239 bytes)
✓ Chunk 4072/4437 written (239 bytes)
✓ Chunk 4073/4437 written (239 bytes)
✓ Chunk 4074/4437 written (239 bytes)
✓ Chunk 4075/4437 written (239 bytes)
✓ Chunk 4076/4437 written (239 bytes)
✓ Chunk 4077/4437 written (239 bytes)
✓ Chunk 4078/4437 written (239 bytes)
✓ Chunk 4079/4437 written (239 bytes)
✓ Chunk 4080/4437 written (239 bytes)
✓ Chunk 4081/4437 written (239 bytes)
✓ Chunk 4082/4437 written (239 bytes)
✓ Chunk 4083/4437 written (239 bytes)
✓ Chunk 4084/4437 written (239 bytes)
✓ Chunk 4085/4437 written (239 bytes)
✓ Chunk 4086/4437 written (239 bytes)
✓ Chunk 4087/4437 written (239 bytes)
✓ Chunk 4088/4437 written (239 bytes)
✓ Chunk 4089/4437 written (239 bytes)
✓ Chunk 4090/4437 written (239 bytes)
✓ Chunk 4091/4437 written (239 bytes)
✓ Chunk 4092/4437 written (239 bytes)
✓ Chunk 4093/4437 written (239 bytes)
✓ Chunk 4094/4437 written (239 bytes)
✓ Chunk 4095/4437 written (239 bytes)
✓ Chunk 4096/4437 written (239 bytes)
✓ Chunk 4097/4437 written (239 bytes)
✓ Chunk 4098/4437 written (239 bytes)
✓ Chunk 4099/4437 written (239 bytes)
✓ Chunk 4100/4437 written (239 bytes)
  [4100/4437] Chunk streamed (seq=4100)
✓ Chunk 4101/4437 written (239 bytes)
✓ Chunk 4102/4437 written (239 bytes)
✓ Chunk 4103/4437 written (239 bytes)
✓ Chunk 4104/4437 written (239 bytes)
✓ Chunk 4105/4437 written (239 bytes)
✓ Chunk 4106/4437 written (239 bytes)
✓ Chunk 4107/4437 written (239 bytes)
✓ Chunk 4108/4437 written (239 bytes)
✓ Chunk 4109/4437 written (239 bytes)
✓ Chunk 4110/4437 written (239 bytes)
✓ Chunk 4111/4437 written (239 bytes)
✓ Chunk 4112/4437 written (239 bytes)
✓ Chunk 4113/4437 written (239 bytes)
✓ Chunk 4114/4437 written (239 bytes)
✓ Chunk 4115/4437 written (239 bytes)
✓ Chunk 4116/4437 written (239 bytes)
✓ Chunk 4117/4437 written (239 bytes)
✓ Chunk 4118/4437 written (239 bytes)
✓ Chunk 4119/4437 written (239 bytes)
✓ Chunk 4120/4437 written (239 bytes)
✓ Chunk 4121/4437 written (239 bytes)
✓ Chunk 4122/4437 written (239 bytes)
✓ Chunk 4123/4437 written (239 bytes)
✓ Chunk 4124/4437 written (239 bytes)
✓ Chunk 4125/4437 written (239 bytes)
✓ Chunk 4126/4437 written (239 bytes)
✓ Chunk 4127/4437 written (239 bytes)
✓ Chunk 4128/4437 written (239 bytes)
✓ Chunk 4129/4437 written (239 bytes)
✓ Chunk 4130/4437 written (239 bytes)
✓ Chunk 4131/4437 written (239 bytes)
✓ Chunk 4132/4437 written (239 bytes)
✓ Chunk 4133/4437 written (239 bytes)
✓ Chunk 4134/4437 written (239 bytes)
✓ Chunk 4135/4437 written (239 bytes)
✓ Chunk 4136/4437 written (239 bytes)
✓ Chunk 4137/4437 written (239 bytes)
✓ Chunk 4138/4437 written (239 bytes)
✓ Chunk 4139/4437 written (239 bytes)
✓ Chunk 4140/4437 written (239 bytes)
✓ Chunk 4141/4437 written (239 bytes)
✓ Chunk 4142/4437 written (239 bytes)
✓ Chunk 4143/4437 written (239 bytes)
✓ Chunk 4144/4437 written (239 bytes)
✓ Chunk 4145/4437 written (239 bytes)
✓ Chunk 4146/4437 written (239 bytes)
✓ Chunk 4147/4437 written (239 bytes)
✓ Chunk 4148/4437 written (239 bytes)
✓ Chunk 4149/4437 written (239 bytes)
✓ Chunk 4150/4437 written (239 bytes)
✓ Chunk 4151/4437 written (239 bytes)
✓ Chunk 4152/4437 written (239 bytes)
✓ Chunk 4153/4437 written (239 bytes)
✓ Chunk 4154/4437 written (239 bytes)
✓ Chunk 4155/4437 written (239 bytes)
✓ Chunk 4156/4437 written (239 bytes)
✓ Chunk 4157/4437 written (239 bytes)
✓ Chunk 4158/4437 written (239 bytes)
✓ Chunk 4159/4437 written (239 bytes)
✓ Chunk 4160/4437 written (239 bytes)
✓ Chunk 4161/4437 written (239 bytes)
✓ Chunk 4162/4437 written (239 bytes)
✓ Chunk 4163/4437 written (239 bytes)
✓ Chunk 4164/4437 written (239 bytes)
✓ Chunk 4165/4437 written (239 bytes)
✓ Chunk 4166/4437 written (239 bytes)
✓ Chunk 4167/4437 written (239 bytes)
✓ Chunk 4168/4437 written (239 bytes)
✓ Chunk 4169/4437 written (239 bytes)
✓ Chunk 4170/4437 written (239 bytes)
✓ Chunk 4171/4437 written (239 bytes)
✓ Chunk 4172/4437 written (239 bytes)
✓ Chunk 4173/4437 written (239 bytes)
✓ Chunk 4174/4437 written (239 bytes)
✓ Chunk 4175/4437 written (239 bytes)
✓ Chunk 4176/4437 written (239 bytes)
✓ Chunk 4177/4437 written (239 bytes)
✓ Chunk 4178/4437 written (239 bytes)
✓ Chunk 4179/4437 written (239 bytes)
✓ Chunk 4180/4437 written (239 bytes)
✓ Chunk 4181/4437 written (239 bytes)
✓ Chunk 4182/4437 written (239 bytes)
✓ Chunk 4183/4437 written (239 bytes)
✓ Chunk 4184/4437 written (239 bytes)
✓ Chunk 4185/4437 written (239 bytes)
✓ Chunk 4186/4437 written (239 bytes)
✓ Chunk 4187/4437 written (239 bytes)
✓ Chunk 4188/4437 written (239 bytes)
✓ Chunk 4189/4437 written (239 bytes)
✓ Chunk 4190/4437 written (239 bytes)
✓ Chunk 4191/4437 written (239 bytes)
✓ Chunk 4192/4437 written (239 bytes)
✓ Chunk 4193/4437 written (239 bytes)
✓ Chunk 4194/4437 written (239 bytes)
✓ Chunk 4195/4437 written (239 bytes)
✓ Chunk 4196/4437 written (239 bytes)
✓ Chunk 4197/4437 written (239 bytes)
✓ Chunk 4198/4437 written (239 bytes)
✓ Chunk 4199/4437 written (239 bytes)
✓ Chunk 4200/4437 written (239 bytes)
  [4200/4437] Chunk streamed (seq=4200)
✓ Chunk 4201/4437 written (239 bytes)
✓ Chunk 4202/4437 written (239 bytes)
✓ Chunk 4203/4437 written (239 bytes)
✓ Chunk 4204/4437 written (239 bytes)
✓ Chunk 4205/4437 written (239 bytes)
✓ Chunk 4206/4437 written (239 bytes)
✓ Chunk 4207/4437 written (239 bytes)
✓ Chunk 4208/4437 written (239 bytes)
✓ Chunk 4209/4437 written (239 bytes)
✓ Chunk 4210/4437 written (239 bytes)
✓ Chunk 4211/4437 written (239 bytes)
✓ Chunk 4212/4437 written (239 bytes)
✓ Chunk 4213/4437 written (239 bytes)
✓ Chunk 4214/4437 written (239 bytes)
✓ Chunk 4215/4437 written (239 bytes)
✓ Chunk 4216/4437 written (239 bytes)
✓ Chunk 4217/4437 written (239 bytes)
✓ Chunk 4218/4437 written (239 bytes)
✓ Chunk 4219/4437 written (239 bytes)
✓ Chunk 4220/4437 written (239 bytes)
✓ Chunk 4221/4437 written (239 bytes)
✓ Chunk 4222/4437 written (239 bytes)
✓ Chunk 4223/4437 written (239 bytes)
✓ Chunk 4224/4437 written (239 bytes)
✓ Chunk 4225/4437 written (239 bytes)
✓ Chunk 4226/4437 written (239 bytes)
✓ Chunk 4227/4437 written (239 bytes)
✓ Chunk 4228/4437 written (239 bytes)
✓ Chunk 4229/4437 written (239 bytes)
✓ Chunk 4230/4437 written (239 bytes)
✓ Chunk 4231/4437 written (239 bytes)
✓ Chunk 4232/4437 written (239 bytes)
✓ Chunk 4233/4437 written (239 bytes)
✓ Chunk 4234/4437 written (239 bytes)
✓ Chunk 4235/4437 written (239 bytes)
✓ Chunk 4236/4437 written (239 bytes)
✓ Chunk 4237/4437 written (239 bytes)
✓ Chunk 4238/4437 written (239 bytes)
✓ Chunk 4239/4437 written (239 bytes)
✓ Chunk 4240/4437 written (239 bytes)
✓ Chunk 4241/4437 written (239 bytes)
✓ Chunk 4242/4437 written (239 bytes)
✓ Chunk 4243/4437 written (239 bytes)
✓ Chunk 4244/4437 written (239 bytes)
✓ Chunk 4245/4437 written (239 bytes)
✓ Chunk 4246/4437 written (239 bytes)
✓ Chunk 4247/4437 written (239 bytes)
✓ Chunk 4248/4437 written (239 bytes)
✓ Chunk 4249/4437 written (239 bytes)
✓ Chunk 4250/4437 written (239 bytes)
✓ Chunk 4251/4437 written (239 bytes)
✓ Chunk 4252/4437 written (239 bytes)
✓ Chunk 4253/4437 written (239 bytes)
✓ Chunk 4254/4437 written (239 bytes)
✓ Chunk 4255/4437 written (239 bytes)
✓ Chunk 4256/4437 written (239 bytes)
✓ Chunk 4257/4437 written (239 bytes)
✓ Chunk 4258/4437 written (239 bytes)
✓ Chunk 4259/4437 written (239 bytes)
✓ Chunk 4260/4437 written (239 bytes)
✓ Chunk 4261/4437 written (239 bytes)
✓ Chunk 4262/4437 written (239 bytes)
✓ Chunk 4263/4437 written (239 bytes)
✓ Chunk 4264/4437 written (239 bytes)
✓ Chunk 4265/4437 written (239 bytes)
✓ Chunk 4266/4437 written (239 bytes)
✓ Chunk 4267/4437 written (239 bytes)
✓ Chunk 4268/4437 written (239 bytes)
✓ Chunk 4269/4437 written (239 bytes)
✓ Chunk 4270/4437 written (239 bytes)
✓ Chunk 4271/4437 written (239 bytes)
✓ Chunk 4272/4437 written (239 bytes)
✓ Chunk 4273/4437 written (239 bytes)
✓ Chunk 4274/4437 written (239 bytes)
✓ Chunk 4275/4437 written (239 bytes)
✓ Chunk 4276/4437 written (239 bytes)
✓ Chunk 4277/4437 written (239 bytes)
✓ Chunk 4278/4437 written (239 bytes)
✓ Chunk 4279/4437 written (239 bytes)
✓ Chunk 4280/4437 written (239 bytes)
✓ Chunk 4281/4437 written (239 bytes)
✓ Chunk 4282/4437 written (239 bytes)
✓ Chunk 4283/4437 written (239 bytes)
✓ Chunk 4284/4437 written (239 bytes)
✓ Chunk 4285/4437 written (239 bytes)
✓ Chunk 4286/4437 written (239 bytes)
✓ Chunk 4287/4437 written (239 bytes)
✓ Chunk 4288/4437 written (239 bytes)
✓ Chunk 4289/4437 written (239 bytes)
✓ Chunk 4290/4437 written (239 bytes)
✓ Chunk 4291/4437 written (239 bytes)
✓ Chunk 4292/4437 written (239 bytes)
✓ Chunk 4293/4437 written (239 bytes)
✓ Chunk 4294/4437 written (239 bytes)
✓ Chunk 4295/4437 written (239 bytes)
✓ Chunk 4296/4437 written (239 bytes)
✓ Chunk 4297/4437 written (239 bytes)
✓ Chunk 4298/4437 written (239 bytes)
✓ Chunk 4299/4437 written (239 bytes)
✓ Chunk 4300/4437 written (239 bytes)
  [4300/4437] Chunk streamed (seq=4300)
✓ Chunk 4301/4437 written (239 bytes)
✓ Chunk 4302/4437 written (239 bytes)
✓ Chunk 4303/4437 written (239 bytes)
✓ Chunk 4304/4437 written (239 bytes)
✓ Chunk 4305/4437 written (239 bytes)
✓ Chunk 4306/4437 written (239 bytes)
✓ Chunk 4307/4437 written (239 bytes)
✓ Chunk 4308/4437 written (239 bytes)
✓ Chunk 4309/4437 written (239 bytes)
✓ Chunk 4310/4437 written (239 bytes)
✓ Chunk 4311/4437 written (239 bytes)
✓ Chunk 4312/4437 written (239 bytes)
✓ Chunk 4313/4437 written (239 bytes)
✓ Chunk 4314/4437 written (239 bytes)
✓ Chunk 4315/4437 written (239 bytes)
✓ Chunk 4316/4437 written (239 bytes)
✓ Chunk 4317/4437 written (239 bytes)
✓ Chunk 4318/4437 written (239 bytes)
✓ Chunk 4319/4437 written (239 bytes)
✓ Chunk 4320/4437 written (239 bytes)
✓ Chunk 4321/4437 written (239 bytes)
✓ Chunk 4322/4437 written (239 bytes)
✓ Chunk 4323/4437 written (239 bytes)
✓ Chunk 4324/4437 written (239 bytes)
✓ Chunk 4325/4437 written (239 bytes)
✓ Chunk 4326/4437 written (239 bytes)
✓ Chunk 4327/4437 written (239 bytes)
✓ Chunk 4328/4437 written (239 bytes)
✓ Chunk 4329/4437 written (239 bytes)
✓ Chunk 4330/4437 written (239 bytes)
✓ Chunk 4331/4437 written (239 bytes)
✓ Chunk 4332/4437 written (239 bytes)
✓ Chunk 4333/4437 written (239 bytes)
✓ Chunk 4334/4437 written (239 bytes)
✓ Chunk 4335/4437 written (239 bytes)
✓ Chunk 4336/4437 written (239 bytes)
✓ Chunk 4337/4437 written (239 bytes)
✓ Chunk 4338/4437 written (239 bytes)
✓ Chunk 4339/4437 written (239 bytes)
✓ Chunk 4340/4437 written (239 bytes)
✓ Chunk 4341/4437 written (239 bytes)
✓ Chunk 4342/4437 written (239 bytes)
✓ Chunk 4343/4437 written (239 bytes)
✓ Chunk 4344/4437 written (239 bytes)
✓ Chunk 4345/4437 written (239 bytes)
✓ Chunk 4346/4437 written (239 bytes)
✓ Chunk 4347/4437 written (239 bytes)
✓ Chunk 4348/4437 written (239 bytes)
✓ Chunk 4349/4437 written (239 bytes)
✓ Chunk 4350/4437 written (239 bytes)
✓ Chunk 4351/4437 written (239 bytes)
✓ Chunk 4352/4437 written (239 bytes)
✓ Chunk 4353/4437 written (239 bytes)
✓ Chunk 4354/4437 written (239 bytes)
✓ Chunk 4355/4437 written (239 bytes)
✓ Chunk 4356/4437 written (239 bytes)
✓ Chunk 4357/4437 written (239 bytes)
✓ Chunk 4358/4437 written (239 bytes)
✓ Chunk 4359/4437 written (239 bytes)
✓ Chunk 4360/4437 written (239 bytes)
✓ Chunk 4361/4437 written (239 bytes)
✓ Chunk 4362/4437 written (239 bytes)
✓ Chunk 4363/4437 written (239 bytes)
✓ Chunk 4364/4437 written (239 bytes)
✓ Chunk 4365/4437 written (239 bytes)
✓ Chunk 4366/4437 written (239 bytes)
✓ Chunk 4367/4437 written (239 bytes)
✓ Chunk 4368/4437 written (239 bytes)
✓ Chunk 4369/4437 written (239 bytes)
✓ Chunk 4370/4437 written (239 bytes)
✓ Chunk 4371/4437 written (239 bytes)
✓ Chunk 4372/4437 written (239 bytes)
✓ Chunk 4373/4437 written (239 bytes)
✓ Chunk 4374/4437 written (239 bytes)
✓ Chunk 4375/4437 written (239 bytes)
✓ Chunk 4376/4437 written (239 bytes)
✓ Chunk 4377/4437 written (239 bytes)
✓ Chunk 4378/4437 written (239 bytes)
✓ Chunk 4379/4437 written (239 bytes)
✓ Chunk 4380/4437 written (239 bytes)
✓ Chunk 4381/4437 written (239 bytes)
✓ Chunk 4382/4437 written (239 bytes)
✓ Chunk 4383/4437 written (239 bytes)
✓ Chunk 4384/4437 written (239 bytes)
✓ Chunk 4385/4437 written (239 bytes)
✓ Chunk 4386/4437 written (239 bytes)
✓ Chunk 4387/4437 written (239 bytes)
✓ Chunk 4388/4437 written (239 bytes)
✓ Chunk 4389/4437 written (239 bytes)
✓ Chunk 4390/4437 written (239 bytes)
✓ Chunk 4391/4437 written (239 bytes)
✓ Chunk 4392/4437 written (239 bytes)
✓ Chunk 4393/4437 written (239 bytes)
✓ Chunk 4394/4437 written (239 bytes)
✓ Chunk 4395/4437 written (239 bytes)
✓ Chunk 4396/4437 written (239 bytes)
✓ Chunk 4397/4437 written (239 bytes)
✓ Chunk 4398/4437 written (239 bytes)
✓ Chunk 4399/4437 written (239 bytes)
✓ Chunk 4400/4437 written (239 bytes)
  [4400/4437] Chunk streamed (seq=4400)
✓ Chunk 4401/4437 written (239 bytes)
✓ Chunk 4402/4437 written (239 bytes)
✓ Chunk 4403/4437 written (239 bytes)
✓ Chunk 4404/4437 written (239 bytes)
✓ Chunk 4405/4437 written (239 bytes)
✓ Chunk 4406/4437 written (239 bytes)
✓ Chunk 4407/4437 written (239 bytes)
✓ Chunk 4408/4437 written (239 bytes)
✓ Chunk 4409/4437 written (239 bytes)
✓ Chunk 4410/4437 written (239 bytes)
✓ Chunk 4411/4437 written (239 bytes)
✓ Chunk 4412/4437 written (239 bytes)
✓ Chunk 4413/4437 written (239 bytes)
✓ Chunk 4414/4437 written (239 bytes)
✓ Chunk 4415/4437 written (239 bytes)
✓ Chunk 4416/4437 written (239 bytes)
✓ Chunk 4417/4437 written (239 bytes)
✓ Chunk 4418/4437 written (239 bytes)
✓ Chunk 4419/4437 written (239 bytes)
✓ Chunk 4420/4437 written (239 bytes)
✓ Chunk 4421/4437 written (239 bytes)
✓ Chunk 4422/4437 written (239 bytes)
✓ Chunk 4423/4437 written (239 bytes)
✓ Chunk 4424/4437 written (239 bytes)
✓ Chunk 4425/4437 written (239 bytes)
✓ Chunk 4426/4437 written (239 bytes)
✓ Chunk 4427/4437 written (239 bytes)
✓ Chunk 4428/4437 written (239 bytes)
✓ Chunk 4429/4437 written (239 bytes)
✓ Chunk 4430/4437 written (239 bytes)
✓ Chunk 4431/4437 written (239 bytes)
✓ Chunk 4432/4437 written (239 bytes)
✓ Chunk 4433/4437 written (239 bytes)
✓ Chunk 4434/4437 written (239 bytes)
✓ Chunk 4435/4437 written (239 bytes)
✓ Chunk 4436/4437 written (239 bytes)
  File CRC32 calculated: 0x83F995A0
✓ Chunk 4437/4437 written (160 bytes)
  [4437/4437] Chunk streamed (seq=4437)
✓ Transfer session finalized:
  Session ID: stream_428701
  File: test_received.jpg
  Total size: 1060364 bytes

Reconstruction complete! Time: 9329 ms

=== Step 4: Verification Summary ===
✓ All 4437 chunks streamed and verified (CRC checks passed)
✓ File reconstructed with 1060364 bytes
✓ File saved as: test_received.jpg
  Original filename: test.jpg
  New filename: test_received.jpg (with _received suffix)
  Total time: 9329 ms (streaming + reconstruction)
  Peak memory usage: ~33 KB (32KB read buffer + chunk buffer)

Waiting for SD card write completion...

=== File Verification ===
Checking if reconstructed file exists...
✓ File found: test_received.jpg (1060364 bytes)
✓ File size matches expected: 1060364 bytes
========================================

Final sync wait (allowing SD card internal write buffering)...
✓ File should now be fully committed to SD card

╔═══════════════════════════════════════════════════════╗
║              Final Summary - Run #1                   ║
╚═══════════════════════════════════════════════════════╝

  � TEST PASSED!

  The Go-Back-N streaming method successfully:
    ✓ Step 1: Initialized streaming read
    ✓ Step 2: Streamed chunks with sliding window flow control
    ✓ Step 3: Reconstructed file with integrity verification

  Iteration time: 432390 ms
  Progress: 1/1 passed, 0 failed

═══════════════════════════════════════════════════════

```