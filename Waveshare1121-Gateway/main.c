/*
 * main.c — Bidirectional LoRa <-> UDP Bridge for MQTT-SN
 *
 * Hardware : Raspberry Pi 4 + Waveshare LR1121
 * Upstream : Paho MQTT-SN Gateway localhost:10000 (UDP)
 * Nodes    : Arduino Maker + Cytron RFM95
 *
 * Flow (uplink):   Node -LoRa-> [this bridge] -UDP-> Paho GW
 * Flow (downlink): Paho GW -UDP-> [this bridge] -LoRa-> Node
 *
 * Each unique LoRa FROM-ID gets its own UDP socket on a fixed
 * local port (CLIENT_PORT_BASE + slot) so Paho tracks clients
 * by stable source address:port.
 *
 * Build:
 *   gcc -o lora_bridge main.c lr1121_config.c wavesahre_lora_1121.c \
 *       -lwiringPi -lpthread
 * Run: sudo ./lora_bridge
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
#include "wavesahre_lora_1121.h"

// Configuration
#define PAHO_GW_IP "127.0.0.1"
#define PAHO_GW_PORT 10000

#define LORA_GW_NODE_ID 0x00 // This bridge's LoRa node ID - do not change
#define LORA_BROADCAST 0xFF

#define MAX_CLIENTS 30
#define CLIENT_PORT_BASE 20000 // Per-client ports: 20000-20029

#define GW_PAYLOAD_MAX 255
#define RX_CONTINUOUS 0xFFFFFF

// RadioHead header
#define RH_TO 0
#define RH_FROM 1
#define RH_ID 2
#define RH_FLAGS 3
#define RH_HDR 4 // header length

// Per-client state
typedef struct {
    uint8_t node_id;
    int udp_sock;
    uint16_t local_port;
    struct sockaddr_in paho_addr;
    uint8_t last_seq;
    time_t last_seen;
    bool active;
} client_t;

static client_t clients[MAX_CLIENTS];
static pthread_mutex_t clients_lock = PTHREAD_MUTEX_INITIALIZER;

// Globals
lr1121_t lr1121;
static volatile bool irq_flag = false;
static volatile int running = 1;
static int pkt_rx_count = 0;
static int pkt_tx_count = 0;
static uint8_t dn_seq = 0; /* downlink rolling sequence     */

// Logging
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

// IRQ / signal
void isr(void) { irq_flag = true; }
static void on_signal(int sig) {
    (void)sig;
    running = 0;
}

// Client registry must be called with clients_lock held
static client_t *get_or_create_client(uint8_t node_id) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].node_id == node_id) {
            clients[i].last_seen = time(NULL);
            return &clients[i];
        }
    }
    // New client - find empty slot and create UDP socket
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active)
            continue;

        clients[i].node_id = node_id;
        clients[i].active = true;
        clients[i].last_seen = time(NULL);
        clients[i].last_seq = 0;
        clients[i].local_port = (uint16_t)(CLIENT_PORT_BASE + i);

        clients[i].udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (clients[i].udp_sock < 0) {
            LOG("[client] socket() failed: %s", strerror(errno));
            clients[i].active = false;
            return NULL;
        }

        // Allow immediate reuse of this port after the program exits
        int reuse = 1;
        setsockopt(clients[i].udp_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

        // Bind to local port for this client
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

        // Set up Paho GW address for this client
        memset(&clients[i].paho_addr, 0, sizeof(clients[i].paho_addr));
        clients[i].paho_addr.sin_family = AF_INET;
        clients[i].paho_addr.sin_port = htons(PAHO_GW_PORT);
        inet_pton(AF_INET, PAHO_GW_IP, &clients[i].paho_addr.sin_addr);

        LOG("[client] New node=0x%02X -> port=%d", node_id, clients[i].local_port);
        return &clients[i];
    }
    LOG("[client] Table full!");
    return NULL;
}

// LoRa TX Lock must NOT be held when calling this
static int lora_send(uint8_t dst, uint8_t seq, const uint8_t *payload, uint8_t len) {
    if (len + RH_HDR > GW_PAYLOAD_MAX) {
        LOG("[tx] Too large (%d)", len);
        return -1;
    }

    uint8_t frame[GW_PAYLOAD_MAX];
    frame[RH_TO] = dst;
    frame[RH_FROM] = LORA_GW_NODE_ID;
    frame[RH_ID] = seq;
    frame[RH_FLAGS] = 0x00;
    memcpy(frame + RH_HDR, payload, len);
    uint8_t total = (uint8_t)(len + RH_HDR);

    /* Put radio to standby before reconfiguring - this is required to reset the radio state after
     TX and avoiding "radio busy" errors when trying to set TX params. Standby also reduces
     current draw while we wait for TX to complete and reconfigure for RX. */
    lr11xx_system_set_standby(&lr1121, LR11XX_SYSTEM_STANDBY_CFG_RC);

    // Re-assert all params - standby may reset radio state
    lr11xx_radio_mod_params_lora_t mod = {
        .sf = LR11XX_RADIO_LORA_SF7,
        .bw = LR11XX_RADIO_LORA_BW_125,
        .cr = LR11XX_RADIO_LORA_CR_4_5,
        .ldro = 0,
    };
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_mod_params(&lr1121, &mod));
    ASSERT_LR11XX_RC(lr11xx_radio_set_rf_freq(&lr1121, 915000000UL));
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_public_network(&lr1121, false));

    // Set TX params and send - this will block until TX done or timeout
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

    // Clear IRQ flags - if we timed out waiting for TX_DONE, this will clear any stale flags so
    // we're ready to receive new packets
    lr11xx_system_irq_mask_t irq_regs = 0;
    lr11xx_system_get_and_clear_irq_status(&lr1121, &irq_regs);
    irq_flag = false;

    // Re-assert RX params after TX
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
    LOG("[tx] %d bytes -> 0x%02X seq=%d", total, dst, seq);
    return 0;
}

// LoRa RX done
static void on_rx_done(void) {
    uint8_t buf[GW_PAYLOAD_MAX + 1];
    lr11xx_radio_rx_buffer_status_t rx_buf;
    lr11xx_radio_pkt_status_lora_t pkt_status;

    lr11xx_radio_get_rx_buffer_status(&lr1121, &rx_buf);
    uint8_t size = rx_buf.pld_len_in_bytes;

    if (size < (uint8_t)(RH_HDR + 1) || size > GW_PAYLOAD_MAX) {
        LOG("[rx] Bad size %d, drop", size);
        return;
    }

    // Read payload + status from radio - this also clears the radio's internal RX buffer so we can
    // receive the next packet
    lr11xx_regmem_read_buffer8(&lr1121, buf, rx_buf.buffer_start_pointer, size);
    lr11xx_radio_get_lora_pkt_status(&lr1121, &pkt_status);

    uint8_t rh_to = buf[RH_TO];
    uint8_t rh_from = buf[RH_FROM];
    uint8_t rh_seq = buf[RH_ID];
    uint8_t *mqttsn = buf + RH_HDR;
    uint8_t mqttsn_len = (uint8_t)(size - RH_HDR);

    pkt_rx_count++;
    LOG("[rx] #%d from=0x%02X to=0x%02X seq=%d len=%d RSSI=%ddBm SNR=%ddB", pkt_rx_count, rh_from,
        rh_to, rh_seq, mqttsn_len, pkt_status.rssi_pkt_in_dbm, pkt_status.snr_pkt_in_db);

    if (rh_to != LORA_GW_NODE_ID && rh_to != LORA_BROADCAST) {
        LOG("[rx] Not for us (0x%02X), skip", rh_to);
        return;
    }

    // Hex dump for debugging
    printf("  MQTT-SN: ");
    for (int i = 0; i < mqttsn_len; i++)
        printf("%02X ", mqttsn[i]);
    printf("\n");
    fflush(stdout);

    // Lookup client by FROM address - if new, create new client and UDP socket
    pthread_mutex_lock(&clients_lock);
    client_t *client = get_or_create_client(rh_from);
    if (!client) {
        pthread_mutex_unlock(&clients_lock);
        return;
    }
    client->last_seq = rh_seq;

    // Send MQTT-SN payload to Paho GW over UDP
    ssize_t sent = sendto(client->udp_sock, mqttsn, mqttsn_len, 0,
                          (struct sockaddr *)&client->paho_addr, sizeof(client->paho_addr));
    pthread_mutex_unlock(&clients_lock);

    if (sent < 0)
        LOG("[rx] sendto failed: %s", strerror(errno));
    else
        LOG("[rx] -> Paho:%d via port=%d (%zd bytes)", PAHO_GW_PORT, client->local_port, sent);
}

static void on_rx_timeout(void) { LOG("[rx] Timeout"); }
static void on_rx_crc_error(void) { LOG("[rx] CRC error"); }

// IRQ mask + dispatch
#define IRQ_MASK                                                                                   \
    (LR11XX_SYSTEM_IRQ_TX_DONE | LR11XX_SYSTEM_IRQ_RX_DONE | LR11XX_SYSTEM_IRQ_TIMEOUT |           \
     LR11XX_SYSTEM_IRQ_CRC_ERROR | LR11XX_SYSTEM_IRQ_HEADER_ERROR)

// This is called from the main loop when an IRQ is detected
static void irq_process(void) {
    irq_flag = false;
    lr11xx_system_irq_mask_t irq_regs = 0;
    // Read and clear all pending IRQ flags
    lr11xx_system_get_and_clear_irq_status(&lr1121, &irq_regs);
    irq_regs &= IRQ_MASK;

    // Dispatch handlers for each IRQ type
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

    /* Re-enter RX mode to receive next packet - we do this after processing the IRQ so we're not
     trying to receive new packets while we're still processing the previous one, which could
    cause issues with timing and buffer management.*/
    ASSERT_LR11XX_RC(lr11xx_radio_set_rx(&lr1121, RX_CONTINUOUS));
}

// Downstream thread: Paho -> LoRa
static void *downstream_thread(void *arg) {
    (void)arg;
    LOG("[dn] Thread started");
    uint8_t rxbuf[GW_PAYLOAD_MAX];

    // Staging arrays so we release the lock before transmitting
    uint8_t d_data[MAX_CLIENTS][GW_PAYLOAD_MAX];
    uint8_t d_len[MAX_CLIENTS];
    uint8_t d_node[MAX_CLIENTS];

    while (running) {
        fd_set rfds;
        int maxfd = -1;
        FD_ZERO(&rfds);

        /* Build fd_set for select() from all active client UDP sockets - we must hold the lock
         while doing*/
        pthread_mutex_lock(&clients_lock);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active && clients[i].udp_sock >= 0) {
                FD_SET(clients[i].udp_sock, &rfds);
                if (clients[i].udp_sock > maxfd)
                    maxfd = clients[i].udp_sock;
            }
        }
        /* We release the lock before calling select() and processing packets so we're not holding
        the lock while doing potentially slow operations like waiting for packets and
        transmitting over LoRa, which could cause contention with the RX IRQ handler trying to
        look up clients and send packets to Paho. By copying active client info into staging
        arrays and releasing the lock before processing, we minimize the time we hold the lock
        and reduce contention between threads.*/
        pthread_mutex_unlock(&clients_lock);

        if (maxfd < 0) {
            usleep(50000);
            continue;
        }

        struct timeval tv = {0, 50000};
        if (select(maxfd + 1, &rfds, NULL, NULL, &tv) <= 0)
            continue;

        // Check which clients have data and read it - we must hold the lock while accessing client
        // info
        int count = 0;
        pthread_mutex_lock(&clients_lock);
        for (int i = 0; i < MAX_CLIENTS && count < MAX_CLIENTS; i++) {
            if (!clients[i].active)
                continue;
            if (!FD_ISSET(clients[i].udp_sock, &rfds))
                continue;

            // We have data for this client - read it and copy to staging arrays for processing
            // outside the lock
            struct sockaddr_in src;
            socklen_t sl = sizeof(src);
            ssize_t n = recvfrom(clients[i].udp_sock, rxbuf, sizeof(rxbuf), 0,
                                 (struct sockaddr *)&src, &sl);
            if (n <= 0)
                continue;

            memcpy(d_data[count], rxbuf, n);
            d_len[count] = (uint8_t)n;
            d_node[count] = clients[i].node_id;
            count++;
            LOG("[dn] %zd bytes queued for node=0x%02X", n, clients[i].node_id);
        }
        pthread_mutex_unlock(&clients_lock);

        for (int p = 0; p < count; p++) {
            printf("  Downlink: ");
            for (int j = 0; j < d_len[p]; j++)
                printf("%02X ", d_data[p][j]);
            printf("\n");
            fflush(stdout);
            lora_send(d_node[p], dn_seq++, d_data[p], d_len[p]);
        }
    }
    LOG("[dn] Thread exiting");
    return NULL;
}

// Radio init
static void radio_init(void) {
    lora_init_io_context(&lr1121);
    lora_init_io(&lr1121);
    lora_spi_init(&lr1121);
    lora_system_init(&lr1121);
    lora_print_version(&lr1121);
    lora_radio_init(&lr1121); /* sets 868MHz — we override below */

    // Re-assert all params to be sure after init - standby may reset radio state
    lr11xx_radio_mod_params_lora_t mod = {
        .sf = LR11XX_RADIO_LORA_SF7,
        .bw = LR11XX_RADIO_LORA_BW_125,
        .cr = LR11XX_RADIO_LORA_CR_4_5,
        .ldro = 0,
    };
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_mod_params(&lr1121, &mod));
    ASSERT_LR11XX_RC(lr11xx_radio_set_rf_freq(&lr1121, 915000000UL));
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_public_network(&lr1121, false));

    // Set max payload size - this is required to receive packets up to this size, otherwise the
    // radio will discard them
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

    LOG("[init] SF7 BW125 CR4/5 | 915MHz | SW=private | CRC ON | MaxPld=%d", GW_PAYLOAD_MAX);
}

// main
int main(void) {
    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    printf("  MQTT-SN LoRa <-> UDP Bridge\n");
    printf("  Paho GW  : %s:%d\n", PAHO_GW_IP, PAHO_GW_PORT);
    printf("  Ports    : %d-%d\n", CLIENT_PORT_BASE, CLIENT_PORT_BASE + MAX_CLIENTS - 1);

    // Initialize client table and radio
    memset(clients, 0, sizeof(clients));
    radio_init();

    // Start downstream thread for Paho -> LoRa - we do this before starting RX so we're ready to
    // process any packets
    pthread_t ds_thread;
    if (pthread_create(&ds_thread, NULL, downstream_thread, NULL) != 0) {
        LOG("[main] pthread_create: %s", strerror(errno));
        return 1;
    }

    // Start RX
    ASSERT_LR11XX_RC(lr11xx_radio_set_rx(&lr1121, RX_CONTINUOUS));
    LOG("[main] Continuous RX started\n");

    int hb = 0;
    while (running) {
        // Check for IRQs and process
        if (irq_flag) {
            irq_process();
        } else {
            usleep(10000);
            if (++hb >= 1000) {
                hb = 0;
                LOG("[hb] rx=%d tx=%d", pkt_rx_count, pkt_tx_count);
                pthread_mutex_lock(&clients_lock);
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i].active)
                        printf("  node=0x%02X port=%d age=%lds\n", clients[i].node_id,
                               clients[i].local_port, (long)(time(NULL) - clients[i].last_seen));
                }
                pthread_mutex_unlock(&clients_lock);
                fflush(stdout);
            }
        }
    }

    // Cleanup
    pthread_join(ds_thread, NULL);
    lr11xx_system_set_standby(&lr1121, LR11XX_SYSTEM_STANDBY_CFG_RC);

    // Close all client sockets - we must hold the lock while accessing the client table
    pthread_mutex_lock(&clients_lock);
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].active && clients[i].udp_sock >= 0)
            close(clients[i].udp_sock);
    pthread_mutex_unlock(&clients_lock);

    printf("[main] Done. RX=%d TX=%d\n", pkt_rx_count, pkt_tx_count);
    return 0;
}