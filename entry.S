#include "arm.h"
#include "memlayout.h"

.text
.code 32

.global _start

_start:
    // Setup SVC mode stack
    mov r0, #(SVC_MODE | CPSR_IRQ_INHIBIT | CPSR_FIQ_INHIBIT )
    msr cpsr_c, r0
    //mov sp, #(64 * 1024 * 1024)
    ldr     sp, =svc_stktop

    // Clear out bss.
    ldr r4, =edata_entry
    ldr r9, =end_entry
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
    b       2f

1:
    // store multiple at r4.
    stmia r4!, {r5-r8}

    // If we are still below bss_end, loop.
2:
    cmp r4, r9
    blo 1b

    ldr r3, =start
    blx r3

halt:
    wfe
    b halt

# during startup, kernel stack uses user address, now switch it to kernel addr
.global jump_stack
jump_stack:
    MOV     r0, sp
    ADD     r0, r0, #KERNBASE
    MOV     sp, r0
    MOV     pc, lr
