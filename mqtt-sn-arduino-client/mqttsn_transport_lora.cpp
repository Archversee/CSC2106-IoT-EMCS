/*
 *   Flooding : broadcasts to all, every relay forwards, dedup prevents loops
 *   Routing  : unicast to specific next hop, less air time, no duplicates
 *
 *   AES-128 ECB encryption on MQTT-SN payload.
 *   No external AES library — tiny inline implementation saves ~600B RAM.
 *   Relay nodes forward ciphertext unchanged (no decrypt needed).
 */

#include "aes_payload.h"
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
#define AES_BUF_SIZE 48 /* max MQTT-SN payload (32B) padded to 48B */

static uint8_t s_rx_buf[LORA_BUF_SIZE];
static uint8_t s_tx_buf[LORA_BUF_SIZE];
static uint8_t s_enc_buf[AES_BUF_SIZE]; /* static encrypt output — no stack use */

static RH_RF95 s_rf95(RFM95_CS, RFM95_INT);
static int16_t s_last_rssi = 0;
static uint8_t s_seq_num = 0;
static uint8_t s_last_hop_count = 0;

/* ── Tiny AES-128 ECB — correct column-major state layout ───────────────── */
/* S-box and rcon in flash (PROGMEM) to save RAM */
static const uint8_t PROGMEM sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};
static const uint8_t PROGMEM rsbox[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d};
static const uint8_t PROGMEM rcon[10] = {0x01, 0x02, 0x04, 0x08, 0x10,
                                         0x20, 0x40, 0x80, 0x1b, 0x36};
#define SB(x) pgm_read_byte(&sbox[(x)])
#define RSB(x) pgm_read_byte(&rsbox[(x)])
#define RC(i) pgm_read_byte(&rcon[(i)])

/* Round keys — 176 bytes static */
static uint8_t s_rk[176];
static bool s_rk_ready = false;

static uint8_t xtime(uint8_t x) { return (x << 1) ^ ((x & 0x80) ? 0x1b : 0); }

static void aes_key_expand(void) {
    memcpy(s_rk, AES_SHARED_KEY, 16);
    for (uint8_t i = 0; i < 10; i++) {
        uint8_t *prev = s_rk + i * 16;
        uint8_t *next = prev + 16;
        /* RotWord + SubWord + Rcon on last word of prev block */
        next[0] = prev[0] ^ SB(prev[13]) ^ RC(i);
        next[1] = prev[1] ^ SB(prev[14]);
        next[2] = prev[2] ^ SB(prev[15]);
        next[3] = prev[3] ^ SB(prev[12]);
        for (uint8_t j = 4; j < 16; j++)
            next[j] = prev[j] ^ next[j - 4];
    }
    s_rk_ready = true;
}

/*
 * AES state is column-major: state[col][row], col=0..3, row=0..3
 * Byte index in the 16-byte block: byte[col*4 + row]
 * This matches the FIPS-197 standard and what OpenSSL produces.
 * s_st is static to avoid stack overflow on Arduino's 2KB RAM.
 */
static uint8_t s_st[16];

static void aes_encrypt_block(uint8_t *blk) {
    if (!s_rk_ready)
        aes_key_expand();
    for (uint8_t i = 0; i < 16; i++)
        s_st[i] = blk[i] ^ s_rk[i];

    for (uint8_t rnd = 1; rnd <= 10; rnd++) {
        for (uint8_t i = 0; i < 16; i++)
            s_st[i] = SB(s_st[i]);
        uint8_t t;
        t = s_st[1];
        s_st[1] = s_st[5];
        s_st[5] = s_st[9];
        s_st[9] = s_st[13];
        s_st[13] = t;
        t = s_st[2];
        s_st[2] = s_st[10];
        s_st[10] = t;
        t = s_st[6];
        s_st[6] = s_st[14];
        s_st[14] = t;
        t = s_st[3];
        s_st[3] = s_st[15];
        s_st[15] = s_st[11];
        s_st[11] = s_st[7];
        s_st[7] = t;
        if (rnd < 10) {
            for (uint8_t c = 0; c < 4; c++) {
                uint8_t *p = s_st + c * 4;
                uint8_t a = p[0], b = p[1], cc = p[2], d = p[3];
                p[0] = xtime(a) ^ xtime(b) ^ b ^ cc ^ d;
                p[1] = a ^ xtime(b) ^ xtime(cc) ^ cc ^ d;
                p[2] = a ^ b ^ xtime(cc) ^ xtime(d) ^ d;
                p[3] = xtime(a) ^ a ^ b ^ cc ^ xtime(d);
            }
        }
        for (uint8_t i = 0; i < 16; i++)
            s_st[i] ^= s_rk[rnd * 16 + i];
    }
    memcpy(blk, s_st, 16);
}

static uint8_t gmul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (b & 1)
            p ^= a;
        bool hi = a & 0x80;
        a <<= 1;
        if (hi)
            a ^= 0x1b;
        b >>= 1;
    }
    return p;
}

static void aes_decrypt_block(uint8_t *blk) {
    if (!s_rk_ready)
        aes_key_expand();
    for (uint8_t i = 0; i < 16; i++)
        s_st[i] = blk[i] ^ s_rk[160 + i];

    for (int8_t rnd = 9; rnd >= 0; rnd--) {
        uint8_t t;
        t = s_st[13];
        s_st[13] = s_st[9];
        s_st[9] = s_st[5];
        s_st[5] = s_st[1];
        s_st[1] = t;
        t = s_st[2];
        s_st[2] = s_st[10];
        s_st[10] = t;
        t = s_st[6];
        s_st[6] = s_st[14];
        s_st[14] = t;
        t = s_st[3];
        s_st[3] = s_st[7];
        s_st[7] = s_st[11];
        s_st[11] = s_st[15];
        s_st[15] = t;
        for (uint8_t i = 0; i < 16; i++)
            s_st[i] = RSB(s_st[i]);
        for (uint8_t i = 0; i < 16; i++)
            s_st[i] ^= s_rk[rnd * 16 + i];
        if (rnd > 0) {
            for (uint8_t c = 0; c < 4; c++) {
                uint8_t *p = s_st + c * 4;
                uint8_t a = p[0], b = p[1], cc = p[2], d = p[3];
                p[0] = gmul(14, a) ^ gmul(11, b) ^ gmul(13, cc) ^ gmul(9, d);
                p[1] = gmul(9, a) ^ gmul(14, b) ^ gmul(11, cc) ^ gmul(13, d);
                p[2] = gmul(13, a) ^ gmul(9, b) ^ gmul(14, cc) ^ gmul(11, d);
                p[3] = gmul(11, a) ^ gmul(13, b) ^ gmul(9, cc) ^ gmul(14, d);
            }
        }
    }
    memcpy(blk, s_st, 16);
}

/* ── AES encrypt/decrypt payload (ECB, PKCS#7) ──────────────────────────── */

static uint8_t aes_encrypt_payload(const uint8_t *src, uint8_t len, uint8_t *dst, uint8_t dst_max) {
    uint8_t pad = 16 - (len % 16);
    uint8_t enc_len = len + pad;
    if (enc_len > dst_max)
        return 0;
    memcpy(dst, src, len);
    memset(dst + len, pad, pad);
    for (uint8_t i = 0; i < enc_len; i += 16)
        aes_encrypt_block(dst + i);
    return enc_len;
}

static uint8_t aes_decrypt_payload(const uint8_t *src, uint8_t len, uint8_t *dst, uint8_t dst_max) {
    if (len == 0 || len % 16 != 0 || len > dst_max)
        return 0;
    memcpy(dst, src, len);
    for (uint8_t i = 0; i < len; i += 16)
        aes_decrypt_block(dst + i);
    uint8_t pad = dst[len - 1];
    if (pad == 0 || pad > 16)
        return 0;
    return len - pad;
}

/* ── Dedup cache ────────────────────────────────────────────────────────── */

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

/* ── Routing table lookup ───────────────────────────────────────────────── */

static uint8_t mesh_next_hop(uint8_t dst) {
#if MESH_MODE == MESH_MODE_ROUTING
    for (uint8_t i = 0; i < ROUTING_TABLE_LEN; i++)
        if (ROUTING_TABLE[i][0] == dst)
            return ROUTING_TABLE[i][1];
#endif
    return dst;
}

/* ── Low-level TX ───────────────────────────────────────────────────────── */

static int radio_tx(const uint8_t *raw, uint8_t raw_len, uint8_t next_hop) {
    if (!raw || raw_len == 0)
        return -1;

    uint32_t t = millis();

    /*
     * CSMA listen-before-talk window.
     * Relay: short window — just check channel is clear before forwarding.
     * Endpoint: short window — per-node jitter in mqttsn_transport_send()
     *           already staggers multiple nodes; CSMA just catches any
     *           residual activity on the channel.
     */
#if MY_NODE_ROLE == NODE_ROLE_RELAY
    uint32_t csma_window = 50;
#else
    uint32_t csma_window = 50;
#endif

    while (millis() - t < csma_window) {
        s_rf95.setModeRx();
        delay(10);
        if (!s_rf95.available())
            break;
        delay(random(10, 50));
    }

    t = millis();
    while (s_rf95.mode() == RHGenericDriver::RHModeTx) {
        if (millis() - t > 500) {
            Serial.println(F("[transport] TX timeout"));
            break;
        }
        delay(5);
    }
#if MESH_MODE == MESH_MODE_ROUTING
    s_rf95.setHeaderTo(next_hop);
#else
    s_rf95.setHeaderTo(RH_BROADCAST_ADDRESS);
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

/* ── Init ───────────────────────────────────────────────────────────────── */

void mqttsn_transport_init(void) {
    memset(s_dedup, 0, sizeof(s_dedup));
    s_seq_num = 0;
    aes_key_expand(); /* pre-expand round keys once at startup */
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
/* Relay nodes must be promiscuous to see downlink packets addressed to
 * other nodes (e.g. CONNACK TO=0x22 forwarded through relay 0x23).
 * Endpoint nodes in routing mode only need their own packets. */
#if MY_NODE_ROLE == NODE_ROLE_RELAY
    s_rf95.setPromiscuous(true);
#else
    s_rf95.setPromiscuous(false);
#endif
#else
    s_rf95.setPromiscuous(true);
#endif

    s_rf95.setThisAddress(LORA_MY_NODE_ID);

    Serial.println(F("[transport] RFM95 ready 915MHz SF7 BW125 CR4/5"));
    Serial.println(F("[transport] AES-128 ECB encryption ENABLED"));
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

/* ── Send (encrypt payload before transmitting) ─────────────────────────── */

int mqttsn_transport_send(const uint8_t *buf, uint8_t len) {
    if (!buf || len == 0)
        return -1;

    /*
     * Per-node jitter delay before every TX.
     * When multiple endpoints receive the same downlink broadcast,
     * they'd all respond simultaneously without this stagger.
     * Node 0x22 → (2 × 10) = 20ms
     * Node 0x24 → (4 × 10) = 40ms
     * Node 0x25 → (5 × 10) = 50ms
     * Relay nodes (0x23) skip this — they just forward, not respond.
     */
#if MY_NODE_ROLE != NODE_ROLE_RELAY
    delay((uint32_t)(LORA_MY_NODE_ID & 0x0F) * 10);
#endif

    /* Encrypt MQTT-SN payload into static buffer — no stack allocation */
    uint8_t enc_len = aes_encrypt_payload(buf, len, s_enc_buf, sizeof(s_enc_buf));
    if (enc_len == 0)
        return -1;

    if ((uint8_t)(MESH_HEADER_SIZE + enc_len) > LORA_BUF_SIZE) {
        Serial.println(F("[transport] send: encrypted frame too large"));
        return -1;
    }

    s_seq_num++;
    dedup_add(LORA_MY_NODE_ID, s_seq_num);

    uint8_t next_hop = mesh_next_hop(MESH_ADDR_GATEWAY);

    s_tx_buf[MESH_OFF_SRC] = LORA_MY_NODE_ID;
    s_tx_buf[MESH_OFF_DST] = MESH_ADDR_GATEWAY;
    s_tx_buf[MESH_OFF_TTL] = MESH_TTL_DEFAULT;
    s_tx_buf[MESH_OFF_SEQ] = s_seq_num;
    memcpy(s_tx_buf + MESH_HEADER_SIZE, s_enc_buf, enc_len);

    int rc = radio_tx(s_tx_buf, MESH_HEADER_SIZE + enc_len, next_hop);
    s_rf95.setModeRx();

    Serial.print(F("[tx] seq="));
    Serial.print(s_seq_num);
    Serial.print(F(" len="));
    Serial.println(enc_len);
    return rc;
}

/* ── Receive (decrypt payload after mesh header stripped) ───────────────── */

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

        if (dedup_seen(src, seq)) {
            Serial.println(F("[mesh] dedup hit, dropping"));
            continue;
        }

        bool for_me = (dst == LORA_MY_NODE_ID || dst == MESH_ADDR_BROADCAST);
        if (for_me) {
            if (payload_len == 0 || payload_len > buf_size)
                continue;
            s_last_hop_count = MESH_TTL_DEFAULT - ttl;
            dedup_add(src, seq);

            /* Decrypt the payload before handing up to MQTT-SN stack */
            uint8_t plain_len =
                aes_decrypt_payload(s_rx_buf + MESH_HEADER_SIZE, payload_len, buf, buf_size);
            if (plain_len == 0) {
                Serial.println(F("[aes] decrypt failed, dropping"));
                continue;
            }
            return plain_len;
        }

        /* Relay / forward — relay nodes pass ciphertext through unchanged */
#if MY_NODE_ROLE == NODE_ROLE_RELAY
        if (ttl == 0) {
            Serial.println(F("[relay] TTL=0, drop"));
            continue;
        }
        /* Clear gateway dedup entries so repeated CONNACKs aren't dropped */
        for (uint8_t i = 0; i < MESH_DEDUP_CACHE_SIZE; i++) {
            if (s_dedup[i].valid && s_dedup[i].src_id == MESH_ADDR_GATEWAY)
                s_dedup[i].valid = false;
        }

#if MESH_MODE == MESH_MODE_ROUTING
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
        Serial.print(F("[rx] not for us (dst=0x"));
        Serial.print(dst, HEX);
        Serial.println(F("), discard"));
#endif

    } while (!non_blocking && millis() < deadline);
    return 0;
}

int16_t mqttsn_transport_last_rssi(void) { return s_last_rssi; }
uint8_t mqttsn_transport_last_hops(void) { return s_last_hop_count; }