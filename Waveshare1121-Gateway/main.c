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
 * Each unique LoRa FROM-ID gets its own UDP socket bound to a fixed
 * local port (CLIENT_PORT_BASE + slot) so the Paho gateway tracks
 * each node as a distinct client by stable source address:port.
 *
 * Threading model:
 *   Main thread      — polls irq_flag and calls irq_process() which
 *                      reads/dispatches radio IRQs and forwards uplink
 *                      MQTT-SN payloads to Paho over UDP.
 *   Downstream thread — blocks on select() across all per-client UDP
 *                      sockets, reads downlink packets from Paho, and
 *                      forwards them to the destination node over LoRa.
 *
 * Mutexes:
 *   clients_lock — protects the client table (get_or_create, expiry,
 *                  heartbeat printing, socket close on exit).
 *   radio_lock   — serialises all SPI/radio access between the main
 *                  thread (irq_process) and the downstream thread
 *                  (lora_send). Both threads must hold this lock for
 *                  the full duration of any radio operation.
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

// Config
#define PAHO_GW_IP "127.0.0.1"
#define PAHO_GW_PORT 10000

/* This bridge's LoRa node ID. Nodes must address packets to this ID. Do not change unless you also
 * update all node sketches. */
#define LORA_GW_NODE_ID 0x00
#define LORA_BROADCAST 0xFF

#define MAX_CLIENTS 30
#define CLIENT_PORT_BASE 20000 /* per-client UDP ports: 20000 – 20029 */

/* Inactive clients are kicked after this many seconds with no uplink. Frees the slot and UDP socket
 * for reuse. */
#define CLIENT_TIMEOUT_S 300

#define GW_PAYLOAD_MAX 255
#define RX_CONTINUOUS 0xFFFFFF

/* Minimum gap between consecutive downlink LoRa transmissions (µs).
   Prevents back-to-back packets from colliding at the node before it
   has finished processing and re-arming its receiver. */
#define DN_SPACING_US 1500000ULL

/* RadioHead header layout*/

#define RH_TO 0
#define RH_FROM 1
#define RH_ID 2
#define RH_FLAGS 3
#define RH_HDR 4

/* IRQ dispatch*/

#define IRQ_MASK                                                                                   \
    (LR11XX_SYSTEM_IRQ_TX_DONE | LR11XX_SYSTEM_IRQ_RX_DONE | LR11XX_SYSTEM_IRQ_TIMEOUT |           \
     LR11XX_SYSTEM_IRQ_CRC_ERROR | LR11XX_SYSTEM_IRQ_HEADER_ERROR)

/* Per-client state */

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

/* Globals*/

lr1121_t lr1121;

/* Set to true by the GPIO ISR when the radio asserts its IRQ line.
   Cleared by irq_process() after reading and dispatching IRQ flags. */
static volatile bool irq_flag = false;

static volatile int running = 1;
static volatile int pkt_rx_count = 0;
static volatile int pkt_tx_count = 0;
static volatile uint8_t dn_seq = 0;

/* Logging */

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

/* Serialises all SPI/radio access between the main thread and the
   downstream thread. Must be held for the full duration of any radio
   operation (irq_process, lora_send). */
static pthread_mutex_t radio_lock = PTHREAD_MUTEX_INITIALIZER;

static void on_rx_timeout(void) { LOG("[rx] Timeout"); }
static void on_rx_crc_error(void) { LOG("[rx] CRC error"); }

/* IRQ / signal */

/* GPIO ISR called from wiringPi's ISR thread on the radio's DIO1 edge.
   Only sets the flag; all radio SPI access happens in the main thread. */
void isr(void) { irq_flag = true; }

static void on_signal(int sig) {
    (void)sig;
    running = 0;
}

/* Client registry*/

/* Must be called with clients_lock held.
   Returns the existing client for node_id, or allocates a new slot and
   creates a bound UDP socket for it. Returns NULL on failure. */
static client_t *get_or_create_client(uint8_t node_id) {
    /* Look up existing client */
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].node_id == node_id) {
            clients[i].last_seen = time(NULL);
            return &clients[i];
        }
    }

    /* If new node, find a free slot */
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

        /* Allow immediate port reuse after the bridge restarts */
        int reuse = 1;
        setsockopt(clients[i].udp_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

        /* Bind to the fixed per-client local port so Paho sees a stable
           source address:port for this node across all its packets. */
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

        /* Cache the Paho gateway address for sendto() in on_rx_done() */
        memset(&clients[i].paho_addr, 0, sizeof(clients[i].paho_addr));
        clients[i].paho_addr.sin_family = AF_INET;
        clients[i].paho_addr.sin_port = htons(PAHO_GW_PORT);
        inet_pton(AF_INET, PAHO_GW_IP, &clients[i].paho_addr.sin_addr);

        LOG("[client] New node=0x%02X -> port=%d", node_id, clients[i].local_port);
        return &clients[i];
    }

    LOG("[client] Table full — cannot register node=0x%02X", node_id);
    return NULL;
}

/* Must be called with clients_lock held.
   Closes and frees any client slot that has not sent an uplink within
   CLIENT_TIMEOUT_S seconds. */
static void expire_clients(void) {
    time_t now = time(NULL);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active)
            continue;
        if ((now - clients[i].last_seen) < CLIENT_TIMEOUT_S)
            continue;
        LOG("[client] Expiring node=0x%02X (idle %lds)", clients[i].node_id,
            (long)(now - clients[i].last_seen));
        close(clients[i].udp_sock);
        memset(&clients[i], 0, sizeof(clients[i]));
    }
}

/* LoRa TX*/

/* Transmit a RadioHead-framed packet to dst.
 *
 * Caller must hold radio_lock for the entire call and must NOT hold
 * clients_lock (to avoid lock-order inversion with irq_process).
 *
 * The radio is put into standby before reconfiguring because the LR1121
 * rejects parameter writes while actively receiving or transmitting.
 * After TX completes (or times out) the radio is returned to continuous RX
 * so the main thread can resume receiving uplinks immediately.
 */
static int lora_send(uint8_t dst, uint8_t seq, const uint8_t *payload, uint8_t len) {
    if (len + RH_HDR > GW_PAYLOAD_MAX) {
        LOG("[tx] Payload too large (%d bytes), dropping", len);
        return -1;
    }

    /* Build RadioHead frame */
    uint8_t frame[GW_PAYLOAD_MAX];
    frame[RH_TO] = dst;
    frame[RH_FROM] = LORA_GW_NODE_ID;
    frame[RH_ID] = seq;
    frame[RH_FLAGS] = 0x00;
    memcpy(frame + RH_HDR, payload, len);
    uint8_t total = (uint8_t)(len + RH_HDR);

    /* Standby is required before writing TX parameters as the radio will
       return BUSY if we attempt to reconfigure while in RX mode. It also
       reduces current draw during the reconfiguration window. */
    lr11xx_system_set_standby(&lr1121, LR11XX_SYSTEM_STANDBY_CFG_RC);

    /* Re-assert modulation and packet parameters so that standby can reset them */
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

    /* Start TX with a 5-second hardware timeout as a safety net */
    ASSERT_LR11XX_RC(lr11xx_radio_set_tx(&lr1121, 5000));

    /* Spin-wait for TX_DONE IRQ (set by the GPIO ISR).
       The 600 ms ceiling matches the 5 s hardware timeout with margin. */
    int waited = 0;
    while (!irq_flag && waited < 600) {
        usleep(2000);
        waited += 2;
    }

    /* Clear all pending IRQ flags. If we hit the spin-wait ceiling without
       TX_DONE this also clears any stale flags so the next RX is clean. */
    lr11xx_system_irq_mask_t irq_regs = 0;
    lr11xx_system_get_and_clear_irq_status(&lr1121, &irq_regs);
    irq_flag = false;

    /* Return to continuous RX so the main thread can receive uplinks again */
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

/* LoRa RX handlers*/

/* Called by irq_process() when RX_DONE is set without a CRC error.
   Reads the received frame from the radio, strips the RadioHead header,
   and forwards the bare MQTT-SN payload to the Paho gateway over UDP. */
static void on_rx_done(void) {
    uint8_t buf[GW_PAYLOAD_MAX + 1];
    lr11xx_radio_rx_buffer_status_t rx_buf;
    lr11xx_radio_pkt_status_lora_t pkt_status;

    lr11xx_radio_get_rx_buffer_status(&lr1121, &rx_buf);
    uint8_t size = rx_buf.pld_len_in_bytes;

    if (size < (uint8_t)(RH_HDR + 1) || size > GW_PAYLOAD_MAX) {
        LOG("[rx] Bad size %d, dropping", size);
        return;
    }

    /* Reading the buffer also clears the radio's internal RX FIFO,
       making it ready to receive the next packet immediately. */
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
        LOG("[rx] Not addressed to us (0x%02X), skipping", rh_to);
        return;
    }

    /*for debugging */
    printf("  MQTT-SN: ");
    for (int i = 0; i < mqttsn_len; i++)
        printf("%02X ", mqttsn[i]);
    printf("\n");
    fflush(stdout);

    /* Look up (or register) the sending node and forward to Paho */
    pthread_mutex_lock(&clients_lock);
    client_t *client = get_or_create_client(rh_from);
    if (!client) {
        pthread_mutex_unlock(&clients_lock);
        return;
    }
    client->last_seq = rh_seq;

    ssize_t sent = sendto(client->udp_sock, mqttsn, mqttsn_len, 0,
                          (struct sockaddr *)&client->paho_addr, sizeof(client->paho_addr));
    pthread_mutex_unlock(&clients_lock);

    if (sent < 0)
        LOG("[rx] sendto Paho failed: %s", strerror(errno));
    else
        LOG("[rx] -> Paho:%d via port=%d (%zd bytes)", PAHO_GW_PORT, client->local_port, sent);
}

/* Called from the main loop whenever irq_flag is set.
 *
 * Reads and clears all pending IRQ flags in one SPI transaction, then
 * dispatches to the appropriate handler. After dispatching, the radio is
 * (re-)placed into continuous RX regardless of which IRQ fired
 *
 * Caller must hold radio_lock.
 */
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

    /* Ensure the radio is in continuous RX after handling any IRQ type */
    ASSERT_LR11XX_RC(lr11xx_radio_set_rx(&lr1121, RX_CONTINUOUS));
}

/* Downstream thread (Paho -> LoRa)*/

static uint64_t last_dn_tx_us = 0;

static void *downstream_thread(void *arg) {
    (void)arg;
    LOG("[dn] Thread started");

    uint8_t rxbuf[GW_PAYLOAD_MAX];

    /* Seed the spacing timer to now so the very first downlink is not
       held up by an uninitialised last_dn_tx_us of zero. */
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    last_dn_tx_us = (uint64_t)ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;

    /* Staging arrays, data is copied here under clients_lock then
       transmitted after the lock is released, keeping the lock window
       short and avoiding contention with the main thread's on_rx_done(). */
    uint8_t d_data[MAX_CLIENTS][GW_PAYLOAD_MAX];
    uint8_t d_len[MAX_CLIENTS];
    uint8_t d_node[MAX_CLIENTS];

    while (running) {
        fd_set rfds;
        int maxfd = -1;
        FD_ZERO(&rfds);

        /* Build the fd_set from active client sockets under the lock */
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
            /* No clients yet, sleep briefly and retry */
            usleep(50000);
            continue;
        }

        /* Wait up to 50 ms for any client socket to become readable */
        struct timeval tv = {0, 50000};
        if (select(maxfd + 1, &rfds, NULL, NULL, &tv) <= 0)
            continue;

        /* Read all ready sockets into the staging arrays */
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
            count++;
            LOG("[dn] %zd bytes queued for node=0x%02X", n, clients[i].node_id);
        }
        pthread_mutex_unlock(&clients_lock);

        /* Transmit each staged packet over LoRa */
        for (int p = 0; p < count; p++) {
            clock_gettime(CLOCK_MONOTONIC, &ts);
            uint64_t now = (uint64_t)ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
            uint8_t seq = dn_seq++;
            uint64_t elapsed = now - last_dn_tx_us;

            /* Enforce minimum inter-packet spacing to give the node time
               to finish processing the previous downlink and re-arm its
               receiver before the next transmission arrives. */
            if (elapsed < DN_SPACING_US) {
                uint64_t wait = DN_SPACING_US - elapsed;
                LOG("[dn] Spacing wait %llums", (unsigned long long)(wait / 1000));
                usleep((useconds_t)wait);
            }

            pthread_mutex_lock(&radio_lock);
            irq_flag = false;
            lora_send(d_node[p], seq, d_data[p], d_len[p]);
            irq_flag = false;
            pthread_mutex_unlock(&radio_lock);

            /* Record send time for spacing calculation on the next packet */
            clock_gettime(CLOCK_MONOTONIC, &ts);
            last_dn_tx_us = (uint64_t)ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
        }
    }

    LOG("[dn] Thread exiting");
    return NULL;
}

/* Radio initialisation*/

static void radio_init(void) {
    lora_init_io_context(&lr1121);
    lora_init_io(&lr1121);
    lora_spi_init(&lr1121);
    lora_system_init(&lr1121);
    lora_print_version(&lr1121);
    lora_radio_init(&lr1121); /* default 868 MHz we override below */

    /* Override modulation parameters for 915 MHz US band */
    lr11xx_radio_mod_params_lora_t mod = {
        .sf = LR11XX_RADIO_LORA_SF7,
        .bw = LR11XX_RADIO_LORA_BW_125,
        .cr = LR11XX_RADIO_LORA_CR_4_5,
        .ldro = 0,
    };
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_mod_params(&lr1121, &mod));
    ASSERT_LR11XX_RC(lr11xx_radio_set_rf_freq(&lr1121, 915000000UL));
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_public_network(&lr1121, false));

    /* Set maximum receive payload size so the radio does not discard
       packets larger than its default buffer configuration. */
    lr11xx_radio_pkt_params_lora_t pkt = {
        .preamble_len_in_symb = 8,
        .header_type = LR11XX_RADIO_LORA_PKT_EXPLICIT,
        .pld_len_in_bytes = GW_PAYLOAD_MAX,
        .crc = LR11XX_RADIO_LORA_CRC_ON,
        .iq = LR11XX_RADIO_LORA_IQ_STANDARD,
    };
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_pkt_params(&lr1121, &pkt));

    /* Register GPIO ISR and configure radio to assert DIO1 for all
       relevant IRQ sources. Clear any stale flags before entering RX. */
    lora_init_irq(&lr1121, &isr);
    ASSERT_LR11XX_RC(lr11xx_system_set_dio_irq_params(&lr1121, IRQ_MASK, 0));
    ASSERT_LR11XX_RC(lr11xx_system_clear_irq_status(&lr1121, LR11XX_SYSTEM_IRQ_ALL_MASK));

    LOG("[init] SF7 BW125 CR4/5 | 915 MHz | SW=private | CRC ON | MaxPld=%d", GW_PAYLOAD_MAX);
}

/* main*/

int main(void) {
    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    printf("  MQTT-SN LoRa <-> UDP Bridge\n");
    printf("  Paho GW  : %s:%d\n", PAHO_GW_IP, PAHO_GW_PORT);
    printf("  Ports    : %d-%d\n", CLIENT_PORT_BASE, CLIENT_PORT_BASE + MAX_CLIENTS - 1);

    memset(clients, 0, sizeof(clients));
    radio_init();

    /* Start downstream thread before enabling RX so any immediate Paho
       traffic after connect is handled without dropping packets. */
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

            /* Heartbeat every ~10 s: print stats and expire idle clients */
            if (++hb >= 1000) {
                hb = 0;
                LOG("[hb] rx=%d tx=%d", pkt_rx_count, pkt_tx_count);

                pthread_mutex_lock(&clients_lock);
                expire_clients();
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

    /* Graceful shutdown */
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