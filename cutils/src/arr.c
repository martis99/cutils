#include "arr.h"

#include "log.h"
#include "mem.h"

#include <stdlib.h>

arr_t *arr_init(arr_t *arr, uint cap, size_t size)
{
	if (arr == NULL) {
		return NULL;
	}

	arr->data = mem_alloc(cap * size);
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

	mem_free(arr->data, arr->cap * arr->size);
	arr->data = NULL;
	arr->cap  = 0;
	arr->cnt  = 0;
	arr->size = 0;
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

static inline int arr_resize(arr_t *arr)
{
	if (arr->cnt < arr->cap) {
		return 0;
	}

	uint new_cap = MAX(1, arr->cap * 2);

	void *data = mem_realloc(arr->data, new_cap * arr->size, arr->cap * arr->size);
	if (data == NULL) {
		return 1;
	}

	arr->data = data;
	arr->cap  = new_cap;

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
	if (arr == NULL) {
		return NULL;
	}

	if (index >= arr->cnt) {
		log_error("cutils", "arr", NULL, "invalid id: %d", index);
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

	return mem_cpy(dst, arr->size, value, arr->size);
}

uint arr_app(arr_t *arr, const void *value)
{
	if (value == NULL) {
		return ARR_END;
	}

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
	}

	for (uint i = 0; i < arr->cnt; i++) {
		if (mem_cmp(arr_get(arr, i), value, arr->size) == 0) {
			return i;
		}
	}

	return ARR_END;
}

uint arr_index_cmp(const arr_t *arr, const void *value, arr_cmp_cb cb)
{
	if (arr == NULL || value == NULL || cb == NULL) {
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

	mem_cpy((byte *)arr->data + arr->cnt * arr->size, arr->cap * arr->size - arr->cnt * arr->size, src->data, src->cnt * src->size);

	arr->cnt += src->cnt;

	return arr;
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

	arr_add_all(arr, arr1);
	arr_add_all(arr, arr2);

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

	arr_add_unique(arr, arr1);
	arr_add_unique(arr, arr2);

	return arr;
}

arr_t *arr_sort(arr_t *arr, arr_cmp_cb cb)
{
	if (arr == NULL) {
		return NULL;
	}

	if (cb == NULL) {
		return arr;
	}

	void *value1;
	arr_foreach(arr, value1)
	{
		void *value2;
		uint j = _i + 1;
		arr_foreach_i(arr, value2, j)
		{
			if (cb(value1, value2) > 0) {
				mem_swap(value1, value2, arr->size);
			}
		}
	}

	return arr;
}

int arr_print(const arr_t *arr, FILE *file, arr_print_cb cb, int ret)
{
	if (arr == NULL || cb == NULL) {
		return ret;
	}

	void *value;
	arr_foreach(arr, value)
	{
		ret = cb(file, value, ret);
	}

	return ret;
}
