# INF2004 Project - Pico W + MQTT-SN Gateway

This project demonstrates microSD card operations on Raspberry Pi Pico W and includes an MQTT-SN Gateway for laptop communication.

## Components

- **Pico W Firmware**: MicroSD card driver and demonstration
- **MQTT-SN Gateway**: Runs on laptop for MQTT-SN over UDP communication
- **MQTT Broker**: Runs on laptop for MQTT communication


## Quick Start

### 1. Setup Environment
```bash
./build.sh setup
```

### 2. Build Pico W Firmware
```bash
./build.sh pico
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

| Command | Description |
|---------|-------------|
| `./build.sh pico` | Build Pico W firmware only |
| `./build.sh gateway` | Build MQTT-SN Gateway for laptop |
| `./build.sh all` | Build both components |
| `./build.sh clean` | Clean all build artifacts |
| `./build.sh flash` | Instructions for flashing Pico W |
| `./build.sh monitor` | Serial monitoring instructions |

## Hardware Setup

### Pico W Pin Configuration (Cytron Kit)
- **SD_MISO**: GPIO 12
- **SD_MOSI**: GPIO 11  
- **SD_SCK**: GPIO 10
- **SD_CS**: GPIO 15
- **SPI Port**: SPI1

## Requirements

### For Pico W Development
- Pico SDK installed and `PICO_SDK_PATH` set
- CMake 3.13+
- GCC ARM toolchain
- Make

### For MQTT-SN Gateway (Laptop)
- GCC compiler
- Make
- Network connectivity

### For MQTT-SN Broker (Laptop)
- Mosquitto
- Network connectivity

## Usage

1. Build and flash the Pico W firmware
2. Insert microSD card into Cytron Pico W kit
3. Connect via serial terminal (115200 baud)
4. Run Mosquitto Broker on laptop for MQTT communication
5. Run MQTT-SN Gateway on laptop for MQTT-SN (UDP) <-> MQTT (TCP/IP) translation

## File Structure

```
├── drivers/
│   ├── microsd_driver.h
│   ├── microsd_driver.c
│   └── CMakeLists.txt
├── microsd_demo.c
├── build.sh
├── CMakeLists.txt
└── paho.mqtt-sn.embedded-c/
    └── MQTTSNGateway/
```
