#include <stdbool.h>
#include <stdint.h>
#include "device/uart.h"
#include "device/mmu.h"
#include "device/timer.h"
#include "device/interrupts.h"

unsigned int *counter;

void print_hex(unsigned int val) {
    char digit[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    char number[8] = {'0','0','0','0','0','0','0','0'};
    unsigned int base = 16;
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
    counter = (unsigned int *)0x30700000;
    (void)r0;
    (void)r1;
    (void)atags;

    char c;
    uart_init();
    print_hex(*counter);
    *counter+=1;
    uart_puts("Hello, Kernel World!\r\n");
    //mmu_init();
    *counter+=1;
    timer_init();


    while(true) {
        *counter+=1;
        print_hex(*counter);
        uart_puts("before getc!\r\n");
        c = uart_getc();
        uart_puts("after getc!\r\n");
        if (c == '0') {
            uart_puts("enable_interrupt()...");
            enable_interrupts();
            uart_puts("OK\r\n");
            break;
        } else {
            uart_putc(c);
        }
    }
    uart_puts("outside!\r\n");
    while(1){}
    uart_puts("outside2!\r\n");
    //while(1){}
}
