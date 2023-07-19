#include "arr.h"

#include "mem.h"

#include <stdlib.h>

arr_t *arr_init(arr_t *arr, uint cap, size_t size)
{
	arr->data = m_malloc(cap * size);
	if (arr->data == NULL) {
		return NULL;
	}

	arr->cap  = cap;
	arr->cnt  = 0;
	arr->size = size;

	return arr;
}

void arr_free(arr_t *arr)
{
	m_free(arr->data, arr->cap * arr->size);
	arr->data = NULL;
	arr->cap  = 0;
	arr->cnt  = 0;
	arr->size = 0;
}

static int arr_resize(arr_t *arr)
{
	if (arr->cnt < arr->cap) {
		return 0;
	}

	const size_t node_size = arr->size;

	size_t old_size = arr->cap * node_size;
	arr->cap *= 2;
	arr->data = m_realloc(arr->data, arr->cap * node_size, old_size);
	if (arr->data == NULL) {
		return 1;
	}

	return 0;
}

uint arr_add(arr_t *arr)
{
	if (arr_resize(arr)) {
		return -1;
	}

	uint index = arr->cnt;
	arr->cnt++;

	return index;
}

void *arr_get(const arr_t *arr, uint index)
{
	if (index >= arr->cnt) {
		return NULL;
	}

	return (byte *)arr->data + index * arr->size;
}

void *arr_set(arr_t *arr, uint index, const void *value)
{
	if (value == NULL) {
		return NULL;
	}

	void *dst = arr_get(arr, index);
	if (dst == NULL) {
		return NULL;
	}

	return m_memcpy(dst, arr->size, value, arr->size);
}

uint arr_app(arr_t *arr, const void *value)
{
	uint index = arr_add(arr);
	arr_set(arr, index, value);
	return index;
}

uint arr_index(const arr_t *arr, const void *value)
{
	for (uint i = 0; i < arr->cnt; i++) {
		if (m_memcmp(arr_get(arr, i), value, arr->size) == 0) {
			return i;
		}
	}

	return -1;
}

uint arr_index_cmp(const arr_t *arr, const void *value, arr_index_cmp_cb cb)
{
	for (uint i = 0; i < arr->cnt; i++) {
		if (cb(arr_get(arr, i), value)) {
			return i;
		}
	}

	return -1;
}

arr_t *arr_add_all(arr_t *arr, const arr_t *src)
{
	if (arr->cap - arr->cnt < src->cnt || arr->size != src->size) {
		return NULL;
	}

	void *ret = m_memcpy((byte *)arr->data + arr->cnt * arr->size, arr->cap * arr->size - arr->cnt * arr->size, src->data, src->cnt * src->size);

	arr->cnt += src->cnt;

	return ret;
}

arr_t *arr_add_unique(arr_t *arr, const arr_t *src)
{
	if (arr->size != src->size) {
		return NULL;
	}

	for (uint i = 0; i < src->cnt; i++) {
		const void *value = arr_get(src, i);

		if (arr_index(arr, value) != -1) {
			continue;
		}

		arr_set(arr, arr_add(arr), value);
	}

	return arr;
}

arr_t *arr_merge_all(arr_t *arr, const arr_t *arr1, const arr_t *arr2)
{
	if (arr1->size != arr2->size) {
		return NULL;
	}

	if (arr_init(arr, arr1->cnt + arr2->cnt, arr1->size) == NULL) {
		return NULL;
	}

	if (arr_add_all(arr, arr1) == NULL) {
		return NULL;
	}

	if (arr_add_all(arr, arr2) == NULL) {
		return NULL;
	}

	return arr;
}

arr_t *arr_merge_unique(arr_t *arr, const arr_t *arr1, const arr_t *arr2)
{
	if (arr1->size != arr2->size) {
		return NULL;
	}

	if (arr_init(arr, arr1->cnt + arr2->cnt, arr1->size) == NULL) {
		return NULL;
	}

	if (arr_add_unique(arr, arr1) == NULL) {
		return NULL;
	}

	if (arr_add_unique(arr, arr2) == NULL) {
		return NULL;
	}

	return arr;
}

typedef struct arr_print_priv_s {
	FILE *file;
	arr_print_cb cb;
} arr_print_priv_t;

static int print_cb(const arr_t *arr, uint index, void *value, int ret, void *priv)
{
	arr_print_priv_t *p = priv;

	return p->cb(p->file, value, ret);
}

int arr_print(const arr_t *arr, FILE *file, arr_print_cb cb, int ret)
{
	void *value;
	arr_foreach(arr, value)
	{
		ret = cb(file, value, ret);
	}

	return ret;
}
