#ifndef PROC_H
#define PROC_H

#define NCPU 4

struct cpu {
    unsigned char           id;
    //struct context*   scheduler;    // swtch() here to enter scheduler
    volatile uint   started;        // Has the CPU started?

    int             ncli;           // Depth of pushcli nesting.
    int             intena;         // Were interrupts enabled before pushcli?
};

extern struct cpu cpus[NCPU];
extern struct cpu* cpu;

#endif
