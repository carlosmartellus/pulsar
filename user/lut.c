#include "include/lut.h"

static uint32_t alpha_fp = 0u;
static uint32_t one_minus_alpha_fp = 0u;
static uint32_t current_stress_fp = 0u;

// Definición de las tablas LUT (visibles externamente)
uint16_t lut_quantum[STRESS_RESOLUTION];
uint32_t lut_aging_threshold[STRESS_RESOLUTION];
uint16_t lut_elastic_decay[STRESS_RESOLUTION];

void stress_engine_init(void) {
    uint32_t alpha_scaled = 
        (FIXED_SCALE * EWMA_ALPHA_NUMERATOR + (EWMA_ALPHA_DENOMINATOR / 2u)) 
        / EWMA_ALPHA_DENOMINATOR;

    alpha_fp = alpha_scaled;
    one_minus_alpha_fp = FIXED_SCALE - alpha_fp;
    
    // Inicializar el estrés al 50% para evitar arranque en frío
    current_stress_fp = FIXED_SCALE / ROUNDING_DIVISOR;
}

uint16_t compute_global_stress(uint16_t active_pcbs) {
    if (active_pcbs > MAX_ACTIVE_PCBS) {
        active_pcbs = MAX_ACTIVE_PCBS;
    }

    uint32_t instantaneous_load = 
        ((uint32_t)active_pcbs * STRESS_INDEX_MAX) / MAX_ACTIVE_PCBS;

    uint32_t load_fp = instantaneous_load << FIXED_SHIFT;

    // EWMA: S_t = alpha * carga + (1 - alpha) * S_{t-1}
    current_stress_fp = 
        ((alpha_fp * load_fp) + (one_minus_alpha_fp * current_stress_fp))
        >> FIXED_SHIFT;

    uint32_t stress_idx = current_stress_fp >> FIXED_SHIFT;

    if (stress_idx > STRESS_INDEX_MAX) {
        stress_idx = STRESS_INDEX_MAX;
    }

    return (uint16_t)stress_idx;
}

uint16_t get_dynamic_quantum(uint16_t stress_idx) {
    // Clamping seguro con límite superior
    if (stress_idx >= STRESS_RESOLUTION) {
        stress_idx = STRESS_RESOLUTION - 1u;
    }
    
    // Asegurar que no sea negativo (uint16_t no puede ser negativo)
    return lut_quantum[stress_idx];
}

uint32_t get_aging_threshold(uint16_t stress_idx) {
    if (stress_idx >= STRESS_RESOLUTION) {
        stress_idx = STRESS_RESOLUTION - 1u;
    }
    
    return lut_aging_threshold[stress_idx];
}

uint16_t compute_tenant_allowance(
    uint16_t stress_idx,
    uint16_t h_i,
    const tenant_contract_t* contract
) {
    if (stress_idx >= STRESS_RESOLUTION) {
        stress_idx = STRESS_RESOLUTION - 1u;
    }

    uint32_t decay_fp = lut_elastic_decay[stress_idx];

    /*
     * Fórmula:
     * allowance = g_i + w_i * (h_i / HUNGER_SCALE) * (decay_fp / FIXED_SCALE)
     *
     * Para mantener todo en enteros y aplicar piso matemático:
     * elastic_part = (w_i * h_i * decay_fp) / (HUNGER_SCALE * FIXED_SCALE)
     */
    uint64_t elastic_scaled = (uint64_t)contract->w_i * h_i * decay_fp;
    uint32_t elastic_part = (uint32_t)(elastic_scaled / COMBINED_SCALE);

    uint32_t total = (uint32_t)contract->g_i + elastic_part;

    if (total > UINT16_MAX) {
        total = UINT16_MAX;
    }

    return (uint16_t)total;
}

void compute_dynamic_hunger(tenant_hunger_t* tenants, uint16_t active_tenant_count) {
    if (active_tenant_count == 0u) {
        return;
    }

    uint16_t h_max = tenants[0].raw_demand;
    uint16_t h_min = tenants[0].raw_demand;

    for (uint16_t i = 1u; i < active_tenant_count; i++) {
        if (tenants[i].raw_demand > h_max) {
            h_max = tenants[i].raw_demand;
        }

        if (tenants[i].raw_demand < h_min) {
            h_min = tenants[i].raw_demand;
        }
    }

    // Verificación de seguridad para evitar underflow
    uint32_t range = 0u;
    if (h_max >= h_min) {
        range = (uint32_t)h_max - h_min;
    }

    for (uint16_t i = 0u; i < active_tenant_count; i++) {
        if (range == 0u) {
            /*
             * Si todos tienen exactamente la misma demanda,
             * cada uno está al 100 % de la demanda máxima observada.
             */
            tenants[i].normalized_h = HUNGER_SCALE;
        } else {
            uint32_t numerator = 
                (uint32_t)(tenants[i].raw_demand - h_min) * HUNGER_SCALE;
            
            // Asegurar que no exceda el rango de uint16_t
            uint32_t normalized = numerator / range;
            tenants[i].normalized_h = (uint16_t)((normalized > HUNGER_SCALE) ? HUNGER_SCALE : normalized);
        }
    }
}

void lut_init(void) {
    for (uint16_t i = 0u; i < STRESS_RESOLUTION; i++) {
        // Mapeamos el índice [0, 99] a un Estrés continuo S_t en el rango [0.0, 1.0]
        double s_t = (double)i / (double)STRESS_INDEX_MAX;

        /* 
         * Ecuación 1: Quantum Dinámico (Transición lineal)
         * Q(S_t) = Q_min + (Q_max - Q_min) * (1 - S_t)
         */
        double q_val = Q_MIN + (Q_MAX - Q_MIN) * (1.0 - s_t);
        lut_quantum[i] = (uint16_t)(q_val + 0.5);  // Redondeo manual sin math.h

        /* 
         * Ecuación 2: Umbral de Aging (Estiramiento cuadrático)
         * A(S_t) = A_base + Delta_A * S_t^2
         */
        double a_val = A_BASE + DELTA_A * (s_t * s_t);
        lut_aging_threshold[i] = (uint32_t)(a_val + 0.5);  // Redondeo manual

        /* 
         * Ecuación 3: Decaimiento Elástico para el Techo de Tenants
         * Factor: (1 - S_t)^beta 
         * Se escala multiplicándolo por FIXED_SCALE (1024) para su uso posterior en punto fijo.
         */
        double decay_val = pow(1.0 - s_t, BETA);
        lut_elastic_decay[i] = (uint16_t)((decay_val * (double)FIXED_SCALE) + 0.5);
    }
}