/*!
@file    pico-client-combined.c
@brief   FreeRTOS-based MQTT-SN Combined Publisher/Subscriber Client
*/
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
#include <stdio.h>
#include <string.h>

// Global Variables
QueueHandle_t g_mqtt_event_queue;
TaskHandle_t xMQTTTaskHandle = NULL;
TaskHandle_t xButtonTaskHandle = NULL;

// Shared State (protected by mutex)
static uint8_t g_current_qos = QOS_LEVEL_0;
static uint8_t g_current_topic_index = 0; // 0 = sensors/data, 1 = sensors/pico/data
static const char *g_topics[] = {TOPIC_DATA_1, TOPIC_DATA_2};

// Button Task - Handles GPIO inputs
void vButtonTask(void *pvParameters) {
    gpio_init(MESSAGEBUTTON_PIN);
    gpio_set_dir(MESSAGEBUTTON_PIN, GPIO_IN);
    gpio_pull_up(MESSAGEBUTTON_PIN);

    gpio_init(QOSBUTTON_PIN);
    gpio_set_dir(QOSBUTTON_PIN, GPIO_IN);
    gpio_pull_up(QOSBUTTON_PIN);

    gpio_init(TOPICBUTTON_PIN);
    gpio_set_dir(TOPICBUTTON_PIN, GPIO_IN);
    gpio_pull_up(TOPICBUTTON_PIN);

    bool last_msg_btn = true;
    bool last_qos_btn = true;
    bool last_topic_btn = true;

    printf("Button Task Started\n");
    fflush(stdout);

    for (;;) {
        bool current_msg_btn = gpio_get(MESSAGEBUTTON_PIN);
        bool current_qos_btn = gpio_get(QOSBUTTON_PIN);
        bool current_topic_btn = gpio_get(TOPICBUTTON_PIN);

        // GP20: Publish message with current QoS level
        if (last_msg_btn && !current_msg_btn) {
            printf("GP20 pressed - Publishing with QoS %d to %s\n", g_current_qos,
                   g_topics[g_current_topic_index]);
            fflush(stdout);
            mqtt_event_t event = {.type = MQTT_EVENT_PUBLISH,
                                  .param1 = g_current_qos,
                                  .param2 = g_current_topic_index};
            xQueueSend(g_mqtt_event_queue, &event, pdMS_TO_TICKS(10));
        }

        // GP21: Cycle through QoS levels (0 -> 1 -> 2 -> 0)
        if (last_qos_btn && !current_qos_btn) {
            if (xSemaphoreTake(g_mqtt_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                g_current_qos = (g_current_qos + 1) % 3;
                printf("GP21 pressed - QoS level changed to: %d\n", g_current_qos);
                fflush(stdout);
                xSemaphoreGive(g_mqtt_mutex);
            }
        }

        // GP22: Toggle between topics
        if (last_topic_btn && !current_topic_btn) {
            if (xSemaphoreTake(g_mqtt_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                g_current_topic_index = (g_current_topic_index + 1) % 2;
                printf("GP22 pressed - Topic changed to: %s\n", g_topics[g_current_topic_index]);
                fflush(stdout);
                xSemaphoreGive(g_mqtt_mutex);
            }
        }

        last_msg_btn = current_msg_btn;
        last_qos_btn = current_qos_btn;
        last_topic_btn = current_topic_btn;

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// MQTT Task - Handles network operations
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

    // Register Topics for Publishing
    if (xSemaphoreTake(g_mqtt_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        mqtt_sn_add_topic_for_registration(mqtt_ctx, TOPIC_DATA_1);
        mqtt_sn_add_topic_for_registration(mqtt_ctx, TOPIC_DATA_2);
        mqtt_sn_process_topic_registrations(mqtt_ctx, pcb, &gateway_addr, UDP_PORT);
        xSemaphoreGive(g_mqtt_mutex);
    }

    // Wait for topic registration
    TickType_t reg_start = xTaskGetTickCount();
    bool registered = false;
    while (!registered && (xTaskGetTickCount() - reg_start) < pdMS_TO_TICKS(5000)) {
        mqtt_client_poll_network();
        if (xSemaphoreTake(g_mqtt_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            uint16_t topic1_id = mqtt_sn_get_topic_id(mqtt_ctx, TOPIC_DATA_1);
            uint16_t topic2_id = mqtt_sn_get_topic_id(mqtt_ctx, TOPIC_DATA_2);
            if (topic1_id > 0 && topic2_id > 0) {
                registered = true;
            }
            xSemaphoreGive(g_mqtt_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // Subscribe to Command Topics
    if (xSemaphoreTake(g_mqtt_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        mqtt_sn_add_topic_for_subscription(mqtt_ctx, TOPIC_CMD_1, QOS_LEVEL_1);
        mqtt_sn_add_topic_for_subscription(mqtt_ctx, TOPIC_CMD_2, QOS_LEVEL_2);
        mqtt_sn_process_topic_registrations(mqtt_ctx, pcb, &gateway_addr, UDP_PORT);
        xSemaphoreGive(g_mqtt_mutex);
    }

    printf("MQTT Task Ready (Initial QoS: %d, Topic: %s)\n", g_current_qos,
           g_topics[g_current_topic_index]);
    fflush(stdout);

    mqtt_event_t event;
    uint32_t last_ping = xTaskGetTickCount();
    uint32_t last_qos_check = xTaskGetTickCount();

    for (;;) {
        mqtt_client_poll_network();

        // Process events from queue
        if (xQueueReceive(g_mqtt_event_queue, &event, pdMS_TO_TICKS(100)) == pdTRUE) {
            if (xSemaphoreTake(g_mqtt_mutex, pdMS_TO_TICKS(500)) == pdTRUE) {
                switch (event.type) {
                case MQTT_EVENT_PUBLISH: {
                    uint8_t qos_level = (uint8_t)event.param1;
                    uint8_t topic_idx = (uint8_t)event.param2;
                    uint8_t payload[32];

                    static uint32_t msg_counter = 0;
                    snprintf((char *)payload, sizeof(payload), "%s %lu", MQTT_SN_CLIENT_ID,
                             ++msg_counter);

                    uint16_t topic_id = mqtt_sn_get_topic_id(mqtt_ctx, g_topics[topic_idx]);
                    if (topic_id > 0) {
                        mqtt_sn_publish_topic_id_auto(pcb, &gateway_addr, UDP_PORT, topic_id,
                                                      payload, strlen((char *)payload), qos_level);
                        printf("Published: %s to %s (QoS %d)\n", payload, g_topics[topic_idx],
                               qos_level);
                        fflush(stdout);
                    }
                } break;

                default:
                    break;
                }
                xSemaphoreGive(g_mqtt_mutex);
            } else {
                printf("FAILED to take g_mqtt_mutex for event\n");
                fflush(stdout);
            }
        }

        // Handle PING
        uint32_t now = xTaskGetTickCount();
        if ((now - last_ping) > pdMS_TO_TICKS(PING_INTERVAL_MS)) {
            if (xSemaphoreTake(g_mqtt_mutex, pdMS_TO_TICKS(500)) == pdTRUE) {
                mqtt_sn_pingreq(pcb, &gateway_addr, UDP_PORT);
                xSemaphoreGive(g_mqtt_mutex);
            }
            last_ping = now;
        }

        // Check QoS timeouts
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

    // Wait for network to be ready
    sleep_ms(2000);

    // Create Queue
    g_mqtt_event_queue = xQueueCreate(10, sizeof(mqtt_event_t));
    if (g_mqtt_event_queue == NULL) {
        printf("Failed to create queue\n");
        return -1;
    }

    // Create Tasks
    xTaskCreate(vMQTTTask, "MQTT Task", 4096, NULL, 2, &xMQTTTaskHandle);
    xTaskCreate(vButtonTask, "Button Task", 1024, NULL, 1, &xButtonTaskHandle);

    // Start Scheduler
    vTaskStartScheduler();

    return 0;
}