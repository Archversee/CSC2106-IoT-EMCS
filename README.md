# CSC2106 IoT EMCS - Heterogeneous IoT Communication System
# Group 11

A heterogeneous IoT communication system evaluating MQTT-SN as a transport protocol across dual wireless networks: UDP over Wi-Fi and multi-hop LoRa mesh. The system features a Raspberry Pi 4 gateway bridging both transports into a unified MQTT broker, AES-128 payload encryption on the LoRa link, a lightweight mesh protocol supporting flooding and static routing, and a real-time performance dashboard.

## Key Features

### Pico W (Wi-Fi) Client
- **FreeRTOS Architecture**: Two concurrent tasks : MQTT state machine and button handler with mutex-protected shared state
- **MQTT-SN over UDP**: Lightweight messaging over lwIP via the cyw43 threadsafe background Wi-Fi driver
- **QoS Support**: QoS 0, 1, and 2 with automatic retransmission and configurable retry limits
- **Runtime Control**: Three GPIO buttons for QoS level cycling, topic switching, and message publishing
- **Auto-reconnect**: Retry loop in network initialisation handles first-boot Wi-Fi connection failures

### Arduino LoRa Mesh Client
- **MQTT-SN over LoRa**: Custom MQTT-SN client using the RadioHead RFM95 library
- **Mesh Protocol**: 4-byte mesh header (Source ID, Destination ID, TTL, Sequence Number) with TTL-based hop limiting
- **Dual Forwarding Modes**: Flooding and static routing selectable at compile time via `MESH_MODE` in `config.h`
- **AES-128 Encryption**: Payload encrypted before LoRa transmission using AESLib, decrypted at gateway via OpenSSL
- **Deduplication**: Circular buffer of size 8 keyed on Source ID and Sequence Number prevents relay loops
- **Node Roles**: Endpoint and relay-only roles configurable via `NODE_SELECT` in `config.h`
- **OLED Display**: SSD1306 display for on-device status feedback
- **Collision Avoidance**: Per-node jitter delays and CSMA listen-before-talk window

### Raspberry Pi 4 Gateway
- **Dual Transport Bridging**: Receives MQTT-SN from Pico W over UDP and from Arduino nodes over LoRa simultaneously
- **LoRa–UDP Bridge**: Two POSIX threads - main thread polls LR1121 interrupt for uplink frames, downstream thread forwards Paho gateway responses back to LoRa nodes
- **AES-128 Decryption**: Decrypts all incoming LoRa payloads using OpenSSL before forwarding to Paho gateway
- **Client Table**: Maps node IDs to per-client UDP sockets with mutex protection
- **Downlink Spacing**: 400ms delay between consecutive LoRa downlink transmissions to handle half-duplex radio timing
- **Paho MQTT-SN Gateway**: Translates MQTT-SN messages into standard MQTT for the Mosquitto broker
- **Mosquitto Broker**: Locally hosted, receives all translated MQTT traffic from both transport layers

### Dashboard
- **Real-time Monitoring**: Live device status, message feed, and per-node traffic visualisation
- **Flooding vs Routing Comparison**: Side-by-side run comparison with packet delivery ratio, duplicate count, and throughput metrics
- **Command Interface**: Send LED on/off commands to all nodes, Pico-only, or Arduino-only via selectable QoS levels
- **WebSocket Updates**: Low-latency frontend updates via Socket.IO
- **Data Export**: JSON export of collected run statistics

---

## Hardware Requirements

### Pico W Nodes
- Raspberry Pi Pico W
- USB cable for power and programming

### Arduino LoRa Nodes
- Maker UNO (Arduino-compatible)
- Cytron LoRa-RFM Shield (RFM95, 915/923 MHz)
- SSD1306 OLED display
- USB cable for programming and power

### Gateway
- Raspberry Pi 4
- Waveshare Core1121-HF LoRa transceiver (SPI-connected)
- Node.js v16+ with npm

---

## Software Dependencies

### Pico W Firmware
- Pico SDK 2.2.0 (includes lwIP and FreeRTOS kernel)
- ARM GCC Toolchain 14.2 Rel1
- CMake 3.5+

### Arduino LoRa Client
- Arduino IDE with Maker UNO board support
- RadioHead library (RFM95 LoRa driver)
- AESLib library (payload encryption)
- SSD1306 ASCII library (OLED display)

### Gateway
- GCC/G++ compiler
- Make
- WiringPi (required for Waveshare1121-Gateway)
- OpenSSL (AES-128 decryption)
- Mosquitto MQTT broker
- Paho MQTT-SN Gateway (built from source)

### Dashboard
- Node.js v16+ and npm
- npm packages: express, socket.io, mqtt, cors (auto-installed via `npm install`)


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

/* 1. SELECT NODE */
#define NODE_SELECT 4 // SELECT 2(Endpoint) ,3(RELAY) ,4(ENDPOINT) ,5(RELAY)  FOR PRESET CONFIGS
/* 2. SELECT MODE */
#define MESH_MODE MESH_MODE_ROUTING // ALL NODES need to have the same mesh mode for mesh to work

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

## RPI4 with Waveshare Core 1121-HF SPI Wiring Instructions
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
- `build/pico-client-IoT.uf2` - Flash to Pico # (Remember to change config if running multiple)

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
sudo nano /etc/mosquitto/conf.d/local.conf

Add:
listener 1883
allow_anonymous true

sudo mosquitto -v -c /etc/mosquitto/conf.d/local.conf
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
#### Step 4: Start Dashboard Backend

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

### Step 7: Open Dashboard

Navigate to: `http://localhost:3000`

---

## Device Behaviour

### Pico W (`pico-client-IoT.uf2`)
The Pico W firmware runs a FreeRTOS-based MQTT-SN client over Wi-Fi (UDP). Once connected, it registers and uses the following topics:
**Button controls (physical buttons on the breadboard):**
- **GP20** - Publish a message on the currently selected topic
- **GP21** - Cycle through QoS levels (0 → 1 → 2) for outgoing publishes
- **GP22** - Switch the active publish topic between `sensors/data` and `sensors/pico/data`

**Topics registered:**
| Topic | Role | QoS |
| `sensors/data` | Publish (switchable via GP22) | Selectable via GP21 |
| `sensors/pico/data` | Publish (switchable via GP22) | Selectable via GP21 |
| `sensors/cmd` | Subscribe - receives commands | 1 |
| `sensors/pico/cmd` | Subscribe - receives Pico-specific commands | 2 |
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
| `sensors/data` | Publish - general sensor readings | 1 |
| `sensors/arduino/data` | Publish - Arduino-specific sensor readings | 0 |
| `sensors/cmd` | Subscribe - receives commands for all nodes | 1 |
| `sensors/arduino/cmd` | Subscribe - receives Arduino-specific commands | 2 |
> Each Arduino node must have a unique `LORA_MY_NODE_ID` and `MQTT_SN_CLIENT_ID` in set with the presets`config.h`.
> Every Arduino node must have a the same  `MESH_MODE` in set with the presets`config.h`.

## MQTT Topic Structure

| Topic | Publisher | Subscriber | Purpose | QoS |
| `sensors/data` | Pico W / Arduino | Broker / Dashboard | General sensor data from any node | Selectable |
| `sensors/pico/data` | Pico W | Broker / Dashboard | Pico-specific sensor data | Selectable |
| `sensors/arduino/data` | Arduino | Broker / Dashboard | Arduino-specific sensor data | - |
| `sensors/cmd` | Broker / Dashboard | Pico W + Arduino | Commands to all nodes | 1 |
| `sensors/pico/cmd` | Broker / Dashboard | Pico W | Commands to Pico nodes only | 2 |
| `sensors/arduino/cmd` | Broker / Dashboard | Arduino | Commands to Arduino nodes only | - |

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

- **CSC2106 Project Team 11** - MQTT-SN and LoRa implementation
- **Eclipse Paho** - MQTT-SN library and gateway
- **Raspberry Pi Foundation** - Pico SDK and FreeRTOS kernel port
- **FreeRTOS.org** - Real-time operating system kernel

---

## References

- [MQTT-SN Protocol Specification v1.2](http://mqtt.org/new/wp-content/uploads/2009/06/MQTT-SN_spec_v1.2.pdf)
- [Pico W SDK Documentation](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)
- [FreeRTOS Documentation](https://www.freertos.org/Documentation/RTOS_book.html)
- [Eclipse Paho MQTT-SN](https://github.com/eclipse/paho.mqtt-sn.embedded-c)
- [Mosquitto MQTT Broker](https://mosquitto.org/)

---

**Last Updated**: March 5, 2026

