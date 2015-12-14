#include "interrupts.h"

void __attribute__((interrupt("ABORT"))) reset_vector(void) {

}

void __attribute__((interrupt("UNDEF"))) undefined_instruction_vector(void) {
    while(1) {
        /* Do Nothing! */
    }
}

void __attribute__((interrupt("SWI"))) software_interrupt_vector(void) {
    while(1) {
        /* Do Nothing! */
    }
}

void __attribute__((interrupt("ABORT"))) prefetch_abort_vector(void) {
}

void __attribute__((interrupt("ABORT"))) data_abort_vector(void) {
}

void __attribute__((interrupt("IRQ"))) interrupt_vector(void) {
}

void __attribute__((interrupt("FIQ"))) fast_interrupt_vector(void) {
}
