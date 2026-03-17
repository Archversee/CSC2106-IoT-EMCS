#ifndef MESH_PROTOCOL_H
#define MESH_PROTOCOL_H

/*
 * mesh_protocol.h — Shared LoRa Mesh Protocol Definition
 *
 * THIS FILE IS IDENTICAL on every device:
 *   - Arduino nodes  (mqtt-sn-arduino-client/)
 *   - Raspberry Pi   (Waveshare1121-Gateway/)
 *
 * Intentionally has NO external includes so it compiles cleanly
 * regardless of include order on Arduino or Linux.
 *
 * Wire format — every LoRa frame carries this 4-byte header before the MQTT-SN payload:
 *
 *  +--------+--------+-----+--------+--------------------+
 *  | SRC_ID | DST_ID | TTL | SEQ_NUM| MQTT-SN payload... |
 *  | 1 byte | 1 byte |1 byte| 1 byte|    N bytes         |
 *  +--------+--------+-----+--------+--------------------+
 *
 *  SRC_ID  : Original sender node ID. Never changed by relays.
 *  DST_ID  : Final destination. 0x00=gateway, 0xFF=broadcast, 0x2X=node.
 *  TTL     : Decremented at each relay hop. Packet dropped when it hits 0.
 *  SEQ_NUM : Per-source counter. Relays use (SRC_ID, SEQ_NUM) for dedup.
 */

#include <stdint.h>
#include <string.h>   /* memcpy */

/* ── Well-known addresses ─────────────────────────────────────────────────── */
#define MESH_ADDR_GATEWAY    0x00
#define MESH_ADDR_BROADCAST  0xFF

/* ── Header layout ────────────────────────────────────────────────────────── */
#define MESH_HEADER_SIZE     4      /* SRC + DST + TTL + SEQ              */
#define MESH_OFF_SRC         0
#define MESH_OFF_DST         1
#define MESH_OFF_TTL         2
#define MESH_OFF_SEQ         3

/* ── Mesh parameters ──────────────────────────────────────────────────────── */
#define MESH_TTL_DEFAULT     3      /* starting TTL for every new packet  */
#define MESH_DEDUP_CACHE_SIZE 16     /* ring-buffer size for dedup entries */

/* ── Payload buffer size ──────────────────────────────────────────────────── */
/* RH_RF95_MAX_MESSAGE_LEN = 251 bytes (RadioHead constant).
   We hardcode it here so this header needs no external includes.
   If you change LoRa library, update this value to match.          */
#define MESH_RH_MAX_LEN      251
#define MESH_PAYLOAD_MAX     (MESH_RH_MAX_LEN - MESH_HEADER_SIZE)  /* 247 bytes */

/* ── Node role ────────────────────────────────────────────────────────────── */
#define NODE_ROLE_ENDPOINT   0   /* leaf — only sends/receives own data   */
#define NODE_ROLE_RELAY      1   /* middle — also floods for other nodes  */

/* ── Mesh packet struct ───────────────────────────────────────────────────── */
typedef struct {
    uint8_t src_id;                    /* original source node ID          */
    uint8_t dst_id;                    /* final destination node ID        */
    uint8_t ttl;                       /* hops remaining                   */
    uint8_t seq_num;                   /* originator sequence number       */
    uint8_t payload[MESH_PAYLOAD_MAX]; /* MQTT-SN bytes                    */
    uint8_t payload_len;               /* valid bytes in payload[]         */
} mesh_packet_t;

/* ── Encode ───────────────────────────────────────────────────────────────── */
static inline uint8_t mesh_encode(const mesh_packet_t *pkt,
                                   uint8_t *buf, uint8_t buf_size)
{
    if (!pkt || !buf) return 0;
    uint8_t total = (uint8_t)(MESH_HEADER_SIZE + pkt->payload_len);
    if (total > buf_size) return 0;
    buf[MESH_OFF_SRC] = pkt->src_id;
    buf[MESH_OFF_DST] = pkt->dst_id;
    buf[MESH_OFF_TTL] = pkt->ttl;
    buf[MESH_OFF_SEQ] = pkt->seq_num;
    if (pkt->payload_len > 0)
        memcpy(buf + MESH_HEADER_SIZE, pkt->payload, pkt->payload_len);
    return total;
}

/* ── Decode ───────────────────────────────────────────────────────────────── */
static inline int mesh_decode(const uint8_t *buf, uint8_t len,
                               mesh_packet_t *pkt)
{
    if (!buf || !pkt || len < MESH_HEADER_SIZE) return 0;
    pkt->src_id      = buf[MESH_OFF_SRC];
    pkt->dst_id      = buf[MESH_OFF_DST];
    pkt->ttl         = buf[MESH_OFF_TTL];
    pkt->seq_num     = buf[MESH_OFF_SEQ];
    pkt->payload_len = (uint8_t)(len - MESH_HEADER_SIZE);
    if (pkt->payload_len > 0)
        memcpy(pkt->payload, buf + MESH_HEADER_SIZE, pkt->payload_len);
    return 1;
}

#endif /* MESH_PROTOCOL_H */