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

void kmain(uint32_t r0, uint32_t r1, uint32_t atags)
{
    (void)r0;
    (void)r1;
    (void)atags;

    char c;
    unsigned int b = 0;
    uart_init();
    uart_puts("Hello, Kernel World!\r\n");

    while(true) {
        c = uart_getc();
        if (c == '4') {
            b = 0;
            asm volatile(
                "mrs %0, cpsr;"
                : "=r"(b)
            );
            print_hex(b);
        } else if (c == '5') {
            asm volatile(
                "mrs r0, cpsr;"
                "bic r0, r0, #0x1f;"
                "orr r0, r0, #0xd6;"
                "msr cpsr, r0;"
                :::"r0"
            );
        } else if (c == '6') {
            asm volatile(
                "mrs r0, cpsr;"
                "bic r0, r0, #0x1f;"
                "orr r0, r0, #0xd3;"
                "msr cpsr, r0;"
                :::"r0"
            );
        } else {
            uart_putc(c);
        }
    }

}
