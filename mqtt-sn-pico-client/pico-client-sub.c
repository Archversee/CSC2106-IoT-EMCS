/*!
 * @file    pico-client-sub.c
 * @brief   FreeRTOS-based MQTT-SN Subscriber Client
 */

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "config.h"
#include "drivers/microsd_driver.h"
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
        if (fs_initialized) {
            mqtt_sn_add_topic_for_subscription(mqtt_ctx, "file/data", QOS_LEVEL_1);
        }
        mqtt_sn_process_topic_registrations(mqtt_ctx, pcb, &gateway_addr, UDP_PORT);

        // Register topics for publishing (control)
        if (fs_initialized) {
            mqtt_sn_add_topic_for_registration(mqtt_ctx, "file/control");
        }

        xSemaphoreGive(g_mqtt_mutex);
    }
    vTaskDelay(pdMS_TO_TICKS(2000));

    printf("MQTT Subscriber Ready\n");

    uint32_t last_ping = xTaskGetTickCount();
    uint32_t last_sd_check = xTaskGetTickCount();
    bool sd_was_initialized = fs_initialized;

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

        // Handle SD Card Hotplug
        if ((now - last_sd_check) > pdMS_TO_TICKS(SD_CHECK_INTERVAL_MS)) {
            last_sd_check = now;
            if (xSemaphoreTake(g_mqtt_mutex, portMAX_DELAY) == pdTRUE) {
                if (fs_initialized) {
                    if (!mqtt_client_check_microsd_present()) {
                        printf("SD Card Removed\n");
                        fs_initialized = false;
                        sd_was_initialized = false;
                        mqtt_ctx->transfer_in_progress = false;
                    }
                } else if (!sd_was_initialized) {
                    if (mqtt_client_initialize_microsd(1, false)) {
                        printf("SD Card Inserted\n");
                        fs_initialized = true;
                        sd_was_initialized = true;
                        mqtt_sn_add_topic_for_subscription(mqtt_ctx, "file/data", QOS_LEVEL_1);
                        // Note: We should probably process registrations again, but for now just add to queue
                    }
                }
                xSemaphoreGive(g_mqtt_mutex);
            }
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
