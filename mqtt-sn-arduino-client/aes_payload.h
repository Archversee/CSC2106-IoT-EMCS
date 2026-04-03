#ifndef AES_PAYLOAD_H
#define AES_PAYLOAD_H

/*
 * aes_payload.h — Shared AES-128 key for LoRa payload encryption
 *
 * Used by:
 *   Arduino  (mqtt-sn-arduino-client) via AESLib
 *   RPi gate (Waveshare1121-Gateway)  via OpenSSL
 *
 *  both sides MUST match.
 */

#define AES_KEY_LEN 16

static const unsigned char AES_SHARED_KEY[AES_KEY_LEN] = {
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};

#endif /* AES_PAYLOAD_H */