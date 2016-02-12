#include "arm.h"
#include "defs.h"
#include "rpi2.h"

static rpi_irq_controller_t* rpiIRQController =
        (rpi_irq_controller_t*)RPI_INTERRUPT_CONTROLLER_BASE;

rpi_irq_controller_t* RPI_GetIrqController( void ) {
    return rpiIRQController;
}


// dispatch the interrupt
void pic_dispatch (struct trapframe *tp) {
    isr_timer(tp, 0);
}
