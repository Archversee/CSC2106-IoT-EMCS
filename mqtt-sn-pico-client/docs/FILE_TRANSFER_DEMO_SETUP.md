# MQTT-SN File Transfer Demo Setup

## Overview
This demo demonstrates file transfer between two Raspberry Pi Pico W devices using MQTT-SN protocol with streaming technology. The file `praise_lord_fauzi.txt` will be transferred from Pico W #1 (Sender) to Pico W #2 (Receiver).

---

## Hardware Requirements

### Both Devices Need:
- Raspberry Pi Pico W
- MicroSD card module (connected via SPI)
- MicroSD card (formatted as exFAT)
- 4 GPIO buttons (GP19, GP20, GP21, GP22)
- USB cable for power and serial monitoring

### SD Card Wiring (Same for Both):
```
Pico W Pin    →    SD Card Module
─────────────────────────────────
GP16 (SPI0 RX)  →  MISO
GP17 (SPI0 CSn) →  CS
GP18 (SPI0 SCK) →  SCK
GP19 (SPI0 TX)  →  MOSI
3.3V            →  VCC
GND             →  GND
```

### Button Wiring:
```
GP19: File Transfer Button (press to send file)
GP20: Message Test Button
GP21: QoS Toggle Button
GP22: Drop ACK Toggle Button

Each button: Connect between GPIO and GND (internal pull-up enabled)
```

---

## Software Setup

### 1. Prepare MicroSD Cards

**Sender (Pico W #1):**
1. Format SD card as exFAT
2. Copy `praise_lord_fauzi.txt` to the root directory of the SD card
3. Insert SD card into Pico W #1's SD card module

**Receiver (Pico W #2):**
1. Format SD card as exFAT
2. Leave it empty (received file will be saved here)
3. Insert SD card into Pico W #2's SD card module

### 2. Configure Network Settings

Edit `config.h` for both devices:

```c
// Wi-Fi Settings (same for both)
#define WIFI_SSID "YourWiFiNetwork"
#define WIFI_PASS "YourPassword"

// MQTT-SN Gateway (same for both)
#define GATEWAY_IP0 192
#define GATEWAY_IP1 168
#define GATEWAY_IP2 1
#define GATEWAY_IP3 100  // Change to your gateway IP

#define UDP_PORT 1883
```

### 3. Build and Flash

```bash
# Navigate to project directory
cd mqtt-sn-pico-client

# Build the project
./build.sh

# Flash to Pico W #1 (Sender)
# 1. Hold BOOTSEL button on Pico W #1
# 2. Connect USB cable
# 3. Copy build/mqtt-sn-pico-client.uf2 to RPI-RP2 drive

# Flash to Pico W #2 (Receiver)
# 1. Hold BOOTSEL button on Pico W #2
# 2. Connect USB cable
# 3. Copy build/mqtt-sn-pico-client.uf2 to RPI-RP2 drive
```

### 4. Setup MQTT-SN Gateway

You need an MQTT-SN gateway running on your network. Using RSMB (Really Small Message Broker):

```bash
# Install RSMB
git clone https://github.com/eclipse/mosquitto.rsmb.git
cd mosquitto.rsmb/rsmb/src
make

# Create config file (config.conf)
cat > config.conf << EOF
# MQTT-SN Gateway Configuration
trace_output protocol

listener 1883 INADDR_ANY mqtt
listener 1883 INADDR_ANY mqtts

# MQTT-SN topics
topic pico/cmd 1
topic pico/status 2
topic file/meta 3
topic file/data 4
EOF

# Run the gateway
./broker_mqtts config.conf
```

---

## Running the Demo

### Step 1: Start Both Pico W Devices

**Monitor Serial Output:**
```bash
# On Linux/Mac - find the correct port
ls /dev/tty.usbmodem*

# Connect to Pico W #1 (Sender)
screen /dev/tty.usbmodem14201 115200

# In another terminal, connect to Pico W #2 (Receiver)
screen /dev/tty.usbmodem14301 115200
```

Both devices should show:
```
Wi-Fi connected. IP: 192.168.1.xxx
UDP client ready...
Waiting for CONNACK...
✓ MicroSD card initialized successfully
✓ File transfer topics subscribed
```

### Step 2: Initiate File Transfer

On **Pico W #1 (Sender)**, press the **GP19 button** (File Transfer Button).

You should see:
```
>>> File Transfer Button Pressed <<<

=== Starting STREAMING File Transfer ===
File: praise_lord_fauzi.txt
Using QoS 1 for reliable delivery
Mode: STREAMING (memory efficient)
✓ File opened for streaming:
  File size: 4994 bytes
  Chunks: 22
  Session ID: stream_12345
  File CRC: 0x813B
Sent metadata (QoS 1, msg_id=1)
Streaming chunks...
  [10/22] 45.5% complete (msg_id=11)
  [20/22] 90.9% complete (msg_id=21)
  [22/22] 100.0% complete (msg_id=23)
✓ File transfer complete:
  Total packets: 23 (1 metadata + 22 data)
  Time: 2500 ms
  Throughput: 1.95 KB/s
  Peak memory: ~0.5 KB (single chunk buffer)
```

### Step 3: Monitor Reception

On **Pico W #2 (Receiver)**, you should see:
```
Received file metadata:
  Filename: praise_lord_fauzi.txt
  Size: 4994 bytes
  Chunks: 22
  File CRC: 0x813B
  Session: stream_12345

Receiving chunks...
Chunk 0/22 received (seq=0, size=237)
Chunk 1/22 received (seq=1, size=237)
...
Chunk 21/22 received (seq=21, size=17)

All chunks received! Reconstructing file...
Verified: 22/22 chunks
File CRC16 verification passed: 0x813B
✓ File reconstructed successfully!
  Output: RCV_praise_lord_fauzi.txt
  Size: 4994 bytes
  Time: 150 ms
```

### Step 4: Verify Transfer

Check the SD card on **Pico W #2 (Receiver)**:
- You should see a new file: `RCV_praise_lord_fauzi.txt`
- Size should be 4994 bytes
- Content should be identical to the original

---

## How It Works

### Sender Side (Pico W #1)

1. **Button Press**: GP19 triggers `send_file_via_mqtt()`
2. **Streaming Init**: `init_streaming_read()` opens file without loading to memory
3. **Send Metadata**: Serializes and publishes metadata to topic ID 3
4. **Stream Chunks**: Loops through chunks:
   - `read_chunk_streaming()` reads one chunk (~237 bytes)
   - Verifies chunk CRC
   - Serializes chunk
   - Publishes to topic ID 4 with QoS 1
5. **Cleanup**: `cleanup_streaming_read()` closes file
6. **Memory**: Only 1 chunk (~0.5 KB) in RAM at any time

### Receiver Side (Pico W #2)

1. **Subscribe**: Listens to topic ID 3 (metadata) and 4 (chunks)
2. **Metadata**: `handle_file_metadata()` receives file info, initializes session
3. **Receive Chunks**: `handle_file_payload()` for each chunk:
   - Deserializes chunk
   - Verifies CRC
   - Marks received in bitmap
   - Handles duplicates (QoS 1 may send duplicates)
4. **Reconstruct**: When all chunks received:
   - `reconstruct()` rebuilds file from chunks
   - Verifies overall file CRC
   - Saves to SD card as `RCV_<original_filename>`

### Key Features

✅ **Memory Efficient**: Only one 237-byte chunk in RAM at a time
✅ **Reliable**: QoS 1 ensures delivery with retransmission
✅ **No Size Limit**: Handles files of any size (limited only by SD card)
✅ **Integrity**: CRC verification on chunks and final file
✅ **Duplicate Handling**: Bitmap prevents duplicate chunk writing
✅ **Streaming**: No need to load entire file into memory

---

## Troubleshooting

### Issue: "File not found"
- **Solution**: Ensure `praise_lord_fauzi.txt` exists on sender's SD card
- Check filename is exact (case-sensitive)
- Verify SD card is properly formatted as exFAT

### Issue: "MicroSD initialization failed"
- **Solution**: Check SD card wiring
- Ensure SD card is formatted as exFAT
- Try reinserting SD card
- Wait a few seconds for initialization

### Issue: "PINGRESP timeout"
- **Solution**: Check Wi-Fi connection on both devices
- Verify MQTT-SN gateway is running
- Check gateway IP address in config.h

### Issue: File transfer incomplete
- **Solution**: Check serial output for chunk errors
- Verify network stability
- Ensure sufficient delay between chunks (configured in code)
- Check SD card has enough space on receiver

### Issue: CRC mismatch
- **Solution**: SD card may be corrupted or failing
- Try different SD card
- Reformat SD card as exFAT
- Check SD card connections

---

## Technical Details

### File Transfer Protocol

**Topic IDs:**
- Topic 1: `pico/cmd` (commands)
- Topic 2: `pico/status` (status messages)
- Topic 3: `file/meta` (file metadata)
- Topic 4: `file/data` (file chunks)

**Metadata Packet (247 bytes):**
```c
struct Metadata {
    char filename[64];      // Original filename
    char session_id[32];    // Unique session ID
    uint32_t total_size;    // File size in bytes
    uint32_t chunk_count;   // Number of chunks
    uint16_t file_crc;      // Overall file CRC16
};
```

**Chunk Packet (247 bytes):**
```c
struct Payload {
    uint32_t sequence;      // Chunk sequence number
    uint16_t size;          // Data size in this chunk
    uint16_t crc;           // CRC16 of chunk data
    uint8_t data[237];      // Actual chunk data
};
```

### Memory Usage

- **Sender**: ~0.5 KB (single chunk buffer)
- **Receiver**: Depends on file size
  - Metadata: ~200 bytes
  - Bitmap: chunk_count bits (22 chunks = 3 bytes)
  - Chunk array: chunk_count × 247 bytes (stored until reconstruction)
  - For 5KB file: ~5.5 KB total

### Performance

Measured with `praise_lord_fauzi.txt` (4994 bytes, 22 chunks):
- **Transfer Time**: ~2.5 seconds
- **Throughput**: ~2 KB/s
- **Overhead**: Minimal (metadata + CRC)
- **Network**: QoS 1 ensures reliability

---

## Next Steps

1. **Test with Larger Files**: Try 10KB, 50KB, 100KB files
2. **Test Packet Loss**: Use DROP_ACK button (GP22) to simulate packet loss
3. **Multiple Files**: Send multiple files in sequence
4. **Bidirectional**: Have both Picos send files to each other
5. **Monitor Network**: Use Wireshark to observe MQTT-SN packets

---

## References

- MQTT-SN Specification: https://www.oasis-open.org/committees/mqtt/
- Pico W Documentation: https://datasheets.raspberrypi.com/picow/pico-w-datasheet.pdf
- exFAT Specification: Microsoft exFAT File System
- CRC-16 CCITT: Standard polynomial 0x1021

---

**Demo created by INF2004 Project Team, 2025**
