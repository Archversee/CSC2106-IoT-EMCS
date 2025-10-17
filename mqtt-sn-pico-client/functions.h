// functions.h
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "config.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"

// Public function declarations
void mqtt_sn_connect(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port);
void mqtt_sn_pingreq(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port);
void mqtt_sn_subscribe_topic_id(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port, u16_t topic_id);
void mqtt_sn_publish_topic_id(struct udp_pcb *pcb,
                              const ip_addr_t *gw_addr,
                              u16_t gw_port,
                              uint16_t topic_id,
                              const uint8_t *payload,
                              size_t payload_len,
                              int qos,
                              uint16_t msg_id,
                              bool is_retransmit);
void mqtt_sn_send_pubrel(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port, uint16_t msg_id);
void mqtt_sn_send_puback(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port, uint16_t topic_id, uint16_t msg_id, uint8_t return_code);
void mqtt_sn_send_pubrec(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port, uint16_t msg_id);
void mqtt_sn_send_pubcomp(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port, uint16_t msg_id);
void udp_recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
void check_qos_timeouts(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port);
void remove_pending_qos_msg(uint16_t msg_id);

typedef struct {
    uint16_t msg_id;
    uint8_t qos;
    uint8_t step;  // 0 = PUBLISH sent, 1 = PUBREL sent (for QoS 2)
    absolute_time_t timestamp;
    uint8_t retry_count;
    uint16_t topic_id;
    uint8_t payload[255];   // Binary-safe payload buffer
    size_t payload_len;     // Actual payload length
    bool in_use;
} qos_msg_t;

typedef struct {
    bool drop_acks;
} mqtt_sn_context_t;

extern qos_msg_t pending_msgs[MAX_PENDING_QOS_MSGS];
uint16_t get_next_msg_id(void);

extern uint32_t last_pingresp;
extern bool ping_ack_received; 

#endif // FUNCTIONS_H