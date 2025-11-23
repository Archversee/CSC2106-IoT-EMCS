# INF2004 Project - MQTT-SN Pico W Client with File Transfer

This project implements a comprehensive MQTT-SN client on Raspberry Pi Pico W with microSD card support and reliable file transfer capabilities over UDP.

## Features

- **MQTT-SN Protocol**: Full implementation with QoS 0/1/2 support
- **File Transfer System**: Chunk-based reliable file transfer with CRC verification
- **MicroSD Card Driver**: exFAT filesystem support with chunk-based read/write
- **Wi-Fi Connectivity**: CYW43 wireless interface with automatic reconnection
- **GPIO Controls**: Buttons for message publishing, QoS selection, and file transfer
- **MQTT-SN Gateway**: Eclipse Paho gateway for MQTT-SN ↔ MQTT translation
- **Web Dashboard**: Real-time monitoring and control interface with file transfer visualization

## Components

### Pico W Firmware (`mqtt-sn-pico-client/`)
- **MQTT-SN UDP Client**: Connection management, publish/subscribe, QoS handling
- **Chunk Transfer System**: Session-based file chunking and reassembly
- **Data Serialization**: Metadata and payload frame handling with CRC16
- **MicroSD Driver**: SPI-based driver with exFAT support
- **File Transfer Protocol**: 247-byte chunks with QoS 1 reliability

### MQTT-SN Gateway (`paho.mqtt-sn.embedded-c-master/`)
- **Protocol Translation**: MQTT-SN (UDP) ↔ MQTT (TCP/IP)
- **Topic Registration**: Predefined and dynamic topic support
- **Gateway Discovery**: SEARCHGW and GWINFO messages
- **Configurable**: Gateway, client, and topic configuration files

### Web Dashboard (`dashboard/`)
- **Real-time Monitoring**: Live device status and telemetry visualization
- **File Transfer Tracking**: Progress bars, chunk counters, checksum validation
- **Command Interface**: Send MQTT messages to devices with QoS support
- **Message Feed**: View recent MQTT messages with timestamps
- **Chart Visualization**: Live telemetry charts using Chart.js

## Complete Project Setup

This guide walks you through setting up all components: Pico W firmware, MQTT-SN Gateway, Mosquitto broker, and the web dashboard.

### Prerequisites

Before starting, ensure you have:

#### For Pico W Development
- **Pico SDK** (v2.2.0+) installed with `PICO_SDK_PATH` environment variable set
- **CMake** 3.13 or higher
- **ARM GCC Toolchain** (`arm-none-eabi-gcc`)
- **Make** or Ninja build system
- **USB cable** for flashing and serial communication

#### For MQTT Infrastructure
- **GCC compiler** (for building the gateway)
- **Make**
- **Mosquitto MQTT broker**
  - macOS: `brew install mosquitto`
  - Ubuntu/Debian: `sudo apt-get install mosquitto mosquitto-clients`
- **Node.js** v16+ (for dashboard, tested with v24)
- **npm** (comes with Node.js)

#### Hardware
- Raspberry Pi Pico W
- Cytron Maker Pi Pico kit (or compatible SD card reader)
- MicroSD card (exFAT formatted, Class 4+)
- USB cable (micro-USB or USB-C depending on your board)

---

## Step-by-Step Setup

### Step 1: Build the Pico W Firmware

```bash
# Navigate to the Pico client directory
cd mqtt-sn-pico-client

# Build the firmware
./build.sh

# Output files will be in mqtt-sn-pico-client/build/
# - mqtt-sn-pico-client-rx.uf2
# - mqtt-sn-pico-client-tx.uf2
```

**Alternative manual build:**
```bash
cd mqtt-sn-pico-client
mkdir -p build
cd build
cmake ..
make -j 8
```

### Step 2: Configure Wi-Fi and Gateway Settings

Edit `mqtt-sn-pico-client/config.h` with your network details:

```c
// Wi-Fi credentials
#define WIFI_SSID "YourNetworkName"
#define WIFI_PASS "YourPassword"

// MQTT-SN Gateway IP address (your laptop's IP)
#define GATEWAY_IP0 192
#define GATEWAY_IP1 168
#define GATEWAY_IP2 1
#define GATEWAY_IP3 100  // Change to your laptop's IP
#define UDP_PORT 10000
```

**To find your laptop's IP:**
- macOS: `ifconfig | grep "inet " | grep -v 127.0.0.1`
- Linux: `ip addr show | grep "inet " | grep -v 127.0.0.1`

After editing, rebuild the firmware:
```bash
cd mqtt-sn-pico-client
./build.sh
```

### Step 3: Flash the Pico W

1. **Prepare the Pico W for flashing:**
   - Hold the **BOOTSEL** button on the Pico W
   - While holding, connect USB cable to your computer
   - Release BOOTSEL - the Pico W appears as a USB drive

2. **Copy the firmware:**
   ```bash
   # For RX device (receiver)
   cp mqtt-sn-pico-client/build/mqtt-sn-pico-client-rx.uf2 /Volumes/RPI-RP2/
   
   # For TX device (transmitter)
   cp mqtt-sn-pico-client/build/mqtt-sn-pico-client-tx.uf2 /Volumes/RPI-RP2/
   ```
   
   The Pico W will automatically reboot and run the firmware.

3. **Verify flashing:**
   ```bash
   # Monitor serial output (macOS)
   screen /dev/tty.usbmodem* 115200
   
   # Linux
   screen /dev/ttyACM0 115200
   
   # Or use minicom
   minicom -D /dev/ttyACM0 -b 115200
   ```

   You should see Wi-Fi connection attempts and status messages.

### Step 4: Prepare MicroSD Card

1. **Format the microSD card as exFAT**
2. **Create a test file** (optional, for file transfer testing):
   ```bash
   echo "Hello from Pico W!" > /Volumes/YOUR_SD_CARD/test.txt
   ```
3. **Insert the card** into the Cytron Pico W kit's SD card slot

### Step 5: Build and Configure MQTT-SN Gateway

```bash
# Build the gateway
cd paho.mqtt-sn.embedded-c-master/MQTTSNGateway
mkdir -p build.gateway
cd build.gateway
cmake ..
make
```

**Configure predefined topics** in `paho.mqtt-sn.embedded-c-master/MQTTSNGateway/predefinedTopic.conf`:
```
# Format: TopicId,TopicName
1,pico/cmd
2,pico/status
3,file/meta
4,file/data
```

**Verify gateway configuration** in `gateway.conf`:
```
GatewayUDP.Port=10000
BrokerName=localhost
BrokerPortNo=1883
```

### Step 6: Start Mosquitto Broker

Open a **new terminal window** and start Mosquitto:

```bash
# Start with verbose logging to see all messages
mosquitto -v

# Or run as background service (macOS)
brew services start mosquitto

# Linux systemd
sudo systemctl start mosquitto
```

Verify it's running:
```bash
# Check if Mosquitto is listening on port 1883
netstat -an | grep 1883

# Or on macOS
lsof -i :1883
```

### Step 7: Start MQTT-SN Gateway

Open **another terminal window**:

```bash
cd paho.mqtt-sn.embedded-c-master/MQTTSNGateway
./MQTT-SNGateway
```

You should see:
```
MQTT-SN Gateway starting...
Gateway started on UDP port 10000
Connected to MQTT broker at localhost:1883
```

### Step 8: Start the Dashboard

The dashboard provides a web interface to monitor devices and file transfers.

#### Install Dependencies

```bash
cd dashboard/backend
npm install
```

#### Start the Backend Server

```bash
cd dashboard/backend
npm start

# Or directly:
node server.js
```

You should see:
```
MQTT Dashboard Backend starting...
Socket.IO server listening on http://localhost:3000
Connected to MQTT broker at mqtt://localhost:1883
Subscribed to: devices/+/telemetry/#
Subscribed to: devices/+/status
Subscribed to: devices/+/file-transfer/#
```

#### Access the Dashboard

Open your web browser and navigate to:
```
http://localhost:3000
```

The dashboard will display:
- Connected devices and their last-seen timestamps
- Real-time telemetry data with charts
- Recent MQTT messages
- File transfer progress with chunk tracking
- Command interface to send messages to devices

---

## Running the Complete System

With everything set up, here's the startup sequence:

### Terminal 1: Mosquitto Broker
```bash
mosquitto -v
```

### Terminal 2: MQTT-SN Gateway
```bash
cd paho.mqtt-sn.embedded-c-master/MQTTSNGateway
./MQTT-SNGateway
```

### Terminal 3: Dashboard Backend
```bash
cd dashboard/backend
node server.js
```

### Terminal 4: Monitor Pico W Serial Output
```bash
screen /dev/tty.usbmodem* 115200
```

### Browser: Dashboard UI
```
http://localhost:3000
```

### Terminal 5 (Optional): Monitor MQTT Traffic
```bash
# Subscribe to all topics
mosquitto_sub -t "#" -v

# Or specific topics
mosquitto_sub -t "pico/#" -v
mosquitto_sub -t "file/#" -v
mosquitto_sub -t "devices/#" -v
```

---

## Testing the System

### Test 1: Basic Connectivity

1. Power on the Pico W (should auto-connect to Wi-Fi and gateway)
2. Watch the serial output for connection confirmation
3. In the dashboard, you should see your device appear
4. Check the gateway terminal for CONNECT messages

### Test 2: Send a Test Message

Press **GP20** button on the Pico W to send a 247-byte test message.

Watch for the message in:
- Serial output
- Gateway logs
- Dashboard message feed
- Terminal 5 (if monitoring MQTT)

### Test 3: File Transfer

1. Ensure `test.txt` exists on the microSD card
2. Press **GP19** button on the Pico W to start file transfer
3. Watch the dashboard for:
   - File transfer status (active)
   - Progress bar updating
   - Chunk counter incrementing
   - Checksum validation
   - Completion status

### Test 4: Send Commands from Dashboard

1. In the dashboard UI, fill in the command form:
   - **Device ID**: `pico-w` (or your device's client ID)
   - **Topic Suffix**: `cmd/led`
   - **Payload**: `{"action":"toggle"}`
   - **QoS**: Check the box for QoS 1
2. Click **Send Command**
3. Watch for confirmation in the dashboard and serial output

### Test 5: QoS Levels

1. Press **GP21** on Pico W to cycle through QoS 0 → 1 → 2
2. Press **GP20** to send test message with current QoS
3. Press **GP22** to toggle ACK dropping (simulates packet loss)
4. Observe retransmission behavior in serial output

---

## Quick Reference Commands

### Build Commands
| Command                                                               | Description         |
| --------------------------------------------------------------------- | ------------------- |
| `cd mqtt-sn-pico-client && ./build.sh`                                | Build Pico firmware |
| `cd paho.mqtt-sn.embedded-c-master/MQTTSNGateway && cmake .. && make` | Build gateway       |

### Run Commands
| Component       | Command                                                               |
| --------------- | --------------------------------------------------------------------- |
| Mosquitto       | `mosquitto -v`                                                        |
| MQTT-SN Gateway | `cd paho.mqtt-sn.embedded-c-master/MQTTSNGateway && ./MQTT-SNGateway` |
| Dashboard       | `cd dashboard/backend && node server.js`                              |
| Serial Monitor  | `screen /dev/tty.usbmodem* 115200`                                    |

### Test Commands
```bash
# Publish test message to device
mosquitto_pub -t "pico/cmd" -m '{"action":"test"}'

# Subscribe to all device messages
mosquitto_sub -t "devices/#" -v

# Test file transfer status
mosquitto_pub -t "devices/pico-001/file-transfer/status" \
  -m '{"status":"active","fileName":"test.txt"}'

# Test file transfer progress
mosquitto_pub -t "devices/pico-001/file-transfer/progress" \
  -m '{"chunk":5,"total":10,"seq":5,"checksum":"abc123"}'
```

## Dashboard Topics

The dashboard subscribes to these MQTT topics to monitor your devices:

### Device Topics
- `devices/+/telemetry/#` - All telemetry data (temperature, sensors, etc.)
- `devices/+/status` - Device status updates (online, offline, etc.)

### File Transfer Topics
- `devices/+/file-transfer/progress` - Chunk-by-chunk progress updates
- `devices/+/file-transfer/status` - Transfer status (active, completed, failed)
- `devices/+/file-transfer/validation` - Checksum validation results

### Command Topics
Send commands to devices via the dashboard UI, which publishes to:
- `devices/<deviceId>/<topic-suffix>` - Custom command topics

## Hardware Setup

### Pico W Pin Configuration (Cytron Kit)
- **SD_MISO**: GPIO 12
- **SD_MOSI**: GPIO 11  
- **SD_SCK**: GPIO 10
- **SD_CS**: GPIO 15
- **SPI Port**: SPI1

### GPIO Button Controls
- **GP20**: Send test message (247-byte binary payload)
- **GP21**: Toggle QoS level (0 → 1 → 2 → 0)
- **GP22**: Toggle ACK dropping (for packet loss testing)
- **GP19**: Initiate file transfer (sends `test.txt` via MQTT-SN)

## Requirements

### For Pico W Development
- Pico SDK (v2.2.0+) installed and `PICO_SDK_PATH` set
- CMake 3.13+
- GCC ARM toolchain (`arm-none-eabi-gcc`)
- Make or Ninja build system
- USB connection for flashing and serial output

### For MQTT-SN Gateway (Laptop)
- GCC compiler
- Make
- Network connectivity (UDP port 10000)
- Mosquitto MQTT broker

### MicroSD Card
- exFAT formatted microSD card
- SPI-compatible card (Class 4 or higher recommended)

## MQTT-SN Topics

The project uses predefined topic IDs for efficient communication:

| Topic ID | Topic Name    | Direction | Purpose                           |
| -------- | ------------- | --------- | --------------------------------- |
| 1        | `pico/cmd`    | Subscribe | Command topic (LED control, etc.) |
| 2        | `pico/status` | Publish   | Status messages and test payloads |
| 3        | `file/meta`   | Both      | File metadata transfer            |
| 4        | `file/data`   | Both      | File chunk transfer               |

**Note**: These topics must be configured in your MQTT-SN gateway's predefined topics file.

## Configuration

### 1. Configure Wi-Fi and Gateway (`mqtt-sn-pico-client/config.h`)
```c
#define WIFI_SSID "YourNetworkName"
#define WIFI_PASS "YourPassword"

#define GATEWAY_IP0 192
#define GATEWAY_IP1 168
#define GATEWAY_IP2 1
#define GATEWAY_IP3 100
#define UDP_PORT 10000
```

### 2. Configure MQTT-SN Gateway Topics
Edit `paho.mqtt-sn.embedded-c-master/MQTTSNGateway/predefinedTopic.conf`:
```
1,pico/cmd
2,pico/status
3,file/meta
4,file/data
```

### 3. Configure Gateway Address
Edit `paho.mqtt-sn.embedded-c-master/MQTTSNGateway/gateway.conf`:
```
GatewayUDP.Port=10000
```

## Usage

### Basic Operation
1. Build and flash the Pico W firmware
2. Insert exFAT-formatted microSD card into Cytron Pico W kit
3. Connect via USB and monitor serial output (115200 baud)
4. Start Mosquitto broker on your laptop:
   ```bash
   mosquitto -v
   ```
5. Start MQTT-SN Gateway:
   ```bash
   cd paho.mqtt-sn.embedded-c-master/MQTTSNGateway
   ./MQTT-SNGateway
   ```
6. The Pico W will automatically:
   - Connect to Wi-Fi
   - Connect to MQTT-SN gateway
   - Subscribe to command topics
   - Initialize microSD card

### File Transfer
1. Place a file named `test.txt` in the root of your microSD card
2. Press **GP19** button on the Pico W
3. The file will be transmitted as:
   - 1 metadata chunk (session ID, filename, size, CRC)
   - N data chunks (237 bytes each with CRC verification)
4. Monitor console for transfer progress
5. QoS 1 ensures reliable delivery with automatic retransmission

### Testing QoS Levels
1. Press **GP21** to cycle through QoS 0/1/2
2. Press **GP20** to send a 247-byte test message
3. Current QoS level is displayed in console
4. Press **GP22** to simulate packet loss (drops acknowledgments)

### Monitoring
```bash
# Linux/macOS
screen /dev/ttyACM0 115200

# Or use minicom
minicom -D /dev/ttyACM0 -b 115200

# Subscribe to topics via MQTT client
mosquitto_sub -t "pico/#" -v
mosquitto_sub -t "file/#" -v
```

## Project Structure

```
mqtt-sn-pico-client/
├── mqtt-sn-pico-client-rx.c   # Receiver firmware
├── mqtt-sn-pico-client-tx.c   # Transmitter firmware
├── mqtt-client.c/h            # Shared MQTT client library
├── config.h                   # Wi-Fi and gateway configuration
├── build.sh                   # Build script for firmware
├── CMakeLists.txt            # Build configuration
├── drivers/                   # MicroSD card driver
│   ├── microsd_driver.c/h    # SD card SPI interface
│   ├── microsd_chunk.c       # Chunk-based operations
│   ├── microsd_file.c        # File operations
│   └── tests/                # Driver test programs
├── fs/                       # File system utilities
│   ├── chunk_transfer.c/h    # Transfer session management
│   ├── data_frame.c/h        # Serialization and CRC
│   └── tests/                # FS test programs
└── mqtt/                     # MQTT-SN protocol
    └── mqtt-sn-udp.c/h       # UDP transport layer

paho.mqtt-sn.embedded-c-master/
└── MQTTSNGateway/            # Eclipse Paho MQTT-SN Gateway
    ├── gateway.conf          # Gateway configuration
    ├── predefinedTopic.conf  # Topic ID mappings
    ├── clients.conf          # Client authentication
    ├── MQTT-SNGateway        # Gateway executable (after build)
    └── src/                  # Gateway source code

dashboard/
├── backend/                  # Node.js backend server
│   ├── server.js            # Express + Socket.IO + MQTT client
│   ├── qos.js               # QoS message handling
│   └── package.json         # Dependencies
└── frontend/                 # Web UI
    ├── index.html           # Dashboard HTML
    ├── client.js            # Browser-side logic
    └── (served at http://localhost:3000)
```

## File Transfer Protocol

### Architecture
The file transfer system uses a chunk-based approach with CRC verification:

- **Chunk Size**: 237 bytes of data per chunk
- **Packet Size**: 247 bytes total (237 data + 10 overhead)
- **QoS Level**: QoS 1 for reliable delivery
- **Verification**: CRC16 checksum per chunk
- **Session Management**: Unique session IDs prevent conflicts

### Data Structures
- **Metadata**: Session ID, filename, size, chunk count, timestamp, file CRC
- **Payload**: Sequence number, 237-byte data block, size, CRC16

### Flow
1. Sender reads file from microSD
2. File is chunked into 237-byte blocks
3. Metadata published to Topic ID 3 (file/meta)
4. Each chunk published to Topic ID 4 (file/data)
5. Receiver validates CRC and writes to microSD
6. Bitmap tracks received chunks
7. On completion, file is finalized

See [`FILE_TRANSFER_README.md`](FILE_TRANSFER_README.md) for detailed protocol documentation.

## Troubleshooting

### Common Issues

**Wi-Fi Connection Fails**
- Verify SSID and password in `config.h`
- Check Wi-Fi network is 2.4GHz (Pico W doesn't support 5GHz)
- Ensure network allows new devices

**MQTT-SN Gateway Connection Fails**
- Verify gateway IP address in `config.h`
- Check UDP port 10000 is not blocked by firewall
- Ensure gateway is running: `./MQTT-SNGateway`
- Check gateway logs for connection attempts

**MicroSD Card Not Detected**
- Verify SPI connections (GP10-GP15)
- Ensure card is formatted as exFAT
- Try different microSD card (some cards have compatibility issues)
- Check driver logs: `microsd_set_log_level(MICROSD_LOG_DEBUG)`

**File Transfer Fails**
- Ensure `test.txt` exists in microSD root directory
- Check available space on receiving device
- Monitor for CRC errors in console
- Verify topics are configured in gateway

**Build Errors**
- Set `PICO_SDK_PATH`: `export PICO_SDK_PATH=/path/to/pico-sdk`
- Ensure Pico SDK submodules initialized: `git submodule update --init`
- Install ARM toolchain: `arm-none-eabi-gcc`
- Clear build directory: `./build.sh clean`

## Performance Characteristics

- **Transfer Rate**: ~50ms per 237-byte chunk (configurable)
- **Throughput**: ~4.7 KB/s (limited by delay between chunks)
- **Overhead**: ~4.2% (10 bytes overhead per 237-byte chunk)
- **Reliability**: 99%+ with QoS 1 and 3 retransmissions
- **Maximum File Size**: Limited by available RAM for bitmap (~64KB practical limit)

## Development

### Adding New Features
1. Modify source files in `mqtt-sn-pico-client/`
2. Rebuild: `./build.sh build`
3. Flash updated firmware: `./build.sh flash`
4. Monitor via serial: `./build.sh monitor`

### Testing
The project includes test programs:
- `drivers/tests/microsd_demo.c` - SD card read/write test
- `drivers/tests/large_file_test.c` - Large file operations
- `fs/tests/chunk_write_example.c` - Chunk writing test
- `fs/tests/file_transfer_demo.c` - File transfer simulation

Build tests individually:
```bash
cd mqtt-sn-pico-client/build
make microsd_demo
make large_file_test
```

## References

- [Eclipse Paho MQTT-SN](https://www.eclipse.org/paho/index.php?page=components/mqtt-sn-transparent-gateway/index.php)
- [MQTT-SN Protocol Specification](http://mqtt.org/new/wp-content/uploads/2009/06/MQTT-SN_spec_v1.2.pdf)
- [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
- [Pico W Datasheet](https://datasheets.raspberrypi.com/picow/pico-w-datasheet.pdf)

## License

Copyright © 2025 CS31 (MQTT-SN via UDP), INF2004 Project Team
