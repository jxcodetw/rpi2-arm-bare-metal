#include "defs.h"

extern void kmain(void);
extern void jump_stack(void);

void start(void) {
    uart_init();
    uart_puts("starting os...!\r\n");
    while(1);
    jump_stack();
    kmain();
}
