#ifndef MMU_H
#define MMU_H

#define PTE_BITS_L1_SECTION (0x2)
#define PTE_L1_SECTION_PADDR_BASE_MASK (0xfff00000)
#define PAGE_TABLE_L1_BASE_ADDR_MASK   (0xffffc000)
#define VIRT_TO_PTE_L1_INDEX(addr)     (((addr) & 0xfff00000)>>18)

#define L1_PTR_BASE_ADDR  (0x30700000)
#define PHYSICAL_MEM_ADDR (0x00000000)
#define VIRTUAL_MEM_ADDR  (0x00000000)
#define MEM_MAP_SIZE      (0x40000000) //1GB

#define PHYSICAL_IO_ADDR  (0x3f000000)
#define VIRTUAL_IO_ADDR   (0xC8000000)
#define IO_MAP_SIZE       (0x18000000)

void mmu_init();
#endif
