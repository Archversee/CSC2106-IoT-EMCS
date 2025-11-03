#include "mqttsn_bridge.h"
#include "MQTTSNPacket.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// Global bridge state
static struct udp_pcb *g_pcb = NULL;
static ip_addr_t g_gateway_addr;
static u16_t g_gateway_port;
static unsigned char g_sendbuf[256];

void mqttsn_bridge_init(struct udp_pcb *pcb, ip_addr_t *gateway_addr, u16_t gateway_port) {
    g_pcb = pcb;
    g_gateway_addr = *gateway_addr;
    g_gateway_port = gateway_port;
    printf("[MQTTSN Bridge] Initialized with gateway %s:%d\n", 
           ip4addr_ntoa(gateway_addr), gateway_port);
}

// Helper function to send MQTT-SN publish message
static void mqttsn_send_publish(const char *topic, const char *payload) {
    if (!g_pcb) {
        printf("[MQTTSN Bridge] ERROR: Not initialized!\n");
        return;
    }

    int len = MQTTSNSerialize_publish(g_sendbuf, sizeof(g_sendbuf), 0, 0, 0, 0,
                                      topic, (unsigned char*)payload, strlen(payload));
    
    if (len > 0) {
        struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
        if (p) {
            memcpy(p->payload, g_sendbuf, len);
            udp_sendto(g_pcb, p, &g_gateway_addr, g_gateway_port);
            pbuf_free(p);
            printf("[MQTTSN Bridge] Published to %s: %s\n", topic, payload);
        }
    }
}

void mqttsn_pub_file_status(const char *device_id, const char *filename, const char *status, const char *reason) {
    char topic[128];
    char payload[256];
    
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