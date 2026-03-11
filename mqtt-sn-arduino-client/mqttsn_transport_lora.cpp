#include "config.h"
#include "mqtt-sn-udp.h"
#include "mqttsn_transport.h"
#include <Arduino.h>
#include <RH_RF95.h>
#include <SPI.h>

// Cytron LoRa-RFM Shield pinout
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

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

    // Set modulation params ONCE
    s_rf95.setSpreadingFactor(7);
    s_rf95.setSignalBandwidth(125000);
    s_rf95.setCodingRate4(5);
    s_rf95.setTxPower(13, false);

    s_rf95.setHeaderTo(LORA_GW_NODE_ID);
    // For multi-node support we set RadioHead's FROM header to LORA_MY_NODE_ID using
    // setHeaderFrom() so the bridge can distinguish nodes.
    s_rf95.setHeaderFrom(LORA_MY_NODE_ID);
    // We don't use RadioHead's ACK/retry features, so set header flags to 0x00 for all packets
    s_rf95.setHeaderFlags(0x00, 0xFF);

    // setThisAddress LAST as modem config calls above may reset it
    s_rf95.setThisAddress(LORA_MY_NODE_ID);

    Serial.println(F("[transport] RFM95 ready 915MHz SF7 BW125 CR4/5"));
    Serial.print(F("[transport] Node ID: 0x"));
    Serial.println(LORA_MY_NODE_ID, HEX);
}

int mqttsn_transport_send(const uint8_t *buf, uint8_t len) {
    if (!buf || len == 0)
        return -1;

    uint32_t t = millis();
    while (s_rf95.mode() == RHGenericDriver::RHModeTx) {
        if (millis() - t > 500) {
            Serial.println(F("[transport] TX timeout, forcing idle"));
            break;
        }
        delay(5);
    }

    s_rf95.setModeIdle();
    delay(5);

    bool ok = s_rf95.send(buf, len);
    s_rf95.waitPacketSent();

    s_rf95.setModeRx(); // explicitly re-enter RX after TX

    Serial.print(F("[tx] sent "));
    Serial.print(len);
    Serial.print(F("B ok="));
    Serial.println(ok);

    return ok ? 0 : -1;
}

uint8_t mqttsn_transport_recv(uint8_t *buf, uint8_t buf_size, uint32_t timeout_ms) {
    if (!buf)
        return 0;
    uint32_t deadline = millis() + timeout_ms;
    do {
        if (s_rf95.available()) { // non-blocking check
            uint8_t raw[RH_RF95_MAX_MESSAGE_LEN];
            uint8_t raw_len = sizeof(raw);
            // For multi-node support, filter by RadioHead's TO header - only process packets
            // addressed to gateway or broadcast
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