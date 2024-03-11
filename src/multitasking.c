#include "./types.h"
#include "./multitasking.h"
#include "./hal.h"

// An array to hold all of the processes we create
proc_t processes[MAX_PROCS];

int max_ready_pid;
int most_recent_user;

// Keep track of the next index to place a newly created process in the process array
uint8 process_index = 0;

proc_t *running;    // The currently running process, can be either kernel or user process
proc_t *next;       // The next process to run
proc_t *kernel;     // The kernel process

// Create a new kernel process
// The kernel process is ran immediately, executing from the function provided (void *func)
// Stack does not to be initialized because it was already initialized when main() was called
// If we have hit the limit for maximum processes, return -1
// Store the newly created process inside the processes array (proc_t processes[])

int users_ready()
{
    for(int i=1; i<process_index; i++)
    {
        if(processes[i].status == PROC_READY)
        {
            return 1;
        }
    }

    return 0;
}

int startkernel(void func())
{
    // If we have filled our process array, return -1

    if(process_index >= MAX_PROCS)
    {
        return -1;
    }

    // Create the new kernel process

    proc_t kernproc;
    kernproc.status = PROC_RUNNING; // Processes start ready to run
    kernproc.type = PROC_KERNEL;    // Process is a kernel process

    // Assign a process ID and add process to process array

    kernproc.pid = process_index;
    processes[process_index] = kernproc;
    kernel = &processes[process_index];     // Use a proc_t pointer to keep track of the kernel process so we don't have to loop through the entire process array to find it
    process_index++;

    // Assign the kernel to the running process and execute

    running = kernel;
    func();

    return 0;
}

int createproc(void *func, void *stack)
{
    if(process_index >= MAX_PROCS)
    {
        return -1;
    }

    proc_t userproc;
    userproc.status = PROC_READY;
    userproc.type = PROC_USER;
    userproc.esp = stack;
    userproc.ebp = stack;
    userproc.eip = func;
    userproc.pid = process_index;

    max_ready_pid = process_index;

    processes[process_index] = userproc;
    process_index++;

    return 0;
}

void exit()
{
    running->status = PROC_TERMINATED;
    if(running->type == PROC_USER)
    {
        next = kernel;
        switchcontext();
    } 
    return;
}

void yield()
{   
    if (running->type == PROC_KERNEL)
    {
        for(int i=0; i<process_index; i++)
        {
            if(processes[i].status == PROC_READY)
            {
                max_ready_pid = i;
            }
        }
        if(max_ready_pid)
        {
            schedule();
        } else {
            printf("\nNo process to yield to! Resuming...\n");
            return;
        }

    } else if (running->type == PROC_USER)
    {
        next = kernel;
    }
    
    if (next)
    {
        running->status = PROC_READY;
        switchcontext();
    }

    return;
}

int schedule()
{ 
    int loop_entry;

    if(most_recent_user >= max_ready_pid)
    {
        loop_entry = 1;
    } else
    {
        loop_entry = most_recent_user + 1;
    }

    for(int i=loop_entry; i<=max_ready_pid; i++)
    {
        if (processes[i].status == PROC_READY)
        {
            next = &processes[i];
            most_recent_user = next->pid;
            return 1;
        }
    }

    next = &processes[running->pid];
    return 1;
}

// Context switching function
// This function will save the context of the running process (proc_t running)
// and switch to the context of the next process we want to run (proc_t next)
// The running and next processes must both be valid for this function to work
// if they are not, our OS will certainly crash
void __attribute__((naked)) switchcontext()
{
    // Capture all the register values so we can reload them if we ever run this process again

    register uint32 eax asm ("eax");    // General purpose registers
    register uint32 ebx asm ("ebx");
    register uint32 ecx asm ("ecx");
    register uint32 edx asm ("edx");
    register uint32 esi asm ("esi");    // Indexing registers
    register uint32 edi asm ("edi");
    register void *ebp asm ("ebp");     // Stack base pointer
    register void *esp asm ("esp");     // Stack top pointer
    
    asm ("pushfl");
    asm ("pop %eax");
    register uint32 eflags asm ("eax"); // Flags and conditions

    asm ("mov %cr3, %eax");
    register uint32 cr3  asm ("eax");   // CR3 for virtual addressing

    // Store all the current register values inside the process that is running

    running->eax    = eax;
    running->ebx    = ebx;
    running->ecx    = ecx;
    running->edx    = edx;

    running->esi    = esi;
    running->edi    = edi;

    running->ebp    = ebp;
    running->esp    = esp;

    running->eflags = eflags;
    running->cr3    = cr3;

    // Set the next instruction for this process to be the resume after the context switch

    running->eip    = &&resume;

    // Start running the next process
    
    running = next;
    running->status = PROC_RUNNING;

    // Reload all the registers previously saved from the process we want to run

    asm ("mov %0, %%eax" : : "r"(running->eflags));
    asm ("push %eax");
    asm ("popfl");

    asm ("mov %0, %%eax" : :    "r"(running->eax));
    asm ("mov %0, %%ebx" : :    "r"(running->ebx));
    asm ("mov %0, %%ecx" : :    "r"(running->ecx));
    asm ("mov %0, %%edx" : :    "r"(running->edx));

    asm ("mov %0, %%esi" : :    "r"(running->esi));
    asm ("mov %0, %%edi" : :    "r"(running->edi));

    asm ("mov %0, %%ebp" : :    "r"(running->ebp));
    asm ("mov %0, %%esp" : :    "r"(running->esp));

    asm ("mov %0, %%cr3" : :    "r"(running->cr3));

    // Jump to the last instruction we saved from the running process
    // If this is a new process this will be the beginning of the process's function

    asm ("jmp *%0" : : "r" (running->eip));

    // This resume address will eventually get executed when the previous process gets executed again
    // This will allow us to resume the previous process after our yield

    resume:
    asm ("ret");
}