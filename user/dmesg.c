#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

char*
safestrcpy(char *s, const char *t, int n)
{
  char *os;

  os = s;
  if(n <= 0)
    return os;
  while(--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  return os;
}

int
main()
{
  for(uint i = 0; i < 5; i++){
    dmesgsend("12345\n");
    sleep(1);
  }

  char buf[DMBSIZE];

  dmesg(buf);

  printf("%s", buf);

  exit(0);
}
