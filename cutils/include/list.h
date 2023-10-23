#ifndef LIST_H
#define LIST_H

#include "arr.h"
#include "print.h"
#include "type.h"

#define LIST_END ARR_END

typedef uint lnode_t;
typedef arr_t list_t;

list_t *list_init(list_t *list, uint cap, size_t size);
void list_free(list_t *list);

lnode_t list_add(list_t *list);
int list_remove(list_t *list, lnode_t node);

lnode_t list_add_next(list_t *list, lnode_t node);
lnode_t list_set_next(list_t *list, lnode_t node, lnode_t next);
lnode_t list_get_next(const list_t *list, lnode_t node);

void *list_get_data(const list_t *list, lnode_t node);

typedef int (*list_print_cb)(FILE *file, void *value, int ret);
int list_print(const list_t *list, lnode_t node, FILE *file, list_print_cb cb, int ret);

#define list_foreach(_list, _node, _val) for (lnode_t _i = _node; _i != LIST_END && (_val = list_get_data(_list, _i)); _i = list_get_next(_list, _i))

#define list_foreach_all(_list, _val) for (lnode_t _i = 0; _i < (_list)->cnt && (_val = list_get_data(_list, _i)); _i++)

#define list_add_node(_list, _start, _node) \
	if (_start == LIST_END) {           \
		_node  = list_add(_list);   \
		_start = _node;             \
	} else {                            \
		_node = _start;             \
	}

#define list_add_next_node(_list, _start, _node)      \
	if (_start == LIST_END) {                     \
		_node  = list_add(_list);             \
		_start = _node;                       \
	} else {                                      \
		_node = list_add_next(_list, _start); \
	}

#define list_set_next_node(_list, _node, _next) _node == LIST_END ? _node = _next : list_set_next(_list, _node, _next);

#endif
