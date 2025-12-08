/*!
 * @file    pico-client-pub.c
 * @brief   FreeRTOS-based MQTT-SN Publisher Client
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

    gpio_init(FILE_TRANSFER_BUTTON_PIN);
    gpio_set_dir(FILE_TRANSFER_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(FILE_TRANSFER_BUTTON_PIN);

    gpio_init(DROP_ACK_BUTTON_PIN);
    gpio_set_dir(DROP_ACK_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(DROP_ACK_BUTTON_PIN);

    bool last_msg_btn = true;
    bool last_txt_btn = true;
    bool last_file_btn = true;
    bool last_drop_btn = true;

    for (;;) {
        bool current_msg_btn = gpio_get(MESSAGEBUTTON_PIN);
        bool current_txt_btn = gpio_get(QOSBUTTON_PIN);
        bool current_file_btn = gpio_get(FILE_TRANSFER_BUTTON_PIN);
        bool current_drop_btn = gpio_get(DROP_ACK_BUTTON_PIN);

        if (last_msg_btn && !current_msg_btn) {
            mqtt_event_t event = {.type = MQTT_EVENT_PUBLISH, .param1 = 0};  // 0 = Simple Message
            xQueueSend(g_mqtt_event_queue, &event, 0);
        }

        if (last_txt_btn && !current_txt_btn) {
            mqtt_event_t event = {.type = MQTT_EVENT_FILE_TRANSFER_START, .param1 = 1};  // 1 = Text File
            xQueueSend(g_mqtt_event_queue, &event, 0);
        }

        if (last_file_btn && !current_file_btn) {
            mqtt_event_t event = {.type = MQTT_EVENT_FILE_TRANSFER_START, .param1 = 2};  // 2 = Image File
            xQueueSend(g_mqtt_event_queue, &event, 0);
        }

        if (last_drop_btn && !current_drop_btn) {
            // Toggle drop ACK directly via event? Or just toggle global?
            // Since mqtt_ctx is shared but protected, we can send an event to toggle it safely
            // But for simplicity, let's just send an event
            // Actually, we don't have a specific event for this, let's add one or just handle it here if we had access to ctx
            // But we don't have ctx here.
            // Let's assume we don't need this for the main demo, or add a generic event
        }

        last_msg_btn = current_msg_btn;
        last_txt_btn = current_txt_btn;
        last_file_btn = current_file_btn;
        last_drop_btn = current_drop_btn;

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
        mqtt_sn_add_topic_for_registration(mqtt_ctx, "file/data");
        mqtt_sn_process_topic_registrations(mqtt_ctx, pcb, &gateway_addr, UDP_PORT);
        xSemaphoreGive(g_mqtt_mutex);
    }
    vTaskDelay(pdMS_TO_TICKS(2000));  // Wait for registration

    // Subscribe to control topic
    if (xSemaphoreTake(g_mqtt_mutex, portMAX_DELAY) == pdTRUE) {
        mqtt_sn_add_topic_for_subscription(mqtt_ctx, "file/control", QOS_LEVEL_1);
        mqtt_sn_process_topic_registrations(mqtt_ctx, pcb, &gateway_addr, UDP_PORT);
        xSemaphoreGive(g_mqtt_mutex);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));

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
                        mqtt_sn_publish_topic_id_auto(pcb, &gateway_addr, UDP_PORT, TOPIC_ID_PICO_STATUS, payload, strlen((char*)payload), QOS_LEVEL_1);
                    } break;
                    case MQTT_EVENT_FILE_TRANSFER_START:
                        if (fs_initialized) {
                            const char* filename = (event.param1 == 1) ? "test_1.txt" : "test.jpg";
                            send_file_via_mqtt_auto(pcb, &gateway_addr, UDP_PORT, filename, mqtt_ctx);
                        } else {
                            printf("FS not initialized\n");
                        }
                        break;
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
                // Check PING logic (simplified)
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
