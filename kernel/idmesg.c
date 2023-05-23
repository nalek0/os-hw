#include <stdarg.h>

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "proc.h"

struct idm_settings {
  uint mode;
  uint64 since;
  uint64 until;

  struct spinlock lock;
};

struct {
  struct idm_settings syscall_settings;
  struct idm_settings devintr_settings;
  struct idm_settings context_settings;
} IDMSettings; // interruption diagnostic messages

// public 
void initIDMSettings() {
  printf("init: IDMSettings\n");

  initlock(&IDMSettings.syscall_settings.lock, "IDMSettings.syscall_settings");
  initlock(&IDMSettings.devintr_settings.lock, "IDMSettings.devintr_settings");
  initlock(&IDMSettings.context_settings.lock, "IDMSettings.context_settings");

  IDMSettings.syscall_settings.mode = DIAG_MODE_OFF;
  IDMSettings.devintr_settings.mode = DIAG_MODE_OFF;
  IDMSettings.context_settings.mode = DIAG_MODE_OFF;
}

// private
void update_settings(struct idm_settings * settings, int mode, uint64 time) {
  if (mode == DIAG_MODE_ON) {
    acquire(&settings->lock);
    settings->mode = DIAG_MODE_ON;
    settings->since = 0;
    settings->until = INF;
    release(&settings->lock);
  } else if (mode == DIAG_MODE_OFF) {
    acquire(&settings->lock);
    settings->mode = DIAG_MODE_OFF;
    settings->since = INF;
    settings->until = INF;
    release(&settings->lock);
  } else if (mode == DIAG_MODE_SECONDS) {
    int ticks_now;
    acquire(&tickslock);
    ticks_now = ticks;  
    release(&tickslock);

    acquire(&settings->lock);
    settings->mode = DIAG_MODE_SECONDS;
    settings->since = ticks_now;  
    settings->until = ticks_now + time;  
    release(&settings->lock);
  }
}

// public
int update_diagmode(int settings_id, int mode, uint64 time) {
  if (settings_id == SYSCALL_IDM_SETTINGS) {
    update_settings(&IDMSettings.syscall_settings, mode, time);

    return 0;
  } else if (settings_id == DEVINTR_IDM_SETTINGS) {
    update_settings(&IDMSettings.devintr_settings, mode, time);

    return 0;
  } else if (settings_id == CONTEXT_IDM_SETTINGS) {
    update_settings(&IDMSettings.context_settings, mode, time);

    return 0;
  } else {
    return 1;
  }
}

// public
int accept_settings(int settings_id) {
  int ticks_now;
  int result;
  struct idm_settings settings;

  if (settings_id == SYSCALL_IDM_SETTINGS) {
    settings = IDMSettings.syscall_settings;
  } else if (settings_id == DEVINTR_IDM_SETTINGS) {
    settings = IDMSettings.devintr_settings;
  } else {
    return 0;
  }

  acquire(&tickslock);
  ticks_now = ticks;  
  release(&tickslock);
  
  acquire(&settings.lock);
  result = settings.since <= ticks_now && ticks_now < settings.until;
  release(&settings.lock);

  return result;
}
