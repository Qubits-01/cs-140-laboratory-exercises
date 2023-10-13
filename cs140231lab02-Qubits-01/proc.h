// Per-CPU state
struct cpu
{
  uchar apicid;              // Local APIC ID
  struct context *scheduler; // swtch() here to enter scheduler
  struct taskstate ts;       // Used by x86 to find stack for interrupt
  struct segdesc gdt[NSEGS]; // x86 global descriptor table
  volatile uint started;     // Has the CPU started?
  int ncli;                  // Depth of pushcli nesting.
  int intena;                // Were interrupts enabled before pushcli?
  struct proc *proc;         // The process running on this cpu or null
};

extern struct cpu cpus[NCPU];
extern int ncpu;

// PAGEBREAK: 17
//  Saved registers for kernel context switches.
//  Don't need to save all the segment registers (%cs, etc),
//  because they are constant across kernel contexts.
//  Don't need to save %eax, %ecx, %edx, because the
//  x86 convention is that the caller has saved them.
//  Contexts are stored at the bottom of the stack they
//  describe; the stack pointer is the address of the context.
//  The layout of the context matches the layout of the stack in swtch.S
//  at the "Switch stacks" comment. Switch doesn't save eip explicitly,
//  but it is on the stack and allocproc() manipulates it.
struct context
{
  uint edi;
  uint esi;
  uint ebx;
  uint ebp;
  uint eip;
};

enum procstate
{
  UNUSED,
  EMBRYO,
  SLEEPING,
  RUNNABLE,
  RUNNING,
  ZOMBIE
};

// Per-process state
struct proc
{
  uint sz;                    // Size of process memory (bytes)
  pde_t *pgdir;               // Page table
  char *kstack;               // Bottom of kernel stack for this process
  enum procstate state;       // Process state
  int pid;                    // Process ID
  struct proc *parent;        // Parent process
  struct trapframe *tf;       // Trap frame for current syscall
  struct context *context;    // swtch() here to run process
  void *chan;                 // If non-zero, sleeping on chan
  int killed;                 // If non-zero, have been killed
  struct file *ofile[NOFILE]; // Open files
  struct inode *cwd;          // Current directory
  char name[16];              // Process name (debugging)
  int nfork;                  // Number of forks made by this process.
  uint created_at_ticks;      // Number of ticks this process was created at.

  // Ref: https://pythontutor.com/render.html#code=%23include%20%3Cstdio.h%3E%0A%0Astruct%20obj%20%7B%0A%20%20char%20name%5B5%5D%3B%0A%20%20int%20id%3B%0A%7D%3B%0A%0Aint%20main%28%29%20%7B%0A%20%20struct%20obj%20*cont%5B3%5D%3B%0A%20%20%0A%20%20struct%20obj%20*myObj1%20%3D%20%28struct%20obj%20*%29%20malloc%28sizeof%28struct%20obj%29%29%3B%0A%20%20struct%20obj%20*myObj2%20%3D%20%28struct%20obj%20*%29%20malloc%28sizeof%28struct%20obj%29%29%3B%0A%20%20struct%20obj%20*myObj3%20%3D%20%28struct%20obj%20*%29%20malloc%28sizeof%28struct%20obj%29%29%3B%0A%20%20%0A%20%20cont%5B0%5D%20%3D%20myObj1%3B%0A%20%20cont%5B1%5D%20%3D%20myObj2%3B%0A%20%20cont%5B2%5D%20%3D%20myObj3%3B%0A%20%20%0A%20%20cont%5B1%5D-%3Ename%5B0%5D%20%3D%20%22Z%22%3B%0A%20%20cont%5B0%5D-%3Eid%20%3D%2069%3B%0A%7D&cumulative=false&curInstr=9&heapPrimitives=nevernest&mode=display&origin=opt-frontend.js&py=c_gcc9.3.0&rawInputLstJSON=%5B%5D&textReferences=false
  int children[999];  // PIDs of the children of this process.
  int children_count; // Number of children of this process.
};

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap

int pcbclear(int pid);
