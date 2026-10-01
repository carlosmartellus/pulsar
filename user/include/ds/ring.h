#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include "../ppp.h"

#define RING_SIZE 4096
#define RING_MASK (RING_SIZE - 1)

/*
 * Ring Buffer SPSC (Single-Producer, Single-Consumer).
 */
typedef struct {
    ppp_t buffer[RING_SIZE];
    
    _Atomic uint32_t head; // Dónde escribe el productor (SDK)
    _Atomic uint32_t tail; // Dónde lee el consumidor (Scheduler)
} ring_ppp_t;


void ring_init(ring_ppp_t* ring);

/* 
 * Inyecta un pasaporte al buffer. (Llamado por el SDK del tenant).
 * Devuelve false si el anillo está lleno.
 */
bool ring_push(ring_ppp_t* ring, const ppp_t* ppp);

/* 
 * Extrae un pasaporte del buffer. (Llamado por el scheduler).
 * Devuelve false si el anillo está vacío.
 */
bool ring_pop(ring_ppp_t* ring, ppp_t* out_ppp);