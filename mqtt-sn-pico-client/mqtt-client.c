/*!
 * @file    mqtt-client.c
 * @brief   Common utilities for MQTT-SN clients (TX and RX)
 * @author  INF2004 Project Team
 * @date    2025
 *
 * This module provides common functionality shared between the TX and RX
 * MQTT-SN clients.
 */

#include "mqtt-client.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "config.h"
#include "mqtt/mqtt-sn-udp.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "task.h"

/*! Global MQTT-SN ping tracking variables */
uint32_t g_last_pingresp = 0U;
bool g_ping_ack_received = true;
SemaphoreHandle_t g_mqtt_mutex = NULL;

/*! Static ping request tracking (internal to this module) */
static uint32_t s_last_pingreq = 0U;

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

    if (g_mqtt_mutex == NULL) {
        g_mqtt_mutex = xSemaphoreCreateMutex();
    }
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
 * @param fs_initialized_out Output parameter for filesystem initialization status (always false
 * now)
 * @return 0 on success, -1 on error
 */
int mqtt_client_network_init(void **mqtt_ctx_out, void **pcb_out, void *gateway_addr_out,
                             bool *fs_initialized_out) {
    // Initialize stdio with custom baud rate from config
    stdio_init_all();
    stdio_uart_init_full(uart0, UART_BAUD_RATE, 0, 1); // Set custom baud rate on UART0

    // Initialize common MQTT client components
    mqtt_client_init();

    // No filesystem support in pure MQTT-SN mode
    *fs_initialized_out = false;

    // Allocate and initialize MQTT context (static to persist)
    static mqtt_sn_context_t mqtt_ctx = {0};
    mqtt_ctx.drop_acks = false;
    mqtt_ctx.file_session = NULL; // No file transfer support
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
        vTaskDelay(pdMS_TO_TICKS(WIFI_RETRY_DELAY_MS));
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

    // Create Mutex
    g_mqtt_mutex = xSemaphoreCreateMutex();
    if (g_mqtt_mutex == NULL) {
        printf("Failed to create MQTT mutex\n");
        return -1;
    }

    udp_recv(pcb, udp_recv_callback, &mqtt_ctx);
    *pcb_out = pcb;

    printf("UDP client ready...\n");

    // Setup MQTT-SN Gateway address
    ip_addr_t *gateway_addr = (ip_addr_t *)gateway_addr_out;
    IP4_ADDR(gateway_addr, GATEWAY_IP0, GATEWAY_IP1, GATEWAY_IP2, GATEWAY_IP3);

    vTaskDelay(pdMS_TO_TICKS(MQTT_CONNECT_DELAY_MS));

    // Connect to MQTT-SN Gateway
    mqtt_sn_connect(pcb, gateway_addr, UDP_PORT);
    // No polling needed with threadsafe background
    vTaskDelay(pdMS_TO_TICKS(MQTT_POLL_DELAY_MS * MQTT_POLL_SHORT_COUNT));

    printf("Waiting for CONNACK...\n");
    // No polling needed with threadsafe background
    vTaskDelay(pdMS_TO_TICKS(MQTT_CONNACK_WAIT_MS * MQTT_POLL_LONG_COUNT));

    vTaskDelay(pdMS_TO_TICKS(MQTT_CONNECT_DELAY_MS));

    // Print client ID
    printf("Client ID: %s\n", MQTT_SN_CLIENT_ID);

    return 0;
}

/*!
 * @brief FreeRTOS stack overflow hook
 * Called when a task stack overflow is detected
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    (void)xTask;
    printf("ERROR: Stack overflow in task: %s\n", pcTaskName);
    while (1) {
        // Halt on stack overflow
    }
}

/*!
 * @brief FreeRTOS malloc failed hook
 * Called when pvPortMalloc fails to allocate memory
 */
void vApplicationMallocFailedHook(void) {
    printf("ERROR: Memory allocation failed\n");
    while (1) {
        // Halt on malloc failure
    }
}
