#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    char dmbuf[DMBSIZE];

    int result = dmesg(dmbuf);

    if (result != 0) {
        exit(result);
    }

    printf("Saved buff:\n>>>%s<<<\n", dmbuf);

    exit(0);
}
