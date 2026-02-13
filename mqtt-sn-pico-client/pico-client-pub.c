/*!
 * @file    pico-client-pub.c
 * @brief   FreeRTOS-based MQTT-SN Publisher Client (Pure MQTT-SN)
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

// Global Queue Handle
QueueHandle_t g_mqtt_event_queue;

// Task Handles
TaskHandle_t xMQTTTaskHandle = NULL;
TaskHandle_t xButtonTaskHandle = NULL;

// Button Task
void vButtonTask(void* pvParameters) {
    // Initialize Buttons
    gpio_init(MESSAGEBUTTON_PIN);
    gpio_set_dir(MESSAGEBUTTON_PIN, GPIO_IN);
    gpio_pull_up(MESSAGEBUTTON_PIN);

    gpio_init(QOSBUTTON_PIN);
    gpio_set_dir(QOSBUTTON_PIN, GPIO_IN);
    gpio_pull_up(QOSBUTTON_PIN);

    bool last_msg_btn = true;
    bool last_qos_btn = true;

    for (;;) {
        bool current_msg_btn = gpio_get(MESSAGEBUTTON_PIN);
        bool current_qos_btn = gpio_get(QOSBUTTON_PIN);

        if (last_msg_btn && !current_msg_btn) {
            mqtt_event_t event = {.type = MQTT_EVENT_PUBLISH, .param1 = 0};  // 0 = Simple Message
            xQueueSend(g_mqtt_event_queue, &event, 0);
        }

        if (last_qos_btn && !current_qos_btn) {
            mqtt_event_t event = {.type = MQTT_EVENT_PUBLISH, .param1 = 1};  // 1 = QoS test
            xQueueSend(g_mqtt_event_queue, &event, 0);
        }

        last_msg_btn = current_msg_btn;
        last_qos_btn = current_qos_btn;

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

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

    // Register Topics
    if (xSemaphoreTake(g_mqtt_mutex, portMAX_DELAY) == pdTRUE) {
        mqtt_sn_add_topic_for_registration(mqtt_ctx, "pico/cmd");
        mqtt_sn_add_topic_for_registration(mqtt_ctx, "pico/status");
        mqtt_sn_process_topic_registrations(mqtt_ctx, pcb, &gateway_addr, UDP_PORT);
        xSemaphoreGive(g_mqtt_mutex);
    }
    vTaskDelay(pdMS_TO_TICKS(2000));  // Wait for registration

    printf("MQTT Task Ready\n");

    mqtt_event_t event;
    uint32_t last_ping = xTaskGetTickCount();

    for (;;) {
        // Wait for event with timeout (for PING)
        if (xQueueReceive(g_mqtt_event_queue, &event, pdMS_TO_TICKS(100)) == pdTRUE) {
            if (xSemaphoreTake(g_mqtt_mutex, portMAX_DELAY) == pdTRUE) {
                switch (event.type) {
                    case MQTT_EVENT_PUBLISH: {
                        uint8_t payload[20] = "Hello FreeRTOS";
                        uint16_t topic_id = mqtt_sn_get_topic_id(mqtt_ctx, "pico/status");
                        if (topic_id > 0) {
                            mqtt_sn_publish_topic_id_auto(pcb, &gateway_addr, UDP_PORT, topic_id, 
                                                         payload, strlen((char*)payload), QOS_LEVEL_1);
                        }
                    } break;
                    default:
                        break;
                }
                xSemaphoreGive(g_mqtt_mutex);
            }
        }

        // Handle PING and Maintenance
        uint32_t now = xTaskGetTickCount();
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
    xTaskCreate(vButtonTask, "Button Task", 1024, NULL, 1, &xButtonTaskHandle);

    // Start Scheduler
    vTaskStartScheduler();

    return 0;
}
