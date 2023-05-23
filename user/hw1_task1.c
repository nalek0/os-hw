#include "kernel/types.h"
#include "user/user.h"

#define BUFFER_SIZE 32

void error(char * msg) {
	fprintf(2, msg);
	exit(1);
}

void read_stdin(char * buf) {
	int read_bytes = read(0, buf, BUFFER_SIZE * sizeof(char));
	
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

		// Closing pipe
		close(p[0]);
		close(p[1]);

		wait((int *) 0);

		exit(0);
	} else {
		// Set p[0] as input
		close(0);
		dup(p[0]);

		// Close pipe
		close(p[0]);
		close(p[1]);
		
		// Exec wc
		char * argv[2] = { "wc", 0 };

		exec("wc", argv);
	}

	exit(1);
}
