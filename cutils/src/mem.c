#include "mem.h"

#include "log.h"
#include "platform.h"
#include "print.h"

#include <memory.h>
#include <stdlib.h>

static mem_t *s_mem;
static int s_oom;

mem_t *mem_init(mem_t *mem)
{
	s_mem = mem;
	s_oom = 0;

	return mem;
}

const mem_t *mem_get()
{
	return s_mem;
}

int mem_print(FILE *file)
{
	if (file == NULL || s_mem == NULL) {
		return 0;
	}

	size_t mem_max = s_mem->mem_max;

	char s	= '\0';
	int ret = 0;

	if (mem_max > (size_t)1024 * 1024) {
		mem_max /= (size_t)1024 * 1024;
		s = 'M';
	} else if (mem_max > 1024) {
		mem_max /= 1024;
		s = 'K';
	}

	if (s == '\0') {
		ret += c_fprintf(file, "mem:      %zu max: %zu B\n", s_mem->mem, s_mem->mem_max);
	} else {
		ret += c_fprintf(file, "mem:      %zu max: %zu %cB (%zu B)\n", s_mem->mem, mem_max, s, s_mem->mem_max);
	}
	ret += c_fprintf(file, "allocs:   %d\n", s_mem->allocs);
	ret += c_fprintf(file, "reallocs: %d\n", s_mem->reallocs);

	c_fflush(file);

	return ret;
}

#define MAX(a, b) (a) > (b) ? (a) : (b)

void *mem_alloc(size_t size)
{
	if (size == 0) {
		log_warn("cutils", "mem", NULL, "malloc 0 bytes");
	}

	void *ptr = size > 0 && s_oom ? NULL : malloc(size);

	if (ptr == NULL) {
		log_error("cutils", "mem", NULL, "out of memory");
		return NULL;
	}

	if (s_mem) {
		s_mem->mem += size;
		s_mem->mem_max = MAX(s_mem->mem, s_mem->mem_max);
		s_mem->allocs++;
	}

	return ptr;
}

void *mem_calloc(size_t count, size_t size)
{
	if (size == 0) {
		log_warn("cutils", "mem", NULL, "calloc 0 bytes");
	}

	void *ptr = count * size > 0 && s_oom ? NULL : calloc(count, size);

	if (ptr == NULL) {
		log_error("cutils", "mem", NULL, "out of memory");
		return NULL;
	}

	if (s_mem) {
		s_mem->mem += count * size;
		s_mem->mem_max = MAX(s_mem->mem, s_mem->mem_max);
		s_mem->allocs++;
	}

	return ptr;
}

void *mem_realloc(void *memory, size_t new_size, size_t old_size)
{
	if (memory == NULL) {
		log_error("cutils", "mem", NULL, "realloc NULL");
		return NULL;
	}

	if (new_size == 0 || new_size == old_size) {
		log_warn("cutils", "mem", NULL, "realloc %zu -> %zu bytes", old_size, new_size);
	} else {
		log_trace("cutils", "mem", NULL, "realloc %zu -> %zu bytes", old_size, new_size);
	}

	if (new_size == 0) {
		return memory;
	}

	if (old_size == 0) {
		return mem_alloc(new_size);
	}

	void *ptr = new_size > old_size && s_oom ? NULL : realloc(memory, new_size);

	if (ptr == NULL) {
		log_error("cutils", "mem", NULL, "out of memory");
		return NULL;
	}

	if (s_mem) {
		s_mem->mem -= old_size;
		s_mem->mem += new_size;
		s_mem->mem_max = MAX(s_mem->mem, s_mem->mem_max);
		s_mem->reallocs++;
	}

	return ptr;
}

void *mem_set(void *dst, int val, size_t size)
{
	if (dst == NULL) {
		return NULL;
	}

	return memset(dst, val, size);
}

void *mem_cpy(void *dst, size_t dst_size, const void *src, size_t src_size)
{
#if defined(C_WIN)
	memcpy_s(dst, dst_size, src, src_size);
#else
	memcpy(dst, src, src_size);
#endif
	return dst;
}

int mem_cmp(const void *l, const void *r, size_t size)
{
	return memcmp(l, r, size);
}

int mem_swap(void *ptr1, void *ptr2, size_t size)
{
	if (ptr1 == NULL || ptr2 == NULL) {
		return 1;
	}

	unsigned char *byte_ptr1 = (unsigned char *)ptr1;
	unsigned char *byte_ptr2 = (unsigned char *)ptr2;

	for (size_t i = 0; i < size; i++) {
		unsigned char temp = byte_ptr1[i];
		byte_ptr1[i]	   = byte_ptr2[i];
		byte_ptr2[i]	   = temp;
	}

	return 0;
}

void mem_free(void *memory, size_t size)
{
	if (memory == NULL) {
		return;
	}

	if (s_mem) {
		s_mem->mem -= size;
	}

	free(memory);
}

void mem_oom(int oom)
{
	s_oom = oom;
}
