/*!
 * @file    pico-client-sub.c
 * @brief   FreeRTOS-based MQTT-SN Subscriber Client (Pure MQTT-SN)
 */

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "config.h"
#include "mqtt-client.h"
#include "mqtt/mqtt-sn-udp.h"
#include "mqtt_task_defs.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

// Global Queue Handle (unused but defined for consistency if needed)
QueueHandle_t g_mqtt_event_queue;

// Task Handles
TaskHandle_t xMQTTTaskHandle = NULL;

// MQTT Task
void vMQTTTask(void* pvParameters) {
    mqtt_sn_context_t* mqtt_ctx;
    struct udp_pcb* pcb;
    ip_addr_t gateway_addr;
    bool fs_initialized;

    // Initialize Network
    if (mqtt_client_network_init((void**)&mqtt_ctx, (void**)&pcb, &gateway_addr, &fs_initialized) != 0) {
        printf("Network init failed\n");
        vTaskDelete(NULL);
    }

    // Subscribe to topics
    if (xSemaphoreTake(g_mqtt_mutex, portMAX_DELAY) == pdTRUE) {
        mqtt_sn_add_topic_for_subscription(mqtt_ctx, "pico/cmd", QOS_LEVEL_2);
        mqtt_sn_add_topic_for_subscription(mqtt_ctx, "pico/status", QOS_LEVEL_1);
        mqtt_sn_process_topic_registrations(mqtt_ctx, pcb, &gateway_addr, UDP_PORT);
        xSemaphoreGive(g_mqtt_mutex);
    }
    vTaskDelay(pdMS_TO_TICKS(2000));

    printf("MQTT Subscriber Ready\n");

    uint32_t last_ping = xTaskGetTickCount();

    for (;;) {
        // Main Loop Delay
        vTaskDelay(pdMS_TO_TICKS(100));

        uint32_t now = xTaskGetTickCount();

        // Handle PING
        if ((now - last_ping) > pdMS_TO_TICKS(PING_INTERVAL_MS)) {
            if (xSemaphoreTake(g_mqtt_mutex, portMAX_DELAY) == pdTRUE) {
                mqtt_sn_pingreq(pcb, &gateway_addr, UDP_PORT);
                xSemaphoreGive(g_mqtt_mutex);
            }
            last_ping = now;
        }

        // Check QoS timeouts
        if (xSemaphoreTake(g_mqtt_mutex, portMAX_DELAY) == pdTRUE) {
            check_qos_timeouts(pcb, &gateway_addr, UDP_PORT);
            xSemaphoreGive(g_mqtt_mutex);
        }
    }
}

int main() {
    stdio_init_all();

    // Create Queue
    g_mqtt_event_queue = xQueueCreate(10, sizeof(mqtt_event_t));

    // Create Tasks
    xTaskCreate(vMQTTTask, "MQTT Task", 4096, NULL, 2, &xMQTTTaskHandle);

    // Start Scheduler
    vTaskStartScheduler();

    return 0;
}
