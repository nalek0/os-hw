#include <stdarg.h>

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "stat.h"
#include "proc.h"

struct {
    // buffer
    char buffer[DMBSIZE];

    uint cursor_start;
    uint cursor_end;

    uint mode;
    uint64 since;
    uint64 until;

    // buffer lock
    struct spinlock lock;
} dmBuffer;

// inits diagnostics messages buffer
void initDMBuffer() {
    printf("init: dmbuffer\n");

    initlock(&dmBuffer.lock, "dmbuffer");

    dmBuffer.mode = DIAG_MODE_OFF;
}

static void setChar(uint ind, char ch) {
  if (ind < 0 || ind >= DMBSIZE) {
    panic("Invalid set index");
  }

  dmBuffer.buffer[ind] = ch;
}

static void setNextChar(char ch) {
  uint index = dmBuffer.cursor_end;
  dmBuffer.cursor_end = (dmBuffer.cursor_end + 1) % DMBSIZE;

  if (dmBuffer.cursor_end == dmBuffer.cursor_start) {
    dmBuffer.cursor_start = (dmBuffer.cursor_start + 1) % DMBSIZE;
  }

  setChar(index, ch);
}

static char digits[] = "0123456789abcdef";

// copy-paste of the method printptr(...) in the kernel/printf.c
static void setNextPtr(uint64 x) {
  setNextChar('0');
  setNextChar('x');

  for (int i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4) {
    setNextChar(digits[x >> (sizeof(uint64) * 8 - 4)]);
  }
}

// copy-paste of the method printint(...) in the kernel/printf.c
static void setNextInt(int xx, int base, int sign) {
  char buf[16];
  int i;
  uint x;

  if(sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    setNextChar(buf[i]);
}

// puts message in the dm buffer
void pr_msg(const char *fmt, ...)
{
  uint current_time;
  
  acquire(&tickslock);
  current_time = ticks;
  release(&tickslock);
  
  if (current_time > dmBuffer.until || current_time < dmBuffer.since) {
    return;
  }

  // lock:
  acquire(&dmBuffer.lock);

  // Printing current ticks
  setNextChar('[');

  uint xticks;
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  setNextInt(xticks, 10, 1);

  setNextChar(']');

  // Put fmt message:
  va_list ap;

  if (fmt == 0) {
    panic("null fmt");
  }

  va_start(ap, fmt);

  char c;
  char * s;

  for (int i = 0; (c = fmt[i] & 0xff) != 0; i++) {
    if (c != '%'){
      setNextChar(c);

      continue;
    }

    c = fmt[++i] & 0xff;
    
    if (c == 0) {
      break;
    }

    switch(c){
    case 'd':
      setNextInt(va_arg(ap, int), 10, 1);

      break;
    case 'x':
      setNextInt(va_arg(ap, int), 16, 1);

      break;
    case 'p':
      setNextPtr(va_arg(ap, uint64));

      break;
    case 's':
      if((s = va_arg(ap, char*)) == 0) {
        s = "(null)";
      }

      for(; *s; s++) {
        setNextChar(*s);
      }

      break;
    case '%':
      setNextChar('%');

      break;
    default:
      // Print unknown % sequence to draw attention.
      setNextChar('%');
      setNextChar(c);

      break;
    }
  }

  va_end(ap);

  // unlock:
  release(&dmBuffer.lock);
}

void cpybuf(uint64 addr) {
  acquire(&dmBuffer.lock);
  
  struct proc *p = myproc();
  
  if (dmBuffer.cursor_start <= dmBuffer.cursor_end) {
    char * from = dmBuffer.buffer + dmBuffer.cursor_start;
    uint64 size = sizeof(char) * (dmBuffer.cursor_end - dmBuffer.cursor_start);

    copyout(p->pagetable, addr, from, size);
  } else {
    char * from_first = dmBuffer.buffer + dmBuffer.cursor_start;
    uint64 size_first = sizeof(char) * (DMBSIZE - dmBuffer.cursor_start);
    copyout(p->pagetable, addr, from_first, size_first);

    char * from_second = dmBuffer.buffer + 0;
    uint64 size_second = sizeof(char) * (dmBuffer.cursor_end - 0);
    copyout(p->pagetable, addr + size_first, from_second, size_second);
  }

  release(&dmBuffer.lock);
}

int
update_diagmode(int mode, uint64 time) {
  if (mode == DIAG_MODE_ON) {
    acquire(&dmBuffer.lock);

    dmBuffer.mode = DIAG_MODE_ON;
    dmBuffer.since = 0;
    dmBuffer.until = INF;

    release(&dmBuffer.lock);

    return 0;
  } else if (mode == DIAG_MODE_OFF) {
    acquire(&dmBuffer.lock);

    dmBuffer.mode = DIAG_MODE_OFF;
    dmBuffer.since = INF;
    dmBuffer.until = INF;

    release(&dmBuffer.lock);

    return 0;
  } else if (mode == DIAG_MODE_SECONDS) {
    acquire(&dmBuffer.lock);

    dmBuffer.mode = DIAG_MODE_SECONDS;
    acquire(&tickslock);
    dmBuffer.since = ticks;  
    dmBuffer.until = ticks + time;  
    release(&tickslock);

    release(&dmBuffer.lock);

    return 0;
  }

  return 1;
}