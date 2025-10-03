#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
#include <string.h>
#include <stdio.h>

#define WIFI_SSID "Neo"
#define WIFI_PASS "Password1"
#define UDP_PORT 10000  // This must match the GatewayPortNo in gateway.conf
#define KEEPALIVE_INTERVAL_SEC 60   // Must match Duration in CONNECT
#define PING_INTERVAL_MS 30000      // Send ping every 30s (half of keepalive)

// Send MQTT-SN CONNECT packet
static void mqtt_sn_connect(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port) {
    const char *client_id = "pico_w";
    size_t id_len = strlen(client_id);
    u16_t packet_len = 6 + id_len; // [len][type=0x04][flags][protocol_id][duration(2)][client_id]

    if (packet_len > 255) {
        printf("Client ID too long\n");
        return;
    }

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, packet_len, PBUF_RAM);
    if (!p) return;

    uint8_t *data = (uint8_t *)p->payload;
    data[0] = (uint8_t)packet_len;
    data[1] = 0x04;                     // CONNECT
    data[2] = 0x04;                     // Flags (clean session)
    data[3] = 0x01;                     // Protocol ID (MQTT-SN v1.2)
    data[4] = 0x00;                     // Duration high (keep-alive = 0 = disabled)
    data[5] = 0x3C;                     // Duration low (60 sec, but 0 is fine for test)
    memcpy(&data[6], client_id, id_len);

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if (err == ERR_OK) {
        printf("Sent CONNECT as '%s'\n", client_id);
    } else {
        printf("Failed to send CONNECT: %d\n", err);
    }
    pbuf_free(p);
}


// Send MQTT-SN PINGREQ to keep connection alive
static void mqtt_sn_pingreq(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port) {
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, 2, PBUF_RAM);
    if (!p) return;

    uint8_t *data = (uint8_t *)p->payload;
    data[0] = 2;        // Length
    data[1] = 0x16;     // PINGREQ

    udp_sendto(pcb, p, gw_addr, gw_port);
    printf("Sent PINGREQ\n");
    pbuf_free(p);
}

// SUBSCRIBE by Predefined Topic ID 
static void mqtt_sn_subscribe_topic_id(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port, u16_t topic_id) {
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, 7, PBUF_RAM);
    if (!p) return;

    uint8_t *data = (uint8_t *)p->payload;
    data[0] = 7;            // length
    data[1] = 0x12;         // SUBSCRIBE
    data[2] = 0x01;         // flags: QoS0, TopicIdType=predefined (0x01)
    data[3] = 0x00;         // msg ID high
    data[4] = 0x01;         // msg ID low
    data[5] = (topic_id >> 8) & 0xFF;  // topic ID high
    data[6] = topic_id & 0xFF;          // topic ID low

    udp_sendto(pcb, p, gw_addr, gw_port);
    printf("Sent SUBSCRIBE to Predefined Topic ID %d\n", topic_id);
    pbuf_free(p);
}

// Callback for when UDP data is received
static void udp_recv_callback(void *arg, struct udp_pcb *pcb,
                              struct pbuf *p, const ip_addr_t *addr, u16_t port) {
    if (p != NULL && p->len >= 3) {
        uint8_t *data = (uint8_t *)p->payload;
        uint8_t length = data[0];
        uint8_t msg_type = data[1];

        //printf("MQTT-SN packet: len=%d, type=0x%02X\n", length, msg_type);

        if (msg_type == 0x05) { // CONNACK
            uint8_t return_code = data[2];
            printf("CONNACK: return_code=%d (%s)\n", 
                   return_code, return_code == 0 ? "Accepted" : "Rejected");
        }
        else if (msg_type == 0x13) { // SUBACK
            uint16_t topic_id = (data[4] << 8) | data[5];
            uint8_t return_code = data[6];
            printf("SUBACK: topic_id=%d, return_code=%d\n", topic_id, return_code);
        }

        else if (msg_type == 0x0C) { // PUBLISH
            if (length >= 7) {
                uint16_t topic_id = (data[3] << 8) | data[4];
                int payload_len = length - 7;
                
                if (payload_len > 0) {
                    printf("Received on Topic ID %d: ", topic_id);
                    // Print character by character
                    for (int i = 0; i < payload_len; i++) {
                        putchar(data[7 + i]);
                    }
                    printf("\n");
                } else {
                    printf("Received empty message on Topic ID %d\n", topic_id);
                }
            }
        }

        pbuf_free(p);
    }
}

int main() {
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    cyw43_arch_enable_sta_mode();

    //  Keep trying to connect forever
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Wi-Fi connect failed. Retrying in 5 seconds...\n");
        sleep_ms(5000);
    }

    printf("Wi-Fi connected. IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));

    // Set up UDP once (after successful connect)
    struct udp_pcb *pcb = udp_new();
    if (!pcb) {
        printf("UDP setup failed\n");
        return -1;
    }
    // Don't bind, or bind to a different port like 0 (auto-assign)
    if (udp_bind(pcb, IP_ADDR_ANY, 0) != ERR_OK) {
        printf("UDP bind failed\n");
        return -1;
    }
    udp_recv(pcb, udp_recv_callback, NULL);

    printf("UDP client ready...\n");

    // Gateway address - your Windows WiFi IP
    ip_addr_t gateway_addr;
    IP4_ADDR(&gateway_addr, 192, 168, 28, 20); //CHANGE TO YOUR GATEWAY IP

    sleep_ms(1000); // wait for network to be fully ready

    // Send to port 10000 (gateway's listening port)
    mqtt_sn_connect(pcb, &gateway_addr, UDP_PORT);
    for (int i = 0; i < 10; i++) {
        cyw43_arch_poll();
        sleep_ms(10);
    }
    printf("Waiting for CONNACK...\n");
    // Wait and poll for CONNACK response
    for (int i = 0; i < 50; i++) {  // 5 seconds total
        cyw43_arch_poll();
        sleep_ms(100);
    }

    sleep_ms(1000); // wait a bit before subscribing

    printf("Subscribing to 'pico/cmd'...\n");
    mqtt_sn_subscribe_topic_id(pcb, &gateway_addr, UDP_PORT, 1);
    for (int i = 0; i < 10; i++) {
    cyw43_arch_poll();
    sleep_ms(10);
    }

    uint32_t last_ping = 0;

    //  Main loop with disconnection monitoring
    while (true) {
        cyw43_arch_poll();

        //Send PINGREQ periodically
        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now - last_ping >= PING_INTERVAL_MS) {
            mqtt_sn_pingreq(pcb, &gateway_addr, UDP_PORT);
            
            for (int i = 0; i < 5; i++) {
                cyw43_arch_poll();
                sleep_ms(10);
            }
            
            last_ping = now;
        }

        // Optional: Check if Wi-Fi is still linked
        if (!cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA)) {
            printf("Wi-Fi disconnected! Reconnecting...\n");
            while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
                sleep_ms(2000);
            }
            printf("Reconnected. IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));
        }

        sleep_ms(100); // Reduce polling frequency
    }

    return 0;
}