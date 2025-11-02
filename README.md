# INF2004 Project - MQTT-SN Pico W Client with File Transfer

This project implements a comprehensive MQTT-SN client on Raspberry Pi Pico W with microSD card support and reliable file transfer capabilities over UDP.

## Features

- **MQTT-SN Protocol**: Full implementation with QoS 0/1/2 support
- **File Transfer System**: Chunk-based reliable file transfer with CRC verification
- **MicroSD Card Driver**: exFAT filesystem support with chunk-based read/write
- **Wi-Fi Connectivity**: CYW43 wireless interface with automatic reconnection
- **GPIO Controls**: Buttons for message publishing, QoS selection, and file transfer
- **MQTT-SN Gateway**: Eclipse Paho gateway for MQTT-SN ↔ MQTT translation

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

## Quick Start

### 1. Setup Environment
```bash
./build.sh setup
```

### 2. Build Pico W Firmware
```bash
./build.sh build
```

### 3. Build MQTT-SN Gateway (Laptop)
```bash
./build.sh gateway
```

### 4. Flash Pico W
```bash
./build.sh flash
```

## Build Commands

| Command              | Description                                     |
| -------------------- | ----------------------------------------------- |
| `./build.sh build`   | Build Pico W firmware (auto-detects Ninja/Make) |
| `./build.sh gateway` | Build MQTT-SN Gateway for laptop                |
| `./build.sh all`     | Build both components                           |
| `./build.sh clean`   | Clean all build artifacts                       |
| `./build.sh flash`   | Instructions for flashing Pico W                |
| `./build.sh monitor` | Serial monitoring instructions                  |
| `./build.sh setup`   | Check development environment                   |

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
├── mqtt-sn-pico-client.c      # Main application
├── config.h                    # Wi-Fi and gateway configuration
├── CMakeLists.txt             # Build configuration
├── drivers/                    # MicroSD card driver
│   ├── microsd_driver.c/h     # SD card SPI interface
│   ├── microsd_spi.c/h        # Low-level SPI operations
│   └── tests/                 # Driver test programs
├── fs/                        # File system utilities
│   ├── chunk_transfer.c/h     # Transfer session management
│   ├── data_frame.c/h         # Serialization and CRC
│   └── tests/                 # FS test programs
├── mqtt/                      # MQTT-SN protocol
│   └── mqtt-sn-udp.c/h        # UDP transport layer
└── docs/                      # Documentation
    ├── chunk_based_write_flow.md
    ├── directory_expansion_implementation.md
    └── microsd_read_write_sequence.md

paho.mqtt-sn.embedded-c-master/
└── MQTTSNGateway/             # Eclipse Paho MQTT-SN Gateway
    ├── gateway.conf           # Gateway configuration
    ├── predefinedTopic.conf   # Topic ID mappings
    ├── clients.conf           # Client authentication
    └── src/                   # Gateway source code
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

Copyright © 2024 INF2004 Team
