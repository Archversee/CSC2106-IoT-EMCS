#include "mqttsn_bridge.h"
#include "config.h"
#include "MQTTSNPacket.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// lwIP forward decls come from headers included elsewhere in your build.
// If your compiler complains, add these includes here:
//   #include "lwip/udp.h"
//   #include "lwip/pbuf.h"
//   #include "lwip/ip_addr.h"

// -----------------------------------------------------------------------------
// Global bridge state
// -----------------------------------------------------------------------------
static struct udp_pcb *g_pcb = NULL;
static ip_addr_t g_gateway_addr;
static u16_t g_gateway_port;
static unsigned char g_sendbuf[256];

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------
void mqttsn_bridge_init(struct udp_pcb *pcb, ip_addr_t *gateway_addr, u16_t gateway_port) {
    g_pcb = pcb;
    g_gateway_addr = *gateway_addr;
    g_gateway_port = gateway_port;
    printf("[MQTTSN Bridge] Initialized with gateway %s:%d\n",
           ip4addr_ntoa(gateway_addr), gateway_port);
}

// -----------------------------------------------------------------------------
// Internal helpers
// -----------------------------------------------------------------------------
static uint16_t next_msg_id(void) {
    static uint16_t mid = 1;
    if (mid == 0) mid = 1;
    return mid++;
}

// Send an MQTT-SN PUBLISH using a **long topic name** (normal string topic)
static void mqttsn_send_publish(const char *topic, const char *payload) {
    if (!g_pcb) {
        printf("[MQTTSN Bridge] ERROR: Not initialized!\n");
        return;
    }

    MQTTSN_topicid topicId;
    topicId.type = MQTTSN_TOPIC_TYPE_NORMAL;        // long topic name
    topicId.data.long_.name = (unsigned char *)topic;
    topicId.data.long_.len  = (int)strlen(topic);

    int len = MQTTSNSerialize_publish(
        g_sendbuf, sizeof(g_sendbuf),
        /*dup*/0,
        /*qos*/0,                                    // QoS 0 is fine for dashboard/status
        /*retained*/0,
        /*msgId*/0,                                   // 0 when using long topic name
        topicId,
        (unsigned char *)payload,
        (int)strlen(payload)
    );

    if (len <= 0) {
        printf("[MQTTSN Bridge] PUBLISH serialize failed (topic=%s)\n", topic);
        return;
    }

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    if (!p) {
        printf("[MQTTSN Bridge] PUBLISH alloc failed\n");
        return;
    }
    memcpy(p->payload, g_sendbuf, len);
    udp_sendto(g_pcb, p, &g_gateway_addr, g_gateway_port);
    pbuf_free(p);
    printf("[MQTTSN Bridge] Published to %s: %s\n", topic, payload);
}

// Send a SUBSCRIBE to a **predefined topic id** (maps via predefinedTopic.conf)
static void mqttsn_send_subscribe_predef(uint16_t topic_id, int qos) {
    if (!g_pcb) {
        printf("[MQTTSN Bridge] ERROR: Not initialized!\n");
        return;
    }

    unsigned char buf[64];
    MQTTSN_topicid t;
    t.type   = MQTTSN_TOPIC_TYPE_PREDEFINED;
    t.data.id = topic_id;

    int len = MQTTSNSerialize_subscribe(
        buf, sizeof(buf),
        /*dup*/0,
        /*qos*/qos,
        /*msgId*/next_msg_id(),
        &t
    );

    if (len <= 0) {
        printf("[MQTTSN Bridge] SUBSCRIBE serialize failed (id=%u)\n", topic_id);
        return;
    }

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    if (!p) {
        printf("[MQTTSN Bridge] SUBSCRIBE alloc failed (id=%u)\n", topic_id);
        return;
    }
    memcpy(p->payload, buf, len);
    udp_sendto(g_pcb, p, &g_gateway_addr, g_gateway_port);
    pbuf_free(p);
    printf("[MQTTSN Bridge] SUBSCRIBE sent (predef id=%u, qos=%d)\n", topic_id, qos);
}

// -----------------------------------------------------------------------------
// Public: call this once **after** you receive CONNACK
// -----------------------------------------------------------------------------
void mqttsn_bridge_subscribe_standard_topics(void) {
    // Matches your config.h and predefinedTopic.conf
    mqttsn_send_subscribe_predef(TOPIC_ID_PICO_CMD, 0);  // pico/cmd  (ID 1)  QoS 0
    mqttsn_send_subscribe_predef(TOPIC_ID_FILE_META, 1); // file/meta (ID 3)  QoS 1
    mqttsn_send_subscribe_predef(TOPIC_ID_FILE_DATA, 1); // file/data (ID 4)  QoS 1
}

// -----------------------------------------------------------------------------
// Existing publish helpers (unchanged API)
// -----------------------------------------------------------------------------
void mqttsn_pub_file_status(const char *device_id, const char *filename, const char *status, const char *reason) {
    char topic[128];
    char payload[256];

    // Dashboard expects long topic names for display
    snprintf(topic, sizeof(topic), "devices/%s/file-transfer/status", device_id);

    if (reason) {
        snprintf(payload, sizeof(payload),
                 "{\"status\":\"%s\",\"fileName\":\"%s\",\"reason\":\"%s\"}",
                 status, filename, reason);
    } else {
        snprintf(payload, sizeof(payload),
                 "{\"status\":\"%s\",\"fileName\":\"%s\"}",
                 status, filename);
    }

    mqttsn_send_publish(topic, payload);
}

void mqttsn_pub_file_progress(const char *device_id, unsigned chunk, unsigned total, unsigned seq, unsigned checksum) {
    char topic[128];
    char payload[256];

    snprintf(topic, sizeof(topic), "devices/%s/file-transfer/progress", device_id);
    snprintf(payload, sizeof(payload),
             "{\"chunk\":%u,\"total\":%u,\"seq\":%u,\"checksum\":\"%x\"}",
             chunk, total, seq, checksum);

    mqttsn_send_publish(topic, payload);
}

void mqttsn_pub_file_validation(const char *device_id, const char *result, const char *expected, const char *actual) {
    char topic[128];
    char payload[256];

    snprintf(topic, sizeof(topic), "devices/%s/file-transfer/validation", device_id);
    snprintf(payload, sizeof(payload),
             "{\"result\":\"%s\",\"expected\":\"%s\",\"actual\":\"%s\"}",
             result, expected, actual);

    mqttsn_send_publish(topic, payload);
}