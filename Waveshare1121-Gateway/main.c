/*
 * main.c — LoRa Gateway Receiver
 * Hardware : Raspberry Pi 4 + Waveshare Core1121-HF (LR1121)
 * Receives : Arduino Maker + Cytron LoRa RFM95 (SX1276) via RadioHead RH_RF95
 *
 * Based on the Waveshare ping-pong example pattern.
 * Run: make run  (needs sudo)
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "lr1121_config.h"
#include "wavesahre_lora_1121.h"

/* ----------------------------------------------------------------
 * Radio config overrides — must match Arduino RH_RF95 TX exactly
 *
 * Arduino TX settings (RadioHead defaults):
 *   Freq     : 915 MHz
 *   SF       : 7  (128 chips/sym)
 *   BW       : 125 kHz
 *   CR       : 4/5
 *   CRC      : ON
 *   SyncWord : 0x34  (RadioHead private network)
 *   Header   : Explicit (4-byte RadioHead header prepended)
 * ---------------------------------------------------------------- */
#define GW_RF_FREQ_HZ 915000000UL
#define GW_SYNCWORD 0x12 /* LR1121 maps: 0x12→private(0x34 on air) */
#define GW_PAYLOAD_MAX 255
#define GW_CRC LR11XX_RADIO_LORA_CRC_ON /* RH_RF95 sends CRC */
#define RX_CONTINUOUS 0xFFFFFF

/* ----------------------------------------------------------------
 * Globals
 * ---------------------------------------------------------------- */
lr1121_t lr1121; /* radio context — also used by lr1121_config.c */
static volatile bool irq_flag = false;
static volatile int running = 1;
static int pkt_count = 0;

/* ----------------------------------------------------------------
 * IRQ handler (called by wiringPi on BCM 23 rising edge)
 * ---------------------------------------------------------------- */
void isr(void) { irq_flag = true; }

/* ----------------------------------------------------------------
 * Signal handler
 * ---------------------------------------------------------------- */
static void handle_signal(int sig) {
    (void)sig;
    printf("\n[gateway] Shutting down...\n");
    running = 0;
}

/* ----------------------------------------------------------------
 * Timestamp
 * ---------------------------------------------------------------- */
static void print_timestamp(void) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", tm);
    printf("[%s] ", ts);
}

/* ----------------------------------------------------------------
 * Process a received packet (mirrors ping-pong on_rx_done)
 * ---------------------------------------------------------------- */
static void on_rx_done(void) {
    uint8_t buf[GW_PAYLOAD_MAX + 1];
    uint8_t size = 0;

    lr11xx_radio_rx_buffer_status_t rx_buf_status;
    lr11xx_radio_pkt_status_lora_t pkt_status;

    lr11xx_radio_get_rx_buffer_status(&lr1121, &rx_buf_status);
    size = rx_buf_status.pld_len_in_bytes;

    if (size == 0 || size > GW_PAYLOAD_MAX) {
        printf("[rx] Invalid payload length: %d\n", size);
        return;
    }

    lr11xx_regmem_read_buffer8(&lr1121, buf, rx_buf_status.buffer_start_pointer, size);
    buf[size] = '\0';

    lr11xx_radio_get_lora_pkt_status(&lr1121, &pkt_status);

    pkt_count++;
    print_timestamp();
    printf("PKT #%d  len=%d  RSSI=%d dBm  SNR=%d dB\n", pkt_count, size, pkt_status.rssi_pkt_in_dbm,
           pkt_status.snr_pkt_in_db);

    /* RadioHead prepends 4 bytes: [TO][FROM][ID][FLAGS] */
    if (size > 4) {
        printf("  Payload : %s\n", (char *)(buf + 4));
    } else {
        printf("  Payload : (too short for RadioHead header)\n");
    }

    printf("  Raw hex : ");
    for (int i = 0; i < size; i++)
        printf("%02X ", buf[i]);
    printf("\n\n");
    fflush(stdout);
}

static void on_rx_timeout(void) { printf("[rx] Timeout — re-entering RX\n"); }

static void on_rx_crc_error(void) { printf("[rx] CRC error — packet dropped\n"); }

/* ----------------------------------------------------------------
 * IRQ processing loop (mirrors lora_irq_process from ping-pong)
 * ---------------------------------------------------------------- */
#define IRQ_MASK                                                                                   \
    (LR11XX_SYSTEM_IRQ_TX_DONE | LR11XX_SYSTEM_IRQ_RX_DONE | LR11XX_SYSTEM_IRQ_TIMEOUT |           \
     LR11XX_SYSTEM_IRQ_CRC_ERROR | LR11XX_SYSTEM_IRQ_HEADER_ERROR)

static void irq_process(void) {
    irq_flag = false;

    lr11xx_system_irq_mask_t irq_regs = 0;
    lr11xx_system_get_and_clear_irq_status(&lr1121, &irq_regs);
    irq_regs &= IRQ_MASK;

    if (irq_regs & LR11XX_SYSTEM_IRQ_RX_DONE) {
        if (irq_regs & LR11XX_SYSTEM_IRQ_CRC_ERROR) {
            on_rx_crc_error();
        } else {
            on_rx_done();
        }
    }

    if (irq_regs & LR11XX_SYSTEM_IRQ_TIMEOUT) {
        on_rx_timeout();
    }

    if (irq_regs & LR11XX_SYSTEM_IRQ_HEADER_ERROR) {
        printf("[rx] Header error\n");
    }

    /* Re-enter continuous RX after every IRQ */
    ASSERT_LR11XX_RC(lr11xx_radio_set_rx(&lr1121, RX_CONTINUOUS));
}

/* ----------------------------------------------------------------
 * main
 * ---------------------------------------------------------------- */
int main(void) {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    printf("=== Waveshare1121 LoRa Gateway ===\n");
    printf("Chip  : LR1121 (transceiver mode)\n");
    printf("Target: Arduino RFM95 @ 915 MHz, SF7, BW125, CR4/5, CRC on\n\n");

    /* ---- Hardware init (same as ping-pong) ---- */
    lora_init_io_context(&lr1121);
    lora_init_io(&lr1121);
    lora_spi_init(&lr1121);

    lora_system_init(&lr1121);   /* clocks, RF switch, regulator */
    lora_print_version(&lr1121); /* prints HW/FW version         */

    /* ---- Radio init using demo's lora_radio_init as base,
            then override freq, syncword, CRC, payload length    ---- */
    lora_radio_init(&lr1121); /* sets PKT_TYPE, mod params, PA, etc. */

    /* Override modulation parameters explicitly */
    lr11xx_radio_mod_params_lora_t mod_params = {
        .sf = LR11XX_RADIO_LORA_SF7,
        .bw = LR11XX_RADIO_LORA_BW_125,
        .cr = LR11XX_RADIO_LORA_CR_4_5,
        .ldro = 0 // Low data rate optimize - auto or 0 for SF7
    };
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_mod_params(&lr1121, &mod_params));
    printf("[init] Modulation: SF7, BW125, CR4/5\n");

    /* Override frequency to 915 MHz */
    ASSERT_LR11XX_RC(lr11xx_radio_set_rf_freq(&lr1121, GW_RF_FREQ_HZ));
    printf("[init] RF frequency overridden to %lu Hz\n", (unsigned long)GW_RF_FREQ_HZ);

    /* Set sync word to 0x12 (RadioHead private network default) */
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_sync_word(&lr1121, 0x12));
    printf("[init] SyncWord set to 0x12 (RadioHead private)\n");

    /* Override packet params with CRC ON */
    lr11xx_radio_pkt_params_lora_t pkt_params = {
        .preamble_len_in_symb = 8,                     /* 8 symbols to match RadioHead */
        .header_type = LR11XX_RADIO_LORA_PKT_EXPLICIT, /* Explicit header */
        .pld_len_in_bytes = GW_PAYLOAD_MAX,
        .crc = LR11XX_RADIO_LORA_CRC_ON, /* CRC ON to match RadioHead */
        .iq = LR11XX_RADIO_LORA_IQ_STANDARD,
    };
    ASSERT_LR11XX_RC(lr11xx_radio_set_lora_pkt_params(&lr1121, &pkt_params));
    printf("[init] Preamble=8, CRC ON, max payload=%d bytes\n", GW_PAYLOAD_MAX);

    /* ---- IRQ setup ---- */
    lora_init_irq(&lr1121, &isr);
    ASSERT_LR11XX_RC(lr11xx_system_set_dio_irq_params(&lr1121, IRQ_MASK, 0));
    ASSERT_LR11XX_RC(lr11xx_system_clear_irq_status(&lr1121, LR11XX_SYSTEM_IRQ_ALL_MASK));

    /* ---- Enter continuous RX ---- */
    ASSERT_LR11XX_RC(lr11xx_radio_set_rx(&lr1121, RX_CONTINUOUS));
    printf("\n[rx] Listening for packets (continuous RX)...\n\n");

    int heartbeat = 0;
    while (running) {
        if (irq_flag) {
            irq_process();
        } else {
            usleep(10000); /* 10 ms */
            heartbeat++;
            if (heartbeat >= 1000) { /* every ~10 seconds */
                heartbeat = 0;
                printf("[heartbeat] Waiting... pkt_count=%d\n", pkt_count);
                fflush(stdout);
            }
        }
    }

    lr11xx_system_set_standby(&lr1121, LR11XX_SYSTEM_STANDBY_CFG_RC);
    printf("[gateway] Exited. Total packets received: %d\n", pkt_count);
    return 0;
}