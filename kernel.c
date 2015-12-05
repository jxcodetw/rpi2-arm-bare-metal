#include <stdbool.h>
#include <stdint.h>
#include "device/uart.h"

void kmain(uint32_t r0, uint32_t r1, uint32_t atags)
{
    (void)r0;
    (void)r1;
    (void)atags;

    char c;
    uart_init();
    uart_puts("Hello, Kernel World!\r\n");

    while(true) {
        c = uart_getc();
        c += 1;
        if (c == '\r') {
            uart_puts("\r\n");
        } else {
            uart_putc(c);
        }
    }

}
