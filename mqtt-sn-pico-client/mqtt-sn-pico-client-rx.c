/*!
 * @file    mqtt-sn-pico-client-rx.c
 * @brief   MQTT-SN Receiver Client with File Transfer Support
 * @author  CS31 (MQTT-SN via UDP), INF2004 Project Team
 * @date    2025
 *
 * This is the RECEIVER version of the MQTT-SN client.
 * It subscribes to topics and receives file transfers.
 *
 * IMPORTANT: MicroSD Initialization
 * ----------------------------------
 * Due to the nature of SPI communication and the microSD card's initialization
 * requirements, the card will require a few seconds to properly initialize,
 * especially on first boot or when the card is freshly inserted. The application
 * implements automatic retry logic with delays to handle this initialization period.
 *
 * MQTT-SN Topics (Receiver subscribes to):
 * - Topic: pico/cmd    - Command topic (QoS 2)
 * - Topic: pico/status - Status messages (QoS 1)
 * - Topic: file/meta   - File metadata (QoS 2, if SD available)
 * - Topic: file/data   - File chunks (QoS 1, if SD available)
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

/*! Application constants - moved common ones to mqtt-client.h */
#define WIFI_RETRY_DELAY_MS (5000U)
#define MQTT_CONNECT_DELAY_MS (1000U)
#define MQTT_CONNACK_WAIT_MS (100U)
#define MQTT_POLL_SHORT_COUNT (10U)
#define MQTT_POLL_LONG_DELAY_MS (100U)
#define MQTT_POLL_LONG_COUNT (50U)
#define MQTT_RECONNECT_POLL_COUNT (20U)
#define CONNACK_POLL_COUNT (50U)
#define SUBSCRIBE_POLL_COUNT (20U)

/*! Global variables - common ones moved to mqtt-client.c */
static uint32_t s_last_pingreq = 0U;

/*!
 * @brief Forward declaration for subscribe_receiver_topics
 */
static void subscribe_receiver_topics(struct udp_pcb *pcb, ip_addr_t *gateway_addr,
                                      bool fs_initialized);

/*!
 * @brief Handle MQTT-SN ping and reconnection logic for RX client
 */
static void handle_mqtt_ping_and_reconnect(struct udp_pcb *pcb, ip_addr_t *gateway_addr,
                                           uint32_t now, bool fs_initialized) {
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

            // Re-subscribe to all receiver topics
            subscribe_receiver_topics(pcb, gateway_addr, fs_initialized);

            s_last_pingreq = now;
        }
    }
}

/*!
 * @brief Handle microSD card hot-plug detection for RX client
 */
static void handle_microsd_hotplug_rx(uint32_t *last_check, uint32_t now, bool *fs_initialized,
                                      bool *sd_was_initialized, mqtt_sn_context_t *mqtt_ctx) {
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

                // Re-add file transfer topics to subscription list
                mqtt_sn_add_topic_for_subscription(mqtt_ctx, "file/meta", QOS_LEVEL_2);
                mqtt_sn_add_topic_for_subscription(mqtt_ctx, "file/data", QOS_LEVEL_1);
                printf("File transfer topics added to subscription queue\n");
                printf("Topics will be subscribed on next registration cycle\n");
            }
        }
    }
}

/*!
 * @brief Handle Wi-Fi reconnection for RX client
 */
static void handle_wifi_reconnection_rx(void) {
    if (!cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA)) {
        printf("Wi-Fi disconnected! Reconnecting...\n");
        while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK,
                                                  10000)) {
            sleep_ms(2000);
        }
        printf("Reconnected. IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));
    }
}

/*!
 * @brief Subscribe to all receiver topics
 *
 * @param pcb UDP protocol control block
 * @param gateway_addr Gateway IP address
 * @param fs_initialized Whether filesystem is initialized
 */
static void subscribe_receiver_topics(struct udp_pcb *pcb, ip_addr_t *gateway_addr,
                                      bool fs_initialized) {
    // Subscribe to topic ID 1 (predefined topic "pico/cmd") default QoS 2 subscription
    printf("Subscribing to 'pico/cmd'...\n");
    mqtt_sn_subscribe_topic_id(pcb, gateway_addr, UDP_PORT, TOPIC_ID_PICO_CMD);
    for (uint8_t i = 0U; i < MQTT_POLL_SHORT_COUNT; i++) {
        cyw43_arch_poll();
        sleep_ms(MQTT_POLL_DELAY_MS);
    }

    // Subscribe to file transfer topics if SD card is ready
    if (fs_initialized) {
        printf("Subscribing to file transfer topics...\n");
        mqtt_sn_subscribe_topic_id(pcb, gateway_addr, UDP_PORT, TOPIC_ID_FILE_META);
        sleep_ms(MQTT_CONNACK_WAIT_MS);
        mqtt_sn_subscribe_topic_id(pcb, gateway_addr, UDP_PORT, TOPIC_ID_FILE_DATA);
        sleep_ms(MQTT_CONNACK_WAIT_MS);
        printf("✓ File transfer topics subscribed\n");
    }
}

int main() {
    printf("===========================================\n");
    printf("  MQTT-SN RECEIVER CLIENT (RX)\n");
    printf("===========================================\n");

    // Initialize network stack (Wi-Fi, UDP, MQTT-SN connection)
    mqtt_sn_context_t *mqtt_ctx;
    struct udp_pcb *pcb;
    ip_addr_t gateway_addr;
    bool fs_initialized;

    if (mqtt_client_network_init((void **)&mqtt_ctx, (void **)&pcb, &gateway_addr,
                                 &fs_initialized) != 0) {
        printf("Network initialization failed\n");
        return -1;
    }

    // Subscribe to topics dynamically by name (receiver subscribes to topics sender registered)
    printf("\n>>> Subscribing to topics dynamically...\n");
    mqtt_sn_add_topic_for_subscription(mqtt_ctx, "pico/cmd", QOS_LEVEL_2);
    mqtt_sn_add_topic_for_subscription(mqtt_ctx, "pico/status", QOS_LEVEL_1);
    if (fs_initialized) {
        mqtt_sn_add_topic_for_subscription(mqtt_ctx, "file/meta", QOS_LEVEL_2);
        mqtt_sn_add_topic_for_subscription(mqtt_ctx, "file/data", QOS_LEVEL_1);
    }
    printf("Topics queued for subscription:\n");
    printf("  - pico/cmd (QoS 2)\n");
    printf("  - pico/status (QoS 1)\n");
    if (fs_initialized) {
        printf("  - file/meta (QoS 2)\n");
        printf("  - file/data (QoS 1)\n");
    }

    // Subscribe to topics before main loop to prevent multiple subscriptions
    printf(">>> Sending topic subscription requests...\n");
    mqtt_sn_process_topic_registrations(mqtt_ctx, pcb, &gateway_addr, UDP_PORT);

    // Wait for subscription responses
    for (uint8_t i = 0U; i < 30; i++) {
        cyw43_arch_poll();
        sleep_ms(100);
    }
    printf("Topic subscription phase complete\n\n");

    // Track microSD status for hot-plug detection
    uint32_t last_sd_check = to_ms_since_boot(get_absolute_time());
    bool sd_was_initialized = fs_initialized;

    printf("\n===========================================\n");
    printf("  RECEIVER MODE: Listening for messages...\n");
    printf("===========================================\n\n");

    // Main loop
    while (true) {
        cyw43_arch_poll();

        uint32_t now = to_ms_since_boot(get_absolute_time());

        // Handle MQTT-SN ping and reconnection
        handle_mqtt_ping_and_reconnect(pcb, &gateway_addr, now, fs_initialized);

        // Check for QoS message timeouts and retransmissions
        check_qos_timeouts(pcb, &gateway_addr, UDP_PORT);

        // Handle microSD card hot-plug detection
        handle_microsd_hotplug_rx(&last_sd_check, now, &fs_initialized, &sd_was_initialized,
                                  mqtt_ctx);

        // Handle Wi-Fi reconnection
        handle_wifi_reconnection_rx();

        sleep_ms(100);
    }

    return 0;
}
