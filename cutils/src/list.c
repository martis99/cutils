#include "list.h"

#include "mem.h"
#include "print.h"

typedef struct header_s {
	lnode_t next;
} header_t;

static inline lnode_t init_node(list_t *list, lnode_t node)
{
	header_t *ptr = arr_get(list, node);
	if (ptr == NULL) {
		return -1;
	}

	ptr->next = -1;
	return node;
}

list_t *list_init(list_t *list, uint cap, size_t size)
{
	return arr_init(list, cap, sizeof(header_t) + size);
}

void list_free(list_t *list)
{
	arr_free(list);
}

lnode_t list_add(list_t *list)
{
	lnode_t node = arr_add(list);
	if (node == -1) {
		return -1;
	}

	return init_node(list, node);
}

void list_remove(list_t *list, lnode_t node)
{
	header_t *header = arr_get(list, node);
	if (header == NULL) {
		return;
	}

	for (uint i = 0; i < list->cnt; i++) {
		header_t *prev = arr_get(list, i);
		if (i != node && prev->next == node) {
			prev->next = header->next;
		}
	}
}

lnode_t list_add_next(list_t *list, lnode_t node)
{
	lnode_t next = list_add(list);

	header_t *header = arr_get(list, node);
	if (header == NULL) {
		return -1;
	}

	lnode_t *target = &header->next;
	while (*target != -1) {
		target = &((header_t *)arr_get(list, *target))->next;
	}

	*target = next;

	return *target;
}

lnode_t list_get_next(const list_t *list, lnode_t node)
{
	header_t *header = arr_get(list, node);
	if (header == NULL) {
		return -1;
	}

	return header->next;
}

void *list_get_data(const list_t *list, lnode_t node)
{
	header_t *header = arr_get(list, node);
	if (header == NULL) {
		return NULL;
	}

	return (byte *)header + sizeof(header_t);
}

int list_print(const list_t *list, lnode_t node, FILE *file, list_print_cb cb, int ret)
{
	void *value;
	list_foreach(list, node, value)
	{
		ret = cb(file, value, ret);
	}

	return ret;
}
