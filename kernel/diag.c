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
  if (dmBuffer.cursor == DMBSIZE) {
    panic("Buffer overflow");
  }
  
  setChar(dmBuffer.cursor++, ch);
}

static char digits[] = "0123456789abcdef";

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

static void setNextString(const char * str) {
    for (uint i = 0; str[i]; i++) {
        setNextChar(str[i]);
    }
} 

// puts message in the dm buffer
void pr_msg(const char *str) {
    acquire(&dmBuffer.lock);

    setNextChar('[');

    // Printing current ticks
    uint xticks;
    acquire(&tickslock);
    xticks = ticks;
    release(&tickslock);
    setNextInt(xticks, 10, 1);

    setNextChar(']');
    setNextString(str);

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