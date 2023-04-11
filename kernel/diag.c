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

    // cursor, that is references to the first unset buffer byte
    uint cursor;

    // buffer lock
    struct spinlock lock;
} dmBuffer;

// inits diagnostics messages buffer
void initDMBuffer() {
    printf("init: dmbuffer\n");

    initlock(&dmBuffer.lock, "dmbuffer");
}

static void setChar(uint ind, char ch) {
    dmBuffer.buffer[ind] = ch;
}

static void setNextChar(char ch) {
    setChar(dmBuffer.cursor++, ch);
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

void print_buff() {

    acquire(&dmBuffer.lock);

    for (int i = 0; i < dmBuffer.cursor; i++) {
        consputc(dmBuffer.buffer[i]);
    }

    consputc('\n');

    release(&dmBuffer.lock);
}