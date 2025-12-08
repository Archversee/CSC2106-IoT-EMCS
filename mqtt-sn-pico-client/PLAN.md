# FreeRTOS Integration Plan

## 1. Configuration & Build System
- [ ] Create `FreeRTOSConfig.h` with appropriate settings for RP2040.
- [ ] Update `CMakeLists.txt` to fetch/link `FreeRTOS-Kernel` and switch to `pico_cyw43_arch_lwip_threadsafe_background`.

## 2. Shared Infrastructure
- [ ] Define Inter-Task Communication (ITC) structures (Queues, Events).
- [ ] Refactor `mqtt-client.c` to be thread-safe (add Mutexes for shared resources).

## 3. TX Client Refactoring (`mqtt-sn-pico-client-tx.c`)
- [ ] Create `AppTask` (Main logic).
- [ ] Create `ButtonTask` to poll buttons and send events to `AppTask`.
- [ ] Create `MQTTTask` to handle network traffic and protocol state.
- [ ] Replace `sleep_ms` with `vTaskDelay`.

## 4. RX Client Refactoring (`mqtt-sn-pico-client-rx.c`)
- [ ] Create `AppTask` (Main logic).
- [ ] Create `MQTTTask` for receiving data.
- [ ] Ensure SD card operations do not starve the network task.

## 5. Testing & Verification
- [ ] Verify compilation.
- [ ] Verify Wi-Fi connection.
- [ ] Verify MQTT-SN connection.
- [ ] Verify File Transfer functionality.
