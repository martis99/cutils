#include "list.h"

#include "mem.h"
#include "print.h"

typedef struct header_s {
	lnode_t next;
} header_t;

static inline header_t *get_node(const list_t *list, lnode_t node)
{
	return (header_t *)((byte *)list->nodes + node * (sizeof(header_t) + list->size));
}

static inline void init_node(list_t *list, lnode_t node)
{
	header_t *ptr = get_node(list, node);

	ptr->next = 0;
}

list_t *list_init(list_t *list, uint cap, size_t size)
{
	list->nodes = m_calloc(cap, sizeof(header_t) + size);
	if (list->nodes == NULL) {
		return NULL;
	}

	list->cap  = cap;
	list->cnt  = 0;
	list->size = size;

	return list;
}

void list_free(list_t *list)
{
	m_free(list->nodes, list->cap * (sizeof(header_t) + list->size));
	list->nodes = NULL;
	list->cap   = 0;
	list->cnt   = 0;
	list->size  = 0;
}

static int list_resize(list_t *list)
{
	if (list->cnt < list->cap) {
		return 0;
	}

	const size_t node_size = sizeof(header_t) + list->size;

	size_t old_size = list->cap * node_size;
	list->cap *= 2;
	list->nodes = m_realloc(list->nodes, list->cap * node_size, old_size);
	if (list->nodes == NULL) {
		return 1;
	}

	return 0;
}

lnode_t list_add(list_t *list)
{
	if (list_resize(list)) {
		return -1;
	}

	lnode_t node = list->cnt;
	list->cnt++;

	init_node(list, node);
	return node;
}

lnode_t list_add_next(list_t *list, lnode_t node)
{
	lnode_t *target = &get_node(list, node)->next;
	while (*target != 0) {
		target = &get_node(list, *target)->next;
	}
	*target = list_add(list);
	return *target;
}

lnode_t list_get_next(const list_t *list, lnode_t node)
{
	return get_node(list, node)->next;
}

void *list_get_data(const list_t *list, lnode_t node)
{
	return (byte *)get_node(list, node) + sizeof(header_t);
}

int list_iterate(const list_t *list, lnode_t node, list_iterate_cb cb, void *priv)
{
	int ret = 0;

	lnode_t cur = node;
	lnode_t next;

	do {
		next = list_get_next(list, cur);
		ret += cb(list, cur, next == 0, priv);
		cur = next;
	} while (cur != 0);

	return ret;
}

int list_iterate_all(const list_t *list, list_iterate_all_cb cb, void *priv)
{
	int ret = 0;

	lnode_t next;

	for (uint i = 0; i < list->cnt; i++) {
		next = list_get_next(list, i);
		ret += cb(list, i, next == 0, priv);
	}

	return ret;
}

typedef struct node_print_priv_s {
	FILE *file;
	list_print_cb cb;
} node_print_priv_t;

static int node_print(const list_t *list, lnode_t node, int last, void *priv)
{
	node_print_priv_t *p = priv;

	return p->cb(p->file, list_get_data(list, node));
}

int list_print(const list_t *list, lnode_t node, FILE *file, list_print_cb cb)
{
	node_print_priv_t priv = {
		.file = file,
		.cb   = cb,
	};
	return list_iterate(list, node, node_print, &priv);
}
