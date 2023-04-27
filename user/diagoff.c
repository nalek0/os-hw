#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int result = diagmode(DIAG_MODE_OFF, 0);

    exit(result);
}
