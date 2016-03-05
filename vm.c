#include "defs.h"
#include "mmu.h"
#include "spinlock.h"

struct run {
    struct run *next;
};

struct {
    struct spinlock lock;
    struct run *freelist;
} kpt_mem;

void init_vmm(void) {
    initlock(&kpt_mem.lock, "vm");
    kpt_mem.freelist = NULL;
}

static void _kpt_free(char *v)
{
    struct run *r;

    r = (struct run*) v;
    r->next = kpt_mem.freelist;
    kpt_mem.freelist = r;
}

// add some memory used for page tables (initialization code)
void kpt_freerange(uint32 low, uint32 hi)
{
    while (low < hi) {
        _kpt_free ((char*)low);
        low += PT_SZ;
    }
}

void* kpt_alloc(void)
{
    struct run *r;

    acquire(&kpt_mem.lock);
    if ((r = kpt_mem.freelist) != NULL ) {
        kpt_mem.freelist = r->next;
    }
    release(&kpt_mem.lock);

    memset(r, 0, PT_SZ);
    return r;
}

static void flush_tlb(void) {
    uint val = 0;
    asm("MCR p15, 0, %[r], c8, c7, 0" : :[r]"r" (val):);

    // invalidate entire data and instruction cache
    // if we run these two line the kernel will stop
    // need to check rpi2 spec
    //asm volatile("mcr p15, 0, %[r], c7, c10, 0"::[r]"r"(val):);
    //asm volatile("mcr p15, 0, %[r], c7, c11, 0"::[r]"r"(val):);
}

void paging_init(uint phy_low, uint phy_hi) {
    (void)phy_low;
    (void)phy_hi;
    //mappages()
    flush_tlb();
}
