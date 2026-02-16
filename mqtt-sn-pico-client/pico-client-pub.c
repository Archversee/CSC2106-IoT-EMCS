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

// Global QoS level (shared between tasks)
static uint8_t g_current_qos = QOS_LEVEL_0;

// Button Task
void vButtonTask(void *pvParameters) {
    gpio_init(MESSAGEBUTTON_PIN);
    gpio_set_dir(MESSAGEBUTTON_PIN, GPIO_IN);
    gpio_pull_up(MESSAGEBUTTON_PIN);

    gpio_init(QOSBUTTON_PIN);
    gpio_set_dir(QOSBUTTON_PIN, GPIO_IN);
    gpio_pull_up(QOSBUTTON_PIN);

    bool last_msg_btn = true;
    bool last_qos_btn = true;
    uint32_t loop_count = 0;

    fflush(stdout);

    for (;;) {
        bool current_msg_btn = gpio_get(MESSAGEBUTTON_PIN);
        bool current_qos_btn = gpio_get(QOSBUTTON_PIN);

        // GP20: Publish message with current QoS level
        if (last_msg_btn && !current_msg_btn) {
            printf("GP20 pressed - Publishing with QoS %d\n", g_current_qos);
            fflush(stdout);
            mqtt_event_t event = {.type = MQTT_EVENT_PUBLISH, .param1 = g_current_qos};
            xQueueSend(g_mqtt_event_queue, &event, pdMS_TO_TICKS(10));
        }

        // GP21: Cycle through QoS levels (0 -> 1 -> 2 -> 0)
        if (last_qos_btn && !current_qos_btn) {
            g_current_qos = (g_current_qos + 1) % 3;
            printf("GP21 pressed - QoS level changed to: %d\n", g_current_qos);
            fflush(stdout);
        }

        last_msg_btn = current_msg_btn;
        last_qos_btn = current_qos_btn;

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// MQTT Task
void vMQTTTask(void *pvParameters) {
    mqtt_sn_context_t *mqtt_ctx;
    struct udp_pcb *pcb;
    ip_addr_t gateway_addr;
    bool fs_initialized;

    printf("MQTT Task Started\n");
    fflush(stdout);

    // Initialize Network
    if (mqtt_client_network_init((void **)&mqtt_ctx, (void **)&pcb, &gateway_addr,
                                 &fs_initialized) != 0) {
        printf("Network init failed\n");
        fflush(stdout);
        vTaskDelete(NULL);
    }

    // Register Topics
    if (xSemaphoreTake(g_mqtt_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        mqtt_sn_add_topic_for_registration(mqtt_ctx, "pico/cmd");
        mqtt_sn_add_topic_for_registration(mqtt_ctx, "pico/status");
        mqtt_sn_process_topic_registrations(mqtt_ctx, pcb, &gateway_addr, UDP_PORT);
        xSemaphoreGive(g_mqtt_mutex);
    }

    TickType_t reg_start = xTaskGetTickCount();
    bool registered = false;
    while (!registered && (xTaskGetTickCount() - reg_start) < pdMS_TO_TICKS(5000)) {
        mqtt_client_poll_network();
        if (xSemaphoreTake(g_mqtt_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            uint16_t cmd_id = mqtt_sn_get_topic_id(mqtt_ctx, "pico/cmd");
            uint16_t status_id = mqtt_sn_get_topic_id(mqtt_ctx, "pico/status");
            if (cmd_id > 0 && status_id > 0) {
                registered = true;
            }
            xSemaphoreGive(g_mqtt_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    printf("MQTT Task Ready (Initial QoS: %d)\n", g_current_qos);
    fflush(stdout);

    mqtt_event_t event;
    uint32_t last_ping = xTaskGetTickCount();
    uint32_t last_qos_check = xTaskGetTickCount();
    uint32_t loop_count = 0;

    for (;;) {
        mqtt_client_poll_network();

        // Wait for event with timeout
        if (xQueueReceive(g_mqtt_event_queue, &event, pdMS_TO_TICKS(100)) == pdTRUE) {
            fflush(stdout);
            if (xSemaphoreTake(g_mqtt_mutex, pdMS_TO_TICKS(500)) == pdTRUE) {
                printf("Got event type=%d\n", event.type);
                fflush(stdout);
                switch (event.type) {
                case MQTT_EVENT_PUBLISH: {
                    uint8_t qos_level = (uint8_t)event.param1;
                    uint8_t payload[20] = "Hello FreeRTOS";
                    uint16_t topic_id = mqtt_sn_get_topic_id(mqtt_ctx, "pico/status");
                    if (topic_id > 0) {
                        mqtt_sn_publish_topic_id_auto(pcb, &gateway_addr, UDP_PORT, topic_id,
                                                      payload, strlen((char *)payload), qos_level);
                        printf("Published message with QoS %d!\n", qos_level);
                        fflush(stdout);
                    }
                } break;
                default:
                    break;
                }
                xSemaphoreGive(g_mqtt_mutex);
            } else {
                printf("FAILED to take g_mqtt_mutex\n");
                fflush(stdout);
            }
        }

        // Handle PING
        uint32_t now = xTaskGetTickCount();
        if ((now - last_ping) > pdMS_TO_TICKS(PING_INTERVAL_MS)) {
            fflush(stdout);
            fflush(stdout);
            if (xSemaphoreTake(g_mqtt_mutex, pdMS_TO_TICKS(500)) == pdTRUE) {
                mqtt_sn_pingreq(pcb, &gateway_addr, UDP_PORT);
                xSemaphoreGive(g_mqtt_mutex);
            } else {
                printf("FAILED to take g_mqtt_mutex for PINGREQ\n");
                fflush(stdout);
            }
            last_ping = now;
        }

        // Check QoS timeouts (less frequently)
        if ((now - last_qos_check) > pdMS_TO_TICKS(5000)) {
            if (xSemaphoreTake(g_mqtt_mutex, pdMS_TO_TICKS(500)) == pdTRUE) {
                check_qos_timeouts(pcb, &gateway_addr, UDP_PORT);
                xSemaphoreGive(g_mqtt_mutex);
            }
            last_qos_check = now;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
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