# MQTT-SN Client RX/TX Split

This document describes the separation of the MQTT-SN client into receiver and sender versions.

## Files Created

### 1. `mqtt-sn-pico-client-rx.c` (Receiver)
**Purpose**: Receives MQTT-SN messages and file transfers

**Key Features**:
- Subscribes to topics (pico/cmd, file/meta, file/data)
- Receives incoming messages
- Receives file transfers and writes to microSD
- Can still send test messages (for testing)

**Button Configuration**:
- GP20: Send test message (for testing purposes)
- GP21: Toggle QoS level
- GP22: Toggle ACK dropping (for testing retransmissions)

**Topics Subscribed**:
- Topic ID 1 (pico/cmd): Command topic
- Topic ID 3 (file/meta): File metadata
- Topic ID 4 (file/data): File chunks

**Unique Methods**:
- `subscribe_receiver_topics()`: Subscribes to all necessary topics for receiving

### 2. `mqtt-sn-pico-client-tx.c` (Sender)
**Purpose**: Sends MQTT-SN messages and initiates file transfers

**Key Features**:
- Publishes messages to topics
- Initiates file transfers from microSD
- Does NOT subscribe to topics (sender only)

**Button Configuration**:
- GP20: Send test message
- GP21: Toggle QoS level
- GP22: **Initiate file transfer** (reads test.txt and sends)
- GP19: Toggle ACK dropping (for testing retransmissions)

**Topics Published**:
- Topic ID 2 (pico/status): Status messages
- Topic ID 3 (file/meta): File metadata (during transfer)
- Topic ID 4 (file/data): File chunks (during transfer)

## Key Differences

| Feature | RX (Receiver) | TX (Sender) |
|---------|---------------|-------------|
| Subscribes to topics | ✅ Yes | ❌ No |
| Publishes messages | ✅ (test only) | ✅ Yes |
| Receives file transfers | ✅ Yes | ❌ No |
| Sends file transfers | ❌ No | ✅ Yes |
| File transfer button | ❌ Not used | ✅ GP22 |
| Primary mode | Listen/Receive | Send/Publish |

## Common Functionality (Both Files)

Both files share these helper functions:
- `initialize_microsd()`: Initialize microSD card with retry logic
- `check_microsd_present()`: Check if SD card is still accessible
- WiFi connection and reconnection logic
- MQTT-SN ping/pong handling
- QoS timeout and retransmission handling
- Hot-plug SD card detection

## Compilation

You'll need to update your `CMakeLists.txt` to build both executables:

```cmake
# Receiver executable
add_executable(mqtt-sn-pico-client-rx
    mqtt-sn-pico-client-rx.c
)

target_link_libraries(mqtt-sn-pico-client-rx
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    drivers
    fs
    mqtt
)

pico_add_extra_outputs(mqtt-sn-pico-client-rx)

# Sender executable
add_executable(mqtt-sn-pico-client-tx
    mqtt-sn-pico-client-tx.c
)

target_link_libraries(mqtt-sn-pico-client-tx
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    drivers
    fs
    mqtt
)

pico_add_extra_outputs(mqtt-sn-pico-client-tx)
```

## Usage

### For Receiver Pico:
1. Flash `mqtt-sn-pico-client-rx.uf2` to the Pico
2. Insert microSD card (for receiving files)
3. The Pico will automatically subscribe to topics
4. Wait for incoming messages and file transfers

### For Sender Pico:
1. Flash `mqtt-sn-pico-client-tx.uf2` to the Pico
2. Insert microSD card with `test.txt` file
3. Press GP22 to initiate file transfer
4. Press GP20 to send test messages

## Configuration

Both versions use the same `config.h` file. The `IS_RECEIVER` flag is still present but is effectively ignored since the functionality is now separated by file.

## Benefits of This Separation

1. **Clearer Code**: Each file has a specific purpose
2. **Reduced Memory**: Each version only includes necessary functionality
3. **Easier Debugging**: Can focus on sender or receiver issues independently
4. **Role Clarity**: Physical devices are clearly designated as sender or receiver
5. **Optimized Builds**: No conditional compilation needed within the main loop
