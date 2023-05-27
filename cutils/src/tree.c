#include "tree.h"

#include "mem.h"
#include "print.h"

typedef struct header_s {
	tnode_t child;
} header_t;

static inline header_t *get_node(const tree_t *tree, tnode_t node)
{
	return list_get_data(tree, node);
}

static inline void init_node(tree_t *tree, tnode_t node)
{
	header_t *ptr = get_node(tree, node);

	ptr->child = 0;
}

tree_t *tree_init(tree_t *tree, uint cap, size_t size)
{
	tree = list_init(tree, cap, sizeof(header_t) + size);
	if (tree == NULL) {
		return NULL;
	}

	tnode_t node = list_add(tree);

	init_node(tree, node);

	return tree;
}

void tree_free(tree_t *tree)
{
	list_free(tree);
}

tnode_t tree_add_child(tree_t *tree, tnode_t node)
{
	header_t *nodeh = get_node(tree, node);
	tnode_t child	= nodeh->child == 0 ? nodeh->child = list_add(tree) : list_add_next(tree, nodeh->child);
	init_node(tree, child);
	return child;
}

tnode_t tree_get_child(const tree_t *tree, tnode_t node)
{
	return get_node(tree, node)->child;
}

tnode_t tree_add_next(tree_t *tree, tnode_t node)
{
	header_t *nodeh = get_node(tree, node);
	tnode_t child	= list_add_next(tree, node);
	init_node(tree, child);
	return child;
}

tnode_t tree_get_next(const tree_t *tree, tnode_t node)
{
	return list_get_next(tree, node);
}

void *tree_get_data(const tree_t *tree, tnode_t node)
{
	return (byte *)get_node(tree, node) + sizeof(header_t);
}

static int node_iterate_pre(const tree_t *tree, tnode_t node, tree_iterate_cb cb, void *priv, int depth, int last)
{
	int ret = cb(tree, node, depth, last, priv);

	tnode_t child = tree_get_child(tree, node);
	tnode_t next;

	while (child != 0) {
		next = tree_get_next(tree, child);
		ret += node_iterate_pre(tree, child, cb, priv, depth + 1, last | ((next == 0) << depth));
		child = next;
	}

	return ret;
}

int tree_iterate_pre(const tree_t *tree, tnode_t node, tree_iterate_cb cb, void *priv)
{
	return node_iterate_pre(tree, node, cb, priv, 0, 0);
}

int tree_iterate_childs(const tree_t *tree, tnode_t node, tree_iterate_childs_cb cb, void *priv)
{
	int ret = 0;

	tnode_t child = tree_get_child(tree, node);
	tnode_t next;

	while (child != 0) {
		next = tree_get_next(tree, child);
		ret += cb(tree, child, next == 0, priv);
		child = next;
	}

	return ret;
}

typedef struct node_print_priv_s {
	FILE *file;
	tree_print_cb cb;
} node_print_priv_t;

static int node_print(const tree_t *tree, tnode_t node, int depth, int last, void *priv)
{
	node_print_priv_t *p = priv;

	for (int i = 0; i < depth - 1; i++) {
		if ((1 << i) & last) {
			p_fprintf(p->file, "  ");
		} else {
			p_v(p->file);
		}
	}

	if (depth > 0) {
		if ((1 << (depth - 1)) & last) {
			p_ur(p->file);
		} else {
			p_vr(p->file);
		}
	}

	return p->cb(p->file, tree_get_data(tree, node));
}

int tree_print(const tree_t *tree, tnode_t node, FILE *file, tree_print_cb cb)
{
	node_print_priv_t priv = {
		.file = file,
		.cb   = cb,
	};
	return tree_iterate_pre(tree, 0, node_print, &priv);
}
