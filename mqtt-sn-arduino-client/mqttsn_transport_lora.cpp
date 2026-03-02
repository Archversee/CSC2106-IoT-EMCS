/*!
 * @file  mqttsn_transport_lora.cpp
 * @brief LoRa transport — Arduino + Cytron LoRa-RFM Shield (RFM95 / SX1276)
 *
 * FIX: RadioHead RH_RF95::send() prepends its own 4-byte header automatically.
 * We disable this by setting both TO and FROM headers to 0xFF (broadcast/any)
 * so RadioHead's 4 bytes become [FF][FF][seq][flags] which is exactly what
 * the RPi bridge expects as the RadioHead header it will strip.
 *
 * We then encode our actual node IDs inside the MQTT-SN payload framing
 * in a way the bridge can use — OR we simply accept that the bridge uses
 * RadioHead's FROM byte (0xFF) as the client key, which is fine for a
 * single Arduino node since it gets a stable port (20000).
 *
 * For multi-node support we set RadioHead's FROM header to LORA_MY_NODE_ID
 * using setHeaderFrom() so the bridge can distinguish nodes.
 */

#include "mqttsn_transport.h"
#include "mqtt-sn-udp.h"
#include <Arduino.h>
#include <RH_RF95.h>
#include <SPI.h>

/* ── Cytron LoRa-RFM Shield pinout ───────────────────────────── */
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

/* ── Node IDs ─────────────────────────────────────────────────── */
#define LORA_BROADCAST 0xFF

static RH_RF95 s_rf95(RFM95_CS, RFM95_INT);
static int16_t s_last_rssi = 0;

void mqttsn_transport_init(void) {
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);

    if (!s_rf95.init()) {
        Serial.println(F("[transport] FATAL: RFM95 init failed"));
        while (1)
            ;
    }
    if (!s_rf95.setFrequency(915.0f)) {
        Serial.println(F("[transport] FATAL: setFrequency failed"));
        while (1)
            ;
    }

    /* Set modulation params ONCE — never repeat these in send() */
    s_rf95.setSpreadingFactor(7);
    s_rf95.setSignalBandwidth(125000);
    s_rf95.setCodingRate4(5);
    s_rf95.setTxPower(13, false);

    /* Set headers ONCE */
    s_rf95.setHeaderTo(LORA_GW_NODE_ID);
    s_rf95.setHeaderFrom(LORA_MY_NODE_ID);
    s_rf95.setHeaderFlags(0x00, 0xFF);

    /* setThisAddress LAST — modem config calls above may reset it */
    s_rf95.setThisAddress(LORA_MY_NODE_ID);

    Serial.println(F("[transport] RFM95 ready 915MHz SF7 BW125 CR4/5"));
    Serial.print(F("[transport] Node ID: 0x"));
    Serial.println(LORA_MY_NODE_ID, HEX);
}

int mqttsn_transport_send(const uint8_t *buf, uint8_t len) {
    if (!buf || len == 0)
        return -1;
    if (len > RH_RF95_MAX_MESSAGE_LEN - RH_RF95_HEADER_LEN) {
        Serial.println(F("[transport] send: payload too large"));
        return -1;
    }

    s_rf95.send(buf, len);
    s_rf95.waitPacketSent();

    /* After TX, stay in RX and wait up to 2s for a response.
       Caller (mqtt_sn_register_topic etc.) will read it via recv().
       This is the key fix — without this, we retransmit before
       the REGACK/CONNACK arrives. */
    uint32_t t = millis();
    while (millis() - t < 2000) {
        if (s_rf95.available())
            break;
        delay(5);
    }

    delay(10);
    return 0;
}

uint8_t mqttsn_transport_recv(uint8_t *buf, uint8_t buf_size, uint32_t timeout_ms) {
    if (!buf)
        return 0;
    uint32_t deadline = millis() + timeout_ms;
    do {
        if (s_rf95.available()) { // non-blocking check
            uint8_t raw[RH_RF95_MAX_MESSAGE_LEN];
            uint8_t raw_len = sizeof(raw);
            if (!s_rf95.recv(raw, &raw_len)) {
                Serial.print(F("[rx] filtered TO=0x"));
                Serial.print(s_rf95.headerTo(), HEX);
                Serial.print(F(" FROM=0x"));
                Serial.println(s_rf95.headerFrom(), HEX);
                continue;
            }
            if (raw_len == 0 || raw_len > buf_size)
                continue;
            memcpy(buf, raw, raw_len);
            s_last_rssi = s_rf95.lastRssi();
            Serial.print(F("[rx] got "));
            Serial.print(raw_len);
            Serial.print(F("B RSSI="));
            Serial.println(s_last_rssi);
            return raw_len;
        }
    } while (timeout_ms > 0 && millis() < deadline);
    return 0;
}

int16_t mqttsn_transport_last_rssi(void) { return s_last_rssi; }