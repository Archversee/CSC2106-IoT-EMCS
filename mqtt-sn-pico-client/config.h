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
#define WIFI_SSID "hoshiyomi"
#define WIFI_PASS "hoshi171"

// MQTT-SN Gateway
#define GATEWAY_IP0 10
#define GATEWAY_IP1 200
#define GATEWAY_IP2 133
#define GATEWAY_IP3 90
#define UDP_PORT 10000

// MQTT-SN Client ID (conditional based on build type)
#ifdef PICO_TX_BUILD
#define MQTT_SN_CLIENT_ID "pico_w_tx"
#else
#define MQTT_SN_CLIENT_ID "pico_w_rx"
#endif

// MQTT-SN Settings
#define KEEPALIVE_INTERVAL_SEC 60
#define PING_INTERVAL_MS 30000
#define PINGRESP_TIMEOUT_MS (PING_INTERVAL_MS * 3)

// QoS Message Tracking
// CRITICAL: Must be >= WINDOW_SIZE_CHUNKS (138) for Go-Back-N file transfer
// Window size = 32KB / 237 bytes = 138 chunks
// Increased to 160 to provide headroom for concurrent operations
#define MAX_PENDING_QOS_MSGS 160

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
#define FILE_TRANSFER_BUTTON_PIN 22

// MQTT-SN Topic IDs
#define TOPIC_ID_PICO_CMD 1    // pico/cmd - control commands
#define TOPIC_ID_PICO_STATUS 2 // pico/status - status updates
#define TOPIC_ID_FILE_DATA 4   // file/data - file chunks (metadata + data)

#define PAYLOAD_SIZE 247

#endif