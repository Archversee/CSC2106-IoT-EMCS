/*
 * mqttsn_transport_lora.cpp — LoRa mesh transport, RAM-optimised for Arduino Uno
 *
 * Key RAM saving: mesh_packet_t (252 bytes) and local raw[] (251 bytes) are NEVER
 * allocated. Instead we use two shared static buffers and parse the 4-byte mesh
 * header inline with direct byte access.
 *
 * Static RAM budget for this file:
 *   s_rx_buf[251]  : 251 bytes
 *   s_tx_buf[251]  : 251 bytes
 *   s_dedup[16*3]  : 48  bytes
 *   s_rf95 + misc  : ~26 bytes
 *   Total          : ~576 bytes
 */

#include "config.h"
#include "mesh_protocol.h"
#include "mqtt-sn-udp.h"
#include "mqttsn_transport.h"
#include <Arduino.h>
#include <RH_RF95.h>
#include <SPI.h>

#define RFM95_CS  10
#define RFM95_RST  9
#define RFM95_INT  2

/* Max realistic packet: PUBLISH(7) + payload(32) + mesh_hdr(4) = 43 bytes.
 * 64 bytes covers all packet types with comfortable margin.
 * Using 64 instead of RH_RF95_MAX_MESSAGE_LEN(251) saves 374 bytes of RAM. */
#define LORA_BUF_SIZE 64

/* Single shared RX and TX buffers — in globals, never on stack */
static uint8_t s_rx_buf[LORA_BUF_SIZE];
static uint8_t s_tx_buf[LORA_BUF_SIZE];

static RH_RF95  s_rf95(RFM95_CS, RFM95_INT);
static int16_t  s_last_rssi      = 0;
static uint8_t  s_seq_num        = 0;
static uint8_t  s_last_hop_count = 0;

/* Dedup cache */
typedef struct { uint8_t src_id; uint8_t seq_num; bool valid; } dedup_entry_t;
static dedup_entry_t s_dedup[MESH_DEDUP_CACHE_SIZE];
static uint8_t       s_dedup_head = 0;

static bool dedup_seen(uint8_t src, uint8_t seq) {
    for (uint8_t i = 0; i < MESH_DEDUP_CACHE_SIZE; i++)
        if (s_dedup[i].valid && s_dedup[i].src_id == src && s_dedup[i].seq_num == seq)
            return true;
    return false;
}
static void dedup_add(uint8_t src, uint8_t seq) {
    s_dedup[s_dedup_head] = { src, seq, true };
    s_dedup_head = (s_dedup_head + 1) % MESH_DEDUP_CACHE_SIZE;
}

static int radio_tx(const uint8_t *raw, uint8_t raw_len) {
    if (!raw || raw_len == 0) return -1;
    uint32_t t = millis();
    while (s_rf95.mode() == RHGenericDriver::RHModeTx) {
        if (millis() - t > 500) { Serial.println(F("[transport] TX timeout")); break; }
        delay(5);
    }
    s_rf95.setHeaderTo(RH_BROADCAST_ADDRESS);
    s_rf95.setHeaderFrom(LORA_MY_NODE_ID);
    s_rf95.setHeaderFlags(0x00, 0xFF);
    s_rf95.setModeIdle();
    delay(5);
    bool ok = s_rf95.send(raw, raw_len);
    s_rf95.waitPacketSent();
    s_rf95.setModeRx();
    return ok ? 0 : -1;
}

void mqttsn_transport_init(void) {
    memset(s_dedup, 0, sizeof(s_dedup));
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, LOW);  delay(10);
    digitalWrite(RFM95_RST, HIGH); delay(10);
    if (!s_rf95.init()) {
        Serial.println(F("[transport] FATAL: RFM95 init failed")); while (1);
    }
    if (!s_rf95.setFrequency(915.0f)) {
        Serial.println(F("[transport] FATAL: setFrequency failed")); while (1);
    }
    s_rf95.setSpreadingFactor(7);
    s_rf95.setSignalBandwidth(125000);
    s_rf95.setCodingRate4(5);
    s_rf95.setTxPower(13, false);
    s_rf95.setHeaderFlags(0x00, 0xFF);
    s_rf95.setPromiscuous(true);
    s_rf95.setThisAddress(LORA_MY_NODE_ID);
    Serial.println(F("[transport] RFM95 ready 915MHz SF7 BW125 CR4/5"));
    Serial.print(F("[transport] Node ID : 0x")); Serial.println(LORA_MY_NODE_ID, HEX);
    Serial.print(F("[transport] Role    : "));
    Serial.println(MY_NODE_ROLE == NODE_ROLE_RELAY ? F("RELAY") : F("ENDPOINT"));
    s_rf95.setModeRx();
}

int mqttsn_transport_send(const uint8_t *buf, uint8_t len) {
    if (!buf || len == 0) return -1;
    if ((uint8_t)(MESH_HEADER_SIZE + len) > sizeof(s_tx_buf)) {
        Serial.println(F("[transport] send: too large")); return -1;
    }
    s_seq_num++;
    dedup_add(LORA_MY_NODE_ID, s_seq_num);
    /* Build mesh header + payload directly into shared tx buffer */
    s_tx_buf[MESH_OFF_SRC] = LORA_MY_NODE_ID;
    s_tx_buf[MESH_OFF_DST] = MESH_ADDR_GATEWAY;
    s_tx_buf[MESH_OFF_TTL] = MESH_TTL_DEFAULT;
    s_tx_buf[MESH_OFF_SEQ] = s_seq_num;
    memcpy(s_tx_buf + MESH_HEADER_SIZE, buf, len);
    int rc = radio_tx(s_tx_buf, MESH_HEADER_SIZE + len);
    Serial.print(F("[tx] seq=")); Serial.print(s_seq_num);
    Serial.print(F(" len="));    Serial.print(len);
    Serial.print(F(" ok="));     Serial.println(rc == 0);
    return rc;
}

uint8_t mqttsn_transport_recv(uint8_t *buf, uint8_t buf_size, uint32_t timeout_ms) {
    if (!buf) return 0;
    uint32_t deadline = millis() + timeout_ms;
    bool non_blocking = (timeout_ms == 0);

    do {
        if (!s_rf95.available()) {
            if (non_blocking) break;
            continue;
        }
        uint8_t raw_len = LORA_BUF_SIZE;
        if (!s_rf95.recv(s_rx_buf, &raw_len)) continue;
        if (raw_len < MESH_HEADER_SIZE) continue;

        s_last_rssi = s_rf95.lastRssi();

        /* Parse header inline — no struct, no copy, no stack allocation */
        uint8_t src         = s_rx_buf[MESH_OFF_SRC];
        uint8_t dst         = s_rx_buf[MESH_OFF_DST];
        uint8_t ttl         = s_rx_buf[MESH_OFF_TTL];
        uint8_t seq         = s_rx_buf[MESH_OFF_SEQ];
        uint8_t payload_len = raw_len - MESH_HEADER_SIZE;

        Serial.print(F("[rx] src=0x")); Serial.print(src, HEX);
        Serial.print(F(" dst=0x"));     Serial.print(dst, HEX);
        Serial.print(F(" ttl="));       Serial.print(ttl);
        Serial.print(F(" seq="));       Serial.print(seq);
        Serial.print(F(" plen="));      Serial.print(payload_len);
        Serial.print(F(" RSSI="));      Serial.println(s_last_rssi);

        if (dedup_seen(src, seq)) {
            Serial.println(F("[mesh] dedup hit, dropping"));
            continue;
        }

        bool for_me = (dst == LORA_MY_NODE_ID || dst == MESH_ADDR_BROADCAST);
        if (for_me) {
            if (payload_len == 0 || payload_len > buf_size) continue;
            s_last_hop_count = MESH_TTL_DEFAULT - ttl;
            dedup_add(src, seq);
            memcpy(buf, s_rx_buf + MESH_HEADER_SIZE, payload_len);
            return payload_len;
        }

#if MY_NODE_ROLE == NODE_ROLE_RELAY
        if (ttl == 0) { Serial.println(F("[relay] TTL=0, drop")); continue; }
        s_rx_buf[MESH_OFF_TTL] = ttl - 1;  /* decrement TTL in-place */
        dedup_add(src, seq);
        int fwd_ok = radio_tx(s_rx_buf, raw_len);
        Serial.print(F("[relay] fwd src=0x")); Serial.print(src, HEX);
        Serial.print(F(" seq="));              Serial.print(seq);
        Serial.print(F(" ttl_left="));         Serial.print(ttl - 1);
        Serial.print(F(" ok="));               Serial.println(fwd_ok == 0);
        continue;
#else
        Serial.print(F("[rx] not for us (dst=0x"));
        Serial.print(dst, HEX);
        Serial.println(F("), discard"));
#endif

    } while (!non_blocking && millis() < deadline);
    return 0;
}

int16_t mqttsn_transport_last_rssi(void) { return s_last_rssi; }
uint8_t mqttsn_transport_last_hops(void) { return s_last_hop_count; }