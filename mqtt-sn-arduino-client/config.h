#ifndef CONFIG_H
#define CONFIG_H

#define LORA_MY_NODE_ID 0x21// Change to "0x2X" for each node to get a unique LoRa node ID
#define MQTT_SN_CLIENT_ID                                                                          \
    "arduino-02" // Change to "arduino-XX" for each node to get a unique MQTT-SN client ID
#define LORA_GW_NODE_ID 0x00 // RPI bridge ID dont change

// Topic names
#define TOPIC_DATA_1 "sensors/data"
#define TOPIC_DATA_2 "sensors/arduino/data"
#define TOPIC_CMD_1 "sensors/cmd"
#define TOPIC_CMD_2 "sensors/arduino/cmd"

#endif