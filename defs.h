#ifndef DEFS_H
#define DEFS_H

#include "arm.h"
#include "types.h"

struct spinlock;
struct trapframe;

// arm.c
void cli(void);
void sti(void);
void pushcli(void);
void popcli(void);

// picirq.c
void pic_dispatch (struct trapframe *tp);

// string.c
void*  memset(void *dst, int v, int n);
uint32 strlen(const char* str);

// spinlock.c
void   acquire(struct spinlock*);
int    holding(struct spinlock*);
void   initlock(struct spinlock*, char*);
void   release(struct spinlock*);

// timer.c
void isr_timer (struct trapframe *tp, int irq_idx);

// trap.c
void trap_init(void);
void dump_trapframe(struct trapframe *);

// uart.c
void uart_putc(unsigned char byte);
void uart_puts(const char* str);
unsigned char uart_getc();
void print_hex(uint val);

// vm.c
void init_vmm(void);
void kpt_freerange(uint32 low, uint32 high);

// main.c
void panic(char* str);

#endif
