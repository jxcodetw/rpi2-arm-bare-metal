#include "defs.h"
#include "rpi2.h"
#include "timer.h"
#include "spinlock.h"

#define TIMER_LOAD    0
#define TIMER_CURVAL  1
#define TIMER_CONTROL 2
#define TIMER_INTCLR  3
#define TIMER_RAWIRQ  4
#define TIMER_MIS     5

struct spinlock tickslock;
uint ticks;

static void ack_timer() {
    volatile uint *timer0 = P2V(TIMER0);
    timer0[TIMER_INTCLR] = 1;
    uart_puts("timer_IRQ hit.\r\n");
}

void timer_init() {
    volatile uint *timer0 = P2V(TIMER0);

    initlock(&tickslock, "time");

    /* Setup the system timer interrupt */
    /* Timer frequency = Clk/256 * 0x400 */
    timer0[TIMER_LOAD] = 0x2000;
    timer0[TIMER_CONTROL] =
        RPI_ARMTIMER_CTRL_23BIT |
        RPI_ARMTIMER_CTRL_ENABLE |
        RPI_ARMTIMER_CTRL_INT_ENABLE |
        RPI_ARMTIMER_CTRL_PRESCALE_256;
    pic_enable(RPI_BASIC_ARM_TIMER_IRQ, isr_timer);
}

void isr_timer (struct trapframe *tf, int irq_idx) {
    (void)tf;
    (void)irq_idx;
    acquire(&tickslock);
    //ticks++;
    //wakeup(&ticks);
    release(&tickslock);
    ack_timer();
}
