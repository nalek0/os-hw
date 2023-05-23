#include "kernel/types.h"
#include "user/user.h"

int main() {
    uint64 result = hello(3, 4);
    printf("%d\n", result);
    exit(0);
}