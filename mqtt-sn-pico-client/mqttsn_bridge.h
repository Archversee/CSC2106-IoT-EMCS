#ifndef MQTTSN_BRIDGE_H
#define MQTTSN_BRIDGE_H

#include "lwip/udp.h"
#include "lwip/ip_addr.h"

// Initialize the bridge with UDP socket and gateway info
void mqttsn_bridge_init(struct udp_pcb *pcb, ip_addr_t *gateway_addr, u16_t gateway_port);

// File transfer status: "active", "completed", "failed"
void mqttsn_pub_file_status(const char *device_id, const char *filename, const char *status, const char *reason);

// File transfer progress: chunk X of Y with sequence and checksum
void mqttsn_pub_file_progress(const char *device_id, unsigned chunk, unsigned total, unsigned seq, unsigned checksum);

// File transfer validation: "success" or "failed" with expected/actual checksums
void mqttsn_pub_file_validation(const char *device_id, const char *result, const char *expected, const char *actual);

#endif // MQTTSN_BRIDGE_H