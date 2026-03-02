/*!
 * @file  mqtt-sn-udp.h
 * @brief MQTT-SN for ATmega328P (2KB RAM) — AVR-constrained port
 *
 * Key reductions vs Pico version:
 *   payload buffer : 247 -> 32 bytes  (button payloads are ~15 bytes)
 *   QoS slots      : 8   -> 3
 *   topic slots    : 10  -> 4
 *   topic_name     : 64  -> 32 chars
 *   All string literals in PROGMEM via F()
 */
#ifndef MQTT_SN_UDP_H
#define MQTT_SN_UDP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ── Arduino-specific config ─────────────────────────────────── */
#define LORA_MY_NODE_ID   0x21        /* unique per node */
#define MQTT_SN_CLIENT_ID "arduino-02" /* short = less flash */
#define LORA_GW_NODE_ID   0x00        /* RPi bridge */
#define KEEPALIVE_INTERVAL_SEC 60U
#define LED_PIN LED_BUILTIN

/* ── Memory budget constants ─────────────────────────────────── */
#define MAX_PENDING_QOS_MSGS 3U       /* was 8  — 3 in-flight is plenty  */
#define MAX_CUSTOM_TOPICS 4U          /* was 10 — we register 2 topics    */
#define MQTTSN_RETRY_PAYLOAD_SIZE 32U /* was 247 — button payload ~15B    */
#define TOPIC_NAME_MAX_LEN 32U        /* was 64                           */

/* ── Packet constants (unchanged from Pico) ─────────────────── */
#define MQTTSN_HEADER_SIZE 2U
#define MQTTSN_CONNECT_FIXED_LEN 6U
#define MQTTSN_SUBSCRIBE_LEN 7U
#define MQTTSN_PUBACK_LEN 7U
#define MQTTSN_PUBREC_LEN 5U
#define MQTTSN_PUBCOMP_LEN 5U
#define MQTTSN_PUBREL_LEN 4U
#define MQTTSN_PINGREQ_LEN 2U
#define MQTTSN_MAX_PACKET_LEN 255U
#define MQTTSN_PUBLISH_HEADER_LEN 7U

/* ── Field offsets (unchanged) ───────────────────────────────── */
#define MQTTSN_OFFSET_LENGTH 0U
#define MQTTSN_OFFSET_MSG_TYPE 1U
#define MQTTSN_OFFSET_FLAGS 2U
#define MQTTSN_OFFSET_PROTOCOL_ID 3U
#define MQTTSN_OFFSET_DURATION_HIGH 4U
#define MQTTSN_OFFSET_DURATION_LOW 5U
#define MQTTSN_OFFSET_CLIENT_ID 6U
#define MQTTSN_OFFSET_TOPIC_ID_HIGH 3U
#define MQTTSN_OFFSET_TOPIC_ID_LOW 4U
#define MQTTSN_OFFSET_MSG_ID_HIGH 5U
#define MQTTSN_OFFSET_MSG_ID_LOW 6U
#define MQTTSN_OFFSET_PAYLOAD 7U
#define MQTTSN_OFFSET_RETURN_CODE 2U

/* ── QoS levels (unchanged) ──────────────────────────────────── */
#define QOS_LEVEL_0 0
#define QOS_LEVEL_1 1
#define QOS_LEVEL_2 2

/* ── Protocol constants (unchanged) ─────────────────────────── */
#define BITS_PER_BYTE 8U
#define MQTTSN_BYTE_MASK 0xFFU
#define MQTTSN_MSG_TYPE_CONNECT 0x04U
#define MQTTSN_MSG_TYPE_CONNACK 0x05U
#define MQTTSN_MSG_TYPE_REGISTER 0x0AU
#define MQTTSN_MSG_TYPE_REGACK 0x0BU
#define MQTTSN_MSG_TYPE_PUBLISH 0x0CU
#define MQTTSN_MSG_TYPE_PUBACK 0x0DU
#define MQTTSN_MSG_TYPE_PUBCOMP 0x0EU
#define MQTTSN_MSG_TYPE_PUBREC 0x0FU
#define MQTTSN_MSG_TYPE_PUBREL 0x10U
#define MQTTSN_MSG_TYPE_SUBSCRIBE 0x12U
#define MQTTSN_MSG_TYPE_SUBACK 0x13U
#define MQTTSN_MSG_TYPE_PINGREQ 0x16U
#define MQTTSN_MSG_TYPE_PINGRESP 0x17U
#define MQTTSN_FLAG_CLEAN_SESSION 0x04U
#define MQTTSN_PROTOCOL_ID 0x01U
#define MQTTSN_FLAG_TOPIC_NORMAL 0x00U
#define MQTTSN_FLAG_TOPIC_PREDEFINED 0x01U
#define MQTTSN_FLAG_QOS1 0x20U
#define MQTTSN_FLAG_QOS2 0x40U
#define MQTTSN_FLAG_QOS_MASK 0x03U
#define MQTTSN_QOS_SHIFT 5U
#define MQTTSN_RETURN_ACCEPTED 0x00U
#define MQTTSN_SUBSCRIBE_FLAGS_QOS2 0x41U
#define LED_ON_CMD_LEN 6U
#define LED_OFF_CMD_LEN 7U

/* ── Timing (unchanged) ──────────────────────────────────────── */
#define TOPIC_RETRY_INTERVAL_MS 5000UL
#define QOS_RETRY_INTERVAL_MS 3000UL
#define QOS_MAX_RETRIES 3U

/* ── Topic names ─────────────────────────────────────────────── */
#define TOPIC_DATA_1 "sensors/data"
#define TOPIC_DATA_2 "sensors/arduino/data"
#define TOPIC_CMD_1 "sensors/cmd"
#define TOPIC_CMD_2 "sensors/arduino/cmd"

/* ────────────────────────────────────────────────────────────────
 * QoS pending message
 * RAM: 3 slots × (2+1+1+4+1+2+32+1) = 3 × 44 = 132 bytes
 * Was: 8 × 276 = 2,208 bytes on Pico
 * ──────────────────────────────────────────────────────────────*/
typedef struct {
    uint16_t msg_id;
    uint8_t qos;
    uint8_t step; /* 0=PUBLISH sent, 1=PUBREL sent */
    uint32_t timestamp_ms;
    uint8_t retry_count;
    uint16_t topic_id;
    uint8_t payload[MQTTSN_RETRY_PAYLOAD_SIZE]; /* 32 bytes */
    uint8_t payload_len;                        /* uint8_t — max 32, no need for size_t */
    bool in_use;
} qos_msg_t;

/* ────────────────────────────────────────────────────────────────
 * Topic entry
 * RAM: 4 slots × (32+2+1+1+1+4+1) = 4 × 42 = 168 bytes
 * Was: 10 × 80 = 800 bytes on Pico
 * ──────────────────────────────────────────────────────────────*/
typedef struct {
    char topic_name[TOPIC_NAME_MAX_LEN];
    uint16_t topic_id;
    uint16_t pending_msg_id;
    uint8_t qos;
    bool is_registered;
    bool is_sender;
    uint32_t last_attempt_ms;
    bool in_use;
} topic_entry_t;

/* ────────────────────────────────────────────────────────────────
 * Context
 * RAM: 4 × 42 + 2 flags = ~170 bytes
 * Was: ~840 bytes on Pico
 * ──────────────────────────────────────────────────────────────*/
typedef struct {
    bool drop_acks; /* test hook — keep for API compat */
    topic_entry_t custom_topics[MAX_CUSTOM_TOPICS];
} mqtt_sn_context_t;

/* ── Globals ─────────────────────────────────────────────────── */
extern qos_msg_t g_pending_msgs[MAX_PENDING_QOS_MSGS];
extern bool g_ping_ack_received;
extern uint32_t g_last_pingresp;

/* ── Public API (identical signatures to previous port) ─────── */
void mqtt_sn_connect(void);
void mqtt_sn_pingreq(void);
void mqtt_sn_register_topic(const char *topic_name, uint16_t msg_id);
void mqtt_sn_subscribe_topic_name(const char *topic_name, uint16_t msg_id, uint8_t qos);
void mqtt_sn_subscribe_topic_id(uint16_t topic_id);
void mqtt_sn_publish_topic_id_auto(uint16_t topic_id, const uint8_t *payload, size_t payload_len,
                                   int qos);
void mqtt_sn_publish_topic_id(uint16_t topic_id, const uint8_t *payload, size_t payload_len,
                              int qos, uint16_t msg_id, bool is_retransmit);
void mqtt_sn_send_puback(uint16_t topic_id, uint16_t msg_id, uint8_t return_code);
void mqtt_sn_send_pubrec(uint16_t msg_id);
void mqtt_sn_send_pubcomp(uint16_t msg_id);
void mqtt_sn_send_pubrel(uint16_t msg_id);
void check_qos_timeouts(void);
void remove_pending_qos_msg(uint16_t msg_id);
uint16_t get_next_msg_id(void);

bool mqtt_sn_add_topic_for_registration(mqtt_sn_context_t *ctx, const char *topic_name);
bool mqtt_sn_add_topic_for_subscription(mqtt_sn_context_t *ctx, const char *topic_name,
                                        uint8_t qos);
void mqtt_sn_process_topic_registrations(mqtt_sn_context_t *ctx);
void mqtt_sn_invalidate_all_topics(mqtt_sn_context_t *ctx);
uint16_t mqtt_sn_get_topic_id(mqtt_sn_context_t *ctx, const char *topic_name);
void mqtt_sn_poll(mqtt_sn_context_t *ctx);
void udp_recv_callback_arduino(mqtt_sn_context_t *ctx, const uint8_t *data, uint8_t length);

#endif /* MQTT_SN_UDP_H */