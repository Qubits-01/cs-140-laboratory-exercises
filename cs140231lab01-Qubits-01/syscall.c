#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int fetchint(uint addr, int *ip)
{
  struct proc *curproc = myproc();

  if (addr >= curproc->sz || addr + 4 > curproc->sz)
    return -1;
  *ip = *(int *)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int fetchstr(uint addr, char **pp)
{
  char *s, *ep;
  struct proc *curproc = myproc();

  if (addr >= curproc->sz)
    return -1;
  *pp = (char *)addr;
  ep = (char *)curproc->sz;
  for (s = *pp; s < ep; s++)
  {
    if (*s == 0)
      return s - *pp;
  }
  return -1;
}

// Fetch the nth 32-bit system call argument.
int argint(int n, int *ip)
{
  return fetchint((myproc()->tf->esp) + 4 + 4 * n, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.
int argptr(int n, char **pp, int size)
{
  int i;
  struct proc *curproc = myproc();

  if (argint(n, &i) < 0)
    return -1;
  if (size < 0 || (uint)i >= curproc->sz || (uint)i + size > curproc->sz)
    return -1;
  *pp = (char *)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int argstr(int n, char **pp)
{
  int addr;
  if (argint(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_yield(void);
extern int sys_shutdown(void);
extern int sys_hello(void);
extern int sys_getname(void);
extern int sys_appendname(void);
extern int sys_coinflip(void);

static int (*syscalls[])(void) = {
    [SYS_fork] sys_fork,
    [SYS_exit] sys_exit,
    [SYS_wait] sys_wait,
    [SYS_pipe] sys_pipe,
    [SYS_read] sys_read,
    [SYS_kill] sys_kill,
    [SYS_exec] sys_exec,
    [SYS_fstat] sys_fstat,
    [SYS_chdir] sys_chdir,
    [SYS_dup] sys_dup,
    [SYS_getpid] sys_getpid,
    [SYS_sbrk] sys_sbrk,
    [SYS_sleep] sys_sleep,
    [SYS_uptime] sys_uptime,
    [SYS_open] sys_open,
    [SYS_write] sys_write,
    [SYS_mknod] sys_mknod,
    [SYS_unlink] sys_unlink,
    [SYS_link] sys_link,
    [SYS_mkdir] sys_mkdir,
    [SYS_close] sys_close,
    [SYS_yield] sys_yield,
    [SYS_shutdown] sys_shutdown,
    [SYS_hello] sys_hello,
    [SYS_getname] sys_getname,
    [SYS_appendname] sys_appendname,
    [SYS_coinflip] sys_coinflip,
};

// Coinflip states for all of the syscalls.
// Value 0 : No coinflip effect.
// Value 1 : Has coinflip effect.
int coinflip_states[NELEM(syscalls)];

// Custom code. Initialize the coinflip states. That is, set all of the
// coinflip states to 0.
int was_initiated = 0;
void init_coinflip_states(void)
{
  if (was_initiated == 0)
  {
    for (int i = 0; i < NELEM(syscalls); i++)
    {
      coinflip_states[i] = 0;
    }

    was_initiated = 1;

    cprintf("\nInitial coinflip states:\n");
    // Debugging. Print the coinflip states.
    for (int i = 0; i < NELEM(syscalls); i++)
    {
      cprintf("coinflip_states[%d]: %d\n", i, coinflip_states[i]);
    }
    cprintf("\n");
  }
}

// Custom made. A pseudorandom generator using XorShift.
// Will be used by the coinflip syscall.
unsigned int x = 120901;
unsigned int random(int max)
{
  x ^= x << 17;
  x ^= x >> 7;
  x ^= x << 5;

  return x % max;
}

// Custom made. Determine if int [n] is a valid
// syscall number. Will return 1 if it is valid, 0 otherwise.
// Will be used by the coinflip syscall.
int is_valid_syscall_number(int n)
{
  // cprintf("NELEM(syscalls): %d\n", NELEM(syscalls));

  if (n > 0 && n < NELEM(syscalls) && syscalls[n])
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

// Custom made. Put the coinflip effect on the syscall number [n].
// Will be used by the coinflip syscall.
void put_coinflip_effect(int n)
{
  coinflip_states[n] = 1;
}

// Custom made. Display the coinflip states.
// Will be used by the coinflip syscall.
void display_coinflip_states(void)
{
  for (int i = 0; i < NELEM(syscalls); i++)
  {
    cprintf("coinflip_states[%d]: %d\n", i, coinflip_states[i]);
  }
}

void syscall(void)
{
  int num;
  struct proc *curproc = myproc();

  // Initialize the coinflip states once.
  init_coinflip_states();

  num = curproc->tf->eax;
  if (num > 0 && num < NELEM(syscalls) && syscalls[num])
  {
    // Coinflip effect.
    if (coinflip_states[num] == 1)
    {
      // Debugging message.
      cprintf("Coinflip effect for syscall num %d is active (50 percent chance of failing).\n", num);

      // 50% chance of failing.
      // 0 means the corresponding syscall will fail.
      // 1 means the corresponding syscall will succeed.
      if (random(2) == 0)
      {
        // Debugging message.
        cprintf("Syscall FAILED.\n");

        cprintf("%d %s: unknown sys call %d\n",
                curproc->pid, curproc->name, num);
        curproc->tf->eax = -1;
      }
      else
      {
        // Debugging message.
        cprintf("Syscall SUCCEEDED.\n");
        curproc->tf->eax = syscalls[num]();
      }
    }
    else
    {
      curproc->tf->eax = syscalls[num]();
    }
  }
  else
  {
    cprintf("%d %s: unknown sys call %d\n",
            curproc->pid, curproc->name, num);
    curproc->tf->eax = -1;
  }
}
