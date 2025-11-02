// mqtt-sn-udp.c
#include "mqtt-sn-udp.h"

#include <stdio.h>
#include <string.h>

#include "../config.h"
#include "config.h"
#include "lwip/pbuf.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

/*! File Transfer Configuration */
#define FILE_TRANSFER_QOS 1 /*!< QoS level for file transfers (1=at-least-once with duplicates handled) */

/*! MQTT-SN Protocol Message Types (as per MQTT-SN v1.2 Specification) */
#define MQTTSN_MSG_TYPE_CONNECT (0x04U)   /*!< CONNECT message type */
#define MQTTSN_MSG_TYPE_CONNACK (0x05U)   /*!< CONNACK message type */
#define MQTTSN_MSG_TYPE_PUBLISH (0x0CU)   /*!< PUBLISH message type */
#define MQTTSN_MSG_TYPE_PUBACK (0x0DU)    /*!< PUBACK message type */
#define MQTTSN_MSG_TYPE_PUBCOMP (0x0EU)   /*!< PUBCOMP message type */
#define MQTTSN_MSG_TYPE_PUBREC (0x0FU)    /*!< PUBREC message type */
#define MQTTSN_MSG_TYPE_PUBREL (0x10U)    /*!< PUBREL message type */
#define MQTTSN_MSG_TYPE_SUBSCRIBE (0x12U) /*!< SUBSCRIBE message type */
#define MQTTSN_MSG_TYPE_SUBACK (0x13U)    /*!< SUBACK message type */
#define MQTTSN_MSG_TYPE_PINGREQ (0x16U)   /*!< PINGREQ message type */
#define MQTTSN_MSG_TYPE_PINGRESP (0x17U)  /*!< PINGRESP message type */

/*! MQTT-SN Protocol Constants */
#define MQTTSN_FLAG_CLEAN_SESSION (0x04U)    /*!< Clean session flag */
#define MQTTSN_PROTOCOL_ID (0x01U)           /*!< MQTT-SN Protocol ID v1.2 */
#define MQTTSN_FLAG_TOPIC_PREDEFINED (0x01U) /*!< Predefined topic type */
#define MQTTSN_FLAG_QOS1 (0x20U)             /*!< QoS level 1 flag (bit 5) */
#define MQTTSN_FLAG_QOS2 (0x40U)             /*!< QoS level 2 flag (bit 6) */
#define MQTTSN_FLAG_QOS_MASK (0x03U)         /*!< QoS mask for extraction */
#define MQTTSN_QOS_SHIFT (5U)                /*!< QoS bit shift position */
#define MQTTSN_RETURN_ACCEPTED (0x00U)       /*!< Return code: accepted */
#define MQTTSN_SUBSCRIBE_FLAGS_QOS2 (0x41U)  /*!< Subscribe with QoS2 + Predefined topic */
#define MQTTSN_BYTE_MASK (0xFFU)             /*!< Byte mask for extraction */

/*! Module-level state variables */
static uint16_t s_next_msg_id = 1U;
qos_msg_t g_pending_msgs[MAX_PENDING_QOS_MSGS];

/*!
 * @brief Get next unique message ID for MQTT-SN
 * @return uint16_t Next message ID (1-65535, wraps around)
 * @note Not thread-safe - assumes single-threaded access
 */
uint16_t get_next_msg_id(void) {
    return s_next_msg_id++;
}

// Send MQTT-SN CONNECT packet
void mqtt_sn_connect(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port) {
    const char* client_id = MQTT_SN_CLIENT_ID;
    size_t id_len = strlen(client_id);
    u16_t packet_len = 6 + id_len;  // [len][type=0x04][flags][protocol_id][duration(2)][client_id]

    if (packet_len > 255) {
        printf("Client ID too long\n");
        return;
    }

    // Allocate pbuf for CONNECT packet
    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, packet_len, PBUF_RAM);
    if (!p)
        return;

    uint8_t* data = (uint8_t*)p->payload;
    data[0] = (uint8_t)packet_len;
    data[1] = MQTTSN_MSG_TYPE_CONNECT;                           // CONNECT
    data[2] = MQTTSN_FLAG_CLEAN_SESSION;                         // Flags (clean session)
    data[3] = MQTTSN_PROTOCOL_ID;                                // Protocol ID (MQTT-SN v1.2)
    data[4] = (KEEPALIVE_INTERVAL_SEC >> 8) & MQTTSN_BYTE_MASK;  // Duration high
    data[5] = KEEPALIVE_INTERVAL_SEC & MQTTSN_BYTE_MASK;         // Duration low
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
void mqtt_sn_pingreq(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port) {
    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, 2, PBUF_RAM);
    if (!p)
        return;

    uint8_t* data = (uint8_t*)p->payload;
    data[0] = 2U;                       // Length
    data[1] = MQTTSN_MSG_TYPE_PINGREQ;  // PINGREQ

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if (err == ERR_OK) {
        printf("Sent PINGREQ\n");
    } else {
        printf("Failed to send PINGREQ: %d\n", err);
    }
    pbuf_free(p);
}

// SUBSCRIBE by Predefined Topic ID
void mqtt_sn_subscribe_topic_id(struct udp_pcb* pcb,
                                const ip_addr_t* gw_addr,
                                u16_t gw_port,
                                u16_t topic_id) {
    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, 7, PBUF_RAM);
    if (!p)
        return;

    uint8_t* data = (uint8_t*)p->payload;
    data[0] = 7U;                                  // length
    data[1] = MQTTSN_MSG_TYPE_SUBSCRIBE;           // SUBSCRIBE
    data[2] = MQTTSN_SUBSCRIBE_FLAGS_QOS2;         // flags: QoS2, TopicIdType=predefined
    data[3] = 0x00U;                               // msg ID high
    data[4] = 0x01U;                               // msg ID low
    data[5] = (topic_id >> 8) & MQTTSN_BYTE_MASK;  // topic ID high
    data[6] = topic_id & MQTTSN_BYTE_MASK;         // topic ID low

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if (err == ERR_OK) {
        printf("Sent SUBSCRIBE to Predefined Topic ID %d\n", topic_id);
    } else {
        printf("Failed to send SUBSCRIBE: %d\n", err);
    }
    pbuf_free(p);
}

// PUBLISH to Predefined Topic ID with qos, binary payload support
void mqtt_sn_publish_topic_id(struct udp_pcb* pcb,
                              const ip_addr_t* gw_addr,
                              u16_t gw_port,
                              u16_t topic_id,
                              const uint8_t* payload,
                              size_t payload_len,
                              int qos,
                              uint16_t msg_id,
                              bool is_retransmit) {
    if (!payload || qos < 0 || qos > 2) {
        printf("Invalid QoS or payload\n");
        return;
    }

    // Calculate total packet length 7(QoS1 header) + payload_len
    u16_t packet_len = 7 + payload_len;

    if (packet_len > 255) {
        printf("PUBLISH payload too long\n");
        return;
    }

    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, packet_len, PBUF_RAM);
    if (!p) {
        printf("Failed to allocate pbuf\n");
        return;
    }

    // QOS Flags
    uint8_t flags = MQTTSN_FLAG_TOPIC_PREDEFINED;  // TopicIdType = Predefined

    if (qos == 1U) {
        flags |= MQTTSN_FLAG_QOS1;  // QoS 1 (bit 5 = 1)
    } else if (qos == 2U) {
        flags |= MQTTSN_FLAG_QOS2;  // QoS 2 (bit 6 = 1)
    }

    uint8_t* data = (uint8_t*)p->payload;
    data[0] = (uint8_t)packet_len;
    data[1] = MQTTSN_MSG_TYPE_PUBLISH;  // PUBLISH
    data[2] = flags;
    data[3] = (topic_id >> 8) & MQTTSN_BYTE_MASK;  // Topic ID high
    data[4] = topic_id & MQTTSN_BYTE_MASK;         // Topic ID low
    data[5] = (qos > 0U) ? (msg_id >> 8) & MQTTSN_BYTE_MASK : 0x00U;
    data[6] = (qos > 0U) ? (msg_id & MQTTSN_BYTE_MASK) : 0x00U;
    memcpy(&data[7], payload, payload_len);

    err_t err = udp_sendto(pcb, p, gw_addr, gw_port);
    if (err == ERR_OK) {
        printf("Sent PUBLISH to Topic ID %d (QoS %d, Msg ID %d, Len %d)\n", topic_id, qos, msg_id, packet_len);

        // Store pending QoS message for retransmission if needed
        if (qos > 0 && !is_retransmit) {
            for (size_t i = 0U; i < MAX_PENDING_QOS_MSGS; i++) {
                if (!g_pending_msgs[i].in_use) {
                    g_pending_msgs[i].in_use = true;
                    g_pending_msgs[i].msg_id = msg_id;
                    g_pending_msgs[i].qos = qos;
                    g_pending_msgs[i].step = 0;
                    g_pending_msgs[i].timestamp = get_absolute_time();
                    g_pending_msgs[i].retry_count = 0U;
                    g_pending_msgs[i].topic_id = topic_id;
                    // Store binary payload safely, truncate if needed
                    size_t copy_len = (payload_len < sizeof(g_pending_msgs[i].payload)) ? payload_len : sizeof(g_pending_msgs[i].payload);
                    memcpy(g_pending_msgs[i].payload, payload, copy_len);
                    g_pending_msgs[i].payload_len = copy_len;
                    break;
                }
            }
        }
    } else {
        printf("Failed to send PUBLISH: %d\n", err);
    }
    pbuf_free(p);
}

// Send PUBACK for QoS 1
void mqtt_sn_send_puback(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port,
                         uint16_t topic_id, uint16_t msg_id, uint8_t return_code) {
    uint8_t msg[7];
    msg[0] = 7U;                      // Length
    msg[1] = MQTTSN_MSG_TYPE_PUBACK;  // PUBACK
    msg[2] = (topic_id >> 8) & MQTTSN_BYTE_MASK;
    msg[3] = topic_id & MQTTSN_BYTE_MASK;
    msg[4] = (msg_id >> 8) & MQTTSN_BYTE_MASK;
    msg[5] = msg_id & MQTTSN_BYTE_MASK;
    msg[6] = return_code;  // typically MQTTSN_RETURN_ACCEPTED

    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    if (!p) return;
    memcpy(p->payload, msg, sizeof(msg));
    udp_sendto(pcb, p, gw_addr, gw_port);
    pbuf_free(p);

    printf("Sent PUBACK (topic_id: %u, msg_id: %u, rc: %u)\n", topic_id, msg_id, return_code);
}

// Send PUBREC for QoS 2
void mqtt_sn_send_pubrec(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port, uint16_t msg_id) {
    uint8_t msg[5];
    msg[0] = 5U;                      // Length
    msg[1] = MQTTSN_MSG_TYPE_PUBREC;  // PUBREC
    msg[2] = (msg_id >> 8) & MQTTSN_BYTE_MASK;
    msg[3] = msg_id & MQTTSN_BYTE_MASK;
    msg[4] = MQTTSN_RETURN_ACCEPTED;  // Return code: ACCEPTED

    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    if (!p) return;
    memcpy(p->payload, msg, sizeof(msg));
    udp_sendto(pcb, p, gw_addr, gw_port);
    pbuf_free(p);
    printf("Sent PUBREC for Msg ID: %d\n", msg_id);
}

//  Send PUBCOMP for QoS 2
void mqtt_sn_send_pubcomp(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port, uint16_t msg_id) {
    uint8_t msg[5];
    msg[0] = 5U;                       // Length
    msg[1] = MQTTSN_MSG_TYPE_PUBCOMP;  // PUBCOMP
    msg[2] = (msg_id >> 8) & MQTTSN_BYTE_MASK;
    msg[3] = msg_id & MQTTSN_BYTE_MASK;
    msg[4] = MQTTSN_RETURN_ACCEPTED;  // Return code: ACCEPTED

    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    if (!p) return;
    memcpy(p->payload, msg, sizeof(msg));
    udp_sendto(pcb, p, gw_addr, gw_port);
    pbuf_free(p);
    printf("Sent PUBCOMP for Msg ID: %d\n", msg_id);
}

//  Send PUBREL for QoS 2
void mqtt_sn_send_pubrel(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port, uint16_t msg_id) {
    uint8_t msg[4];
    msg[0] = 4U;                      // Length
    msg[1] = MQTTSN_MSG_TYPE_PUBREL;  // PUBREL
    msg[2] = (msg_id >> 8) & MQTTSN_BYTE_MASK;
    msg[3] = msg_id & MQTTSN_BYTE_MASK;

    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    if (!p) return;

    memcpy(p->payload, msg, sizeof(msg));
    udp_sendto(pcb, p, gw_addr, gw_port);
    pbuf_free(p);
}

// Check and handle QoS message timeouts and retransmissions
void check_qos_timeouts(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port) {
    absolute_time_t now = get_absolute_time();
    // Iterate through pending QoS messages
    for (size_t i = 0U; i < MAX_PENDING_QOS_MSGS; i++) {
        if (!g_pending_msgs[i].in_use) continue;

        // Check if timeout exceeded
        if (absolute_time_diff_us(g_pending_msgs[i].timestamp, now) > QOS_RETRY_INTERVAL_US) {
            // Check if max retries reached
            if (g_pending_msgs[i].retry_count >= QOS_MAX_RETRIES) {
                printf("QoS %d Msg ID %d failed after retries\n", g_pending_msgs[i].qos, g_pending_msgs[i].msg_id);
                g_pending_msgs[i].in_use = false;
                continue;
            }

            // Retransmit based on QoS level and step
            if (g_pending_msgs[i].qos == 1) {
                printf("Retransmitting QoS1 PUBLISH for Msg ID %d\n", g_pending_msgs[i].msg_id);
                mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port,
                                         g_pending_msgs[i].topic_id,
                                         g_pending_msgs[i].payload,
                                         g_pending_msgs[i].payload_len,
                                         g_pending_msgs[i].qos,
                                         g_pending_msgs[i].msg_id,
                                         true);
            } else if (g_pending_msgs[i].qos == 2) {
                if (g_pending_msgs[i].step == 0) {
                    printf("Retransmitting QoS2 PUBLISH for Msg ID %d\n", g_pending_msgs[i].msg_id);
                    mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port,
                                             g_pending_msgs[i].topic_id,
                                             g_pending_msgs[i].payload,
                                             g_pending_msgs[i].payload_len,
                                             g_pending_msgs[i].qos,
                                             g_pending_msgs[i].msg_id,
                                             true);
                } else if (g_pending_msgs[i].step == 1) {
                    printf("Retransmitting PUBREL for Msg ID %d\n", g_pending_msgs[i].msg_id);
                    mqtt_sn_send_pubrel(pcb, gw_addr, gw_port, g_pending_msgs[i].msg_id);
                }
            }

            g_pending_msgs[i].retry_count++;
            g_pending_msgs[i].timestamp = get_absolute_time();
        }
    }
}

/*!
 * @brief Remove pending QoS message by message ID
 * @param msg_id Message ID to remove from pending queue
 */
void remove_pending_qos_msg(uint16_t msg_id) {
    for (size_t i = 0U; i < MAX_PENDING_QOS_MSGS; i++) {
        if (g_pending_msgs[i].in_use && g_pending_msgs[i].msg_id == msg_id) {
            g_pending_msgs[i].in_use = false;
            break;
        }
    }
}

// Callback for when UDP data is received
void udp_recv_callback(
    void* arg, struct udp_pcb* pcb, struct pbuf* p, const ip_addr_t* addr, u16_t port) {
    mqtt_sn_context_t* ctx = (mqtt_sn_context_t*)arg;

    if (p != NULL && p->len >= 2) {
        uint8_t* data = (uint8_t*)p->payload;
        uint8_t length = data[0];
        uint8_t msg_type = data[1];

        // Simulate dropping ACKs
        if (ctx && ctx->drop_acks &&
            (msg_type == MQTTSN_MSG_TYPE_PUBACK ||
             msg_type == MQTTSN_MSG_TYPE_PUBREC ||
             msg_type == MQTTSN_MSG_TYPE_PUBCOMP)) {
            printf("Simulated drop of ACK type 0x%02X\n", msg_type);
            pbuf_free(p);
            return;
        }

        // PINGRESP
        if (msg_type == MQTTSN_MSG_TYPE_PINGRESP) {
            g_last_pingresp = to_ms_since_boot(get_absolute_time());
            g_ping_ack_received = true;
            printf("Received PINGRESP\n");
        }

        // CONNACK
        else if (msg_type == MQTTSN_MSG_TYPE_CONNACK) {
            uint8_t return_code = data[2];
            printf("CONNACK: return_code=%d (%s)\n",
                   return_code,
                   return_code == MQTTSN_RETURN_ACCEPTED ? "Accepted" : "Rejected");
            g_ping_ack_received = true;
        }

        // SUBACK
        else if (msg_type == MQTTSN_MSG_TYPE_SUBACK) {
            uint8_t flags = data[2];
            uint16_t topic_id = (data[3] << 8) | data[4];
            uint16_t msg_id = (data[5] << 8) | data[6];
            uint8_t return_code = data[7];
            printf("SUBACK: topic_id=%d, msg_id=%d, return_code=%d\n", topic_id, msg_id, return_code);
        }

        // PUBLISH received
        else if (msg_type == MQTTSN_MSG_TYPE_PUBLISH) {
            if (length >= 7U) {
                uint8_t flags = data[2];
                uint8_t qos = (flags >> MQTTSN_QOS_SHIFT) & MQTTSN_FLAG_QOS_MASK;
                uint16_t topic_id = (data[3] << 8) | data[4];
                uint16_t msg_id = (data[5] << 8) | data[6];
                int payload_len = length - 7;
                const uint8_t* payload = &data[7];

                // Check for file transfer topics first
                if (topic_id == 3U) {
                    // file/meta topic
                    printf("PUBLISH: File metadata received (Msg ID %d)\n", msg_id);
                    handle_file_metadata(ctx, payload, payload_len);
                    // Send PUBACK
                    if (qos == 1U) {
                        mqtt_sn_send_puback(pcb, addr, port, topic_id, msg_id, MQTTSN_RETURN_ACCEPTED);
                    }
                } else if (topic_id == 4U) {
                    // file/data topic
                    handle_file_payload(ctx, payload, payload_len);
                    // Send PUBACK
                    if (qos == 1U) {
                        mqtt_sn_send_puback(pcb, addr, port, topic_id, msg_id, MQTTSN_RETURN_ACCEPTED);
                    }
                } else {
                    // Regular message handling
                    // Print binary payload in hex
                    printf("PUBLISH received (QoS %d, Msg ID %d), Payload (%d bytes): ", qos, msg_id, payload_len);
                    for (size_t i = 0U; i < (size_t)payload_len; i++) {
                        printf("%02X ", payload[i]);
                    }
                    printf("\n");

                    // handle text commands embedded in binary
                    if (payload_len == 6 && memcmp(payload, "led on", 6) == 0) {
                        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1U);
                    } else if (payload_len == 7 && memcmp(payload, "led off", 7) == 0) {
                        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0U);
                    }

                    // QoS ACKs
                    if (qos == 1U) {
                        mqtt_sn_send_puback(pcb, addr, port, topic_id, msg_id, MQTTSN_RETURN_ACCEPTED);
                    } else if (qos == 2U) {
                        mqtt_sn_send_pubrec(pcb, addr, port, msg_id);
                    }
                }
            }
        }

        // PUBACK QoS 1 RECEIVED
        else if (msg_type == MQTTSN_MSG_TYPE_PUBACK) {
            uint16_t msg_id = (data[4] << 8) | data[5];
            printf("PUBACK received for Msg ID: %d \n", msg_id);
            remove_pending_qos_msg(msg_id);
        }

        // PUBCOMP (QoS 2 final ack) RECEIVED
        else if (msg_type == MQTTSN_MSG_TYPE_PUBCOMP) {
            uint16_t msg_id = (data[2] << 8) | data[3];
            printf("PUBCOMP received for Msg ID: %d\n", msg_id);
            remove_pending_qos_msg(msg_id);
        }
        // PUBREC (QoS 2 Step 1) RECEIVED
        else if (msg_type == MQTTSN_MSG_TYPE_PUBREC) {
            uint16_t msg_id = (data[2] << 8) | data[3];
            printf("PUBREC received for Msg ID: %d. Sending PUBREL...\n", msg_id);
            // Send PUBREL in response
            mqtt_sn_send_pubrel(pcb, addr, port, msg_id);
            // Update step of retransmission packet to indicate PUBREL was sent
            for (size_t i = 0U; i < MAX_PENDING_QOS_MSGS; i++) {
                if (g_pending_msgs[i].in_use && g_pending_msgs[i].msg_id == msg_id) {
                    g_pending_msgs[i].step = 1U;
                    g_pending_msgs[i].timestamp = get_absolute_time();  // reset timer
                    break;
                }
            }
        }
        // PUBREL (QoS 2 Step 2) RECEIVED
        else if (msg_type == MQTTSN_MSG_TYPE_PUBREL) {
            // send PUBCOMP
            uint16_t msg_id = (data[2] << 8) | data[3];
            printf("PUBREL received for Msg ID: %d. Sending PUBCOMP...\n", msg_id);
            mqtt_sn_send_pubcomp(pcb, addr, port, msg_id);
            remove_pending_qos_msg(msg_id);
        }
        pbuf_free(p);
    } else {
        if (p)
            pbuf_free(p);  // Always free if not handled
    }
}

// ============================================================================
// File Transfer Functions
// ============================================================================

/**
 * @brief Send a file via MQTT-SN by chunking and publishing
 *
 * This function ALWAYS uses QoS 1 for reliable file transfer.
 * QoS 1 ensures each chunk is acknowledged and retransmitted if lost.
 *
 * @param pcb UDP PCB
 * @param gw_addr Gateway address
 * @param gw_port Gateway port
 * @param filename Name of file to send from microSD
 *
 * @note QoS 1 may result in duplicate chunks being received, but the
 *       chunk_transfer module handles duplicates by checking the bitmap.
 *
 * @warning This function contains blocking SD card reads (~5ms per chunk).
 *          To prevent network stack starvation, cyw43_arch_poll() is called
 *          after each SD read and during inter-chunk delays to process
 *          incoming PUBACKs and prevent spurious QoS 1 retransmissions.
 */
void send_file_via_mqtt(struct udp_pcb* pcb,
                        const ip_addr_t* gw_addr,
                        u16_t gw_port,
                        const char* filename) {
    struct Metadata metadata = {0};

    printf("\n=== Starting STREAMING File Transfer ===\n");
    printf("File: %s\n", filename);
    printf("Using QoS %d for reliable delivery\n", FILE_TRANSFER_QOS);
    printf("Mode: STREAMING (memory efficient)\n");

    // Step 1: Initialize streaming read
    if (init_streaming_read((char*)filename, &metadata) != 0) {
        printf("ERROR: Failed to initialize streaming read\n");
        return;
    }

    printf("✓ File opened for streaming:\n");
    printf("  File size: %lu bytes\n", (unsigned long)metadata.total_size);
    printf("  Chunks: %lu\n", (unsigned long)metadata.chunk_count);
    printf("  Session ID: %s\n", metadata.session_id);
    printf("  File CRC: 0x%04X\n", metadata.file_crc);

    // Step 2: Serialize and send metadata (chunk 0)
    uint8_t meta_buffer[PAYLOAD_SIZE];
    if (serialize_metadata(&metadata, meta_buffer) != PAYLOAD_SIZE) {
        printf("ERROR: Failed to serialize metadata\n");
        cleanup_streaming_read();
        return;
    }

    // Publish metadata to topic ID 3 (file/meta) with QoS 1
    uint16_t msg_id = get_next_msg_id();
    mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port, 3,
                             meta_buffer, PAYLOAD_SIZE, FILE_TRANSFER_QOS, msg_id, false);
    printf("Sent metadata (QoS %d, msg_id=%u)\n", FILE_TRANSFER_QOS, msg_id);
    sleep_ms(100);  // Allow time for ACK

    // Step 3: Stream chunks one at a time (memory efficient!)
    printf("Streaming chunks...\n");
    uint32_t chunks_sent = 0;
    absolute_time_t start_time = get_absolute_time();

    for (uint32_t i = 0; i < metadata.chunk_count; i++) {
        struct Payload chunk = {0};

        // Read chunk from SD card (BLOCKING: ~5ms)
        if (read_chunk_streaming(i, &chunk) != 0) {
            printf("ERROR: Failed to read chunk %lu\n", (unsigned long)i);
            cleanup_streaming_read();
            return;
        }

        // Poll network immediately after blocking SD read to process any pending PUBACKs
        cyw43_arch_poll();

        // Verify chunk integrity before sending
        if (!verify_chunk(&chunk)) {
            printf("ERROR: Chunk %lu failed verification\n", (unsigned long)i);
            cleanup_streaming_read();
            return;
        }

        // Serialize chunk
        uint8_t payload_buffer[PAYLOAD_SIZE];
        if (serialize_payload(&chunk, payload_buffer) != PAYLOAD_SIZE) {
            printf("ERROR: Failed to serialize chunk %lu\n", (unsigned long)i);
            continue;
        }

        // Publish to topic ID 4 (file/data) with QoS 1
        msg_id = get_next_msg_id();
        mqtt_sn_publish_topic_id(pcb, gw_addr, gw_port, 4,
                                 payload_buffer, PAYLOAD_SIZE, FILE_TRANSFER_QOS, msg_id, false);

        chunks_sent++;

        // Progress updates
        if ((i + 1) % 10 == 0 || i == metadata.chunk_count - 1) {
            float progress = ((float)(i + 1) / metadata.chunk_count) * 100.0f;
            printf("  [%lu/%lu] %.1f%% complete (msg_id=%u)\n",
                   (unsigned long)(i + 1),
                   (unsigned long)metadata.chunk_count,
                   progress,
                   msg_id);
        }

        // Interleaved delay with network polling to process incoming PUBACKs
        // This prevents QoS 1 timeout/retransmission during file transfer
        absolute_time_t delay_start = get_absolute_time();
        while (absolute_time_diff_us(delay_start, get_absolute_time()) < 50000) {
            cyw43_arch_poll();  // Process network events
            sleep_us(100);      // Yield CPU briefly
        }
    }

    absolute_time_t end_time = get_absolute_time();
    int64_t elapsed_ms = absolute_time_diff_us(start_time, end_time) / 1000;

    // Cleanup streaming session
    cleanup_streaming_read();

    printf("✓ File transfer complete:\n");
    printf("  Total packets: %lu (1 metadata + %lu data)\n",
           (unsigned long)(chunks_sent + 1),
           (unsigned long)chunks_sent);
    printf("  Time: %lld ms\n", elapsed_ms);
    printf("  Throughput: %.2f KB/s\n",
           (float)metadata.total_size / (elapsed_ms / 1000.0f) / 1024.0f);
    printf("  Peak memory: ~0.5 KB (single chunk buffer)\n");
    printf("==============================\n\n");
}

/**
 * @brief Handle received file metadata packet
 *
 * Processes the initial metadata chunk that describes the file transfer.
 * Sent via QoS 1 for reliability. If duplicate metadata is received
 * (from retransmission), the session will be reinitialized.
 *
 * @param ctx MQTT-SN context containing session info
 * @param payload Serialized metadata payload
 * @param len Payload length
 *
 * @note Uses QoS 1 for reliable metadata delivery
 */
void handle_file_metadata(mqtt_sn_context_t* ctx, const uint8_t* payload, size_t len) {
    if (!ctx || !payload) {
        printf("ERROR: NULL parameter in handle_file_metadata\n");
        return;
    }

    if (len != PAYLOAD_SIZE) {
        printf("ERROR: Invalid metadata size (expected %d, got %zu)\n", PAYLOAD_SIZE, len);
        return;
    }

    struct Metadata metadata = {0};
    if (deserialize_metadata((uint8_t*)payload, &metadata) != 0) {
        printf("ERROR: Failed to deserialize metadata\n");
        return;
    }

    printf("\n=== File Transfer Started ===\n");
    printf("Received metadata:\n");
    printf("  Session ID: %s\n", metadata.session_id);
    printf("  Filename:   %s\n", metadata.filename);
    printf("  Size:       %lu bytes\n", (unsigned long)metadata.total_size);
    printf("  Chunks:     %lu\n", (unsigned long)metadata.chunk_count);
    printf("  File CRC:   0x%04X\n", metadata.file_crc);

    // Initialize filesystem if not already done
    if (!ctx->fs_info) {
        printf("ERROR: Filesystem not initialized\n");
        return;
    }

    // Initialize transfer session
    if (!ctx->file_session) {
        printf("ERROR: No session buffer allocated\n");
        return;
    }

    if (!chunk_transfer_init_session(ctx->fs_info, &metadata, ctx->file_session)) {
        printf("ERROR: Failed to init transfer session\n");
        return;
    }

    ctx->transfer_in_progress = true;
    printf("✓ Transfer session active\n");
    printf("=============================\n\n");
}

/**
 * @brief Handle received file payload chunk
 *
 * Processes incoming file chunks sent via QoS 1.
 * QoS 1 guarantees at-least-once delivery, which may result in duplicate chunks.
 * Duplicates are automatically detected and safely ignored using the chunk bitmap.
 *
 * @param ctx MQTT-SN context containing session info
 * @param payload Serialized payload chunk
 * @param len Payload length
 *
 * @note Duplicate chunks (from QoS 1 retransmissions) are handled gracefully:
 *       - CRC is verified to ensure data integrity
 *       - Bitmap is checked to detect duplicates
 *       - Duplicates are skipped without error
 *       - Only new chunks are written to microSD
 */
void handle_file_payload(mqtt_sn_context_t* ctx, const uint8_t* payload, size_t len) {
    if (!ctx || !payload) {
        printf("ERROR: NULL parameter in handle_file_payload\n");
        return;
    }

    if (!ctx->transfer_in_progress) {
        printf("WARNING: Received payload but no active session\n");
        return;
    }

    if (len != PAYLOAD_SIZE) {
        printf("ERROR: Invalid payload size (expected %d, got %zu)\n", PAYLOAD_SIZE, len);
        return;
    }

    struct Payload chunk = {0};
    if (deserialize_payload((uint8_t*)payload, &chunk) != 0) {
        printf("ERROR: Failed to deserialize payload\n");
        return;
    }

    // Verify chunk integrity with CRC16
    if (!verify_chunk(&chunk)) {
        printf("ERROR: Chunk %lu failed CRC check\n", (unsigned long)chunk.sequence);
        return;
    }

    // Write chunk to microSD
    // Note: chunk_transfer_write_payload automatically handles duplicates by checking
    // the bitmap and skipping chunks that have already been received.
    // This is essential for QoS 1 which may deliver duplicates.
    if (!chunk_transfer_write_payload(ctx->fs_info, ctx->file_session, &chunk)) {
        printf("ERROR: Failed to write chunk %lu\n", (unsigned long)chunk.sequence);
        return;
    }

    uint32_t received, total;
    chunk_transfer_get_progress(ctx->file_session, &received, &total);

    if (received % 10 == 0 || received == total) {
        printf("Progress: %lu/%lu chunks received\n", (unsigned long)received, (unsigned long)total);
    }

    // Check if transfer complete
    if (chunk_transfer_is_complete(ctx->file_session)) {
        printf("\n=== File Transfer Complete ===\n");
        printf("All chunks received! Finalizing...\n");

        if (chunk_transfer_finalize(ctx->fs_info, ctx->file_session)) {
            printf("✓ File saved: %s\n", ctx->file_session->filename);
            printf("  Size: %lu bytes\n", (unsigned long)ctx->file_session->metadata.total_size);
        } else {
            printf("ERROR: Failed to finalize transfer\n");
        }

        ctx->transfer_in_progress = false;
        printf("==============================\n\n");
    }
}
