/*!
 * @file  arduino_mqttsn_lora.ino
 * @brief MQTT-SN over LoRa — ATmega328P optimised application layer
 *
 * Publishes periodically:
 *   sensors/data         QoS 1  every 30s
 *   sensors/arduino/data QoS 0  every 10s
 *
 * Subscribes and prints received messages from:
 *   sensors/cmd          QoS 1
 *   sensors/arduino/cmd  QoS 2
 */

#include "mqtt-sn-udp.h"
#include "mqttsn_transport.h"
#include <SSD1306AsciiWire.h>
#include <Wire.h>

#define OLED_ADDR 0x3C
#define KEEPALIVE_MS (KEEPALIVE_INTERVAL_SEC * 1000UL)
#define QOS_CHK_MS 5000UL

#define PUB_QOS1_MS 30000UL /* sensors/data         every 30s  QoS 1 */
#define PUB_QOS0_MS 10000UL /* sensors/arduino/data every 10s  QoS 0 */

SSD1306AsciiWire oled;

static mqtt_sn_context_t g_ctx;
static uint32_t g_last_ping_ms = 0;
static uint32_t g_last_qos_chk_ms = 0;
static uint32_t g_last_pub_qos1 = 0;
static uint32_t g_last_pub_qos0 = 0;
static uint16_t g_pub_count = 0;

typedef enum : uint8_t {
    STATE_INIT,
    STATE_CONNECTING,
    STATE_REGISTERING,
    STATE_READY,
    STATE_ERROR
} state_t;

static state_t g_state = STATE_INIT;
static uint32_t g_state_ms = 0;

#define CONNECT_TIMEOUT_MS 8000UL
#define REGISTER_TIMEOUT_MS 60000UL

/* ── OLED ────────────────────────────────────────────────────── */
static void oledShow(const __FlashStringHelper *l1, const __FlashStringHelper *l2 = nullptr) {
    oled.clear();
    oled.println(l1);
    if (l2)
        oled.println(l2);
}

/* ── Add all 4 topics ────────────────────────────────────────── */
static void add_all_topics(void) {
    mqtt_sn_add_topic_for_registration(&g_ctx, TOPIC_DATA_1);
    mqtt_sn_add_topic_for_registration(&g_ctx, TOPIC_DATA_2);
    mqtt_sn_add_topic_for_subscription(&g_ctx, TOPIC_CMD_1, QOS_LEVEL_1);
    mqtt_sn_add_topic_for_subscription(&g_ctx, TOPIC_CMD_2, QOS_LEVEL_2);
}

/* ── setup ───────────────────────────────────────────────────── */
void setup() {
    Serial.begin(115200);
    Wire.begin();
    delay(200);

    oled.begin(&Adafruit128x64, OLED_ADDR);
    oled.setFont(System5x7);
    oled.clear();
    oledShow(F("MQTT-SN LoRa"), F("Starting..."));

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    memset(&g_ctx, 0, sizeof(g_ctx));
    add_all_topics();

    mqttsn_transport_init();

    oledShow(F("LoRa ready"), F("Connecting..."));
    Serial.println(F("setup() complete"));
}

/* ── loop ────────────────────────────────────────────────────── */
void loop() {
    uint32_t now = millis();

    mqtt_sn_poll(&g_ctx);

    switch (g_state) {

    /* ── INIT ───────────────────────────────────────────────── */
    case STATE_INIT:
        g_ping_ack_received = false;
        mqtt_sn_connect();
        g_state_ms = now;
        g_state = STATE_CONNECTING;
        oledShow(F("Connecting..."));
        Serial.println(F("CONNECTING"));
        break;

    /* ── CONNECTING ─────────────────────────────────────────── */
    case STATE_CONNECTING:
        if (g_ping_ack_received) {
            Serial.println(F("CONNACK ok"));
            oledShow(F("Connected!"), F("Registering..."));
            add_all_topics(); /* handle_connack wiped table */
            delay(500);
            mqtt_sn_process_topic_registrations(&g_ctx);
            g_state_ms = now;
            g_state = STATE_REGISTERING;
        } else if (now - g_state_ms > CONNECT_TIMEOUT_MS) {
            Serial.println(F("CONNECT timeout"));
            oledShow(F("CONN timeout"), F("Retrying..."));
            g_state = STATE_INIT;
        }
        break;

    /* ── REGISTERING ────────────────────────────────────────── */
    case STATE_REGISTERING:
        mqtt_sn_process_topic_registrations(&g_ctx);

        if (mqtt_sn_get_topic_id(&g_ctx, TOPIC_DATA_1) > 0 &&
            mqtt_sn_get_topic_id(&g_ctx, TOPIC_DATA_2) > 0 &&
            mqtt_sn_get_topic_id(&g_ctx, TOPIC_CMD_1) > 0 &&
            mqtt_sn_get_topic_id(&g_ctx, TOPIC_CMD_2) > 0) {

            Serial.println(F("Topics ready"));
            Serial.print(F("  DATA1 tid="));
            Serial.println(mqtt_sn_get_topic_id(&g_ctx, TOPIC_DATA_1));
            Serial.print(F("  DATA2 tid="));
            Serial.println(mqtt_sn_get_topic_id(&g_ctx, TOPIC_DATA_2));
            Serial.print(F("  CMD1  tid="));
            Serial.println(mqtt_sn_get_topic_id(&g_ctx, TOPIC_CMD_1));
            Serial.print(F("  CMD2  tid="));
            Serial.println(mqtt_sn_get_topic_id(&g_ctx, TOPIC_CMD_2));

            oledShow(F("Ready!"), F("Publishing..."));
            g_last_ping_ms = now;
            g_last_qos_chk_ms = now;
            g_last_pub_qos1 = now;               /* first QoS1 in 30s  */
            g_last_pub_qos0 = now - PUB_QOS0_MS; /* first QoS0 NOW     */
            g_state = STATE_READY;

        } else if (now - g_state_ms > REGISTER_TIMEOUT_MS) {
            Serial.println(F("REG timeout"));
            mqtt_sn_invalidate_all_topics(&g_ctx);
            g_state = STATE_INIT;
        }
        break;

    /* ── READY ──────────────────────────────────────────────── */
    case STATE_READY: {
        uint16_t tid1 = mqtt_sn_get_topic_id(&g_ctx, TOPIC_DATA_1);
        uint16_t tid2 = mqtt_sn_get_topic_id(&g_ctx, TOPIC_DATA_2);

        /* ── QoS 0 publish — sensors/arduino/data — every 10s ── */
        if (now - g_last_pub_qos0 >= PUB_QOS0_MS) {
            g_last_pub_qos0 = now;
            g_pub_count++;
            char payload[20];
            snprintf(payload, sizeof(payload), "D0:%u", g_pub_count);
            Serial.print(F("PUB QoS0 #"));
            Serial.println(g_pub_count);
            oledShow(F("PUB QoS0"), F(TOPIC_DATA_2));
            mqtt_sn_publish_topic_id_auto(tid2, (const uint8_t *)payload, strlen(payload),
                                          QOS_LEVEL_0);
            oledShow(F("Ready!"), F("Publishing..."));
        }

        /* ── QoS 1 publish — sensors/data — every 30s ────────── */
        if (now - g_last_pub_qos1 >= PUB_QOS1_MS) {
            g_last_pub_qos1 = now;
            char payload[20];
            snprintf(payload, sizeof(payload), "D1:%u", g_pub_count);
            Serial.print(F("PUB QoS1 #"));
            Serial.println(g_pub_count);
            oledShow(F("PUB QoS1"), F(TOPIC_DATA_1));
            mqtt_sn_publish_topic_id_auto(tid1, (const uint8_t *)payload, strlen(payload),
                                          QOS_LEVEL_1);
            oledShow(F("Ready!"), F("Publishing..."));
        }

        /* ── Keepalive ────────────────────────────────────────── */
        if (now - g_last_ping_ms > KEEPALIVE_MS) {
            mqtt_sn_pingreq();
            g_last_ping_ms = now;
        }

        /* ── QoS retry ────────────────────────────────────────── */
        if (now - g_last_qos_chk_ms > QOS_CHK_MS) {
            check_qos_timeouts();
            g_last_qos_chk_ms = now;
        }
        break;
    }

    /* ── ERROR ──────────────────────────────────────────────── */
    case STATE_ERROR:
        oledShow(F("ERROR"), F("Reconnect 5s"));
        delay(5000);
        mqtt_sn_invalidate_all_topics(&g_ctx);
        g_state = STATE_INIT;
        break;
    }
}
