#include "uart.h"
#include "../../lib/string.h"

void uart_init() {

}

char uart_getc() {
    while(1) {}
    return '0';
}

void uart_putc(unsigned char byte) {
    volatile char * uart0 = (char*)(UART0);
    *uart0 = byte;
}

void uart_puts(const char* str) {
    for(;*str!='\0';str++) {
        uart_putc(*str);
    }
}
