#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include "config.h" 
#include "functions.h" 
#include "mqttsn_bridge.h"

#ifndef DEVICE_ID
#define DEVICE_ID "pico-001" 
#endif


uint32_t last_pingresp = 0;
static uint32_t last_pingreq = 0;
bool ping_ack_received = true;

int main() {
    
    stdio_init_all();

    // Initialize Message button GP20
    gpio_init(MESSAGEBUTTON_PIN);
    gpio_set_dir(MESSAGEBUTTON_PIN, GPIO_IN);
    gpio_pull_up(MESSAGEBUTTON_PIN);

    // Initialize QoS toggle button GP21
    gpio_init(QOSBUTTON_PIN);
    gpio_set_dir(QOSBUTTON_PIN, GPIO_IN);
    gpio_pull_up(QOSBUTTON_PIN); 

    // Initialize Drop ACK toggle button GP22
    gpio_init(DROP_ACK_BUTTON_PIN);
    gpio_set_dir(DROP_ACK_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(DROP_ACK_BUTTON_PIN);

    mqtt_sn_context_t mqtt_ctx = {
        .drop_acks = false
    };
    
    int qos_level = 0; 

    // Initialize Wi-Fi
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    cyw43_arch_enable_sta_mode();

    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Wi-Fi connect failed. Retrying in 5 seconds...\n");
        sleep_ms(5000);
    }

    printf("Wi-Fi connected. IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));

    // Setup UDP
    struct udp_pcb *pcb = udp_new();
    if (!pcb) {
        printf("UDP setup failed\n");
        return -1;
    }
    if (udp_bind(pcb, IP_ADDR_ANY, 0) != ERR_OK) {
        printf("UDP bind failed\n");
        return -1;
    }
    udp_recv(pcb, udp_recv_callback, &mqtt_ctx);

    printf("UDP client ready...\n");

    // setup MQTT-SN Gateway address
    ip_addr_t gateway_addr;
    IP4_ADDR(&gateway_addr, GATEWAY_IP0, GATEWAY_IP1, GATEWAY_IP2, GATEWAY_IP3); //RMB to change to your gateway IP

    mqttsn_bridge_init(pcb, &gateway_addr, UDP_PORT);

    sleep_ms(1000);

    // Connect to MQTT-SN Gateway
    mqtt_sn_connect(pcb, &gateway_addr, UDP_PORT);
    for (int i = 0; i < 10; i++) {
        cyw43_arch_poll();
        sleep_ms(10);
    }
    printf("Waiting for CONNACK...\n");
    for (int i = 0; i < 50; i++) {
        cyw43_arch_poll();
        sleep_ms(100);
    }
    sleep_ms(1000);
    
    // Subscribe to topic ID 1 (predefined topic "pico/cmd") fefault QoS 2 subscription - can still receive < QoS <2 messages
    printf("Subscribing to 'pico/cmd'...\n");
    mqtt_sn_subscribe_topic_id(pcb, &gateway_addr, UDP_PORT, 1);
    for (int i = 0; i < 10; i++) {
        cyw43_arch_poll();
        sleep_ms(10);
    }

    uint32_t last_ping = to_ms_since_boot(get_absolute_time());
    bool last_button_state = gpio_get(MESSAGEBUTTON_PIN);
    bool last_qos_button = gpio_get(QOSBUTTON_PIN);
    bool last_drop_button = gpio_get(DROP_ACK_BUTTON_PIN);

    // Test binary payload MAX SIXZE 247 for QoS 1
    uint8_t payload[PAYLOAD_SIZE];
    // Fill payload with example binary data: 0x00, 0x01, 0x02, ..., 0xFE
    for (int i = 0; i < PAYLOAD_SIZE; i++) {
        payload[i] = i & 0xFF;
    }

    // Main loop
    while (true) {
        cyw43_arch_poll();

        // Check message button press (active LOW)
        bool current_button = gpio_get(MESSAGEBUTTON_PIN);
        if (last_button_state && !current_button) {
            // GP 20 Button pressed (falling edge)
            printf("Button pressed! Publishing message...\n");
            uint16_t id = get_next_msg_id();
            // Publish to topic ID 2 (predefined topic "pico/status") with selected QoS
            mqtt_sn_publish_topic_id(pcb, &gateway_addr, UDP_PORT, 2, payload, PAYLOAD_SIZE, qos_level, id, false);
            sleep_ms(200); // Debounce
        }
        last_button_state = current_button;

        // Check Qos button press (active LOW)
        bool current_qos_button = gpio_get(QOSBUTTON_PIN);
        if (last_qos_button && !current_qos_button) {
            // GP 21 Button pressed (falling edge)
            qos_level++;
            if (qos_level > 2) qos_level = 0; // Wrap around 0->1->2->0
            printf("QoS level changed to: %d\n", qos_level);
            sleep_ms(200); // Debounce
        }
        last_qos_button = current_qos_button;

        //Check drop ack button press (active LOW)
        bool cur_drop_btn = gpio_get(DROP_ACK_BUTTON_PIN);
        if (last_drop_button && !cur_drop_btn) { 
            // GP 22 falling edge
            mqtt_ctx.drop_acks = !mqtt_ctx.drop_acks;
            printf("drop_acks = %d\n", mqtt_ctx.drop_acks);
            sleep_ms(200); // Debounce
        }
        last_drop_button = cur_drop_btn;

        uint32_t now = to_ms_since_boot(get_absolute_time());
        // Send PINGREQ 
        if (ping_ack_received) {
        // Previous ping was acknowledged, can send new PINGREQ periodically
            if (now - last_pingreq >= PING_INTERVAL_MS) {
                mqtt_sn_pingreq(pcb, &gateway_addr, UDP_PORT);
                ping_ack_received = false;  // now waiting for PINGRESP
                last_pingreq = now;
            }
        } 
        else {
            // Waiting for PINGRESP, check timeout
            if (now - last_pingreq > PINGRESP_TIMEOUT_MS) {
                printf("PINGRESP timeout, reconnecting MQTT-SN...\n");
                ping_ack_received = true; // reset flag before reconnect
                mqtt_sn_connect(pcb, &gateway_addr, UDP_PORT);

                // Poll and wait for CONNACK
                for (int i = 0; i < 50; i++) {
                    cyw43_arch_poll();
                    sleep_ms(10);
                }
                printf("Waiting for CONNACK after reconnect...\n");

                mqtt_sn_subscribe_topic_id(pcb, &gateway_addr, UDP_PORT, 1);

                for (int i = 0; i < 20; i++) {
                    cyw43_arch_poll();
                    sleep_ms(10);
                }

                last_pingreq = now;  // reset ping timer after reconnect
            }
        }

        // Check for QoS message timeouts and retransmissions
        check_qos_timeouts(pcb, &gateway_addr, UDP_PORT);

        // Check Wi-Fi connection
        if (!cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA)) {
            printf("Wi-Fi disconnected! Reconnecting...\n");
            while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
                sleep_ms(2000);
            }
            printf("Reconnected. IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));
        }

        sleep_ms(100);
    }

    return 0;
}