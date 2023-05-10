#include <stdarg.h>

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"

struct {
    uint mode;
    uint64 since;
    uint64 until;

    struct spinlock lock;
} IDMSettings; // interruption diagnostic messages

void initIDMSettings() {
  printf("init: IDMSettings\n");

  initlock(&IDMSettings.lock, "IDMSettings");

  IDMSettings.mode = DIAG_MODE_OFF;
}

int
update_diagmode(int mode, uint64 time) {
  if (mode == DIAG_MODE_ON) {
    acquire(&IDMSettings.lock);

    IDMSettings.mode = DIAG_MODE_ON;
    IDMSettings.since = 0;
    IDMSettings.until = INF;

    release(&IDMSettings.lock);

    return 0;
  } else if (mode == DIAG_MODE_OFF) {
    acquire(&IDMSettings.lock);

    IDMSettings.mode = DIAG_MODE_OFF;
    IDMSettings.since = INF;
    IDMSettings.until = INF;

    release(&IDMSettings.lock);

    return 0;
  } else if (mode == DIAG_MODE_SECONDS) {
    acquire(&IDMSettings.lock);

    IDMSettings.mode = DIAG_MODE_SECONDS;
    acquire(&tickslock);
    IDMSettings.since = ticks;  
    IDMSettings.until = ticks + time;  
    release(&tickslock);

    release(&IDMSettings.lock);

    return 0;
  }

  return 1;
}

int 
can_send_idm() {
  uint current_time;
  int result;
  
  acquire(&tickslock);
  current_time = ticks;
  release(&tickslock);

  acquire(&IDMSettings.lock);
  result = current_time <= IDMSettings.until && current_time >= IDMSettings.since;
  release(&IDMSettings.lock);

  return result;
}
