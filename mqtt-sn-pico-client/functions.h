// functions.h
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "lwip/udp.h"
#include "lwip/ip_addr.h"

// Public function declarations
void mqtt_sn_connect(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port);
void mqtt_sn_pingreq(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port);
void mqtt_sn_subscribe_topic_id(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port, u16_t topic_id);
void mqtt_sn_publish_topic_id(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port, u16_t topic_id, const char *payload);
void udp_recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

#endif // FUNCTIONS_H