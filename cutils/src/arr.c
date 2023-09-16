#include "arr.h"

#include "mem.h"

#include <stdlib.h>

arr_t *arr_init(arr_t *arr, uint cap, size_t size)
{
	if (arr == NULL || cap == 0) {
		return NULL;
	}

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
	if (arr == NULL) {
		return;
	}

	m_free(arr->data, arr->cap * arr->size);
	arr->data = NULL;
	arr->cap  = 0;
	arr->cnt  = 0;
	arr->size = 0;
}

static inline int arr_resize(arr_t *arr)
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
	if (arr == NULL) {
		return ARR_END;
	}

	if (arr_resize(arr)) {
		return ARR_END;
	}

	uint index = arr->cnt;
	arr->cnt++;

	return index;
}

void *arr_get(const arr_t *arr, uint index)
{
	if (arr == NULL || index >= arr->cnt) {
		return NULL;
	}

	return (byte *)arr->data + index * arr->size;
}

void *arr_set(arr_t *arr, uint index, const void *value)
{
	if (arr == NULL || value == NULL) {
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

	if (arr_set(arr, index, value) == NULL) {
		return ARR_END;
	}

	return index;
}

uint arr_index(const arr_t *arr, const void *value)
{
	if (arr == NULL || value == NULL) {
		return ARR_END;
		;
	}

	for (uint i = 0; i < arr->cnt; i++) {
		if (m_memcmp(arr_get(arr, i), value, arr->size) == 0) {
			return i;
		}
	}

	return ARR_END;
}

uint arr_index_cmp(const arr_t *arr, const void *value, arr_index_cmp_cb cb)
{
	if (arr == NULL || value == NULL) {
		return ARR_END;
	}

	for (uint i = 0; i < arr->cnt; i++) {
		if (cb(arr_get(arr, i), value)) {
			return i;
		}
	}

	return ARR_END;
}

arr_t *arr_add_all(arr_t *arr, const arr_t *src)
{
	if (arr == NULL || src == NULL || arr->cap - arr->cnt < src->cnt || arr->size != src->size) {
		return NULL;
	}

	void *ret = m_memcpy((byte *)arr->data + arr->cnt * arr->size, arr->cap * arr->size - arr->cnt * arr->size, src->data, src->cnt * src->size);

	arr->cnt += src->cnt;

	return ret;
}

arr_t *arr_add_unique(arr_t *arr, const arr_t *src)
{
	if (arr == NULL || src == NULL || arr->size != src->size) {
		return NULL;
	}

	for (uint i = 0; i < src->cnt; i++) {
		const void *value = arr_get(src, i);

		if (arr_index(arr, value) != ARR_END) {
			continue;
		}

		arr_set(arr, arr_add(arr), value);
	}

	return arr;
}

arr_t *arr_merge_all(arr_t *arr, const arr_t *arr1, const arr_t *arr2)
{
	if (arr1 == NULL || arr2 == NULL || arr1->size != arr2->size) {
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
	if (arr1 == NULL || arr2 == NULL || arr1->size != arr2->size) {
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

int arr_print(const arr_t *arr, FILE *file, arr_print_cb cb, int ret)
{
	if (arr == NULL) {
		return 1;
	}

	void *value;
	arr_foreach(arr, value)
	{
		ret = cb(file, value, ret);
	}

	return ret;
}
