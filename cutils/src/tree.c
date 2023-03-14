#include "tree.h"

#include "mem.h"
#include "print.h"

typedef struct header_s {
	tnode_t child;
	tnode_t next;
} header_t;

typedef struct node_s {
	header_t header;
	void *data;
} node_t;

static inline node_t *get_node(const tree_t *tree, tnode_t node)
{
	return (node_t *)((unsigned char *)tree->nodes + node * (sizeof(header_t) + tree->size));
}

static inline void init_node(tree_t *tree, tnode_t node)
{
	node_t *ptr = get_node(tree, node);

	ptr->header.child = 0;
	ptr->header.next  = 0;
}

tree_t *tree_init(tree_t *tree, unsigned int cap, size_t size)
{
	tree->nodes = m_calloc(cap, sizeof(header_t) + size);
	if (tree->nodes == NULL) {
		return NULL;
	}

	tree->cap  = cap;
	tree->cnt  = 1;
	tree->size = size;

	init_node(tree, 0);

	return tree;
}

void tree_free(tree_t *tree)
{
	m_free(tree->nodes, tree->cap * (sizeof(header_t) + tree->size));
	tree->nodes = NULL;
	tree->cap   = 0;
	tree->cnt   = 0;
	tree->size  = 0;
}

tnode_t tree_add_child(tree_t *tree, tnode_t node)
{
	const size_t node_size = sizeof(header_t) + tree->size;

	if (tree->cnt >= tree->cap) {
		size_t old_size = tree->cap * node_size;
		tree->cap *= 2;
		tree->nodes = m_realloc(tree->nodes, tree->cap * node_size, old_size);
		if (tree->nodes == NULL) {
			return -1;
		}
	}

	tnode_t child = tree->cnt;

	tnode_t *target = &get_node(tree, node)->header.child;
	while (*target != 0) {
		target = &get_node(tree, *target)->header.next;
	}
	*target = child;
	tree->cnt++;

	init_node(tree, child);
	return child;
}

tnode_t tree_get_child(const tree_t *tree, tnode_t node)
{
	return get_node(tree, node)->header.child;
}

tnode_t tree_get_next(const tree_t *tree, tnode_t node)
{
	return get_node(tree, node)->header.next;
}

void *tree_get_data(const tree_t *tree, tnode_t node)
{
	return &get_node(tree, node)->data;
}

static void node_iterate_pre(const tree_t *tree, tnode_t node, tree_iterate_cb cb, void *priv, int depth, int last)
{
	cb(tree, node, depth, last, priv);

	tnode_t child = tree_get_child(tree, node);
	tnode_t next;

	while (child != 0) {
		next = tree_get_next(tree, child);
		node_iterate_pre(tree, child, cb, priv, depth + 1, last | ((next == 0) << depth));
		child = next;
	}
}

void tree_iterate_pre(const tree_t *tree, tnode_t node, tree_iterate_cb cb, void *priv)
{
	node_iterate_pre(tree, node, cb, priv, 0, 0);
}

typedef struct node_print_priv_s {
	FILE *f;
	tree_print_cb cb;
} node_print_priv_t;

static void node_print(const tree_t *tree, tnode_t node, int depth, int last, void *priv)
{
	node_print_priv_t *p = priv;

	for (int i = 0; i < depth - 1; i++) {
		if ((1 << i) & last) {
			p_fprintf(p->f, "  ");
		} else {
			p_v(p->f);
		}
	}

	if (depth > 0) {
		if ((1 << (depth - 1)) & last) {
			p_ur(p->f);
		} else {
			p_vr(p->f);
		}
	}

	p->cb(p->f, tree_get_data(tree, node));
}

void tree_print(const tree_t *tree, tnode_t node, FILE *f, tree_print_cb cb)
{
	node_print_priv_t priv = {
		.f  = f,
		.cb = cb,
	};
	tree_iterate_pre(tree, 0, node_print, &priv);
}
