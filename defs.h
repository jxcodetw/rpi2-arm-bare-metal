#ifndef DEFS_H
#define DEFS_H

#include "arm.h"
#include "types.h"

// arm.c
void cli(void);
void sti(void);

// picirq.c
void pic_dispatch (struct trapframe *tp);

// string.c
void* memset(void *dst, int v, int n);
uint32 strlen(const char* str);

// timer.c
void isr_timer (struct trapframe *tp, int irq_idx);

// trap.c
void dump_trapframe(struct trapframe *);

// uart.c
void uart_putc(unsigned char byte);
void uart_puts(const char* str);
unsigned char uart_getc();
void test_mmu_putc(unsigned char byte);

#endif
