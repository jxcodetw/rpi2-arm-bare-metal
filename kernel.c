#include <stdbool.h>
#include <stdint.h>
#include "device/uart.h"
#include "device/mmu.h"

void print_hex(unsigned int addr) {
    char digit[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    char number[8] = {'0','0','0','0','0','0','0','0'};
    unsigned int base = 16;
    int i = 7;
    uart_putc('0');
    uart_putc('x');

    while(addr > 0) {
        number[i--] = digit[addr % base];
        addr /= base;
    }

    for(i=0;i<8;++i) {
        uart_putc(number[i]);
    }

    uart_putc('\r');
    uart_putc('\n');
}

unsigned int getcpsr() {
    unsigned int retval = 0xdeadbeef;
    asm(
        "mrs %0, cpsr;"
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
    unsigned int a = 0;
    unsigned int b = 0;
    uart_init();
    mmu_init();
    uart_puts("Hello, Kernel World!\r\n");
    uart_puts("&a: ");
    print_hex((unsigned int)&a);

    while(true) {
        c = uart_getc();
        if (c == '3') {
            b = *(volatile unsigned int *)0x30703208;
            print_hex(b);
        } else if (c == '4') {
            b = getcpsr();
            print_hex(b);
        } else if (c == '5') {
            *(volatile unsigned int *)(&a+0xc8000000) = a+1;
        } else if (c == '6') {
            b = 0;
            uart_puts("c1: ");
            asm volatile(
                "mrc p15, 0, r0, c1, c0, 0;"
                "mov %0, r0;"
                :"=r"(b)::"r0"
            );
            print_hex(b);
        } else if (c == '7') {
            b = 0;
            asm volatile(
                "mrc p15, 0, r0, c1, c0, 0;"
                "orr r0, r0, #0x1;"
                "mov %0, r0;"
                "mcr p15, 0, r0, c1, c0, 0;"
                :"=r"(b)::"r0"
            );
            print_hex(b);
            uart_puts("mmu on!\r\n");
        } else if (c == '8') {
            b = 0;
            asm volatile(
                "mrc p15, 0, r0, c1, c0, 0;"
                "bic r0, r0, #0x1;"
                "mov %0, r0;"
                "mcr p15, 0, r0, c1, c0, 0;"
                :"=r"(b)::"r0"
            );
            print_hex(b);
            uart_puts("mmu off!\r\n");
        } else if (c == '9') {
            print_hex(a);
        } else {
            uart_putc(c);
        }
    }
}
