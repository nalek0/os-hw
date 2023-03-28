#include "kernel/types.h"
#include "user/user.h"

#define BUFFER_SIZE 32

void error(char * msg) {
	fprintf(2, msg);
	exit(1);
}

void read_stdin(char * buf) {
	int read_bytes = read(0, buf, sizeof buf);
	
	if (read_bytes <= 0) {
		error("Read error\n");
	}
}

int main() {
	// Init pipe
	int p[2];
	pipe(p);

	// Run different methods depending in what process we are 
	if (fork() > 0) {
		// Read stdin:
		char input[BUFFER_SIZE];
		read_stdin(input);

		// Send input:
		fprintf(p[1], input);

		close(p[0]);
		close(p[1]);

		exit(0);
	} else {
		char * argv[2];
		argv[0] = "wc";
		argv[1] = 0;

		close(0);
		dup(p[0]);
		close(p[0]);
		close(p[1]);
		
		exec("wc", argv);
	}

	exit(1);
}