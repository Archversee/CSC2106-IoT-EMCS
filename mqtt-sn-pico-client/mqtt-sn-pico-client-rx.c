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
 * Button Configuration:
 * - GP20: Send test message (uses current QoS level)
 * - GP21: Toggle QoS level (0 -> 1 -> 2 -> 0)
 * - GP22: Toggle ACK dropping (for testing retransmissions)
 *
 * MQTT-SN Topics (Receiver subscribes to):
 * - Topic ID 1 (pico/cmd):    Command topic (subscribed)
 * - Topic ID 3 (file/meta):   File metadata (subscribe)
 * - Topic ID 4 (file/data):   File chunks (subscribe)
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
#include "mqtt/mqtt-sn-udp.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "sd_card.h"

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

/*! Global variables */
uint32_t g_last_pingresp = 0U;
static uint32_t s_last_pingreq = 0U;
bool g_ping_ack_received = true;

/*!
 * @brief Initialize microSD card with retry logic
 *
 * Uses FatFS library for filesystem operations.
 * The card needs a few seconds to properly initialize, especially on first boot
 * or when the card is freshly inserted or reinserted.
 *
 * @param max_attempts Maximum number of initialization attempts
 * @param verbose Print detailed status messages
 * @return true if initialization succeeded, false otherwise
 */
static bool initialize_microsd(uint8_t max_attempts, bool verbose) {
    static bool driver_initialized = false;

    if (verbose) {
        printf("Initializing microSD card...\n");
        printf("(This may take a few seconds on first boot or after card insertion)\n");
    }

    // Initialize microSD driver (only once)
    if (!driver_initialized) {
        printf("Initializing microSD driver...\n");
        if (!microsd_driver_init()) {
            printf("✗ ERROR: MicroSD driver initialization failed\n");
            if (verbose) {
                printf("⚠ WARNING: MicroSD initialization failed\n");
                printf("File transfer features will be disabled.\n");
                printf("You can continue with MQTT-SN messaging features.\n");
            }
            return false;
        }
        driver_initialized = true;
        if (verbose) {
            printf("✓ MicroSD card initialized successfully\n");
            printf("MicroSD ready for file operations.\n");
        }
        return true;
    }

    // Already initialized
    if (verbose) {
        printf("✓ MicroSD already initialized\n");
    }
    return true;
}

/*!
 * @brief Check if microSD card is still accessible
 *
 * This is a lightweight check to detect if the card was removed.
 * We use microsd_driver to check if the card is present.
 *
 * @return true if card is accessible, false if removed or error
 */
static bool check_microsd_present(void) {
    return microsd_driver_is_present();
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
    stdio_init_all();

    printf("===========================================\n");
    printf("  MQTT-SN RECEIVER CLIENT (RX)\n");
    printf("===========================================\n");

    // Initialize Message button GP20
    gpio_init(MESSAGEBUTTON_PIN);
    gpio_set_dir(MESSAGEBUTTON_PIN, GPIO_IN);
    gpio_pull_up(MESSAGEBUTTON_PIN);

    // Initialize QoS toggle button GP21
    gpio_init(QOSBUTTON_PIN);
    gpio_set_dir(QOSBUTTON_PIN, GPIO_IN);
    gpio_pull_up(QOSBUTTON_PIN);

    // Initialize Drop ACK toggle button GP22 (for testing)
    gpio_init(DROP_ACK_BUTTON_PIN);
    gpio_set_dir(DROP_ACK_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(DROP_ACK_BUTTON_PIN);

    // Initialize transfer session
    static transfer_session_t file_session = {0};
    bool fs_initialized = false;

    // Initial microSD card initialization
    fs_initialized = initialize_microsd(MICROSD_INIT_MAX_ATTEMPTS, true);

    // MQTT context (no fs_info pointer needed with FatFS)
    mqtt_sn_context_t mqtt_ctx = {
        .drop_acks = false, .file_session = &file_session, .transfer_in_progress = false};

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
    IP4_ADDR(&gateway_addr, GATEWAY_IP0, GATEWAY_IP1, GATEWAY_IP2, GATEWAY_IP3);

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

    // print client ID
    printf("Client ID: %s\n", MQTT_SN_CLIENT_ID);

    // Subscribe to topics dynamically by name (receiver subscribes to topics sender registered)
    printf("\n>>> Subscribing to topics dynamically...\n");
    mqtt_sn_add_topic_for_subscription(&mqtt_ctx, "pico/cmd", QOS_LEVEL_2);
    mqtt_sn_add_topic_for_subscription(&mqtt_ctx, "pico/status", QOS_LEVEL_1);
    if (fs_initialized) {
        mqtt_sn_add_topic_for_subscription(&mqtt_ctx, "file/meta", QOS_LEVEL_2);
        mqtt_sn_add_topic_for_subscription(&mqtt_ctx, "file/data", QOS_LEVEL_1);
    }
    printf("Topics queued for subscription:\n");
    printf("  - pico/cmd (QoS 2)\n");
    printf("  - pico/status (QoS 1)\n");
    if (fs_initialized) {
        printf("  - file/meta (QoS 2)\n");
        printf("  - file/data (QoS 1)\n");
    }

    uint32_t last_ping = to_ms_since_boot(get_absolute_time());
    bool last_button_state = gpio_get(MESSAGEBUTTON_PIN);
    bool last_qos_button = gpio_get(QOSBUTTON_PIN);
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
    printf("  RECEIVER MODE: Listening for messages...\n");
    printf("===========================================\n\n");

    // Subscribe to topics before main loop to prevent multiple subscriptions
    printf(">>> Sending topic subscription requests...\n");
    mqtt_sn_process_topic_registrations(&mqtt_ctx, pcb, &gateway_addr, UDP_PORT);

    // Wait for subscription responses
    for (uint8_t i = 0U; i < 30; i++) {
        cyw43_arch_poll();
        sleep_ms(100);
    }
    printf("Topic subscription phase complete\n\n");

    // Main loop
    while (true) {
        cyw43_arch_poll();

        // Check message button press (active LOW) - can still send test messages
        bool current_button = gpio_get(MESSAGEBUTTON_PIN);
        if (last_button_state && !current_button) {
            // GP 20 Button pressed (falling edge)
            printf("Button pressed! Publishing test message...\n");
            uint16_t id = get_next_msg_id();
            // Publish to topic ID 2 (predefined topic "pico/status") with selected QoS
            mqtt_sn_publish_topic_id(pcb, &gateway_addr, UDP_PORT, TOPIC_ID_PICO_STATUS, payload,
                                     PAYLOAD_SIZE, (int)qos_level, id, false);
            sleep_ms(200); // Debounce
        }
        last_button_state = current_button;

        // Check QoS button press (active LOW)
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

        // Check drop ack button press (active LOW) - for testing
        bool cur_drop_btn = gpio_get(DROP_ACK_BUTTON_PIN);
        if (last_drop_button && !cur_drop_btn) {
            // GP 22 falling edge
            mqtt_ctx.drop_acks = !mqtt_ctx.drop_acks;
            printf("drop_acks = %u\n", mqtt_ctx.drop_acks ? 1U : 0U);
            sleep_ms(200U); // Debounce
        }
        last_drop_button = cur_drop_btn;

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

                // Re-subscribe to all receiver topics
                subscribe_receiver_topics(pcb, &gateway_addr, fs_initialized);

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
                if (!check_microsd_present()) {
                    printf("\n⚠ WARNING: MicroSD card removed!\n");
                    printf("File transfer features disabled.\n");
                    fs_initialized = false;
                    sd_was_initialized = false;
                    mqtt_ctx.transfer_in_progress = false;
                }
            } else if (!sd_was_initialized) {
                // Card was not initialized, check if one was inserted
                printf("Checking for microSD card...\n");
                if (initialize_microsd(1, false)) {
                    printf("✓ MicroSD card detected and initialized!\n");
                    printf("File transfer features now enabled.\n");
                    fs_initialized = true;
                    sd_was_initialized = true;

                    // Re-add file transfer topics to subscription list
                    mqtt_sn_add_topic_for_subscription(&mqtt_ctx, "file/meta", QOS_LEVEL_2);
                    mqtt_sn_add_topic_for_subscription(&mqtt_ctx, "file/data", QOS_LEVEL_1);
                    printf("File transfer topics added to subscription queue\n");
                    printf("Topics will be subscribed on next registration cycle\n");
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
