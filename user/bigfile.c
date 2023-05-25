#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  if (argc < 3) {
    printf("bigfile: not enough arguments\n");
    
    exit(1);
  }

  int sz = atoi(argv[2]);
  int fd = mkfile(argv[1], sz);

  if (fd < 0) {
    printf("bigfile: cannot create file '%s'\n", argv[1]);

    exit(1);
  }

  close(fd);
  exit(0);
}
