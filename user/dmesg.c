#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main()
{
  for(uint i = 0; i < 5; i++){
    dmesgsend("1234567\n");
    sleep(1);
  }

  dmesg();

  exit(0);
}
