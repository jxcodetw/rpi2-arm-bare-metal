#include "types.h"
#include "memlayout.h"
#include "arm.h"

static void uart_putc(unsigned char byte)
{
    // Wait for UART to become ready to transmit.
    while(mmio_read(UART0_FR) & (1 << 5));
    mmio_write(UART0_DR, byte);
}

static void uart_puts(const char* str)
{
    while(*str != '\0') {
        uart_putc(*str);
        str++;
    }
}

void kmain(uint32 r0, uint32 r1, uint32 atags)
{
    (void)r0;
    (void)r1;
    (void)atags;

    uart_puts("in kmain.\r\n");
    while(1);
}
