#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int sys_fork(void)
{
  return fork();
}

int sys_exit(void)
{
  exit();
  return 0; // not reached
}

int sys_wait(void)
{
  return wait();
}

int sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int sys_getpid(void)
{
  return myproc()->pid;
}

int sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

int sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_yield(void)
{
  yield();
  return 0;
}

int sys_shutdown(void)
{
  shutdown();
  return 0;
}

int sys_hello(void)
{
  cprintf("Hello from the xv6 kernel!\n");
  return 0;
}

int sys_getname(void)
{
  char *name;

  if (argstr(0, &name) < 0)
  {
    return -1;
  }

  safestrcpy(name, myproc()->name, 16);

  return strlen(name);
}

int sys_appendname(void)
{
  char *name;

  // [ Step 1. ]
  // Get the string [name] from the syscall argument.
  if (argstr(0, &name) < 0)
  {
    return -1;
  }

  // // For debugging purposes. Display the strings.
  // cprintf("name: %s\n", name);
  // cprintf("mproc()->name: %s\n", myproc()->name);
  // cprintf("\n");

  // [ Step 2. ]
  // Determine the string length of [name] and [myproc()->name].
  int name_length = strlen(name);
  int myproc_name_length = strlen(myproc()->name);

  // // For debugging purposes. Display the string lengths.
  // cprintf("name_length: %d\n", name_length);
  // cprintf("myproc_name_length: %d\n", myproc_name_length);
  // cprintf("\n");

  // [ Step 3.]
  // Determine if resulting string will be valid. If not, return -1.
  int combined_length = name_length + myproc_name_length;
  if ((combined_length > 15) || (combined_length == 0))
  {
    return -1;
  }

  // Append name to myproc()->name.
  // [ Step 4.1. ]
  // Copy [myproc()->name] to [resulting_name].
  char resulting_name[16];
  int i = 0;

  while (myproc()->name[i] != '\0')
  {
    resulting_name[i] = myproc()->name[i];
    i++;
  }

  // [ Step 4.2. ]
  // Append [name] to [resulting_name].
  int j = 0;
  while (name[j] != '\0')
  {
    resulting_name[i + j] = name[j];
    j++;
  }

  // [ Step 4.3. ]
  // Add a null terminator at the end of the char array.
  resulting_name[i + j] = '\0';

  // [ Step 5. ]
  // Overwrite myproc()->name with [resulting_name].
  safestrcpy(myproc()->name, resulting_name, 16);

  // For debugging purposes. Verify that the string [name] was appended
  // to myproc()->name correctly.
  cprintf("new myproc()->name: %s | (%d)\n", myproc()->name, combined_length);

  return 0;
}

int sys_coinflip(void)
{
  // [ Step 1. ]
  // Get the int [n] from the syscall argument.
  int n;
  if (argint(0, &n) < 0)
  {
    return -1;
  }

  // [ Step 2. ]
  // Determine if [n] is valid. If not, return -1.
  if (is_valid_syscall_number(n) == 1)
  {
    // Valid syscall number.

    // [ Step 3. ]
    // Apply the coinflip effect to the syscall number [n].
    put_coinflip_effect(n);

    // For debugging purposes. Display the coinflip states.
    display_coinflip_states();

    return 0;
  }
  else
  {
    // Invalid syscall number.
    return -1;
  }
}
