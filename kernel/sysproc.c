#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "syslockactions.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_dmesg(void)
{
  uint64 addr;
  argaddr(0, &addr);
  cpybuf(addr);

  return 0;
}

uint64
sys_dmesgsend(void)
{
  char msg[128];

  argstr(0, msg, 128);

  pr_msg(msg);
  
  return 0;
}

uint64
sys_hello(void)
{
  int n1, n2;
  
  argint(0, &n1);
  argint(1, &n2);

  return n1 + n2;
}

uint64 
sys_lockcall(void)
{
  int action, action_arg;
  
  argint(0, &action);
  argint(1, &action_arg);

  switch (action)
  {
  case INIT_ACTION:
    return sleeplocktable_init();
  case REMOVE_ACTION:
    sleeplocktable_remove(action_arg);

    return 0;
  case ACQUIRE_ACTION:
    sleeplocktable_acquire(action_arg);

    return 0;
  case RELEASE_ACTION:
    sleeplocktable_release(action_arg);

    return 0;
  default:
    return 1;
  }
}

uint64 
sys_diagmode(void) {
  int settings_id;
  int mode;
  uint64 arg;
  
  argint(0, &settings_id);
  argint(1, &mode);
  argaddr(2, &arg);

  return update_diagmode(settings_id, mode, arg);
}
