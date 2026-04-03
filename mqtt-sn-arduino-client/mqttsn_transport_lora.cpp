/*
 * mqttsn_transport_lora.cpp — LoRa mesh transport with STATIC ROUTING
 *
 * Routing mode: each node has a compile-time routing table in config.h.
 * Format: { final_destination, next_hop }
 * The relay looks up the next hop for each packet's DST and forwards
 * to that specific node — no flooding, no broadcast rebroadcast.
 *
 * Compare with flooding mode (previous version) for your performance eval:
 *   Flooding : broadcasts to all, every relay forwards, dedup prevents loops
 *   Routing  : unicast to specific next hop, less air time, no duplicates
 *
 * MESH_MODE must be defined in config.h:
 *   #define MESH_MODE_FLOODING  0
 *   #define MESH_MODE_ROUTING   1
 *   #define MESH_MODE MESH_MODE_ROUTING   // or MESH_MODE_FLOODING
 *
 * RAM budget (same as flooding version):
 *   s_rx_buf[64] + s_tx_buf[64] + dedup[8*3] + misc = ~170 bytes
 */

#include "config.h"
#include "mesh_protocol.h"
#include "mqtt-sn-udp.h"
#include "mqttsn_transport.h"
#include <Arduino.h>
#include <RH_RF95.h>
#include <SPI.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define LORA_BUF_SIZE 64

static uint8_t s_rx_buf[LORA_BUF_SIZE];
static uint8_t s_tx_buf[LORA_BUF_SIZE];

static RH_RF95 s_rf95(RFM95_CS, RFM95_INT);
static int16_t s_last_rssi = 0;
static uint8_t s_seq_num = 0;
static uint8_t s_last_hop_count = 0;

/* ── Dedup cache (used in flooding mode only, harmless in routing) ────────── */
typedef struct {
    uint8_t src_id;
    uint8_t seq_num;
    bool valid;
} dedup_entry_t;
static dedup_entry_t s_dedup[MESH_DEDUP_CACHE_SIZE];
static uint8_t s_dedup_head = 0;

static bool dedup_seen(uint8_t src, uint8_t seq) {
    for (uint8_t i = 0; i < MESH_DEDUP_CACHE_SIZE; i++) {
        if (!s_dedup[i].valid)
            continue;
        if (s_dedup[i].src_id != src)
            continue;
        // Only treat as dupe if seq is within 32 of cached value
        // This prevents old stale entries from blocking new sessions
        int8_t diff = (int8_t)(seq - s_dedup[i].seq_num);
        if (diff == 0)
            return true;
    }
    return false;
}
static void dedup_add(uint8_t src, uint8_t seq) {
    s_dedup[s_dedup_head] = {src, seq, true};
    s_dedup_head = (s_dedup_head + 1) % MESH_DEDUP_CACHE_SIZE;
}

/* ── Routing table lookup ─────────────────────────────────────────────────── */
/*
 * Returns the next-hop node ID for a given final destination.
 * Table is defined in config.h as:
 *   static const uint8_t ROUTING_TABLE[][2] = {
 *       { MESH_ADDR_GATEWAY, 0x23 },  // to reach gateway, go via 0x23
 *   };
 *   #define ROUTING_TABLE_LEN (sizeof(ROUTING_TABLE)/sizeof(ROUTING_TABLE[0]))
 *
 * Returns dst directly if no entry found (attempt direct delivery).
 */
static uint8_t mesh_next_hop(uint8_t dst) {
#if MESH_MODE == MESH_MODE_ROUTING
    for (uint8_t i = 0; i < ROUTING_TABLE_LEN; i++)
        if (ROUTING_TABLE[i][0] == dst)
            return ROUTING_TABLE[i][1];
#endif
    return dst; /* no entry or flooding mode — use dst as next hop */
}

/* ── Low-level TX ─────────────────────────────────────────────────────────── */
/*
 * In ROUTING mode:  sets RadioHead TO = next_hop (unicast).
 * In FLOODING mode: sets RadioHead TO = RH_BROADCAST_ADDRESS.
 * next_hop parameter is ignored in flooding mode.
 */
static int radio_tx(const uint8_t *raw, uint8_t raw_len, uint8_t next_hop) {
    if (!raw || raw_len == 0)
        return -1;

    uint32_t t = millis();

#if MY_NODE_ROLE == NODE_ROLE_RELAY
    // Relay: shorter CSMA window so we don't miss incoming packets
    uint32_t csma_window = 50;
#else
    // Endpoint: longer window to avoid colliding with relay rebroadcast
    uint32_t csma_window = 200;
#endif

    while (millis() - t < csma_window) {
        s_rf95.setModeRx();
        delay(10);
        if (!s_rf95.available())
            break;
        delay(random(10, 50));
    }

    // reuse t for TX timeout
    t = millis();
    while (s_rf95.mode() == RHGenericDriver::RHModeTx) {
        if (millis() - t > 500) {
            Serial.println(F("[transport] TX timeout"));
            break;
        }
        delay(5);
    }
#if MESH_MODE == MESH_MODE_ROUTING
    s_rf95.setHeaderTo(next_hop); /* unicast to specific next hop */
#else
    s_rf95.setHeaderTo(RH_BROADCAST_ADDRESS); /* flood to all neighbours    */
#endif
    s_rf95.setHeaderFrom(LORA_MY_NODE_ID);
    s_rf95.setHeaderFlags(0x00, 0xFF);
    s_rf95.setModeIdle();
    delay(5);
    bool ok = s_rf95.send(raw, raw_len);
    s_rf95.waitPacketSent();
    s_rf95.setModeRx();
    return ok ? 0 : -1;
}

/* ── Init ─────────────────────────────────────────────────────────────────── */
void mqttsn_transport_init(void) {
    memset(s_dedup, 0, sizeof(s_dedup));
    s_seq_num = 0;
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
    s_rf95.setSpreadingFactor(7);
    s_rf95.setSignalBandwidth(125000);
    s_rf95.setCodingRate4(5);
    s_rf95.setTxPower(13, false);
    s_rf95.setHeaderFlags(0x00, 0xFF);

#if MESH_MODE == MESH_MODE_ROUTING
    /* Routing: only accept packets addressed to this node or broadcast.
     * RadioHead's built-in address filter handles this automatically
     * when promiscuous mode is OFF. */
    s_rf95.setPromiscuous(false);
#else
    /* Flooding: must hear all packets to decide whether to relay them */
    s_rf95.setPromiscuous(true);
#endif

    s_rf95.setThisAddress(LORA_MY_NODE_ID);

    Serial.println(F("[transport] RFM95 ready 915MHz SF7 BW125 CR4/5"));
    Serial.print(F("[transport] Node ID : 0x"));
    Serial.println(LORA_MY_NODE_ID, HEX);
    Serial.print(F("[transport] Role    : "));
    Serial.println(MY_NODE_ROLE == NODE_ROLE_RELAY ? F("RELAY") : F("ENDPOINT"));
    Serial.print(F("[transport] Mode    : "));
#if MESH_MODE == MESH_MODE_ROUTING
    Serial.println(F("STATIC ROUTING"));
#else
    Serial.println(F("FLOODING"));
#endif
    s_rf95.setModeRx();
}

/* ── Send ─────────────────────────────────────────────────────────────────── */
int mqttsn_transport_send(const uint8_t *buf, uint8_t len) {
    if (!buf || len == 0)
        return -1;
    if ((uint8_t)(MESH_HEADER_SIZE + len) > LORA_BUF_SIZE) {
        Serial.println(F("[transport] send: too large"));
        return -1;
    }

    s_seq_num++;
    dedup_add(LORA_MY_NODE_ID, s_seq_num);

    /* In routing mode: DST field carries the FINAL destination (gateway).
     * The RadioHead TO header carries the NEXT HOP.
     * The relay reads DST to know where to forward, reads TO to know
     * the packet is meant for it to relay. */
    uint8_t next_hop = mesh_next_hop(MESH_ADDR_GATEWAY);

    s_tx_buf[MESH_OFF_SRC] = LORA_MY_NODE_ID;
    s_tx_buf[MESH_OFF_DST] = MESH_ADDR_GATEWAY; /* final destination always gateway */
    s_tx_buf[MESH_OFF_TTL] = MESH_TTL_DEFAULT;
    s_tx_buf[MESH_OFF_SEQ] = s_seq_num;
    memcpy(s_tx_buf + MESH_HEADER_SIZE, buf, len);

    int rc = radio_tx(s_tx_buf, MESH_HEADER_SIZE + len, next_hop);

    // Give relay time to rebroadcast and stay in RX to catch the echo
    s_rf95.setModeRx();

    Serial.print(F("[tx] seq="));
    Serial.print(s_seq_num);
    Serial.print(F(" dst=0x"));
    Serial.print(MESH_ADDR_GATEWAY, HEX);
    Serial.print(F(" next_hop=0x"));
    Serial.print(next_hop, HEX);
    Serial.print(F(" len="));
    Serial.print(len);
    Serial.print(F(" ok="));
    Serial.println(rc == 0);
    return rc;
}

/* ── Receive ──────────────────────────────────────────────────────────────── */
uint8_t mqttsn_transport_recv(uint8_t *buf, uint8_t buf_size, uint32_t timeout_ms) {
    if (!buf)
        return 0;
    uint32_t deadline = millis() + timeout_ms;
    bool non_blocking = (timeout_ms == 0);

    do {
        if (!s_rf95.available()) {
            if (non_blocking)
                break;
            continue;
        }
        uint8_t raw_len = LORA_BUF_SIZE;
        if (!s_rf95.recv(s_rx_buf, &raw_len))
            continue;
        if (raw_len < MESH_HEADER_SIZE)
            continue;

        s_last_rssi = s_rf95.lastRssi();

        uint8_t src = s_rx_buf[MESH_OFF_SRC];
        uint8_t dst = s_rx_buf[MESH_OFF_DST];
        uint8_t ttl = s_rx_buf[MESH_OFF_TTL];
        uint8_t seq = s_rx_buf[MESH_OFF_SEQ];
        uint8_t payload_len = raw_len - MESH_HEADER_SIZE;

        Serial.print(F("[rx] src=0x"));
        Serial.print(src, HEX);
        Serial.print(F(" dst=0x"));
        Serial.print(dst, HEX);
        Serial.print(F(" ttl="));
        Serial.print(ttl);
        Serial.print(F(" seq="));
        Serial.print(seq);
        Serial.print(F(" plen="));
        Serial.print(payload_len);
        Serial.print(F(" RSSI="));
        Serial.println(s_last_rssi);

        /* Dedup check (important for flooding, harmless for routing) */
        if (dedup_seen(src, seq)) {
            Serial.println(F("[mesh] dedup hit, dropping"));
            continue;
        }

        /* ── Deliver if addressed to us ───────────────────────────────── */
        bool for_me = (dst == LORA_MY_NODE_ID || dst == MESH_ADDR_BROADCAST);
        if (for_me) {
            if (payload_len == 0 || payload_len > buf_size)
                continue;
            s_last_hop_count = MESH_TTL_DEFAULT - ttl;
            dedup_add(src, seq);
            memcpy(buf, s_rx_buf + MESH_HEADER_SIZE, payload_len);
            return payload_len;
        }

        /* ── Relay / forward ──────────────────────────────────────────── */
#if MY_NODE_ROLE == NODE_ROLE_RELAY
        if (ttl == 0) {
            Serial.println(F("[relay] TTL=0, drop"));
            continue;
        }

#if MESH_MODE == MESH_MODE_ROUTING
        /* ROUTING: look up next hop toward DST, forward unicast */
        uint8_t next_hop = mesh_next_hop(dst);
        s_rx_buf[MESH_OFF_TTL] = ttl - 1;
        dedup_add(src, seq);
        int fwd_ok = radio_tx(s_rx_buf, raw_len, next_hop);
        Serial.print(F("[relay] route src=0x"));
        Serial.print(src, HEX);
        Serial.print(F(" dst=0x"));
        Serial.print(dst, HEX);
        Serial.print(F(" next=0x"));
        Serial.print(next_hop, HEX);
        Serial.print(F(" seq="));
        Serial.print(seq);
        Serial.print(F(" ok="));
        Serial.println(fwd_ok == 0);
#else
        /* FLOODING: decrement TTL, rebroadcast to all neighbours */
        s_rx_buf[MESH_OFF_TTL] = ttl - 1;
        dedup_add(src, seq);
        delay(random(20, 80));
        int fwd_ok = radio_tx(s_rx_buf, raw_len, RH_BROADCAST_ADDRESS);
        Serial.print(F("[relay] flood src=0x"));
        Serial.print(src, HEX);
        Serial.print(F(" seq="));
        Serial.print(seq);
        Serial.print(F(" ttl_left="));
        Serial.print(ttl - 1);
        Serial.print(F(" ok="));
        Serial.println(fwd_ok == 0);
#endif
        continue;

#else
        /* Endpoint node: packet not for us */
        Serial.print(F("[rx] not for us (dst=0x"));
        Serial.print(dst, HEX);
        Serial.println(F("), discard"));
#endif

    } while (!non_blocking && millis() < deadline);
    return 0;
}

int16_t mqttsn_transport_last_rssi(void) { return s_last_rssi; }
uint8_t mqttsn_transport_last_hops(void) { return s_last_hop_count; }