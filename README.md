# INF2004 Project - MQTT-SN File Transfer System with Web Dashboard

A complete IoT file transfer system featuring Raspberry Pi Pico W devices communicating via MQTT-SN protocol with a real-time web dashboard for monitoring and control.

## Project Overview

This project implements a robust file transfer system between Raspberry Pi Pico W devices using the MQTT-SN (MQTT for Sensor Networks) protocol over UDP. The system includes:

- **Sender (TX)** and **Receiver (RX)** Pico W clients with microSD card support
- **Chunked file transfer** with Go-Back-N ARQ protocol for reliability
- **QoS support** (Quality of Service levels 0, 1, and 2)
- **Real-time web dashboard** for monitoring device status and file transfers
- **Hot-plug microSD detection** and automatic reconnection handling
- **MQTT-SN Gateway** for protocol translation to standard MQTT

## Key Features

### Embedded System Features
- **MQTT-SN Protocol**: Lightweight messaging protocol optimized for constrained devices
- **File Transfer**: Reliable chunked file transfer with 32KB sliding window (Go-Back-N)
- **QoS Levels**: Support for QoS 0 (at-most-once), QoS 1 (at-least-once), QoS 2 (exactly-once)
- **MicroSD Integration**: Read/write files with hot-plug detection and automatic recovery
- **Retry Logic**: Automatic retransmission on timeout with configurable retry limits
- **Dual Client Architecture**: Separate TX (sender) and RX (receiver) builds

### Dashboard Features
- **Real-time Monitoring**: Live device status and message feeds
- **Command Interface**: Send commands to devices with QoS selection
- **File Transfer Visualization**: Progress tracking with chunk counters
- **Telemetry Charts**: Live data visualization for sensor readings
- **WebSocket Communication**: Low-latency updates via Socket.IO

---

## Project Structure

### Root Directory Files

| File             | Purpose                                         |
| ---------------- | ----------------------------------------------- |
| `CMakeLists.txt` | Root CMake build configuration for the project  |
| `README.md`      | This file - comprehensive project documentation |

### `/mqtt-sn-pico-client/` - Main Pico W Client Code

Core MQTT-SN client implementation for Raspberry Pi Pico W with file transfer capabilities.

#### Main Application Files

| File                       | Description                                                                                                                                            |
| -------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `mqtt-sn-pico-client-tx.c` | **Sender client** - Publishes messages and initiates file transfers. Uses GPIO buttons to send test messages and transfer files (test_1.txt, test.jpg) |
| `mqtt-sn-pico-client-rx.c` | **Receiver client** - Subscribes to topics and receives file transfers. Automatically saves received files to microSD card                             |
| `mqtt-sn-pico-client.c`    | Legacy unified client (deprecated - use TX/RX versions)                                                                                                |
| `mqtt-client.h/c`          | Shared MQTT client library with network initialization and common functionality                                                                        |
| `mqttsn_bridge.h/c`        | Bridge module for publishing file transfer status/progress to dashboard                                                                                |

#### Configuration Files

| File                   | Description                                                                                                                 |
| ---------------------- | --------------------------------------------------------------------------------------------------------------------------- |
| `config.h`             | **Main configuration file** - Wi-Fi credentials, gateway IP, MQTT-SN settings, QoS parameters, GPIO pin mappings, topic IDs |
| `hw_config.c`          | Hardware configuration for SD card SPI pins                                                                                 |
| `lwipopts.h`           | lwIP (lightweight IP) stack configuration for networking                                                                    |
| `pico_flash_region.ld` | Custom linker script for flash memory layout                                                                                |

#### MQTT Protocol Implementation

| Directory/File         | Description                                                                                                                             |
| ---------------------- | --------------------------------------------------------------------------------------------------------------------------------------- |
| `mqtt/mqtt-sn-udp.h/c` | MQTT-SN protocol implementation over UDP - handles CONNECT, PUBLISH, SUBSCRIBE, PINGREQ/PINGRESP, QoS message tracking, retransmissions |
| `mqtt/tests/`          | Unit tests for MQTT-SN implementation                                                                                                   |

#### File System & Storage

| Directory                           | Description                                                                                                                                                      |
| ----------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `fs/chunk_transfer.h/c`             | High-level chunk-based file transfer session management - handles metadata reception (QoS 2), data chunk writes (QoS 1), bitmap tracking for out-of-order chunks |
| `fs/data_frame.h/c`                 | Data frame structures for file transfer - defines Metadata and Payload chunk formats with session IDs                                                            |
| `fs/tests/`                         | File system module tests                                                                                                                                         |
| `drivers/microsd_driver.h/c`        | High-level microSD card interface wrapper for FatFS library - provides init, read, write, seek, directory operations                                             |
| `drivers/microsd_chunk.c`           | Optimized chunk write operations for SD card with 32KB buffering                                                                                                 |
| `drivers/microsd_file.c`            | File-level operations wrapper                                                                                                                                    |
| `drivers/tests/`                    | Driver tests                                                                                                                                                     |
| `no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/` | Third-party FatFS library for SD card access via SPI                                                                                                             |

#### Build System

| File/Directory   | Description                                                                                   |
| ---------------- | --------------------------------------------------------------------------------------------- |
| `CMakeLists.txt` | CMake build configuration for Pico client - defines TX/RX executables with PICO_TX_BUILD flag |
| `build.sh`       | Build script - creates build directory and runs cmake/make                                    |
| `build/`         | Build output directory - contains compiled .uf2 files, makefiles, and build artifacts         |
| `CMakeFiles/`    | CMake-generated build system files                                                            |

#### Test Files & Results

| Directory      | Description                                                                                                      |
| -------------- | ---------------------------------------------------------------------------------------------------------------- |
| `testfiles/`   | Sample files for testing (test_1.txt, etc.)                                                                      |
| `testresults/` | Test result documentation - automated chunk tests, streaming tests, Go-Back-N validation, multi-file write tests |

### `/dashboard/` - Real-time Web Dashboard

Node.js-based web dashboard for monitoring and controlling Pico devices via MQTT.

#### Dashboard Files

| File                 | Description                                                                |
| -------------------- | -------------------------------------------------------------------------- |
| `DASHBOARDREADME.md` | Detailed dashboard documentation with setup instructions and API reference |
| `package.json`       | Root npm package configuration                                             |

#### Backend (`/dashboard/backend/`)

| File           | Description                                                                                                                                                 |
| -------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `server.js`    | Express + Socket.IO server - connects to MQTT broker, subscribes to device topics, relays messages to browsers, handles command publishing with QoS support |
| `qos.js`       | QoS message handling utilities                                                                                                                              |
| `package.json` | Backend dependencies (express, socket.io, mqtt, cors)                                                                                                       |

#### Frontend (`/dashboard/frontend/`)

| File         | Description                                                                                                         |
| ------------ | ------------------------------------------------------------------------------------------------------------------- |
| `index.html` | Dashboard UI - device list, live message feed, telemetry charts, command form, file transfer progress visualization |
| `client.js`  | Frontend JavaScript - Socket.IO client, chart rendering, UI event handlers                                          |

### `/paho.mqtt-sn.embedded-c-master/` - MQTT-SN Library

Eclipse Paho MQTT-SN embedded C library (third-party dependency).

| Directory        | Description                                                                                           |
| ---------------- | ----------------------------------------------------------------------------------------------------- |
| `MQTTSNPacket/`  | Low-level MQTT-SN packet serialization/deserialization                                                |
| `MQTTSNGateway/` | **MQTT-SN Gateway** - Transparent/aggregating gateway that bridges MQTT-SN (UDP) to MQTT (TCP) broker |
| `MQTTSNClient/`  | High-level C++ client (not currently used)                                                            |
| `doc/`           | Doxygen documentation configuration                                                                   |
| `build.gateway/` | Gateway build directory                                                                               |

#### Gateway Configuration

The MQTT-SN Gateway connects your Pico devices to a standard MQTT broker (e.g., Mosquitto).

| File           | Description                                                        |
| -------------- | ------------------------------------------------------------------ |
| `gateway.conf` | Gateway configuration - broker address, port, client ID, log level |
| `clients.conf` | Client whitelist/configuration                                     |

---

## Hardware Requirements

### For Pico W Devices
- **2x Raspberry Pi Pico W** (one for TX, one for RX)
- **2x MicroSD card modules** (SPI interface)
- **2x MicroSD cards** (formatted as FAT32 or exFAT)
- **Breadboards and jumper wires**
- **4 Push buttons** (for TX: message send, text file, image file, ACK drop; RX: none required but can be added)
- **USB cables** for power and programming

### For Dashboard/Gateway
- **Computer** running macOS/Linux/Windows
- **MQTT Broker** (Mosquitto recommended)
- **Node.js** v16+ with npm

---

## Pin Configuration

### MicroSD Card Wiring (Both TX and RX)

Configure in `hw_config.c`:

| SD Card Pin | Pico Pin | Function    |
| ----------- | -------- | ----------- |
| CS          | GP17     | Chip Select |
| MOSI        | GP19     | SPI TX      |
| MISO        | GP16     | SPI RX      |
| SCK         | GP18     | SPI Clock   |
| GND         | GND      | Ground      |
| VCC         | 3.3V     | Power       |

### GPIO Buttons (TX Client)

Configure in `config.h`:

| Button     | GPIO Pin | Function                                         |
| ---------- | -------- | ------------------------------------------------ |
| Message    | GP20     | Send test message with current QoS               |
| Text File  | GP21     | Transfer test_1.txt                              |
| Image File | GP22     | Transfer test.jpg                                |
| ACK Drop   | GP19     | Toggle ACK dropping (for testing retransmission) |

---

## Software Dependencies

### Pico W Firmware
- **Pico SDK 2.2.0** (includes lwIP networking stack)
- **ARM GCC Toolchain** 14.2 Rel1
- **CMake** 3.5+
- **FatFS library** (included in `no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/`)
- **Paho MQTT-SN embedded C** (included in `paho.mqtt-sn.embedded-c-master/`)

### MQTT-SN Gateway
- **CMake** for building
- **C++ compiler** (gcc/g++)

### Dashboard
- **Node.js** v16+ and npm
- **npm packages**: express, socket.io, mqtt, cors (auto-installed)

### MQTT Broker
- **Mosquitto** or any MQTT broker supporting MQTT 3.1.1

---

## Configuration Guide

### 1. Configure Pico W Wi-Fi & Gateway

Edit `/mqtt-sn-pico-client/config.h`:

```c
// Wi-Fi Credentials
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_wifi_password"

// MQTT-SN Gateway IP (computer running the gateway)
#define GATEWAY_IP0 10
#define GATEWAY_IP1 200
#define GATEWAY_IP2 133
#define GATEWAY_IP3 90
#define UDP_PORT 10000
```

### 2. Configure MQTT-SN Gateway

Edit `/paho.mqtt-sn.embedded-c-master/MQTTSNGateway/gateway.conf`:

```conf
# Broker connection
BrokerName=localhost
BrokerPortNo=1883
BrokerSecurePortNo=8883

# Gateway UDP port (must match UDP_PORT in config.h)
GatewayPortNo=10000
GatewayUDP=YES

# Client authentication
ClientAuthentication=NO
```

### 3. Configure Dashboard Backend

Edit `/dashboard/backend/server.js` (if needed):

```javascript
const MQTT_BROKER = process.env.MQTT_URL || 'mqtt://localhost:1883';
const PORT = process.env.PORT || 3000;
```

---

## Compilation & Build Instructions

### Build Pico W Firmware

#### Option 1: Using VS Code Pico Extension (Recommended)

1. Open the project in VS Code
2. Install the "Raspberry Pi Pico" extension
3. Press `Ctrl+Shift+P` → "CMake: Configure"
4. Press `Ctrl+Shift+P` → "CMake: Build"
5. Build artifacts will be in `mqtt-sn-pico-client/build/`:
   - `mqtt-sn-pico-client-tx.uf2` (sender)
   - `mqtt-sn-pico-client-rx.uf2` (receiver)

#### Option 2: Command Line Build

```bash
cd mqtt-sn-pico-client
./build.sh
```

Or manually:

```bash
cd mqtt-sn-pico-client
mkdir -p build
cd build
cmake ..
make -j8
```

Build outputs:
- `build/mqtt-sn-pico-client-tx.uf2` - Flash to sender Pico
- `build/mqtt-sn-pico-client-rx.uf2` - Flash to receiver Pico

### Build MQTT-SN Gateway

```bash
cd paho.mqtt-sn.embedded-c-master/MQTTSNGateway
./build.sh
# Or manually:
mkdir -p build.gateway
cd build.gateway
cmake ..
make
```

Binary output: `build.gateway/MQTT-SNGateway`

---

## Running the System

### Step 1: Start MQTT Broker (Mosquitto)

#### macOS (Homebrew)
```bash
brew install mosquitto
mosquitto -v  # Run with verbose logging
```

#### Linux
```bash
sudo apt-get install mosquitto
mosquitto -v
```

Default ports:
- MQTT: `1883`
- MQTT over WebSocket: `9001`

### Step 2: Start MQTT-SN Gateway

```bash
cd paho.mqtt-sn.embedded-c-master/MQTTSNGateway/build.gateway
./MQTT-SNGateway
```

Expected output:
```
MQTT-SN Gateway starting...
Listening on UDP port 10000
Connected to broker at localhost:1883
```

### Step 3: Start Dashboard Backend

```bash
cd dashboard/backend
npm install  # First time only
node server.js
```

Expected output:
```
[MQTT] connected to mqtt://localhost:1883
[MQTT] subscribed: pico/#
[MQTT] subscribed: file/#
HTTP+Socket server listening on http://localhost:3000
```

### Step 4: Flash Pico W Devices

1. **Hold BOOTSEL button** on Pico W while connecting USB
2. **Drag and drop** .uf2 file to RPI-RP2 drive:
   - TX Pico: `mqtt-sn-pico-client-tx.uf2`
   - RX Pico: `mqtt-sn-pico-client-rx.uf2`
3. Pico will reboot automatically

### Step 5: Monitor Serial Output

Use any serial terminal (115200 baud):
- VS Code Serial Monitor
- `screen /dev/ttyACM0 115200` (Linux/Mac)
- PuTTY (Windows)

### Step 6: Open Dashboard

Navigate to: `http://localhost:3000`

---

## Testing the System

### Test 1: Basic Message Publishing (TX)

1. Press **GP20 button** on TX Pico
2. Check TX serial: `"Button pressed! Publishing message..."`
3. Check dashboard: New message appears in "Recent Messages"
4. Check RX serial: Message received on `pico/status` topic

### Test 2: Text File Transfer

1. Prepare `test_1.txt` on TX Pico's microSD card
2. Press **GP21 button** on TX Pico
3. Monitor TX serial for chunk progress
4. Monitor RX serial for reception and write progress
5. Check RX microSD card: `test_1_received.txt` created
6. Verify file integrity with checksum

### Test 3: Image File Transfer

1. Prepare `test.jpg` on TX Pico's microSD card
2. Press **GP22 button** on TX Pico
3. Monitor dashboard for file transfer progress
4. Check RX microSD card: `test_received.jpg` created
5. Verify file integrity

### Test 4: QoS and Retransmission

1. Press **GP19 button** on TX to enable ACK dropping
2. Send a message with **GP20 button**
3. Observe automatic retransmissions in serial output
4. Verify delivery despite packet loss

### Test 5: Dashboard Commands

1. In dashboard, enter:
   - **Device ID**: `pico_me`
   - **Topic**: `pico/cmd`
   - **Payload**: `{"action":"test"}`
   - **QoS**: Check for QoS 1
2. Click "Send Command"
3. Check RX serial: Command received and processed

### Test 6: Hot-plug MicroSD Detection

1. Remove microSD card from RX during operation
2. Observe warning in serial output
3. Reinsert card
4. System automatically re-initializes
5. File transfers resume functionality

---

## MQTT Topic Structure

### Predefined Topics (config.h)

| Topic ID | Topic Name    | Direction      | Purpose                       | QoS |
| -------- | ------------- | -------------- | ----------------------------- | --- |
| 1        | `pico/cmd`    | Dashboard → RX | Commands to device            | 2   |
| 2        | `pico/status` | TX → Dashboard | Status updates                | 0-2 |
| 4        | `file/data`   | TX ↔ RX        | File chunks (metadata + data) | 1-2 |

### Dynamic Topics (Dashboard)

| Topic                                   | Direction          | Purpose                          |
| --------------------------------------- | ------------------ | -------------------------------- |
| `file/control`                          | RX → TX            | Go-Back-N flow control (ACK/NAK) |
| `devices/{id}/telemetry/#`              | Device → Dashboard | Sensor readings                  |
| `devices/{id}/file-transfer/status`     | Device → Dashboard | Transfer status                  |
| `devices/{id}/file-transfer/progress`   | Device → Dashboard | Chunk progress                   |
| `devices/{id}/file-transfer/validation` | Device → Dashboard | Checksum validation              |

---

## File Transfer Protocol

### Metadata Chunk (Chunk 0)

- **QoS 2** (exactly-once delivery)
- Contains: session ID, filename, total chunks, chunk size, file size, checksum
- Must be received before data chunks are accepted

### Data Chunks (Chunk 1+)

- **QoS 1** (at-least-once delivery, duplicates handled via bitmap)
- Contains: session ID, chunk number, sequence number, payload data
- Uses Go-Back-N ARQ with 32KB sliding window (~138 chunks)

### Flow Control

1. TX sends metadata chunk (QoS 2)
2. RX initializes session, sends ACK to `file/control`
3. TX sends window of data chunks (QoS 1)
4. RX validates chunks, sends cumulative ACK or NAK
5. On NAK: TX retransmits from requested chunk
6. Repeat until all chunks received
7. RX finalizes file, validates checksum

---

## Troubleshooting

### Pico Won't Connect to Wi-Fi

- Verify `WIFI_SSID` and `WIFI_PASS` in `config.h`
- Check Wi-Fi is 2.4GHz (Pico W doesn't support 5GHz)
- Ensure WPA2-AES authentication

### Gateway Connection Failed

- Verify gateway IP matches Pico configuration
- Check firewall allows UDP port 10000
- Ensure gateway is running before Pico boots
- Use `ping` to verify network connectivity

### MicroSD Initialization Fails

- Verify SPI wiring matches `hw_config.c`
- Try different microSD card (some cards are incompatible)
- Check card is formatted as FAT32 or exFAT
- Wait 5-10 seconds after power-on for initialization
- Check serial output for detailed error messages

### File Transfer Stalls

- Check dashboard shows ACK messages in `file/control`
- Verify RX has enough free space on SD card
- Monitor QoS retry counters in serial output
- Ensure `MAX_PENDING_QOS_MSGS >= 160` in `config.h`

### Dashboard Not Receiving Messages

- Check Mosquitto is running: `mosquitto -v`
- Verify backend subscribed: Look for `[MQTT] subscribed: pico/#`
- Check browser console (F12) for WebSocket errors
- Ensure correct broker URL in `server.js`

### Message Retransmissions

- Normal for QoS 1/2 (ensures delivery)
- Excessive retries indicate network issues
- Check `QOS_RETRY_INTERVAL_US` and `QOS_MAX_RETRIES` in `config.h`
- Use ACK drop button to test retry behavior

---

## Performance Characteristics

### File Transfer Speed

- **Chunk size**: 237 bytes (MQTT-SN payload limit)
- **Window size**: 32KB (~138 chunks)
- **Typical throughput**: ~5-15 KB/s (depends on Wi-Fi quality)
- **Large file example**: 1MB file ≈ 60-120 seconds

### Memory Usage (Pico W)

- **Max pending QoS messages**: 160 (configurable)
- **Chunk bitmap**: Dynamically allocated based on file size
- **SD card buffer**: 32KB per active transfer

### Network Parameters

- **Keepalive interval**: 60 seconds
- **Ping interval**: 30 seconds
- **Ping timeout**: 90 seconds (3x ping interval)
- **QoS retry interval**: 2 seconds
- **Max retries**: 3 attempts

---

## Development Notes

### Adding New Topics

1. Define topic ID in `config.h`:
   ```c
   #define TOPIC_ID_NEW_TOPIC 5
   ```

2. Register/subscribe in TX or RX client:
   ```c
   // TX (register for publishing)
   mqtt_sn_add_topic_for_registration(mqtt_ctx, "new/topic");
   
   // RX (subscribe)
   mqtt_sn_add_topic_for_subscription(mqtt_ctx, "new/topic", QOS_LEVEL_1);
   ```

3. Update dashboard subscriptions in `server.js`:
   ```javascript
   mqttClient.subscribe('new/#', { qos: 1 });
   ```

### Modifying QoS Settings

Edit `config.h`:
```c
#define QOS_RETRY_INTERVAL_US 2000000  // 2 seconds
#define QOS_MAX_RETRIES 3              // 3 attempts
#define MAX_PENDING_QOS_MSGS 160       // Must be >= window size
```

### Changing File Transfer Window Size

Window size affects memory and throughput. Modify in `chunk_transfer.h/c`:
```c
#define WINDOW_SIZE_CHUNKS 138  // 32KB / 237 bytes
```

**Important**: Update `MAX_PENDING_QOS_MSGS` accordingly:
```c
#define MAX_PENDING_QOS_MSGS (WINDOW_SIZE_CHUNKS + 22)  // Headroom for metadata + control
```

---

## License

This project uses components under multiple licenses:

- **Project code**: Developed for INF2004 coursework
- **Paho MQTT-SN**: Dual-licensed under EPL and EDL (see `paho.mqtt-sn.embedded-c-master/about.html`)
- **FatFS**: BSD-style license (see `no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/LICENSE`)
- **Pico SDK**: BSD 3-Clause License

---

## Authors & Credits

- **INF2004 Project Team** - Main application development
- **CS31** - Original MQTT-SN via UDP implementation
- **Eclipse Paho** - MQTT-SN library and gateway
- **Raspberry Pi Foundation** - Pico SDK
- **carlk3** - no-OS-FatFS-SD-SDIO-SPI-RPi-Pico library

---

## References

- [MQTT-SN Protocol Specification v1.2](http://mqtt.org/new/wp-content/uploads/2009/06/MQTT-SN_spec_v1.2.pdf)
- [Pico W SDK Documentation](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)
- [Eclipse Paho MQTT-SN](https://github.com/eclipse/paho.mqtt-sn.embedded-c)
- [Mosquitto MQTT Broker](https://mosquitto.org/)

---

**Last Updated**: November 24, 2025