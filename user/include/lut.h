#ifndef LUT_H
#define LUT_H

#include <stdint.h>

#define MAX_ACTIVE_PCBS       2048u
#define STRESS_RESOLUTION      100u
#define STRESS_INDEX_MAX       (STRESS_RESOLUTION - 1u)

#define EWMA_ALPHA_NUMERATOR     1u
#define EWMA_ALPHA_DENOMINATOR   5u
#define ROUNDING_DIVISOR         2u

#define FIXED_SCALE            1024u
#define FIXED_SHIFT              10u

#define HUNGER_SCALE           1000u
#define COMBINED_SCALE         (HUNGER_SCALE * FIXED_SCALE)

#define Q_MAX 10.0  // Quantum máximo en ms (Estrés 0%)
#define Q_MIN  2.0  // Quantum mínimo en ms (Estrés 100%)

#define A_BASE   100.0  // ms de tolerancia base
#define DELTA_A 1000.0  // ms extra de paciencia bajo colapso

#define BETA 2.5  // Factor de corte agresivo para inquilinos (Si > 1, protege latencia)

// Estructura para el contrato de un tenant
typedef struct {
    uint16_t g_i; // Garantía base
    uint16_t w_i; // Peso comercial
} tenant_contract_t;

// Estructura para el estado de hambre de un tenant
typedef struct {
    uint16_t tenant_id;
    uint16_t raw_demand;
    uint16_t normalized_h;
} tenant_hunger_t;

// Funciones de inicialización
void stress_engine_init(void);
void lut_init(void);

// Funciones de cómputo de estrés
uint16_t compute_global_stress(uint16_t active_pcbs);

// Funciones de consulta de LUT
uint16_t get_dynamic_quantum(uint16_t stress_idx);
uint32_t get_aging_threshold(uint16_t stress_idx);

// Funciones de gestión de tenants
uint16_t compute_tenant_allowance(
    uint16_t stress_idx,
    uint16_t h_i,
    const tenant_contract_t* contract
);

void compute_dynamic_hunger(
    tenant_hunger_t* tenants, 
    uint16_t active_tenant_count
);

// Tablas LUT (externas para que sean accesibles desde otros archivos)
extern uint16_t lut_quantum[STRESS_RESOLUTION];
extern uint32_t lut_aging_threshold[STRESS_RESOLUTION];
extern uint16_t lut_elastic_decay[STRESS_RESOLUTION];

#endif