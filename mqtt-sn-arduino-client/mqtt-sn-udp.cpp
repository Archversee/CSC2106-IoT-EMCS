/*!
 * @file  mqtt-sn-udp.cpp
 * @brief MQTT-SN for ATmega328P — RAM-optimised
 *
 * AVR changes vs previous port:
 *   - All string literals moved to PROGMEM via F() macro
 *   - payload_len is uint8_t not size_t
 *   - Structs sized for 2KB RAM (see .h)
 *   - No static publish_count (saves 4 bytes)
 *   - strncmp on first TOPIC_NAME_MAX_LEN-1 chars only
 */

#include "mqtt-sn-udp.h"
#include "mqttsn_transport.h"
#include <Arduino.h>
#include <string.h>

/* ── Globals ─────────────────────────────────────────────────── */
qos_msg_t g_pending_msgs[MAX_PENDING_QOS_MSGS];
bool g_ping_ack_received = false;
uint32_t g_last_pingresp = 0;

static uint16_t s_next_msg_id = 1U;

/* ── Helpers ─────────────────────────────────────────────────── */
uint16_t get_next_msg_id(void) {
    if (s_next_msg_id == 0U || s_next_msg_id == 0xFFFFU)
        s_next_msg_id = 1U;
    return s_next_msg_id++;
}

static inline uint8_t get_qos_flags(int qos, uint8_t base) {
    if (qos == QOS_LEVEL_1)
        return base | MQTTSN_FLAG_QOS1;
    if (qos == QOS_LEVEL_2)
        return base | MQTTSN_FLAG_QOS2;
    return base;
}

/* ════════════════════════════════════════════════════════════════
 * Packet senders — stack buffers, F() for all string literals
 * ════════════════════════════════════════════════════════════════ */

void mqtt_sn_connect(void) {
    /* client ID kept short to save flash + RAM */
    const char *cid = MQTT_SN_CLIENT_ID;
    uint8_t id_len = (uint8_t)strlen(cid);
    uint8_t plen = MQTTSN_CONNECT_FIXED_LEN + id_len;
    uint8_t buf[MQTTSN_CONNECT_FIXED_LEN + sizeof(MQTT_SN_CLIENT_ID)];

    buf[0] = plen;
    buf[1] = MQTTSN_MSG_TYPE_CONNECT;
    buf[2] = MQTTSN_FLAG_CLEAN_SESSION;
    buf[3] = MQTTSN_PROTOCOL_ID;
    buf[4] = (KEEPALIVE_INTERVAL_SEC >> 8) & 0xFF;
    buf[5] = KEEPALIVE_INTERVAL_SEC & 0xFF;
    memcpy(buf + 6, cid, id_len);

    mqttsn_transport_send(buf, plen);
    Serial.println(F("Sent CONNECT"));
}

void mqtt_sn_pingreq(void) {
    uint8_t buf[2] = {MQTTSN_PINGREQ_LEN, MQTTSN_MSG_TYPE_PINGREQ};
    mqttsn_transport_send(buf, 2);
    Serial.println(F("Sent PINGREQ"));
}

void mqtt_sn_register_topic(const char *topic_name, uint16_t msg_id) {
    if (!topic_name)
        return;
    uint8_t tlen = (uint8_t)strlen(topic_name);
    /* stack buffer sized to worst-case topic length */
    uint8_t buf[6 + TOPIC_NAME_MAX_LEN];
    uint8_t plen = 6 + tlen;

    buf[0] = plen;
    buf[1] = MQTTSN_MSG_TYPE_REGISTER;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = (msg_id >> 8) & 0xFF;
    buf[5] = msg_id & 0xFF;
    memcpy(buf + 6, topic_name, tlen);

    mqttsn_transport_send(buf, plen);
    Serial.print(F("Sent REGISTER: "));
    Serial.println(topic_name);

    delay(15);
}

void mqtt_sn_subscribe_topic_name(const char *topic_name, uint16_t msg_id, uint8_t qos) {
    if (!topic_name)
        return;
    Serial.print(F("Sent SUBSCRIBE: "));
    Serial.println(topic_name);
    uint8_t tlen = (uint8_t)strlen(topic_name);
    uint8_t buf[5 + TOPIC_NAME_MAX_LEN];
    uint8_t plen = 5 + tlen;

    buf[0] = plen;
    buf[1] = MQTTSN_MSG_TYPE_SUBSCRIBE;
    buf[2] = get_qos_flags(qos, 0x00);
    buf[3] = (msg_id >> 8) & 0xFF;
    buf[4] = msg_id & 0xFF;
    memcpy(buf + 5, topic_name, tlen);

    mqttsn_transport_send(buf, plen);
}

void mqtt_sn_subscribe_topic_id(uint16_t topic_id) {
    uint8_t buf[MQTTSN_SUBSCRIBE_LEN];
    buf[0] = MQTTSN_SUBSCRIBE_LEN;
    buf[1] = MQTTSN_MSG_TYPE_SUBSCRIBE;
    buf[2] = MQTTSN_SUBSCRIBE_FLAGS_QOS2;
    buf[3] = 0x00;
    buf[4] = 0x01;
    buf[5] = (topic_id >> 8) & 0xFF;
    buf[6] = topic_id & 0xFF;
    mqttsn_transport_send(buf, MQTTSN_SUBSCRIBE_LEN);
}

void mqtt_sn_publish_topic_id_auto(uint16_t topic_id, const uint8_t *payload, size_t payload_len,
                                   int qos) {
    uint16_t mid = (qos > QOS_LEVEL_0) ? get_next_msg_id() : 0;
    mqtt_sn_publish_topic_id(topic_id, payload, payload_len, qos, mid, false);
}

void mqtt_sn_publish_topic_id(uint16_t topic_id, const uint8_t *payload, size_t payload_len,
                              int qos, uint16_t msg_id, bool is_retransmit) {
    if (!payload || qos < 0 || qos > QOS_LEVEL_2)
        return;

    uint8_t plen = (uint8_t)(MQTTSN_PUBLISH_HEADER_LEN + payload_len);
    if (plen > MQTTSN_MAX_PACKET_LEN) {
        Serial.println(F("PUBLISH too long"));
        return;
    }

    /* Use a single shared send buffer on the stack — 255 bytes max but
       actual button payload is ~15 bytes so real stack use is ~22 bytes */
    uint8_t buf[MQTTSN_PUBLISH_HEADER_LEN + MQTTSN_RETRY_PAYLOAD_SIZE];

    buf[0] = plen;
    buf[1] = MQTTSN_MSG_TYPE_PUBLISH;
    buf[2] = get_qos_flags(qos, MQTTSN_FLAG_TOPIC_NORMAL);
    buf[3] = (topic_id >> 8) & 0xFF;
    buf[4] = topic_id & 0xFF;
    buf[5] = (qos > QOS_LEVEL_0) ? (msg_id >> 8) & 0xFF : 0x00;
    buf[6] = (qos > QOS_LEVEL_0) ? msg_id & 0xFF : 0x00;
    memcpy(buf + MQTTSN_PUBLISH_HEADER_LEN, payload, payload_len);

    if (mqttsn_transport_send(buf, plen) != 0) {
        Serial.println(F("PUBLISH send failed"));
        return;
    }

    if (!is_retransmit) {
        Serial.print(F("PUBLISH tid="));
        Serial.print(topic_id);
        Serial.print(F(" qos="));
        Serial.print(qos);
        Serial.print(F(" mid="));
        Serial.println(msg_id);
    }

    /* Store for QoS retry */
    if (qos > QOS_LEVEL_0 && !is_retransmit) {
        noInterrupts();
        bool stored = false;
        for (uint8_t i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
            if (!g_pending_msgs[i].in_use) {
                uint8_t copy_len = (payload_len < MQTTSN_RETRY_PAYLOAD_SIZE)
                                       ? (uint8_t)payload_len
                                       : MQTTSN_RETRY_PAYLOAD_SIZE;
                g_pending_msgs[i].in_use = true;
                g_pending_msgs[i].msg_id = msg_id;
                g_pending_msgs[i].qos = (uint8_t)qos;
                g_pending_msgs[i].step = 0;
                g_pending_msgs[i].timestamp_ms = millis();
                g_pending_msgs[i].retry_count = 0;
                g_pending_msgs[i].topic_id = topic_id;
                g_pending_msgs[i].payload_len = copy_len;
                memcpy(g_pending_msgs[i].payload, payload, copy_len);
                stored = true;
                break;
            }
        }
        interrupts();
        if (!stored)
            Serial.println(F("WARN: QoS slots full"));
    }
}

void mqtt_sn_send_puback(uint16_t topic_id, uint16_t msg_id, uint8_t return_code) {
    uint8_t buf[MQTTSN_PUBACK_LEN];
    buf[0] = MQTTSN_PUBACK_LEN;
    buf[1] = MQTTSN_MSG_TYPE_PUBACK;
    buf[2] = (topic_id >> 8) & 0xFF;
    buf[3] = topic_id & 0xFF;
    buf[4] = (msg_id >> 8) & 0xFF;
    buf[5] = msg_id & 0xFF;
    buf[6] = return_code;
    mqttsn_transport_send(buf, MQTTSN_PUBACK_LEN);
}

void mqtt_sn_send_pubrec(uint16_t msg_id) {
    uint8_t buf[MQTTSN_PUBREC_LEN];
    buf[0] = MQTTSN_PUBREC_LEN;
    buf[1] = MQTTSN_MSG_TYPE_PUBREC;
    buf[2] = (msg_id >> 8) & 0xFF;
    buf[3] = msg_id & 0xFF;
    buf[4] = MQTTSN_RETURN_ACCEPTED;
    mqttsn_transport_send(buf, MQTTSN_PUBREC_LEN);
}

void mqtt_sn_send_pubcomp(uint16_t msg_id) {
    uint8_t buf[MQTTSN_PUBCOMP_LEN];
    buf[0] = MQTTSN_PUBCOMP_LEN;
    buf[1] = MQTTSN_MSG_TYPE_PUBCOMP;
    buf[2] = (msg_id >> 8) & 0xFF;
    buf[3] = msg_id & 0xFF;
    buf[4] = MQTTSN_RETURN_ACCEPTED;
    mqttsn_transport_send(buf, MQTTSN_PUBCOMP_LEN);
}

void mqtt_sn_send_pubrel(uint16_t msg_id) {
    uint8_t buf[MQTTSN_PUBREL_LEN];
    buf[0] = MQTTSN_PUBREL_LEN;
    buf[1] = MQTTSN_MSG_TYPE_PUBREL;
    buf[2] = (msg_id >> 8) & 0xFF;
    buf[3] = msg_id & 0xFF;
    mqttsn_transport_send(buf, MQTTSN_PUBREL_LEN);
}

/* ════════════════════════════════════════════════════════════════
 * QoS timeout checker
 * ════════════════════════════════════════════════════════════════ */
void check_qos_timeouts(void) {
    uint32_t now = millis();

    for (uint8_t i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
        if (!g_pending_msgs[i].in_use)
            continue;
        if ((uint32_t)(now - g_pending_msgs[i].timestamp_ms) < QOS_RETRY_INTERVAL_MS)
            continue;

        if (g_pending_msgs[i].retry_count >= QOS_MAX_RETRIES) {
            Serial.print(F("QoS gave up mid="));
            Serial.println(g_pending_msgs[i].msg_id);
            g_pending_msgs[i].in_use = false;
            continue;
        }

        g_pending_msgs[i].retry_count++;
        g_pending_msgs[i].timestamp_ms = now;

        if (g_pending_msgs[i].qos == QOS_LEVEL_1 || g_pending_msgs[i].step == 0) {
            Serial.print(F("Retry PUBLISH mid="));
            Serial.println(g_pending_msgs[i].msg_id);
            mqtt_sn_publish_topic_id(g_pending_msgs[i].topic_id, g_pending_msgs[i].payload,
                                     g_pending_msgs[i].payload_len, g_pending_msgs[i].qos,
                                     g_pending_msgs[i].msg_id, true);
        } else {
            Serial.print(F("Retry PUBREL mid="));
            Serial.println(g_pending_msgs[i].msg_id);
            mqtt_sn_send_pubrel(g_pending_msgs[i].msg_id);
        }
    }
}

void remove_pending_qos_msg(uint16_t msg_id) {
    if (msg_id == 0U)
        return;
    for (uint8_t i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
        if (g_pending_msgs[i].in_use && g_pending_msgs[i].msg_id == msg_id)
            g_pending_msgs[i].in_use = false;
    }
}

/* ════════════════════════════════════════════════════════════════
 * Topic management
 * ════════════════════════════════════════════════════════════════ */
bool mqtt_sn_add_topic_for_registration(mqtt_sn_context_t *ctx, const char *topic_name) {
    if (!ctx || !topic_name)
        return false;
    uint8_t tlen = (uint8_t)strlen(topic_name);
    if (tlen == 0 || tlen >= TOPIC_NAME_MAX_LEN)
        return false;

    for (uint8_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
        if (ctx->custom_topics[i].in_use &&
            strncmp(ctx->custom_topics[i].topic_name, topic_name, TOPIC_NAME_MAX_LEN - 1) == 0)
            return true; /* already exists */
    }
    for (uint8_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
        if (!ctx->custom_topics[i].in_use) {
            strncpy(ctx->custom_topics[i].topic_name, topic_name, TOPIC_NAME_MAX_LEN - 1);
            ctx->custom_topics[i].topic_name[TOPIC_NAME_MAX_LEN - 1] = '\0';
            ctx->custom_topics[i].topic_id = 0;
            ctx->custom_topics[i].pending_msg_id = 0;
            ctx->custom_topics[i].qos = 0;
            ctx->custom_topics[i].is_registered = false;
            ctx->custom_topics[i].is_sender = true;
            ctx->custom_topics[i].last_attempt_ms = 0UL;
            ctx->custom_topics[i].in_use = true;
            Serial.print(F("Added TX topic: "));
            Serial.println(topic_name);
            return true;
        }
    }
    Serial.println(F("ERR: topic table full"));
    return false;
}

bool mqtt_sn_add_topic_for_subscription(mqtt_sn_context_t *ctx, const char *topic_name,
                                        uint8_t qos) {
    if (!ctx || !topic_name || qos > QOS_LEVEL_2)
        return false;
    uint8_t tlen = (uint8_t)strlen(topic_name);
    if (tlen == 0 || tlen >= TOPIC_NAME_MAX_LEN)
        return false;

    for (uint8_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
        if (ctx->custom_topics[i].in_use &&
            strncmp(ctx->custom_topics[i].topic_name, topic_name, TOPIC_NAME_MAX_LEN - 1) == 0)
            return true;
    }
    for (uint8_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
        if (!ctx->custom_topics[i].in_use) {
            strncpy(ctx->custom_topics[i].topic_name, topic_name, TOPIC_NAME_MAX_LEN - 1);
            ctx->custom_topics[i].topic_name[TOPIC_NAME_MAX_LEN - 1] = '\0';
            ctx->custom_topics[i].topic_id = 0;
            ctx->custom_topics[i].qos = qos;
            ctx->custom_topics[i].is_registered = false;
            ctx->custom_topics[i].is_sender = false;
            ctx->custom_topics[i].last_attempt_ms = 0UL;
            ctx->custom_topics[i].in_use = true;
            return true;
        }
    }
    return false;
}

void mqtt_sn_process_topic_registrations(mqtt_sn_context_t *ctx) {
    if (!ctx)
        return;
    uint32_t now = millis();

    for (uint8_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
        if (!ctx->custom_topics[i].in_use)
            continue;
        if (ctx->custom_topics[i].is_registered)
            continue;
        if (ctx->custom_topics[i].last_attempt_ms != 0UL &&
            (uint32_t)(now - ctx->custom_topics[i].last_attempt_ms) < TOPIC_RETRY_INTERVAL_MS)
            continue;

        uint16_t mid = get_next_msg_id();
        ctx->custom_topics[i].pending_msg_id = mid;
        ctx->custom_topics[i].last_attempt_ms = now;

        if (ctx->custom_topics[i].is_sender)
            mqtt_sn_register_topic(ctx->custom_topics[i].topic_name, mid);
        else
            mqtt_sn_subscribe_topic_name(ctx->custom_topics[i].topic_name, mid,
                                         ctx->custom_topics[i].qos);
    }
}

void mqtt_sn_invalidate_all_topics(mqtt_sn_context_t *ctx) {
    if (!ctx)
        return;
    for (uint8_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
        if (ctx->custom_topics[i].in_use) {
            ctx->custom_topics[i].is_registered = false;
            ctx->custom_topics[i].topic_id = 0;
            ctx->custom_topics[i].last_attempt_ms = 0UL;
        }
    }
}

uint16_t mqtt_sn_get_topic_id(mqtt_sn_context_t *ctx, const char *topic_name) {
    if (!ctx || !topic_name)
        return 0;
    for (uint8_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
        if (ctx->custom_topics[i].in_use && ctx->custom_topics[i].is_registered &&
            strncmp(ctx->custom_topics[i].topic_name, topic_name, TOPIC_NAME_MAX_LEN - 1) == 0)
            return ctx->custom_topics[i].topic_id;
    }
    return 0;
}

/* ════════════════════════════════════════════════════════════════
 * Message handlers (private)
 * ════════════════════════════════════════════════════════════════ */
static void handle_connack(mqtt_sn_context_t *ctx, const uint8_t *d, uint8_t len) {
    (void)len;
    uint8_t rc = d[MQTTSN_OFFSET_FLAGS];
    Serial.print(F("CONNACK rc="));
    Serial.println(rc);
    g_ping_ack_received = true;
    if (ctx && rc == MQTTSN_RETURN_ACCEPTED)
        memset(ctx->custom_topics, 0, sizeof(ctx->custom_topics));
}

static void handle_regack(mqtt_sn_context_t *ctx, const uint8_t *d, uint8_t len) {
    if (len < 7)
        return;
    uint16_t tid = ((uint16_t)d[2] << 8) | d[3];
    uint16_t mid = ((uint16_t)d[4] << 8) | d[5]; // ← The msg_id from REGACK
    uint8_t rc = d[6];

    Serial.print(F("REGACK tid="));
    Serial.print(tid);
    Serial.print(F(" mid="));
    Serial.print(mid);
    Serial.print(F(" rc="));
    Serial.println(rc);

    if (rc != MQTTSN_RETURN_ACCEPTED || !ctx)
        return;

    // ✅ Match by pending_msg_id, not timestamp
    for (uint8_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
        if (!ctx->custom_topics[i].in_use)
            continue;
        if (!ctx->custom_topics[i].is_sender)
            continue;
        if (ctx->custom_topics[i].is_registered)
            continue;

        // ← KEY FIX: Match the msg_id
        if (ctx->custom_topics[i].pending_msg_id == mid) {
            ctx->custom_topics[i].is_registered = true;
            ctx->custom_topics[i].topic_id = tid;
            ctx->custom_topics[i].pending_msg_id = 0; // Clear pending
            Serial.print(F("✓ Registered: "));
            Serial.println(ctx->custom_topics[i].topic_name);
            return; // Done
        }
    }
    Serial.print(F("⚠ REGACK mid="));
    Serial.print(mid);
    Serial.println(F(" no matching pending topic"));
}

static void handle_suback(mqtt_sn_context_t *ctx, const uint8_t *d, uint8_t len) {
    (void)len;
    uint16_t tid = ((uint16_t)d[3] << 8) | d[4];
    uint16_t mid = ((uint16_t)d[5] << 8) | d[6];
    uint8_t rc = d[7];

    Serial.print(F("SUBACK tid="));
    Serial.print(tid);
    Serial.print(F(" mid="));
    Serial.print(mid);
    Serial.print(F(" rc="));
    Serial.println(rc);

    if (rc != MQTTSN_RETURN_ACCEPTED || !ctx)
        return;

    for (uint8_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
        if (!ctx->custom_topics[i].in_use)
            continue;
        if (ctx->custom_topics[i].is_sender)
            continue;
        if (ctx->custom_topics[i].is_registered)
            continue;
        if (ctx->custom_topics[i].pending_msg_id != mid)
            continue;

        ctx->custom_topics[i].is_registered = true;
        ctx->custom_topics[i].topic_id = tid;
        ctx->custom_topics[i].pending_msg_id = 0;
        Serial.print(F("Subscribed: "));
        Serial.println(ctx->custom_topics[i].topic_name);
        return;
    }
}

static void handle_puback(const uint8_t *d, uint8_t len) {
    (void)len;
    uint16_t mid = ((uint16_t)d[4] << 8) | d[5];
    Serial.print(F("PUBACK mid="));
    Serial.println(mid);
    remove_pending_qos_msg(mid);
}

static void handle_pubrec(const uint8_t *d, uint8_t len) {
    (void)len;
    uint16_t mid = ((uint16_t)d[2] << 8) | d[3];
    Serial.print(F("PUBREC mid="));
    Serial.println(mid);
    mqtt_sn_send_pubrel(mid);
    for (uint8_t i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
        if (g_pending_msgs[i].in_use && g_pending_msgs[i].msg_id == mid) {
            g_pending_msgs[i].step = 1;
            g_pending_msgs[i].timestamp_ms = millis();
            break;
        }
    }
}

static void handle_pubcomp(const uint8_t *d, uint8_t len) {
    (void)len;
    uint16_t mid = ((uint16_t)d[2] << 8) | d[3];
    Serial.print(F("PUBCOMP mid="));
    Serial.println(mid);
    remove_pending_qos_msg(mid);
}

static void handle_pubrel(const uint8_t *d, uint8_t len) {
    (void)len;
    uint16_t mid = ((uint16_t)d[2] << 8) | d[3];
    mqtt_sn_send_pubcomp(mid);
}

static void handle_publish(mqtt_sn_context_t *ctx, const uint8_t *d, uint8_t len) {
    (void)ctx;
    if (len < MQTTSN_PUBLISH_HEADER_LEN)
        return;
    uint8_t qos = (d[MQTTSN_OFFSET_FLAGS] >> MQTTSN_QOS_SHIFT) & MQTTSN_FLAG_QOS_MASK;
    uint16_t tid = ((uint16_t)d[MQTTSN_OFFSET_TOPIC_ID_HIGH] << 8) | d[MQTTSN_OFFSET_TOPIC_ID_LOW];
    uint16_t mid = ((uint16_t)d[MQTTSN_OFFSET_MSG_ID_HIGH] << 8) | d[MQTTSN_OFFSET_MSG_ID_LOW];
    uint8_t plen = len - MQTTSN_PUBLISH_HEADER_LEN;
    const uint8_t *payload = &d[MQTTSN_OFFSET_PAYLOAD];

    /* Print received message */
    Serial.print(F("[MSG] tid="));
    Serial.print(tid);
    Serial.print(F(" qos="));
    Serial.print(qos);
    Serial.print(F(" payload="));
    for (uint8_t i = 0; i < plen; i++)
        Serial.print((char)payload[i]);
    Serial.println();

    /* LED commands */
    if (plen == LED_ON_CMD_LEN && memcmp(payload, "led on", 6) == 0)
        digitalWrite(LED_PIN, HIGH);
    if (plen == LED_OFF_CMD_LEN && memcmp(payload, "led off", 7) == 0)
        digitalWrite(LED_PIN, LOW);

    if (qos == QOS_LEVEL_1)
        mqtt_sn_send_puback(tid, mid, MQTTSN_RETURN_ACCEPTED);
    if (qos == QOS_LEVEL_2)
        mqtt_sn_send_pubrec(mid);
}

static void handle_pingresp(void) {
    g_ping_ack_received = true;
    g_last_pingresp = millis();
    Serial.println(F("PINGRESP"));
}

/* ── Dispatcher ──────────────────────────────────────────────── */
void udp_recv_callback_arduino(mqtt_sn_context_t *ctx, const uint8_t *data, uint8_t length) {
    if (!data || length < MQTTSN_HEADER_SIZE)
        return;

    /* ACK drop test hook */
    uint8_t mt = data[MQTTSN_OFFSET_MSG_TYPE];
    if (ctx && ctx->drop_acks &&
        (mt == MQTTSN_MSG_TYPE_PUBACK || mt == MQTTSN_MSG_TYPE_PUBREC ||
         mt == MQTTSN_MSG_TYPE_PUBCOMP)) {
        Serial.print(F("Dropped ACK 0x"));
        Serial.println(mt, HEX);
        return;
    }

    switch (mt) {
    case MQTTSN_MSG_TYPE_CONNACK:
        handle_connack(ctx, data, length);
        break;
    case MQTTSN_MSG_TYPE_REGACK:
        handle_regack(ctx, data, length);
        break;
    case MQTTSN_MSG_TYPE_SUBACK:
        handle_suback(ctx, data, length);
        break;
    case MQTTSN_MSG_TYPE_PUBACK:
        handle_puback(data, length);
        break;
    case MQTTSN_MSG_TYPE_PUBREC:
        handle_pubrec(data, length);
        break;
    case MQTTSN_MSG_TYPE_PUBCOMP:
        handle_pubcomp(data, length);
        break;
    case MQTTSN_MSG_TYPE_PUBREL:
        handle_pubrel(data, length);
        break;
    case MQTTSN_MSG_TYPE_PUBLISH:
        handle_publish(ctx, data, length);
        break;
    case MQTTSN_MSG_TYPE_PINGRESP:
        handle_pingresp();
        break;
    default:
        Serial.print(F("Unknown type 0x"));
        Serial.println(mt, HEX);
        break;
    }
}

void mqtt_sn_poll(mqtt_sn_context_t *ctx) {
    uint8_t buf[MQTTSN_PUBLISH_HEADER_LEN + MQTTSN_RETRY_PAYLOAD_SIZE];
    uint8_t len = mqttsn_transport_recv(buf, sizeof(buf), 20);
    if (len > 0)
        udp_recv_callback_arduino(ctx, buf, len);
}