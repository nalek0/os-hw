#include "kernel/types.h"
#include "user/user.h"

void parent(int child_pid) {
	int waited_child_pid = wait((int *) 0);

	if (waited_child_pid < 0) {
		// There are no children error
		fprintf(2, "Wait error: %d\n", waited_child_pid);
		exit(1);
	}
	
	printf("Children %d ended \n", waited_child_pid);	

	char buf[8];

	int read_bytes = read(0, buf, sizeof buf);
	
	if (read_bytes <= 0) {
		// Read error
		fprintf(2, "Read error\n");
		exit(1);
	} else {
		if (write(1, buf, read_bytes) != read_bytes) {
			// Write error
			fprintf(2, "Write error\n");
			exit(1);
		}
	}

	exit(0);
}

void child() {
	printf("child: exiting\n");
	exit(0);
}

int main() {
	int child_pid = fork();

	if (child_pid > 0) {
		parent(child_pid);
	} else if (child_pid == 0) {
		child();
	} else {
		// Fork error
		fprintf(2, "Fork error\n");
	}

	exit(1);
}