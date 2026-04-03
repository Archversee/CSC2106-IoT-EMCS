#ifndef CONFIG_H
#define CONFIG_H

#include "mesh_protocol.h"

/* 1. SELECT NODE */
#define NODE_SELECT 4

/* 2. SELECT MODE */
#define MESH_MODE_FLOODING 0
#define MESH_MODE_ROUTING 1
// #define MESH_MODE MESH_MODE_FLOODING
#define MESH_MODE MESH_MODE_ROUTING

/* Arduino-02 */
#if NODE_SELECT == 2
#define LORA_MY_NODE_ID 0x22
#define MQTT_SN_CLIENT_ID "arduino-02"
#define MY_NODE_ROLE NODE_ROLE_ENDPOINT
static const uint8_t ROUTING_TABLE[][2] = {
    {MESH_ADDR_GATEWAY, 0x23},
};
#define ROUTING_TABLE_LEN (sizeof(ROUTING_TABLE) / sizeof(ROUTING_TABLE[0]))

#elif NODE_SELECT == 3
#define LORA_MY_NODE_ID 0x23
#define MQTT_SN_CLIENT_ID "arduino-03"
#define MY_NODE_ROLE NODE_ROLE_RELAY
static const uint8_t ROUTING_TABLE[][2] = {
    {MESH_ADDR_GATEWAY, MESH_ADDR_GATEWAY},
    {0x22, 0x22},
};
#define ROUTING_TABLE_LEN (sizeof(ROUTING_TABLE) / sizeof(ROUTING_TABLE[0]))

#elif NODE_SELECT == 4
#define LORA_MY_NODE_ID 0x24
#define MQTT_SN_CLIENT_ID "arduino-04"
#define MY_NODE_ROLE NODE_ROLE_ENDPOINT
static const uint8_t ROUTING_TABLE[][2] = {
    {MESH_ADDR_GATEWAY, 0x23},
};
#define ROUTING_TABLE_LEN (sizeof(ROUTING_TABLE) / sizeof(ROUTING_TABLE[0]))

#elif NODE_SELECT == 5
#define LORA_MY_NODE_ID 0x25
#define MQTT_SN_CLIENT_ID "arduino-05"
#define MY_NODE_ROLE NODE_ROLE_RELAY
static const uint8_t ROUTING_TABLE[][2] = {
    {MESH_ADDR_GATEWAY, 0x23},
    {0x22, 0x22},
    {0x24, 0x24},
};
#define ROUTING_TABLE_LEN (sizeof(ROUTING_TABLE) / sizeof(ROUTING_TABLE[0]))

#else
#error "NODE_SELECT must be 2, 3, 4, or 5"
#endif

/* RELAY_ONLY */
#if MY_NODE_ROLE == NODE_ROLE_RELAY
#define MESH_RELAY_ONLY 1
#else
#define MESH_RELAY_ONLY 0
#endif

/* Common */
#define LORA_GW_NODE_ID MESH_ADDR_GATEWAY

/* Topics */
#define TOPIC_DATA_1 "sensors/data"
#define TOPIC_DATA_2 "sensors/arduino/data"
#define TOPIC_CMD_1 "sensors/cmd"
#define TOPIC_CMD_2 "sensors/arduino/cmd"

#endif /* CONFIG_H */