#include "memlayout.h"
#include "mmu.h"

typedef unsigned int uint32_t;
extern void kmain(void);
extern void jump_stack(void);


static void uart_init()
{
    // Disable UART0.
    mmio_write(UART0_CR, 0x00000000);
    // Setup the GPIO pin 14 && 15.

    // Disable pull up/down for all GPIO pins & delay for 150 cycles.
    mmio_write(GPPUD, 0x00000000);
    delay(150);

    // Disable pull up/down for pin 14,15 & delay for 150 cycles.
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);

    // Write 0 to GPPUDCLK0 to make it take effect.
    mmio_write(GPPUDCLK0, 0x00000000);

    // Clear pending interrupts.
    mmio_write(UART0_ICR, 0x7FF);

    // Set integer & fractional part of baud rate.
    // Divider = UART_CLOCK/(16 * Baud)
    // Fraction part register = (Fractional part * 64) + 0.5
    // UART_CLOCK = 3000000; Baud = 115200.

    // Divider = 3000000 / (16 * 115200) = 1.627 = ~1.
    // Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
    mmio_write(UART0_IBRD, 1);
    mmio_write(UART0_FBRD, 40);

    // Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
    mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

    // Mask all interrupts.
    mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
    (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));

    // Enable UART0, receive & transfer part of UART.
    mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

static void uart_putc(unsigned char byte)
{
    // Wait for UART to become ready to transmit.
    while(mmio_read(UART0_FR) & (1 << 5));
    mmio_write(UART0_DR, byte);
}

static void uart_puts(const char* str)
{
    while(*str != '\0') {
        uart_putc(*str);
        str++;
    }
}

extern uint _kernel_pgtbl;
extern uint _user_pgtbl;

uint *kernel_pgtbl = &_kernel_pgtbl;
uint *user_pgtbl = &_user_pgtbl;

#define PDE_SHIFT 20

// setup the boot page table: dev_mem whether it is device memory
void set_bootpgtbl (uint32 virt, uint32 phy, uint len, int dev_mem )
{
    uint32	pde;
    uint    idx;

    // convert all the parameters to indexes
    virt >>= PDE_SHIFT;
    phy  >>= PDE_SHIFT;
    len  >>= PDE_SHIFT;

    for (idx = 0; idx < len; idx++) {
        pde = (phy << PDE_SHIFT);

        if (!dev_mem) {
            // normal memory, make it kernel-only, cachable, bufferable
            pde |= (AP_KO << 10) | PE_CACHE | PE_BUF | KPDE_TYPE;
        } else {
            // device memory, make it non-cachable and non-bufferable
            pde |= (AP_KO << 10) | KPDE_TYPE;
        }

        // use different page table for user/kernel space
        if (virt < NUM_UPDE) {
            user_pgtbl[virt] = pde;
        } else {
            kernel_pgtbl[virt] = pde;
        }
        //kernel_pgtbl[virt] = pde;
        virt++;
        phy++;
    }
}

static void _flush_all (void)
{
    uint val = 0;
    // flush all TLB
    asm("MCR p15, 0, %[r], c8, c7, 0" : :[r]"r" (val):);
}

void load_pgtlb ()
{
    uint val;
    // set domain access control: all domain will be checked for permission
    val = 0x55555555;
    asm("MCR p15, 0, %[v], c3, c0, 0": :[v]"r" (val):);

    // set the page table base registers. We use two page tables: TTBR0
    // for user space and TTBR1 for kernel space
    val = 32 - UADDR_BITS;
    asm("MCR p15, 0, %[v], c2, c0, 2": :[v]"r" (val):);

    // set the kernel page table
    val = (uint)kernel_pgtbl | 0x00;
    asm("MCR p15, 0, %[v], c2, c0, 1": :[v]"r" (val):);

    // set the user page table
    val = (uint)user_pgtbl | 0x00;
    asm("MCR p15, 0, %[v], c2, c0, 0": :[v]"r" (val):);

    // ok, enable paging using read/modify/write
    asm("MRC p15, 0, %[r], c1, c0, 0": [r]"=r" (val)::);

    val |= 0x80300D; // enable MMU, cache, write buffer, high vector tbl,
                     // disable subpage
    asm("MCR p15, 0, %[r], c1, c0, 0": :[r]"r" (val):);

    _flush_all();
}

void print_hex(uint val) {
    char digit[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    char number[8] = {'0','0','0','0','0','0','0','0'};
    uint base = 16;
    int i = 7;
    uart_putc('0');
    uart_putc('x');

    while(val > 0) {
        number[i--] = digit[val % base];
        val /= base;

    }

    for(i=0;i<8;++i) {
        uart_putc(number[i]);

    }

    uart_putc('\r');
    uart_putc('\n');

}

void start(void) {
    uart_init();
    uart_puts("starting os...\r\n");
    
    // double map the low memory, required to enable paging
    // we do not map all the physical memory
    set_bootpgtbl(0, 0, INIT_KERNMAP, 0); 
    set_bootpgtbl(KERNBASE, 0, INIT_KERNMAP, 0); 
    set_bootpgtbl(0x3f200000, 0x3f200000, 0x18000000, 1);  // DEVICE MAP

    uart_puts("loading pgtlb...");
    load_pgtlb (kernel_pgtbl, user_pgtbl);
    uart_puts("done\r\n");
    //jump_stack();
    //kmain();
    while(1);
}
