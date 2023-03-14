#ifndef MEM_H
#define MEM_H

#include <stddef.h>

typedef struct m_stats_s {
	size_t mem;
	size_t mem_max;
	unsigned int allocs;
	unsigned int reallocs;
} m_stats_t;

void m_init(m_stats_t *stats);
const m_stats_t *m_get_stats();

void *m_malloc(size_t size);
void *m_calloc(size_t count, size_t size);
void *m_realloc(void *memory, size_t new_size, size_t old_size);

void *m_memset(void *dst, int val, size_t size);
void *m_memcpy(void *dst, size_t dst_size, const void *src, size_t src_size);
int m_memcmp(const void *l, const void *r, size_t size);

void m_free(void *memory, size_t size);

#endif
