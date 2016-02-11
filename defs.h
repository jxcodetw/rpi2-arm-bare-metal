#ifndef DEFS_H
#define DEFS_H

#include "types.h"

// string.c
uint32 strlen(const char* str);

// uart.c
void uart_init();
void uart_putc(unsigned char byte);
void uart_puts(const char* str);
unsigned char uart_getc();
void test_mmu_putc(unsigned char byte);

#endif
