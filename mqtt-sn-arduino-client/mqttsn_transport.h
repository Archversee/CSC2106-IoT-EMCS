#ifndef MQTTSN_TRANSPORT_H
#define MQTTSN_TRANSPORT_H

#include "config.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Send raw MQTT-SN bytes. Replaces udp_sendto().
int mqttsn_transport_send(const uint8_t *buf, uint8_t len, bool wait_response = false);

// Non-blocking receive.
uint8_t mqttsn_transport_recv(uint8_t *buf, uint8_t buf_size, uint32_t timeout_ms);

// Hardware init
void mqttsn_transport_init(void);

// RSSI of last received frame
int16_t mqttsn_transport_last_rssi(void);

#ifdef __cplusplus
}
#endif
#endif