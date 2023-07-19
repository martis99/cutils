#ifndef hash_map_h
#define hash_map_h

#include "type.h"

struct bucket {
	struct bucket *next;

	void *key;
	size_t ksize;
	u32 hash;
	void *value;
};

typedef struct hashmap_s {
	struct bucket *buckets;
	int capacity;
	int count;

	struct bucket *first;
	struct bucket *last;
} hashmap_t;

typedef void (*hashmap_callback)(void *key, size_t ksize, void *value, void *priv);
typedef void (*hashmap_callback_c)(void *key, size_t ksize, void *value, const void *priv);
typedef void (*hashmap_callback_hc)(void *key, size_t ksize, void *value, void *priv);

hashmap_t *hashmap_init(hashmap_t *map, int capacity);
void hashmap_free(hashmap_t *map);

void hashmap_set(hashmap_t *map, void *key, size_t ksize, void *value);

int hashmap_get(const hashmap_t *map, void *key, size_t ksize, void **out_val);

#define hashmap_foreach(_hashmap, _bucket) for (struct bucket *_bucket = (_hashmap)->first; _bucket != NULL; _bucket = _bucket->next)

#endif
