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

static inline tnode_t init_node(tree_t *tree, tnode_t node)
{
	header_t *data = get_node(tree, node);
	if (data == NULL) {
		return -1;
	}

	data->child = -1;
	return node;
}

tree_t *tree_init(tree_t *tree, uint cap, size_t size)
{
	return list_init(tree, cap, sizeof(header_t) + size);
}

void tree_free(tree_t *tree)
{
	list_free(tree);
}

tnode_t tree_add(tree_t *tree)
{
	return init_node(tree, list_add(tree));
}

void tree_remove(tree_t *tree, tnode_t node)
{
	for (uint i = 0; i < tree->cnt; i++) {
		header_t *data = get_node(tree, i);
		if (data == NULL) {
			continue;
		}
		if (data->child == node) {
			data->child = list_get_next(tree, node);
		}
	}

	list_remove(tree, node);
}

tnode_t tree_add_child(tree_t *tree, tnode_t node)
{
	header_t *header = get_node(tree, node);
	if (header == NULL) {
		return -1;
	}

	tnode_t child = header->child == -1 ? get_node(tree, node)->child = list_add(tree) : list_add_next(tree, header->child);
	return init_node(tree, child);
}

tnode_t tree_get_child(const tree_t *tree, tnode_t node)
{
	header_t *header = get_node(tree, node);
	return header == NULL ? -1 : header->child;
}

bool tree_has_child(const tree_t *tree, tnode_t node)
{
	header_t *header = get_node(tree, node);
	return header != NULL && header->child != -1;
}

tnode_t tree_add_next(tree_t *tree, tnode_t node)
{
	return init_node(tree, list_add_next(tree, node));
}

tnode_t tree_get_next(const tree_t *tree, tnode_t node)
{
	return list_get_next(tree, node);
}

void *tree_get_data(const tree_t *tree, tnode_t node)
{
	header_t *header = get_node(tree, node);
	return header == NULL ? NULL : header + 1;
}

static int node_iterate_pre(const tree_t *tree, tnode_t node, tree_iterate_cb cb, int ret, void *priv, int depth, int last)
{
	if (node >= tree->cnt) {
		return ret;
	}

	ret = cb(tree, node, tree_get_data(tree, node), ret, depth, last, priv);

	tnode_t child = tree_get_child(tree, node);
	tnode_t next;

	while (child != -1) {
		next  = tree_get_next(tree, child);
		ret   = node_iterate_pre(tree, child, cb, ret, priv, depth + 1, last | ((next == -1) << depth));
		child = next;
	}

	return ret;
}

int tree_iterate_pre(const tree_t *tree, tnode_t node, tree_iterate_cb cb, int ret, void *priv)
{
	return node_iterate_pre(tree, node, cb, ret, priv, 0, 0);
}

int tree_iterate_childs(const tree_t *tree, tnode_t node, tree_iterate_childs_cb cb, int ret, void *priv)
{
	tnode_t child = tree_get_child(tree, node);
	tnode_t next;

	while (child != -1) {
		next  = tree_get_next(tree, child);
		ret   = cb(tree, child, tree_get_data(tree, child), ret, next == -1, priv);
		child = next;
	}

	return ret;
}

typedef struct tree_print_priv_s {
	FILE *file;
	tree_print_cb cb;
} tree_print_priv_t;

static int print_cb(const tree_t *tree, tnode_t node, void *value, int ret, int depth, int last, void *priv)
{
	tree_print_priv_t *p = priv;

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

	return p->cb(p->file, value, ret);
}

int tree_print(const tree_t *tree, tnode_t node, FILE *file, tree_print_cb cb, int ret)
{
	tree_print_priv_t priv = {
		.file = file,
		.cb   = cb,
	};
	return tree_iterate_pre(tree, node, print_cb, ret, &priv);
}
