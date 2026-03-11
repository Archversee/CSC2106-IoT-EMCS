#include "config.h"
#include "mqtt-sn-udp.h"
#include "mqttsn_transport.h"
#include <SSD1306AsciiWire.h>
#include <Wire.h>

#define OLED_ADDR 0x3C
#define KEEPALIVE_MS (KEEPALIVE_INTERVAL_SEC * 1000UL)
#define QOS_CHK_MS 5000UL

#define PUB_QOS1_MS 10000UL // sensors/data - every 30s - QoS 1
#define PUB_QOS0_MS 10000UL // sensors/arduino/data - every 10s - QoS 0
#define PUB_QOS0_REPS 3U

SSD1306AsciiWire oled;

static mqtt_sn_context_t g_ctx;
static uint32_t g_last_ping_ms = 0;
static uint32_t g_last_qos_chk_ms = 0;
static uint32_t g_last_pub_qos1 = 0;
static uint32_t g_last_pub_qos0 = 0;
static uint16_t g_pub_count_qos0 = 0; // sensors/arduino/data counter
static uint16_t g_pub_count_qos1 = 0; // sensors/data counter

static uint8_t seq_phase = 0; // 0-2 = QoS0 x3, 3 = QoS1, 4 = delay
static uint8_t qos0_rep = 0;
static uint32_t seq_phase_ms = 0;

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

// OLED
static void oledShow(const __FlashStringHelper *l1, const __FlashStringHelper *l2 = nullptr) {
    oled.clear();
    oled.println(l1);
    if (l2)
        oled.println(l2);
}

// Add all 4 topics
static void add_all_topics(void) {
    mqtt_sn_add_topic_for_registration(&g_ctx, TOPIC_DATA_1);
    mqtt_sn_add_topic_for_registration(&g_ctx, TOPIC_DATA_2);
    mqtt_sn_add_topic_for_subscription(&g_ctx, TOPIC_CMD_1, QOS_LEVEL_1);
    mqtt_sn_add_topic_for_subscription(&g_ctx, TOPIC_CMD_2, QOS_LEVEL_2);
}

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

void loop() {
    uint32_t now = millis();

    mqtt_sn_poll(&g_ctx);

    switch (g_state) {

    // CONNECT to broker, wait for CONNACK
    case STATE_INIT:
        g_ping_ack_received = false;
        mqtt_sn_connect();
        g_state_ms = now;
        g_state = STATE_CONNECTING;
        oledShow(F("Connecting..."));
        Serial.println(F("CONNECTING"));
        break;

    // Wait for CONNACK, then start registering topics
    case STATE_CONNECTING: {
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
    }
    // Register topics, wait for REGACKs and topic IDs.
    case STATE_REGISTERING: {
        // Drain any ACKs that arrived during registration
        if (g_puback_pending) {
            g_puback_pending = false;
            mqtt_sn_send_puback(g_puback_tid, g_puback_mid, MQTTSN_RETURN_ACCEPTED);
            break;
        }
        if (g_pubrec_pending) {
            g_pubrec_pending = false;
            mqtt_sn_send_pubrec(g_pubrec_mid);
            break;
        }
        if (g_pubcomp_pending) {
            g_pubcomp_pending = false;
            mqtt_sn_send_pubcomp(g_pubcomp_mid);
            break;
        }

        // Check if any topic has been sent but not yet acknowledged
        bool waiting_for_ack = false;
        for (uint8_t i = 0; i < MAX_CUSTOM_TOPICS; i++) {
            if (g_ctx.custom_topics[i].in_use && !g_ctx.custom_topics[i].is_registered &&
                g_ctx.custom_topics[i].pending_msg_id != 0) {
                waiting_for_ack = true;
                break;
            }
        }

        // Only send REGISTER/SUBSCRIBE if not waiting for an ACK or if the retry interval has
        // expired
        if (!waiting_for_ack)
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
            g_last_pub_qos1 = now;
            g_last_pub_qos0 = now - PUB_QOS0_MS;
            g_state = STATE_READY;

        } else if (now - g_state_ms > REGISTER_TIMEOUT_MS) {
            Serial.println(F("REG timeout"));
            mqtt_sn_invalidate_all_topics(&g_ctx);
            g_state = STATE_INIT;
        }
        break;
    }
    // Main loop: publish data, check QoS timeouts, send keepalive PINGREQs
    case STATE_READY: {
        uint16_t tid1 = mqtt_sn_get_topic_id(&g_ctx, TOPIC_DATA_1);
        uint16_t tid2 = mqtt_sn_get_topic_id(&g_ctx, TOPIC_DATA_2);

        bool ack_pending = g_puback_pending || g_pubrec_pending || g_pubcomp_pending;

        // If any ACK is pending, defer all publish timers to prevent
        // outgoing TX from grabbing the radio before ACK fires
        if (ack_pending) {
            g_last_pub_qos0 = now; // reset so they don't fire immediately after ACK
            g_last_pub_qos1 = now;
        }

        if (g_pubrec_pending) {
            g_pubrec_pending = false;
            mqtt_sn_send_pubrec(g_pubrec_mid);
            break;
        }
        if (g_pubcomp_pending) {
            g_pubcomp_pending = false;
            mqtt_sn_send_pubcomp(g_pubcomp_mid);
            break;
        }

        if (g_puback_pending) {
            g_puback_pending = false;
            mqtt_sn_send_puback(g_puback_tid, g_puback_mid, MQTTSN_RETURN_ACCEPTED);
            break;
        }

        // QoS 0 publish phase (3 times)
        if (seq_phase < 3) {
            if (now - g_last_pub_qos0 >= PUB_QOS0_MS) {
                g_last_pub_qos0 = now;
                g_pub_count_qos0++;
                char payload[48];
                snprintf(payload, sizeof(payload), "%s %u", MQTT_SN_CLIENT_ID, g_pub_count_qos0);
                Serial.print(F("PUB QoS0 #"));
                Serial.println(g_pub_count_qos0);
                oledShow(F("PUB QoS0"), F(TOPIC_DATA_2));
                mqtt_sn_publish_topic_id_auto(tid2, (const uint8_t *)payload, strlen(payload),
                                              QOS_LEVEL_0);
                oledShow(F("Ready!"), F("Publishing..."));
                seq_phase++;
                if (seq_phase == 3)
                    seq_phase_ms = now; // start 10s wait before QoS1
                break;
            }
        }
        // 10s delay then QoS 1
        else if (seq_phase == 3) {
            if (now - seq_phase_ms >= PUB_QOS1_MS) {
                g_pub_count_qos1++;
                char payload[48];
                snprintf(payload, sizeof(payload), "%s %u", MQTT_SN_CLIENT_ID, g_pub_count_qos1);
                Serial.print(F("PUB QoS1 #"));
                Serial.println(g_pub_count_qos1);
                oledShow(F("PUB QoS1"), F(TOPIC_DATA_1));
                mqtt_sn_publish_topic_id_auto(tid1, (const uint8_t *)payload, strlen(payload),
                                              QOS_LEVEL_1);
                oledShow(F("Ready!"), F("Publishing..."));
                seq_phase = 4;
                seq_phase_ms = now; // start 10s final delay
                break;
            }
        }
        // 10s delay then restart
        else if (seq_phase == 4) {
            if (now - seq_phase_ms >= PUB_QOS1_MS) {
                seq_phase = 0;
                g_last_pub_qos0 = now; // reset so first QoS0 fires after 10s
                break;
            }
        }
        // Keepalive PINGREQ every KEEPALIVE_MS we dont need keepalive here since we are constantly
        // sending data and it causes radio switcihig between TX and RX interference
        // if (now - g_last_ping_ms > KEEPALIVE_MS) {
        //     mqtt_sn_pingreq();
        //     g_last_ping_ms = now;
        //     break;
        // }

        // QoS retry check every QOS_CHK_MS
        if (now - g_last_qos_chk_ms > QOS_CHK_MS) {
            check_qos_timeouts();
            g_last_qos_chk_ms = now;
            break;
        }
        break;
    }

    // Error state: show message, wait, then reset to INIT to retry connection
    case STATE_ERROR:
        oledShow(F("ERROR"), F("Reconnect 5s"));
        delay(5000);
        mqtt_sn_invalidate_all_topics(&g_ctx);
        g_state = STATE_INIT;
        break;
    }
}
