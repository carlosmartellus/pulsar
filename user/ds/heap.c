#include "ds/heap.h"


/* 
 * Intercambia dos elementos en el arreglo del Heap y, 
 * en el mismo ciclo, actualiza el "GPS" interno de sus PCBs.
 */
static inline void heap_swap(min_heap_t* h, uint16_t idx_a, uint16_t idx_b, pcb_t* global_pcbs) {
    uint16_t pcb_a = h->elements[idx_a];
    uint16_t pcb_b = h->elements[idx_b];

    // Swap físico en el arreglo
    h->elements[idx_a] = pcb_b;
    h->elements[idx_b] = pcb_a;

    // Actualización O(1) en los PCBs
    global_pcbs[pcb_a].heap_idx = idx_b;
    global_pcbs[pcb_b].heap_idx = idx_a;
}

static void bubble_up(min_heap_t* h, uint16_t idx, pcb_t* global_pcbs) {
    while (idx > 0) {
        uint16_t parent_idx = (idx - 1) / 2;
        
        uint32_t ts_current = global_pcbs[h->elements[idx]].enqueue_ts;
        uint32_t ts_parent  = global_pcbs[h->elements[parent_idx]].enqueue_ts;

        if (ts_current < ts_parent) {
            heap_swap(h, idx, parent_idx, global_pcbs);
            idx = parent_idx;
        } else {
            break;
        }
    }
}

static void bubble_down(min_heap_t* h, uint16_t idx, pcb_t* global_pcbs) {
    while (1) {
        uint16_t left_child  = 2 * idx + 1;
        uint16_t right_child = 2 * idx + 2;
        uint16_t smallest    = idx;

        if (left_child < h->size) {
            uint32_t ts_smallest = global_pcbs[h->elements[smallest]].enqueue_ts;
            uint32_t ts_left     = global_pcbs[h->elements[left_child]].enqueue_ts;
            if (ts_left < ts_smallest) {
                smallest = left_child;
            }
        }

        if (right_child < h->size) {
            uint32_t ts_smallest = global_pcbs[h->elements[smallest]].enqueue_ts;
            uint32_t ts_right    = global_pcbs[h->elements[right_child]].enqueue_ts;
            if (ts_right < ts_smallest) {
                smallest = right_child;
            }
        }

        if (smallest == idx) {
            break;
        }

        heap_swap(h, idx, smallest, global_pcbs);
        idx = smallest;
    }
}


void heap_init(min_heap_t* h) {
    h->size = 0;
}

bool heap_push(min_heap_t* h, uint16_t pcb_idx, pcb_t* global_pcbs) {
    if (h->size >= MAX_HEAP) {
        return false;
    }

    uint16_t insert_idx = h->size;
    h->elements[insert_idx] = pcb_idx;
    global_pcbs[pcb_idx].heap_idx = insert_idx;
    h->size++;

    bubble_up(h, insert_idx, global_pcbs);
    return true;
}

uint16_t heap_pop(min_heap_t* h, pcb_t* global_pcbs) {
    if (h->size == 0) {
        return NULL_IDX;
    }

    uint16_t root_pcb = h->elements[0];
    global_pcbs[root_pcb].heap_idx = NULL_IDX; // Ya no está en el heap

    h->size--;
    if (h->size > 0) {
        h->elements[0] = h->elements[h->size];
        global_pcbs[h->elements[0]].heap_idx = 0;
        bubble_down(h, 0, global_pcbs);
    }

    return root_pcb;
}

uint16_t heap_peek(const min_heap_t* h) {
    return (h->size > 0) ? h->elements[0] : NULL_IDX;
}

void heap_remove(min_heap_t* h, uint16_t pcb_idx, pcb_t* global_pcbs) {
    uint16_t idx_in_heap = global_pcbs[pcb_idx].heap_idx;
    
    if (idx_in_heap == NULL_IDX || idx_in_heap >= h->size) {
        return; 
    }

    global_pcbs[pcb_idx].heap_idx = NULL_IDX;
    h->size--;

    if (idx_in_heap == h->size) {
        // Si el elemento a borrar era justo el último, ya terminamos
        return;
    }

    h->elements[idx_in_heap] = h->elements[h->size];
    global_pcbs[h->elements[idx_in_heap]].heap_idx = idx_in_heap;

    bubble_up(h, idx_in_heap, global_pcbs);
    bubble_down(h, idx_in_heap, global_pcbs);
}