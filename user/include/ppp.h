#pragma once

#include <stdint.h>

enum {
    OP_CPU   = 0x00, // Pura computación, sin bloqueos
    OP_READ  = 0x01, // Lectura compartida (RAR permitido)
    OP_WRITE = 0x02, // Escritura exclusiva
    OP_IO    = 0x03  // Bloqueo externo (red/disco)
};

/* 
 * Process PassPort (PPP)
 */
typedef struct {
    uint16_t tenant_id;     // 2 bytes de id
    uint8_t  op_type;       // 1 byte del ENUM de operaciones
    uint8_t  flags;         // 1 byte de padding
    uint32_t timeout_ms;    // 4 bytes

    uint64_t resource_hash; // 8 bytes: MurmurHash3. 0 si es OP_CPU.
} ppp_t;

