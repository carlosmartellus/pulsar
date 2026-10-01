#pragma once

#include "ds/heap.h"
#include "ds/ring.h"


typedef struct {

}

/* Inicializa la memoria, las colas y el Ring Buffer */
void scheduler_init(void);

/* Inyecta peticiones de prueba al Ring Buffer (Mock del SDK) */
void scheduler_submit_mock_request(uint16_t tenant_id, uint8_t op_type, uint64_t hash);

/* El latido del motor: drena el buffer y despacha */
void scheduler_tick(uint32_t current_time_ms);