#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main()
{
  for(uint i = 0; i < 5; i++){
    dmesgsend(" msg\n");
    sleep(1);
  }

  printf("Buffer: {\n");
  dmesg();
  printf("}\n");

  exit(0);
}
