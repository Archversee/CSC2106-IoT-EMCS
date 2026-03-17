#ifndef CONFIG_H
#define CONFIG_H

#include "mesh_protocol.h"

/* ── Node identity ────────────────────────────────────────────────────────── */
#define LORA_MY_NODE_ID   0x22
#define MQTT_SN_CLIENT_ID "arduino-02"
#define LORA_GW_NODE_ID   MESH_ADDR_GATEWAY

/* ── Node role ────────────────────────────────────────────────────────────── */
#define MY_NODE_ROLE  NODE_ROLE_ENDPOINT

/* ── Mesh mode ────────────────────────────────────────────────────────────── */
#define MESH_MODE_FLOODING  0
#define MESH_MODE_ROUTING   1
#define MESH_MODE  MESH_MODE_FLOODING     /* ← must match arduino-02 */

/* ── Static routing table (used when MESH_MODE == MESH_MODE_ROUTING) ──────── */
/*  Arduino-03 (relay, 0x23) can reach both the gateway and arduino-02 directly.
 *
 *  Format: { final_destination, next_hop }                          */
static const uint8_t ROUTING_TABLE[][2] = {
    { MESH_ADDR_GATEWAY, MESH_ADDR_GATEWAY }, /* gateway is direct neighbour */
    { 0x22,              0x22              }, /* arduino-02 is direct neighbour */
};
#define ROUTING_TABLE_LEN (sizeof(ROUTING_TABLE) / sizeof(ROUTING_TABLE[0]))

/* ── Topics ───────────────────────────────────────────────────────────────── */
#define TOPIC_DATA_1  "sensors/data"
#define TOPIC_DATA_2  "sensors/arduino/data"
#define TOPIC_CMD_1   "sensors/cmd"
#define TOPIC_CMD_2   "sensors/arduino/cmd"

#endif /* CONFIG_H */