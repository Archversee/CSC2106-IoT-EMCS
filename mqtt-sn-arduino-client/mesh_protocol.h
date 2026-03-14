#ifndef MESH_PROTOCOL_H
#define MESH_PROTOCOL_H

#include <stdint.h>

#define MESH_DEFAULT_TTL 3
#define MESH_MAX_PAYLOAD 64

typedef struct __attribute__((packed)) {
    uint8_t src_id;
    uint8_t dst_id;
    uint8_t ttl;
    uint16_t seq_num;
} mesh_header_t;

#define MESH_HEADER_SIZE ((uint8_t)sizeof(mesh_header_t))

#endif