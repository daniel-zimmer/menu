#ifndef CACHE_H
#define CACHE_H

#include "hashmap/hashmap.h"

char    *CACHE_getPath();
Hashmap *CACHE_read(Hashmap *hm);
void     CACHE_build();

#endif // CACHE_H