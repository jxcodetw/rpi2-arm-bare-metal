#include "types.h"
#include "memlayout.h"
#include "arm.h"
#include "defs.h"
#include "proc.h"
#include "mmu.h"
#include "rpi2.h"

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
    uart_puts("kmain ker_ker_ri!\r\n");
    unsigned char c;

    cpu = &cpus[0];
    init_vmm();
    kpt_freerange(align_up(&__end, PT_SZ), P2V_WO(INIT_KERNMAP));
    paging_init(INIT_KERNMAP, PHYSTOP);

    // interrupt related
    trap_init();
    pic_init(P2V(VIC_BASE));
    timer_init();

    // file system init

    while(1) {
        c = uart_getc();
        if (c == '0') {
            sti();
        } else {
            uart_putc(c);
        }
    }
}
