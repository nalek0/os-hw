// Sleeping locks
#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"

struct {
  struct sleeplock locks[SLTABLE_SIZE];
  uint8 used[SLTABLE_SIZE];
  struct spinlock lock;
} sleeplocktable;

void sleeplocktableinit() {
  for (uint8 i = 0; i < SLTABLE_SIZE; i++) {
    initsleeplock(&sleeplocktable.locks[i], "Global sleep lock");

    sleeplocktable.used[i] = 0;
  }

  initlock(&sleeplocktable.lock, "Sleeplock table");
}

int sleeplocktable_init() {
  int result = -1;
  
  acquire(&sleeplocktable.lock);

  for (uint8 i = 0; i < SLTABLE_SIZE; i++) {
    if (sleeplocktable.used[i] == 0) {
      sleeplocktable.used[i] = 1;
      result = i;

      break;
    }
  }
  
  release(&sleeplocktable.lock);

  return result;
}

void sleeplocktable_remove(uint8 ind) {
  acquire(&sleeplocktable.lock);

  sleeplocktable.used[ind] = 0;
  
  release(&sleeplocktable.lock);
}

void sleeplocktable_acquire(uint8 ind) {
  struct sleeplock * lock;

  acquire(&sleeplocktable.lock);

  lock = &sleeplocktable.locks[ind];
  
  release(&sleeplocktable.lock);

  acquiresleep(lock);
}

void sleeplocktable_release(uint8 ind) {
  struct sleeplock * lock;

  acquire(&sleeplocktable.lock);

  lock = &sleeplocktable.locks[ind];
  
  release(&sleeplocktable.lock);

  releasesleep(lock);
}



void
initsleeplock(struct sleeplock *lk, char *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

void
acquiresleep(struct sleeplock *lk)
{
  acquire(&lk->lk);
  while (lk->locked) {
    sleep(lk, &lk->lk);
  }
  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

void
releasesleep(struct sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

int
holdingsleep(struct sleeplock *lk)
{
  int r;
  
  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  return r;
}
