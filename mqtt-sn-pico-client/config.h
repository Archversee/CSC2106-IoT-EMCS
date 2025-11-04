#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi Credentials
#define WIFI_SSID "SS23"
#define WIFI_PASS "pxrh9861"

// MQTT-SN Gateway
#define GATEWAY_IP0 192
#define GATEWAY_IP1 168
#define GATEWAY_IP2 123
#define GATEWAY_IP3 100
#define UDP_PORT 10000
#define MQTT_SN_CLIENT_ID "pico_w_sw"
#define IS_RECEIVER true

// MQTT-SN Settings
#define KEEPALIVE_INTERVAL_SEC 60
#define PING_INTERVAL_MS 30000
#define PINGRESP_TIMEOUT_MS (PING_INTERVAL_MS * 3)
#define MAX_PENDING_QOS_MSGS 32
#define QOS_RETRY_INTERVAL_US 2000000
#define QOS_MAX_RETRIES 3

// Compile-time validation of configuration parameters
#if (PINGRESP_TIMEOUT_MS <= PING_INTERVAL_MS)
#error "PINGRESP_TIMEOUT_MS must be greater than PING_INTERVAL_MS"
#endif

#if (MAX_PENDING_QOS_MSGS < 1)
#error "MAX_PENDING_QOS_MSGS must be at least 1"
#endif

#if (QOS_MAX_RETRIES < 1)
#error "QOS_MAX_RETRIES must be at least 1"
#endif

// File Transfer Settings
// File transfers ALWAYS use QoS 1 (at-least-once delivery)
// This ensures reliable delivery with automatic retransmissions
// Duplicate chunks from retransmissions are handled automatically via bitmap

// GPIO Pins
#define MESSAGEBUTTON_PIN 20
#define QOSBUTTON_PIN 21
#define DROP_ACK_BUTTON_PIN 19
#define FILE_TRANSFER_BUTTON_PIN 22

// MQTT-SN Topic IDs
#define TOPIC_ID_PICO_CMD 1     // pico/cmd - command topic
#define TOPIC_ID_PICO_STATUS 2  // pico/status - status topic
#define TOPIC_ID_FILE_META 3    // file/meta - file metadata
#define TOPIC_ID_FILE_DATA 4    // file/data - file chunks

#define PAYLOAD_SIZE 247

#endif