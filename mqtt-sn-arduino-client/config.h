#ifndef CONFIG_H
#define CONFIG_H

/* mesh_protocol.h must be first — defines NODE_ROLE_ENDPOINT/RELAY */
#include "mesh_protocol.h"

/* ── Node identity ────────────────────────────────────────────────────────── */
/* Change these two lines uniquely for each Arduino before flashing.          */
#define LORA_MY_NODE_ID   0x22        /* 0x22 = arduino-02 (source)        */
#define MQTT_SN_CLIENT_ID "arduino-02"  /* must be unique across all devices */
#define LORA_GW_NODE_ID   MESH_ADDR_GATEWAY   /* = 0x00                      */

/* ── Node role ────────────────────────────────────────────────────────────── */
/*  NODE_ROLE_ENDPOINT : leaf — sends/receives own data only.
 *  NODE_ROLE_RELAY    : middle — also floods packets for other nodes.
 *
 *  arduino-02 (source)  → NODE_ROLE_ENDPOINT
 *  arduino-03 (relay)   → NODE_ROLE_RELAY
 */
#define MY_NODE_ROLE  NODE_ROLE_ENDPOINT   /* ← change per device */

/* ── Topics ───────────────────────────────────────────────────────────────── */
#define TOPIC_DATA_1  "sensors/data"
#define TOPIC_DATA_2  "sensors/arduino/data"
#define TOPIC_CMD_1   "sensors/cmd"
#define TOPIC_CMD_2   "sensors/arduino/cmd"

#endif /* CONFIG_H */