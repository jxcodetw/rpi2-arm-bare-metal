#include "types.h"
#include "arm.h"

// disable interrupt
void cli(void) {
    uint32 val;

    asm("mrs %[v], cpsr":[v]"=r"(val)::);
    val |= DIS_INT;
    asm("msr cpsr_c, %[v]"::[v]"r"(val):);
}

// enable interrupt
void sti(void) {
    uint32 val;
    
    asm("mrs %[v], cpsr":[v]"=r"(val)::);
    val &= ~DIS_INT;
    asm("msr cpsr_c, %[v]"::[v]"r"(val):);
}

// return if interrupt is enabled
bool int_enabled(void) {
    uint32 val;

    asm("mrs %[v], cpsr":[v]"=r"(val)::);

    // if DIS_INT bit is set(1) then interrupt is disabled
    // so we add a not(!) here
    return !(val & DIS_INT);
}
