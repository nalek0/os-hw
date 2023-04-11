#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main()
{
  for(uint i = 0; i < 5; i++){
    dmesgsend("Message!");
    sleep(1);
  }

  dmesg();

  exit(0);
}
