#ifndef ARR_H
#define ARR_H

#include "type.h"

#include <stdio.h>

typedef struct arr_s {
	void *data;
	uint cap;
	uint cnt;
	size_t size;
} arr_t;

arr_t *arr_init(arr_t *arr, uint cap, size_t size);
void arr_free(arr_t *arr);

uint arr_add(arr_t *arr);

void *arr_get(const arr_t *arr, uint index);

void *arr_set(arr_t *arr, uint index, const void *value);

uint arr_app(arr_t *arr, const void *value);

uint arr_index(const arr_t *arr, const void *value);

typedef int (*arr_index_cmp_cb)(const void *value1, const void *value2);
uint arr_index_cmp(const arr_t *arr, const void *value, arr_index_cmp_cb cb);

arr_t *arr_add_all(arr_t *arr, const arr_t *src);

arr_t *arr_add_unique(arr_t *arr, const arr_t *src);

arr_t *arr_merge_all(arr_t *arr, const arr_t *arr1, const arr_t *arr2);

arr_t *arr_merge_unique(arr_t *arr, const arr_t *arr1, const arr_t *arr2);

typedef int (*arr_iterate_cb)(const arr_t *arr, uint index, void *value, int ret, void *priv);
int arr_iterate(const arr_t *arr, arr_iterate_cb cb, int ret, void *priv);

typedef int (*arr_print_cb)(FILE *file, void *value, int ret);
int arr_print(const arr_t *arr, FILE *file, arr_print_cb cb, int ret);

#endif
