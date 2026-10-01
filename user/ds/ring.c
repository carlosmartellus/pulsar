#include "ds/ring.h"

void ring_init(ring_ppp_t* ring) {
    atomic_init(&ring->head, 0);
    atomic_init(&ring->tail, 0);
}

bool ring_push(ring_ppp_t* ring, const ppp_t* ppp) {
    // Cargamos los índices atómicamente
    uint32_t current_head = atomic_load_explicit(&ring->head, memory_order_relaxed);
    uint32_t current_tail = atomic_load_explicit(&ring->tail, memory_order_acquire);

    // Si la diferencia es igual al tamaño, dimos la vuelta completa: el buffer está lleno
    if (current_head - current_tail == RING_SIZE) {
        return false; 
    }

    // Escribimos el pasaporte por valor
    ring->buffer[current_head & RING_MASK] = *ppp;

    // Publicamos el nuevo head para que el consumidor lo vea
    atomic_store_explicit(&ring->head, current_head + 1, memory_order_release);
    
    return true;
}

bool ring_pop(ring_ppp_t* ring, ppp_t* out_ppp) {
    // Cargamos los índices atómicamente
    uint32_t current_tail = atomic_load_explicit(&ring->tail, memory_order_relaxed);
    uint32_t current_head = atomic_load_explicit(&ring->head, memory_order_acquire);

    // Si head y tail son iguales, nadie ha escrito nada nuevo: el buffer está vacío
    if (current_tail == current_head) {
        return false;
    }

    // Leemos el pasaporte
    *out_ppp = ring->buffer[current_tail & RING_MASK];

    // Publicamos el nuevo tail para liberar el espacio al productor
    atomic_store_explicit(&ring->tail, current_tail + 1, memory_order_release);
    
    return true;
}