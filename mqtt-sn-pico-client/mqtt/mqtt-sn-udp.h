// mqtt-sn-udp.h
#ifndef MQTT_SN_UDP_H
#define MQTT_SN_UDP_H

/*
 * MQTT-SN UDP Protocol Implementation with File Transfer Support
 *
 * FILE TRANSFER QOS POLICY:
 * -------------------------
 * File transfers ALWAYS use QoS 1 (at-least-once delivery) for reliability.
 *
 * QoS 1 Guarantees:
 * - Every packet is acknowledged (PUBACK)
 * - Unacknowledged packets are automatically retransmitted
 * - May result in duplicate packet delivery
 *
 * Duplicate Handling:
 * - Each chunk has a unique sequence number and CRC16
 * - Bitmap tracks which chunks have been received
 * - Duplicate chunks are detected and safely ignored
 * - No duplicate data is written to microSD
 *
 * This approach ensures reliable file transfer while handling
 * network packet loss and duplicate deliveries gracefully.
 */

#include "../config.h"
#include "../drivers/microsd_driver.h"
#include "../fs/chunk_transfer.h"
#include "../fs/data_frame.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"

typedef struct {
    uint16_t msg_id;
    uint8_t qos;
    uint8_t step;  // 0 = PUBLISH sent, 1 = PUBREL sent (for QoS 2)
    absolute_time_t timestamp;
    uint8_t retry_count;
    uint16_t topic_id;
    uint8_t payload[255];  // Binary-safe payload buffer
    size_t payload_len;    // Actual payload length
    bool in_use;
} qos_msg_t;

typedef struct {
    bool drop_acks;
    transfer_session_t* file_session;
    filesystem_info_t* fs_info;
    bool transfer_in_progress;
} mqtt_sn_context_t;

extern qos_msg_t g_pending_msgs[MAX_PENDING_QOS_MSGS];
extern uint32_t g_last_pingresp;
extern bool g_ping_ack_received;

// Public function declarations
void mqtt_sn_connect(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port);
void mqtt_sn_pingreq(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port);
void mqtt_sn_subscribe_topic_id(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port, u16_t topic_id);
void mqtt_sn_publish_topic_id(struct udp_pcb* pcb,
                              const ip_addr_t* gw_addr,
                              u16_t gw_port,
                              uint16_t topic_id,
                              const uint8_t* payload,
                              size_t payload_len,
                              int qos,
                              uint16_t msg_id,
                              bool is_retransmit);
void mqtt_sn_send_pubrel(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port, uint16_t msg_id);
void mqtt_sn_send_puback(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port, uint16_t topic_id, uint16_t msg_id, uint8_t return_code);
void mqtt_sn_send_pubrec(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port, uint16_t msg_id);
void mqtt_sn_send_pubcomp(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port, uint16_t msg_id);
void udp_recv_callback(void* arg, struct udp_pcb* pcb, struct pbuf* p, const ip_addr_t* addr, u16_t port);
void check_qos_timeouts(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port);
void remove_pending_qos_msg(uint16_t msg_id);
uint16_t get_next_msg_id(void);

// File transfer functions
void send_file_via_mqtt(struct udp_pcb* pcb,
                        const ip_addr_t* gw_addr,
                        u16_t gw_port,
                        const char* filename);
void handle_file_metadata(mqtt_sn_context_t* ctx, const uint8_t* payload, size_t len);
void handle_file_payload(mqtt_sn_context_t* ctx, const uint8_t* payload, size_t len);

#endif  // MQTT_SN_UDP_H
