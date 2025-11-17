/*!
 * @file    mqtt-sn-pico-client-tx.c
 * @brief   MQTT-SN Sender Client with File Transfer Support
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 *
 * This is the SENDER version of the MQTT-SN client.
 * It publishes messages and initiates file transfers.
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
 * - GP22: Initiate file transfer (sends test.txt via MQTT-SN)
 * - GP19: Toggle ACK dropping (for testing retransmissions)
 *
 * MQTT-SN Topics (Sender publishes to):
 * - Topic ID 2 (pico/status): Status topic (publish)
 * - Topic ID 3 (file/meta):   File metadata (publish)
 * - Topic ID 4 (file/data):   File chunks (publish)
 */

#include <string.h>

#include "config.h"
#include "drivers/microsd_driver.h"
#include "ff.h"
#include "fs/chunk_transfer.h"
#include "fs/data_frame.h"
#include "hw_config.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "mqtt-client.h"
#include "mqtt/mqtt-sn-udp.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "sd_card.h"

/*! Application constants */
#define FILE_TRANSFER_NAME "test.jpg"

/*! Static variables for ping tracking */
static uint32_t s_last_pingreq = 0U;

/*!
 * @brief Handle message button press - publishes test message
 */
/*!
 * @brief Handle message button press - publishes test message
 */
static void handle_message_button(bool* last_state, struct udp_pcb* pcb, ip_addr_t* gateway_addr,
                                  uint8_t qos_level, uint8_t* payload, size_t payload_size,
                                  mqtt_sn_context_t* mqtt_ctx) {
    bool current = gpio_get(MESSAGEBUTTON_PIN);
    if (*last_state && !current) {
        printf("Button pressed! Publishing message...\n");

        mqtt_sn_publish_topic_id_auto(pcb, gateway_addr, UDP_PORT, TOPIC_ID_PICO_STATUS, payload,
                                      payload_size, (int)qos_level);
        sleep_ms(200);
    }
    *last_state = current;
}

/*!
 * @brief Handle QoS button press - cycles through QoS levels
 */
static void handle_qos_button(bool* last_state, uint8_t* qos_level) {
    bool current = gpio_get(QOSBUTTON_PIN);
    if (*last_state && !current) {
        (*qos_level)++;
        if (*qos_level > 2U) {
            *qos_level = 0U;
        }
        printf("QoS level changed to: %u\n", *qos_level);
        sleep_ms(200);
    }
    *last_state = current;
}

/*!
 * @brief Handle file transfer button press - sends file via MQTT
 */
static void handle_file_transfer_button(bool* last_state, struct udp_pcb* pcb,
                                        ip_addr_t* gateway_addr, bool* fs_initialized,
                                        bool* sd_was_initialized, mqtt_sn_context_t* mqtt_ctx) {
    bool current = gpio_get(FILE_TRANSFER_BUTTON_PIN);
    if (*last_state && !current) {
        printf("\n>>> File Transfer Button Pressed <<<\n");

        if (*fs_initialized) {
            send_file_via_mqtt_auto(pcb, gateway_addr, UDP_PORT, FILE_TRANSFER_NAME, mqtt_ctx);
        } else {
            printf("⚠ MicroSD card not initialized. Attempting to initialize...\n");

            if (mqtt_client_initialize_microsd(MICROSD_INIT_MAX_ATTEMPTS, true)) {
                printf("✓ MicroSD card successfully initialized!\n");
                *fs_initialized = true;
                *sd_was_initialized = true;
                printf("\nProceeding with file transfer...\n");
                send_file_via_mqtt_auto(pcb, gateway_addr, UDP_PORT, FILE_TRANSFER_NAME, mqtt_ctx);
            } else {
                printf("✗ Failed to initialize MicroSD card\n");
                printf("┌─────────────────────────────────────────────────────┐\n");
                printf("│  Please insert or reconnect the MicroSD card and    │\n");
                printf("│  press the file transfer button again.              │\n");
                printf("└─────────────────────────────────────────────────────┘\n");
            }
        }
        sleep_ms(200);
    }
    *last_state = current;
}

/*!
 * @brief Handle drop ACK button press - toggles ACK dropping for testing
 */
static void handle_drop_ack_button(bool* last_state, mqtt_sn_context_t* mqtt_ctx) {
    bool current = gpio_get(DROP_ACK_BUTTON_PIN);
    if (*last_state && !current) {
        mqtt_ctx->drop_acks = !mqtt_ctx->drop_acks;
        printf("drop_acks = %u\n", mqtt_ctx->drop_acks ? 1U : 0U);
        sleep_ms(200);
    }
    *last_state = current;
}

/*!
 * @brief Handle MQTT-SN ping and reconnection logic
 */
static void handle_mqtt_ping(struct udp_pcb* pcb, ip_addr_t* gateway_addr, uint32_t now) {
    if (g_ping_ack_received) {
        // Previous ping acknowledged, send new PINGREQ periodically
        if (now - s_last_pingreq >= PING_INTERVAL_MS) {
            mqtt_sn_pingreq(pcb, gateway_addr, UDP_PORT);
            g_ping_ack_received = false;
            s_last_pingreq = now;
        }
    } else {
        // Waiting for PINGRESP, check timeout
        if (now - s_last_pingreq > PINGRESP_TIMEOUT_MS) {
            printf("PINGRESP timeout, reconnecting MQTT-SN...\n");
            g_ping_ack_received = true;
            mqtt_sn_connect(pcb, gateway_addr, UDP_PORT);

            for (uint8_t i = 0U; i < CONNACK_POLL_COUNT; i++) {
                cyw43_arch_poll();
                sleep_ms(MQTT_POLL_DELAY_MS);
            }
            printf("Waiting for CONNACK after reconnect...\n");
            s_last_pingreq = now;
        }
    }
}

/*!
 * @brief Handle microSD card hot-plug detection
 */
static void handle_microsd_hotplug(uint32_t* last_check, uint32_t now, bool* fs_initialized,
                                   bool* sd_was_initialized, mqtt_sn_context_t* mqtt_ctx) {
    if (now - *last_check >= SD_CHECK_INTERVAL_MS) {
        *last_check = now;

        if (*fs_initialized) {
            // Card was initialized, check if still present
            if (!mqtt_client_check_microsd_present()) {
                printf("\n⚠ WARNING: MicroSD card removed!\n");
                printf("File transfer features disabled.\n");
                *fs_initialized = false;
                *sd_was_initialized = false;
                mqtt_ctx->transfer_in_progress = false;
            }
        } else if (!*sd_was_initialized) {
            // Card not initialized, check if one was inserted
            printf("Checking for microSD card...\n");
            if (mqtt_client_initialize_microsd(1, false)) {
                printf("✓ MicroSD card detected and initialized!\n");
                printf("File transfer features now enabled.\n");
                *fs_initialized = true;
                *sd_was_initialized = true;
            }
        }
    }
}

/*!
 * @brief Handle Wi-Fi reconnection
 */
static void handle_wifi_reconnection(void) {
    if (!cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA)) {
        printf("Wi-Fi disconnected! Reconnecting...\n");
        while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK,
                                                  10000)) {
            sleep_ms(2000);
        }
        printf("Reconnected. IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));
    }
}

int main() {
    printf("===========================================\n");
    printf("  MQTT-SN SENDER CLIENT (TX)\n");
    printf("===========================================\n");

    // Initialize network stack (Wi-Fi, UDP, MQTT-SN connection)
    mqtt_sn_context_t* mqtt_ctx;
    struct udp_pcb* pcb;
    ip_addr_t gateway_addr;
    bool fs_initialized;

    if (mqtt_client_network_init((void**)&mqtt_ctx, (void**)&pcb, &gateway_addr,
                                 &fs_initialized) != 0) {
        printf("Network initialization failed\n");
        return -1;
    }

    // Initialize Message button GP20
    gpio_init(MESSAGEBUTTON_PIN);
    gpio_set_dir(MESSAGEBUTTON_PIN, GPIO_IN);
    gpio_pull_up(MESSAGEBUTTON_PIN);

    // Initialize QoS toggle button GP21
    gpio_init(QOSBUTTON_PIN);
    gpio_set_dir(QOSBUTTON_PIN, GPIO_IN);
    gpio_pull_up(QOSBUTTON_PIN);

    // Initialize File Transfer button GP22
    gpio_init(FILE_TRANSFER_BUTTON_PIN);
    gpio_set_dir(FILE_TRANSFER_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(FILE_TRANSFER_BUTTON_PIN);

    // Initialize Drop ACK toggle button GP19 (for testing)
    gpio_init(DROP_ACK_BUTTON_PIN);
    gpio_set_dir(DROP_ACK_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(DROP_ACK_BUTTON_PIN);

    uint8_t qos_level = 0U;

    printf("SENDER MODE: Ready to publish messages and send files\n");

    // Register custom topics dynamically (sender registers topics it will publish to)
    printf("\n>>> Registering topics dynamically...\n");
    mqtt_sn_add_topic_for_registration(mqtt_ctx, "pico/cmd");
    mqtt_sn_add_topic_for_registration(mqtt_ctx, "pico/status");
    mqtt_sn_add_topic_for_registration(mqtt_ctx, "file/meta");
    mqtt_sn_add_topic_for_registration(mqtt_ctx, "file/data");
    mqtt_sn_add_topic_for_registration(mqtt_ctx, "file/control");  // For Go-Back-N flow control
    printf("Topics queued for registration:\n");
    printf("  - pico/cmd\n");
    printf("  - pico/status\n");
    printf("  - file/meta\n");
    printf("  - file/data\n");
    printf("  - file/control (for Go-Back-N)\n");

    uint32_t last_ping = to_ms_since_boot(get_absolute_time());
    // Register topics before main loop to prevent multiple registrations
    printf(">>> Sending topic registration requests...\n");
    mqtt_sn_process_topic_registrations(mqtt_ctx, pcb, &gateway_addr, UDP_PORT);

    // Wait for registration responses
    for (uint8_t i = 0U; i < 30; i++) {
        cyw43_arch_poll();
        sleep_ms(100);
    }
    printf("Topic registration phase complete\n");

    // Subscribe to file/control for Go-Back-N flow control responses
    printf("\n>>> Subscribing to file/control for Go-Back-N...\n");
    mqtt_sn_add_topic_for_subscription(mqtt_ctx, "file/control", QOS_LEVEL_1);
    mqtt_sn_process_topic_registrations(mqtt_ctx, pcb, &gateway_addr, UDP_PORT);

    // Wait for subscription confirmation
    for (uint8_t i = 0U; i < 20; i++) {
        cyw43_arch_poll();
        sleep_ms(100);
    }
    printf("Subscription phase complete\n\n");

    // Track button states
    bool last_button_state = gpio_get(MESSAGEBUTTON_PIN);
    bool last_qos_button = gpio_get(QOSBUTTON_PIN);
    bool last_file_button = gpio_get(FILE_TRANSFER_BUTTON_PIN);
    bool last_drop_button = gpio_get(DROP_ACK_BUTTON_PIN);

    // Track microSD status for hot-plug detection
    uint32_t last_sd_check = to_ms_since_boot(get_absolute_time());
    bool sd_was_initialized = fs_initialized;

    // Test binary payload MAX SIZE 247 for QoS 1
    uint8_t payload[PAYLOAD_SIZE];
    // Fill payload with example binary data: 0x00, 0x01, 0x02, ..., 0xFE
    for (size_t i = 0U; i < PAYLOAD_SIZE; i++) {
        payload[i] = (uint8_t)(i & 0xFFU);
    }

    printf("\n===========================================\n");
    printf("  SENDER MODE: Ready to send messages\n");
    printf("  - GP20: Send test message (pico/status)\n");
    printf("  - GP21: Toggle QoS (current: %u)\n", qos_level);
    printf("  - GP22: Send file transfer\n");
    printf("  - GP19: Toggle ACK dropping (testing)\n");
    printf("\n");
    printf("  USAGE: To publish to custom topics:\n");
    printf("  1. Register topic: mqtt_sn_add_topic_for_registration()\n");
    printf("  2. Get topic ID: mqtt_sn_get_topic_id()\n");
    printf("  3. Publish: mqtt_sn_publish_topic_id()\n");
    printf("===========================================\n\n");

    // Main loop
    while (true) {
        cyw43_arch_poll();

        // Handle all button inputs
        handle_message_button(&last_button_state, pcb, &gateway_addr, qos_level, payload,
                              PAYLOAD_SIZE, mqtt_ctx);
        handle_qos_button(&last_qos_button, &qos_level);
        handle_file_transfer_button(&last_file_button, pcb, &gateway_addr, &fs_initialized,
                                    &sd_was_initialized, mqtt_ctx);
        handle_drop_ack_button(&last_drop_button, mqtt_ctx);

        uint32_t now = to_ms_since_boot(get_absolute_time());

        // Handle MQTT-SN ping/reconnection
        handle_mqtt_ping(pcb, &gateway_addr, now);

        // Check for QoS message timeouts and retransmissions
        check_qos_timeouts(pcb, &gateway_addr, UDP_PORT);

        // Handle microSD card hot-plug detection
        handle_microsd_hotplug(&last_sd_check, now, &fs_initialized, &sd_was_initialized, mqtt_ctx);

        // Handle Wi-Fi reconnection
        handle_wifi_reconnection();

        sleep_ms(100);
    }

    return 0;
}
