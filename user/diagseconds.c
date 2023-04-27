#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

void error(char * msg) {
	fprintf(2, msg);
	exit(1);
}

uint stoi(char * st) {
    return st[0] - '0';
}

int main(int argc, char *argv[]) {
	if (argc <= 1) {
		error("Invalid number of command arguments\n");
	}

	char * seconds_string = argv[1];
    uint seconds = stoi(seconds_string);

    int result = diagmode(DIAG_MODE_SECONDS, seconds);

    exit(result);
}
