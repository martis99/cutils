#include "hashmap.h"

#include "mem.h"

#include <stdlib.h>
#include <string.h>

#define HASHMAP_MAX_LOAD      0.75f
#define HASHMAP_RESIZE_FACTOR 2

hashmap_t *hashmap_init(hashmap_t *map, int capacity)
{
	if (map == NULL) {
		return NULL;
	}

	map->buckets = m_calloc(capacity, sizeof(struct bucket));
	if (map->buckets == NULL) {
		return NULL;
	}

	map->capacity = capacity;
	map->count    = 0;
	map->first    = NULL;
	map->last     = (struct bucket *)&map->first;

	return map;
}

void hashmap_free(hashmap_t *map)
{
	if (map == NULL) {
		return;
	}

	m_free(map->buckets, map->capacity * sizeof(struct bucket));
}

static struct bucket *resize_entry(hashmap_t *map, struct bucket *old_entry)
{
	u32 index = old_entry->hash % map->capacity;
	for (;;) {
		struct bucket *entry = &map->buckets[index];

		if (entry->key == NULL) {
			*entry = *old_entry;
			return entry;
		}

		index = (index + 1) % map->capacity;
	}
}

static void hashmap_resize(hashmap_t *map)
{
	struct bucket *old_buckets = map->buckets;
	int old_capacity	   = map->capacity;

	map->capacity *= HASHMAP_RESIZE_FACTOR;
	map->buckets = m_calloc(map->capacity, sizeof(struct bucket));

	map->last = (struct bucket *)&map->first;

	do {
		map->last->next = resize_entry(map, map->last->next);
		map->last	= map->last->next;
	} while (map->last->next != NULL);

	m_free(old_buckets, old_capacity * sizeof(struct bucket));
}

#define HASHMAP_HASH_INIT 2166136261u

static inline u32 hash_data(const byte *data, size_t size)
{
	size_t nblocks = size / 8;
	u64 hash       = HASHMAP_HASH_INIT;
	for (size_t i = 0; i < nblocks; ++i) {
		hash ^= (u64)data[0] << 0 | (u64)data[1] << 8 | (u64)data[2] << 16 | (u64)data[3] << 24 | (u64)data[4] << 32 | (u64)data[5] << 40 | (u64)data[6] << 48 |
			(u64)data[7] << 56;
		hash *= 0xbf58476d1ce4e5b9;
		data += 8;
	}

	u64 last = size & 0xff;
	switch (size % 8) {
	case 7: last |= (u64)data[6] << 56;
	case 6: last |= (u64)data[5] << 48;
	case 5: last |= (u64)data[4] << 40;
	case 4: last |= (u64)data[3] << 32;
	case 3: last |= (u64)data[2] << 24;
	case 2: last |= (u64)data[1] << 16;
	case 1:
		last |= (u64)data[0] << 8;
		hash ^= last;
		hash *= 0xd6e8feb86659fd93;
	}

	return (u32)(hash ^ hash >> 32);
}

static struct bucket *find_entry(const hashmap_t *map, void *key, size_t ksize, u32 hash)
{
	u32 index = hash % map->capacity;

	for (;;) {
		struct bucket *entry = &map->buckets[index];

		if (entry->key == NULL || (entry->ksize == ksize && entry->hash == hash && m_memcmp(entry->key, key, ksize) == 0)) {
			return entry;
		}

		index = (index + 1) % map->capacity;
	}
}

void hashmap_set(hashmap_t *map, void *key, size_t ksize, void *val)
{
	if (map == NULL || key == NULL) {
		return;
	}

	if (map->count + 1 > HASHMAP_MAX_LOAD * map->capacity) {
		hashmap_resize(map);
	}

	u32 hash	     = hash_data(key, ksize);
	struct bucket *entry = find_entry(map, key, ksize, hash);
	if (entry->key == NULL) {
		map->last->next = entry;
		map->last	= entry;
		entry->next	= NULL;

		++map->count;

		entry->key   = key;
		entry->ksize = ksize;
		entry->hash  = hash;
	}
	entry->value = val;
}

int hashmap_get(const hashmap_t *map, void *key, size_t ksize, void **out_val)
{
	if (map == NULL || key == NULL) {
		return 1;
	}

	u32 hash	     = hash_data(key, ksize);
	struct bucket *entry = find_entry(map, key, ksize, hash);

	if (out_val != NULL) {
		*out_val = entry->value;
	}

	return entry->key == NULL;
}

void hashmap_iterate(hashmap_t *map, hashmap_callback callback, void *priv)
{
	if (map == NULL) {
		return;
	}

	struct bucket *current = map->first;

	int co = 0;

	while (current != NULL) {
		callback(current->key, current->ksize, current->value, priv);

		current = current->next;

		if (co > 1000) {
			break;
		}
		co++;
	}
}

void hashmap_iterate_c(const hashmap_t *map, hashmap_callback_c callback, const void *priv)
{
	if (map == NULL) {
		return;
	}

	struct bucket *current = map->first;

	int co = 0;

	while (current != NULL) {
		callback(current->key, current->ksize, current->value, priv);

		current = current->next;

		if (co > 1000) {
			break;
		}
		co++;
	}
}

void hashmap_iterate_hc(const hashmap_t *map, hashmap_callback_hc callback, void *priv)
{
	if (map == NULL) {
		return;
	}

	struct bucket *current = map->first;

	int co = 0;

	while (current != NULL) {
		callback(current->key, current->ksize, current->value, priv);

		current = current->next;

		if (co > 1000) {
			break;
		}
		co++;
	}
}
