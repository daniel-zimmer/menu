#include "hashmap.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

struct hashmap {
	uint32_t size;
	uint32_t cap;
	struct bucket {
		char     *key;
		uint64_t  hash;
		void     *value;
	} bucket[];
};

static uint64_t hash(char *key) {
	uint64_t h_val = 0;
	while (*key != '\0') {
		h_val = (h_val << 5) - h_val + *(key++);
	}
	return h_val;
}

static Hashmap *create(int cap) {
	Hashmap *hm = malloc(
		sizeof(struct hashmap) +
		sizeof(struct bucket) * cap
	);

	hm->size = 0;
	hm->cap = cap;

	for (int i = 0; i < hm->cap; i++) {
		hm->bucket[i].key  = NULL;
	}

	return hm;
}

Hashmap *HASHMAP_create() {
	return create(HASHMAP_INIT_CAP);
}

static Hashmap *resize(Hashmap *old) {
	Hashmap *new = create(old->cap<<1);
	new->size = old->size;

	for (int i = 0; i < old->cap; i++) {
		uint64_t index = old->bucket[i].hash%new->cap;
		while (new->bucket[index].key != NULL) {
			index = (index + 1)%new->cap;
		}

		new->bucket[index].key   = old->bucket[i].key;
		new->bucket[index].hash  = old->bucket[i].hash;
		new->bucket[index].value = old->bucket[i].value;
	}

	free(old);
	return new;
}

Hashmap *HASHMAP_put(Hashmap *hm, char *key, void *value) {
	uint64_t hash_val = hash(key);

	if (hm->size > hm->cap>>1) {
		hm = resize(hm);
	}

	uint64_t index = hash_val%hm->cap;
	while (hm->bucket[index].key != NULL) {
		if (
			hm->bucket[index].hash == hash_val &&
			!strcmp(hm->bucket[index].key, key)
		) {
			break;
		}
		index = (index + 1)%hm->cap;
	}

	hm->bucket[index].key   = strdup(key);
	hm->bucket[index].hash  = hash_val;
	hm->bucket[index].value = value;
	hm->size++;

	return hm;
}

void *HASHMAP_get(Hashmap *hm, char *key) {
	uint64_t hash_val = hash(key);

	uint64_t index = hash_val%hm->cap;
	while (hm->bucket[index].key != NULL) {
		if (
			hm->bucket[index].hash == hash_val &&
			!strcmp(hm->bucket[index].key, key)
		) {
			return hm->bucket[index].value;
		}

		index = (index + 1) % hm->cap;
	}

	return NULL;
}

void HASHMAP_iterate(Hashmap *hm, void (*iter_func) (char *key, void *value)) {
	for (int i = 0; i < hm->cap; i++) {
		if (hm->bucket[i].key != NULL) {
			iter_func(hm->bucket[i].key, hm->bucket[i].value);
		}
	}
}

