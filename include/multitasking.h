// The maximum number of user procs
#define MAX_USER_PROCS 7

// The maximum number of kernel procs
#define MAX_KERN_PROCS 1

// The maximum number of total procs
#define MAX_PROCS MAX_USER_PROCS + MAX_KERN_PROCS

// All possible statuses for processes
typedef enum
{
	// Add process statuses here
	PROC_READY,
    PROC_RUNNING,
    PROC_TERMINATED
} proc_status_t;

// All possible types of processes
typedef enum
{
	// Add process types here
    PROC_USER,
	PROC_KERNEL
} proc_type_t;


// Process control block
// Contains all registers and info for each process
typedef struct
{
    int pid;
	proc_type_t type;
	proc_status_t status;
	uint32 eax;
	uint32 ebx;
	uint32 ecx;
	uint32 edx;
	uint32 esi;
	uint32 edi;
	void *ebp;
	void *esp;
	uint32 eflags;
	uint32 cr3;
	void *eip;
} proc_t;

int users_ready();
int createproc(void *func, void *stack);
int startkernel(void func());
void switchcontext();
void exit();
void yield();
int schedule();
