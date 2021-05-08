// MENU - ZIMMER - 2020

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include "hashmap.h"

DIR *open_directory(char *path);
void print_names(char *key, void *value);

int fd[2];

int main(int argc, char **argv) {

	if (argc < 2) {
		printf("usage: %s dir1, dir2, ...\n", argv[0]);
		return 1;
	}

	Hashmap *map = HASHMAP_create();

	for (int i = 1; i < argc; i++) {
		struct dirent *dir;
		DIR *d = open_directory(argv[i]);

		int path_len = strlen(argv[i]);
		char *full_path = malloc(path_len + 256);
		strcpy(full_path, argv[i]);
		full_path[path_len++] = '/';

		while((dir = readdir(d)) != NULL) {
			if (dir->d_name[0] == '.' || dir->d_type == DT_DIR) {
				continue;
			}

			char *dot = strrchr(dir->d_name, '.');
			if (!dot || strcmp(dot, ".desktop")) {
				continue;
			}

			strcpy(&full_path[path_len], dir->d_name);
			FILE *f = fopen(full_path, "r");

			// SCAN FILE
			char name_str[] = "\nName=";
			char exec_str[] = "\nExec=";

			int name_idx = 0;
			int exec_idx = 0;

			int name_found = 0;
			int exec_found = 0;

			char name_buff[512];
			char exec_buff[512];

			int c;
			while ( (c = fgetc(f)) != EOF) {

				if (!name_found) {
					if (c == name_str[name_idx]) {
						name_idx++;
					} else if (c == name_str[0]) {
						name_idx = 1;
					} else {
						name_idx = 0;
					}

					if (name_idx == sizeof(name_str) - 1) {
						name_found = 1;
						name_idx = 0;
						c = fgetc(f);
						while (c != '\n') {
							if (c == EOF) {
								name_buff[name_idx++] = '\0';
								break;
							}
							name_buff[name_idx++] = c;
							c = fgetc(f);
						}
						name_buff[name_idx++] = '\0';
						if (!exec_found) {
							exec_idx=1;
						}
					}
				}

				if (!exec_found) {
					if (c == exec_str[exec_idx]) {
						exec_idx++;
					} else if (c == exec_str[0]) {
						exec_idx = 1;
					} else {
						exec_idx = 0;
					}

					if (exec_idx == sizeof(exec_str) - 1) {
						exec_found = 1;
						exec_idx = 0;
						c = fgetc(f);
						while (c != '\n') {
							if (c == EOF) {
								exec_buff[exec_idx++] = '\0';
								break;
							}
							exec_buff[exec_idx++] = c;
							c = fgetc(f);
						}
						exec_buff[exec_idx++] = '\0';
						if (!name_found) {
							name_idx=1;
						}
					}
				}

				if (name_found && exec_found) {
					break;
				}
			}

			char *exec = malloc(exec_idx);
			strcpy(exec, exec_buff);

			map = HASHMAP_put(map, name_buff, exec);

		}

		closedir(d);
	}

	// FORK DMENU
	int fd2[2];

	pipe(fd);
	pipe(fd2);

	if (!fork()) {

		dup2(fd[0], 0);
		close(fd[0]);
		close(fd[1]);

		dup2(fd2[1], 1);
		close(fd2[0]);
		close(fd2[1]);

		char *args[] = {"dmenu", "-i", NULL};
		execvp("dmenu", args);
	}

	close(fd2[1]);
	close(fd[0]);

	HASHMAP_iterate(map, print_names);

	close(fd[1]);

	char buff[256];
	ssize_t len = read(fd2[0], &buff, 256);
	buff[len-1] = '\0';

	while (wait(NULL) > 0);

	char *exec_string = HASHMAP_get(map, buff);

	char *args[64];
	int args_idx = 0;

	char *arg = strtok(exec_string, " ");
	do {
		if (
			strcmp(arg, "%u") &&
			strcmp(arg, "%U") &&
			strcmp(arg, "%f") &&
			strcmp(arg, "%F")
		) {
			args[args_idx++] = arg;
		}
		arg = strtok(NULL, " ");
	} while (arg != NULL);
	
	args[args_idx++] = NULL;

	execvp(args[0], args);

	return 0;
}


void print_names(char *key, void *value) {
	dprintf(fd[1], "%s\n", key);
}

DIR *open_directory(char *path) {
	DIR *d = opendir(path);
	
	if (!d) {
		
		fprintf(stderr,
			"ERROR! Cannot open directory: %s.\n", path
		);

		switch(errno) {

		case EACCES:
			fprintf(stderr,
				"Permission denied.\n"
			);
			break;

		case ENFILE:
			fprintf(stderr,
				"The  system-wide limit on the total "
				"number of open files has been reached.\n"
			);
			break;

		case ENOENT:
			fprintf(stderr,
				"Directory does not exist, "
				"or path is  an  empty string.\n"
			);
			break;

		case ENOMEM:
			fprintf(stderr,
				"ERROR: Insufficient memory to "
				"complete the operation.\n"
			);
			break;

		case ENOTDIR:
			fprintf(stderr,
				"Path is not a directory.\n"
			);
			break;
		}

		exit(errno);
	}
	
	return d;
}
