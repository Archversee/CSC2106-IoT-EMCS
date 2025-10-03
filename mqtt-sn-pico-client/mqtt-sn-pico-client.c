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
    udp_recv(pcb, udp_recv_callback, NULL);

    printf("UDP client ready...\n");

    ip_addr_t gateway_addr;
    IP4_ADDR(&gateway_addr, GATEWAY_IP0, GATEWAY_IP1, GATEWAY_IP2, GATEWAY_IP3); //RMB to change to your  gateway IP

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

    while (true) {
        cyw43_arch_poll();

        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now - last_ping >= PING_INTERVAL_MS) {
            mqtt_sn_pingreq(pcb, &gateway_addr, UDP_PORT);
            for (int i = 0; i < 5; i++) {
                cyw43_arch_poll();
                sleep_ms(10);
            }
            last_ping = now;
        }

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