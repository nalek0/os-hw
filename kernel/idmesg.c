#include <stdarg.h>

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "proc.h"

struct settings {
  uint mode;
  uint64 since;
  uint64 until;
};

struct {
  struct settings syscall_settings;
  struct settings devintr_settings;

  struct spinlock lock;
} IDMSettings; // interruption diagnostic messages

void
initIDMSettings() {
  printf("init: IDMSettings\n");

  initlock(&IDMSettings.lock, "IDMSettings");

  IDMSettings.syscall_settings.mode = DIAG_MODE_OFF;
  IDMSettings.devintr_settings.mode = DIAG_MODE_OFF;
}

void
update_settings(struct settings * settings, int mode, uint64 time) {
  if (mode == DIAG_MODE_ON) {
    settings->mode = DIAG_MODE_ON;
    settings->since = 0;
    settings->until = INF;
  } else if (mode == DIAG_MODE_OFF) {
    settings->mode = DIAG_MODE_OFF;
    settings->since = INF;
    settings->until = INF;
  } else if (mode == DIAG_MODE_SECONDS) {
    settings->mode = DIAG_MODE_SECONDS;

    acquire(&tickslock);
    settings->since = ticks;  
    settings->until = ticks + time;  
    release(&tickslock);
  }
}

int
update_diagmode(int settings_id, int mode, uint64 time) {
  if (settings_id == SYSCALL_IDM_SETTINGS) {
    update_settings(&IDMSettings.syscall_settings, mode, time);

    return 0;
  } else if (settings_id == DEVINTR_IDM_SETTINGS) {
    update_settings(&IDMSettings.devintr_settings, mode, time);

    return 0;
  } else {
    return 1;
  }
}

int
accept(struct settings * settings) {
  int ticks_now;
  acquire(&tickslock);
  ticks_now = ticks;  
  release(&tickslock);
  
  return settings->since <= ticks_now && ticks_now < settings -> until;
}

void send_syscall_idm(const char * fmt, const char * syscall_name, const struct proc * p) {
  if (accept(&IDMSettings.syscall_settings)) {
    pr_msg("syscall `%s` interruption. process: '%s'(id=%d)\n", syscall_name, p->name, p->pid);
  }
}

void send_devintr_idm(const char * fmt, int irq) {
  if (accept(&IDMSettings.devintr_settings)) {
    pr_msg("devintr(): device(irq=%d) interruption.\n", irq);
  }
}

void send_devintr_undef(const char * fmt) {
  if (accept(&IDMSettings.devintr_settings)) {
    pr_msg("devintr(): undefined device interruption.\n");
  }
}