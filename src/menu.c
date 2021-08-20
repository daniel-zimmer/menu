// MENU - ZIMMER - 2021

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/wait.h>

#include "hashmap/hashmap.h"
#include "cache.h"
#include "helper.h"

#define EXEC_ARGS_COUNT 64

char *runFromCache();
void execcpy(char *dest, char *src, uint32_t len);
void printNames(char *key, void *value);

int printNamesInFd;

int main(int argc, char **argv) {

	char *cachePath = CACHE_getPath();
	if(access(cachePath, F_OK) != 0) { // cache does not exist
		CACHE_build();
		wait(NULL);
	}

	char *exec = runFromCache();

	if (!fork()) {
		CACHE_build();
		wait(NULL);
		exit(0);
	}

	if (exec == NULL) {
		return 0;
	}

	char *execArgs[EXEC_ARGS_COUNT];
	uint8_t execArgsIdx = 0;

	for(; *exec == ' '; exec++);
	execArgs[execArgsIdx++] = exec;
	while (*exec) {
		if (*exec == ' ') {
			for(; *exec == ' '; exec++){ *exec = '\0'; }
			if (*exec == '\0') { break; }
			if (*exec == '"') {
				exec++; if (*exec == '\0') { break; }
				for(execArgs[execArgsIdx++] = exec; *exec != '"' && *exec != '\0'; exec++);
				if (*exec == '\0') { break; }
				*exec = '\0';
			}
			if (*exec == '\'') {
				exec++; if (*exec == '\0') { break; }
				for(execArgs[execArgsIdx++] = exec; *exec != '\'' && *exec != '\0'; exec++);
				if (*exec == '\0') { break; }
				*exec = '\0';
			}
			execArgs[execArgsIdx++] = exec;
		}

		exec++;
	}
	execArgs[execArgsIdx] = NULL;

	execvp(execArgs[0], execArgs);

	return 0;
}

char *runFromCache() {
	int in, out;

	Hashmap *hm = HASHMAP_create();

	hm = CACHE_read(hm);

	char *args1[] = {"dmenu", "-i", "-nb", "#000", NULL};
	HELPER_call(args1, &in, &out);

	printNamesInFd = in;
	HASHMAP_iterate(hm, printNames);

	close(in);

	int wstatus;
	wait(&wstatus);

	if (WIFEXITED(wstatus)) {
		if (WEXITSTATUS(wstatus) != 0) {
			return NULL;
		}
	} else {
		return NULL;
	}

	char buff[256];
	ssize_t len1 = read(out, &buff, 256);
	buff[len1-1] = '\0';

	close(out);

	char *exec = strdup(HASHMAP_get(hm, buff));

	HASHMAP_delete(hm, HASHMAP_freeDel);

	return exec;
}

void printNames(char *key, void *value) {
	dprintf(printNamesInFd, "%s\n", key);
}
