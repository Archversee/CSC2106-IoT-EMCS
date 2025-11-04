// mqtt-sn-udp.h
#ifndef MQTT_SN_UDP_H
#define MQTT_SN_UDP_H

/*
 * MQTT-SN UDP Protocol Implementation with File Transfer Support
 *
 * FILE TRANSFER QOS POLICY:
 * -------------------------
 * Metadata chunks use QoS 2 (exactly-once delivery) for guaranteed reception.
 * Data chunks use QoS 1 (at-least-once delivery) for reliable transmission.
 *
 * QoS 2 for Metadata (PUBREC/PUBREL/PUBCOMP handshake):
 * - Guarantees exactly-once delivery
 * - Ensures session initialization before any data chunks
 * - No data chunks accepted without metadata
 *
 * QoS 1 for Data Chunks (PUBACK):
 * - Every packet is acknowledged (PUBACK)
 * - Unacknowledged packets are automatically retransmitted
 * - May result in duplicate packet delivery
 *
 * Duplicate Handling:
 * - Each chunk has a unique sequence number and CRC16
 * - Bitmap tracks which chunks have been received
 * - Duplicate chunks are detected and safely ignored
 * - No duplicate data is written to microSD
 *
 * This approach ensures reliable file transfer while handling
 * network packet loss and duplicate deliveries gracefully.
 */

#include "../config.h"
#include "../drivers/microsd_driver.h"
#include "../fs/chunk_transfer.h"
#include "../fs/data_frame.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"

/*! File Transfer Configuration */
#define FILE_TRANSFER_METADATA_QOS 2U /*!< QoS level for metadata (2=exactly-once delivery) */
#define FILE_TRANSFER_DATA_QOS \
    1U                                     /*!< QoS level for data chunks (1=at-least-once with duplicates handled) */
#define FILE_TRANSFER_TOPIC_METADATA 3U    /*!< Topic ID for file metadata (file/meta) */
#define FILE_TRANSFER_TOPIC_DATA 4U        /*!< Topic ID for file data chunks (file/data) */
#define METADATA_CONFIRM_TIMEOUT_MS 5000U  // 5 second timeout

/*! MQTT-SN Packet Structure Constants */
#define MQTTSN_HEADER_SIZE 2U          /*!< Minimum header size (length + type) */
#define MQTTSN_CONNECT_FIXED_LEN 6U    /*!< CONNECT packet fixed fields length */
#define MQTTSN_SUBSCRIBE_LEN 7U        /*!< SUBSCRIBE packet length */
#define MQTTSN_PUBACK_LEN 7U           /*!< PUBACK packet length */
#define MQTTSN_PUBREC_LEN 5U           /*!< PUBREC packet length */
#define MQTTSN_PUBCOMP_LEN 5U          /*!< PUBCOMP packet length */
#define MQTTSN_PUBREL_LEN 4U           /*!< PUBREL packet length */
#define MQTTSN_PINGREQ_LEN 2U          /*!< PINGREQ packet length */
#define MQTTSN_MAX_PACKET_LEN 255U     /*!< Maximum MQTT-SN packet length (1-byte length field) */
#define MQTTSN_PUBLISH_HEADER_LEN 7U   /*!< PUBLISH packet header length (without payload) */
#define MQTTSN_RETRY_PAYLOAD_SIZE 247U /*!< Max payload size for retry buffer (255 - 7 - 1) */

/*! MQTT-SN Packet Field Offsets */
#define MQTTSN_OFFSET_LENGTH 0U        /*!< Offset: packet length field */
#define MQTTSN_OFFSET_MSG_TYPE 1U      /*!< Offset: message type field */
#define MQTTSN_OFFSET_FLAGS 2U         /*!< Offset: flags field */
#define MQTTSN_OFFSET_PROTOCOL_ID 3U   /*!< Offset: protocol ID field */
#define MQTTSN_OFFSET_DURATION_HIGH 4U /*!< Offset: duration high byte */
#define MQTTSN_OFFSET_DURATION_LOW 5U  /*!< Offset: duration low byte */
#define MQTTSN_OFFSET_CLIENT_ID 6U     /*!< Offset: client ID field in CONNECT */
#define MQTTSN_OFFSET_TOPIC_ID_HIGH 3U /*!< Offset: topic ID high byte in PUBLISH */
#define MQTTSN_OFFSET_TOPIC_ID_LOW 4U  /*!< Offset: topic ID low byte in PUBLISH */
#define MQTTSN_OFFSET_MSG_ID_HIGH 5U   /*!< Offset: message ID high byte */
#define MQTTSN_OFFSET_MSG_ID_LOW 6U    /*!< Offset: message ID low byte */
#define MQTTSN_OFFSET_PAYLOAD 7U       /*!< Offset: payload start in PUBLISH */
#define MQTTSN_OFFSET_RETURN_CODE 2U   /*!< Offset: return code in CONNACK */

/*! QoS Levels */
#define QOS_LEVEL_0 0 /*!< QoS 0: At most once (fire and forget) */
#define QOS_LEVEL_1 1 /*!< QoS 1: At least once (acknowledged delivery) */
#define QOS_LEVEL_2 2 /*!< QoS 2: Exactly once (assured delivery) */

/*! Bit Manipulation Constants */
#define BITS_PER_BYTE 8U /*!< Number of bits in a byte */

/*! LED Command Constants */
#define LED_ON_CMD_LEN 6U  /*!< Length of "led on" command */
#define LED_OFF_CMD_LEN 7U /*!< Length of "led off" command */

/*! File Transfer Validation Limits */
#define MAX_CHUNK_COUNT 100000U                   /*!< Maximum allowed chunk count */
#define MAX_FILE_SIZE_BYTES (10U * 1024U * 1024U) /*!< Maximum file size: 10 MB */

/*! Timing Constants */
#define QOS2_HANDSHAKE_DELAY_MS 150U /*!< Delay for QoS 2 handshake completion */
#define INTER_CHUNK_DELAY_US 50000U  /*!< Inter-chunk delay (microseconds) */
#define POLL_YIELD_DELAY_US 100U     /*!< CPU yield delay during polling */
#define PROGRESS_UPDATE_INTERVAL 10U /*!< Report progress every N chunks */

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

typedef struct {
    uint16_t msg_id;
    uint8_t qos;
    uint8_t step;  // 0 = PUBLISH sent, 1 = PUBREL sent (for QoS 2)
    absolute_time_t timestamp;
    uint8_t retry_count;
    uint16_t topic_id;
    uint8_t payload[255];  // Binary-safe payload buffer
    size_t payload_len;    // Actual payload length
    bool in_use;
} qos_msg_t;

typedef struct {
    bool drop_acks;
    transfer_session_t* file_session;
    filesystem_info_t* fs_info;
    bool transfer_in_progress;
} mqtt_sn_context_t;

extern qos_msg_t g_pending_msgs[MAX_PENDING_QOS_MSGS];
extern uint32_t g_last_pingresp;
extern bool g_ping_ack_received;

// Public function declarations
void mqtt_sn_connect(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port);
void mqtt_sn_pingreq(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port);
void mqtt_sn_subscribe_topic_id(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port,
                                u16_t topic_id);
void mqtt_sn_publish_topic_id(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port,
                              uint16_t topic_id, const uint8_t* payload, size_t payload_len,
                              int qos, uint16_t msg_id, bool is_retransmit);
void mqtt_sn_send_pubrel(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port,
                         uint16_t msg_id);
void mqtt_sn_send_puback(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port,
                         uint16_t topic_id, uint16_t msg_id, uint8_t return_code);
void mqtt_sn_send_pubrec(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port,
                         uint16_t msg_id);
void mqtt_sn_send_pubcomp(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port,
                          uint16_t msg_id);
void udp_recv_callback(void* arg, struct udp_pcb* pcb, struct pbuf* p, const ip_addr_t* addr,
                       u16_t port);
void check_qos_timeouts(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port);
void remove_pending_qos_msg(uint16_t msg_id);
uint16_t get_next_msg_id(void);

// File transfer functions
void send_file_via_mqtt(struct udp_pcb* pcb, const ip_addr_t* gw_addr, u16_t gw_port,
                        const char* filename);
void handle_file_metadata(mqtt_sn_context_t* ctx, const uint8_t* payload, size_t len,
                          struct udp_pcb* pcb, const ip_addr_t* addr, u16_t port);
void handle_file_payload(mqtt_sn_context_t* ctx, const uint8_t* payload, size_t len);

#endif  // MQTT_SN_UDP_H
