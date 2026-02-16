#ifndef MQTT_TASK_DEFS_H
#define MQTT_TASK_DEFS_H

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

// Event types for the MQTT Task
typedef enum {
    MQTT_EVENT_CONNECT,
    MQTT_EVENT_PUBLISH,
    MQTT_EVENT_SUBSCRIBE,
    MQTT_EVENT_BUTTON_PRESS,
    MQTT_EVENT_RECEIVED,
    MQTT_EVENT_TERMINATE
} mqtt_event_type_t;

// Event structure
typedef struct {
    mqtt_event_type_t type;
    void *data;
    uint32_t timestamp;
    uint32_t param1; // Generic parameter
    uint32_t param2; // Generic parameter
} mqtt_event_t;

// Shared Queue Handle
extern QueueHandle_t g_mqtt_event_queue;

#endif // MQTT_TASK_DEFS_H
