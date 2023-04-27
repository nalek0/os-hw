#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int result = diagmode(DIAG_MODE_ON, 0);

    printf("Set mode: DIAG_MODE_ON\n");

    exit(result);
}
