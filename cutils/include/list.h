#ifndef LIST_H
#define LIST_H

#include "arr.h"
#include "type.h"

#include <stdio.h>

typedef uint lnode_t;
typedef arr_t list_t;

list_t *list_init(list_t *list, uint cap, size_t size);
void list_free(list_t *list);

lnode_t list_add(list_t *list);
void list_remove(list_t *list, lnode_t node);

lnode_t list_add_next(list_t *list, lnode_t node);
lnode_t list_get_next(const list_t *list, lnode_t node);

void *list_get_data(const list_t *list, lnode_t node);

typedef int (*list_iterate_cb)(const list_t *list, lnode_t node, void *value, int ret, int last, void *priv);
int list_iterate(const list_t *list, lnode_t node, list_iterate_cb cb, int ret, void *priv);

typedef int (*list_iterate_all_cb)(const list_t *list, lnode_t node, void *value, int ret, int last, void *priv);
int list_iterate_all(const list_t *list, list_iterate_all_cb cb, int ret, void *priv);

typedef int (*list_print_cb)(FILE *file, void *value, int ret);
int list_print(const list_t *list, lnode_t node, FILE *file, list_print_cb cb, int ret);

#endif
