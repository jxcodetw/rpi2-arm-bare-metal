#include "types.h"
#include "memlayout.h"
#include "arm.h"
#include "defs.h"
#include "cpu.h"
#include "mmu.h"

extern void* __end;

struct cpu cpus[NCPU];
struct cpu * cpu;

void panic(char* str)
{
    cli(); // disable interrupt
    uart_puts("kernel panic");
    if (str != NULL) {
        uart_puts(str);
    }
    while (1);
}

void kmain(void)
{
    uart_puts("kmain ker_ri!\r\n");
    unsigned char c;

    cpu = &cpus[0];
    init_vmm();
    kpt_freerange(align_up(&__end, PT_SZ), P2V_WO(INIT_KERNMAP));
    uart_puts("trap_init...");
    trap_init();
    uart_puts("done\r\n");
    uart_puts("timer...");
    timer_init();
    uart_puts("done\r\n");

    while(1) {
        c = uart_getc();
        if (c == '0') {
            sti();
        } else {
            uart_putc(c);
        }
    }
}
