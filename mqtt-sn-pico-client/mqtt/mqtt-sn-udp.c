// mqtt-sn-udp.c
#include "mqtt-sn-udp.h"

#include <stdio.h>
#include <string.h>

#include "../config.h"
#include "../drivers/microsd_driver.h"
#include "config.h"
#include "hardware/sync.h"
#include "lwip/pbuf.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

/*! Module-level state variables */
static uint16_t s_next_msg_id = 1U;
qos_msg_t g_pending_msgs[MAX_PENDING_QOS_MSGS];

/*! Forward declarations for message handlers */
typedef void (*msg_handler_fn)(mqtt_sn_context_t *ctx, struct udp_pcb *pcb, const uint8_t *data,
                               uint8_t length, const ip_addr_t *addr, u16_t port);

static void handle_pingresp(mqtt_sn_context_t *ctx, struct udp_pcb *pcb, const uint8_t *data,
                            uint8_t length, const ip_addr_t *addr, u16_t port);
static void handle_connack(mqtt_sn_context_t *ctx, struct udp_pcb *pcb, const uint8_t *data,
                           uint8_t length, const ip_addr_t *addr, u16_t port);
static void handle_suback(mqtt_sn_context_t *ctx, struct udp_pcb *pcb, const uint8_t *data,
                          uint8_t length, const ip_addr_t *addr, u16_t port);
static void handle_regack(mqtt_sn_context_t *ctx, struct udp_pcb *pcb, const uint8_t *data,
                          uint8_t length, const ip_addr_t *addr, u16_t port);
static void handle_publish_received(mqtt_sn_context_t *ctx, struct udp_pcb *pcb,
                                    const uint8_t *data, uint8_t length, const ip_addr_t *addr,
                                    u16_t port);
static void handle_puback_received(mqtt_sn_context_t *ctx, struct udp_pcb *pcb, const uint8_t *data,
                                   uint8_t length, const ip_addr_t *addr, u16_t port);
static void handle_pubcomp_received(mqtt_sn_context_t *ctx, struct udp_pcb *pcb,
                                    const uint8_t *data, uint8_t length, const ip_addr_t *addr,
                                    u16_t port);
static void handle_pubrec_received(mqtt_sn_context_t *ctx, struct udp_pcb *pcb, const uint8_t *data,
                                   uint8_t length, const ip_addr_t *addr, u16_t port);
static void handle_pubrel_received(mqtt_sn_context_t *ctx, struct udp_pcb *pcb, const uint8_t *data,
                                   uint8_t length, const ip_addr_t *addr, u16_t port);

/*! Message handler lookup table */
typedef struct {
    uint8_t msg_type;
    msg_handler_fn handler;
} msg_handler_entry_t;

static const msg_handler_entry_t msg_handlers[] = {
    {MQTTSN_MSG_TYPE_PINGRESP, handle_pingresp},
    {MQTTSN_MSG_TYPE_CONNACK, handle_connack},
    {MQTTSN_MSG_TYPE_SUBACK, handle_suback},
    {MQTTSN_MSG_TYPE_REGACK, handle_regack},
    {MQTTSN_MSG_TYPE_PUBLISH, handle_publish_received},
    {MQTTSN_MSG_TYPE_PUBACK, handle_puback_received},
    {MQTTSN_MSG_TYPE_PUBCOMP, handle_pubcomp_received},
    {MQTTSN_MSG_TYPE_PUBREC, handle_pubrec_received},
    {MQTTSN_MSG_TYPE_PUBREL, handle_pubrel_received},
};

#define NUM_MSG_HANDLERS (sizeof(msg_handlers) / sizeof(msg_handler_entry_t))

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

/*!
 * @brief Get QoS flags for MQTT-SN packet
 * @param qos QoS level (0, 1, or 2)
 * @param base_flags Base flags to OR with QoS flags
 * @return uint8_t Flags byte with QoS bits set
 */
static inline uint8_t get_qos_flags(int qos, uint8_t base_flags) {
    static const uint8_t qos_flag_map[] = {
        [QOS_LEVEL_0] = 0x00,
        [QOS_LEVEL_1] = MQTTSN_FLAG_QOS1,
        [QOS_LEVEL_2] = MQTTSN_FLAG_QOS2,
    };

    if (qos < 0 || qos > QOS_LEVEL_2) {
        return base_flags; // Invalid QoS, return base flags only
    }

    return base_flags | qos_flag_map[qos];
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
    uint8_t flags = get_qos_flags(qos, 0x00); // TopicIdType = 0 (normal name)

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

// Auto-generates msg_id for initial publishes (QoS > 0)
void mqtt_sn_publish_topic_id_auto(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port,
                                   u16_t topic_id, const uint8_t *payload, size_t payload_len,
                                   int qos) {
    uint16_t msg_id = (qos > QOS_LEVEL_0) ? get_next_msg_id() : 0;
    mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port, topic_id, payload, payload_len, qos, msg_id,
                             false);
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
    uint8_t flags = get_qos_flags(qos, MQTTSN_FLAG_TOPIC_NORMAL); // TopicIdType = Dynamic

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
        // Only log for QoS 0, retransmits, or every 100th message to reduce spam
        static uint32_t publish_count = 0;
        if (qos == QOS_LEVEL_0 || is_retransmit || (++publish_count % 100 == 0)) {
            printf("Sent PUBLISH to Topic ID %d (QoS %d, Msg ID %d, Len %d)%s\n", topic_id, qos,
                   msg_id, packet_len, is_retransmit ? " [RETRY]" : "");
        }

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

        switch (retry_list[i].qos) {
        case QOS_LEVEL_1:
            printf("Retransmitting QoS1 PUBLISH for Msg ID %d\n", retry_list[i].msg_id);
            mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port, retry_list[i].topic_id,
                                     retry_list[i].payload, retry_list[i].payload_len,
                                     retry_list[i].qos, retry_list[i].msg_id, true);
            break;

        case QOS_LEVEL_2:
            if (retry_list[i].step == 0) {
                printf("Retransmitting QoS2 PUBLISH for Msg ID %d\n", retry_list[i].msg_id);
                mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port, retry_list[i].topic_id,
                                         retry_list[i].payload, retry_list[i].payload_len,
                                         retry_list[i].qos, retry_list[i].msg_id, true);
            } else {
                printf("Retransmitting PUBREL for Msg ID %d\n", retry_list[i].msg_id);
                mqtt_sn_send_pubrel(pcb, gw_addr, gw_port, retry_list[i].msg_id);
            }
            break;

        default:
            break;
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

        // Dispatch to appropriate handler using lookup table
        bool handled = false;
        for (size_t i = 0; i < NUM_MSG_HANDLERS; i++) {
            if (msg_handlers[i].msg_type == msg_type) {
                msg_handlers[i].handler(ctx, pcb, data, length, addr, port);
                handled = true;
                break;
            }
        }

        if (!handled) {
            printf("WARNING: Unhandled message type 0x%02X\n", msg_type);
        }

        pbuf_free(p);
    } else {
        if (p)
            pbuf_free(p); // Always free if not handled
    }
}

// ============================================================================
// Go-Back-N Sliding Window Protocol Helper Functions
// ============================================================================

/*!
 * @brief Initialize sliding window state for Go-Back-N protocol
 * @param window Pointer to sliding window structure
 * @param total_chunks Total number of chunks in file
 * @param session_id Session identifier for this transfer
 * @return bool true on success
 */
bool init_sliding_window(sliding_window_t *window, uint32_t total_chunks, const char *session_id) {
    if (!window || !session_id)
        return false;

    window->base = 1; // Start from chunk 1 (0 is metadata)
    window->next_seq = 1;
    window->window_size = WINDOW_SIZE_CHUNKS;
    window->total_chunks = total_chunks;
    window->retries = 0;
    window->active = true;
    strncpy(window->session_id, session_id, sizeof(window->session_id) - 1);
    window->session_id[sizeof(window->session_id) - 1] = '\0';

    // Allocate ACK bitmap for window
    window->acked = (bool *)calloc(window->window_size, sizeof(bool));
    if (!window->acked) {
        printf("ERROR: Failed to allocate ACK bitmap\n");
        return false;
    }

    window->last_send_time = get_absolute_time();

    printf("Sliding Window initialized:\n");
    printf("  Window size: %u chunks (%u bytes)\n", window->window_size,
           window->window_size * CHUNK_SIZE);
    printf("  Total chunks: %u\n", total_chunks);
    printf("  Session ID: %s\n", window->session_id);

    return true;
}

/*!
 * @brief Clean up sliding window resources
 * @param window Pointer to sliding window structure
 */
void cleanup_sliding_window(sliding_window_t *window) {
    if (window && window->acked) {
        free(window->acked);
        window->acked = NULL;
    }
    if (window) {
        window->active = false;
    }
}

/*!
 * @brief Send a control message to file/control topic
 * @param pcb UDP PCB
 * @param gw_addr Gateway address
 * @param gw_port Gateway port
 * @param ctrl_msg Control message to send
 */
void send_control_message(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port,
                          const control_message_t *ctrl_msg, mqtt_sn_context_t *ctx) {
    if (!ctrl_msg || !ctx)
        return;

    // Look up the actual topic ID for file/control (it's dynamically assigned)
    uint16_t topic_id = mqtt_sn_get_topic_id(ctx, "file/control");
    if (topic_id == 0) {
        printf("ERROR: file/control topic not registered\n");
        return;
    }

    // Publish control message to file/control topic with QoS 1
    mqtt_sn_publish_topic_id_auto(pcb, gw_addr, gw_port, topic_id, (const uint8_t *)ctrl_msg,
                                  sizeof(control_message_t), QOS_LEVEL_1);

    // Log control message
    switch (ctrl_msg->type) {
    case CTRL_ACK:
        printf("  [CONTROL] Sent ACK up to chunk %u\n", ctrl_msg->seq_num);
        break;
    case CTRL_NACK:
        printf("  [CONTROL] Sent NACK - retransmit from chunk %u\n", ctrl_msg->seq_num + 1);
        break;
    case CTRL_REQUEST_NEXT:
        printf("  [CONTROL] Sent REQUEST_NEXT window [%u-%u]\n", ctrl_msg->window_start,
               ctrl_msg->window_end - 1);
        break;
    case CTRL_COMPLETE:
        printf("  [CONTROL] Sent TRANSFER_COMPLETE\n");
        break;
    }
}

/*!
 * @brief Check if chunk is within current window
 * @param window Pointer to sliding window structure
 * @param seq Sequence number to check
 * @return bool true if within window
 */
static bool is_in_window(const sliding_window_t *window, uint32_t seq) {
    return (seq >= window->base && seq < window->base + window->window_size);
}

/*!
 * @brief Process ACK and slide window if possible
 * @param window Pointer to sliding window structure
 * @param ack_seq Sequence number being acknowledged
 */
static void process_ack(sliding_window_t *window, uint32_t ack_seq) {
    if (!is_in_window(window, ack_seq)) {
        return; // ACK outside window, ignore
    }

    // Mark chunk as ACKed
    uint32_t index = (ack_seq - window->base) % window->window_size;
    window->acked[index] = true;

    // Try to slide window forward
    while (window->base < window->total_chunks + 1) {
        // Always check index 0 (the base of the window)
        if (!window->acked[0]) {
            break; // Stop at first unACKed chunk
        }

        // Slide window forward - shift all acked flags left
        for (uint32_t i = 0; i < window->window_size - 1; i++) {
            window->acked[i] = window->acked[i + 1];
        }
        window->acked[window->window_size - 1] = false; // Clear last position
        window->base++;
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
                        const char *filename, mqtt_sn_context_t *ctx) {
    struct Metadata metadata = {0};
    uint16_t msg_id;

    if (!ctx) {
        printf("ERROR: NULL context\n");
        return;
    }

    // Look up the actual topic ID for file/data (it's dynamically assigned)
    uint16_t topic_id = mqtt_sn_get_topic_id(ctx, "file/data");
    if (topic_id == 0) {
        printf("ERROR: file/data topic not registered\n");
        return;
    }

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
    printf("  Memory: ~33 KB (32KB read buffer + chunk)\n");
    printf("  Note: File CRC will be calculated incrementally\n");

    // Step 2: Serialize and send metadata (chunk 0)
    uint8_t meta_buffer[PAYLOAD_SIZE];
    if (serialize_metadata(&metadata, meta_buffer) != PAYLOAD_SIZE) {
        printf("ERROR: Failed to serialize metadata\n");
        cleanup_streaming_read();
        return;
    }

    // Publish metadata to file/data topic with QoS 2 for guaranteed delivery
    uint16_t metadata_msg_id = get_next_msg_id();
    mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port, topic_id, meta_buffer, PAYLOAD_SIZE,
                             FILE_TRANSFER_METADATA_QOS, metadata_msg_id, false);
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

        // Publish to file/data topic with QoS 1
        msg_id = get_next_msg_id();
        mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port, topic_id, payload_buffer, PAYLOAD_SIZE,
                                 FILE_TRANSFER_DATA_QOS, msg_id, false);

        chunks_sent++;

        // Progress updates (reduced verbosity)
        if ((i + 1) % PROGRESS_UPDATE_INTERVAL == 0 || i == metadata.chunk_count - 1) {
            float progress = ((float)(i + 1) / metadata.chunk_count) * 100.0f;
            printf("  [%lu/%lu] %.1f%% complete\n", (unsigned long)(i + 1),
                   (unsigned long)metadata.chunk_count, progress);
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
    printf("  Peak memory: ~33 KB (32KB buffer + chunk)\n");
    printf("  SD card reads: ~%lu (vs %lu without buffering)\n",
           (unsigned long)((metadata.total_size + 32767) / 32768), (unsigned long)chunks_sent);
    printf("==============================\n\n");
}

/**
 * @brief Send a file via MQTT-SN using Go-Back-N sliding window protocol
 *
 * This function implements the Go-Back-N protocol with:
 * - 32KB sliding windows (~138 chunks per window)
 * - Flow control via file/control topic
 * - Automatic retransmission on NACK
 * - Window-based transmission with REQUEST_NEXT handshake
 *
 * @param pcb UDP PCB
 * @param gw_addr Gateway address
 * @param gw_port Gateway port
 * @param filename Name of file to send from microSD
 * @param ctx MQTT-SN context (must have file/control topic registered)
 *
 * @note Sender must subscribe to file/control topic before calling this function
 * @note Receiver publishes ACK/NACK/REQUEST_NEXT to file/control topic
 */
void send_file_via_mqtt_gbn(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port,
                            const char *filename, mqtt_sn_context_t *ctx) {
    if (!ctx) {
        printf("ERROR: NULL context\n");
        return;
    }

    // Look up the actual topic ID for file/data (it's dynamically assigned)
    uint16_t topic_id = mqtt_sn_get_topic_id(ctx, "file/data");
    if (topic_id == 0) {
        printf("ERROR: file/data topic not registered\n");
        return;
    }

    struct Metadata metadata = {0};
    uint16_t msg_id;

    printf("\n=== Starting Go-Back-N File Transfer ===\n");
    printf("File: %s\n", filename);
    printf("Protocol: Go-Back-N Sliding Window\n");
    printf("Window size: %u chunks (%u bytes)\n", WINDOW_SIZE_CHUNKS, WINDOW_SIZE_BYTES);

    // Step 1: Initialize streaming read
    if (init_streaming_read((char *)filename, &metadata) != 0) {
        printf("ERROR: Failed to initialize streaming read\n");
        return;
    }

    printf("✓ File opened for streaming:\n");
    printf("  File size: %lu bytes\n", (unsigned long)metadata.total_size);
    printf("  Chunks: %lu\n", (unsigned long)metadata.chunk_count);
    printf("  Session ID: %s\n", metadata.session_id);

    // Step 2: Send metadata (chunk 0)
    uint8_t meta_buffer[PAYLOAD_SIZE];
    if (serialize_metadata(&metadata, meta_buffer) != PAYLOAD_SIZE) {
        printf("ERROR: Failed to serialize metadata\n");
        cleanup_streaming_read();
        return;
    }

    uint16_t metadata_msg_id = get_next_msg_id();
    mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port, topic_id, meta_buffer, PAYLOAD_SIZE,
                             FILE_TRANSFER_METADATA_QOS, metadata_msg_id, false);
    printf("[MQTT:file/data] Sent metadata (QoS %d, msg_id=%u)\n", FILE_TRANSFER_METADATA_QOS,
           metadata_msg_id);

    // Wait for metadata confirmation
    printf("Waiting for metadata confirmation...\n");
    absolute_time_t wait_start = get_absolute_time();
    while (absolute_time_diff_us(wait_start, get_absolute_time()) <
           (METADATA_CONFIRM_TIMEOUT_MS * 1000)) {
        cyw43_arch_poll();

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
        sleep_ms(10);
    }

    // Step 3: Initialize sliding window
    if (!init_sliding_window(&ctx->tx_window, metadata.chunk_count, metadata.session_id)) {
        cleanup_streaming_read();
        return;
    }

    uint32_t total_transmitted = 0;
    uint32_t total_retransmissions = 0;
    absolute_time_t start_time = get_absolute_time();

    // Step 4: Main transmission loop with Go-Back-N
    printf("\nStarting Go-Back-N transmission...\n");

    while (ctx->tx_window.base <= metadata.chunk_count) {
        uint32_t window_end = ctx->tx_window.base + ctx->tx_window.window_size;
        if (window_end > metadata.chunk_count + 1) {
            window_end = metadata.chunk_count + 1;
        }

        printf("\n--- Window [%u-%u] ---\n", ctx->tx_window.base, window_end - 1);

        uint32_t window_start_base = ctx->tx_window.base;
        bool nack_received = false;

        // Transmit window
        for (uint32_t seq = ctx->tx_window.base; seq < window_end; seq++) {
            uint32_t index = (seq - ctx->tx_window.base) % ctx->tx_window.window_size;

            // Skip already ACKed chunks
            if (ctx->tx_window.acked[index]) {
                continue;
            }

            struct Payload chunk = {0};

            // Read chunk (seq is 1-indexed for data chunks)
            if (read_chunk_streaming(seq - 1, &chunk) != 0) {
                printf("ERROR: Failed to read chunk %u\n", seq);
                cleanup_sliding_window(&ctx->tx_window);
                cleanup_streaming_read();
                return;
            }

            cyw43_arch_poll(); // Poll network after SD read

            // Check if NACK was received (window base changed during transmission)
            if (ctx->tx_window.base != window_start_base) {
                printf("  ! NACK received during transmission - restarting from base=%u\n",
                       ctx->tx_window.base);
                nack_received = true;
                break; // Exit loop and restart window from new base
            }

            // Verify chunk
            if (!verify_chunk(&chunk)) {
                printf("ERROR: Invalid chunk %u\n", seq);
                cleanup_sliding_window(&ctx->tx_window);
                cleanup_streaming_read();
                return;
            }

            // Serialize chunk
            uint8_t payload_buffer[PAYLOAD_SIZE];
            if (serialize_payload(&chunk, payload_buffer) != PAYLOAD_SIZE) {
                printf("ERROR: Failed to serialize chunk %u\n", seq);
                continue;
            }

            // Publish to file/data topic with QoS 1
            msg_id = get_next_msg_id();
            mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port, topic_id, payload_buffer, PAYLOAD_SIZE,
                                     FILE_TRANSFER_DATA_QOS, msg_id, false);

            total_transmitted++;

            // Show progress only at window boundaries to reduce spam
            if (seq == ctx->tx_window.base || seq == window_end - 1) {
                printf("  [MQTT:file/data] Chunk %u/%u (seq=%u)\n", seq, metadata.chunk_count,
                       chunk.sequence);
            }

            // Small delay and poll network
            sleep_us(POLL_YIELD_DELAY_US);
            cyw43_arch_poll();

            // Check again after poll (control messages may have arrived)
            if (ctx->tx_window.base != window_start_base) {
                printf("  ! NACK received during transmission - restarting from base=%u\n",
                       ctx->tx_window.base);
                nack_received = true;
                break;
            }
        }

        if (!nack_received) {
            printf("  ✓ Transmitted %u chunks in window [%u-%u]\n", window_end - window_start_base,
                   window_start_base, window_end - 1);
        }

        // If NACK was received, immediately restart transmission from new base
        if (nack_received) {
            continue; // Skip waiting, go back to start of while loop with new base
        }

        // Wait for control message from receiver (REQUEST_NEXT or COMPLETE)
        // The control message handler will update ctx->tx_window accordingly
        printf("  Waiting for receiver response on file/control...\n");

        absolute_time_t wait_for_control = get_absolute_time();
        bool control_received = false;
        uint32_t saved_base = ctx->tx_window.base;

        while (absolute_time_diff_us(wait_for_control, get_absolute_time()) <
               (QOS_RETRY_INTERVAL_US * 2)) {
            cyw43_arch_poll(); // Process incoming control messages

            // Check if window slid forward (ACK received)
            if (ctx->tx_window.base != saved_base) {
                control_received = true;
                printf("  ✓ Window slid to base=%u\n", ctx->tx_window.base);
                break;
            }

            sleep_ms(10);
        }

        if (!control_received) {
            printf("  WARNING: No control message received, retrying window...\n");
            total_retransmissions += (window_end - ctx->tx_window.base);
            ctx->tx_window.retries++;

            if (ctx->tx_window.retries >= MAX_RETRIES_GBN) {
                printf("ERROR: Max retries exceeded\n");
                cleanup_sliding_window(&ctx->tx_window);
                cleanup_streaming_read();
                return;
            }
        } else {
            ctx->tx_window.retries = 0; // Reset retry counter on success
        }
    }

    absolute_time_t end_time = get_absolute_time();
    int64_t elapsed_ms = absolute_time_diff_us(start_time, end_time) / 1000;

    cleanup_sliding_window(&ctx->tx_window);
    cleanup_streaming_read();

    printf("\n=== GO-BACK-N TRANSMISSION COMPLETE ===\n");
    printf("  Total chunks: %u\n", metadata.chunk_count);
    printf("  Chunks transmitted: %u\n", total_transmitted);
    printf("  Retransmissions: %u (%.1f%%)\n", total_retransmissions,
           total_transmitted > 0 ? (float)total_retransmissions * 100.0 / total_transmitted : 0.0);
    printf("  Total bytes: %u\n", metadata.total_size);
    printf("  Time: %lld ms\n", elapsed_ms);
    printf("  Throughput: %.2f KB/s\n",
           elapsed_ms > 0 ? (float)metadata.total_size / (elapsed_ms / 1000.0) / 1024.0 : 0.0);
    printf("  Window size: %u chunks (%u bytes)\n", WINDOW_SIZE_CHUNKS, WINDOW_SIZE_BYTES);
    printf("========================================\n\n");
}

/**
 * @brief Automatically select file transfer method based on file size
 *
 * This function uses Go-Back-N protocol for all file transfers to ensure
 * reliable delivery with automatic error recovery and retransmission.
 *
 * @param pcb UDP PCB
 * @param gw_addr Gateway address
 * @param gw_port Gateway port
 * @param filename Name of file to send from microSD
 * @param ctx MQTT-SN context (required for Go-Back-N transfers)
 *
 * @note ctx must have file/control topic registered and subscribed
 */
void send_file_via_mqtt_auto(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port,
                             const char *filename, mqtt_sn_context_t *ctx) {
    if (!filename) {
        printf("ERROR: NULL filename\n");
        return;
    }

    // Get file size using FatFS
    FILINFO file_info;
    if (!microsd_driver_stat(filename, &file_info)) {
        printf("ERROR: Could not stat file '%s'\n", filename);
        return;
    }

    uint32_t file_size = file_info.fsize;

    printf("\n=== Auto File Transfer Selection ===\n");
    printf("File: %s\n", filename);
    printf("Size: %lu bytes (%.2f KB)\n", (unsigned long)file_size, file_size / 1024.0);
    printf("Transfer method: Go-Back-N Sliding Window Protocol\n");
    printf("Reason: Reliable transfer with automatic error recovery\n");
    printf("====================================\n");

    if (!ctx) {
        printf("ERROR: MQTT context required for Go-Back-N transfers\n");
        return;
    }

    send_file_via_mqtt_gbn(pcb, gw_addr, gw_port, filename, ctx);
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
        // Silently ignore invalid metadata (likely retransmitted old messages after transfer
        // complete)
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
    printf("  File CRC:   0x%08X\n", metadata.file_crc32);

    // Check if filesystem is valid (SD card may have been unplugged)
    // With FatFS, we just attempt to use it - FatFS will handle errors
    bool fs_valid = true;

    if (!fs_valid) {
        printf("⚠ WARNING: Filesystem not initialized - attempting to initialize SD card...\n");
        printf("(SD card may have been removed and re-inserted)\n");

        // Attempt to re-mount (handled by application layer, not here)
        printf("✗ ERROR: SD card not initialized\n");
        printf("┌─────────────────────────────────────────────────────┐\n");
        printf("│  Cannot receive file: MicroSD card not initialized  │\n");
        printf("│  Please ensure SD card is properly inserted         │\n");
        printf("└─────────────────────────────────────────────────────┘\n");

        // Send error message back to sender
        const char *error_msg =
            "ERROR: SD card not initialized. Cannot receive file. Please insert SD card.";
        mqtt_sn_publish_topic_id_auto(pcb, addr, port, TOPIC_ID_PICO_STATUS,
                                      (const uint8_t *)error_msg, strlen(error_msg), QOS_LEVEL_1);
        printf("✓ Error notification sent to sender\n");
        return;
    }

    // Initialize transfer session
    if (!ctx->file_session) {
        printf("✗ ERROR: No session buffer allocated\n");

        // Send error message back to sender
        const char *error_msg = "ERROR: Session buffer not allocated. Cannot receive file.";
        mqtt_sn_publish_topic_id_auto(pcb, addr, port, TOPIC_ID_PICO_STATUS,
                                      (const uint8_t *)error_msg, strlen(error_msg), QOS_LEVEL_1);
        printf("✓ Error notification sent to sender\n");
        return;
    }

    // Use new filename (true) to avoid overwriting existing files
    if (!chunk_transfer_init_session(&metadata, ctx->file_session, true)) {
        printf("✗ ERROR: Failed to init transfer session (SD card may be full or corrupted)\n");

        // Send error message back to sender
        const char *error_msg =
            "ERROR: Failed to initialize transfer session. SD card may be full or corrupted.";
        mqtt_sn_publish_topic_id_auto(pcb, addr, port, TOPIC_ID_PICO_STATUS,
                                      (const uint8_t *)error_msg, strlen(error_msg), QOS_LEVEL_1);
        printf("✓ Error notification sent to sender\n");
        return;
    }

    ctx->transfer_in_progress = true;
    ctx->last_acked_seq = 0; // Reset for new transfer
    strncpy(ctx->rx_session_id, metadata.session_id, sizeof(ctx->rx_session_id) - 1);
    ctx->rx_session_id[sizeof(ctx->rx_session_id) - 1] = '\0';

    printf("✓ Transfer session active - READY TO RECEIVE DATA CHUNKS\n");
    printf("  Memory allocated: ~%lu bytes (bitmap)\n",
           (unsigned long)ctx->file_session->chunk_meta.bitmap_size);
    printf("  Temp file handle: OPEN (persistent until complete)\n");
    printf("  Session ID: %s\n", ctx->rx_session_id);
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
/**
 * @brief Handle received file payload chunk (Go-Back-N receiver)
 *
 * Processes incoming file chunks with Go-Back-N flow control:
 * - Buffers chunks in memory until full window received
 * - Writes window to SD card after all chunks in window arrive
 * - Sends REQUEST_NEXT for next window or COMPLETE when done
 * - Handles missing chunks by sending NACK
 *
 * @param ctx MQTT-SN context containing session info
 * @param payload Serialized payload chunk
 * @param len Payload length
 * @param pcb UDP PCB for sending control messages
 * @param addr Gateway address
 * @param port Gateway port
 *
 * @note Sends control messages to file/control topic
 * @note Syncs SD card writes after each window
 */
void handle_file_payload(mqtt_sn_context_t *ctx, const uint8_t *payload, size_t len,
                         struct udp_pcb *pcb, const ip_addr_t *addr, u16_t port) {
    if (!ctx || !payload) {
        printf("ERROR: NULL parameter in handle_file_payload\n");
        return;
    }

    if (!ctx->transfer_in_progress) {
        printf("WARNING: Received payload but no active session\n");
        printf("  (Did you receive metadata first? Check if SD card is initialized)\n");
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

    // Write chunk to microSD (buffered)
    if (!chunk_transfer_write_payload(ctx->file_session, &chunk)) {
        printf("ERROR: Failed to write chunk %lu\n", (unsigned long)chunk.sequence);
        return;
    }

    uint32_t received, total;
    chunk_transfer_get_progress(ctx->file_session, &received, &total);

    // Update last ACKed sequence number to highest CONTIGUOUS chunk received
    // Don't just take the highest chunk number - find highest contiguous sequence
    uint32_t highest_contiguous = 0;
    for (uint32_t seq = 1; seq <= total; seq++) {
        uint32_t bitmap_index = seq / 8;
        uint32_t bit_offset = seq % 8;

        if (bitmap_index < ctx->file_session->chunk_meta.bitmap_size) {
            if (ctx->file_session->chunk_meta.chunk_bitmap[bitmap_index] & (1 << bit_offset)) {
                highest_contiguous = seq;
            } else {
                break; // Found a gap, stop here
            }
        }
    }
    ctx->last_acked_seq = highest_contiguous;

    // Show progress only at meaningful intervals
    if (received % (PROGRESS_UPDATE_INTERVAL * 5) == 0 || received == total) {
        printf("  [RECEIVER] Progress: %lu/%lu chunks (%.1f%%)\n", (unsigned long)received,
               (unsigned long)total, (received * 100.0f) / total);
    }

    // Check window completion when:
    // 1. Every 200ms (reduced from 500ms for faster gap detection)
    // 2. When we receive a duplicate (indicates retransmission)
    // 3. Every 10 chunks to catch gaps early
    absolute_time_t now = get_absolute_time();
    bool is_duplicate = (chunk.sequence <= ctx->last_acked_seq);
    int64_t time_since_last_check = absolute_time_diff_us(ctx->last_window_check, now) / 1000;

    static uint32_t chunks_since_check = 0;
    chunks_since_check++;

    bool should_check =
        is_duplicate || (time_since_last_check >= 200) || (chunks_since_check >= 10);

    if (!should_check) {
        // Don't check yet - chunks arriving normally
        return;
    }

    ctx->last_window_check = now;
    chunks_since_check = 0;

    // Calculate current window boundary
    uint32_t current_window_end =
        ((ctx->last_acked_seq / WINDOW_SIZE_CHUNKS) + 1) * WINDOW_SIZE_CHUNKS;
    if (current_window_end > total) {
        current_window_end = total;
    }

    // Check if we've completed a window or the entire transfer
    bool window_complete = false;
    bool all_chunks_in_window = true;

    uint32_t window_start = (ctx->last_acked_seq / WINDOW_SIZE_CHUNKS) * WINDOW_SIZE_CHUNKS + 1;
    if (window_start == 0)
        window_start = 1; // Skip metadata chunk (0)

    // Check if all chunks in current window have been received
    // Must check ENTIRE window, not just up to last_acked_seq
    for (uint32_t i = window_start; i <= current_window_end; i++) {
        // Check bitmap to see if chunk i has been received
        uint32_t bitmap_index = i / 8;
        uint32_t bit_offset = i % 8;

        if (bitmap_index < ctx->file_session->chunk_meta.bitmap_size) {
            if (!(ctx->file_session->chunk_meta.chunk_bitmap[bitmap_index] & (1 << bit_offset))) {
                all_chunks_in_window = false;
                // Only log first missing chunk to reduce spam
                static uint32_t last_logged_missing = 0;
                if (i != last_logged_missing) {
                    printf("  [RECEIVER] Gap detected at chunk %u in window [%u-%u]\n", i,
                           window_start, current_window_end);
                    last_logged_missing = i;
                }
                break;
            }
        }
    }

    window_complete = all_chunks_in_window;

    // Only send control messages when:
    // 1. Window is complete (send REQUEST_NEXT or COMPLETE)
    // 2. Gap detected AND we've received chunks beyond the gap indicating sender moved on
    // Don't spam NACKs while chunks are still arriving in order!

    bool should_send_control = false;

    if (window_complete || chunk_transfer_is_complete(ctx->file_session)) {
        should_send_control = true;
    } else if (!all_chunks_in_window) {
        // There's a gap. Check if we should send NACK
        uint32_t first_missing = 0;
        uint32_t last_received = 0;

        for (uint32_t i = window_start; i <= current_window_end; i++) {
            uint32_t bitmap_index = i / 8;
            uint32_t bit_offset = i % 8;

            if (bitmap_index < ctx->file_session->chunk_meta.bitmap_size) {
                if (ctx->file_session->chunk_meta.chunk_bitmap[bitmap_index] & (1 << bit_offset)) {
                    last_received = i;
                } else if (first_missing == 0) {
                    first_missing = i;
                }
            }
        }

        // Send NACK if:
        // - We've received at least 8 chunks after the gap (reduced from 20), OR
        // - We've received chunks at/near the end of the window (indicating sender finished)
        if (first_missing > 0) {
            uint32_t chunks_past_gap =
                (last_received > first_missing) ? (last_received - first_missing) : 0;
            bool near_window_end = (last_received >= current_window_end - 10);

            if (chunks_past_gap >= 8 || near_window_end) {
                should_send_control = true;
            }
        }
    }

    if (!should_send_control) {
        // Chunks still arriving, don't send control messages yet
        return;
    }

    if (window_complete || chunk_transfer_is_complete(ctx->file_session)) {
        printf("\n  [RECEIVER] Window [%u-%u] complete\n", window_start, current_window_end);

        // Sync window to SD card
        printf("  [RECEIVER] Writing window to SD card...\n");
        if (!chunk_transfer_sync_window(ctx->file_session)) {
            printf("ERROR: Failed to sync window to SD card\n");
            return;
        }
        printf("  [RECEIVER] ✓ Window synced to SD card\n");

        // Check if transfer complete
        if (chunk_transfer_is_complete(ctx->file_session)) {
            printf("\n=== File Transfer Complete ===\n");
            printf("All chunks received! Finalizing...\n");

            if (chunk_transfer_finalize(ctx->file_session)) {
                printf("✓ File saved: %s\n", ctx->file_session->filename);
                printf("  Size: %lu bytes\n",
                       (unsigned long)ctx->file_session->metadata.total_size);
                printf("  Temp file closed and renamed\n");
                printf("  Bitmap freed: %lu bytes\n",
                       (unsigned long)ctx->file_session->chunk_meta.bitmap_size);
                printf("✓ File is ready on SD card\n");

                // Send COMPLETE control message
                control_message_t ctrl_msg = {0};
                ctrl_msg.type = CTRL_COMPLETE;
                ctrl_msg.seq_num = total;
                strncpy(ctrl_msg.session_id, ctx->rx_session_id, sizeof(ctrl_msg.session_id) - 1);

                send_control_message(pcb, addr, port, &ctrl_msg, ctx);

            } else {
                printf("ERROR: Failed to finalize transfer\n");
            }

            ctx->transfer_in_progress = false;
            ctx->last_acked_seq = 0;
            printf("==============================\n\n");
        } else {
            // Request next window
            uint32_t next_window_start = current_window_end + 1;
            uint32_t next_window_end = next_window_start + WINDOW_SIZE_CHUNKS - 1;
            if (next_window_end > total) {
                next_window_end = total;
            }

            control_message_t ctrl_msg = {0};
            ctrl_msg.type = CTRL_REQUEST_NEXT;
            ctrl_msg.seq_num = ctx->last_acked_seq;
            ctrl_msg.window_start = next_window_start;
            ctrl_msg.window_end = next_window_end;
            strncpy(ctrl_msg.session_id, ctx->rx_session_id, sizeof(ctrl_msg.session_id) - 1);

            send_control_message(pcb, addr, port, &ctrl_msg, ctx);
        }
    } else if (!all_chunks_in_window) {
        // Missing chunks detected - send NACK
        printf("  [RECEIVER] Window incomplete - waiting for missing chunks\n");

        // Find first missing chunk
        uint32_t first_missing = 0;
        for (uint32_t i = window_start; i <= current_window_end; i++) {
            uint32_t bitmap_index = i / 8;
            uint32_t bit_offset = i % 8;

            if (bitmap_index < ctx->file_session->chunk_meta.bitmap_size) {
                if (!(ctx->file_session->chunk_meta.chunk_bitmap[bitmap_index] &
                      (1 << bit_offset))) {
                    first_missing = i;
                    break;
                }
            }
        }

        if (first_missing > 0) {
            control_message_t ctrl_msg = {0};
            ctrl_msg.type = CTRL_NACK;
            ctrl_msg.seq_num = first_missing - 1; // Last successfully received before gap
            ctrl_msg.window_start = window_start;
            ctrl_msg.window_end = current_window_end;
            strncpy(ctrl_msg.session_id, ctx->rx_session_id, sizeof(ctrl_msg.session_id) - 1);

            send_control_message(pcb, addr, port, &ctrl_msg, ctx);
        }
    }
}

// ============================================================================
// Control Message Handler (Sender side - processes ACK/NACK/REQUEST_NEXT)
// ============================================================================

/**
 * @brief Handle control message from receiver (for sender TX side)
 *
 * Processes control messages on file/control topic:
 * - CTRL_ACK: Mark chunks as acknowledged, slide window
 * - CTRL_NACK: Retransmit from specified sequence number
 * - CTRL_REQUEST_NEXT: Slide window to next batch
 * - CTRL_COMPLETE: Transfer complete confirmation
 *
 * @param ctx MQTT-SN context (sender side with tx_window)
 * @param payload Serialized control message
 * @param len Payload length
 * @param pcb UDP PCB
 * @param addr Gateway address
 * @param port Gateway port
 */
void handle_control_message(mqtt_sn_context_t *ctx, const uint8_t *payload, size_t len,
                            struct udp_pcb *pcb, const ip_addr_t *addr, u16_t port) {
    if (!ctx || !payload) {
        printf("ERROR: NULL parameter in handle_control_message\n");
        return;
    }

    if (len != sizeof(control_message_t)) {
        printf("ERROR: Invalid control message size (expected %zu, got %zu)\n",
               sizeof(control_message_t), len);
        return;
    }

    control_message_t ctrl_msg = {0};
    memcpy(&ctrl_msg, payload, sizeof(control_message_t));

    // Ignore control messages if no active Go-Back-N window
    // This prevents normal streaming transfers from being affected by control messages
    if (!ctx->tx_window.active) {
        printf("  [CONTROL] Ignoring control message (no active Go-Back-N session)\n");
        return;
    }

    // Verify session ID matches
    if (strncmp(ctrl_msg.session_id, ctx->tx_window.session_id, sizeof(ctrl_msg.session_id)) != 0) {
        printf("WARNING: Control message session ID mismatch\n");
        return;
    }

    switch (ctrl_msg.type) {
    case CTRL_ACK:
        printf("  [CONTROL] Received ACK up to chunk %u\n", ctrl_msg.seq_num);
        // Mark chunks as ACKed and slide window
        for (uint32_t seq = ctx->tx_window.base; seq <= ctrl_msg.seq_num; seq++) {
            process_ack(&ctx->tx_window, seq);
        }
        break;

    case CTRL_NACK:
        printf("  [CONTROL] Received NACK - retransmit from chunk %u\n", ctrl_msg.seq_num + 1);
        // Reset window to retransmit from seq_num + 1
        // Mark all chunks up to seq_num as ACKed
        for (uint32_t seq = ctx->tx_window.base; seq <= ctrl_msg.seq_num; seq++) {
            process_ack(&ctx->tx_window, seq);
        }
        break;

    case CTRL_REQUEST_NEXT:
        printf("  [CONTROL] Received REQUEST_NEXT for window [%u-%u]\n", ctrl_msg.window_start,
               ctrl_msg.window_end);
        // Mark all chunks in current window as ACKed and slide to next window
        for (uint32_t seq = ctx->tx_window.base; seq < ctrl_msg.window_start; seq++) {
            process_ack(&ctx->tx_window, seq);
        }
        break;

    case CTRL_COMPLETE:
        printf("  [CONTROL] Received TRANSFER_COMPLETE\n");
        // Mark all remaining chunks as ACKed
        for (uint32_t seq = ctx->tx_window.base; seq <= ctx->tx_window.total_chunks; seq++) {
            process_ack(&ctx->tx_window, seq);
        }
        ctx->tx_window.active = false;
        break;

    default:
        printf("WARNING: Unknown control message type: %d\n", ctrl_msg.type);
        break;
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
 * @brief Invalidate all registered/subscribed topics in the MQTT-SN context
 * @param[in] ctx Pointer to the MQTT-SN client context
 */
void mqtt_sn_invalidate_all_topics(mqtt_sn_context_t *ctx) {
    if (!ctx)
        return;
    for (size_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
        if (ctx->custom_topics[i].in_use) {
            ctx->custom_topics[i].is_registered = false;
            ctx->custom_topics[i].topic_id = 0;
            ctx->custom_topics[i].last_attempt = nil_time;
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

// ============================================================================
// Message Handler Implementations
// ============================================================================

static void handle_pingresp(mqtt_sn_context_t *ctx, struct udp_pcb *pcb, const uint8_t *data,
                            uint8_t length, const ip_addr_t *addr, u16_t port) {
    (void)pcb;
    (void)data;
    (void)length;
    (void)addr;
    (void)port;
    (void)ctx;
    g_last_pingresp = to_ms_since_boot(get_absolute_time());
    g_ping_ack_received = true;
    printf("Received PINGRESP\\n");
}

static void handle_connack(mqtt_sn_context_t *ctx, struct udp_pcb *pcb, const uint8_t *data,
                           uint8_t length, const ip_addr_t *addr, u16_t port) {
    (void)pcb;
    (void)length;
    (void)addr;
    (void)port;
    uint8_t return_code = data[MQTTSN_OFFSET_FLAGS];
    printf("CONNACK: return_code=%d (%s)\\n", return_code,
           return_code == MQTTSN_RETURN_ACCEPTED ? "Accepted" : "Rejected");
    g_ping_ack_received = true;

    // Clear topic list on reconnection to reset topic IDs
    if (ctx && return_code == MQTTSN_RETURN_ACCEPTED) {
        memset(ctx->custom_topics, 0, sizeof(ctx->custom_topics));
    }
}

static void handle_suback(mqtt_sn_context_t *ctx, struct udp_pcb *pcb, const uint8_t *data,
                          uint8_t length, const ip_addr_t *addr, u16_t port) {
    (void)pcb;
    (void)length;
    (void)addr;
    (void)port;
    uint8_t flags = data[MQTTSN_OFFSET_FLAGS];
    uint16_t topic_id =
        (data[MQTTSN_OFFSET_PROTOCOL_ID] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_DURATION_HIGH];
    uint16_t msg_id =
        (data[MQTTSN_OFFSET_DURATION_LOW] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_CLIENT_ID];
    uint8_t return_code = data[MQTTSN_OFFSET_PAYLOAD];
    printf("SUBACK: topic_id=%d, msg_id=%d, return_code=%d\\n", topic_id, msg_id, return_code);

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
                    printf("\u2713 Topic '%s' subscribed successfully (ID %u)\n",
                           ctx->custom_topics[i].topic_name, topic_id);
                    break;
                }
            }
        }
    }
}

static void handle_regack(mqtt_sn_context_t *ctx, struct udp_pcb *pcb, const uint8_t *data,
                          uint8_t length, const ip_addr_t *addr, u16_t port) {
    (void)pcb;
    (void)addr;
    (void)port;
    if (length < 7)
        return; // REGACK packet: [len][type][topic_id(2)][msg_id(2)][return_code]

    uint16_t topic_id = (data[2] << BITS_PER_BYTE) | data[3];
    uint16_t msg_id = (data[4] << BITS_PER_BYTE) | data[5];
    uint8_t return_code = data[6];

    if (return_code == MQTTSN_RETURN_ACCEPTED) {
        printf("\u2713 REGACK: Topic registered successfully with ID %u (msg_id=%u)\n", topic_id,
               msg_id);

        // Mark custom topic as registered
        if (ctx) {
            for (size_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
                if (ctx->custom_topics[i].in_use && ctx->custom_topics[i].is_sender &&
                    !ctx->custom_topics[i].is_registered) {
                    // Match by last attempt timing (within last 2 seconds)
                    absolute_time_t now = get_absolute_time();
                    int64_t elapsed_ms =
                        absolute_time_diff_us(ctx->custom_topics[i].last_attempt, now) / 1000;
                    if (elapsed_ms < 2000) {
                        ctx->custom_topics[i].is_registered = true;
                        ctx->custom_topics[i].topic_id = topic_id;
                        printf("\u2713 Topic '%s' registered with ID %u (ready to publish)\n",
                               ctx->custom_topics[i].topic_name, topic_id);
                        break;
                    }
                }
            }
        }
    } else {
        printf("\u2717 REGACK: Registration failed, return_code=%u (msg_id=%u)\n", return_code,
               msg_id);
    }
}

static void send_qos_ack(struct udp_pcb *pcb, const ip_addr_t *addr, u16_t port, uint8_t qos,
                         uint16_t topic_id, uint16_t msg_id) {
    switch (qos) {
    case QOS_LEVEL_1:
        mqtt_sn_send_puback(pcb, addr, port, topic_id, msg_id, MQTTSN_RETURN_ACCEPTED);
        break;
    case QOS_LEVEL_2:
        mqtt_sn_send_pubrec(pcb, addr, port, msg_id);
        break;
    default:
        break; // QoS 0 needs no ACK
    }
}

static void handle_publish_received(mqtt_sn_context_t *ctx, struct udp_pcb *pcb,
                                    const uint8_t *data, uint8_t length, const ip_addr_t *addr,
                                    u16_t port) {
    if (length < MQTTSN_PUBLISH_HEADER_LEN)
        return;

    uint8_t flags = data[MQTTSN_OFFSET_FLAGS];
    uint8_t qos = (flags >> MQTTSN_QOS_SHIFT) & MQTTSN_FLAG_QOS_MASK;
    uint16_t topic_id =
        (data[MQTTSN_OFFSET_TOPIC_ID_HIGH] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_TOPIC_ID_LOW];
    uint16_t msg_id =
        (data[MQTTSN_OFFSET_MSG_ID_HIGH] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_MSG_ID_LOW];
    int payload_len = length - MQTTSN_PUBLISH_HEADER_LEN;
    const uint8_t *payload = &data[MQTTSN_OFFSET_PAYLOAD];

    // Lookup dynamic topic IDs
    uint16_t file_data_topic_id = mqtt_sn_get_topic_id(ctx, "file/data");
    uint16_t file_control_topic_id = mqtt_sn_get_topic_id(ctx, "file/control");

    // Check for file transfer topics first
    if (topic_id == file_data_topic_id && file_data_topic_id != 0) {
        // file/data topic - can be either metadata or data chunks
        // Distinguish by QoS level AND transfer state:
        // - QoS 2 = metadata (exactly-once delivery)
        // - QoS 1 = data chunks (at-least-once delivery)
        if (qos == QOS_LEVEL_2 || !ctx->transfer_in_progress) {
            printf("PUBLISH: File metadata received on file/data (QoS %d, Msg ID %d)\n", qos,
                   msg_id);
            handle_file_metadata(ctx, payload, payload_len, pcb, addr, port);
        } else {
            handle_file_payload(ctx, payload, payload_len, pcb, addr, port);
        }
        send_qos_ack(pcb, addr, port, qos, topic_id, msg_id);
    } else if (topic_id == file_control_topic_id && file_control_topic_id != 0) {
        // file/control topic - for Go-Back-N flow control (QoS 1)
        printf("PUBLISH: Control message received (QoS %d, Msg ID %d, Len %d)\n", qos, msg_id,
               payload_len);
        handle_control_message(ctx, payload, payload_len, pcb, addr, port);
        send_qos_ack(pcb, addr, port, qos, topic_id, msg_id);
    } else {
        // Regular message handling
        printf("PUBLISH received (QoS %d, Msg ID %d), Payload (%d bytes)\n", qos, msg_id,
               payload_len);

        // Handle text commands embedded in binary
        if (payload_len == LED_ON_CMD_LEN && memcmp(payload, "led on", LED_ON_CMD_LEN) == 0) {
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1U);
        } else if (payload_len == LED_OFF_CMD_LEN &&
                   memcmp(payload, "led off", LED_OFF_CMD_LEN) == 0) {
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0U);
        }

        send_qos_ack(pcb, addr, port, qos, topic_id, msg_id);
    }
}

static void handle_puback_received(mqtt_sn_context_t *ctx, struct udp_pcb *pcb, const uint8_t *data,
                                   uint8_t length, const ip_addr_t *addr, u16_t port) {
    (void)ctx;
    (void)pcb;
    (void)length;
    (void)addr;
    (void)port;
    uint16_t msg_id =
        (data[MQTTSN_OFFSET_DURATION_HIGH] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_DURATION_LOW];
    printf("PUBACK received for Msg ID: %d \n", msg_id);
    remove_pending_qos_msg(msg_id);
}

static void handle_pubcomp_received(mqtt_sn_context_t *ctx, struct udp_pcb *pcb,
                                    const uint8_t *data, uint8_t length, const ip_addr_t *addr,
                                    u16_t port) {
    (void)ctx;
    (void)pcb;
    (void)length;
    (void)addr;
    (void)port;
    uint16_t msg_id =
        (data[MQTTSN_OFFSET_FLAGS] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_PROTOCOL_ID];
    printf("PUBCOMP received for Msg ID: %d\n", msg_id);
    remove_pending_qos_msg(msg_id);
}

static void handle_pubrec_received(mqtt_sn_context_t *ctx, struct udp_pcb *pcb, const uint8_t *data,
                                   uint8_t length, const ip_addr_t *addr, u16_t port) {
    (void)ctx;
    (void)length;
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

static void handle_pubrel_received(mqtt_sn_context_t *ctx, struct udp_pcb *pcb, const uint8_t *data,
                                   uint8_t length, const ip_addr_t *addr, u16_t port) {
    (void)ctx;
    (void)length;
    uint16_t msg_id =
        (data[MQTTSN_OFFSET_FLAGS] << BITS_PER_BYTE) | data[MQTTSN_OFFSET_PROTOCOL_ID];
    printf("PUBREL received for Msg ID: %d. Sending PUBCOMP...\n", msg_id);
    mqtt_sn_send_pubcomp(pcb, addr, port, msg_id);
}
