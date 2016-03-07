# Initial process execs /init.


.globl start

# exec(init, argv)
start:
    ldr     r0, =init
    bl      0x80020e98

halt:
    wfe
    b halt

# char init[] = "/init\0";
init:
    .string "/hello init world.\0"

# char *argv[] = { init, 0 };
.p2align 2
argv:
    .word init
    .word 0

