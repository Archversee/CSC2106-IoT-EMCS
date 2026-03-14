#include "config.h"
#include "mqtt-sn-udp.h"
#include "mqttsn_transport.h"
#include "mesh_protocol.h"
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
static uint16_t s_mesh_seq = 0;

#define MESH_SEEN_CACHE_SIZE 8

typedef struct {
    uint8_t src_id;
    uint16_t seq_num;
    bool valid;
} mesh_seen_entry_t;

static mesh_seen_entry_t s_seen_cache[MESH_SEEN_CACHE_SIZE];
static uint8_t s_seen_index = 0;

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

    s_rf95.setHeaderTo(LORA_BROADCAST);
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

    for (uint8_t i = 0; i < MESH_SEEN_CACHE_SIZE; i++) {
        s_seen_cache[i].valid = false;
    }
    s_seen_index = 0;
}

static bool mesh_packet_seen(uint8_t src_id, uint16_t seq_num) {
    for (uint8_t i = 0; i < MESH_SEEN_CACHE_SIZE; i++) {
        if (s_seen_cache[i].valid &&
            s_seen_cache[i].src_id == src_id &&
            s_seen_cache[i].seq_num == seq_num) {
            return true;
        }
    }
    return false;
}

static void mesh_remember_packet(uint8_t src_id, uint16_t seq_num) {
    s_seen_cache[s_seen_index].src_id = src_id;
    s_seen_cache[s_seen_index].seq_num = seq_num;
    s_seen_cache[s_seen_index].valid = true;

    s_seen_index++;
    if (s_seen_index >= MESH_SEEN_CACHE_SIZE) {
        s_seen_index = 0;
    }
}

int mqttsn_transport_send(const uint8_t *buf, uint8_t len) {
    if (!buf || len == 0)
        return -1;

    if ((uint16_t)len + MESH_HEADER_SIZE > RH_RF95_MAX_MESSAGE_LEN) {
        Serial.println(F("[transport] TX too large for LoRa frame"));
        return -1;
    }

    uint8_t tx_buf[RH_RF95_MAX_MESSAGE_LEN];

    mesh_header_t hdr;
    hdr.src_id = LORA_MY_NODE_ID;
    hdr.dst_id = LORA_GW_NODE_ID;   // for now, all traffic goes to gateway
    hdr.ttl = MESH_DEFAULT_TTL;
    hdr.seq_num = s_mesh_seq++;

    memcpy(tx_buf, &hdr, MESH_HEADER_SIZE);
    memcpy(tx_buf + MESH_HEADER_SIZE, buf, len);

    uint8_t tx_len = MESH_HEADER_SIZE + len;

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

    bool ok = s_rf95.send(tx_buf, tx_len);
    s_rf95.waitPacketSent();

    s_rf95.setModeRx();

    Serial.print(F("[tx] mesh sent "));
    Serial.print(tx_len);
    Serial.print(F("B src=0x"));
    Serial.print(hdr.src_id, HEX);
    Serial.print(F(" dst=0x"));
    Serial.print(hdr.dst_id, HEX);
    Serial.print(F(" ttl="));
    Serial.print(hdr.ttl);
    Serial.print(F(" seq="));
    Serial.print(hdr.seq_num);
    Serial.print(F(" ok="));
    Serial.println(ok);

    return ok ? 0 : -1;
}



uint8_t mqttsn_transport_recv(uint8_t *buf, uint8_t buf_size, uint32_t timeout_ms) {
    if (!buf)
        return 0;

    uint32_t deadline = millis() + timeout_ms;

    do {
        if (s_rf95.available()) {
            uint8_t raw[RH_RF95_MAX_MESSAGE_LEN];
            uint8_t raw_len = sizeof(raw);

            if (!s_rf95.recv(raw, &raw_len)) {
                continue;
            }

            if (raw_len < MESH_HEADER_SIZE) {
                continue;
            }

            mesh_header_t hdr;
            memcpy(&hdr, raw, MESH_HEADER_SIZE);

            if (mesh_packet_seen(hdr.src_id, hdr.seq_num)) {
                Serial.print(F("[dup] drop src=0x"));
                Serial.print(hdr.src_id, HEX);
                Serial.print(F(" seq="));
                Serial.println(hdr.seq_num);
                continue;
            }

            mesh_remember_packet(hdr.src_id, hdr.seq_num);

            uint8_t *payload = raw + MESH_HEADER_SIZE;
            uint8_t payload_len = raw_len - MESH_HEADER_SIZE;

            Serial.print(F("[mesh-rx] src=0x"));
            Serial.print(hdr.src_id, HEX);
            Serial.print(F(" dst=0x"));
            Serial.print(hdr.dst_id, HEX);
            Serial.print(F(" ttl="));
            Serial.print(hdr.ttl);
            Serial.print(F(" seq="));
            Serial.println(hdr.seq_num);

            // Case 1: packet is for me -> pass MQTT-SN payload upward
            if (hdr.dst_id == LORA_MY_NODE_ID) {
                if (payload_len == 0 || payload_len > buf_size) {
                    Serial.println(F("[rx] payload invalid for local delivery"));
                    continue;
                }

                memcpy(buf, payload, payload_len);
                s_last_rssi = s_rf95.lastRssi();

                Serial.print(F("[rx] delivered "));
                Serial.print(payload_len);
                Serial.println(F(" bytes to MQTT-SN"));

                return payload_len;
            }

            // Case 2: packet is for gateway and I am relay -> forward it
#if MESH_ENABLE_RELAY
            if (LORA_MY_NODE_ID == MESH_RELAY_NODE_ID) {
                if (hdr.dst_id != LORA_MY_NODE_ID && hdr.ttl > 1) {
                    hdr.ttl--;

                    uint8_t fwd_buf[RH_RF95_MAX_MESSAGE_LEN];
                    memcpy(fwd_buf, &hdr, MESH_HEADER_SIZE);
                    memcpy(fwd_buf + MESH_HEADER_SIZE, payload, payload_len);

                    s_rf95.setModeIdle();
                    delay(5);

                    bool ok = s_rf95.send(fwd_buf, MESH_HEADER_SIZE + payload_len);
                    s_rf95.waitPacketSent();
                    s_rf95.setModeRx();

                    Serial.print(F("[relay] forwarded src=0x"));
                    Serial.print(hdr.src_id, HEX);
                    Serial.print(F(" dst=0x"));
                    Serial.print(hdr.dst_id, HEX);
                    Serial.print(F(" ttl="));
                    Serial.print(hdr.ttl);
                    Serial.print(F(" seq="));
                    Serial.print(hdr.seq_num);
                    Serial.print(F(" ok="));
                    Serial.println(ok);

                    continue;
                }
            }
#endif
        }
    } while ((int32_t)(millis() - deadline) < 0);

    return 0;
}

int16_t mqttsn_transport_last_rssi(void) { return s_last_rssi; }