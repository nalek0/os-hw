#include "kernel/types.h"
#include "user/user.h"

#define BUFFER_SIZE 32

#define PIPE_READ(p) (p[0])
#define PIPE_WRITE(p) (p[1])

void error(char * msg) {
	fprintf(2, msg);
	exit(1);
}

int main(int argc, char *argv[]) {
	if (argc <= 1) {
		error("Invalid number of command arguments\n");
	}

	char * message = argv[1];

	int from_par[2];
	int from_ch[2];
	
	if (pipe(from_par) != 0) {
		error("Pipe error.");
	}

	if (pipe(from_ch) != 0) {
		close(PIPE_READ(from_par));
		close(PIPE_WRITE(from_par));

		error("Pipe error.");
	}

	int fork_pid = fork();

	if (fork_pid > 0) {
		// Parent
		close(PIPE_READ(from_par));
		close(PIPE_WRITE(from_ch));

		// Sending message byte by byte to the children process
		for (char * symb = message; *symb; symb++) {
			if (write(PIPE_WRITE(from_par), symb, 1) != 1) {
				close(PIPE_WRITE(from_par));
				close(PIPE_READ(from_ch));
				
				error("Write error.");
			}
		}

		close(PIPE_WRITE(from_par));

		// Read message byte by byte from the children process
		char buf[1];

		for (;;) {
			int read_bytes = read(PIPE_READ(from_ch), buf, 1);

			if (read_bytes == 0) {
				break;
			}

			printf("<%d>: received <%s>\n", getpid(), buf);
		}

		close(PIPE_READ(from_ch));

		exit(0);
	} else if (fork_pid == 0) {
		// Children
		close(PIPE_WRITE(from_par));
		close(PIPE_READ(from_ch));

		char buf[1];

		for (;;) {
			int read_bytes = read(PIPE_READ(from_par), buf, 1);

			if (read_bytes == 0) {
				break;
			}

			printf("<%d>: received <%s>\n", getpid(), buf);
			
			if (write(PIPE_WRITE(from_ch), buf, 1) != 1) {
				close(PIPE_READ(from_par));
				close(PIPE_WRITE(from_ch));

				error("Write error.");
			}
		}

		close(PIPE_READ(from_par));
		close(PIPE_WRITE(from_ch));

		exit(0);
	} else {
		close(PIPE_READ(from_par));
		close(PIPE_WRITE(from_par));
		close(PIPE_READ(from_ch));
		close(PIPE_WRITE(from_ch));

		error("Fork error");
	}

	exit(1);
}
