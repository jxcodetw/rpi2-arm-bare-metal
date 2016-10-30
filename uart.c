#include "defs.h"
#include "arm.h"
#include "memlayout.h"

void uart_putc(unsigned char byte)
{
    // Wait for UART to become ready to transmit.
    while(mmio_read(UART0_FR+KERNBASE) & (1 << 5));
    mmio_write(UART0_DR+KERNBASE, byte);
}

void uart_puts(const char* str)
{
    uint i, size;
    size = strlen(str);
    for(i=0;i<size;++i) {
        uart_putc(str[i]);
    }
}

unsigned char uart_getc()
{
    // Wait for UART to have recieved something.
    while(mmio_read(UART0_FR+KERNBASE) & (1 << 4));
    return mmio_read(UART0_DR+KERNBASE);
}

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
