# CSC2106 IoT EMCS - Heterogeneous IoT Communication System

A comprehensive IoT communication system featuring multiple wireless protocols (MQTT-SN and LoRa), multi-hop mesh networking, heterogeneous gateway architecture, and real-time performance visualization dashboard.

---

## 📋 Project Implementation Roadmap

### MQTT-SN Implementation ✅ (COMPLETED)
1. ✅ **Packet building and client side implementation** - FreeRTOS-based multithreaded architecture with MQTT-SN protocol over UDP
2. ✅ **Multi-hop mesh layer (routing/forwarding)** - Sender (TX) and Receiver (RX) Pico W clients with QoS support
3. ✅ **Heterogeneous Gateway for protocol translation on Raspberry Pi** - MQTT-SN Gateway bridging UDP to standard MQTT broker
4. ✅ **Performance collection/eval of transport and comparison** - QoS levels (0, 1, 2) with threadsafe background Wi-Fi driver
5. ✅ **Dashboard and visualisation of results** - Real-time web dashboard for monitoring and control

### LoRa Implementation 🚧 (IN PROGRESS)
1. ✅ **LoRa packet building and Arduino client side implementation** - Custom packet structure with Arduino-based MQTT-SN client (`mqtt-sn-arduino-client`)
2. ⬜ **LoRa multi-hop mesh layer (routing/forwarding)** - Routing protocols and packet forwarding for extended range
3. ✅ **Heterogeneous Gateway for protocol translation on Raspberry Pi** - LoRa to UDP Gateway (`Waveshare1121-Gateway`) bridging LoRa to MQTT-SN Gateway
4. ⬜ **Performance collection/eval of transport and comparison** - Transport layer benchmarking and MQTT-SN vs LoRa comparison analysis
5. ⬜ **Dashboard and visualisation of results** - Extended dashboard with protocol comparison charts and real-time LoRa metrics

---

## Key Features

### MQTT-SN System Features
- **FreeRTOS Integration**: Preemptive multitasking with separate MQTT and button handler tasks
- **Thread-Safe Design**: Mutex-protected shared resources and inter-task communication via queues
- **MQTT-SN Protocol**: Lightweight messaging protocol optimized for constrained devices over UDP
- **QoS Levels**: Support for QoS 0 (at-most-once), QoS 1 (at-least-once), QoS 2 (exactly-once)
- **Retry Logic**: Automatic retransmission on timeout with configurable retry limits
- **Dual Client Architecture**: Separate TX (sender) and RX (receiver) builds
- **Background Wi-Fi Processing**: Automatic network stack servicing via threadsafe background mode

### LoRa System Features
- **Arduino-based LoRa Client**: MQTT-SN over LoRa using the `mqtt-sn-arduino-client` sketch
- **LoRa to UDP Gateway**: `Waveshare1121-Gateway` bridges LoRa packets to the MQTT-SN Gateway on the Raspberry Pi
- **Unique Node Addressing**: Configurable LoRa node IDs per device via `config.h`
- **Multi-topic Support**: Publishes and subscribes across multiple sensor topics

### Dashboard Features
- **Real-time Monitoring**: Live device status and message feeds
- **Command Interface**: Send commands to devices with QoS selection
- **Telemetry Charts**: Live data visualization for sensor readings
- **WebSocket Communication**: Low-latency updates via Socket.IO

---

## Hardware Requirements

### For Pico W Devices
- **2x Raspberry Pi Pico W** (one for TX, one for RX)
- **Breadboards and jumper wires**
- **Push buttons** for testing message sending
- **USB cables** for power and programming

### For Arduino LoRa Devices
- **Arduino board** with LoRa module (Waveshare or compatible)
- **USB cable** for programming and power

### For Dashboard/Gateway
- **Raspberry Pi 4** running Mosquitto broker, Paho MQTT-SN Gateway, and LoRa–UDP Gateway
- **Node.js** v16+ with npm (for dashboard)

---

## Software Dependencies

### Pico W Firmware
- **Pico SDK 2.2.0** (includes lwIP networking stack and FreeRTOS kernel)
- **FreeRTOS Kernel** (included in Pico SDK, configured for SMP on RP2040)
- **ARM GCC Toolchain** 14.2 Rel1
- **CMake** 3.5+
- **Paho MQTT-SN embedded C** (included in repository)

### Arduino LoRa Client
- **Arduino IDE** with board support for your Arduino target
- **`mqtt-sn-arduino-client` sketch** — open in Arduino IDE and flash to device

### MQTT-SN Gateway
- **CMake** for building
- **C++ compiler** (gcc/g++)

### LoRa–UDP Gateway
- **Make** — build and run via `make run` in `Waveshare1121-Gateway/`

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

// MQTT-SN Gateway IP (Raspberry Pi running the paho gateway -- RUN ip address)
#define GATEWAY_IP0 10
#define GATEWAY_IP1 200
#define GATEWAY_IP2 133
#define GATEWAY_IP3 90
#define UDP_PORT 10000

// Pico W MQTT-SN ID (Need to change to unique name (cant clash with other Pico or Arduino))
#define MQTT_SN_CLIENT_ID "pico_w_XX"
```

### 2. Configure Arduino LoRa Client

Edit `/mqtt-sn-arduino-client/config.h`

```cpp

// Lora ID (Need to change to unique name (cant clash with other Arduino that uses LoRa))
#define LORA_MY_NODE_ID 0x2X      

// Arduino MQTT-SN ID  (Need to change to unique name (cant clash with other Pico or Arduino))
#define MQTT_SN_CLIENT_ID "arduino-XX" 

#endif
```

### 3. Configure MQTT-SN Gateway

# Current config shoudnt need to be chnaged
Edit `/paho.mqtt-sn.embedded-c/MQTTSNGateway/gateway.conf`:

```conf
# Broker connection
BrokerName=localhost
BrokerPortNo=1883

# Gateway UDP port (must match UDP_PORT in config.h)
GatewayPortNo=10000
GatewayUDP=YES

# Client authentication
ClientAuthentication=NO
```

---

## Build Instructions
Waveshare core1121-HF to RPI4 GPIO PINS
| Core1121-HF | Connect To Pi 4 | GPIO
| 3.3V        | 3.3V            | 1
| GND         | GND             | 6
| DIO9        | GPI23           | 16
| CS          | GPIO8           | 24
| CLK         | GPIO11          | 23
| MOSI        | GPIO10          | 19
| MISO        | GPIO9           | 21
| RESET       | GPIO22          | 15
| BUSY        | GPIO24          | 18


### Build RPI4 with Waveshare Core 1121-HF SPI
```bash
sudo raspi-config  # enable SPI
sudo reboot

# install packages on RPI4
sudo apt update
sudo apt install -y mosquitto mosquitto-clients
sudo apt install -y build-essential cmake git
sudo apt install -y libssl-dev

####  Copy Project to Raspberry Pi run on local device rmb connect same network

```bash
scp -r CSC2106-IoT-EMCS RPIusername@RIPipaddress.20.10.13:/home/RPIusername/Documents/ 
```

### Build Pico W Firmware

```bash
cd mqtt-sn-pico-client
mkdir -p build
cd build
cmake ..
make -j8
```

Build outputs:
- `build/pico-client-IoT.uf2` — Flash to Pico # (Remember to change config if running multiple)

### Flash Arduino LoRa Client (115200 baud)
1. Open the `mqtt-sn-arduino-client` sketch (`.ino` file) in **Arduino IDE**
2. Update `config.h` with the correct node ID and client ID for this device
3. Select the correct board and port
4. Click **Upload**

---

## Running the System

### Raspberry Pi 4 Setup
# We require 3 terminals running concurrently (Mosquitto broker, Paho Gateway, LoRa Gateway)
# and 1 more for testing 

#### Step 1: Start MQTT Broker (Mosquitto) (Teminal 1)
```bash
mosquitto -v

sudo systemctl stop mosquitto # if mosquitto auto runs on boot and port is in use
```

#### Step 2: Start MQTT-SN Gateway (Terminal 2)
```bash
cd /home/RPIusername/Documents/CSC2106-IoT-EMCS/paho.mqtt-sn.embedded-c/MQTTSNGateway
chmod +x build.sh
./build.sh udp  # Might encounter errors building due to prev cmake delete cached cmake files (build.gateway)
cd bin
./MQTT-SNGateway
```

#### Step 3: Start LoRa → UDP Gateway  (Terminal 3)
```bash 
cd /home/RPIusername/Documents
git clone https://github.com/WiringPi/WiringPi # we require WiringPi to run waveshare gateway onmly need to install once
cd WiringPi
./build

cd /home/RPIusername/Documents/CSC2106-IoT-EMCS/Waveshare1121-Gateway
make run

./build
```

### Dashboard Setup
#### Step 4: Start Dashboard Backend (NOT DONE)

```bash
cd dashboard/backend
npm install  # First time only
node server.js
```

### Device Setup

#### Step 5: Flash Pico W Devices

1. **Hold BOOTSEL button** on Pico W while connecting USB
2. **Drag and drop** `.uf2` file to RPI-RP2 drive
3. Pico will reboot automatically

#### Step 6: Flash Arduino LoRa Devices

Flash the `mqtt-sn-arduino-client` sketch via Arduino IDE (see Build Instructions above).

### Step 7: Open Dashboard (NOT DONE)

Navigate to: `http://localhost:3000`

---

## Device Behaviour

### Pico W (`pico-client-IoT.uf2`)
The Pico W firmware runs a FreeRTOS-based MQTT-SN client over Wi-Fi (UDP). Once connected, it registers and uses the following topics:
**Button controls (physical buttons on the breadboard):**
- **GP20** — Publish a message on the currently selected topic
- **GP21** — Cycle through QoS levels (0 → 1 → 2) for outgoing publishes
- **GP22** — Switch the active publish topic between `sensors/data` and `sensors/pico/data`

**Topics registered:**
| Topic | Role | QoS |
| `sensors/data` | Publish (switchable via GP22) | Selectable via GP21 |
| `sensors/pico/data` | Publish (switchable via GP22) | Selectable via GP21 |
| `sensors/cmd` | Subscribe — receives commands | 1 |
| `sensors/pico/cmd` | Subscribe — receives Pico-specific commands | 2 |
> If multiple Pico W devices are deployed, ensure each has a unique `MQTT_SN_CLIENT_ID` in `config.h` to avoid client ID conflicts on the broker.

---
### Arduino LoRa (`mqtt-sn-arduino-client`)
The Arduino runs an MQTT-SN client over LoRa radio. Packets are forwarded via the `Waveshare1121-Gateway` on the Raspberry Pi, which bridges LoRa → UDP → MQTT-SN Gateway → Mosquitto broker.

THe Arduino LoRa sends
 QoS 1 publish to sensors/data every 30s
QoS 0 publish to sensors/arduino/data every 10s
Payload: "<client_id> <pub_count>"

**Topics registered:**
| Topic | Role | QoS |
| `sensors/data` | Publish — general sensor readings | 1 |
| `sensors/arduino/data` | Publish — Arduino-specific sensor readings | 0 |
| `sensors/cmd` | Subscribe — receives commands for all nodes | 1 |
| `sensors/arduino/cmd` | Subscribe — receives Arduino-specific commands | 2 |
> Each Arduino node must have a unique `LORA_MY_NODE_ID` and `MQTT_SN_CLIENT_ID` in `config.h`.

## MQTT Topic Structure

| Topic | Publisher | Subscriber | Purpose | QoS |
| `sensors/data` | Pico W / Arduino | Broker / Dashboard | General sensor data from any node | Selectable |
| `sensors/pico/data` | Pico W | Broker / Dashboard | Pico-specific sensor data | Selectable |
| `sensors/arduino/data` | Arduino | Broker / Dashboard | Arduino-specific sensor data | — |
| `sensors/cmd` | Broker / Dashboard | Pico W + Arduino | Commands to all nodes | 1 |
| `sensors/pico/cmd` | Broker / Dashboard | Pico W | Commands to Pico nodes only | 2 |
| `sensors/arduino/cmd` | Broker / Dashboard | Arduino | Commands to Arduino nodes only | — |

### Testing MQTT Topics (run on CLI on RPI4)

```bash
# Send commands to all nodes
mosquitto_pub -t sensors/cmd -m "led on"
mosquitto_pub -t sensors/cmd -m "led off"

mosquitto_pub -t sensors/cmd -m "led on" -q 1 # qos level 1

# Send commands to Arduino nodes only
mosquitto_pub -t sensors/arduino/cmd -m ""

# Send commands to Pico W nodes only
mosquitto_pub -t sensors/pico/cmd -m "led on"

# Monitor sensor data
mosquitto_sub -t sensors/data          # All nodes
mosquitto_sub -t sensors/pico/data     # Pico W only
mosquitto_sub -t sensors/arduino/data  # Arduino only

sudo shutdown -h now # power RPI off
```

---

## Troubleshooting

### Pico Won't Connect to Wi-Fi

- Verify `WIFI_SSID` and `WIFI_PASS` in `/mqtt-sn-pico-client/config.h`
- Check Wi-Fi is 2.4GHz (Pico W doesn't support 5GHz)
- Ensure WPA2-AES authentication

### Arduino LoRa Not Communicating

- Verify `LORA_MY_NODE_ID` is unique for each device in `config.h`
- Check that `LORA_GW_NODE_ID` matches the Raspberry Pi gateway node ID (`0x00`)
- Ensure the LoRa–UDP Gateway (`Waveshare1121-Gateway`) is running on the Raspberry Pi

### Gateway Connection Failed

- Verify gateway IP matches Pico configuration
- Check firewall allows UDP port 10000
- Ensure gateway is running before Pico boots
- Use `ping` to verify network connectivity

### Dashboard Not Receiving Messages

- Check Mosquitto is running: `mosquitto -v`
- Verify backend subscribed to correct topics
- Check browser console (F12) for WebSocket errors
- Ensure correct broker URL in `server.js`

---

## License

This project uses components under multiple licenses:

- **Project code**: Developed for CSC2106 coursework
- **Paho MQTT-SN**: Dual-licensed under EPL and EDL
- **Pico SDK**: BSD 3-Clause License

---

## Authors & Credits

- **CSC2106 Project Team** — MQTT-SN and LoRa implementation
- **Eclipse Paho** — MQTT-SN library and gateway
- **Raspberry Pi Foundation** — Pico SDK and FreeRTOS kernel port
- **FreeRTOS.org** — Real-time operating system kernel

---

## References

- [MQTT-SN Protocol Specification v1.2](http://mqtt.org/new/wp-content/uploads/2009/06/MQTT-SN_spec_v1.2.pdf)
- [Pico W SDK Documentation](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)
- [FreeRTOS Documentation](https://www.freertos.org/Documentation/RTOS_book.html)
- [Eclipse Paho MQTT-SN](https://github.com/eclipse/paho.mqtt-sn.embedded-c)
- [Mosquitto MQTT Broker](https://mosquitto.org/)

---

**Last Updated**: March 4, 2026

