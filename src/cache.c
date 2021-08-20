#include "cache.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "hashmap/hashmap.h"
#include "helper.h"

#define CACHE_PATH_BUFF_SIZE 256
#define CACHE_LINE_BUFF_SIZE 2048

#define PATH_BUFF_SIZE 512
#define LINE_BUFF_SIZE 4096
#define NAME_BUFF_SIZE 64
#define EXEC_BUFF_SIZE 1024

#define NAME_STR "Name="
#define EXEC_STR "Exec="

static void execcpy(char *dest, char *src, uint32_t len);

char cachePath[CACHE_PATH_BUFF_SIZE] = {0};

char *CACHE_getPath() {
	if (!*cachePath) {
		strcpy(cachePath, getenv("HOME"));
		strcat(cachePath, "/.cache/menu/menu-cache.txt");
	}

	return cachePath;
}

Hashmap *CACHE_read(Hashmap *hm) {
	char *path = CACHE_getPath();

	FILE *cache = fopen(path, "r");

	char line[CACHE_LINE_BUFF_SIZE];
	while(HELPER_readNextLine(line, HELPER_fgetChar, cache, NULL)) {
		
		char *div = strchr(line, '\t');
		*div = '\0';

		char *name = line;
		char *exec = &div[1];

		hm = HASHMAP_put(hm, name, strdup(exec));
	}
	fclose(cache);

	return hm;
}

void CACHE_build() {
	char *cachePath = CACHE_getPath();

	int in, out;
	char *args[] = {"mlocate", "*/applications/*.desktop", NULL};
	HELPER_call(args, &in, &out);
	close(in);

	char name[NAME_BUFF_SIZE];
	char exec[EXEC_BUFF_SIZE];

	uint32_t len;
	char line[LINE_BUFF_SIZE];
	char path[PATH_BUFF_SIZE];

	FILE *cache = fopen(cachePath, "w+");

	while(HELPER_readNextLine(path, HELPER_readChar, out, NULL)) {
		FILE *f = fopen(path, "r");

		uint8_t nameFound = 0;
		uint8_t execFound = 0;
		while(HELPER_readNextLine(line, HELPER_fgetChar, f, &len)) {

			if (len <= (
				(sizeof(NAME_STR) > sizeof(EXEC_STR)) ?
					sizeof(NAME_STR)-1 : sizeof(EXEC_STR)-1
				)
			) { continue; }

			if (!nameFound && !strncmp(NAME_STR, line, sizeof(NAME_STR)-1)) {
				strcpy(name, &line[sizeof(NAME_STR)-1]);
				nameFound = 1;
			}

			if (!execFound && !strncmp(EXEC_STR, line, sizeof(EXEC_STR)-1)) {
				execcpy(exec, &line[sizeof(EXEC_STR)-1], len - (sizeof(EXEC_STR)-1));
				execFound = 1;
			}

			if (nameFound && execFound) {
				break;
			}
		}

		if (nameFound && execFound) {
			fprintf(cache, "%s\t%s\n", name, exec);
		}

		fclose(f);
	}

	close(out);
	fclose(cache);
}

static void execcpy(char *dest, char *src, uint32_t len) {

	if (len <= 3) {
		uint8_t i;
		for (i = 0; i < len; i++) {
			dest[i] = src[i];
		}
		dest[i] = '\0';

		return;
	}

	uint32_t i;
	for (i = 0; i < len-3; i++) {
		if (
			 src[i]   == ' ' &&
			 src[i+1] == '%' &&
			(src[i+2] == 'u' ||
			 src[i+2] == 'U' ||
			 src[i+2] == 'f' ||
			 src[i+2] == 'F' )
		) {
			for(i+=3; i < len; i++) {
				dest[i-3] = src[i];
			}
			dest[len] = '\0';
			return;
		}

		dest[i] = src[i];
	}

	if (
		 src[i]   == ' ' &&
		 src[i+1] == '%' &&
		(src[i+2] == 'u' ||
		 src[i+2] == 'U' ||
		 src[i+2] == 'f' ||
		 src[i+2] == 'F' )
	) {
		dest[i] = '\0';
		return;
	}

	dest[i++] = src[len-3];
	dest[i++] = src[len-2];
	dest[i++] = src[len-1];
	dest[i  ] = '\0';
}
