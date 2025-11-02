#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi Credentials
#define WIFI_SSID "Neo"
#define WIFI_PASS "Password1"

// MQTT-SN Gateway
#define GATEWAY_IP0 192
#define GATEWAY_IP1 168
#define GATEWAY_IP2 28
#define GATEWAY_IP3 20
#define UDP_PORT 10000
#define MQTT_SN_CLIENT_ID "pico_w"

// MQTT-SN Settings
#define KEEPALIVE_INTERVAL_SEC 60
#define PING_INTERVAL_MS 30000
#define PINGRESP_TIMEOUT_MS (PING_INTERVAL_MS * 3)
#define MAX_PENDING_QOS_MSGS 5
#define QOS_RETRY_INTERVAL_US 2000000
#define QOS_MAX_RETRIES 3

// GPIO Pins
#define MESSAGEBUTTON_PIN 20
#define QOSBUTTON_PIN 21
#define DROP_ACK_BUTTON_PIN 22
#define FILE_TRANSFER_BUTTON_PIN 19

// Initialize File Transfer button GP19
#define FILE_TRANSFER_BUTTON_PIN 19

// MQTT-SN Topic IDs
#define TOPIC_ID_PICO_CMD 1     // pico/cmd - command topic
#define TOPIC_ID_PICO_STATUS 2  // pico/status - status topic
#define TOPIC_ID_FILE_META 3    // file/meta - file metadata
#define TOPIC_ID_FILE_DATA 4    // file/data - file chunks

#define PAYLOAD_SIZE 247

#endif