# Initial process execs /init.


.globl start

# exec(init, argv)
start:
    LDR     r1, =init
    LDR     r2, =argv
    MOV     r0, #1

halt:
    wfe
    b halt

# char init[] = "/init\0";
init:
    .string "/init\0"

# char *argv[] = { init, 0 };
.p2align 2
argv:
    .word init
    .word 0

