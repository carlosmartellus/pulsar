#pragma once

#include <stdint.h>
#include "ppp.h"

/* Estados del Proceso */
enum {
    STATE_READY   = 0x00, // Listo en el Heap del MLFQ
    STATE_RUNNING = 0x01, // Actualmente en la CPU (Anillo 0)
    STATE_BLOCKED = 0x02, // Atrapado en la Lock Table (DAG)
    STATE_DONE    = 0x03  // Ejecución finalizada (listo para reciclaje)
};

/* 
 * Process Control Block (PCB)
 */
typedef struct __attribute__((aligned(32))) {
    ppp_t passport;         // Toda la intención del inquilino

    uint32_t linux_pid;     // 4 bytes: ID del proceso en el kernel
    uint32_t enqueue_ts;    // 4 bytes: Marca de tiempo absoluta (Para FCFS y Preemption)

    uint16_t quantum_used;  // 2 bytes: ms consumidos. (65,535 ms max, sobra para CPU)
    uint8_t  state;         // 1 byte:  Estado actual (STATE_*)
    uint8_t  mlfq_level;    // 1 byte:  Cola actual (0=High, 1=Mid, 2=Low)
    uint16_t heap_idx;      // 2 bytes: Su posición actual en el Min-Heap (Para borrado O(1))
    uint16_t dag_next_idx;  // 2 bytes: Índice del siguiente PCB esperando el mismo recurso
} pcb_t;