#include <stdint.h>
#include <stdio.h>

#define PACKET_SIZE 1024
#define CHECKSUM_SIZE 4 /* CRC32 */
#define FILENAME_MAX_LEN 64
#define HASH_LEN 32 /* SHA-256 hex string */

typedef struct DataPacket {
    uint32_t session_id;
    uint32_t seq;
    uint16_t data_len;
    uint32_t timestamp;
    char checksum[CHECKSUM_SIZE];  // MD5 checksum
    char payload[PACKET_SIZE];     // 1KB payload
};

typedef struct {
    uint32_t session_id;
    uint32_t seq;
} AckPacket;

typedef struct {
    uint32_t session_id;             /* Unique transfer ID */
    char filename[FILENAME_MAX_LEN]; /* e.g. "data.bin" */
    uint64_t file_size;              /* File length in bytes */
    uint32_t total_packets;          /* Total number of chunks */
    uint16_t packet_size;            /* Typically 1024 */
    char file_hash[HASH_LEN];        /* Optional, hex string */
} FileMetadata;

/* Function prototypes */
void send_file(const char *filename, int sock, struct sockaddr_in *receiver);
int receive_file(int sock, struct sockaddr_in *sender);
int send_ack(int sock, struct sockaddr_in *receiver, uint32_t session_id, uint32_t seq);
int receive_ack(int sock, struct sockaddr_in *sender, uint32_t *session_id, uint32_t *seq);
