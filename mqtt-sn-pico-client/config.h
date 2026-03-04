#ifndef CONFIG_H
#define CONFIG_H

// UART/Serial Configuration
/**
 * Common Baud Rate for UART communication on Pico W
 * Used across multiple modules for consistency.
 *
 * 9600
 * 19200
 * 38400
 * 57600
 * 115200 (default, most common)
 * 230400
 * 460800
 * 921600
 */
#define UART_BAUD_RATE 115200 // Standard baud rate for Pico W serial communication

// Wi-Fi Credentials
#define WIFI_SSID "Denny"
#define WIFI_PASS "rieo8jebs"

// MQTT-SN Gateway
#define GATEWAY_IP0 172
#define GATEWAY_IP1 20
#define GATEWAY_IP2 10
#define GATEWAY_IP3 13
#define UDP_PORT 10000

// MQTT-SN Client ID (conditional based on build type)
// #ifdef PICO_TX_BUILD
// #define MQTT_SN_CLIENT_ID "pico_w_tx"
// #else
// #define MQTT_SN_CLIENT_ID "pico_w_rx"
// #endif

#define MQTT_SN_CLIENT_ID                                                                          \
    "pico_w_iot" // Change to "pico_w_XX" for each node to get a unique MQTT-SN client ID

// MQTT-SN Settings
#define KEEPALIVE_INTERVAL_SEC 60
#define PING_INTERVAL_MS 30000
#define PINGRESP_TIMEOUT_MS (PING_INTERVAL_MS * 3)

// QoS Message Tracking
#define MAX_PENDING_QOS_MSGS 5

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

// GPIO Pins
#define MESSAGEBUTTON_PIN 20
#define QOSBUTTON_PIN 21
#define DROP_ACK_BUTTON_PIN 19
#define TOPICBUTTON_PIN 22

// MQTT-SN Topic IDs
#define TOPIC_ID_PICO_CMD 1    // pico/cmd - control commands
#define TOPIC_ID_PICO_STATUS 2 // pico/status - status updates

#define TOPIC_DATA_1 "sensors/data"
#define TOPIC_DATA_2 "sensors/pico/data"
#define TOPIC_CMD_1 "sensors/cmd"
#define TOPIC_CMD_2 "sensors/pico/cmd"

#define PAYLOAD_SIZE 247

#endif