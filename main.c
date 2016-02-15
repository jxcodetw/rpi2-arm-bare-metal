#include "types.h"
#include "arm.h"
#include "defs.h"
#include "timer.h"

void kmain(uint32_t r0, uint32_t r1, uint32_t atags)
{
    (void)r0;
    (void)r1;
    (void)atags;

    while(1);
}
