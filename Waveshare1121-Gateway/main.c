/*
 * main.c — Bidirectional LoRa <-> UDP Bridge for MQTT-SN  (mesh-aware)
 *
 * Hardware : Raspberry Pi 4 + Waveshare LR1121
 * Upstream : Paho MQTT-SN Gateway localhost:10000 (UDP)
 * Nodes    : Arduino + Cytron RFM95
 *
 * ── What changed vs original ──────────────────────────────────────────────
 *
 * UPLINK (Node → Gateway):
 *   Packets now carry a 4-byte mesh header before the MQTT-SN payload.
 *   on_rx_done() strips this header, uses SRC_ID as the stable client key,
 *   and logs TTL/hop_count for performance analysis.
 *
 * DOWNLINK (Gateway → Node):
 *   Paho sends raw MQTT-SN bytes (SUBACK, CONNACK, REGACK etc.) back via UDP.
 *   The downstream thread now WRAPS these with a mesh header before sending
 *   over LoRa so that relay nodes can forward them if needed.
 *   Header: src=GATEWAY(0x00), dst=target_node, ttl=MESH_TTL_DEFAULT, seq=dn_seq
 *
 * ── Wire format ───────────────────────────────────────────────────────────
 *   RadioHead header (4B): TO | FROM | ID | FLAGS
 *   Mesh header      (4B): SRC_ID | DST_ID | TTL | SEQ_NUM
 *   MQTT-SN payload  (NB)
 *
 * ── Threading ─────────────────────────────────────────────────────────────
 *   Main thread       — polls irq_flag → irq_process() → on_rx_done()
 *   Downstream thread — select() on per-client UDP sockets → lora_send()
 *   clients_lock      — protects client table
 *   radio_lock        — serialises all SPI/radio access
 */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "lr1121_config.h"
#include "mesh_protocol.h" /* shared with Arduino nodes */
#include "wavesahre_lora_1121.h"

/* ── Configuration ───────────────────────────────────────────────────────── */
#define PAHO_GW_IP "127.0.0.1"
#define PAHO_GW_PORT 10000

#define MAX_CLIENTS 30
#define CLIENT_PORT_BASE 20000
#define CLIENT_TIMEOUT_S 300
#define GW_PAYLOAD_MAX 255
#define RX_CONTINUOUS 0xFFFFFF
#define DN_SPACING_US 800000ULL

/* ── RadioHead header layout ─────────────────────────────────────────────── */
#define RH_TO 0
#define RH_FROM 1
#define RH_ID 2
#define RH_FLAGS 3
#define RH_HDR 4

/* Total overhead per outgoing LoRa frame */
#define FRAME_HDR_TOTAL (RH_HDR + MESH_HEADER_SIZE)

/* ── IRQ mask ────────────────────────────────────────────────────────────── */
#define IRQ_MASK                                                                                   \
    (LR11XX_SYSTEM_IRQ_TX_DONE | LR11XX_SYSTEM_IRQ_RX_DONE | LR11XX_SYSTEM_IRQ_TIMEOUT |           \
     LR11XX_SYSTEM_IRQ_CRC_ERROR | LR11XX_SYSTEM_IRQ_HEADER_ERROR)

/* ── Range simulation */
#define SIMULATE_RANGE_LIMIT 1                      /* set to 0 to disable */
static const uint8_t OUT_OF_RANGE[] = {0x22, 0x24}; /* arduino-02, arduino-04 */

/* ── Per-client state ────────────────────────────────────────────────────── */
typedef struct {
    uint8_t node_id; /* SRC_ID of the originating end-device          */
    int udp_sock;
    uint16_t local_port;
    struct sockaddr_in paho_addr;
    uint8_t last_seq;
    time_t last_seen;
    bool active;
    /* Performance counters */
    uint32_t pkt_count;
    uint32_t hop_total; /* sum of (TTL_DEFAULT - remaining TTL) per pkt  */
    uint8_t dn_seq;     // per-client downlink sequence number
} client_t;

static client_t clients[MAX_CLIENTS];
static pthread_mutex_t clients_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t radio_lock = PTHREAD_MUTEX_INITIALIZER;

/* ── Globals ─────────────────────────────────────────────────────────────── */
lr1121_t lr1121;

static volatile bool irq_flag = false;
static volatile int running = 1;
static volatile int pkt_rx_count = 0;
static volatile int pkt_tx_count = 0;

/* ── Gateway-side dedup ───────────────────────────────────────────────────── */
/* Prevents duplicate uplink packets (flooding sends same pkt via multiple
   paths) from being forwarded to Paho twice, which causes double REGACKs
   that confuse endpoint nodes during topic registration.               */
#define GW_DEDUP_SIZE 16
typedef struct {
    uint8_t src;
    uint8_t seq;
    bool valid;
} gw_dedup_t;
static gw_dedup_t gw_dedup[GW_DEDUP_SIZE];
static uint8_t gw_dedup_head = 0;

static bool gw_dedup_seen(uint8_t src, uint8_t seq) {
    for (int i = 0; i < GW_DEDUP_SIZE; i++)
        if (gw_dedup[i].valid && gw_dedup[i].src == src && gw_dedup[i].seq == seq)
            return true;
    return false;
}
static void gw_dedup_add(uint8_t src, uint8_t seq) {
    gw_dedup[gw_dedup_head] = (gw_dedup_t){src, seq, true};
    gw_dedup_head = (gw_dedup_head + 1) % GW_DEDUP_SIZE;
}

/* ── Logging ─────────────────────────────────────────────────────────────── */
static void print_ts(void) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("[%s] ", ts);
}
#define LOG(fmt, ...)                                                                              \
    do {                                                                                           \
        print_ts();                                                                                \
        printf(fmt "\n", ##__VA_ARGS__);                                                           \
        fflush(stdout);                                                                            \
    } while (0)

/* ── ISR / signal ────────────────────────────────────────────────────────── */
static void on_rx_timeout(void) { LOG("[rx] Timeout"); }
static void on_rx_crc_error(void) { LOG("[rx] CRC error"); }
void isr(void) { irq_flag = true; }
static void on_signal(int sig) {
    (void)sig;
    running = 0;
}

/* ── Client registry ─────────────────────────────────────────────────────── */

/* Keyed on mesh SRC_ID (original end-device), not RadioHead FROM.
   Must be called with clients_lock held. */
static client_t *get_or_create_client(uint8_t src_id, bool is_connect) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].node_id == src_id) {
            if (is_connect) {
                // Check if client ID changed (different node reusing same port)
                clients[i].pkt_count = 0;
                clients[i].hop_total = 0;
                clients[i].last_seq = 0;
                clients[i].dn_seq = 0;

                // Force Paho to see a fresh connection by closing/reopening socket
                close(clients[i].udp_sock);

                // Rebind to same port
                clients[i].udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
                int reuse = 1;
                setsockopt(clients[i].udp_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
                struct sockaddr_in local = {0};
                local.sin_family = AF_INET;
                local.sin_port = htons(clients[i].local_port);
                local.sin_addr.s_addr = INADDR_ANY;
                bind(clients[i].udp_sock, (struct sockaddr *)&local, sizeof(local));

                // Reset Paho address
                memset(&clients[i].paho_addr, 0, sizeof(clients[i].paho_addr));
                clients[i].paho_addr.sin_family = AF_INET;
                clients[i].paho_addr.sin_port = htons(PAHO_GW_PORT);
                inet_pton(AF_INET, PAHO_GW_IP, &clients[i].paho_addr.sin_addr);

                LOG("[client] Reset socket for src=0x%02X port=%d", src_id, clients[i].local_port);
            }
            clients[i].last_seen = time(NULL);
            return &clients[i];
        }
    }
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active)
            continue;

        clients[i].node_id = src_id;
        clients[i].active = true;
        clients[i].last_seen = time(NULL);
        clients[i].last_seq = 0;
        clients[i].pkt_count = 0;
        clients[i].hop_total = 0;
        clients[i].local_port = (uint16_t)(CLIENT_PORT_BASE + i);

        clients[i].udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (clients[i].udp_sock < 0) {
            LOG("[client] socket() failed: %s", strerror(errno));
            clients[i].active = false;
            return NULL;
        }
        int reuse = 1;
        setsockopt(clients[i].udp_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

        struct sockaddr_in local = {0};
        local.sin_family = AF_INET;
        local.sin_port = htons(clients[i].local_port);
        local.sin_addr.s_addr = INADDR_ANY;
        if (bind(clients[i].udp_sock, (struct sockaddr *)&local, sizeof(local)) < 0) {
            LOG("[client] bind() port %d failed: %s", clients[i].local_port, strerror(errno));
            close(clients[i].udp_sock);
            clients[i].active = false;
            return NULL;
        }

        memset(&clients[i].paho_addr, 0, sizeof(clients[i].paho_addr));
        clients[i].paho_addr.sin_family = AF_INET;
        clients[i].paho_addr.sin_port = htons(PAHO_GW_PORT);
        inet_pton(AF_INET, PAHO_GW_IP, &clients[i].paho_addr.sin_addr);

        LOG("[client] New src=0x%02X -> port=%d", src_id, clients[i].local_port);
        return &clients[i];
    }
    LOG("[client] Table full — cannot register src=0x%02X", src_id);
    return NULL;
}

static void expire_clients(void) {
    time_t now = time(NULL);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active)
            continue;
        if ((now - clients[i].last_seen) < CLIENT_TIMEOUT_S)
            continue;
        LOG("[client] Expiring src=0x%02X (idle %lds, pkts=%u, avg_hops=%.2f)", clients[i].node_id,
            (long)(now - clients[i].last_seen), clients[i].pkt_count,
            clients[i].pkt_count > 0 ? (double)clients[i].hop_total / clients[i].pkt_count : 0.0);
        close(clients[i].udp_sock);
        memset(&clients[i], 0, sizeof(clients[i]));
    }
}

/* ── LoRa TX ─────────────────────────────────────────────────────────────── */

/*
 * lora_send()
 * Sends a DOWNLINK packet to a specific node, wrapped with:
 *   RadioHead header (4B): TO=dst, FROM=GATEWAY, ID=seq, FLAGS=0
 *   Mesh header      (4B): src=GATEWAY, dst=dst_node, ttl=MESH_TTL_DEFAULT, seq=seq
 *   MQTT-SN payload  (NB)
 *
 * Caller must hold radio_lock; must NOT hold clients_lock.
 */
static int lora_send(uint8_t dst_node, uint8_t seq, const uint8_t *mqttsn_payload,
                     uint8_t mqttsn_len) {
    if (mqttsn_len + FRAME_HDR_TOTAL > GW_PAYLOAD_MAX) {
        LOG("[tx] Payload too large (%d bytes), dropping", mqttsn_len);
        return -1;
    }

    /* Build mesh header */
    mesh_packet_t mesh_pkt;
    mesh_pkt.src_id = MESH_ADDR_GATEWAY;
    mesh_pkt.dst_id = dst_node;
    mesh_pkt.ttl = MESH_TTL_DEFAULT;
    mesh_pkt.seq_num = seq;
    mesh_pkt.payload_len = mqttsn_len;
    memcpy(mesh_pkt.payload, mqttsn_payload, mqttsn_len);

    /* Encode mesh header + payload */
    uint8_t mesh_buf[GW_PAYLOAD_MAX];
    uint8_t mesh_len = mesh_encode(&mesh_pkt, mesh_buf, sizeof(mesh_buf));
    if (mesh_len == 0)
        return -1;

    /* Prepend RadioHead header */
    uint8_t frame[GW_PAYLOAD_MAX];
    frame[RH_TO] = dst_node;
    frame[RH_FROM] = MESH_ADDR_GATEWAY;
    frame[RH_ID] = seq;
    frame[RH_FLAGS] = 0x00;
    memcpy(frame + RH_HDR, mesh_buf, mesh_len);
    uint8_t total = (uint8_t)(RH_HDR + mesh_len);

    /* Put radio into standby before reconfiguring */
    lr11xx_system_set_standby(&lr1121, LR11XX_SYSTEM_STANDBY_CFG_RC);

    lr11xx_radio_mod_params_lora_t mod = {
        .sf = LR11XX_RADIO_LORA_SF7,
        .bw = LR11XX_RADIO_LORA_BW_125,
        .cr = LR11XX_RADIO_LORA_CR_4_5,
        .ldro = 0,
    };
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_mod_params(&lr1121, &mod));
    ASSERT_LR11XX_RC(lr11xx_radio_set_rf_freq(&lr1121, 915000000UL));
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_public_network(&lr1121, false));

    lr11xx_radio_pkt_params_lora_t tx_pkt = {
        .preamble_len_in_symb = 8,
        .header_type = LR11XX_RADIO_LORA_PKT_EXPLICIT,
        .pld_len_in_bytes = total,
        .crc = LR11XX_RADIO_LORA_CRC_ON,
        .iq = LR11XX_RADIO_LORA_IQ_STANDARD,
    };
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_pkt_params(&lr1121, &tx_pkt));
    lr11xx_regmem_write_buffer8(&lr1121, frame, total);
    ASSERT_LR11XX_RC(lr11xx_radio_set_tx(&lr1121, 5000));

    int waited = 0;
    while (!irq_flag && waited < 600) {
        usleep(2000);
        waited += 2;
    }

    lr11xx_system_irq_mask_t irq_regs = 0;
    lr11xx_system_get_and_clear_irq_status(&lr1121, &irq_regs);
    irq_flag = false;

    /* Return to continuous RX */
    lr11xx_radio_pkt_params_lora_t rx_pkt = {
        .preamble_len_in_symb = 8,
        .header_type = LR11XX_RADIO_LORA_PKT_EXPLICIT,
        .pld_len_in_bytes = GW_PAYLOAD_MAX,
        .crc = LR11XX_RADIO_LORA_CRC_ON,
        .iq = LR11XX_RADIO_LORA_IQ_STANDARD,
    };
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_pkt_params(&lr1121, &rx_pkt));
    ASSERT_LR11XX_RC(lr11xx_radio_set_rx(&lr1121, RX_CONTINUOUS));

    pkt_tx_count++;
    LOG("[tx] %dB -> node=0x%02X seq=%d (rh=%dB mesh=%dB mqttsn=%dB)", total, dst_node, seq, RH_HDR,
        MESH_HEADER_SIZE, mqttsn_len);
    return 0;
}

/* ── LoRa RX handler ─────────────────────────────────────────────────────── */

static void gw_dedup_clear_src(uint8_t src) {
    for (int i = 0; i < GW_DEDUP_SIZE; i++) {
        if (gw_dedup[i].valid && gw_dedup[i].src == src) {
            gw_dedup[i].valid = false; // Clear all entries for this src
        }
    }
}

/*
 * on_rx_done()
 *
 * Incoming frame layout:
 *   [RH_TO][RH_FROM][RH_ID][RH_FLAGS]          ← RadioHead header (4B)
 *   [SRC_ID][DST_ID][TTL][SEQ_NUM]              ← Mesh header (4B)
 *   [MQTT-SN payload...]                        ← N bytes
 *
 * We strip both headers and forward bare MQTT-SN bytes to Paho,
 * using SRC_ID as the stable client identity.
 */
static void on_rx_done(void) {
    uint8_t buf[GW_PAYLOAD_MAX + 1];
    lr11xx_radio_rx_buffer_status_t rx_buf;
    lr11xx_radio_pkt_status_lora_t pkt_status;

    lr11xx_radio_get_rx_buffer_status(&lr1121, &rx_buf);
    uint8_t size = rx_buf.pld_len_in_bytes;

    /* Minimum: RH(4) + Mesh(4) + 1 byte MQTT-SN */
    if (size < (uint8_t)(FRAME_HDR_TOTAL + 1) || size > GW_PAYLOAD_MAX) {
        LOG("[rx] Bad size %d, dropping", size);
        return;
    }

    lr11xx_regmem_read_buffer8(&lr1121, buf, rx_buf.buffer_start_pointer, size);
    lr11xx_radio_get_lora_pkt_status(&lr1121, &pkt_status);

    /* ── Strip RadioHead header ──────────────────────────────────────── */
    uint8_t rh_seq = buf[RH_ID];
    uint8_t *mesh = buf + RH_HDR;  /* pointer to mesh header   */
    uint8_t msize = size - RH_HDR; /* bytes from mesh hdr on   */

    /* ── Decode mesh header ─────────────────────────────────────────── */
    mesh_packet_t pkt;
    if (!mesh_decode(mesh, msize, &pkt)) {
        LOG("[rx] mesh_decode failed, dropping");
        return;
    }

    uint8_t hops_taken = MESH_TTL_DEFAULT - pkt.ttl;

/* ── Range simulation ── */
#if SIMULATE_RANGE_LIMIT
    for (int i = 0; i < (int)(sizeof(OUT_OF_RANGE) / sizeof(OUT_OF_RANGE[0])); i++) {
        if (pkt.src_id == OUT_OF_RANGE[i] && hops_taken == 0) {
            LOG("[range-sim] DROP direct pkt from 0x%02X — simulating out-of-range", pkt.src_id);
            return;
        }
    }
#endif

    pkt_rx_count++;
    LOG("[rx] #%d src=0x%02X dst=0x%02X ttl=%d seq=%d hops=%d len=%d RSSI=%ddBm SNR=%ddB",
        pkt_rx_count, pkt.src_id, pkt.dst_id, pkt.ttl, pkt.seq_num, hops_taken, pkt.payload_len,
        pkt_status.rssi_pkt_in_dbm, pkt_status.snr_pkt_in_db);

    /* Discard if not destined for the gateway */
    if (pkt.dst_id != MESH_ADDR_GATEWAY && pkt.dst_id != MESH_ADDR_BROADCAST) {
        LOG("[rx] mesh dst=0x%02X not for gateway, skipping", pkt.dst_id);
        return;
    }
    if (pkt.payload_len == 0) {
        LOG("[rx] empty MQTT-SN payload, dropping");
        return;
    }

    /* ── Gateway dedup: drop if already forwarded this (src, seq) ───── */
    if (gw_dedup_seen(pkt.src_id, pkt.seq_num)) {
        LOG("[rx] dedup hit src=0x%02X seq=%d, dropping duplicate uplink", pkt.src_id, pkt.seq_num);
        return;
    }
    gw_dedup_add(pkt.src_id, pkt.seq_num);

    /* Debug: raw MQTT-SN bytes */
    // printf("  MQTT-SN (%dB): ", pkt.payload_len);
    // for (int i = 0; i < pkt.payload_len; i++)
    //     printf("%02X ", pkt.payload[i]);
    // printf("\n");
    // fflush(stdout);

    // After gw_dedup_add():
    const char *msg_type = "UNKNOWN";
    if (pkt.payload_len >= 2) {
        switch (pkt.payload[1]) {
        case 0x04:
            msg_type = "CONNECT";
            break;
        case 0x0A:
            msg_type = "REGISTER";
            break;
        case 0x0C:
            msg_type = "PUBLISH";
            break;
        case 0x12:
            msg_type = "SUBSCRIBE";
            break;
        case 0x16:
            msg_type = "PINGREQ";
            break;
        }
    }
    LOG("[rx] MQTT-SN type=%s src=0x%02X", msg_type, pkt.src_id);

    bool is_connect = (pkt.payload_len >= 2 && pkt.payload[1] == 0x04);

    if (is_connect) {
        gw_dedup_clear_src(pkt.src_id); // Allow fresh connection attempt
        LOG("[rx] CONNECT from src=0x%02X — cleared dedup cache", pkt.src_id);
    }

    /* ── Forward to Paho, keyed on SRC_ID ───────────────────────────── */
    pthread_mutex_lock(&clients_lock);
    client_t *client = get_or_create_client(pkt.src_id, is_connect);
    if (!client) {
        pthread_mutex_unlock(&clients_lock);
        return;
    }

    client->last_seq = rh_seq;
    client->pkt_count++;
    client->hop_total += hops_taken;

    ssize_t sent = sendto(client->udp_sock, pkt.payload, pkt.payload_len, 0,
                          (struct sockaddr *)&client->paho_addr, sizeof(client->paho_addr));
    pthread_mutex_unlock(&clients_lock);

    if (sent < 0)
        LOG("[rx] sendto Paho failed: %s", strerror(errno));
    else
        LOG("[rx] -> Paho:%d port=%d (%zdB) [src=0x%02X hops=%d]", PAHO_GW_PORT, client->local_port,
            sent, pkt.src_id, hops_taken);
}

/* ── IRQ dispatcher ──────────────────────────────────────────────────────── */
static void irq_process(void) {
    irq_flag = false;
    lr11xx_system_irq_mask_t irq_regs = 0;
    lr11xx_system_get_and_clear_irq_status(&lr1121, &irq_regs);
    irq_regs &= IRQ_MASK;

    if (irq_regs & LR11XX_SYSTEM_IRQ_RX_DONE) {
        if (irq_regs & LR11XX_SYSTEM_IRQ_CRC_ERROR)
            on_rx_crc_error();
        else
            on_rx_done();
    }
    if (irq_regs & LR11XX_SYSTEM_IRQ_TIMEOUT)
        on_rx_timeout();
    if (irq_regs & LR11XX_SYSTEM_IRQ_HEADER_ERROR)
        LOG("[rx] Header error");
    if (irq_regs & LR11XX_SYSTEM_IRQ_TX_DONE)
        LOG("[tx] TX_DONE IRQ");

    ASSERT_LR11XX_RC(lr11xx_radio_set_rx(&lr1121, RX_CONTINUOUS));
}

/* ── Downstream thread (Paho → LoRa) ─────────────────────────────────────── */
static uint64_t last_dn_tx_us = 0;

static void *downstream_thread(void *arg) {
    (void)arg;
    LOG("[dn] Thread started");

    uint8_t rxbuf[GW_PAYLOAD_MAX];
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    last_dn_tx_us = (uint64_t)ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;

    /* Staging buffers — filled under clients_lock, sent after releasing it */
    uint8_t d_data[MAX_CLIENTS][GW_PAYLOAD_MAX];
    uint8_t d_len[MAX_CLIENTS];
    uint8_t d_node[MAX_CLIENTS];
    uint8_t d_seq[MAX_CLIENTS];

    while (running) {
        fd_set rfds;
        int maxfd = -1;
        FD_ZERO(&rfds);

        pthread_mutex_lock(&clients_lock);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active && clients[i].udp_sock >= 0) {
                FD_SET(clients[i].udp_sock, &rfds);
                if (clients[i].udp_sock > maxfd)
                    maxfd = clients[i].udp_sock;
            }
        }
        pthread_mutex_unlock(&clients_lock);

        if (maxfd < 0) {
            usleep(50000);
            continue;
        }

        struct timeval tv = {0, 50000};
        if (select(maxfd + 1, &rfds, NULL, NULL, &tv) <= 0)
            continue;

        int count = 0;
        pthread_mutex_lock(&clients_lock);
        for (int i = 0; i < MAX_CLIENTS && count < MAX_CLIENTS; i++) {
            if (!clients[i].active)
                continue;
            if (!FD_ISSET(clients[i].udp_sock, &rfds))
                continue;

            struct sockaddr_in src;
            socklen_t sl = sizeof(src);
            ssize_t n = recvfrom(clients[i].udp_sock, rxbuf, sizeof(rxbuf), 0,
                                 (struct sockaddr *)&src, &sl);
            if (n <= 0)
                continue;

            memcpy(d_data[count], rxbuf, n);
            d_len[count] = (uint8_t)n;
            d_node[count] = clients[i].node_id;
            d_seq[count] = clients[i].dn_seq++;
            count++;
            LOG("[dn] %zdB queued for node=0x%02X (will be mesh-wrapped)", n, clients[i].node_id);
        }
        pthread_mutex_unlock(&clients_lock);

        /* Transmit each staged downlink packet with mesh wrapper */
        for (int p = 0; p < count; p++) {
            clock_gettime(CLOCK_MONOTONIC, &ts);
            uint64_t now = (uint64_t)ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
            uint64_t elapsed = now - last_dn_tx_us;

            if (elapsed < DN_SPACING_US) {
                uint64_t wait = DN_SPACING_US - elapsed;
                LOG("[dn] Spacing wait %llums", (unsigned long long)(wait / 1000));
                usleep((useconds_t)wait);
            }

            pthread_mutex_lock(&radio_lock);
            irq_flag = false;
            lora_send(d_node[p], d_seq[p], d_data[p], d_len[p]); // ← use d_seq[p]
            irq_flag = false;
            pthread_mutex_unlock(&radio_lock);

            clock_gettime(CLOCK_MONOTONIC, &ts);
            last_dn_tx_us = (uint64_t)ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
        }
    }

    LOG("[dn] Thread exiting");
    return NULL;
}

/* ── Radio init ──────────────────────────────────────────────────────────── */
static void radio_init(void) {
    lora_init_io_context(&lr1121);
    lora_init_io(&lr1121);
    lora_spi_init(&lr1121);
    lora_system_init(&lr1121);
    lora_print_version(&lr1121);
    lora_radio_init(&lr1121);

    lr11xx_radio_mod_params_lora_t mod = {
        .sf = LR11XX_RADIO_LORA_SF7,
        .bw = LR11XX_RADIO_LORA_BW_125,
        .cr = LR11XX_RADIO_LORA_CR_4_5,
        .ldro = 0,
    };
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_mod_params(&lr1121, &mod));
    ASSERT_LR11XX_RC(lr11xx_radio_set_rf_freq(&lr1121, 915000000UL));
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_public_network(&lr1121, false));

    lr11xx_radio_pkt_params_lora_t pkt = {
        .preamble_len_in_symb = 8,
        .header_type = LR11XX_RADIO_LORA_PKT_EXPLICIT,
        .pld_len_in_bytes = GW_PAYLOAD_MAX,
        .crc = LR11XX_RADIO_LORA_CRC_ON,
        .iq = LR11XX_RADIO_LORA_IQ_STANDARD,
    };
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_pkt_params(&lr1121, &pkt));

    lora_init_irq(&lr1121, &isr);
    ASSERT_LR11XX_RC(lr11xx_system_set_dio_irq_params(&lr1121, IRQ_MASK, 0));
    ASSERT_LR11XX_RC(lr11xx_system_clear_irq_status(&lr1121, LR11XX_SYSTEM_IRQ_ALL_MASK));

    LOG("[init] SF7 BW125 CR4/5 | 915 MHz | SW=private | CRC ON | MaxPld=%d", GW_PAYLOAD_MAX);
}

/* ── main ────────────────────────────────────────────────────────────────── */
int main(void) {
    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    printf("  MQTT-SN LoRa <-> UDP Bridge  [mesh-aware v2]\n");
    printf("  Paho GW       : %s:%d\n", PAHO_GW_IP, PAHO_GW_PORT);
    printf("  Client ports  : %d-%d\n", CLIENT_PORT_BASE, CLIENT_PORT_BASE + MAX_CLIENTS - 1);
    printf("  Frame layout  : RH(%dB) + Mesh(%dB) + MQTT-SN(NB)\n", RH_HDR, MESH_HEADER_SIZE);
    printf("  Downlink      : mesh-wrapped (src=GW, ttl=%d)\n", MESH_TTL_DEFAULT);

    memset(clients, 0, sizeof(clients));
    memset(gw_dedup, 0, sizeof(gw_dedup));
    radio_init();

    pthread_t ds_thread;
    if (pthread_create(&ds_thread, NULL, downstream_thread, NULL) != 0) {
        LOG("[main] pthread_create failed: %s", strerror(errno));
        return 1;
    }

    ASSERT_LR11XX_RC(lr11xx_radio_set_rx(&lr1121, RX_CONTINUOUS));
    LOG("[main] Continuous RX started\n");

    int hb = 0;
    while (running) {
        if (irq_flag) {
            pthread_mutex_lock(&radio_lock);
            irq_process();
            pthread_mutex_unlock(&radio_lock);
        } else {
            usleep(10000);
            if (++hb >= 1000) {
                hb = 0;
                LOG("[hb] rx=%d tx=%d", pkt_rx_count, pkt_tx_count);
                pthread_mutex_lock(&clients_lock);
                expire_clients();
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (!clients[i].active)
                        continue;
                    printf("  src=0x%02X port=%d pkts=%u avg_hops=%.2f age=%lds\n",
                           clients[i].node_id, clients[i].local_port, clients[i].pkt_count,
                           clients[i].pkt_count > 0
                               ? (double)clients[i].hop_total / clients[i].pkt_count
                               : 0.0,
                           (long)(time(NULL) - clients[i].last_seen));
                }
                pthread_mutex_unlock(&clients_lock);
                fflush(stdout);
            }
        }
    }

    pthread_join(ds_thread, NULL);
    lr11xx_system_set_standby(&lr1121, LR11XX_SYSTEM_STANDBY_CFG_RC);

    pthread_mutex_lock(&clients_lock);
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].active && clients[i].udp_sock >= 0)
            close(clients[i].udp_sock);
    pthread_mutex_unlock(&clients_lock);

    printf("[main] Done. RX=%d TX=%d\n", pkt_rx_count, pkt_tx_count);
    return 0;
}