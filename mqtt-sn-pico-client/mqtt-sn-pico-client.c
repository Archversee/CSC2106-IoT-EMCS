#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include "config.h" 
#include "functions.h" 

// Override Wi-Fi credentials here if needed
// #define WIFI_SSID "MyOtherSSID"
// #define WIFI_PASS "MyOtherPass"

int main() {
    
    stdio_init_all();

    // Initialize Message button
    gpio_init(MESSAGEBUTTON_PIN);
    gpio_set_dir(MESSAGEBUTTON_PIN, GPIO_IN);
    gpio_pull_up(MESSAGEBUTTON_PIN);

    // Initialize QoS toggle button
    gpio_init(QOSBUTTON_PIN);
    gpio_set_dir(QOSBUTTON_PIN, GPIO_IN);
    gpio_pull_up(QOSBUTTON_PIN); 

    gpio_init(DROP_ACK_BUTTON_PIN);
    gpio_set_dir(DROP_ACK_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(DROP_ACK_BUTTON_PIN);

    mqtt_sn_context_t mqtt_ctx = {
        .drop_acks = false
    };
    
    int qos_level = 0; 

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

    ip_addr_t gateway_addr;
    IP4_ADDR(&gateway_addr, GATEWAY_IP0, GATEWAY_IP1, GATEWAY_IP2, GATEWAY_IP3); //RMB to change to your gateway IP

    sleep_ms(1000);

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

    while (true) {
        cyw43_arch_poll();

        // Check message button press (active LOW)
        bool current_button = gpio_get(MESSAGEBUTTON_PIN);
        if (last_button_state && !current_button) {
            // GP 20 Button pressed (falling edge)
            printf("Button pressed! Publishing message...\n");
            uint16_t id = get_next_msg_id();
            mqtt_sn_publish_topic_id(pcb, &gateway_addr, UDP_PORT, 2, "Hello from pico W!", qos_level, id, false);
            sleep_ms(200); // Debounce
        }
        last_button_state = current_button;

        // Check Qos button press (active LOW)
        bool current_qos_button = gpio_get(QOSBUTTON_PIN);
        if (last_qos_button && !current_qos_button) {
            // GP 21 Button pressed (falling edge)
            qos_level++;
            if (qos_level > 2) qos_level = 0; // Wrap around
            printf("QoS level changed to: %d\n", qos_level);
            sleep_ms(200); // debounce
        }
        last_qos_button = current_qos_button;

        //Toggle drop_acks with button (active LOW)
        bool cur_drop_btn = gpio_get(DROP_ACK_BUTTON_PIN);
        if (last_drop_button && !cur_drop_btn) { // falling edge
            mqtt_ctx.drop_acks = !mqtt_ctx.drop_acks;
            printf("drop_acks = %d\n", mqtt_ctx.drop_acks);
            sleep_ms(200);
        }
        last_drop_button = cur_drop_btn;

        // SEND PINGREQ at intervals
        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now - last_ping >= PING_INTERVAL_MS) {
            mqtt_sn_pingreq(pcb, &gateway_addr, UDP_PORT);
            for (int i = 0; i < 5; i++) {
                cyw43_arch_poll();
                sleep_ms(10);
            }
            last_ping = now;
        }

        // Check for QoS message timeouts and retransmissions
        check_qos_timeouts(pcb, &gateway_addr, UDP_PORT);

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