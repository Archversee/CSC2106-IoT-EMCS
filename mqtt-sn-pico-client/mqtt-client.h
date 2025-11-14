/*!
 * @file    mqtt-client.h
 * @brief   Common utilities for MQTT-SN clients (TX and RX)
 * @author  INF2004 Project Team
 * @date    2025
 *
 * This header provides common functionality shared between the TX and RX
 * MQTT-SN clients, including microSD card management and utility functions.
 */

#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! Common application constants */
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

/*! Global MQTT-SN ping tracking variables */
extern uint32_t g_last_pingresp;
extern bool g_ping_ack_received;

/*!
 * @brief Initialize common MQTT client network stack
 *
 * Sets up Wi-Fi, UDP, and connects to MQTT-SN gateway.
 *
 * @param mqtt_ctx_out Output parameter for MQTT-SN context (will be initialized)
 * @param pcb_out Output parameter for UDP PCB
 * @param gateway_addr_out Output parameter for gateway address
 * @param fs_initialized_out Output parameter for filesystem initialization status
 * @return 0 on success, -1 on error
 */
int mqtt_client_network_init(void **mqtt_ctx_out, void **pcb_out, void *gateway_addr_out,
                             bool *fs_initialized_out);

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
bool mqtt_client_initialize_microsd(uint8_t max_attempts, bool verbose);

/*!
 * @brief Check if microSD card is still accessible
 *
 * This is a lightweight check to detect if the card was removed.
 * Uses microsd_driver to check if the card is present.
 *
 * @return true if card is accessible, false if removed or error
 */
bool mqtt_client_check_microsd_present(void);

/*!
 * @brief Initialize common MQTT client components
 *
 * This function should be called once at startup to initialize
 * common global state.
 */
void mqtt_client_init(void);

#ifdef __cplusplus
}
#endif

#endif /* MQTT_CLIENT_H */
