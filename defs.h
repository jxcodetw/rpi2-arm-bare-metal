#ifndef DEFS_H
#define DEFS_H

#include "arm.h"
#include "types.h"

struct spinlock;
struct trapframe;

// arm.c
void   set_stk(uint mode, uint addr);
void*  get_fp (void);
void   cli(void);
void   sti(void);
void   pushcli(void);
void   popcli(void);

// picirq.c
void   pic_dispatch (struct trapframe *tp);

// string.c
void*  memset(void *dst, int v, int n);
uint32 strlen(const char* str);

// spinlock.c
void   acquire(struct spinlock*);
int    holding(struct spinlock*);
void   initlock(struct spinlock*, char*);
void   release(struct spinlock*);

// timer.c
void   timer_init(void);
void   isr_timer(struct trapframe *tp, int irq_idx);

// trap.c
void   trap_init(void);
void   dump_trapframe(struct trapframe *);

// trap_asm.S
void   trap_reset(void);
void   trap_und(void);
void   trap_swi(void);
void   trap_iabort(void);
void   trap_dabort(void);
void   trap_na(void);
void   trap_irq(void);
void   trap_fiq(void);

// uart.c
void uart_putc(unsigned char byte);
void uart_puts(const char* str);
unsigned char uart_getc();
void print_hex(uint val);

// vm.c
void init_vmm(void);
void kpt_freerange(uint32 low, uint32 high);
void* kpt_alloc(void);

// main.c
void panic(char* str);

#endif
