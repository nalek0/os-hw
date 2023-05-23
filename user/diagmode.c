#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int stoi(char * string) {
    int result = 0;

    for (char * curr = string; *curr; curr++) {
        if ('0' < *curr && *curr < '9') {
            result = result * 10 + (*curr - '0');
        } else {
            exit(1);
        }
    }

    return result;
}

int main(int argc, char *argv[]) {
    int settings_id;
    int mode;
    int arg;

    if (argc == 3) {
        settings_id = stoi(argv[1]);
        mode        = stoi(argv[2]);
        arg         = 0;
    } else if (argc == 4) {
        settings_id = stoi(argv[1]);
        mode        = stoi(argv[2]);
        arg         = stoi(argv[3]);
    } else {
        exit(1);
    }

    if (settings_id == SYSCALL_IDM_SETTINGS) {
        printf("Editing SYSCALL_IDM_SETTINGS settings\n");
    } else if (settings_id == DEVINTR_IDM_SETTINGS) {
        printf("Editing DEVINTR_IDM_SETTINGS settings\n");
    } else if (settings_id == CONTEXT_IDM_SETTINGS) {
        printf("Editing CONTEXT_IDM_SETTINGS settings\n");
    } else {
        printf("Editing undefined settings\n");
    }

    if (mode == DIAG_MODE_ON) {
        printf("Setting mode: DIAG_MODE_ON\n");
    } else if (mode == DIAG_MODE_OFF) {
        printf("Setting mode: DIAG_MODE_OFF\n");
    } else if (mode == DIAG_MODE_SECONDS) {
        printf("Setting mode: DIAG_MODE_SECONDS\n");
    } else {
        printf("Undefined diagmode provided\n");
    }

    exit(diagmode(settings_id, mode, arg));
}
