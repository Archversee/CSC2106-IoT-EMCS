/*!
 * @file    data_frame.c
 * @brief   File deconstruction and reconstruction implementation
 * @author  INF2004 Team
 * @date    2024
 */

#include "data_frame.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../drivers/microsd_driver.h"

/*! Storage layout constants */
#define METADATA_BASE_ADDRESS (0U)  /*!< Base address for metadata storage */
#define PACKET_BASE_ADDRESS (1024U) /*!< Base address for packet storage */
#define MAX_STORED_FILES (10U)      /*!< Maximum number of stored files */
#define MAX_PACKETS_PER_FILE (100U) /*!< Maximum packets per file */
#define METADATA_SIZE (sizeof(file_metadata_t))
#define PACKET_STORAGE_SIZE (sizeof(data_packet_t))

/*! Private function prototypes */
static uint32_t calculate_crc32(uint8_t const *const p_data, size_t const length);
static bool validate_packet_checksum(data_packet_t const *const p_packet);
static void generate_packet_checksum(data_packet_t *const p_packet);
static uint32_t generate_session_id(void);

/*!
 * @brief Calculate CRC32 checksum
 * @param[in] p_data    Pointer to data buffer
 * @param[in] length    Length of data
 * @return uint32_t     CRC32 checksum
 */
static uint32_t calculate_crc32(uint8_t const *const p_data, size_t const length) {
    uint32_t crc = 0xFFFFFFFFU;
    uint32_t const polynomial = 0xEDB88320U;

    if (NULL != p_data) {
        for (size_t i = 0U; i < length; i++) {
            crc ^= p_data[i];

            for (int j = 0; j < 8; j++) {
                if (0U != (crc & 1U)) {
                    crc = (crc >> 1U) ^ polynomial;
                } else {
                    crc >>= 1U;
                }
            }
        }
    }

    return ~crc;
}

/*!
 * @brief Validate packet checksum
 * @param[in] p_packet  Pointer to data packet
 * @return bool         true if checksum is valid, false otherwise
 */
static bool validate_packet_checksum(data_packet_t const *const p_packet) {
    bool result = false;
    uint32_t calculated_crc;
    uint32_t stored_crc;

    if (NULL != p_packet) {
        calculated_crc = calculate_crc32((uint8_t const *)p_packet->payload,
                                         p_packet->data_len);

        /* Convert stored checksum from string to uint32_t */
        stored_crc = *(uint32_t const *)p_packet->checksum;

        result = (calculated_crc == stored_crc);
    }

    return result;
}

/*!
 * @brief Generate packet checksum
 * @param[in,out] p_packet  Pointer to data packet
 * @return void
 */
static void generate_packet_checksum(data_packet_t *const p_packet) {
    uint32_t crc;

    if (NULL != p_packet) {
        crc = calculate_crc32((uint8_t const *)p_packet->payload,
                              p_packet->data_len);

        /* Store CRC as binary data */
        (void)memcpy(p_packet->checksum, &crc, sizeof(uint32_t));
    }
}

/*!
 * @brief Generate unique session ID
 * @return uint32_t  Unique session identifier
 */
static uint32_t generate_session_id(void) {
    static uint32_t session_counter = 0U;

    session_counter++;

    /* Use counter only since time() may not be available on Pico */
    return session_counter;
}

/*!
 * @brief Calculate storage address for metadata
 * @param[in] file_index    File index (0-9)
 * @return uint32_t         Storage address
 */
static uint32_t get_metadata_address(uint8_t const file_index) {
    return METADATA_BASE_ADDRESS + (file_index * METADATA_SIZE);
}

/*!
 * @brief Calculate storage address for packet
 * @param[in] file_index    File index (0-9)
 * @param[in] packet_seq    Packet sequence number
 * @return uint32_t         Storage address
 */
static uint32_t get_packet_address(uint8_t const file_index, uint32_t const packet_seq) {
    return PACKET_BASE_ADDRESS +
           (file_index * MAX_PACKETS_PER_FILE * PACKET_STORAGE_SIZE) +
           (packet_seq * PACKET_STORAGE_SIZE);
}

/*!
 * @brief Find available file slot
 * @return uint8_t  File index (0-9) or 0xFF if no slot available
 */
static uint8_t find_available_file_slot(void) {
    uint8_t file_index = 0xFFU;
    file_metadata_t metadata = {0};

    for (uint8_t i = 0U; i < MAX_STORED_FILES; i++) {
        uint32_t const addr = get_metadata_address(i);
        uint8_t temp_data;

        /* Check if slot is empty (first byte is 0) */
        if (microsd_read_byte(addr, &temp_data)) {
            if (0U == temp_data) {
                file_index = i;
                break;
            }
        }
    }

    return file_index;
}

/*!
 * @brief Store metadata to microSD
 * @param[in] file_index    File index
 * @param[in] p_metadata    Pointer to metadata structure
 * @return bool             true on success, false on failure
 */
static bool store_metadata(uint8_t const file_index,
                           file_metadata_t const *const p_metadata) {
    bool result = false;
    uint32_t const addr = get_metadata_address(file_index);
    uint8_t const *p_data = (uint8_t const *)p_metadata;
    size_t i;

    if (NULL != p_metadata) {
        /* Write metadata byte by byte */
        for (i = 0U; i < sizeof(file_metadata_t); i++) {
            if (!microsd_write_byte(addr + i, p_data[i])) {
                break;
            }
        }

        if (sizeof(file_metadata_t) == i) {
            result = true;
        }
    }

    return result;
}

/*!
 * @brief Load metadata from microSD
 * @param[in]  file_index   File index
 * @param[out] p_metadata   Pointer to metadata structure
 * @return bool             true on success, false on failure
 */
static bool load_metadata(uint8_t const file_index,
                          file_metadata_t *const p_metadata) {
    bool result = false;
    uint32_t const addr = get_metadata_address(file_index);
    uint8_t *p_data = (uint8_t *)p_metadata;
    size_t i;

    if (NULL != p_metadata) {
        /* Read metadata byte by byte */
        for (i = 0U; i < sizeof(file_metadata_t); i++) {
            if (!microsd_read_byte(addr + i, &p_data[i])) {
                break;
            }
        }

        if (sizeof(file_metadata_t) == i) {
            result = true;
        }
    }

    return result;
}

/*!
 * @brief Store packet to microSD
 * @param[in] file_index    File index
 * @param[in] p_packet      Pointer to packet structure
 * @return bool             true on success, false on failure
 */
static bool store_packet(uint8_t const file_index,
                         data_packet_t const *const p_packet) {
    bool result = false;
    uint32_t const addr = get_packet_address(file_index, p_packet->seq);
    uint8_t const *p_data = (uint8_t const *)p_packet;
    size_t i;

    if (NULL != p_packet) {
        /* Write packet byte by byte */
        for (i = 0U; i < sizeof(data_packet_t); i++) {
            if (!microsd_write_byte(addr + i, p_data[i])) {
                break;
            }
        }

        if (sizeof(data_packet_t) == i) {
            result = true;
        }
    }

    return result;
}

/*!
 * @brief Load packet from microSD
 * @param[in]  file_index   File index
 * @param[in]  packet_seq   Packet sequence number
 * @param[out] p_packet     Pointer to packet structure
 * @return bool             true on success, false on failure
 */
static bool load_packet(uint8_t const file_index,
                        uint32_t const packet_seq,
                        data_packet_t *const p_packet) {
    bool result = false;
    uint32_t const addr = get_packet_address(file_index, packet_seq);
    uint8_t *p_data = (uint8_t *)p_packet;
    size_t i;

    if (NULL != p_packet) {
        /* Read packet byte by byte */
        for (i = 0U; i < sizeof(data_packet_t); i++) {
            if (!microsd_read_byte(addr + i, &p_data[i])) {
                break;
            }
        }

        if (sizeof(data_packet_t) == i) {
            result = true;
        }
    }

    return result;
}

void send_file(char const *const p_filename,
               int const sock,
               void *const p_receiver) {
    uint8_t file_index;
    file_metadata_t metadata = {0};
    data_packet_t packet = {0};
    uint32_t bytes_processed = 0U;

    /* Unused parameters for Pico implementation */
    (void)sock;
    (void)p_receiver;

    if (NULL != p_filename) {
        /* Find file in storage by filename */
        bool file_found = false;

        for (uint8_t i = 0U; i < MAX_STORED_FILES; i++) {
            if (load_metadata(i, &metadata)) {
                if (0 == strncmp(metadata.filename, p_filename, FILENAME_MAX_LEN)) {
                    file_index = i;
                    file_found = true;
                    break;
                }
            }
        }

        if (file_found) {
            printf("Deconstructing file: %s (%llu bytes, %u packets)\n",
                   metadata.filename, metadata.file_size, metadata.total_packets);

            /* Read and output each packet */
            for (uint32_t seq = 0U; seq < metadata.total_packets; seq++) {
                if (load_packet(file_index, seq, &packet)) {
                    if (validate_packet_checksum(&packet)) {
                        printf("Packet %u: %u bytes, checksum valid\n",
                               seq, packet.data_len);

                        /* Here you would send via MQTT-SN when implemented */
                        /* For now, just validate the deconstruction */
                        bytes_processed += packet.data_len;
                    } else {
                        printf("Packet %u: checksum validation failed\n", seq);
                    }
                } else {
                    printf("Failed to load packet %u\n", seq);
                }
            }

            printf("File deconstruction complete: %u bytes processed\n", bytes_processed);
        } else {
            printf("File not found in storage: %s\n", p_filename);
        }
    }
}

int receive_file(int const sock, void *const p_sender) {
    int result = -1;
    uint8_t file_index;
    file_metadata_t metadata = {0};
    data_packet_t packet = {0};
    uint32_t packets_stored = 0U;

    /* Unused parameters for Pico implementation */
    (void)sock;
    (void)p_sender;

    /* Find available file slot */
    file_index = find_available_file_slot();

    if (0xFFU != file_index) {
        /* For demonstration, create a test file reconstruction */
        /* In real implementation, this would receive via MQTT-SN */

        /* Create sample metadata */
        metadata.session_id = generate_session_id();
        (void)strncpy(metadata.filename, "received_file.bin", FILENAME_MAX_LEN - 1U);
        metadata.filename[FILENAME_MAX_LEN - 1U] = '\0';
        metadata.file_size = 2048U; /* 2KB test file */
        metadata.total_packets = (uint32_t)((metadata.file_size + PACKET_SIZE - 1U) / PACKET_SIZE);
        metadata.packet_size = PACKET_SIZE;

        printf("Reconstructing file: %s (%llu bytes, %u packets)\n",
               metadata.filename, metadata.file_size, metadata.total_packets);

        /* Store metadata */
        if (store_metadata(file_index, &metadata)) {
            /* Create and store test packets */
            for (uint32_t seq = 0U; seq < metadata.total_packets; seq++) {
                /* Fill packet with test data */
                packet.session_id = metadata.session_id;
                packet.seq = seq;
                packet.timestamp = seq; /* Use sequence as timestamp */

                /* Calculate data length for this packet */
                uint64_t const remaining_bytes = metadata.file_size - ((uint64_t)seq * PACKET_SIZE);
                packet.data_len = (remaining_bytes > PACKET_SIZE) ? PACKET_SIZE : (uint16_t)remaining_bytes;

                /* Fill with test pattern */
                for (uint16_t i = 0U; i < packet.data_len; i++) {
                    packet.payload[i] = (char)((seq + i) & 0xFFU);
                }

                /* Generate checksum */
                generate_packet_checksum(&packet);

                /* Store packet */
                if (store_packet(file_index, &packet)) {
                    packets_stored++;
                    printf("Stored packet %u/%u (%u bytes)\n",
                           seq + 1U, metadata.total_packets, packet.data_len);
                } else {
                    printf("Failed to store packet %u\n", seq);
                    break;
                }
            }

            if (packets_stored == metadata.total_packets) {
                printf("File reconstruction completed successfully!\n");
                result = 0;
            }
        } else {
            printf("Failed to store metadata\n");
        }
    } else {
        printf("No available file slots\n");
    }

    return result;
}
