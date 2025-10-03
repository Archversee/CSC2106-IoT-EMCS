// functions.c
#include "functions.h"
#include "pico/stdlib.h"
#include "lwip/pbuf.h"
#include <string.h>
#include <stdio.h>
#include "config.h"

// Send MQTT-SN CONNECT packet
void mqtt_sn_connect(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port) {
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
    data[4] = (KEEPALIVE_INTERVAL_SEC >> 8) & 0xFF; // Duration high
    data[5] = KEEPALIVE_INTERVAL_SEC & 0xFF;        // Duration low
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
void mqtt_sn_pingreq(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port) {
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, 2, PBUF_RAM);
    if (!p) return;

    uint8_t *data = (uint8_t *)p->payload;
    data[0] = 2;        // Length
    data[1] = 0x16;     // PINGREQ

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if (err == ERR_OK) {
        printf("Sent PINGREQ\n");
    } else {
        printf("Failed to send PINGREQ: %d\n", err);
    }
    pbuf_free(p);
}

// SUBSCRIBE by Predefined Topic ID 
void mqtt_sn_subscribe_topic_id(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port, u16_t topic_id) {
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

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if (err == ERR_OK) {
        printf("Sent SUBSCRIBE to Predefined Topic ID %d\n", topic_id);
    } else {
        printf("Failed to send SUBSCRIBE: %d\n", err);
    }
    pbuf_free(p);
}

// Callback for when UDP data is received
void udp_recv_callback(void *arg, struct udp_pcb *pcb,
                       struct pbuf *p, const ip_addr_t *addr, u16_t port) {
    if (p != NULL && p->len >= 3) {
        uint8_t *data = (uint8_t *)p->payload;
        uint8_t length = data[0];
        uint8_t msg_type = data[1];

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
    } else {
        if (p) pbuf_free(p); // Always free if not handled
    }
}