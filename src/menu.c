// MENU - ZIMMER - 2021

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/wait.h>

#include "hashmap/hashmap.h"

#define PATH_BUFF_SIZE 512

void parseDesktopFile(char *path);
void print_names(char *key, void *value);
void call(int fd[2], char *args[]);

int main(int argc, char **argv) {

	Hashmap *hm = HASHMAP_create();

	int fd[2];
	char *args[] = {"mlocate", "*.desktop", NULL};
	call(fd, args);
	close(fd[1]);

	char path[PATH_BUFF_SIZE];
	int  pathIdx = 0;

	char buff[1];
	ssize_t len = read(fd[0], &buff, 1);
	while (len) {

		if (*buff == '\n'){

			path[pathIdx] = '\0';
			pathIdx = 0;
			parseDesktopFile(path);

		} else {
			path[pathIdx++] = *buff;
		}

		len = read(fd[0], &buff, 1);
	}

	while (wait(NULL) > 0);

}

void parseDesktopFile(char *path) {
	printf("------------------\n%s\n------------------------\n", path);

	FILE *f = fopen(path, "r");

	char c = 0;
	while ((c = fgetc(f)) != EOF) {
		putchar(c);
	}
}

void call(int fd[2], char *args[]) {
	pipe(fd);

	if (!fork()) {
		dup2(fd[1], 1);
		close(fd[0]); close(fd[1]);

		execvp(args[0], args);
	}

}

void print_names(char *key, void *value) {
	//dprintf(fd[1], "%s\n", key);
}
