/*!
 * @file    mqtt-client.h
 * @brief   Common utilities for MQTT-SN clients (TX and RX)
 * @author  INF2004 Project Team
 * @date    2025
 *
 * This header provides common functionality shared between the TX and RX
 * MQTT-SN clients.
 */

#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <stdbool.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Common application constants */
#define WIFI_CONNECT_TIMEOUT_MS (10000U)
#define WIFI_RETRY_DELAY_MS (5000U)
#define MQTT_CONNECT_DELAY_MS (1000U)
#define MQTT_CONNACK_WAIT_MS (100U)
#define MQTT_POLL_DELAY_MS (10U)
#define MQTT_POLL_SHORT_COUNT (10U)
#define MQTT_POLL_LONG_DELAY_MS (100U)
#define MQTT_POLL_LONG_COUNT (50U)
#define MQTT_RECONNECT_POLL_COUNT (20U)
#define CONNACK_POLL_COUNT (50U)

/*! Global MQTT-SN ping tracking variables */
extern uint32_t g_last_pingresp;
extern bool g_ping_ack_received;
extern SemaphoreHandle_t g_mqtt_mutex;

/*!
 * @brief Initialize common MQTT client network stack
 *
 * Sets up Wi-Fi, UDP, and connects to MQTT-SN gateway.
 *
 * @param mqtt_ctx_out Output parameter for MQTT-SN context (will be initialized)
 * @param pcb_out Output parameter for UDP PCB
 * @param gateway_addr_out Output parameter for gateway address
 * @param fs_initialized_out Output parameter for filesystem initialization status (always false)
 * @return 0 on success, -1 on error
 */
int mqtt_client_network_init(void **mqtt_ctx_out, void **pcb_out, void *gateway_addr_out,
                             bool *fs_initialized_out);

/*!
 * @brief Initialize common MQTT client components
 *
 * This function should be called once at startup to initialize
 * common global state.
 */
void mqtt_client_init(void);

/*!
 * @brief Get last PINGREQ timestamp
 * @return Timestamp of last PINGREQ sent
 */
uint32_t mqtt_client_get_last_pingreq(void);

/*!
 * @brief Set last PINGREQ timestamp
 * @param timestamp Timestamp to set
 */
void mqtt_client_set_last_pingreq(uint32_t timestamp);

void mqtt_client_poll_network(void);

#ifdef __cplusplus
}
#endif

#endif /* MQTT_CLIENT_H */
