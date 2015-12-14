#ifndef UART_H
#define UART_H

#define DEVICE_BASE 0x10000000
#define UART0       0x101f1000

void uart_init();
void uart_putc(unsigned char byte);
void uart_puts(const char* str);
char uart_getc();

#endif
