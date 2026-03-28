#ifndef CONFIG_H
#define CONFIG_H

#include "mesh_protocol.h"

/* ═══════════════════════════════════════════════════════════════════════════
 *  HOW TO USE — no editing needed before flashing!
 *
 *  Just set the NODE_SELECT below to match the Arduino you are flashing:
 *
 *    #define NODE_SELECT 2   → arduino-02  (endpoint,  0x22)
 *    #define NODE_SELECT 3   → arduino-03  (relay,     0x23)
 *    #define NODE_SELECT 4   → arduino-04  (endpoint,  0x24)
 *    #define NODE_SELECT 5   → arduino-05  (relay,     0x25)
 *
 *  Then set the mesh mode:
 *    #define MESH_MODE MESH_MODE_FLOODING
 *    #define MESH_MODE MESH_MODE_ROUTING
 *
 *  That's it — select the right COM port and flash!
 * ═══════════════════════════════════════════════════════════════════════════ */

/* ── 1. SELECT NODE ──────────────────────────────────────────────────────── */
#define NODE_SELECT  5        /* ← change to 2, 3, 4, or 5 before flashing  */

/* ── 2. SELECT MODE ──────────────────────────────────────────────────────── */
#define MESH_MODE_FLOODING  0
#define MESH_MODE_ROUTING   1
#define MESH_MODE  MESH_MODE_FLOODING   /* ← MESH_MODE_FLOODING or MESH_MODE_ROUTING */

/* ═══════════════════════════════════════════════════════════════════════════
 *  Everything below is AUTO-CONFIGURED — do not edit
 * ═══════════════════════════════════════════════════════════════════════════ */

/* ── Arduino-02 : endpoint, reaches gateway via relay 0x23 ──────────────── */
#if NODE_SELECT == 2
  #define LORA_MY_NODE_ID    0x22
  #define MQTT_SN_CLIENT_ID  "arduino-02"
  #define MY_NODE_ROLE       NODE_ROLE_ENDPOINT
  static const uint8_t ROUTING_TABLE[][2] = {
      { MESH_ADDR_GATEWAY, 0x23 },   /* gateway via relay 03 */
  };
  #define ROUTING_TABLE_LEN (sizeof(ROUTING_TABLE) / sizeof(ROUTING_TABLE[0]))

/* ── Arduino-03 : relay, direct neighbour of gateway and 02 ─────────────── */
#elif NODE_SELECT == 3
  #define LORA_MY_NODE_ID    0x23
  #define MQTT_SN_CLIENT_ID  "arduino-03"
  #define MY_NODE_ROLE       NODE_ROLE_RELAY
  static const uint8_t ROUTING_TABLE[][2] = {

    { MESH_ADDR_GATEWAY, MESH_ADDR_GATEWAY }, /* gateway is direct neighbour */
    { 0x22, 0x22 }, /* arduino-02 is direct neighbour */

};
  #define ROUTING_TABLE_LEN (sizeof(ROUTING_TABLE) / sizeof(ROUTING_TABLE[0]))

/* ── Arduino-04 : endpoint, reaches gateway via relay 0x23 ──────────────── */
#elif NODE_SELECT == 4
  #define LORA_MY_NODE_ID    0x24
  #define MQTT_SN_CLIENT_ID  "arduino-04"
  #define MY_NODE_ROLE       NODE_ROLE_ENDPOINT
  static const uint8_t ROUTING_TABLE[][2] = {
      { MESH_ADDR_GATEWAY, 0x23 },   /* gateway via relay 03 */
  };
  #define ROUTING_TABLE_LEN (sizeof(ROUTING_TABLE) / sizeof(ROUTING_TABLE[0]))

/* ── Arduino-05 : relay, reaches gateway via relay 0x23 ─────────────────── */
#elif NODE_SELECT == 5
  #define LORA_MY_NODE_ID    0x25
  #define MQTT_SN_CLIENT_ID  "arduino-05"
  #define MY_NODE_ROLE       NODE_ROLE_RELAY
  static const uint8_t ROUTING_TABLE[][2] = {
      { MESH_ADDR_GATEWAY, 0x23 },   /* gateway via relay 03 */
      { 0x22,              0x22 },   /* arduino-02 via relay 03 */
      { 0x24,              0x24 },   /* arduino-04 is direct   */
  };
  #define ROUTING_TABLE_LEN (sizeof(ROUTING_TABLE) / sizeof(ROUTING_TABLE[0]))

#else
  #error "NODE_SELECT must be 2, 3, 4, or 5"
#endif

/* ── Common ──────────────────────────────────────────────────────────────── */
#define LORA_GW_NODE_ID  MESH_ADDR_GATEWAY

/* ── Topics ──────────────────────────────────────────────────────────────── */
#define TOPIC_DATA_1  "sensors/data"
#define TOPIC_DATA_2  "sensors/arduino/data"
#define TOPIC_CMD_1   "sensors/cmd"
#define TOPIC_CMD_2   "sensors/arduino/cmd"

/* ── Compile-time confirmation (shows in Arduino IDE output) ─────────────── */
#pragma message("==============================================")
#if NODE_SELECT == 2
  #pragma message("NODE: arduino-02 | ROLE: ENDPOINT | ID: 0x22")
#elif NODE_SELECT == 3
  #pragma message("NODE: arduino-03 | ROLE: RELAY    | ID: 0x23")
#elif NODE_SELECT == 4
  #pragma message("NODE: arduino-04 | ROLE: ENDPOINT | ID: 0x24")
#elif NODE_SELECT == 5
  #pragma message("NODE: arduino-05 | ROLE: RELAY    | ID: 0x25")
#endif
#if MESH_MODE == MESH_MODE_FLOODING
  #pragma message("MODE: FLOODING")
#else
  #pragma message("MODE: ROUTING")
#endif
#pragma message("==============================================")

#endif /* CONFIG_H */