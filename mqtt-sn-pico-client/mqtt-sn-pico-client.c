/*!
 * @file    mqtt-sn-pico-client.c
 * @brief   MQTT-SN Client with File Transfer Support
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 *
 * IMPORTANT: MicroSD Initialization
 * ----------------------------------
 * Due to the nature of SPI communication and the microSD card's initialization
 * requirements, the card will require a few seconds to properly initialize,
 * especially on first boot or when the card is freshly inserted. The application
 * implements automatic retry logic with delays to handle this initialization period.
 *
 * Button Configuration:
 * - GP20: Send test message (uses current QoS level)
 * - GP21: Toggle QoS level (0 -> 1 -> 2 -> 0)
 * - GP22: Toggle ACK dropping (for testing retransmissions)
 * - GP19: Initiate file transfer (sends test.txt via MQTT-SN)
 *
 * MQTT-SN Topics:
 * - Topic ID 1 (pico/cmd):    Command topic (subscribed)
 * - Topic ID 2 (pico/status): Status topic (publish)
 * - Topic ID 3 (file/meta):   File metadata (publish/subscribe)
 * - Topic ID 4 (file/data):   File chunks (publish/subscribe)
 */

#include <string.h>

#include "config.h"
#include "fs/chunk_transfer.h"
#include "fs/data_frame.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "mqtt/mqtt-sn-udp.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "mqttsn_bridge.h"

#ifndef DEVICE_ID
#define DEVICE_ID "pico-001" 
#endif


/*! Application constants */
#define WIFI_CONNECT_TIMEOUT_MS (10000U)
#define WIFI_RETRY_DELAY_MS (5000U)
#define MQTT_CONNECT_DELAY_MS (1000U)
#define MQTT_CONNACK_WAIT_MS (100U)
#define SD_CHECK_INTERVAL_MS (5000U)
#define MICROSD_INIT_MAX_ATTEMPTS (3U)
#define MQTT_POLL_DELAY_MS (10U)
#define MQTT_POLL_SHORT_COUNT (10U)
#define MQTT_POLL_LONG_DELAY_MS (100U)
#define MQTT_POLL_LONG_COUNT (50U)
#define MQTT_RECONNECT_POLL_COUNT (20U)
#define CONNACK_POLL_COUNT (50U)
#define SUBSCRIBE_POLL_COUNT (20U)

#define FILE_TRANSFER_NAME "test.txt"

/*! Global variables */
uint32_t g_last_pingresp = 0U;
static uint32_t s_last_pingreq = 0U;
bool g_ping_ack_received = true;

/*!
 * @brief Initialize microSD card with retry logic
 *
 * Due to SPI communication nature and microSD initialization requirements,
 * the card needs a few seconds to properly initialize, especially on first boot
 * or when the card is freshly inserted or reinserted.
 *
 * @param fs_info Pointer to filesystem_info_t structure to initialize
 * @param max_attempts Maximum number of initialization attempts
 * @param verbose Print detailed status messages
 * @return true if initialization succeeded, false otherwise
 */
static bool initialize_microsd(filesystem_info_t *fs_info, uint8_t max_attempts, bool verbose) {
    uint8_t sd_init_attempts = 0U;

    if (verbose) {
        printf("Initializing microSD card...\n");
        printf("(This may take a few seconds on first boot or after card insertion)\n");
    }

    while (sd_init_attempts < max_attempts) {
        sd_init_attempts++;
        if (verbose) {
            printf("Attempt %u/%u: ", sd_init_attempts, max_attempts);
        }

        // Allow time for SPI communication to stabilize
        sleep_ms(1000);

        if (microsd_init()) {
            // Additional delay for filesystem to be ready
            sleep_ms(500);

            if (microsd_init_filesystem(fs_info)) {
                if (verbose) {
                    printf("✓ MicroSD card initialized successfully\n");
                    printf("MicroSD ready for file operations.\n");
                }
                return true;
            } else {
                if (verbose) {
                    printf("Failed to initialize filesystem\n");
                }
            }
        } else {
            if (verbose) {
                printf("Failed to initialize microSD card\n");
            }
        }

        if (sd_init_attempts < max_attempts) {
            if (verbose) {
                printf("Retrying in 2 seconds...\n");
            }
            sleep_ms(2000);
        }
    }

    if (verbose) {
        printf("⚠ WARNING: MicroSD initialization failed after %d attempts\n", max_attempts);
        printf("File transfer features will be disabled.\n");
        printf("You can continue with MQTT-SN messaging features.\n");
    }

    return false;
}

/*!
 * @brief Check if microSD card is still accessible
 *
 * This is a lightweight check to detect if the card was removed.
 * We check if the filesystem info still indicates a valid filesystem.
 *
 * @param fs_info Pointer to filesystem_info_t structure
 * @return true if card is accessible, false if removed or error
 */
static bool check_microsd_present(filesystem_info_t *fs_info) {
    if (!fs_info)
        return false;

    // Simple check: verify filesystem is still marked as valid
    // A valid exFAT filesystem will have is_exfat=true and non-zero root_cluster
    return (fs_info->is_exfat && fs_info->root_cluster > 0);
}
int main() {
    stdio_init_all();

    // Initialize Message button GP20
    gpio_init(MESSAGEBUTTON_PIN);
    gpio_set_dir(MESSAGEBUTTON_PIN, GPIO_IN);
    gpio_pull_up(MESSAGEBUTTON_PIN);

    // Initialize QoS toggle button GP21
    gpio_init(QOSBUTTON_PIN);
    gpio_set_dir(QOSBUTTON_PIN, GPIO_IN);
    gpio_pull_up(QOSBUTTON_PIN);

    // Initialize Drop ACK toggle button GP22
    gpio_init(DROP_ACK_BUTTON_PIN);
    gpio_set_dir(DROP_ACK_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(DROP_ACK_BUTTON_PIN);

    gpio_init(FILE_TRANSFER_BUTTON_PIN);
    gpio_set_dir(FILE_TRANSFER_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(FILE_TRANSFER_BUTTON_PIN);

    // Initialize filesystem and transfer session
    static filesystem_info_t fs_info = {0};
    static transfer_session_t file_session = {0};
    bool fs_initialized = false;

    // Initial microSD card initialization
    fs_initialized = initialize_microsd(&fs_info, MICROSD_INIT_MAX_ATTEMPTS, true);

    // Always pass fs_info pointer - let handlers check if it's valid
    mqtt_sn_context_t mqtt_ctx = {.drop_acks = false,
                                  .file_session = &file_session,
                                  .fs_info = &fs_info,
                                  .transfer_in_progress = false};

    uint8_t qos_level = 0U;

    // Initialize Wi-Fi
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    cyw43_arch_enable_sta_mode();

    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK,
                                              WIFI_CONNECT_TIMEOUT_MS)) {
        printf("Wi-Fi connect failed. Retrying in %u seconds...\n", WIFI_RETRY_DELAY_MS / 1000U);
        sleep_ms(WIFI_RETRY_DELAY_MS);
    }

    printf("Wi-Fi connected. IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));

    // Setup UDP
    struct udp_pcb *pcb = udp_new();
    if (!pcb) {
        printf("UDP setup failed\n");
        return -1;
    }
    if (udp_bind(pcb, IP_ADDR_ANY, 0) != ERR_OK) {
        printf("UDP bind failed\n");
        return -1;
    }
    udp_recv(pcb, udp_recv_callback, &mqtt_ctx);

    printf("UDP client ready...\n");

    // setup MQTT-SN Gateway address
    ip_addr_t gateway_addr;
    IP4_ADDR(&gateway_addr, GATEWAY_IP0, GATEWAY_IP1, GATEWAY_IP2,
             GATEWAY_IP3); // RMB to change to your gateway IP

    sleep_ms(MQTT_CONNECT_DELAY_MS);

    // Connect to MQTT-SN Gateway
    mqtt_sn_connect(pcb, &gateway_addr, UDP_PORT);
    for (uint8_t i = 0U; i < MQTT_POLL_SHORT_COUNT; i++) {
        cyw43_arch_poll();
        sleep_ms(MQTT_POLL_DELAY_MS);
    }
    printf("Waiting for CONNACK...\n");
    for (uint8_t i = 0U; i < MQTT_POLL_LONG_COUNT; i++) {
        cyw43_arch_poll();
        sleep_ms(MQTT_CONNACK_WAIT_MS);
    }
    sleep_ms(MQTT_CONNECT_DELAY_MS);

    // Subscribe to topics only if IS_RECEIVER is enabled
    if (IS_RECEIVER) {
        // Subscribe to topic ID 1 (predefined topic "pico/cmd") default QoS 2 subscription - can
        // still receive < QoS <2 messages
        printf("Subscribing to 'pico/cmd'...\n");
        mqtt_sn_subscribe_topic_id(pcb, &gateway_addr, UDP_PORT, 1U);
        for (uint8_t i = 0U; i < MQTT_POLL_SHORT_COUNT; i++) {
            cyw43_arch_poll();
            sleep_ms(MQTT_POLL_DELAY_MS);
        }

        // Subscribe to file transfer topics
        if (fs_initialized) {
            printf("Subscribing to file transfer topics...\n");
            mqtt_sn_subscribe_topic_id(pcb, &gateway_addr, UDP_PORT, 3U); // file/meta
            sleep_ms(MQTT_CONNACK_WAIT_MS);
            mqtt_sn_subscribe_topic_id(pcb, &gateway_addr, UDP_PORT, 4U); // file/data
            sleep_ms(MQTT_CONNACK_WAIT_MS);
            printf("✓ File transfer topics subscribed\n");
        }
    } else {
        printf("IS_RECEIVER disabled - skipping topic subscriptions\n");
    }

    uint32_t last_ping = to_ms_since_boot(get_absolute_time());
    bool last_button_state = gpio_get(MESSAGEBUTTON_PIN);
    bool last_qos_button = gpio_get(QOSBUTTON_PIN);
    bool last_drop_button = gpio_get(DROP_ACK_BUTTON_PIN);
    bool last_file_button = gpio_get(FILE_TRANSFER_BUTTON_PIN);

    // Track microSD status for hot-plug detection
    uint32_t last_sd_check = to_ms_since_boot(get_absolute_time());
    bool sd_was_initialized = fs_initialized;

    // Test binary payload MAX SIZE 247 for QoS 1
    uint8_t payload[PAYLOAD_SIZE];
    // Fill payload with example binary data: 0x00, 0x01, 0x02, ..., 0xFE
    for (size_t i = 0U; i < PAYLOAD_SIZE; i++) {
        payload[i] = (uint8_t)(i & 0xFFU);
    }

    // Main loop
    while (true) {
        cyw43_arch_poll();

        // Check message button press (active LOW)
        bool current_button = gpio_get(MESSAGEBUTTON_PIN);
        if (last_button_state && !current_button) {
            // GP 20 Button pressed (falling edge)
            printf("Button pressed! Publishing message...\n");
            // Publish to topic ID 1 (predefined topic "pico/status") with selected QoS
            mqtt_sn_publish_topic_id_auto(pcb, &gateway_addr, UDP_PORT, 1U, payload, PAYLOAD_SIZE,
                                          (int)qos_level);
            sleep_ms(200); // Debounce
        }
        last_button_state = current_button;

        // Check Qos button press (active LOW)
        bool current_qos_button = gpio_get(QOSBUTTON_PIN);
        if (last_qos_button && !current_qos_button) {
            // GP 21 Button pressed (falling edge)
            qos_level++;
            if (qos_level > 2U) {
                qos_level = 0U; // Wrap around 0->1->2->0
            }
            printf("QoS level changed to: %u\n", qos_level);
            sleep_ms(200U); // Debounce
        }
        last_qos_button = current_qos_button;

        // Check drop ack button press (active LOW)
        bool cur_drop_btn = gpio_get(DROP_ACK_BUTTON_PIN);
        if (last_drop_button && !cur_drop_btn) {
            // GP 22 falling edge
            mqtt_ctx.drop_acks = !mqtt_ctx.drop_acks;
            printf("drop_acks = %u\n", mqtt_ctx.drop_acks ? 1U : 0U);
            sleep_ms(200U); // Debounce
        }
        last_drop_button = cur_drop_btn;

        // Check file transfer button press (active LOW)
        bool cur_file_btn = gpio_get(FILE_TRANSFER_BUTTON_PIN);
        if (last_file_button && !cur_file_btn) {
            // GP 19 falling edge
            if (fs_initialized) {
                printf("\n>>> File Transfer Button Pressed <<<\n");
                send_file_via_mqtt(pcb, &gateway_addr, UDP_PORT, "test.txt");
            } else {
                printf("\n>>> File Transfer Button Pressed <<<\n");
                printf("⚠ MicroSD card not initialized. Attempting to initialize...\n");

                // Attempt to initialize SD card
                if (initialize_microsd(&fs_info, MICROSD_INIT_MAX_ATTEMPTS, true)) {
                    printf("✓ MicroSD card successfully initialized!\n");
                    fs_initialized = true;
                    sd_was_initialized = true;
                    // fs_info pointer already in context, no need to reassign

                    // Subscribe to file transfer topics if IS_RECEIVER is enabled
                    if (IS_RECEIVER) {
                        printf("Subscribing to file transfer topics...\n");
                        mqtt_sn_subscribe_topic_id(pcb, &gateway_addr, UDP_PORT, 3); // file/meta
                        sleep_ms(100);
                        mqtt_sn_subscribe_topic_id(pcb, &gateway_addr, UDP_PORT, 4); // file/data
                        sleep_ms(100);
                        printf("✓ File transfer topics subscribed\n");
                    }

                    printf("\nProceeding with file transfer...\n");
                    send_file_via_mqtt(pcb, &gateway_addr, UDP_PORT, "test.txt");
                } else {
                    printf("✗ Failed to initialize MicroSD card\n");
                    printf("┌─────────────────────────────────────────────────────┐\n");
                    printf("│  Please insert or reconnect the MicroSD card and    │\n");
                    printf("│  press the file transfer button again.              │\n");
                    printf("└─────────────────────────────────────────────────────┘\n");
                }
            }
            sleep_ms(200); // Debounce
        }
        last_file_button = cur_file_btn;

        uint32_t now = to_ms_since_boot(get_absolute_time());
        // Send PINGREQ
        if (g_ping_ack_received) {
            // Previous ping was acknowledged, can send new PINGREQ periodically
            if (now - s_last_pingreq >= PING_INTERVAL_MS) {
                mqtt_sn_pingreq(pcb, &gateway_addr, UDP_PORT);
                g_ping_ack_received = false; // now waiting for PINGRESP
                s_last_pingreq = now;
            }
        } else {
            // Waiting for PINGRESP, check timeout
            if (now - s_last_pingreq > PINGRESP_TIMEOUT_MS) {
                printf("PINGRESP timeout, reconnecting MQTT-SN...\n");
                g_ping_ack_received = true; // reset flag before reconnect
                mqtt_sn_connect(pcb, &gateway_addr, UDP_PORT);

                // Poll and wait for CONNACK
                for (uint8_t i = 0U; i < CONNACK_POLL_COUNT; i++) {
                    cyw43_arch_poll();
                    sleep_ms(MQTT_POLL_DELAY_MS);
                }
                printf("Waiting for CONNACK after reconnect...\n");

                // Re-subscribe to topics only if IS_RECEIVER is enabled
                if (IS_RECEIVER) {
                    mqtt_sn_subscribe_topic_id(pcb, &gateway_addr, UDP_PORT, 1U);

                    for (uint8_t i = 0U; i < SUBSCRIBE_POLL_COUNT; i++) {
                        cyw43_arch_poll();
                        sleep_ms(MQTT_POLL_DELAY_MS);
                    }
                }

                s_last_pingreq = now; // reset ping timer after reconnect
            }
        }

        // Check for QoS message timeouts and retransmissions
        check_qos_timeouts(pcb, &gateway_addr, UDP_PORT);

        // Periodically check microSD card status (hot-plug detection)
        if (now - last_sd_check >= SD_CHECK_INTERVAL_MS) {
            last_sd_check = now;

            if (fs_initialized) {
                // Card was initialized, check if it's still present
                if (!check_microsd_present(&fs_info)) {
                    printf("\n⚠ WARNING: MicroSD card removed!\n");
                    printf("File transfer features disabled.\n");
                    fs_initialized = false;
                    sd_was_initialized = false;
                    // Don't set to NULL, keep the pointer but clear the structure
                    mqtt_ctx.transfer_in_progress = false;

                    // Clear filesystem info (keep pointer valid but mark as uninitialized)
                    memset(&fs_info, 0, sizeof(filesystem_info_t));
                }
            } else if (!sd_was_initialized) {
                // Card was not initialized, check if one was inserted
                printf("Checking for microSD card...\n");
                if (initialize_microsd(&fs_info, 1, false)) {
                    printf("✓ MicroSD card detected and initialized!\n");
                    printf("File transfer features now enabled.\n");
                    fs_initialized = true;
                    sd_was_initialized = true;
                    // fs_info pointer already in context, no need to reassign

                    // Subscribe to file transfer topics if we weren't before and IS_RECEIVER is
                    // enabled
                    if (IS_RECEIVER) {
                        printf("Subscribing to file transfer topics...\n");
                        mqtt_sn_subscribe_topic_id(pcb, &gateway_addr, UDP_PORT, 3); // file/meta
                        sleep_ms(100);
                        mqtt_sn_subscribe_topic_id(pcb, &gateway_addr, UDP_PORT, 4); // file/data
                        sleep_ms(100);
                        printf("✓ File transfer topics subscribed\n");
                    }
                }
            }
        }

        // Check Wi-Fi connection
        if (!cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA)) {
            printf("Wi-Fi disconnected! Reconnecting...\n");
            while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK,
                                                      10000)) {
                sleep_ms(2000);
            }
            printf("Reconnected. IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));
        }

        sleep_ms(100);
    }

    return 0;
}