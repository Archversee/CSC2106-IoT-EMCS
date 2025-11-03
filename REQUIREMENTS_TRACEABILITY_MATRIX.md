# Requirements Traceability Matrix
## INF2004 MQTT-SN via UDP Project - CS31 Team

**Document Version:** 1.0  
**Date:** 3 November 2025  
**Team:** Po Hao Ting, Neo Sun Wei, Haley Tan Hui Xin, Nico Caleb Lim, Travis Neo Kuang Yi

---

## Matrix Format
```
Requirement ID → Use Case/Scenario ID → Function Name → Test ID → Pass/Fail Status
```

---

## 1. FUNCTIONAL REQUIREMENTS (FR)

### FR1: MQTT-SN Client Implementation (Pico W)

| Req ID | Use Case                     | Function Name                          | File Location           | Test ID | Pass/Fail |
| ------ | ---------------------------- | -------------------------------------- | ----------------------- | ------- | --------- |
| FR1    | UC1 (CONNECT)                | `mqtt_sn_connect()`                    | `mqtt/mqtt-sn-udp.c`    | ST-4    | ✅ PASS    |
| FR1    | UC11 (Session Establishment) | `main()`                               | `mqtt-sn-pico-client.c` | ST-4    | ✅ PASS    |
| FR1    | UC1                          | `cyw43_arch_init()`                    | `mqtt-sn-pico-client.c` | IT-1    | ✅ PASS    |
| FR1    | UC1                          | `cyw43_arch_wifi_connect_timeout_ms()` | `mqtt-sn-pico-client.c` | IT-1    | ✅ PASS    |
| FR1    | UC1                          | `udp_new()`                            | `mqtt-sn-pico-client.c` | IT-1    | ✅ PASS    |
| FR1    | UC1                          | `udp_recv()`                           | `mqtt-sn-pico-client.c` | IT-1    | ✅ PASS    |

---

### FR2: QoS and Reliability

| Req ID | Use Case    | Function Name                | File Location        | Test ID          | Pass/Fail |
| ------ | ----------- | ---------------------------- | -------------------- | ---------------- | --------- |
| FR2    | UC4 (QoS 0) | `mqtt_sn_publish_topic_id()` | `mqtt/mqtt-sn-udp.c` | Manual           | ✅ PASS    |
| FR2    | UC5 (QoS 1) | `mqtt_sn_publish_topic_id()` | `mqtt/mqtt-sn-udp.c` | ST-1             | ✅ PASS    |
| FR2    | UC6 (QoS 2) | `mqtt_sn_publish_topic_id()` | `mqtt/mqtt-sn-udp.c` | ST-2, ST-3       | ✅ PASS    |
| FR2    | UC5         | `mqtt_sn_send_puback()`      | `mqtt/mqtt-sn-udp.c` | ST-1             | ✅ PASS    |
| FR2    | UC6         | `mqtt_sn_send_pubrec()`      | `mqtt/mqtt-sn-udp.c` | ST-2             | ✅ PASS    |
| FR2    | UC6         | `mqtt_sn_send_pubrel()`      | `mqtt/mqtt-sn-udp.c` | ST-3             | ✅ PASS    |
| FR2    | UC6         | `mqtt_sn_send_pubcomp()`     | `mqtt/mqtt-sn-udp.c` | ST-3             | ✅ PASS    |
| FR2    | UC5, UC6    | `check_qos_timeouts()`       | `mqtt/mqtt-sn-udp.c` | ST-1, ST-2, ST-3 | ✅ PASS    |
| FR2    | UC5, UC6    | `remove_pending_qos_msg()`   | `mqtt/mqtt-sn-udp.c` | ST-1, ST-2, ST-3 | ✅ PASS    |
| FR2    | UC5, UC6    | `udp_recv_callback()`        | `mqtt/mqtt-sn-udp.c` | ST-1, ST-2, ST-3 | ✅ PASS    |

---

### FR3: Session Management

| Req ID | Use Case          | Function Name              | File Location           | Test ID | Pass/Fail |
| ------ | ----------------- | -------------------------- | ----------------------- | ------- | --------- |
| FR3    | UC1 (CONNECT)     | `mqtt_sn_connect()`        | `mqtt/mqtt-sn-udp.c`    | ST-4    | ✅ PASS    |
| FR3    | UC11 (Keep-Alive) | `mqtt_sn_pingreq()`        | `mqtt/mqtt-sn-udp.c`    | ST-4    | ✅ PASS    |
| FR3    | UC11              | Main loop ping logic       | `mqtt-sn-pico-client.c` | ST-4    | ✅ PASS    |
| FR3    | UC11              | Reconnection logic         | `mqtt-sn-pico-client.c` | ST-4    | ✅ PASS    |
| FR3    | UC11              | `cyw43_wifi_link_status()` | `mqtt-sn-pico-client.c` | Manual  | ✅ PASS    |

---

### FR4: Data Publishing and Subscribing

| Req ID | Use Case        | Function Name                  | File Location        | Test ID | Pass/Fail |
| ------ | --------------- | ------------------------------ | -------------------- | ------- | --------- |
| FR4    | UC3 (SUBSCRIBE) | `mqtt_sn_subscribe_topic_id()` | `mqtt/mqtt-sn-udp.c` | Manual  | ✅ PASS    |
| FR4    | UC5 (PUBLISH)   | `mqtt_sn_publish_topic_id()`   | `mqtt/mqtt-sn-udp.c` | IT-5    | ✅ PASS    |
| FR4    | UC2 (REGISTER)  | Topic registration logic       | `mqtt/mqtt-sn-udp.c` | Manual  | ✅ PASS    |
| FR4    | UC4, UC5, UC6   | `udp_recv_callback()`          | `mqtt/mqtt-sn-udp.c` | IT-5    | ✅ PASS    |

---

### FR5: File Transfer Driver (Bulk Data Handling)

| Req ID | Use Case             | Function Name                    | File Location         | Test ID    | Pass/Fail |
| ------ | -------------------- | -------------------------------- | --------------------- | ---------- | --------- |
| FR5    | UC8 (Deconstruction) | `deconstruct()`                  | `fs/data_frame.c`     | UT-3, ST-5 | ✅ PASS    |
| FR5    | UC9 (Reconstruction) | `reconstruct()`                  | `fs/data_frame.c`     | UT-4, ST-5 | ✅ PASS    |
| FR5    | UC8                  | `init_streaming_read()`          | `fs/data_frame.c`     | ST-5       | ✅ PASS    |
| FR5    | UC8                  | `read_chunk_streaming()`         | `fs/data_frame.c`     | ST-5       | ✅ PASS    |
| FR5    | UC8                  | `cleanup_streaming_read()`       | `fs/data_frame.c`     | ST-5       | ✅ PASS    |
| FR5    | UC9                  | `chunk_transfer_init_session()`  | `fs/chunk_transfer.c` | ST-5       | ✅ PASS    |
| FR5    | UC9                  | `chunk_transfer_write_payload()` | `fs/chunk_transfer.c` | ST-5       | ✅ PASS    |
| FR5    | UC9                  | `chunk_transfer_is_complete()`   | `fs/chunk_transfer.c` | ST-5       | ✅ PASS    |
| FR5    | UC9                  | `chunk_transfer_finalize()`      | `fs/chunk_transfer.c` | ST-5       | ✅ PASS    |
| FR5    | UC8, UC9             | `serialize_payload()`            | `fs/data_frame.c`     | UT-2       | ✅ PASS    |
| FR5    | UC8, UC9             | `deserialize_payload()`          | `fs/data_frame.c`     | UT-2       | ✅ PASS    |
| FR5    | UC8, UC9             | `serialize_metadata()`           | `fs/data_frame.c`     | UT-2       | ✅ PASS    |
| FR5    | UC8, UC9             | `deserialize_metadata()`         | `fs/data_frame.c`     | UT-2       | ✅ PASS    |
| FR5    | UC8, UC9             | `crc16()`                        | `fs/data_frame.c`     | UT-2       | ✅ PASS    |
| FR5    | UC9                  | `verify_chunk()`                 | `fs/data_frame.c`     | UT-2       | ✅ PASS    |
| FR5    | UC8, UC9             | `send_file_via_mqtt()`           | `mqtt/mqtt-sn-udp.c`  | ST-5       | ✅ PASS    |
| FR5    | UC9                  | `handle_file_metadata()`         | `mqtt/mqtt-sn-udp.c`  | ST-5       | ✅ PASS    |
| FR5    | UC9                  | `handle_file_payload()`          | `mqtt/mqtt-sn-udp.c`  | ST-5       | ✅ PASS    |

---

### FR6-FR9: Gateway, Broker, Dashboard (External Components)

| Req ID | Use Case  | Component             | Test ID | Pass/Fail |
| ------ | --------- | --------------------- | ------- | --------- |
| FR6    | UC1, UC11 | Paho MQTT-SN Gateway  | IT-1    | ✅ PASS    |
| FR7    | UC1-UC11  | Mosquitto MQTT Broker | IT-5    | ✅ PASS    |
| FR8    | UC7       | Subscriber Dashboard  | IT-5    | ✅ PASS    |
| FR9    | All       | Dashboard logging     | Manual  | ✅ PASS    |

---

## 2. DEMONSTRATION APPLICATION REQUIREMENTS (FR-A)

### FR-A1: Telemetry Publishing (Button GP20)

| Req ID | Use Case | Function Name                | File Location           | Test ID | Pass/Fail |
| ------ | -------- | ---------------------------- | ----------------------- | ------- | --------- |
| FR-A1  | UC7      | Button polling logic (GP20)  | `mqtt-sn-pico-client.c` | Manual  | ✅ PASS    |
| FR-A1  | UC7      | `mqtt_sn_publish_topic_id()` | `mqtt/mqtt-sn-udp.c`    | Manual  | ✅ PASS    |
| FR-A1  | UC7      | `gpio_get()`                 | `mqtt-sn-pico-client.c` | Manual  | ✅ PASS    |

---

### FR-A2: QoS Selection (Button GP21)

| Req ID | Use Case | Function Name               | File Location           | Test ID | Pass/Fail |
| ------ | -------- | --------------------------- | ----------------------- | ------- | --------- |
| FR-A2  | N/A      | Button polling logic (GP21) | `mqtt-sn-pico-client.c` | IT-4    | ✅ PASS    |
| FR-A2  | N/A      | QoS level cycling           | `mqtt-sn-pico-client.c` | IT-4    | ✅ PASS    |

---

### FR-A3: Control Topic Subscription

| Req ID | Use Case | Function Name                  | File Location           | Test ID | Pass/Fail |
| ------ | -------- | ------------------------------ | ----------------------- | ------- | --------- |
| FR-A3  | UC3      | `mqtt_sn_subscribe_topic_id()` | `mqtt/mqtt-sn-udp.c`    | IT-3    | ✅ PASS    |
| FR-A3  | UC3      | Topic ID 1 subscription        | `mqtt-sn-pico-client.c` | IT-3    | ✅ PASS    |

---

### FR-A4: LED Control

| Req ID | Use Case | Function Name         | File Location        | Test ID | Pass/Fail |
| ------ | -------- | --------------------- | -------------------- | ------- | --------- |
| FR-A4  | UC7      | `udp_recv_callback()` | `mqtt/mqtt-sn-udp.c` | IT-3    | ✅ PASS    |
| FR-A4  | UC7      | LED control logic     | `mqtt/mqtt-sn-udp.c` | IT-3    | ✅ PASS    |

---

### FR-A5: File Deconstruction

| Req ID | Use Case | Function Name            | File Location     | Test ID | Pass/Fail |
| ------ | -------- | ------------------------ | ----------------- | ------- | --------- |
| FR-A5  | UC8      | `deconstruct()`          | `fs/data_frame.c` | UT-3    | ✅ PASS    |
| FR-A5  | UC8      | `init_streaming_read()`  | `fs/data_frame.c` | UT-3    | ✅ PASS    |
| FR-A5  | UC8      | `read_chunk_streaming()` | `fs/data_frame.c` | UT-3    | ✅ PASS    |

---

### FR-A6: File Reconstruction

| Req ID | Use Case | Function Name                    | File Location         | Test ID | Pass/Fail |
| ------ | -------- | -------------------------------- | --------------------- | ------- | --------- |
| FR-A6  | UC9      | `reconstruct()`                  | `fs/data_frame.c`     | UT-4    | ✅ PASS    |
| FR-A6  | UC9      | `chunk_transfer_write_payload()` | `fs/chunk_transfer.c` | UT-4    | ✅ PASS    |
| FR-A6  | UC9      | `chunk_transfer_finalize()`      | `fs/chunk_transfer.c` | UT-4    | ✅ PASS    |

---

### FR-A7: Reliability Test (Drop ACKs - Button GP22)

| Req ID | Use Case | Function Name                         | File Location           | Test ID          | Pass/Fail |
| ------ | -------- | ------------------------------------- | ----------------------- | ---------------- | --------- |
| FR-A7  | N/A      | Button polling logic (GP22)           | `mqtt-sn-pico-client.c` | ST-1, ST-2, ST-3 | ✅ PASS    |
| FR-A7  | N/A      | `drop_acks` flag toggle               | `mqtt-sn-pico-client.c` | ST-1, ST-2, ST-3 | ✅ PASS    |
| FR-A7  | N/A      | ACK dropping in `udp_recv_callback()` | `mqtt/mqtt-sn-udp.c`    | ST-1, ST-2, ST-3 | ✅ PASS    |

---

## 3. NON-FUNCTIONAL REQUIREMENTS (NFR)

### NFR1: Reliability (QoS Retries)

| Req ID | Metric            | Function Name           | File Location        | Test ID          | Pass/Fail |
| ------ | ----------------- | ----------------------- | -------------------- | ---------------- | --------- |
| NFR1   | Max 3 retries     | `check_qos_timeouts()`  | `mqtt/mqtt-sn-udp.c` | ST-1, ST-2, ST-3 | ✅ PASS    |
| NFR1   | 2s timeout        | `QOS_RETRY_INTERVAL_US` | `config.h`           | ST-1, ST-2, ST-3 | ✅ PASS    |
| NFR1   | 99% delivery rate | Full system             | End-to-end           | ST-5             | ✅ PASS    |
| NFR1   | Retry logic       | `g_pending_msgs` array  | `mqtt/mqtt-sn-udp.c` | ST-1, ST-2, ST-3 | ✅ PASS    |

---

### NFR2: Latency

| Req ID | Metric             | Component                     | Test ID | Pass/Fail |
| ------ | ------------------ | ----------------------------- | ------- | --------- |
| NFR2   | ≤150ms avg latency | Full stack (Pico → Dashboard) | IT-5    | ✅ PASS    |
| NFR2   | ≤300ms max spike   | Full stack under stress       | Manual  | ✅ PASS    |

---

### NFR3: Efficiency (Chunk Size)

| Req ID | Metric                 | Constant/Function     | File Location     | Test ID | Pass/Fail |
| ------ | ---------------------- | --------------------- | ----------------- | ------- | --------- |
| NFR3   | ≤247 bytes per packet  | `PAYLOAD_SIZE`        | `fs/data_frame.h` | UT-2    | ✅ PASS    |
| NFR3   | 237 bytes data         | `PAYLOAD_DATA_SIZE`   | `fs/data_frame.h` | UT-2    | ✅ PASS    |
| NFR3   | Serialization overhead | `serialize_payload()` | `fs/data_frame.c` | UT-2    | ✅ PASS    |

---

### NFR4: Usability

| Req ID | Metric                | Component | Test ID | Pass/Fail |
| ------ | --------------------- | --------- | ------- | --------- |
| NFR4   | Telemetry visible <2s | Dashboard | IT-5    | ✅ PASS    |
| NFR4   | UI response <200ms    | Dashboard | Manual  | ✅ PASS    |

---

### NFR5: Data Integrity

| Req ID | Metric             | Function Name          | File Location     | Test ID | Pass/Fail |
| ------ | ------------------ | ---------------------- | ----------------- | ------- | --------- |
| NFR5   | ≤0.1% corruption   | Full system            | End-to-end        | ST-5    | ✅ PASS    |
| NFR5   | 100% CRC accuracy  | `crc16()`              | `fs/data_frame.c` | UT-2    | ✅ PASS    |
| NFR5   | Chunk verification | `verify_chunk()`       | `fs/data_frame.c` | UT-2    | ✅ PASS    |
| NFR5   | File CRC           | `file_crc` in Metadata | `fs/data_frame.h` | ST-5    | ✅ PASS    |

---

## 4. USE CASE TO DRIVER INTERFACE MAPPING

### UC10: MicroSD Card Read & Write

| Req ID | Use Case                 | Function Name                         | File Location              | Test ID | Pass/Fail |
| ------ | ------------------------ | ------------------------------------- | -------------------------- | ------- | --------- |
| UC10   | Read byte                | `microsd_read_byte()`                 | `drivers/microsd_driver.c` | UT-5    | ✅ PASS    |
| UC10   | Write byte               | `microsd_write_byte()`                | `drivers/microsd_driver.c` | UT-5    | ✅ PASS    |
| UC10   | Read block               | `microsd_read_block()`                | `drivers/microsd_driver.c` | UT-5    | ✅ PASS    |
| UC10   | Write block              | `microsd_write_block()`               | `drivers/microsd_driver.c` | UT-5    | ✅ PASS    |
| UC10   | Initialize               | `microsd_init()`                      | `drivers/microsd_driver.c` | IT-2    | ✅ PASS    |
| UC10   | Filesystem init          | `microsd_init_filesystem()`           | `drivers/microsd_driver.c` | IT-2    | ✅ PASS    |
| UC10   | Create file              | `microsd_create_file()`               | `drivers/microsd_driver.c` | IT-2    | ✅ PASS    |
| UC10   | Read file                | `microsd_read_file()`                 | `drivers/microsd_driver.c` | IT-2    | ✅ PASS    |
| UC10   | Chunked write init       | `microsd_init_chunk_write()`          | `drivers/microsd_driver.c` | ST-5    | ✅ PASS    |
| UC10   | Write chunk              | `microsd_write_chunk()`               | `drivers/microsd_driver.c` | ST-5    | ✅ PASS    |
| UC10   | Finalize chunk write     | `microsd_finalize_chunk_write()`      | `drivers/microsd_driver.c` | ST-5    | ✅ PASS    |
| UC10   | Read chunk               | `microsd_read_chunk()`                | `drivers/microsd_driver.c` | ST-5    | ✅ PASS    |
| UC10   | Check all chunks         | `microsd_check_all_chunks_received()` | `drivers/microsd_driver.c` | ST-5    | ✅ PASS    |
| UC10   | Large file chunked write | `microsd_create_large_file_chunked()` | `drivers/microsd_driver.c` | Manual  | ✅ PASS    |
| UC10   | Large file chunked read  | `microsd_read_large_file_chunked()`   | `drivers/microsd_driver.c` | Manual  | ✅ PASS    |

---

## 5. UNIT TEST MAPPING

### UT-1: Message ID Generation

| Test ID | Function            | File                 | Expected                    | Result |
| ------- | ------------------- | -------------------- | --------------------------- | ------ |
| UT-1    | `get_next_msg_id()` | `mqtt/mqtt-sn-udp.c` | Sequential IDs (1, 2, 3...) | ✅ PASS |

---

### UT-2: CRC16 Calculation

| Test ID | Function         | File              | Expected                        | Result |
| ------- | ---------------- | ----------------- | ------------------------------- | ------ |
| UT-2    | `crc16()`        | `fs/data_frame.c` | Correct CRC16-CCITT-FALSE value | ✅ PASS |
| UT-2    | `verify_chunk()` | `fs/data_frame.c` | 1 for valid, 0 for invalid      | ✅ PASS |

---

### UT-3: File Deconstruction

| Test ID | Function            | File              | Expected                                    | Result |
| ------- | ------------------- | ----------------- | ------------------------------------------- | ------ |
| UT-3    | `deconstruct()`     | `fs/data_frame.c` | Correct chunk count for 1KB file (5 chunks) | ✅ PASS |
| UT-3    | Metadata generation | `fs/data_frame.c` | Valid session ID, filename, size, CRC       | ✅ PASS |

---

### UT-4: File Reconstruction

| Test ID | Function        | File              | Expected                            | Result |
| ------- | --------------- | ----------------- | ----------------------------------- | ------ |
| UT-4    | `reconstruct()` | `fs/data_frame.c` | Reconstructed file == original file | ✅ PASS |
| UT-4    | CRC validation  | `fs/data_frame.c` | File CRC matches metadata           | ✅ PASS |

---

### UT-5: SD Card Block I/O

| Test ID | Function                | File                       | Expected                  | Result |
| ------- | ----------------------- | -------------------------- | ------------------------- | ------ |
| UT-5    | `microsd_write_block()` | `drivers/microsd_driver.c` | Returns true              | ✅ PASS |
| UT-5    | `microsd_read_block()`  | `drivers/microsd_driver.c` | Read data == written data | ✅ PASS |

---

## 6. INTEGRATION TEST MAPPING

### IT-1: Wi-Fi + MQTT-SN Connection

| Test ID | Components          | Functions                                                   | Result |
| ------- | ------------------- | ----------------------------------------------------------- | ------ |
| IT-1    | Wi-Fi/UDP + MQTT-SN | `cyw43_arch_wifi_connect_timeout_ms()`, `mqtt_sn_connect()` | ✅ PASS |

---

### IT-2: MQTT-SN + MicroSD

| Test ID | Components        | Functions                                           | Result |
| ------- | ----------------- | --------------------------------------------------- | ------ |
| IT-2    | MQTT-SN + MicroSD | `microsd_read_file()`, `mqtt_sn_publish_topic_id()` | ✅ PASS |

---

### IT-3: MQTT-SN + LED Control

| Test ID | Components     | Functions                                | Result |
| ------- | -------------- | ---------------------------------------- | ------ |
| IT-3    | MQTT-SN + GPIO | `udp_recv_callback()`, LED control logic | ✅ PASS |

---

### IT-4: MQTT-SN + Button Input

| Test ID | Components     | Functions                                      | Result |
| ------- | -------------- | ---------------------------------------------- | ------ |
| IT-4    | MQTT-SN + GPIO | QoS button logic, `mqtt_sn_publish_topic_id()` | ✅ PASS |

---

### IT-5: MQTT-SN + Dashboard

| Test ID | Components | Functions                                                | Result |
| ------- | ---------- | -------------------------------------------------------- | ------ |
| IT-5    | Full stack | `mqtt_sn_publish_topic_id()`, Gateway, Broker, Dashboard | ✅ PASS |

---

## 7. SYSTEM TEST MAPPING

### ST-1: QoS 1 Retry Logic

| Test ID | Scenario    | Functions Tested                                     | Result |
| ------- | ----------- | ---------------------------------------------------- | ------ |
| ST-1    | Drop PUBACK | `check_qos_timeouts()`, `mqtt_sn_publish_topic_id()` | ✅ PASS |

---

### ST-2: QoS 2 Retry (PUBREC)

| Test ID | Scenario    | Functions Tested                    | Result |
| ------- | ----------- | ----------------------------------- | ------ |
| ST-2    | Drop PUBREC | `check_qos_timeouts()`, retry logic | ✅ PASS |

---

### ST-3: QoS 2 Retry (PUBCOMP)

| Test ID | Scenario     | Functions Tested                                | Result |
| ------- | ------------ | ----------------------------------------------- | ------ |
| ST-3    | Drop PUBCOMP | `check_qos_timeouts()`, `mqtt_sn_send_pubrel()` | ✅ PASS |

---

### ST-4: Session Timeout & Reconnect

| Test ID | Scenario           | Functions Tested                        | Result |
| ------- | ------------------ | --------------------------------------- | ------ |
| ST-4    | Disconnect gateway | `mqtt_sn_pingreq()`, reconnection logic | ✅ PASS |

---

### ST-5: File Transfer End-to-End

| Test ID | Scenario           | Functions Tested            | Result |
| ------- | ------------------ | --------------------------- | ------ |
| ST-5    | Full file transfer | All file transfer functions | ✅ PASS |

---

## 8. TEST COVERAGE SUMMARY

### By Module

| Module           | Total Functions | Tested Functions | Coverage | Status     |
| ---------------- | --------------- | ---------------- | -------- | ---------- |
| MQTT-SN Protocol | 12              | 12               | 100%     | ✅ PASS     |
| File Transfer    | 15              | 15               | 100%     | ✅ PASS     |
| MicroSD Driver   | 18              | 18               | 100%     | ✅ PASS     |
| Main Application | 8               | 8                | 100%     | ✅ PASS     |
| **TOTAL**        | **53**          | **53**           | **100%** | ✅ **PASS** |

---

### By Requirement Type

| Requirement Type        | Total  | Tested | Pass   | Fail  | Coverage |
| ----------------------- | ------ | ------ | ------ | ----- | -------- |
| Functional (FR1-FR9)    | 9      | 9      | 9      | 0     | 100%     |
| Demo App (FR-A1-A7)     | 7      | 7      | 7      | 0     | 100%     |
| Non-Functional (NFR1-5) | 5      | 5      | 5      | 0     | 100%     |
| **TOTAL**               | **21** | **21** | **21** | **0** | **100%** |

---

### By Test Type

| Test Type              | Total  | Pass   | Fail  | Pass Rate |
| ---------------------- | ------ | ------ | ----- | --------- |
| Unit Tests (UT)        | 5      | 5      | 0     | 100%      |
| Integration Tests (IT) | 5      | 5      | 0     | 100%      |
| System Tests (ST)      | 5      | 5      | 0     | 100%      |
| Manual Tests           | 8      | 8      | 0     | 100%      |
| **TOTAL**              | **23** | **23** | **0** | **100%**  |

---

## 9. CRITICAL PATH ANALYSIS

### High-Priority Requirements (Mission Critical)

| Priority | Req ID | Requirement                   | Status | Risk |
| -------- | ------ | ----------------------------- | ------ | ---- |
| P0       | FR1    | MQTT-SN Client Implementation | ✅ PASS | LOW  |
| P0       | FR2    | QoS 0/1/2 Support             | ✅ PASS | LOW  |
| P0       | FR3    | Session Management            | ✅ PASS | LOW  |
| P0       | NFR1   | Reliability (99% delivery)    | ✅ PASS | LOW  |
| P0       | NFR5   | Data Integrity (CRC)          | ✅ PASS | LOW  |

---

### Medium-Priority Requirements

| Priority | Req ID | Requirement              | Status | Risk |
| -------- | ------ | ------------------------ | ------ | ---- |
| P1       | FR4    | Publish/Subscribe        | ✅ PASS | LOW  |
| P1       | FR5    | File Transfer            | ✅ PASS | LOW  |
| P1       | NFR3   | Efficiency (247B chunks) | ✅ PASS | LOW  |

---

### Low-Priority Requirements (Nice-to-Have)

| Priority | Req ID | Requirement          | Status | Risk |
| -------- | ------ | -------------------- | ------ | ---- |
| P2       | FR-A1  | Button telemetry     | ✅ PASS | LOW  |
| P2       | FR-A2  | QoS selection button | ✅ PASS | LOW  |
| P2       | FR-A7  | Debug ACK dropping   | ✅ PASS | LOW  |
| P2       | NFR2   | Latency <150ms       | ✅ PASS | LOW  |
| P2       | NFR4   | Usability            | ✅ PASS | LOW  |

---

## 10. KNOWN ISSUES & LIMITATIONS

### Documented Limitations

| ID  | Component      | Limitation                            | Impact                           | Mitigation                                  |
| --- | -------------- | ------------------------------------- | -------------------------------- | ------------------------------------------- |
| L1  | MicroSD Driver | Synchronous blocking I/O              | CPU blocked during SD operations | Acceptable for demo; use DMA for production |
| L2  | File Transfer  | MAX_CHUNKS limit (256 chunks)         | Max file size ~60KB              | Documented in code; sufficient for demo     |
| L3  | Memory         | Dynamic allocation in `deconstruct()` | RAM usage scales with file size  | Use streaming read instead                  |
| L4  | Wi-Fi          | Requires 2.4GHz network               | Won't work on 5GHz-only networks | Documented in README                        |
| L5  | Polling        | `cyw43_arch_poll()` required          | Blocking >50ms degrades network  | Documented in driver interface              |

---

## 11. COMPLIANCE MATRIX

### Requirements vs Design Artifacts

| Requirement | Use Case      | Sequence Diagram    | Code Implementation | Test Coverage |
| ----------- | ------------- | ------------------- | ------------------- | ------------- |
| FR1         | UC1, UC11     | Fig 3, Fig 14       | ✅ Complete          | ✅ 100%        |
| FR2         | UC4, UC5, UC6 | Fig 6, Fig 7, Fig 8 | ✅ Complete          | ✅ 100%        |
| FR3         | UC1, UC11     | Fig 3, Fig 15       | ✅ Complete          | ✅ 100%        |
| FR4         | UC2, UC3, UC5 | Fig 4, Fig 5, Fig 7 | ✅ Complete          | ✅ 100%        |
| FR5         | UC8, UC9      | Fig 10, Fig 11      | ✅ Complete          | ✅ 100%        |
| FR-A1       | UC7           | Fig 9               | ✅ Complete          | ✅ Manual      |
| FR-A2       | N/A           | N/A                 | ✅ Complete          | ✅ Manual      |
| FR-A3       | UC3           | Fig 5               | ✅ Complete          | ✅ Manual      |
| FR-A4       | UC7           | Fig 9               | ✅ Complete          | ✅ Manual      |
| FR-A5       | UC8           | Fig 10              | ✅ Complete          | ✅ 100%        |
| FR-A6       | UC9           | Fig 11              | ✅ Complete          | ✅ 100%        |
| FR-A7       | N/A           | N/A                 | ✅ Complete          | ✅ 100%        |
| NFR1-5      | Various       | Various             | ✅ Complete          | ✅ 100%        |

---

## 12. RECOMMENDATIONS

### For Production Deployment

1. **Replace Dynamic Allocation**: Replace `deconstruct()` with `init_streaming_read()` for all file transfers
2. **Implement DMA**: Add DMA support to MicroSD driver to reduce CPU blocking
3. **Add Security**: Implement DTLS for UDP encryption (currently out of scope)
4. **Increase MAX_CHUNKS**: If larger files needed, expand bitmap or implement segmented transfers
5. **Add RTOS**: Consider using FreeRTOS for better task scheduling and non-blocking operations

---

## DOCUMENT APPROVAL

| Role                  | Name                              | Date       | Signature  |
| --------------------- | --------------------------------- | ---------- | ---------- |
| Technical Lead        | Neo Sun Wei                       | 3 Nov 2025 | ✅ Approved |
| Algorithm Developer   | Po Hao Ting                       | 3 Nov 2025 | ✅ Approved |
| QoS Implementation    | Haley Tan Hui Xin, Nico Caleb Lim | 3 Nov 2025 | ✅ Approved |
| Client Implementation | Travis Neo Kuang Yi               | 3 Nov 2025 | ✅ Approved |

---

**End of Requirements Traceability Matrix**
