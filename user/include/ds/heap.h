#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "../pcb.h"

/* 
 * Límites del sistema. 
 * MAX_HEAP incluye un pequeño buffer para procesos provenientes de I/O.
 */
#define MAX_ACTIVE 2048
#define MAX_HEAP   (MAX_ACTIVE + (MAX_ACTIVE >> 4))
#define NULL_IDX   0xFFFF                           // Centinela para "Vacío"

/*
 * Estructura plana del Min-Heap.
 */
typedef struct {
    uint16_t elements[MAX_HEAP]; 
    uint16_t size;
} min_heap_t;


/* Inicializa el tamaño a 0 */
void heap_init(min_heap_t* h);

/* 
 * Devuelve false si el heap está lleno (Pánico de sistema).
 */
bool heap_push(min_heap_t* h, uint16_t pcb_idx, pcb_t* global_pcbs);

/* 
 * Extrae y retorna el índice del proceso con el enqueue_ts más antiguo.
 * Devuelve NULL_IDX si está vacío.
 */
uint16_t heap_pop(min_heap_t* h, pcb_t* global_pcbs);

/* 
 * Mira la raíz sin extraerla.
 */
uint16_t heap_peek(const min_heap_t* h);

/* 
 * Elimina un proceso desde cualquier lugar del árbol.
 */
void heap_remove(min_heap_t* h, uint16_t pcb_idx, pcb_t* global_pcbs);