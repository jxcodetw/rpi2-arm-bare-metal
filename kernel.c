#include <stdbool.h>
#include <stdint.h>
#include "device/uart.h"
#include "device/mmu.h"

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

void kmain(uint32_t r0, uint32_t r1, uint32_t atags)
{
    (void)r0;
    (void)r1;
    (void)atags;

    char c;
    unsigned int * ptr_low;
    unsigned int * ptr_high;
    unsigned int a = 0;
    unsigned int b = 0;
    unsigned int d = 0;
    unsigned int e = 0;
    ptr_low = &a;
    ptr_high = (unsigned int *)((unsigned int)ptr_low + 0xc8000000);
    d = (unsigned int)ptr_high;
    e = (unsigned int)ptr_high;
    uart_init();
    uart_puts("Hello, Kernel World!\r\n");
    mmu_init();
    uart_puts("&a: ");
    print_hex((unsigned int)&a);
    uart_puts("ptr low: ");
    print_hex((unsigned int)ptr_low);
    uart_puts("ptr high: ");
    print_hex((unsigned int)ptr_high);

    uart_puts("ptr high[31:20]: ");
    d = d >> 20;
    print_hex(d);
    uart_puts("entry: ");
    d = 0x30700000 | d << 2;
    print_hex(d);
    uart_puts("entry val: ");
    d = *(volatile unsigned int *)d;
    print_hex(d);
    uart_puts("phy addr: ");
    d = (d & 0xfff00000) | (e & 0x000fffff);
    print_hex(d);

    while(true) {
        c = uart_getc();
        if (c == '3') {
            print_hex(*ptr_low);
            print_hex(*ptr_high);
        } else if (c == '4') {
            b = getcpsr();
            print_hex(b);
        } else if (c == '5') {
            *ptr_low = *ptr_low + 1;
            *ptr_high = *ptr_high + 1;
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
                "mov r0, r0;"
                "mov r0, r0;"
                "mov r0, r0;"
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
            uart_puts("mmu started");
        } else if (c == '0') {
            //uart_puts("ptr high: ");
            //print_hex((unsigned int)ptr_high);
            //*ptr_high = 0;
            test_mmu_putc('0');
        } else {
            uart_putc(c);
        }
    }
}
