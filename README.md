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
1. Add in Arduino Maker own client side code, building MQTT-SN packets over LoRa and sending with correct topics, subscribing to correct topics, output messages
2. Amend Waveshare-gateway main.c to convert LoRa packets to UDP to translate to Paho MQTT-SN Gateway (shld be pretty quick im almost done with this hard to test without 1.)
3. Implement the LoRa Mesh
4. Add statistics into the packets, hop count, latency, payload size
5. Dashboard to view all messages, topics, and statistics.

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

### For Dashboard/Gateway
- **Computer** running macOS/Linux/Windows
- **MQTT Broker** (Mosquitto recommended)
- **Node.js** v16+ with npm

---

## Software Dependencies

### Pico W Firmware
- **Pico SDK 2.2.0** (includes lwIP networking stack and FreeRTOS kernel)
- **FreeRTOS Kernel** (included in Pico SDK, configured for SMP on RP2040)
- **ARM GCC Toolchain** 14.2 Rel1
- **CMake** 3.5+
- **Paho MQTT-SN embedded C** (included in repository)

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

### Build Pico W Firmware

```bash
cd mqtt-sn-pico-client
mkdir -p build
cd build
cmake ..
make -j8
```

Build outputs:
- `build/pico-client-pub.uf2` - Flash to sender Pico (Publisher)
- `build/pico-client-sub.uf2` - Flash to receiver Pico (Subscriber)

### Build MQTT-SN Gateway

```bash
cd paho.mqtt-sn.embedded-c/MQTTSNGateway
mkdir -p build.gateway
cd build.gateway
cmake ..
make
```

Binary output: `build.gateway/MQTT-SNGateway`

---

## Running the System

### Step 1: Start MQTT Broker (Mosquitto)

```bash
# macOS (Homebrew)
brew install mosquitto
mosquitto -v

# Linux
sudo apt-get install mosquitto
mosquitto -v
```

### Step 2: Start MQTT-SN Gateway

```bash
cd paho.mqtt-sn.embedded-c/MQTTSNGateway/build.gateway
./MQTT-SNGateway
```

### Step 3: Start Dashboard Backend

```bash
cd dashboard/backend
npm install  # First time only
node server.js
```

### Step 4: Flash Pico W Devices

1. **Hold BOOTSEL button** on Pico W while connecting USB
2. **Drag and drop** .uf2 file to RPI-RP2 drive
3. Pico will reboot automatically

### Step 5: Open Dashboard

Navigate to: `http://localhost:3000`

---

## MQTT Topic Structure

| Topic Name    | Direction      | Purpose            | QoS |
| ------------- | -------------- | ------------------ | --- |
| `pico/cmd`    | Dashboard → RX | Commands to device | 2   |
| `pico/status` | TX → Dashboard | Status updates     | 0-2 |

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

- **CSC2106 Project Team** - MQTT-SN and LoRa implementation
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

**Last Updated**: February 13, 2026
