#ifndef HASHMAP_H
#define HASHMAP_H

#define HASHMAP_INIT_CAP 100

typedef struct hashmap Hashmap;

Hashmap *HASHMAP_create();
Hashmap *HASHMAP_put(Hashmap *hm, char *key, void *value);
void    *HASHMAP_get(Hashmap *hm, char *key);
void     HASHMAP_iterate(Hashmap *hm, void (*iter_func) (char *key, void *value));

#endif // HASHMAP_H
