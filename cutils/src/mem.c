#include "mem.h"

#include "platform.h"

#include <memory.h>
#include <stdlib.h>

static m_stats_t *s_stats;

void m_init(m_stats_t *stats)
{
	s_stats = stats;
}

const m_stats_t *m_get_stats()
{
	return s_stats;
}

#define MAX(a, b) (a) > (b) ? (a) : (b)

void *m_malloc(size_t size)
{
	s_stats->mem += size;
	s_stats->max_mem = MAX(s_stats->mem, s_stats->max_mem);
	s_stats->allocs++;

	void *ptr = malloc(size);
	if (ptr == NULL) {
		return NULL;
	}

	return memset(ptr, 0, size);
}

void *m_calloc(size_t count, size_t size)
{
	s_stats->mem += count * size;
	s_stats->max_mem = MAX(s_stats->mem, s_stats->max_mem);
	s_stats->allocs++;

	return calloc(count, size);
}

void *m_realloc(void *memory, size_t new_size, size_t old_size)
{
	s_stats->mem -= old_size;
	s_stats->mem += new_size;
	s_stats->max_mem = MAX(s_stats->mem, s_stats->max_mem);
	s_stats->reallocs++;

	return realloc(memory, new_size);
}

void *m_memset(void *dst, int val, size_t size)
{
	return memset(dst, val, size);
}

void *m_memcpy(void *dst, size_t dst_size, const void *src, size_t src_size)
{
#if defined(P_WIN)
	memcpy_s(dst, dst_size, src, src_size);
#else
	memcpy(dst, src, src_size);
#endif
	return dst;
}

int m_memcmp(const void *l, const void *r, size_t size)
{
	return memcmp(l, r, size);
}

void m_free(void *memory, size_t size)
{
	s_stats->mem -= size;
	free(memory);
}
