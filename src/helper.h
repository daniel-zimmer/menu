#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>

int  HELPER_readNextLine(char *line, char (*readChar) (void *where), void *where, uint32_t *len);
void HELPER_call(char *args[], int *in, int *out);

char HELPER_readChar(void *where);
char HELPER_fgetChar(void *where);

#endif // HELPER_H
