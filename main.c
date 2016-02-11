#include "types.h"
#include "arm.h"
#include "defs.h"
#include "timer.h"
#include "interrupts.h"


void print_hex(uint val) {
    char digit[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    char number[8] = {'0','0','0','0','0','0','0','0'};
    uint base = 16;
    int i = 7;
    uart_putc('0');
    uart_putc('x');

    while(val > 0) {
        number[i--] = digit[val % base];
        val /= base;
    }

    for(i=0;i<8;++i) {
        uart_putc(number[i]);
    }

    uart_putc('\r');
    uart_putc('\n');
}

unsigned int getcpsr() {
    unsigned int retval = 0;
    asm(
        "mrs %0, cpsr;"
        : "=r"(retval)
    );
    return retval;
}

unsigned int getcp15c1() {
    unsigned int retval = 0;
    asm(
        "mrc p15, #0, %0, c1, c0, #0;"
        : "=r"(retval)
    );
    return retval;
}

void kmain(uint32_t r0, uint32_t r1, uint32_t atags)
{
    (void)r0;
    (void)r1;
    (void)atags;

    char c;
    uart_init();
    uart_puts("Hello, Kernel World!\r\n");
    //mmu_init();
    timer_init();

    while(true) {
        c = uart_getc();
        if (c == '0') {
            enable_interrupts();
            break;
        } else {
            uart_putc(c);
        }
    }
    while(1){}
}
