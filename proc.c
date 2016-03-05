#include "types.h"
#include "defs.h"
#include "proc.h"
#include "param.h"
#include "spinlock.h"
#include "mmu.h"

struct {
    struct spinlock lock;
    struct proc proc[NPROC];
} ptable;

struct proc *curproc;
int nextpid = 1;

void forkret(void);
extern void trapret(void);

void pinit(void) {
    initlock(&ptable.lock, "ptable");
}

static struct proc* find_unused_proc(void) {
    struct proc *p;

    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; ++p) {
        if (p->state == UNUSED) {
            // found
            p->state = EMBRYO;
            p->pid = nextpid++;
            release(&ptable.lock);
            return p;
        }
    }
    // not found
    release(&ptable.lock);
    return NULL;
}

static struct proc* allocproc(void) {
    struct proc *p;
    char *sp;

    if ((p = find_unused_proc()) == NULL) {
        return NULL;
    }

    if ((p->kstack = kpt_alloc()) == NULL) {
        p->state = UNUSED;
        return NULL;
    }

    // setup user kstack
    sp = p->kstack + KSTACKSIZE;

    // Leave room for trapframe
    sp -= sizeof(struct trapframe);
    p->tf = (struct trapframe*)sp;
    //800257c8 <forkret>:
    // ... after forkret finished
    //800257fc:   e8bd8800    pop {fp, pc}
    sp -= 4;
    *(uint*)sp = (uint)trapret;                // will be pop to pc
    sp -= 4;
    *(uint*)sp = (uint)p->kstack + KSTACKSIZE; // will be pop to fp

    // Leave room for context
    sp -= sizeof(struct context);
    p->context = (struct context*)sp;
    memset(p->context, 0, sizeof(struct context));

    // fp = r11 frame pointer register
    //800257c8 <forkret>: // as an example
    //800257c8:   e92d4800    push    {fp, lr}
    //800257cc:   e28db004    add fp, sp, #4
    //800257fc:   e8bd8800    pop {fp, pc}

    // add 4 to skip push{fp, lr}
    // we have pushed the value we want(check the sp-=4 code above)
    p->context->lr = (uint)forkret+4;

    return p;
}

void error_init(void) {
    panic("failed to craft first process\n");
}

void userinit(void) {
    struct proc *p;
    extern char _binary_initcode_start[], _binary_initcode_size[];

    uart_puts("1\r\n");
    p = allocproc();
    uart_puts("2\r\n");
    if (p == NULL) {
        panic("userinit: allocproc error");
    }
    uart_puts("3\r\n");
    if ((p->pgdir = kpt_alloc()) == NULL) {
        panic("userinit: out of memory");
    }
    uart_puts("4\r\n");

    inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);

    uart_puts("5\r\n");
    p->size = PTE_SZ;
    memset(p->tf, 0, sizeof(struct trapframe));

    uart_puts("6\r\n");
    p->tf->r14_svc = (uint)error_init;
    p->tf->spsr = spsr_usr();
    p->tf->sp_usr = PTE_SZ; // user stack, happend to be the end of first page for now
    p->tf->lr_usr = 0;
    p->tf->pc = 0;

    uart_puts("7\r\n");
    strncpy(p->name, "initcode", sizeof(p->name));

    uart_puts("8\r\n");
    p->state = RUNNABLE;
    uart_puts("9\r\n");
}

void scheduler(void) {
    struct proc *p;

    for(;;) {
        sti();

        acquire(&ptable.lock);
        for(p = ptable.proc; p < &ptable.proc[NPROC]; ++p) {
            if (p->state != RUNNABLE) {
                continue;
            }

            curproc = p;
            // switchuvm(p);
            p->state = RUNNING;
            swtch(&cpu->scheduler, curproc->context);

            // process is done running for now
            // it should have changed its p->state before coming back
            // haven't know why
            curproc = 0;
        }
        release(&ptable.lock);
    }
}

// enter scheduler must hold only ptable.lock
// and have changed proc->state
void sched(void) {
    int intena; // interrupt enabled

    if (!holding(&ptable.lock)) {
        panic("sched ptable.lock");
    }

    if (cpu->ncli != 1) {
        panic("sched locks");
    }

    if (curproc->state == RUNNING) {
        panic("sched running");
    }

    if (int_enabled()) {
        panic("sched interruptible");
    }

    // don't know intena usage
    intena = cpu->intena;
    swtch(&curproc->context, cpu->scheduler);
    cpu->intena = intena;
}

// don't know what this function for
void yield(void) {
    acquire(&ptable.lock);
    curproc->state = RUNNABLE;
    sched();
    release(&ptable.lock);
}

void forkret(void) {
    static bool first = true;

    // still holding ptable.lock from scheduler.
    release(&ptable.lock);

    if (first) {
        // dont know why check first run here
        // need to be figure out
        first = false;
    }

    // will return to trapret (see allocproc)
}
