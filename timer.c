#include "defs.h"
#include "rpi2.h"
#include "timer.h"

static rpi_arm_timer_t* rpiTimer = (rpi_arm_timer_t*)RPI_ARMTIMER_BASE;

rpi_arm_timer_t* RPI_GetArmTimer(void) {
    return rpiTimer;
}

void timer_init() {
    RPI_GetIrqController()->Enable_Basic_IRQs = RPI_BASIC_ARM_TIMER_IRQ;

    /* Setup the system timer interrupt */
    /* Timer frequency = Clk/256 * 0x400 */
    rpiTimer->Load = 0x2000;

    rpiTimer->Control =
        RPI_ARMTIMER_CTRL_23BIT |
        RPI_ARMTIMER_CTRL_ENABLE |
        RPI_ARMTIMER_CTRL_INT_ENABLE |
        RPI_ARMTIMER_CTRL_PRESCALE_256;
}

void isr_timer (struct trapframe *tp, int irq_idx) {
    (void)tp;
    (void)irq_idx;
    RPI_GetArmTimer()->IRQClear = 1;
    uart_puts("timer_IRQ hit.\r\n");
}

