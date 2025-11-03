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
        s_next_msg_id = 1U;  // Start from 1, skip 0
    }
    return s_next_msg_id++;
}

// Send MQTT-SN CONNECT packet
void mqtt_sn_connect(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port) {
    const char* client_id = MQTT_SN_CLIENT_ID;
    size_t id_len = strlen(client_id);
    u16_t packet_len = MQTTSN_CONNECT_FIXED_LEN + id_len;  // [len][type=0x04][flags][protocol_id][duration(2)][client_id]

    if (packet_len > MQTTSN_MAX_PACKET_LEN) {
        printf("Client ID too long\n");
        return;
    }

    // Allocate pbuf for CONNECT packet
    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, packet_len, PBUF_RAM);
    if (!p)
        return;

    uint8_t* data = (uint8_t*)p->payload;
    data[MQTTSN_OFFSET_LENGTH] = (uint8_t)packet_len;
    data[MQTTSN_OFFSET_MSG_TYPE] = MQTTSN_MSG_TYPE_CONNECT;                                            // CONNECT
    data[MQTTSN_OFFSET_FLAGS] = MQTTSN_FLAG_CLEAN_SESSION;                                             // Flags (clean session)
    data[MQTTSN_OFFSET_PROTOCOL_ID] = MQTTSN_PROTOCOL_ID;                                              // Protocol ID (MQTT-SN v1.2)
    data[MQTTSN_OFFSET_DURATION_HIGH] = (KEEPALIVE_INTERVAL_SEC >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK;  // Duration high
    data[MQTTSN_OFFSET_DURATION_LOW] = KEEPALIVE_INTERVAL_SEC & MQTTSN_BYTE_MASK;                      // Duration low
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
void mqtt_sn_pingreq(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port) {
    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, MQTTSN_PINGREQ_LEN, PBUF_RAM);
    if (!p)
        return;

    uint8_t* data = (uint8_t*)p->payload;
    data[MQTTSN_OFFSET_LENGTH] = MQTTSN_PINGREQ_LEN;         // Length
    data[MQTTSN_OFFSET_MSG_TYPE] = MQTTSN_MSG_TYPE_PINGREQ;  // PINGREQ

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if (err == ERR_OK) {
        printf("Sent PINGREQ\n");
    } else {
        printf("Failed to send PINGREQ: %d\n", err);
    }
    pbuf_free(p);
}

// SUBSCRIBE by Predefined Topic ID
void mqtt_sn_subscribe_topic_id(struct udp_pcb* pcb,
                                const ip_addr_t* gw_addr,
                                u16_t gw_port,
                                u16_t topic_id) {
    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, MQTTSN_SUBSCRIBE_LEN, PBUF_RAM);
    if (!p)
        return;

    uint8_t* data = (uint8_t*)p->payload;
    data[MQTTSN_OFFSET_LENGTH] = MQTTSN_SUBSCRIBE_LEN;                                  // length
    data[MQTTSN_OFFSET_MSG_TYPE] = MQTTSN_MSG_TYPE_SUBSCRIBE;                           // SUBSCRIBE
    data[MQTTSN_OFFSET_FLAGS] = MQTTSN_SUBSCRIBE_FLAGS_QOS2;                            // flags: QoS2, TopicIdType=predefined
    data[MQTTSN_OFFSET_PROTOCOL_ID] = 0x00U;                                            // msg ID high
    data[MQTTSN_OFFSET_DURATION_HIGH] = 0x01U;                                          // msg ID low
    data[MQTTSN_OFFSET_DURATION_LOW] = (topic_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK;  // topic ID high
    data[MQTTSN_OFFSET_CLIENT_ID] = topic_id & MQTTSN_BYTE_MASK;                        // topic ID low

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if (err == ERR_OK) {
        printf("Sent SUBSCRIBE to Predefined Topic ID %d\n", topic_id);
    } else {
        printf("Failed to send SUBSCRIBE: %d\n", err);
    }
    pbuf_free(p);
}

// PUBLISH to Predefined Topic ID with qos, binary payload support
void mqtt_sn_publish_topic_id(struct udp_pcb* pcb,
                              const ip_addr_t* gw_addr,
                              u16_t gw_port,
                              u16_t topic_id,
                              const uint8_t* payload,
                              size_t payload_len,
                              int qos,
                              uint16_t msg_id,
                              bool is_retransmit) {
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

    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, packet_len, PBUF_RAM);
    if (!p) {
        printf("Failed to allocate pbuf\n");
        return;
    }

    // QOS Flags
    uint8_t flags = MQTTSN_FLAG_TOPIC_PREDEFINED;  // TopicIdType = Predefined

    if (qos == QOS_LEVEL_1) {
        flags |= MQTTSN_FLAG_QOS1;  // QoS 1 (bit 5 = 1)
    } else if (qos == QOS_LEVEL_2) {
        flags |= MQTTSN_FLAG_QOS2;  // QoS 2 (bit 6 = 1)
    }

    uint8_t* data = (uint8_t*)p->payload;
    data[MQTTSN_OFFSET_LENGTH] = (uint8_t)packet_len;
    data[MQTTSN_OFFSET_MSG_TYPE] = MQTTSN_MSG_TYPE_PUBLISH;  // PUBLISH
    data[MQTTSN_OFFSET_FLAGS] = flags;
    data[MQTTSN_OFFSET_TOPIC_ID_HIGH] = (topic_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK;  // Topic ID high
    data[MQTTSN_OFFSET_TOPIC_ID_LOW] = topic_id & MQTTSN_BYTE_MASK;                      // Topic ID low
    data[MQTTSN_OFFSET_MSG_ID_HIGH] = (qos > QOS_LEVEL_0) ? (msg_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK : 0x00U;
    data[MQTTSN_OFFSET_MSG_ID_LOW] = (qos > QOS_LEVEL_0) ? (msg_id & MQTTSN_BYTE_MASK) : 0x00U;
    memcpy(&data[MQTTSN_OFFSET_PAYLOAD], payload, payload_len);

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if (err == ERR_OK) {
        printf("Sent PUBLISH to Topic ID %d (QoS %d, Msg ID %d, Len %d)\n", topic_id, qos, msg_id, packet_len);

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
                    size_t copy_len = (payload_len < sizeof(g_pending_msgs[i].payload)) ? payload_len : sizeof(g_pending_msgs[i].payload);
                    if (payload_len > sizeof(g_pending_msgs[i].payload)) {
                        printf("WARNING: Payload truncated from %zu to %zu bytes for QoS tracking\n",
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
                printf("ERROR: All QoS slots full (%d), message %d will not be tracked for retransmission\n",
                       MAX_PENDING_QOS_MSGS, msg_id);
            }
        }
    } else {
        printf("Failed to send PUBLISH: %d\n", err);
    }
    pbuf_free(p);
}

// Send PUBACK for QoS 1
void mqtt_sn_send_puback(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port,
                         uint16_t topic_id, uint16_t msg_id, uint8_t return_code) {
    uint8_t msg[MQTTSN_PUBACK_LEN];
    msg[MQTTSN_OFFSET_LENGTH] = MQTTSN_PUBACK_LEN;         // Length
    msg[MQTTSN_OFFSET_MSG_TYPE] = MQTTSN_MSG_TYPE_PUBACK;  // PUBACK
    msg[MQTTSN_OFFSET_FLAGS] = (topic_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_PROTOCOL_ID] = topic_id & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_DURATION_HIGH] = (msg_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_DURATION_LOW] = msg_id & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_CLIENT_ID] = return_code;  // typically MQTTSN_RETURN_ACCEPTED

    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    if (!p) return;
    memcpy(p->payload, msg, sizeof(msg));
    udp_sendto(pcb, p, gw_addr, gw_port);
    pbuf_free(p);

    printf("Sent PUBACK (topic_id: %u, msg_id: %u, rc: %u)\n", topic_id, msg_id, return_code);
}

// Send PUBREC for QoS 2
void mqtt_sn_send_pubrec(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port, uint16_t msg_id) {
    uint8_t msg[MQTTSN_PUBREC_LEN];
    msg[MQTTSN_OFFSET_LENGTH] = MQTTSN_PUBREC_LEN;         // Length
    msg[MQTTSN_OFFSET_MSG_TYPE] = MQTTSN_MSG_TYPE_PUBREC;  // PUBREC
    msg[MQTTSN_OFFSET_FLAGS] = (msg_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_PROTOCOL_ID] = msg_id & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_DURATION_HIGH] = MQTTSN_RETURN_ACCEPTED;  // Return code: ACCEPTED

    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    if (!p) return;
    memcpy(p->payload, msg, sizeof(msg));
    udp_sendto(pcb, p, gw_addr, gw_port);
    pbuf_free(p);
    printf("Sent PUBREC for Msg ID: %d\n", msg_id);
}

//  Send PUBCOMP for QoS 2
void mqtt_sn_send_pubcomp(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port, uint16_t msg_id) {
    uint8_t msg[MQTTSN_PUBCOMP_LEN];
    msg[MQTTSN_OFFSET_LENGTH] = MQTTSN_PUBCOMP_LEN;         // Length
    msg[MQTTSN_OFFSET_MSG_TYPE] = MQTTSN_MSG_TYPE_PUBCOMP;  // PUBCOMP
    msg[MQTTSN_OFFSET_FLAGS] = (msg_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_PROTOCOL_ID] = msg_id & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_DURATION_HIGH] = MQTTSN_RETURN_ACCEPTED;  // Return code: ACCEPTED

    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    if (!p) return;
    memcpy(p->payload, msg, sizeof(msg));
    udp_sendto(pcb, p, gw_addr, gw_port);
    pbuf_free(p);
    printf("Sent PUBCOMP for Msg ID: %d\n", msg_id);
}

//  Send PUBREL for QoS 2
void mqtt_sn_send_pubrel(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port, uint16_t msg_id) {
    uint8_t msg[MQTTSN_PUBREL_LEN];
    msg[MQTTSN_OFFSET_LENGTH] = MQTTSN_PUBREL_LEN;         // Length
    msg[MQTTSN_OFFSET_MSG_TYPE] = MQTTSN_MSG_TYPE_PUBREL;  // PUBREL
    msg[MQTTSN_OFFSET_FLAGS] = (msg_id >> BITS_PER_BYTE) & MQTTSN_BYTE_MASK;
    msg[MQTTSN_OFFSET_PROTOCOL_ID] = msg_id & MQTTSN_BYTE_MASK;

    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    if (!p) return;

    memcpy(p->payload, msg, sizeof(msg));
    udp_sendto(pcb, p, gw_addr, gw_port);
    pbuf_free(p);
}

// Check and handle QoS message timeouts and retransmissions
void check_qos_timeouts(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port) {
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
        if (!g_pending_msgs[i].in_use) continue;

        // Check if timeout exceeded
        if (absolute_time_diff_us(g_pending_msgs[i].timestamp, now) > QOS_RETRY_INTERVAL_US) {
            // Check if max retries reached
            if (g_pending_msgs[i].retry_count >= QOS_MAX_RETRIES) {
                printf("QoS %d Msg ID %d failed after %d retries\n",
                       g_pending_msgs[i].qos, g_pending_msgs[i].msg_id, QOS_MAX_RETRIES);
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
            memcpy(retry_list[retry_count].payload, g_pending_msgs[i].payload, g_pending_msgs[i].payload_len);

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
        if (!retry_list[i].valid) continue;

        if (retry_list[i].qos == QOS_LEVEL_1) {
            printf("Retransmitting QoS1 PUBLISH for Msg ID %d\n", retry_list[i].msg_id);
            mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port,
                                     retry_list[i].topic_id,
                                     retry_list[i].payload,
                                     retry_list[i].payload_len,
                                     retry_list[i].qos,
                                     retry_list[i].msg_id,
                                     true);
        } else if (retry_list[i].qos == QOS_LEVEL_2) {
            if (retry_list[i].step == 0) {
                printf("Retransmitting QoS2 PUBLISH for Msg ID %d\n", retry_list[i].msg_id);
                mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port,
                                         retry_list[i].topic_id,
                                         retry_list[i].payload,
                                         retry_list[i].payload_len,
                                         retry_list[i].qos,
                                         retry_list[i].msg_id,
                                         true);
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
        printf("WARNING: msg_id %u not found in pending queue (may have already been removed)\n", msg_id);
    }
}

// Callback for when UDP data is received
void udp_recv_callback(
    void* arg, struct udp_pcb* pcb, struct pbuf* p, const ip_addr_t* addr, u16_t port) {
    mqtt_sn_context_t* ctx = (mqtt_sn_context_t*)arg;

    if (p != NULL && p->len >= MQTTSN_HEADER_SIZE) {
        uint8_t* data = (uint8_t*)p->payload;
        uint8_t length = data[MQTTSN_OFFSET_LENGTH];
        uint8_t msg_type = data[MQTTSN_OFFSET_MSG_TYPE];

        // Simulate dropping ACKs
        if (ctx && ctx->drop_acks &&
            (msg_type == MQTTSN_MSG_TYPE_PUBACK ||
             msg_type == MQTTSN_MSG_TYPE_PUBREC ||
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
            printf("CONNACK: return_code=%d (%s)\n",
                   return_code,
                   return_code == MQTTSN_RETURN_ACCEPTED ? "Accepted" : "Rejected");
            g_ping_ack_received = true;
        }

        // SUBACK
        else if (msg_type == MQTTSN_MSG_TYPE_SUBACK) {
            uint8_t flags = data[MQTTSN_OFFSET_FLAGS];
            uint16_t topic_id = (data[MQTTSN_OFFSET_PROTOCOL_ID] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_DURATION_HIGH];
            uint16_t msg_id = (data[MQTTSN_OFFSET_DURATION_LOW] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_CLIENT_ID];
            uint8_t return_code = data[MQTTSN_OFFSET_PAYLOAD];
            printf("SUBACK: topic_id=%d, msg_id=%d, return_code=%d\n", topic_id, msg_id, return_code);
        }

        // PUBLISH received
        else if (msg_type == MQTTSN_MSG_TYPE_PUBLISH) {
            if (length >= MQTTSN_PUBLISH_HEADER_LEN) {
                uint8_t flags = data[MQTTSN_OFFSET_FLAGS];
                uint8_t qos = (flags >> MQTTSN_QOS_SHIFT) & MQTTSN_FLAG_QOS_MASK;
                uint16_t topic_id = (data[MQTTSN_OFFSET_TOPIC_ID_HIGH] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_TOPIC_ID_LOW];
                uint16_t msg_id = (data[MQTTSN_OFFSET_MSG_ID_HIGH] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_MSG_ID_LOW];
                int payload_len = length - MQTTSN_PUBLISH_HEADER_LEN;
                const uint8_t* payload = &data[MQTTSN_OFFSET_PAYLOAD];

                // Check for file transfer topics first
                if (topic_id == FILE_TRANSFER_TOPIC_METADATA) {
                    // file/meta topic (expects QoS 2)
                    printf("PUBLISH: File metadata received (QoS %d, Msg ID %d)\n", qos, msg_id);
                    handle_file_metadata(ctx, payload, payload_len);
                    // Send appropriate QoS ACK
                    if (qos == QOS_LEVEL_1) {
                        mqtt_sn_send_puback(pcb, addr, port, topic_id, msg_id, MQTTSN_RETURN_ACCEPTED);
                    } else if (qos == QOS_LEVEL_2) {
                        mqtt_sn_send_pubrec(pcb, addr, port, msg_id);
                    }
                } else if (topic_id == FILE_TRANSFER_TOPIC_DATA) {
                    // file/data topic (expects QoS 1)
                    handle_file_payload(ctx, payload, payload_len);
                    // Send PUBACK for QoS 1
                    if (qos == QOS_LEVEL_1) {
                        mqtt_sn_send_puback(pcb, addr, port, topic_id, msg_id, MQTTSN_RETURN_ACCEPTED);
                    }
                } else {
                    // Regular message handling
                    // Print binary payload in hex
                    printf("PUBLISH received (QoS %d, Msg ID %d), Payload (%d bytes): ", qos, msg_id, payload_len);
                    for (size_t i = 0U; i < (size_t)payload_len; i++) {
                        printf("%02X ", payload[i]);
                    }
                    printf("\n");

                    // handle text commands embedded in binary
                    if (payload_len == LED_ON_CMD_LEN && memcmp(payload, "led on", LED_ON_CMD_LEN) == 0) {
                        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1U);
                    } else if (payload_len == LED_OFF_CMD_LEN && memcmp(payload, "led off", LED_OFF_CMD_LEN) == 0) {
                        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0U);
                    }

                    // QoS ACKs
                    if (qos == QOS_LEVEL_1) {
                        mqtt_sn_send_puback(pcb, addr, port, topic_id, msg_id, MQTTSN_RETURN_ACCEPTED);
                    } else if (qos == QOS_LEVEL_2) {
                        mqtt_sn_send_pubrec(pcb, addr, port, msg_id);
                    }
                }
            }
        }

        // PUBACK QoS 1 RECEIVED
        else if (msg_type == MQTTSN_MSG_TYPE_PUBACK) {
            uint16_t msg_id = (data[MQTTSN_OFFSET_DURATION_HIGH] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_DURATION_LOW];
            printf("PUBACK received for Msg ID: %d \n", msg_id);
            remove_pending_qos_msg(msg_id);
        }

        // PUBCOMP (QoS 2 final ack) RECEIVED
        else if (msg_type == MQTTSN_MSG_TYPE_PUBCOMP) {
            uint16_t msg_id = (data[MQTTSN_OFFSET_FLAGS] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_PROTOCOL_ID];
            printf("PUBCOMP received for Msg ID: %d\n", msg_id);
            remove_pending_qos_msg(msg_id);
        }
        // PUBREC (QoS 2 Step 1) RECEIVED
        else if (msg_type == MQTTSN_MSG_TYPE_PUBREC) {
            uint16_t msg_id = (data[MQTTSN_OFFSET_FLAGS] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_PROTOCOL_ID];
            printf("PUBREC received for Msg ID: %d. Sending PUBREL...\n", msg_id);
            // Send PUBREL in response
            mqtt_sn_send_pubrel(pcb, addr, port, msg_id);
            // Update step of retransmission packet to indicate PUBREL was sent
            for (size_t i = 0U; i < MAX_PENDING_QOS_MSGS; i++) {
                if (g_pending_msgs[i].in_use && g_pending_msgs[i].msg_id == msg_id) {
                    g_pending_msgs[i].step = 1U;
                    g_pending_msgs[i].timestamp = get_absolute_time();  // reset timer
                    break;
                }
            }
        }
        // PUBREL (QoS 2 Step 2) RECEIVED
        else if (msg_type == MQTTSN_MSG_TYPE_PUBREL) {
            // send PUBCOMP
            uint16_t msg_id = (data[MQTTSN_OFFSET_FLAGS] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_PROTOCOL_ID];
            printf("PUBREL received for Msg ID: %d. Sending PUBCOMP...\n", msg_id);
            mqtt_sn_send_pubcomp(pcb, addr, port, msg_id);
            remove_pending_qos_msg(msg_id);
        }
        pbuf_free(p);
    } else {
        if (p)
            pbuf_free(p);  // Always free if not handled
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
void send_file_via_mqtt(struct udp_pcb* pcb,
                        const ip_addr_t* gw_addr,
                        u16_t gw_port,
                        const char* filename) {
    struct Metadata metadata = {0};

    printf("\n=== Starting STREAMING File Transfer ===\n");
    printf("File: %s\n", filename);
    printf("Using QoS %d for metadata, QoS %d for data chunks\n",
           FILE_TRANSFER_METADATA_QOS, FILE_TRANSFER_DATA_QOS);
    printf("Mode: STREAMING (memory efficient)\n");

    // Step 1: Initialize streaming read
    if (init_streaming_read((char*)filename, &metadata) != 0) {
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
    uint16_t msg_id = get_next_msg_id();
    mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port, FILE_TRANSFER_TOPIC_METADATA,
                             meta_buffer, PAYLOAD_SIZE, FILE_TRANSFER_METADATA_QOS, msg_id, false);
    printf("Sent metadata (QoS %d, msg_id=%u)\n", FILE_TRANSFER_METADATA_QOS, msg_id);
    sleep_ms(QOS2_HANDSHAKE_DELAY_MS);  // Allow time for QoS 2 handshake (PUBREC/PUBREL/PUBCOMP)

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
        mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port, FILE_TRANSFER_TOPIC_DATA,
                                 payload_buffer, PAYLOAD_SIZE, FILE_TRANSFER_DATA_QOS, msg_id, false);

        chunks_sent++;

        // Progress updates
        if ((i + 1) % PROGRESS_UPDATE_INTERVAL == 0 || i == metadata.chunk_count - 1) {
            float progress = ((float)(i + 1) / metadata.chunk_count) * 100.0f;
            printf("  [%lu/%lu] %.1f%% complete (msg_id=%u)\n",
                   (unsigned long)(i + 1),
                   (unsigned long)metadata.chunk_count,
                   progress,
                   msg_id);
        }

        // Interleaved delay with network polling to process incoming PUBACKs
        // This prevents QoS 1 timeout/retransmission during file transfer
        absolute_time_t delay_start = get_absolute_time();
        while (absolute_time_diff_us(delay_start, get_absolute_time()) < INTER_CHUNK_DELAY_US) {
            cyw43_arch_poll();              // Process network events
            sleep_us(POLL_YIELD_DELAY_US);  // Yield CPU briefly
        }
    }

    absolute_time_t end_time = get_absolute_time();
    int64_t elapsed_ms = absolute_time_diff_us(start_time, end_time) / 1000;

    // Cleanup streaming session
    cleanup_streaming_read();

    printf("✓ File transfer complete:\n");
    printf("  Total packets: %lu (1 metadata + %lu data)\n",
           (unsigned long)(chunks_sent + 1),
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
void handle_file_metadata(mqtt_sn_context_t* ctx, const uint8_t* payload, size_t len) {
    if (!ctx || !payload) {
        printf("ERROR: NULL parameter in handle_file_metadata\n");
        return;
    }

    if (len != PAYLOAD_SIZE) {
        printf("ERROR: Invalid metadata size (expected %d, got %zu)\n", PAYLOAD_SIZE, len);
        return;
    }

    struct Metadata metadata = {0};
    if (deserialize_metadata((uint8_t*)payload, &metadata) != 0) {
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

    // Initialize filesystem if not already done
    if (!ctx->fs_info) {
        printf("ERROR: Filesystem not initialized\n");
        return;
    }

    // Initialize transfer session
    if (!ctx->file_session) {
        printf("ERROR: No session buffer allocated\n");
        return;
    }

    // Use new filename (true) to avoid overwriting existing files
    if (!chunk_transfer_init_session(ctx->fs_info, &metadata, ctx->file_session, true)) {
        printf("ERROR: Failed to init transfer session\n");
        return;
    }

    ctx->transfer_in_progress = true;
    printf("✓ Transfer session active\n");
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
void handle_file_payload(mqtt_sn_context_t* ctx, const uint8_t* payload, size_t len) {
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
    if (deserialize_payload((uint8_t*)payload, &chunk) != 0) {
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
        printf("Progress: %lu/%lu chunks received\n", (unsigned long)received, (unsigned long)total);
    }

    // Check if transfer complete
    if (chunk_transfer_is_complete(ctx->file_session)) {
        printf("\n=== File Transfer Complete ===\n");
        printf("All chunks received! Finalizing...\n");

        if (chunk_transfer_finalize(ctx->fs_info, ctx->file_session)) {
            printf("✓ File saved: %s\n", ctx->file_session->filename);
            printf("  Size: %lu bytes\n", (unsigned long)ctx->file_session->metadata.total_size);
        } else {
            printf("ERROR: Failed to finalize transfer\n");
        }

        ctx->transfer_in_progress = false;
        printf("==============================\n\n");
    }
}
