#include "defs.h"
#include "mmu.h"
#include "memlayout.h"
#include "spinlock.h"

extern uint _kernel_pgtbl;

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
    uart_puts("kpt  low: ");
    print_hex(low);
    uart_puts("kpt high: ");
    print_hex(hi);
    uart_puts("alloc num: ");
    print_hex((hi-low) / PT_SZ);
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

static pte_t* walkpgdir(pde_t *pgdir, const void *va, int alloc) {
    pde_t *pde;
    pte_t *pgtab;

    pde = &pgdir[PDE_IDX(va)];

    if (*pde & PE_TYPES) {
        pgtab = (pte_t*)P2V(PT_ADDR(*pde));
    } else {
        if (alloc) {
            if ((pgtab = (pte_t*)kpt_alloc()) == 0) {
                return 0;
            }
        } else {
            return 0;
        }
        memset(pgtab, 0, PT_SZ);
        *pde = V2P(pgtab) | UPDE_TYPE;
    }

    return &pgtab[PTE_IDX(va)];
}

static int mappages(pde_t *pgdir, uint va, uint size, uint pa, uint ap) {
    char *a, *last;
    pte_t *pte;

    a    = (char*)align_dn(va       , PTE_SZ);
    last = (char*)align_dn(va+size-1, PTE_SZ);

    for(;;) {
        if ((pte = walkpgdir(pgdir, a, 1)) == 0) {
            return -1;
        }

        if (*pte & PE_TYPES) {
            panic("remap");
        }

        *pte = pa | ((ap & 0x3) << 4) | PE_CACHE | PE_BUF | PTE_TYPE;

        if (a == last) {
            break;
        }
        a  += PTE_SZ;
        pa += PTE_SZ;
    }

    return 0;
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
    // _kernel_pgtbl is preserved in kernel.ld
    // AP_KU means full access
    mappages(P2V(&_kernel_pgtbl), P2V_WO(phy_low), phy_hi-phy_low, phy_low, AP_KU);
    flush_tlb();
}
