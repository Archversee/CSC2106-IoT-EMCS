/*!
 * @file    mqtt-client.c
 * @brief   Common utilities for MQTT-SN clients (TX and RX)
 * @author  INF2004 Project Team
 * @date    2025
 *
 * This module provides common functionality shared between the TX and RX
 * MQTT-SN clients, including microSD card management and utility functions.
 */

#include "mqtt-client.h"

#include <stdio.h>

#include "config.h"
#include "drivers/microsd_driver.h"
#include "fs/chunk_transfer.h"
#include "mqtt/mqtt-sn-udp.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

/*! Global MQTT-SN ping tracking variables */
uint32_t g_last_pingresp = 0U;
bool g_ping_ack_received = true;

/*! Static ping request tracking (internal to this module) */
static uint32_t s_last_pingreq = 0U;

/*!
 * @brief Initialize microSD card with retry logic
 *
 * Uses microsd_driver for filesystem operations.
 * The card needs a few seconds to properly initialize, especially on first boot
 * or when the card is freshly inserted or reinserted.
 *
 * @param max_attempts Maximum number of initialization attempts (currently ignored)
 * @param verbose Print detailed status messages
 * @return true if initialization succeeded, false otherwise
 */
bool mqtt_client_initialize_microsd(uint8_t max_attempts, bool verbose) {
    static bool driver_initialized = false;

    (void)max_attempts;  // Unused parameter - kept for API compatibility

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
 * Uses microsd_driver to check if the card is present.
 *
 * @return true if card is accessible, false if removed or error
 */
bool mqtt_client_check_microsd_present(void) { return microsd_driver_is_present(); }

/*!
 * @brief Initialize common MQTT client components
 *
 * This function should be called once at startup to initialize
 * common global state.
 */
void mqtt_client_init(void) {
    g_last_pingresp = 0U;
    g_ping_ack_received = true;
    s_last_pingreq = 0U;
}

/*!
 * @brief Get last PINGREQ timestamp
 * @return Timestamp of last PINGREQ sent
 */
uint32_t mqtt_client_get_last_pingreq(void) { return s_last_pingreq; }

/*!
 * @brief Set last PINGREQ timestamp
 * @param timestamp Timestamp to set
 */
void mqtt_client_set_last_pingreq(uint32_t timestamp) { s_last_pingreq = timestamp; }

/*!
 * @brief Initialize common MQTT client network stack
 *
 * Sets up Wi-Fi, UDP, and connects to MQTT-SN gateway.
 *
 * @param mqtt_ctx_out Output parameter for MQTT-SN context (will be allocated and initialized)
 * @param pcb_out Output parameter for UDP PCB
 * @param gateway_addr_out Output parameter for gateway address
 * @param fs_initialized_out Output parameter for filesystem initialization status
 * @return 0 on success, -1 on error
 */
int mqtt_client_network_init(void** mqtt_ctx_out, void** pcb_out, void* gateway_addr_out,
                             bool* fs_initialized_out) {
    // Initialize stdio with custom baud rate from config
    stdio_init_all();
    stdio_uart_init_full(uart0, UART_BAUD_RATE, 0, 1);  // Set custom baud rate on UART0

    // Initialize common MQTT client components
    mqtt_client_init();

    // Initialize transfer session (static to persist)
    static transfer_session_t file_session = {0};
    bool fs_initialized = false;

    // Initial microSD card initialization
    fs_initialized = mqtt_client_initialize_microsd(MICROSD_INIT_MAX_ATTEMPTS, true);
    *fs_initialized_out = fs_initialized;

    // Allocate and initialize MQTT context (static to persist)
    static mqtt_sn_context_t mqtt_ctx = {0};
    mqtt_ctx.drop_acks = false;
    mqtt_ctx.file_session = &file_session;
    mqtt_ctx.transfer_in_progress = false;
    *mqtt_ctx_out = &mqtt_ctx;

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
    struct udp_pcb* pcb = udp_new();
    if (!pcb) {
        printf("UDP setup failed\n");
        return -1;
    }
    if (udp_bind(pcb, IP_ADDR_ANY, 0) != ERR_OK) {
        printf("UDP bind failed\n");
        return -1;
    }
    udp_recv(pcb, udp_recv_callback, &mqtt_ctx);
    *pcb_out = pcb;

    printf("UDP client ready...\n");

    // Setup MQTT-SN Gateway address
    ip_addr_t* gateway_addr = (ip_addr_t*)gateway_addr_out;
    IP4_ADDR(gateway_addr, GATEWAY_IP0, GATEWAY_IP1, GATEWAY_IP2, GATEWAY_IP3);

    sleep_ms(MQTT_CONNECT_DELAY_MS);

    // Connect to MQTT-SN Gateway
    mqtt_sn_connect(pcb, gateway_addr, UDP_PORT);
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

    // Print client ID
    printf("Client ID: %s\n", MQTT_SN_CLIENT_ID);

    return 0;
}
