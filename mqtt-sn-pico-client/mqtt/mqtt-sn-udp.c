// mqtt-sn-udp.c
#include "mqtt-sn-udp.h"

#include <stdio.h>
#include <string.h>

#include "../config.h"
#include "config.h"
#include "hardware/sync.h"
#include "lwip/pbuf.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

/*! Module-level state variables */
static uint16_t s_next_msg_id = 1U;
qos_msg_t g_pending_msgs[MAX_PENDING_QOS_MSGS];

/*!
 * @brief Get next unique message ID for MQTT-SN
 * @return uint16_t Next message ID (1-65535, wraps around)
 * @note Not thread-safe - assumes single-threaded access
 * @note Message ID 0 is reserved/invalid per MQTT-SN spec, so we skip it
 */
uint16_t get_next_msg_id(void) {
    if (s_next_msg_id == 0U || s_next_msg_id == 0xFFFFU) {
        s_next_msg_id = 1U; // Start from 1, skip 0
    }
    return s_next_msg_id++;
}

// Send MQTT-SN CONNECT packet
void mqtt_sn_connect(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port) {
    const char *client_id = MQTT_SN_CLIENT_ID;
    size_t id_len = strlen(client_id);
    u16_t packet_len = MQTTSN_CONNECT_FIXED_LEN +
                       id_len; // [len][type=0x04][flags][protocol_id][duration(2)][client_id]

    if (packet_len > MQTTSN_MAX_PACKET_LEN) {
        printf("Client ID too long\n");
        return;
    }

    // Allocate pbuf for CONNECT packet
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, packet_len, PBUF_RAM);
    if (!p)
        return;

    uint8_t *data = (uint8_t *)p->payload;
    data[MQTTSN_OFFSET_LENGTH] = (uint8_t)packet_len;
    data[MQTTSN_OFFSET_MSG_TYPE] = MQTTSN_MSG_TYPE_CONNECT; // CONNECT
    data[MQTTSN_OFFSET_FLAGS] = MQTTSN_FLAG_CLEAN_SESSION;  // Flags (clean session)
    data[MQTTSN_OFFSET_PROTOCOL_ID] = MQTTSN_PROTOCOL_ID;   // Protocol ID (MQTT-SN v1.2)
    data[MQTTSN_OFFSET_DURATION_HIGH] =
        (KEEPALIVE_INTERVAL_SEC >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK;             // Duration high
    data[MQTTSN_OFFSET_DURATION_LOW] = KEEPALIVE_INTERVAL_SEC & MQTTSN_BYTE_MASK; // Duration low
    memcpy(&data[MQTTSN_OFFSET_CLIENT_ID], client_id, id_len);

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if (err == ERR_OK) {
        printf("Sent CONNECT as '%s'\n", client_id);
    } else {
        printf("Failed to send CONNECT: %d\n", err);
    }
    pbuf_free(p);
}

// Send MQTT-SN PINGREQ to keep connection alive
void mqtt_sn_pingreq(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port) {
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, MQTTSN_PINGREQ_LEN, PBUF_RAM);
    if (!p)
        return;

    uint8_t *data = (uint8_t *)p->payload;
    data[MQTTSN_OFFSET_LENGTH] = MQTTSN_PINGREQ_LEN;        // Length
    data[MQTTSN_OFFSET_MSG_TYPE] = MQTTSN_MSG_TYPE_PINGREQ; // PINGREQ

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if (err == ERR_OK) {
        printf("Sent PINGREQ\n");
    } else {
        printf("Failed to send PINGREQ: %d\n", err);
    }
    pbuf_free(p);
}

// REGISTER a topic name to get a topic ID
void mqtt_sn_register_topic(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port,
                            const char *topic_name, uint16_t msg_id) {
    if (!topic_name) {
        printf("Invalid topic name\n");
        return;
    }

    size_t topic_len = strlen(topic_name);
    if (topic_len == 0 || topic_len > 250) {
        printf("Topic name length invalid: %zu\n", topic_len);
        return;
    }

    // REGISTER packet: [len][type=0x0A][topic_id(2)][msg_id(2)][topic_name]
    u16_t packet_len = 6 + topic_len; // 6 = len(1) + type(1) + topic_id(2) + msg_id(2)

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, packet_len, PBUF_RAM);
    if (!p) {
        printf("Failed to allocate pbuf for REGISTER\n");
        return;
    }

    uint8_t *data = (uint8_t *)p->payload;
    data[0] = (uint8_t)packet_len;
    data[1] = MQTTSN_MSG_TYPE_REGISTER;
    data[2] = 0x00U; // Topic ID high (0 = requesting new topic ID)
    data[3] = 0x00U; // Topic ID low
    data[4] = (msg_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK; // Msg ID high
    data[5] = msg_id & MQTTSN_BYTE_MASK;                    // Msg ID low
    memcpy(&data[6], topic_name, topic_len);

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if (err == ERR_OK) {
        printf("Sent REGISTER for topic '%s' (msg_id=%u)\n", topic_name, msg_id);
    } else {
        printf("Failed to send REGISTER: %d\n", err);
    }
    pbuf_free(p);
}

// SUBSCRIBE by Topic Name (normal topics)
void mqtt_sn_subscribe_topic_name(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port,
                                  const char *topic_name, uint16_t msg_id, uint8_t qos) {
    if (!topic_name) {
        printf("Invalid topic name\n");
        return;
    }

    size_t topic_len = strlen(topic_name);
    if (topic_len == 0 || topic_len > 250) {
        printf("Topic name length invalid: %zu\n", topic_len);
        return;
    }

    // SUBSCRIBE packet: [len][type=0x12][flags][msg_id(2)][topic_name]
    u16_t packet_len = 5 + topic_len; // 5 = len(1) + type(1) + flags(1) + msg_id(2)

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, packet_len, PBUF_RAM);
    if (!p) {
        printf("Failed to allocate pbuf for SUBSCRIBE\n");
        return;
    }

    // Flags: QoS (bits 5-6), TopicIdType=0 (normal topic name)
    uint8_t flags = 0x00; // TopicIdType = 0 (normal name)
    if (qos == QOS_LEVEL_1) {
        flags |= MQTTSN_FLAG_QOS1; // QoS 1
    } else if (qos == QOS_LEVEL_2) {
        flags |= MQTTSN_FLAG_QOS2; // QoS 2
    }

    uint8_t *data = (uint8_t *)p->payload;
    data[0] = (uint8_t)packet_len;
    data[1] = MQTTSN_MSG_TYPE_SUBSCRIBE;
    data[2] = flags;
    data[3] = (msg_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK; // Msg ID high
    data[4] = msg_id & MQTTSN_BYTE_MASK;                    // Msg ID low
    memcpy(&data[5], topic_name, topic_len);

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if (err == ERR_OK) {
        printf("Sent SUBSCRIBE for topic '%s' (QoS %u, msg_id=%u)\n", topic_name, qos, msg_id);
    } else {
        printf("Failed to send SUBSCRIBE: %d\n", err);
    }
    pbuf_free(p);
}

// SUBSCRIBE by Predefined Topic ID
void mqtt_sn_subscribe_topic_id(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port,
                                u16_t topic_id) {
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, MQTTSN_SUBSCRIBE_LEN, PBUF_RAM);
    if (!p)
        return;

    uint8_t *data = (uint8_t *)p->payload;
    data[MQTTSN_OFFSET_LENGTH] = MQTTSN_SUBSCRIBE_LEN;        // length
    data[MQTTSN_OFFSET_MSG_TYPE] = MQTTSN_MSG_TYPE_SUBSCRIBE; // SUBSCRIBE
    data[MQTTSN_OFFSET_FLAGS] = MQTTSN_SUBSCRIBE_FLAGS_QOS2;  // flags: QoS2, TopicIdType=predefined
    data[MQTTSN_OFFSET_PROTOCOL_ID] = 0x00U;                  // msg ID high
    data[MQTTSN_OFFSET_DURATION_HIGH] = 0x01U;                // msg ID low
    data[MQTTSN_OFFSET_DURATION_LOW] =
        (topic_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK;          // topic ID high
    data[MQTTSN_OFFSET_CLIENT_ID] = topic_id & MQTTSN_BYTE_MASK; // topic ID low

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if (err == ERR_OK) {
        printf("Sent SUBSCRIBE to Predefined Topic ID %d\n", topic_id);
    } else {
        printf("Failed to send SUBSCRIBE: %d\n", err);
    }
    pbuf_free(p);
}

// PUBLISH to Predefined Topic ID with qos, binary payload support
void mqtt_sn_publish_topic_id(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port,
                              u16_t topic_id, const uint8_t *payload, size_t payload_len, int qos,
                              uint16_t msg_id, bool is_retransmit) {
    if (!payload || qos < QOS_LEVEL_0 || qos > QOS_LEVEL_2) {
        printf("Invalid QoS or payload\n");
        return;
    }

    // Calculate total packet length 7(QoS1 header) + payload_len
    u16_t packet_len = MQTTSN_PUBLISH_HEADER_LEN + payload_len;

    if (packet_len > MQTTSN_MAX_PACKET_LEN) {
        printf("PUBLISH payload too long\n");
        return;
    }

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, packet_len, PBUF_RAM);
    if (!p) {
        printf("Failed to allocate pbuf\n");
        return;
    }

    // QOS Flags
    uint8_t flags = MQTTSN_FLAG_TOPIC_PREDEFINED; // TopicIdType = Predefined

    if (qos == QOS_LEVEL_1) {
        flags |= MQTTSN_FLAG_QOS1; // QoS 1 (bit 5 = 1)
    } else if (qos == QOS_LEVEL_2) {
        flags |= MQTTSN_FLAG_QOS2; // QoS 2 (bit 6 = 1)
    }

    uint8_t *data = (uint8_t *)p->payload;
    data[MQTTSN_OFFSET_LENGTH] = (uint8_t)packet_len;
    data[MQTTSN_OFFSET_MSG_TYPE] = MQTTSN_MSG_TYPE_PUBLISH; // PUBLISH
    data[MQTTSN_OFFSET_FLAGS] = flags;
    data[MQTTSN_OFFSET_TOPIC_ID_HIGH] =
        (topic_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK;             // Topic ID high
    data[MQTTSN_OFFSET_TOPIC_ID_LOW] = topic_id & MQTTSN_BYTE_MASK; // Topic ID low
    data[MQTTSN_OFFSET_MSG_ID_HIGH] =
        (qos > QOS_LEVEL_0) ? (msg_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK : 0x00U;
    data[MQTTSN_OFFSET_MSG_ID_LOW] = (qos > QOS_LEVEL_0) ? (msg_id & MQTTSN_BYTE_MASK) : 0x00U;
    memcpy(&data[MQTTSN_OFFSET_PAYLOAD], payload, payload_len);

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if (err == ERR_OK) {
        printf("Sent PUBLISH to Topic ID %d (QoS %d, Msg ID %d, Len %d)\n", topic_id, qos, msg_id,
               packet_len);

        // Store pending QoS message for retransmission if needed
        if (qos > QOS_LEVEL_0 && !is_retransmit) {
            // Disable interrupts during critical section to prevent race conditions
            // between checking in_use and setting it to true
            uint32_t save = save_and_disable_interrupts();
            bool slot_found = false;

            for (size_t i = 0U; i < MAX_PENDING_QOS_MSGS; i++) {
                if (!g_pending_msgs[i].in_use) {
                    g_pending_msgs[i].in_use = true;
                    g_pending_msgs[i].msg_id = msg_id;
                    g_pending_msgs[i].qos = qos;
                    g_pending_msgs[i].step = 0;
                    g_pending_msgs[i].timestamp = get_absolute_time();
                    g_pending_msgs[i].retry_count = 0U;
                    g_pending_msgs[i].topic_id = topic_id;
                    // Store binary payload safely, truncate if needed
                    size_t copy_len = (payload_len < sizeof(g_pending_msgs[i].payload))
                                          ? payload_len
                                          : sizeof(g_pending_msgs[i].payload);
                    if (payload_len > sizeof(g_pending_msgs[i].payload)) {
                        printf(
                            "WARNING: Payload truncated from %zu to %zu bytes for QoS tracking\n",
                            payload_len, sizeof(g_pending_msgs[i].payload));
                    }
                    memcpy(g_pending_msgs[i].payload, payload, copy_len);
                    g_pending_msgs[i].payload_len = copy_len;
                    slot_found = true;
                    break;
                }
            }

            restore_interrupts(save);

            if (!slot_found) {
                printf("ERROR: All QoS slots full (%d), message %d will not be tracked for "
                       "retransmission\n",
                       MAX_PENDING_QOS_MSGS, msg_id);
            }
        }
    } else {
        printf("Failed to send PUBLISH: %d\n", err);
    }
    pbuf_free(p);
}

// Send PUBACK for QoS 1
void mqtt_sn_send_puback(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port,
                         uint16_t topic_id, uint16_t msg_id, uint8_t return_code) {
    uint8_t msg[MQTTSN_PUBACK_LEN];
    msg[MQTTSN_OFFSET_LENGTH] = MQTTSN_PUBACK_LEN;        // Length
    msg[MQTTSN_OFFSET_MSG_TYPE] = MQTTSN_MSG_TYPE_PUBACK; // PUBACK
    msg[MQTTSN_OFFSET_FLAGS] = (topic_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_PROTOCOL_ID] = topic_id & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_DURATION_HIGH] = (msg_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_DURATION_LOW] = msg_id & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_CLIENT_ID] = return_code; // typically MQTTSN_RETURN_ACCEPTED

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    if (!p)
        return;
    memcpy(p->payload, msg, sizeof(msg));
    udp_sendto(pcb, p, gw_addr, gw_port);
    pbuf_free(p);

    printf("Sent PUBACK (topic_id: %u, msg_id: %u, rc: %u)\n", topic_id, msg_id, return_code);
}

// Send PUBREC for QoS 2
void mqtt_sn_send_pubrec(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port,
                         uint16_t msg_id) {
    uint8_t msg[MQTTSN_PUBREC_LEN];
    msg[MQTTSN_OFFSET_LENGTH] = MQTTSN_PUBREC_LEN;        // Length
    msg[MQTTSN_OFFSET_MSG_TYPE] = MQTTSN_MSG_TYPE_PUBREC; // PUBREC
    msg[MQTTSN_OFFSET_FLAGS] = (msg_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_PROTOCOL_ID] = msg_id & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_DURATION_HIGH] = MQTTSN_RETURN_ACCEPTED; // Return code: ACCEPTED

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    if (!p)
        return;
    memcpy(p->payload, msg, sizeof(msg));
    udp_sendto(pcb, p, gw_addr, gw_port);
    pbuf_free(p);
    printf("Sent PUBREC for Msg ID: %d\n", msg_id);
}

//  Send PUBCOMP for QoS 2
void mqtt_sn_send_pubcomp(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port,
                          uint16_t msg_id) {
    uint8_t msg[MQTTSN_PUBCOMP_LEN];
    msg[MQTTSN_OFFSET_LENGTH] = MQTTSN_PUBCOMP_LEN;        // Length
    msg[MQTTSN_OFFSET_MSG_TYPE] = MQTTSN_MSG_TYPE_PUBCOMP; // PUBCOMP
    msg[MQTTSN_OFFSET_FLAGS] = (msg_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_PROTOCOL_ID] = msg_id & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_DURATION_HIGH] = MQTTSN_RETURN_ACCEPTED; // Return code: ACCEPTED

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    if (!p)
        return;
    memcpy(p->payload, msg, sizeof(msg));
    udp_sendto(pcb, p, gw_addr, gw_port);
    pbuf_free(p);
    printf("Sent PUBCOMP for Msg ID: %d\n", msg_id);
}

//  Send PUBREL for QoS 2
void mqtt_sn_send_pubrel(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port,
                         uint16_t msg_id) {
    uint8_t msg[MQTTSN_PUBREL_LEN];
    msg[MQTTSN_OFFSET_LENGTH] = MQTTSN_PUBREL_LEN;        // Length
    msg[MQTTSN_OFFSET_MSG_TYPE] = MQTTSN_MSG_TYPE_PUBREL; // PUBREL
    msg[MQTTSN_OFFSET_FLAGS] = (msg_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_PROTOCOL_ID] = msg_id & MQTTSN_BYTE_MASK;

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    if (!p)
        return;

    memcpy(p->payload, msg, sizeof(msg));
    udp_sendto(pcb, p, gw_addr, gw_port);
    pbuf_free(p);
}

// Check and handle QoS message timeouts and retransmissions
void check_qos_timeouts(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port) {
    // Snapshot entries to retry to avoid use-after-free if ACK arrives during iteration
    typedef struct {
        bool valid;
        uint16_t msg_id;
        uint8_t qos;
        uint8_t step;
        uint16_t topic_id;
        uint8_t payload[MQTTSN_RETRY_PAYLOAD_SIZE];
        size_t payload_len;
        bool should_expire;
    } retry_entry_t;

    retry_entry_t retry_list[MAX_PENDING_QOS_MSGS] = {0};
    size_t retry_count = 0;

    absolute_time_t now = get_absolute_time();

    // Phase 1: Identify entries to retry or expire (minimize time spent iterating)
    for (size_t i = 0U; i < MAX_PENDING_QOS_MSGS; i++) {
        if (!g_pending_msgs[i].in_use)
            continue;

        // Check if timeout exceeded
        if (absolute_time_diff_us(g_pending_msgs[i].timestamp, now) > QOS_RETRY_INTERVAL_US) {
            // Check if max retries reached
            if (g_pending_msgs[i].retry_count >= QOS_MAX_RETRIES) {
                printf("QoS %d Msg ID %d failed after %d retries\n", g_pending_msgs[i].qos,
                       g_pending_msgs[i].msg_id, QOS_MAX_RETRIES);
                g_pending_msgs[i].in_use = false;
                continue;
            }

            // Copy to retry list for later retransmission
            retry_list[retry_count].valid = true;
            retry_list[retry_count].msg_id = g_pending_msgs[i].msg_id;
            retry_list[retry_count].qos = g_pending_msgs[i].qos;
            retry_list[retry_count].step = g_pending_msgs[i].step;
            retry_list[retry_count].topic_id = g_pending_msgs[i].topic_id;
            retry_list[retry_count].payload_len = g_pending_msgs[i].payload_len;
            memcpy(retry_list[retry_count].payload, g_pending_msgs[i].payload,
                   g_pending_msgs[i].payload_len);

            // Update retry count and timestamp immediately
            g_pending_msgs[i].retry_count++;
            g_pending_msgs[i].timestamp = get_absolute_time();
            retry_count++;
        }
    }

    // Phase 2: Perform retransmissions using snapshot data
    // This way, if ACK arrives and removes entry from g_pending_msgs during
    // retransmission, we're operating on our local copy
    for (size_t i = 0; i < retry_count; i++) {
        if (!retry_list[i].valid)
            continue;

        if (retry_list[i].qos == QOS_LEVEL_1) {
            printf("Retransmitting QoS1 PUBLISH for Msg ID %d\n", retry_list[i].msg_id);
            mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port, retry_list[i].topic_id,
                                     retry_list[i].payload, retry_list[i].payload_len,
                                     retry_list[i].qos, retry_list[i].msg_id, true);
        } else if (retry_list[i].qos == QOS_LEVEL_2) {
            if (retry_list[i].step == 0) {
                printf("Retransmitting QoS2 PUBLISH for Msg ID %d\n", retry_list[i].msg_id);
                mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port, retry_list[i].topic_id,
                                         retry_list[i].payload, retry_list[i].payload_len,
                                         retry_list[i].qos, retry_list[i].msg_id, true);
            } else if (retry_list[i].step == 1) {
                printf("Retransmitting PUBREL for Msg ID %d\n", retry_list[i].msg_id);
                mqtt_sn_send_pubrel(pcb, gw_addr, gw_port, retry_list[i].msg_id);
            }
        }
    }
}

/*!
 * @brief Remove pending QoS message by message ID
 * @param msg_id Message ID to remove from pending queue
 * @note Removes ALL matching entries to handle potential duplicates from corruption
 */
void remove_pending_qos_msg(uint16_t msg_id) {
    if (msg_id == 0U) {
        printf("WARNING: Attempted to remove invalid msg_id=0\n");
        return;
    }

    bool found = false;
    for (size_t i = 0U; i < MAX_PENDING_QOS_MSGS; i++) {
        if (g_pending_msgs[i].in_use && g_pending_msgs[i].msg_id == msg_id) {
            g_pending_msgs[i].in_use = false;
            found = true;
            // Continue searching to remove ALL duplicates (don't break)
        }
    }

    if (!found) {
        printf("WARNING: msg_id %u not found in pending queue (may have already been removed)\n",
               msg_id);
    }
}

// Callback for when UDP data is received
void udp_recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr,
                       u16_t port) {
    mqtt_sn_context_t *ctx = (mqtt_sn_context_t *)arg;

    if (p != NULL && p->len >= MQTTSN_HEADER_SIZE) {
        uint8_t *data = (uint8_t *)p->payload;
        uint8_t length = data[MQTTSN_OFFSET_LENGTH];
        uint8_t msg_type = data[MQTTSN_OFFSET_MSG_TYPE];

        // Simulate dropping ACKs
        if (ctx && ctx->drop_acks &&
            (msg_type == MQTTSN_MSG_TYPE_PUBACK || msg_type == MQTTSN_MSG_TYPE_PUBREC ||
             msg_type == MQTTSN_MSG_TYPE_PUBCOMP)) {
            printf("Simulated drop of ACK type 0x%02X\n", msg_type);
            pbuf_free(p);
            return;
        }

        // PINGRESP
        if (msg_type == MQTTSN_MSG_TYPE_PINGRESP) {
            g_last_pingresp = to_ms_since_boot(get_absolute_time());
            g_ping_ack_received = true;
            printf("Received PINGRESP\n");
        }

        // CONNACK
        else if (msg_type == MQTTSN_MSG_TYPE_CONNACK) {
            uint8_t return_code = data[MQTTSN_OFFSET_FLAGS];
            printf("CONNACK: return_code=%d (%s)\n", return_code,
                   return_code == MQTTSN_RETURN_ACCEPTED ? "Accepted" : "Rejected");
            g_ping_ack_received = true;
        }

        // SUBACK
        else if (msg_type == MQTTSN_MSG_TYPE_SUBACK) {
            uint8_t flags = data[MQTTSN_OFFSET_FLAGS];
            uint16_t topic_id = (data[MQTTSN_OFFSET_PROTOCOL_ID] << BITS_PER_BYTE) |
                                data[MQTTSN_OFFSET_DURATION_HIGH];
            uint16_t msg_id =
                (data[MQTTSN_OFFSET_DURATION_LOW] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_CLIENT_ID];
            uint8_t return_code = data[MQTTSN_OFFSET_PAYLOAD];
            printf("SUBACK: topic_id=%d, msg_id=%d, return_code=%d\n", topic_id, msg_id,
                   return_code);

            // Mark custom topic as subscribed if successful
            if (ctx && return_code == MQTTSN_RETURN_ACCEPTED) {
                for (size_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
                    if (ctx->custom_topics[i].in_use && !ctx->custom_topics[i].is_sender &&
                        !ctx->custom_topics[i].is_registered) {
                        // Match by last attempt timing (within last 2 seconds)
                        absolute_time_t now = get_absolute_time();
                        int64_t elapsed_ms =
                            absolute_time_diff_us(ctx->custom_topics[i].last_attempt, now) / 1000;
                        if (elapsed_ms < 2000) {
                            ctx->custom_topics[i].is_registered = true;
                            ctx->custom_topics[i].topic_id = topic_id;
                            printf("✓ Topic '%s' subscribed successfully (ID %u)\n",
                                   ctx->custom_topics[i].topic_name, topic_id);
                            break;
                        }
                    }
                }
            }
        }

        // REGACK (Topic Registration Acknowledgment)
        else if (msg_type == MQTTSN_MSG_TYPE_REGACK) {
            if (length >= 7) { // REGACK packet: [len][type][topic_id(2)][msg_id(2)][return_code]
                uint16_t topic_id = (data[2] << BITS_PER_BYTE) | data[3];
                uint16_t msg_id = (data[4] << BITS_PER_BYTE) | data[5];
                uint8_t return_code = data[6];

                if (return_code == MQTTSN_RETURN_ACCEPTED) {
                    printf("✓ REGACK: Topic registered successfully with ID %u (msg_id=%u)\n",
                           topic_id, msg_id);

                    // Mark custom topic as registered
                    if (ctx) {
                        for (size_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
                            if (ctx->custom_topics[i].in_use && ctx->custom_topics[i].is_sender &&
                                !ctx->custom_topics[i].is_registered) {
                                // Match by last attempt timing (within last 2 seconds)
                                absolute_time_t now = get_absolute_time();
                                int64_t elapsed_ms =
                                    absolute_time_diff_us(ctx->custom_topics[i].last_attempt, now) /
                                    1000;
                                if (elapsed_ms < 2000) {
                                    ctx->custom_topics[i].is_registered = true;
                                    ctx->custom_topics[i].topic_id = topic_id;
                                    printf(
                                        "✓ Topic '%s' registered with ID %u (ready to publish)\n",
                                        ctx->custom_topics[i].topic_name, topic_id);
                                    break;
                                }
                            }
                        }
                    }
                } else {
                    printf("✗ REGACK: Registration failed, return_code=%u (msg_id=%u)\n",
                           return_code, msg_id);
                }
            }
        }

        // PUBLISH received
        else if (msg_type == MQTTSN_MSG_TYPE_PUBLISH) {
            if (length >= MQTTSN_PUBLISH_HEADER_LEN) {
                uint8_t flags = data[MQTTSN_OFFSET_FLAGS];
                uint8_t qos = (flags >> MQTTSN_QOS_SHIFT) & MQTTSN_FLAG_QOS_MASK;
                uint16_t topic_id = (data[MQTTSN_OFFSET_TOPIC_ID_HIGH] << BITS_PER_BYTE) |
                                    data[MQTTSN_OFFSET_TOPIC_ID_LOW];
                uint16_t msg_id = (data[MQTTSN_OFFSET_MSG_ID_HIGH] << BITS_PER_BYTE) |
                                  data[MQTTSN_OFFSET_MSG_ID_LOW];
                int payload_len = length - MQTTSN_PUBLISH_HEADER_LEN;
                const uint8_t *payload = &data[MQTTSN_OFFSET_PAYLOAD];

                // Check for file transfer topics first
                if (topic_id == FILE_TRANSFER_TOPIC_METADATA) {
                    // file/meta topic (expects QoS 2)
                    printf("PUBLISH: File metadata received (QoS %d, Msg ID %d)\n", qos, msg_id);
                    handle_file_metadata(ctx, payload, payload_len, pcb, addr, port);
                    // Send appropriate QoS ACK
                    if (qos == QOS_LEVEL_1) {
                        mqtt_sn_send_puback(pcb, addr, port, topic_id, msg_id,
                                            MQTTSN_RETURN_ACCEPTED);
                    } else if (qos == QOS_LEVEL_2) {
                        mqtt_sn_send_pubrec(pcb, addr, port, msg_id);
                    }
                } else if (topic_id == FILE_TRANSFER_TOPIC_DATA) {
                    // file/data topic (expects QoS 1)
                    handle_file_payload(ctx, payload, payload_len);
                    // Send PUBACK for QoS 1
                    if (qos == QOS_LEVEL_1) {
                        mqtt_sn_send_puback(pcb, addr, port, topic_id, msg_id,
                                            MQTTSN_RETURN_ACCEPTED);
                    }
                } else {
                    // Regular message handling
                    // Print binary payload in hex
                    printf("PUBLISH received (QoS %d, Msg ID %d), Payload (%d bytes): ", qos,
                           msg_id, payload_len);
                    for (size_t i = 0U; i < (size_t)payload_len; i++) {
                        printf("%02X ", payload[i]);
                    }
                    printf("\n");

                    // handle text commands embedded in binary
                    if (payload_len == LED_ON_CMD_LEN &&
                        memcmp(payload, "led on", LED_ON_CMD_LEN) == 0) {
                        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1U);
                    } else if (payload_len == LED_OFF_CMD_LEN &&
                               memcmp(payload, "led off", LED_OFF_CMD_LEN) == 0) {
                        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0U);
                    }

                    // QoS ACKs
                    if (qos == QOS_LEVEL_1) {
                        mqtt_sn_send_puback(pcb, addr, port, topic_id, msg_id,
                                            MQTTSN_RETURN_ACCEPTED);
                    } else if (qos == QOS_LEVEL_2) {
                        mqtt_sn_send_pubrec(pcb, addr, port, msg_id);
                    }
                }
            }
        }

        // PUBACK QoS 1 RECEIVED
        else if (msg_type == MQTTSN_MSG_TYPE_PUBACK) {
            uint16_t msg_id = (data[MQTTSN_OFFSET_DURATION_HIGH] << BITS_PER_BYTE) |
                              data[MQTTSN_OFFSET_DURATION_LOW];
            printf("PUBACK received for Msg ID: %d \n", msg_id);
            remove_pending_qos_msg(msg_id);
        }

        // PUBCOMP (QoS 2 final ack) RECEIVED
        else if (msg_type == MQTTSN_MSG_TYPE_PUBCOMP) {
            uint16_t msg_id =
                (data[MQTTSN_OFFSET_FLAGS] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_PROTOCOL_ID];
            printf("PUBCOMP received for Msg ID: %d\n", msg_id);
            remove_pending_qos_msg(msg_id);
        }
        // PUBREC (QoS 2 Step 1) RECEIVED
        else if (msg_type == MQTTSN_MSG_TYPE_PUBREC) {
            uint16_t msg_id =
                (data[MQTTSN_OFFSET_FLAGS] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_PROTOCOL_ID];
            printf("PUBREC received for Msg ID: %d. Sending PUBREL...\n", msg_id);
            // Send PUBREL in response
            mqtt_sn_send_pubrel(pcb, addr, port, msg_id);
            // Update step of retransmission packet to indicate PUBREL was sent
            for (size_t i = 0U; i < MAX_PENDING_QOS_MSGS; i++) {
                if (g_pending_msgs[i].in_use && g_pending_msgs[i].msg_id == msg_id) {
                    g_pending_msgs[i].step = 1U;
                    g_pending_msgs[i].timestamp = get_absolute_time(); // reset timer
                    break;
                }
            }
        }
        // PUBREL (QoS 2 Step 2) RECEIVED
        else if (msg_type == MQTTSN_MSG_TYPE_PUBREL) {
            // send PUBCOMP
            uint16_t msg_id =
                (data[MQTTSN_OFFSET_FLAGS] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_PROTOCOL_ID];
            printf("PUBREL received for Msg ID: %d. Sending PUBCOMP...\n", msg_id);
            mqtt_sn_send_pubcomp(pcb, addr, port, msg_id);
            remove_pending_qos_msg(msg_id);
        }
        pbuf_free(p);
    } else {
        if (p)
            pbuf_free(p); // Always free if not handled
    }
}

// ============================================================================
// File Transfer Functions
// ============================================================================

/**
 * @brief Send a file via MQTT-SN by chunking and publishing
 *
 * This function uses QoS 2 for metadata and QoS 1 for data chunks.
 * - QoS 2 (exactly-once) ensures metadata is delivered before any data chunks
 * - QoS 1 (at-least-once) provides reliable data transfer with duplicate handling
 *
 * @param pcb UDP PCB
 * @param gw_addr Gateway address
 * @param gw_port Gateway port
 * @param filename Name of file to send from microSD
 *
 * @note QoS 1 for data may result in duplicate chunks being received, but the
 *       chunk_transfer module handles duplicates by checking the bitmap.
 * @note QoS 2 for metadata ensures the receiver initializes the session before
 *       accepting any data chunks, preventing data loss.
 *
 * @warning This function contains blocking SD card reads (~5ms per chunk).
 *          To prevent network stack starvation, cyw43_arch_poll() is called
 *          after each SD read and during inter-chunk delays to process
 *          incoming ACKs and prevent spurious retransmissions.
 */
void send_file_via_mqtt(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port,
                        const char *filename) {
    struct Metadata metadata = {0};
    uint16_t msg_id;

    printf("\n=== Starting STREAMING File Transfer ===\n");
    printf("File: %s\n", filename);
    printf("Using QoS %d for metadata, QoS %d for data chunks\n", FILE_TRANSFER_METADATA_QOS,
           FILE_TRANSFER_DATA_QOS);
    printf("Mode: STREAMING (memory efficient)\n");

    // Step 1: Initialize streaming read
    if (init_streaming_read((char *)filename, &metadata) != 0) {
        printf("ERROR: Failed to initialize streaming read\n");
        return;
    }

    printf("✓ File opened for streaming:\n");
    printf("  File size: %lu bytes\n", (unsigned long)metadata.total_size);
    printf("  Chunks: %lu\n", (unsigned long)metadata.chunk_count);
    printf("  Session ID: %s\n", metadata.session_id);
    printf("  File CRC: 0x%04X\n", metadata.file_crc);

    // Step 2: Serialize and send metadata (chunk 0)
    uint8_t meta_buffer[PAYLOAD_SIZE];
    if (serialize_metadata(&metadata, meta_buffer) != PAYLOAD_SIZE) {
        printf("ERROR: Failed to serialize metadata\n");
        cleanup_streaming_read();
        return;
    }

    // Publish metadata to topic ID 3 (file/meta) with QoS 2 for guaranteed delivery
    uint16_t metadata_msg_id = get_next_msg_id();
    mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port, FILE_TRANSFER_TOPIC_METADATA, meta_buffer,
                             PAYLOAD_SIZE, FILE_TRANSFER_METADATA_QOS, metadata_msg_id, false);
    printf("Sent metadata (QoS %d, msg_id=%u)\n", FILE_TRANSFER_METADATA_QOS, metadata_msg_id);

    // Wait for QoS 2 completion (PUBCOMP) before sending data chunks
    // This ensures receiver has initialized the transfer session
    printf("Waiting for metadata confirmation (PUBCOMP)...\n");
    absolute_time_t wait_start = get_absolute_time();

    while (absolute_time_diff_us(wait_start, get_absolute_time()) <
           (METADATA_CONFIRM_TIMEOUT_MS * 1000)) {
        cyw43_arch_poll(); // Process incoming packets (PUBREC, PUBCOMP)

        // Check if metadata message was acknowledged (removed from pending queue)
        bool metadata_confirmed = true;
        for (size_t i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
            if (g_pending_msgs[i].in_use && g_pending_msgs[i].msg_id == metadata_msg_id) {
                metadata_confirmed = false;
                break;
            }
        }

        if (metadata_confirmed) {
            printf("✓ Metadata confirmed by receiver\n");
            break;
        }

        sleep_ms(10); // Small delay between checks
    }

    // Final check - did we timeout?
    bool still_pending = false;
    for (size_t i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
        if (g_pending_msgs[i].in_use && g_pending_msgs[i].msg_id == metadata_msg_id) {
            still_pending = true;
            break;
        }
    }

    if (still_pending) {
        printf("ERROR: Metadata not confirmed after %u ms, aborting transfer\n",
               METADATA_CONFIRM_TIMEOUT_MS);
        cleanup_streaming_read();
        return;
    }

    // Step 3: Stream chunks one at a time (memory efficient!)
    printf("Streaming chunks...\n");
    uint32_t chunks_sent = 0;
    absolute_time_t start_time = get_absolute_time();

    for (uint32_t i = 0; i < metadata.chunk_count; i++) {
        struct Payload chunk = {0};

        // Read chunk from SD card (BLOCKING: ~5ms)
        if (read_chunk_streaming(i, &chunk) != 0) {
            printf("ERROR: Failed to read chunk %lu\n", (unsigned long)i);
            cleanup_streaming_read();
            return;
        }

        // Poll network immediately after blocking SD read to process any pending PUBACKs
        cyw43_arch_poll();

        // Verify chunk integrity before sending
        if (!verify_chunk(&chunk)) {
            printf("ERROR: Chunk %lu failed verification\n", (unsigned long)i);
            cleanup_streaming_read();
            return;
        }

        // Serialize chunk
        uint8_t payload_buffer[PAYLOAD_SIZE];
        if (serialize_payload(&chunk, payload_buffer) != PAYLOAD_SIZE) {
            printf("ERROR: Failed to serialize chunk %lu\n", (unsigned long)i);
            continue;
        }

        // Publish to topic ID 4 (file/data) with QoS 1
        msg_id = get_next_msg_id();
        mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port, FILE_TRANSFER_TOPIC_DATA, payload_buffer,
                                 PAYLOAD_SIZE, FILE_TRANSFER_DATA_QOS, msg_id, false);

        chunks_sent++;

        // Progress updates
        if ((i + 1) % PROGRESS_UPDATE_INTERVAL == 0 || i == metadata.chunk_count - 1) {
            float progress = ((float)(i + 1) / metadata.chunk_count) * 100.0f;
            printf("  [%lu/%lu] %.1f%% complete (msg_id=%u)\n", (unsigned long)(i + 1),
                   (unsigned long)metadata.chunk_count, progress, msg_id);
        }

        // Interleaved delay with network polling to process incoming PUBACKs
        // This prevents QoS 1 timeout/retransmission during file transfer
        absolute_time_t delay_start = get_absolute_time();
        while (absolute_time_diff_us(delay_start, get_absolute_time()) < INTER_CHUNK_DELAY_US) {
            cyw43_arch_poll();             // Process network events
            sleep_us(POLL_YIELD_DELAY_US); // Yield CPU briefly
        }
    }

    absolute_time_t end_time = get_absolute_time();
    int64_t elapsed_ms = absolute_time_diff_us(start_time, end_time) / 1000;

    // Cleanup streaming session
    cleanup_streaming_read();

    printf("✓ File transfer complete:\n");
    printf("  Total packets: %lu (1 metadata + %lu data)\n", (unsigned long)(chunks_sent + 1),
           (unsigned long)chunks_sent);
    printf("  Time: %lld ms\n", elapsed_ms);
    printf("  Throughput: %.2f KB/s\n",
           (float)metadata.total_size / (elapsed_ms / 1000.0f) / 1024.0f);
    printf("  Peak memory: ~0.5 KB (single chunk buffer)\n");
    printf("==============================\n\n");
}

/**
 * @brief Handle received file metadata packet
 *
 * Processes the initial metadata chunk that describes the file transfer.
 * Sent via QoS 2 for exactly-once delivery. This ensures the transfer
 * session is properly initialized before any data chunks are accepted.
 *
 * @param ctx MQTT-SN context containing session info
 * @param payload Serialized metadata payload
 * @param len Payload length
 *
 * @note Uses QoS 2 for guaranteed metadata delivery
 * @note Session must be initialized successfully before data chunks are accepted
 */
void handle_file_metadata(mqtt_sn_context_t *ctx, const uint8_t *payload, size_t len,
                          struct udp_pcb *pcb, const ip_addr_t *addr, u16_t port) {
    if (!ctx || !payload) {
        printf("ERROR: NULL parameter in handle_file_metadata\n");
        return;
    }

    if (len != PAYLOAD_SIZE) {
        printf("ERROR: Invalid metadata size (expected %d, got %zu)\n", PAYLOAD_SIZE, len);
        return;
    }

    struct Metadata metadata = {0};
    if (deserialize_metadata((uint8_t *)payload, &metadata) != 0) {
        printf("ERROR: Failed to deserialize metadata\n");
        return;
    }

    // Validate deserialized metadata fields
    if (metadata.chunk_count == 0 || metadata.chunk_count > MAX_CHUNK_COUNT) {
        printf("ERROR: Invalid chunk count %lu (valid range: 1-%u)\n",
               (unsigned long)metadata.chunk_count, MAX_CHUNK_COUNT);
        return;
    }

    if (metadata.total_size == 0 || metadata.total_size > MAX_FILE_SIZE_BYTES) {
        printf("ERROR: Invalid file size %lu (valid range: 1-%u bytes)\n",
               (unsigned long)metadata.total_size, MAX_FILE_SIZE_BYTES);
        return;
    }

    if (strlen(metadata.filename) == 0 || metadata.filename[0] == '\0') {
        printf("ERROR: Empty or invalid filename\n");
        return;
    }

    // Verify chunk count matches file size
    uint32_t expected_chunks = (metadata.total_size + PAYLOAD_DATA_SIZE - 1) / PAYLOAD_DATA_SIZE;
    if (metadata.chunk_count != expected_chunks) {
        printf("WARNING: Chunk count mismatch (expected %lu, got %lu)\n",
               (unsigned long)expected_chunks, (unsigned long)metadata.chunk_count);
    }

    printf("\n=== File Transfer Started ===\n");
    printf("Received metadata:\n");
    printf("  Session ID: %s\n", metadata.session_id);
    printf("  Filename:   %s\n", metadata.filename);
    printf("  Size:       %lu bytes\n", (unsigned long)metadata.total_size);
    printf("  Chunks:     %lu\n", (unsigned long)metadata.chunk_count);
    printf("  File CRC:   0x%04X\n", metadata.file_crc);

    // Check if filesystem is valid (may have been unplugged)
    bool fs_valid = (ctx->fs_info && ctx->fs_info->is_exfat && ctx->fs_info->root_cluster > 0);

    if (!fs_valid) {
        printf("⚠ WARNING: Filesystem not initialized - attempting to initialize SD card...\n");
        printf("(SD card may have been removed and re-inserted)\n");

        // Attempt to initialize/re-initialize SD card
        sleep_ms(500); // Brief delay for card stabilization

        if (ctx->fs_info && microsd_init()) {
            sleep_ms(500);
            // Re-initialize the existing fs_info structure
            if (microsd_init_filesystem(ctx->fs_info)) {
                printf("✓ SD card successfully re-initialized!\n");
            } else {
                printf("✗ ERROR: Failed to initialize SD card filesystem\n");
                printf("┌─────────────────────────────────────────────────────┐\n");
                printf("│  Cannot receive file: MicroSD card not initialized  │\n");
                printf("│  Please ensure SD card is properly inserted         │\n");
                printf("└─────────────────────────────────────────────────────┘\n");

                // Send error message back to sender
                const char *error_msg =
                    "ERROR: SD card not initialized. Cannot receive file. Please insert SD card.";
                uint16_t msg_id = get_next_msg_id();
                mqtt_sn_publish_topic_id(pcb, addr, port, TOPIC_ID_PICO_STATUS,
                                         (const uint8_t *)error_msg, strlen(error_msg), QOS_LEVEL_1,
                                         msg_id, false);
                printf("✓ Error notification sent to sender\n");
                return;
            }
        } else {
            printf("✗ ERROR: SD card initialization failed\n");
            printf("┌─────────────────────────────────────────────────────┐\n");
            printf("│  Cannot receive file: MicroSD card not detected     │\n");
            printf("│  Please insert SD card and try again                │\n");
            printf("└─────────────────────────────────────────────────────┘\n");

            // Send error message back to sender
            const char *error_msg =
                "ERROR: SD card not detected. Cannot receive file. Please insert SD card.";
            uint16_t msg_id = get_next_msg_id();
            mqtt_sn_publish_topic_id(pcb, addr, port, TOPIC_ID_PICO_STATUS,
                                     (const uint8_t *)error_msg, strlen(error_msg), QOS_LEVEL_1,
                                     msg_id, false);
            printf("✓ Error notification sent to sender\n");
            return;
        }
    }

    // Initialize transfer session
    if (!ctx->file_session) {
        printf("✗ ERROR: No session buffer allocated\n");

        // Send error message back to sender
        const char *error_msg = "ERROR: Session buffer not allocated. Cannot receive file.";
        uint16_t msg_id = get_next_msg_id();
        mqtt_sn_publish_topic_id(pcb, addr, port, TOPIC_ID_PICO_STATUS, (const uint8_t *)error_msg,
                                 strlen(error_msg), QOS_LEVEL_1, msg_id, false);
        printf("✓ Error notification sent to sender\n");
        return;
    }

    // Use new filename (true) to avoid overwriting existing files
    if (!chunk_transfer_init_session(ctx->fs_info, &metadata, ctx->file_session, true)) {
        printf("✗ ERROR: Failed to init transfer session (SD card may be full or corrupted)\n");

        // Send error message back to sender
        const char *error_msg =
            "ERROR: Failed to initialize transfer session. SD card may be full or corrupted.";
        uint16_t msg_id = get_next_msg_id();
        mqtt_sn_publish_topic_id(pcb, addr, port, TOPIC_ID_PICO_STATUS, (const uint8_t *)error_msg,
                                 strlen(error_msg), QOS_LEVEL_1, msg_id, false);
        printf("✓ Error notification sent to sender\n");
        return;
    }

    ctx->transfer_in_progress = true;
    printf("✓ Transfer session active - READY TO RECEIVE DATA CHUNKS\n");
    printf("=============================\n\n");
}

/**
 * @brief Handle received file payload chunk
 *
 * Processes incoming file chunks sent via QoS 1.
 * QoS 1 guarantees at-least-once delivery, which may result in duplicate chunks.
 * Duplicates are automatically detected and safely ignored using the chunk bitmap.
 *
 * @param ctx MQTT-SN context containing session info
 * @param payload Serialized payload chunk
 * @param len Payload length
 *
 * @note Data chunks are REFUSED if metadata has not been received first.
 * @note Duplicate chunks (from QoS 1 retransmissions) are handled gracefully:
 *       - CRC is verified to ensure data integrity
 *       - Bitmap is checked to detect duplicates
 *       - Duplicates are skipped without error
 *       - Only new chunks are written to microSD
 */
void handle_file_payload(mqtt_sn_context_t *ctx, const uint8_t *payload, size_t len) {
    if (!ctx || !payload) {
        printf("ERROR: NULL parameter in handle_file_payload\n");
        return;
    }

    if (!ctx->transfer_in_progress) {
        printf("WARNING: Received payload but no active session\n");
        return;
    }

    if (len != PAYLOAD_SIZE) {
        printf("ERROR: Invalid payload size (expected %d, got %zu)\n", PAYLOAD_SIZE, len);
        return;
    }

    struct Payload chunk = {0};
    if (deserialize_payload((uint8_t *)payload, &chunk) != 0) {
        printf("ERROR: Failed to deserialize payload\n");
        return;
    }

    // Verify chunk integrity with CRC16
    if (!verify_chunk(&chunk)) {
        printf("ERROR: Chunk %lu failed CRC check\n", (unsigned long)chunk.sequence);
        return;
    }

    // Write chunk to microSD
    // Note: chunk_transfer_write_payload automatically handles duplicates by checking
    // the bitmap and skipping chunks that have already been received.
    // This is essential for QoS 1 which may deliver duplicates.
    if (!chunk_transfer_write_payload(ctx->fs_info, ctx->file_session, &chunk)) {
        printf("ERROR: Failed to write chunk %lu\n", (unsigned long)chunk.sequence);
        return;
    }

    uint32_t received, total;
    chunk_transfer_get_progress(ctx->file_session, &received, &total);

    if (received % PROGRESS_UPDATE_INTERVAL == 0 || received == total) {
        printf("Progress: %lu/%lu chunks received\n", (unsigned long)received,
               (unsigned long)total);
    }

    // Check if transfer complete
    if (chunk_transfer_is_complete(ctx->file_session)) {
        printf("\n=== File Transfer Complete ===\n");
        printf("All chunks received! Finalizing...\n");

        if (chunk_transfer_finalize(ctx->fs_info, ctx->file_session)) {
            printf("✓ File saved: %s\n", ctx->file_session->filename);
            printf("  Size: %lu bytes\n", (unsigned long)ctx->file_session->metadata.total_size);

            // Dump critical sectors for debugging/verification
            printf("\n");
            printf("═══════════════════════════════════════════════════════════════════\n");
            printf("Dumping SD card sectors for verification...\n");
            printf("═══════════════════════════════════════════════════════════════════\n");
            microsd_hex_dump(0, 1); // MBR
            printf("\n");

            // Dump partition boot sector (starts at LBA 16384 per MBR)
            printf("Dumping exFAT Boot Sector (partition start at sector %lu)...\n",
                   (unsigned long)ctx->fs_info->partition_offset);
            microsd_hex_dump(ctx->fs_info->partition_offset, 1);
            printf("\n");

            // Dump root directory sector
            // Calculate: partition_offset + cluster_heap_offset + (root_cluster - 2) *
            // sectors_per_cluster
            uint32_t root_sector =
                ctx->fs_info->partition_offset + ctx->fs_info->cluster_heap_offset +
                ((ctx->fs_info->root_cluster - 2) * ctx->fs_info->sectors_per_cluster);
            printf("Dumping Root Directory (cluster %lu, sector %lu)...\n",
                   (unsigned long)ctx->fs_info->root_cluster, (unsigned long)root_sector);
            microsd_hex_dump(root_sector, 2); // Dump 2 sectors of root directory

            // List directory contents to verify file is visible
            printf("\n");
            microsd_list_directory(ctx->fs_info);
        } else {
            printf("ERROR: Failed to finalize transfer\n");
        }

        ctx->transfer_in_progress = false;
        printf("==============================\n\n");
    }
}

// ============================================================================
// Custom Topic Management Functions
// ============================================================================

/**
 * @brief Add a topic for registration (sender pico)
 * @param ctx MQTT-SN context
 * @param topic_name Topic name to register
 * @return bool true on success, false if no slots available
 */
bool mqtt_sn_add_topic_for_registration(mqtt_sn_context_t *ctx, const char *topic_name) {
    if (!ctx || !topic_name) {
        printf("ERROR: Invalid parameters for topic registration\n");
        return false;
    }

    size_t topic_len = strlen(topic_name);
    if (topic_len == 0 || topic_len >= 64) {
        printf("ERROR: Invalid topic name length: %zu\n", topic_len);
        return false;
    }

    // Check if topic already exists
    for (size_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
        if (ctx->custom_topics[i].in_use &&
            strcmp(ctx->custom_topics[i].topic_name, topic_name) == 0) {
            printf("Topic '%s' already added for registration\n", topic_name);
            return true;
        }
    }

    // Find free slot
    for (size_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
        if (!ctx->custom_topics[i].in_use) {
            strncpy(ctx->custom_topics[i].topic_name, topic_name, 63);
            ctx->custom_topics[i].topic_name[63] = '\0';
            ctx->custom_topics[i].topic_id = 0;
            ctx->custom_topics[i].qos = 0;
            ctx->custom_topics[i].is_registered = false;
            ctx->custom_topics[i].is_sender = true;
            ctx->custom_topics[i].last_attempt = nil_time;
            ctx->custom_topics[i].in_use = true;
            printf("✓ Added topic '%s' for registration (sender)\n", topic_name);
            return true;
        }
    }

    printf("ERROR: No free slots for topic registration (max %u)\n", MAX_CUSTOM_TOPICS);
    return false;
}

/**
 * @brief Add a topic for subscription (receiver pico)
 * @param ctx MQTT-SN context
 * @param topic_name Topic name to subscribe to
 * @param qos QoS level for subscription
 * @return bool true on success, false if no slots available
 */
bool mqtt_sn_add_topic_for_subscription(mqtt_sn_context_t *ctx, const char *topic_name,
                                        uint8_t qos) {
    if (!ctx || !topic_name) {
        printf("ERROR: Invalid parameters for topic subscription\n");
        return false;
    }

    if (qos > QOS_LEVEL_2) {
        printf("ERROR: Invalid QoS level: %u\n", qos);
        return false;
    }

    size_t topic_len = strlen(topic_name);
    if (topic_len == 0 || topic_len >= 64) {
        printf("ERROR: Invalid topic name length: %zu\n", topic_len);
        return false;
    }

    // Check if topic already exists
    for (size_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
        if (ctx->custom_topics[i].in_use &&
            strcmp(ctx->custom_topics[i].topic_name, topic_name) == 0) {
            printf("Topic '%s' already added for subscription\n", topic_name);
            return true;
        }
    }

    // Find free slot
    for (size_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
        if (!ctx->custom_topics[i].in_use) {
            strncpy(ctx->custom_topics[i].topic_name, topic_name, 63);
            ctx->custom_topics[i].topic_name[63] = '\0';
            ctx->custom_topics[i].topic_id = 0;
            ctx->custom_topics[i].qos = qos;
            ctx->custom_topics[i].is_registered = false;
            ctx->custom_topics[i].is_sender = false;
            ctx->custom_topics[i].last_attempt = nil_time;
            ctx->custom_topics[i].in_use = true;
            printf("✓ Added topic '%s' for subscription (receiver, QoS %u)\n", topic_name, qos);
            return true;
        }
    }

    printf("ERROR: No free slots for topic subscription (max %u)\n", MAX_CUSTOM_TOPICS);
    return false;
}

/**
 * @brief Process topic registrations/subscriptions with retry logic
 * @param ctx MQTT-SN context
 * @param pcb UDP PCB
 * @param gw_addr Gateway address
 * @param gw_port Gateway port
 *
 * @note Call this periodically in the main loop
 * @note Automatically retries failed registrations/subscriptions every TOPIC_RETRY_INTERVAL_MS
 */
void mqtt_sn_process_topic_registrations(mqtt_sn_context_t *ctx, struct udp_pcb *pcb,
                                         const ip_addr_t *gw_addr, u16_t gw_port) {
    if (!ctx || !pcb || !gw_addr) {
        return;
    }

    absolute_time_t now = get_absolute_time();

    for (size_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
        if (!ctx->custom_topics[i].in_use) {
            continue;
        }

        // Skip if already registered/subscribed
        if (ctx->custom_topics[i].is_registered) {
            continue;
        }

        // Check if enough time has passed since last attempt
        if (!is_nil_time(ctx->custom_topics[i].last_attempt)) {
            int64_t elapsed_ms =
                absolute_time_diff_us(ctx->custom_topics[i].last_attempt, now) / 1000;
            if (elapsed_ms < TOPIC_RETRY_INTERVAL_MS) {
                continue; // Not time to retry yet
            }
        }

        // Update last attempt time
        ctx->custom_topics[i].last_attempt = now;

        // Send REGISTER or SUBSCRIBE
        uint16_t msg_id = get_next_msg_id();
        if (ctx->custom_topics[i].is_sender) {
            // Sender: Register topic to get topic ID
            mqtt_sn_register_topic(pcb, gw_addr, gw_port, ctx->custom_topics[i].topic_name, msg_id);
        } else {
            // Receiver: Subscribe to topic
            mqtt_sn_subscribe_topic_name(pcb, gw_addr, gw_port, ctx->custom_topics[i].topic_name,
                                         msg_id, ctx->custom_topics[i].qos);
        }
    }
}

/**
 * @brief Get topic ID for a registered topic name
 * @param ctx MQTT-SN context
 * @param topic_name Topic name to look up
 * @return uint16_t Topic ID, or 0 if not found/registered
 */
uint16_t mqtt_sn_get_topic_id(mqtt_sn_context_t *ctx, const char *topic_name) {
    if (!ctx || !topic_name) {
        return 0;
    }

    for (size_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
        if (ctx->custom_topics[i].in_use && ctx->custom_topics[i].is_registered &&
            strcmp(ctx->custom_topics[i].topic_name, topic_name) == 0) {
            return ctx->custom_topics[i].topic_id;
        }
    }

    return 0; // Not found or not registered yet
}
