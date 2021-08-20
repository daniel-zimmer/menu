#include "helper.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int HELPER_readNextLine(char *line, char (*readChar) (void *where), void *where, uint32_t *len) {
	uint32_t lineIdx = 0;

	char c;
	while ( (c = readChar(where)) ) {

		if (c != '\n' && c != '\r'){
			line[lineIdx++] = c;
		} else {

			line[lineIdx] = '\0';
			if (len) { *len = lineIdx; };
			return 1;
		}

	}

	line[lineIdx] = '\0';
	if (len) { *len = lineIdx; };
	return 0;
}

void HELPER_call(char *args[], int *in, int *out) {
	int fd1[2], fd2[2];

	int pipe1 = pipe(fd1), pipe2 = pipe(fd2);
	if (pipe1 || pipe2) {
		perror("Error creating pipe!");
		exit(10);
	}

	if (!fork()) {
		dup2(fd1[0], 0);
		close(fd1[0]); close(fd1[1]);

		dup2(fd2[1], 1);
		close(fd2[0]); close(fd2[1]);

		execvp(args[0], args);
	}

	close(fd1[0]); close(fd2[1]);

	*in = fd1[1]; *out = fd2[0];
}

char HELPER_readChar(void *where) {
	char buff[1];
	ssize_t len = read((int) (long) where, &buff, 1);
	return (len) ? *buff : '\0';
}

char HELPER_fgetChar(void *where) {
	char c = fgetc((FILE *) where);
	return (c == EOF) ? '\0' : c;
}

