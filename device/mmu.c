#include "mmu.h"
#include "uart.h"

static unsigned int gen_l1_pte(unsigned int paddr) {
    return (paddr & 0xfff00000);
}

static unsigned int gen_l1_pte_addr(unsigned int baddr, unsigned int vaddr) {
    return (baddr & 0xffffc000) | VIRT_TO_PTE_L1_INDEX(vaddr);
}

void gen_mapping(unsigned int paddr,
                 unsigned int vaddr,
                 unsigned int memsize,
                 unsigned int device) {
    unsigned int pte;
    unsigned int pte_addr;
    unsigned int i;

    // map 1GB virt to phy one to one
    for(i=0;i<memsize>>20;++i) {
        pte = gen_l1_pte(paddr+(i<<20));
        //pte |= 0x2; //[1:0] = 10
        //pte |= 0x3 << 10; // AP[11:10]=11
        //pte |= 0xf << 5; // Domain[8:5]=1111
        if (device == 0) {
            pte |= 0xDE2;
        } else {
            pte |= 0xDE6;
        }
        pte_addr = gen_l1_pte_addr(L1_PTR_BASE_ADDR, vaddr+(i<<20));
        *(volatile unsigned int *)pte_addr = pte;
    }
}

void mmu_init() {
    uart_puts("mmu_init()...");
    //unsigned int ttb = L1_PTR_BASE_ADDR;
    gen_mapping(PHYSICAL_MEM_ADDR, VIRTUAL_MEM_ADDR, MEM_MAP_SIZE, 0);
    gen_mapping(PHYSICAL_IO_ADDR, VIRTUAL_IO_ADDR, IO_MAP_SIZE, 1);
    asm(
        "mov r0, #0xffffffff;"
        "mcr p15, 0, r0, c3, c0, 0;"
        "ldr r0, =0x30700000;"
        "mcr p15, 0, r0, c2, c0, 0;"
        ::: "r0"
    );
    uart_puts("done\r\n");
    /*asm(
        "mcr p15, 0, %0, c2, c0, 0;"
        "mrc p15, 0, r0, c1, c0, 0;"
        "orr r0, r0, #0x1;"
        "mcr p15, 0, r0, c1, c0, 0;"
        "mov r0, r0;"
        "mov r0, r0;"
        "mov r0, r0;"
        :
        : "r" (ttb)
        : "r0"
    );*/
}
