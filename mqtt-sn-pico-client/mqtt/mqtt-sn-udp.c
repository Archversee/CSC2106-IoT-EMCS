// mqtt-sn-udp.c
#include "mqtt-sn-udp.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../config.h"
#include "../mqtt-client.h"
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
    static bool initialized = false;

    // Initialize only once
    if (!initialized) {
        memset(retry_list, 0, sizeof(retry_list));
        initialized = true;
    }

    size_t retry_count = 0;
    absolute_time_t now = get_absolute_time();

    // Reset valid flags at start of each call
    for (size_t i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
        retry_list[i].valid = false;
    }

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
    printf("Received PINGRESP\n");
}

static void handle_connack(mqtt_sn_context_t *ctx, struct udp_pcb *pcb, const uint8_t *data,
                           uint8_t length, const ip_addr_t *addr, u16_t port) {
    (void)pcb;
    (void)length;
    (void)addr;
    (void)port;
    uint8_t return_code = data[MQTTSN_OFFSET_FLAGS];
    printf("CONNACK: return_code=%d (%s)\n", return_code,
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
    printf("SUBACK: topic_id=%d, msg_id=%d, return_code=%d\n", topic_id, msg_id, return_code);

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

    // Regular message handling
    printf("PUBLISH received (QoS %d, Msg ID %d), Payload (%d bytes)\n", qos, msg_id, payload_len);

    // Handle text commands embedded in binary
    if (payload_len == LED_ON_CMD_LEN && memcmp(payload, "led on", LED_ON_CMD_LEN) == 0) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1U);
    } else if (payload_len == LED_OFF_CMD_LEN && memcmp(payload, "led off", LED_OFF_CMD_LEN) == 0) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0U);
    }

    send_qos_ack(pcb, addr, port, qos, topic_id, msg_id);
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
