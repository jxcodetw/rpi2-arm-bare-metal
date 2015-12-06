#ifndef UART_H
#define UART_H

void uart_init();
void uart_putc(unsigned char byte);
void uart_puts(const char* str);
unsigned char uart_getc();

#endif
