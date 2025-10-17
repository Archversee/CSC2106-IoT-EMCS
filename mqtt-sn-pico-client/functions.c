// functions.c
#include "functions.h"
#include "config.h"
#include "lwip/pbuf.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

static uint16_t next_msg_id = 1;
qos_msg_t pending_msgs[MAX_PENDING_QOS_MSGS];

uint16_t get_next_msg_id(void) {
    return next_msg_id++;
}

// Send MQTT-SN CONNECT packet
void mqtt_sn_connect(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port)
{
    const char *client_id = MQTT_SN_CLIENT_ID;
    size_t id_len = strlen(client_id);
    u16_t packet_len = 6 + id_len; // [len][type=0x04][flags][protocol_id][duration(2)][client_id]

    if(packet_len > 255)
    {
        printf("Client ID too long\n");
        return;
    }

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, packet_len, PBUF_RAM);
    if(!p)
        return;

    uint8_t *data = (uint8_t *) p->payload;
    data[0] = (uint8_t) packet_len;
    data[1] = 0x04;                                 // CONNECT
    data[2] = 0x04;                                 // Flags (clean session)
    data[3] = 0x01;                                 // Protocol ID (MQTT-SN v1.2)
    data[4] = (KEEPALIVE_INTERVAL_SEC >> 8) & 0xFF; // Duration high
    data[5] = KEEPALIVE_INTERVAL_SEC & 0xFF;        // Duration low
    memcpy(&data[6], client_id, id_len);

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if(err == ERR_OK)
    {
        printf("Sent CONNECT as '%s'\n", client_id);
    }
    else
    {
        printf("Failed to send CONNECT: %d\n", err);
    }
    pbuf_free(p);
}

// Send MQTT-SN PINGREQ to keep connection alive
void mqtt_sn_pingreq(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port)
{
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, 2, PBUF_RAM);
    if(!p)
        return;

    uint8_t *data = (uint8_t *) p->payload;
    data[0] = 2;    // Length
    data[1] = 0x16; // PINGREQ

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if(err == ERR_OK)
    {
        printf("Sent PINGREQ\n");
    }
    else
    {
        printf("Failed to send PINGREQ: %d\n", err);
    }
    pbuf_free(p);
}

// SUBSCRIBE by Predefined Topic ID
void mqtt_sn_subscribe_topic_id(struct udp_pcb *pcb,
                                const ip_addr_t *gw_addr,
                                u16_t gw_port,
                                u16_t topic_id)
{
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, 7, PBUF_RAM);
    if(!p)
        return;

    uint8_t *data = (uint8_t *) p->payload;
    data[0] = 7;                      // length
    data[1] = 0x12;                   // SUBSCRIBE
    data[2] = 0x41;                   // flags: QoS2, TopicIdType=predefined (0x01)
    data[3] = 0x00;                   // msg ID high
    data[4] = 0x01;                   // msg ID low
    data[5] = (topic_id >> 8) & 0xFF; // topic ID high
    data[6] = topic_id & 0xFF;        // topic ID low

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if(err == ERR_OK)
    {
        printf("Sent SUBSCRIBE to Predefined Topic ID %d\n", topic_id);
    }
    else
    {
        printf("Failed to send SUBSCRIBE: %d\n", err);
    }
    pbuf_free(p);
}

// PUBLISH to Predefined Topic ID with qos, binary payload support
void mqtt_sn_publish_topic_id(struct udp_pcb *pcb,
                              const ip_addr_t *gw_addr,
                              u16_t gw_port,
                              u16_t topic_id,
                              const uint8_t *payload,
                              size_t payload_len,
                              int qos,
                              uint16_t msg_id,
                              bool is_retransmit)
{
    if (!payload || qos < 0 || qos > 2) {
        printf("Invalid QoS or payload\n");
        return;
    }

    u16_t packet_len = 7 + payload_len;

    if(packet_len > 255)
    {
        printf("PUBLISH payload too long\n");
        return;
    }

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, packet_len, PBUF_RAM);
    if(!p)
    {
        printf("Failed to allocate pbuf\n");
        return;
    }

    // QOS Flags 
    uint8_t flags = 0x01; // TopicIdType = Predefined

    if (qos == 1) {
        flags |= 0x20; // QoS 1 (bit 5 = 1)
    } else if (qos == 2) {
        flags |= 0x40; // QoS 2 (bit 6 = 1)
    }

    uint8_t *data = (uint8_t *) p->payload;
    data[0] = (uint8_t) packet_len;
    data[1] = 0x0C;                   // PUBLISH
    data[2] = flags;                  
    data[3] = (topic_id >> 8) & 0xFF; // Topic ID high
    data[4] = topic_id & 0xFF;        // Topic ID low
    data[5] = (qos > 0) ? (msg_id >> 8) & 0xFF : 0x00;
    data[6] = (qos > 0) ? (msg_id & 0xFF) : 0x00;
    memcpy(&data[7], payload, payload_len);

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if (err == ERR_OK) {
        printf("Sent PUBLISH to Topic ID %d (QoS %d, Msg ID %d, Len %d)\n", topic_id, qos, msg_id, packet_len);

        if (qos > 0 && !is_retransmit) {
            for (int i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
                if (!pending_msgs[i].in_use) {
                    pending_msgs[i].in_use = true;
                    pending_msgs[i].msg_id = msg_id;
                    pending_msgs[i].qos = qos;
                    pending_msgs[i].step = 0;
                    pending_msgs[i].timestamp = get_absolute_time();
                    pending_msgs[i].retry_count = 0;
                    pending_msgs[i].topic_id = topic_id;
                    // Store binary payload safely, truncate if needed
                    size_t copy_len = (payload_len < sizeof(pending_msgs[i].payload)) ? payload_len : sizeof(pending_msgs[i].payload);
                    memcpy(pending_msgs[i].payload, payload, copy_len);
                    pending_msgs[i].payload_len = copy_len;
                    break;
                }
            }
        }
    } else {
        printf("Failed to send PUBLISH: %d\n", err);
    }
    pbuf_free(p);
}


void mqtt_sn_send_puback(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port, 
                         uint16_t topic_id, uint16_t msg_id, uint8_t return_code) {
    uint8_t msg[7];
    msg[0] = 7; // Length
    msg[1] = 0x0D; // PUBACK
    msg[2] = (topic_id >> 8) & 0xFF;
    msg[3] = topic_id & 0xFF;
    msg[4] = (msg_id >> 8) & 0xFF;
    msg[5] = msg_id & 0xFF;
    msg[6] = return_code; // 0x00 = Accepted

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    if (!p) return;
    memcpy(p->payload, msg, sizeof(msg));
    udp_sendto(pcb, p, gw_addr, gw_port);
    pbuf_free(p);

    printf("Sent PUBACK (topic_id: %u, msg_id: %u, rc: %u)\n", topic_id, msg_id, return_code);
}


void mqtt_sn_send_pubrec(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port, uint16_t msg_id) {
    uint8_t msg[5];
    msg[0] = 5;
    msg[1] = 0x0F; // PUBREC
    msg[2] = (msg_id >> 8) & 0xFF;
    msg[3] = msg_id & 0xFF;
    msg[4] = 0x00;                   // Return code: ACCEPTED (0x00)

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    if (!p) return;
    memcpy(p->payload, msg, sizeof(msg));
    udp_sendto(pcb, p, gw_addr, gw_port);
    pbuf_free(p);
    printf("Sent PUBREC for Msg ID: %d\n", msg_id);
}

void mqtt_sn_send_pubcomp(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port, uint16_t msg_id) {
    uint8_t msg[5];
    msg[0] = 5;
    msg[1] = 0x0E; // PUBCOMP
    msg[2] = (msg_id >> 8) & 0xFF;
    msg[3] = msg_id & 0xFF;
    msg[4] = 0x00;                   // Return code: ACCEPTED (0x00)

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    if (!p) return;
    memcpy(p->payload, msg, sizeof(msg));
    udp_sendto(pcb, p, gw_addr, gw_port);
    pbuf_free(p);
    printf("Sent PUBCOMP for Msg ID: %d\n", msg_id);
}

void mqtt_sn_send_pubrel(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port, uint16_t msg_id) {
    uint8_t msg[4];
    msg[0] = 4; // Length
    msg[1] = 0x10; // PUBREL
    msg[2] = (msg_id >> 8) & 0xFF;
    msg[3] = msg_id & 0xFF;

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    if (!p) return;

    memcpy(p->payload, msg, sizeof(msg));
    udp_sendto(pcb, p, gw_addr, gw_port);
    pbuf_free(p);
}

//Check and handle QoS message timeouts and retransmissions
void check_qos_timeouts(struct udp_pcb *pcb, const ip_addr_t *gw_addr, u16_t gw_port) {
    absolute_time_t now = get_absolute_time();
    for (int i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
        if (!pending_msgs[i].in_use) continue;

        if (absolute_time_diff_us(pending_msgs[i].timestamp, now) > QOS_RETRY_INTERVAL_US) { 
            if (pending_msgs[i].retry_count >= QOS_MAX_RETRIES) {
                printf("QoS %d Msg ID %d failed after retries\n", pending_msgs[i].qos, pending_msgs[i].msg_id);
                pending_msgs[i].in_use = false;
                continue;
            }

            if (pending_msgs[i].qos == 1) {
                printf("Retransmitting QoS1 PUBLISH for Msg ID %d\n", pending_msgs[i].msg_id);
                mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port,
                                         pending_msgs[i].topic_id,
                                         pending_msgs[i].payload,
                                         pending_msgs[i].payload_len,
                                         pending_msgs[i].qos,
                                         pending_msgs[i].msg_id,
                                         true);
            } else if (pending_msgs[i].qos == 2) {
                if (pending_msgs[i].step == 0) {
                    printf("Retransmitting QoS2 PUBLISH for Msg ID %d\n", pending_msgs[i].msg_id);
                    mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port,
                                             pending_msgs[i].topic_id,
                                             pending_msgs[i].payload,
                                             pending_msgs[i].payload_len,
                                             pending_msgs[i].qos,
                                             pending_msgs[i].msg_id,
                                             true);
                } else if (pending_msgs[i].step == 1) {
                    printf("Retransmitting PUBREL for Msg ID %d\n", pending_msgs[i].msg_id);
                    mqtt_sn_send_pubrel(pcb, gw_addr, gw_port, pending_msgs[i].msg_id);
                }
            }

            pending_msgs[i].retry_count++;
            pending_msgs[i].timestamp = get_absolute_time();
        }
    }
}

void remove_pending_qos_msg(uint16_t msg_id) {
    for (int i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
        if (pending_msgs[i].in_use && pending_msgs[i].msg_id == msg_id) {
            pending_msgs[i].in_use = false;
            break;
        }
    }
}


// Callback for when UDP data is received
void udp_recv_callback(
    void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    mqtt_sn_context_t *ctx = (mqtt_sn_context_t *)arg;

    if(p != NULL && p->len >= 2)
    {
        uint8_t *data = (uint8_t *) p->payload;
        uint8_t length = data[0];
        uint8_t msg_type = data[1];

        if (ctx && ctx->drop_acks && (msg_type == 0x0D || msg_type == 0x0F || msg_type == 0x0E)) {
            printf("Simulated drop of ACK type 0x%02X\n", msg_type);
            pbuf_free(p);
            return;
        }

        if(msg_type == 0x17) {  
            // PINGRESP
            last_pingresp = to_ms_since_boot(get_absolute_time());
            ping_ack_received = true;
            printf("Received PINGRESP\n");
        } 

        if(msg_type == 0x05)
        { // CONNACK
            uint8_t return_code = data[2];
            printf("CONNACK: return_code=%d (%s)\n",
                   return_code,
                   return_code == 0 ? "Accepted" : "Rejected");
            ping_ack_received = true;
        }
        
        else if(msg_type == 0x13)
        { // SUBACK
            uint8_t flags = data[2];
            uint16_t topic_id = (data[3] << 8) | data[4];
            uint16_t msg_id = (data[5] << 8) | data[6];
            uint8_t return_code = data[7];
            printf("SUBACK: topic_id=%d, msg_id=%d, return_code=%d\n", topic_id, msg_id, return_code);
        }
        else if (msg_type == 0x0C) 
        { // PUBLISH received
            if (length >= 7) {
                uint8_t flags = data[2];
                uint8_t qos = (flags >> 5) & 0x03;
                uint16_t topic_id = (data[3] << 8) | data[4];
                uint16_t msg_id = (data[5] << 8) | data[6];
                int payload_len = length - 7;
                const uint8_t *payload = &data[7];

                // Print binary payload in hex
                printf("PUBLISH received (QoS %d, Msg ID %d), Payload (%d bytes): ", qos, msg_id, payload_len);
                for (int i = 0; i < payload_len; i++) {
                    printf("%02X ", payload[i]);
                }
                printf("\n");

                // handle text commands embedded in binary
                if (payload_len == 6 && memcmp(payload, "led on", 6) == 0) {
                    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
                } 
                else if (payload_len == 7 && memcmp(payload, "led off", 7) == 0) {
                    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
                }

                // QoS ACKs
                if (qos == 1) {
                    mqtt_sn_send_puback(pcb, addr, port, topic_id, msg_id, 0x00);
                } else if (qos == 2) {
                    mqtt_sn_send_pubrec(pcb, addr, port, msg_id);
                }
            }
        }
        else if (msg_type == 0x0D) 
        {   //PUBACK QOS 1
            uint16_t msg_id = (data[4] << 8) | data[5];
            printf("PUBACK received for Msg ID: %d \n", msg_id);
            remove_pending_qos_msg(msg_id);
        }
        else if (msg_type == 0x0E) 
        {   //PUBCOMP (QoS 2 final ack)
            uint16_t msg_id = (data[2] << 8) | data[3];
            printf("PUBCOMP received for Msg ID: %d\n", msg_id);
            remove_pending_qos_msg(msg_id);
        }
        else if (msg_type == 0x0F) 
        {   //PUBREC (QoS 2 Step 1) then send PUBREL
            uint16_t msg_id = (data[2] << 8) | data[3];
            printf("PUBREC received for Msg ID: %d. Sending PUBREL...\n", msg_id);
            sleep_ms(200); // brief pause so i can debug and turn of gateway
            mqtt_sn_send_pubrel(pcb, addr, port, msg_id); // Send PUBREL in response
            // Update step to indicate PUBREL was sent
            for (int i = 0; i < MAX_PENDING_QOS_MSGS; i++) {
                if (pending_msgs[i].in_use && pending_msgs[i].msg_id == msg_id) {
                    pending_msgs[i].step = 1;
                    pending_msgs[i].timestamp = get_absolute_time(); // reset timer
                    break;
                }
            }
        }
        else if (msg_type == 0x10) 
        { //PUBREL (QoS 2 Step 2) then send PUBCOMP
            uint16_t msg_id = (data[2] << 8) | data[3];
            printf("PUBREL received for Msg ID: %d. Sending PUBCOMP...\n", msg_id);
            mqtt_sn_send_pubcomp(pcb, addr, port, msg_id);
            remove_pending_qos_msg(msg_id);
        }
        pbuf_free(p);
    }
    else
    {
        if(p)
            pbuf_free(p); // Always free if not handled
    }
}
