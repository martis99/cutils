#ifndef MEM_H
#define MEM_H

#include "type.h"

#include <stdio.h>

typedef struct mem_stats_s {
	size_t mem;
	size_t mem_max;
	uint allocs;
	uint reallocs;
} mem_stats_t;

void mem_init(mem_stats_t *stats);
const mem_stats_t *mem_get_stats();
void mem_print(FILE *file);

void *mem_alloc(size_t size);
void *mem_calloc(size_t count, size_t size);
void *mem_realloc(void *memory, size_t new_size, size_t old_size);

void *mem_set(void *dst, int val, size_t size);
void *mem_cpy(void *dst, size_t dst_size, const void *src, size_t src_size);
int mem_cmp(const void *l, const void *r, size_t size);

void mem_free(void *memory, size_t size);

void mem_oom(int oom);

#endif
