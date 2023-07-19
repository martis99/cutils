#ifndef TREE_H
#define TREE_H

#include "list.h"
#include "type.h"

#include <stdio.h>

typedef lnode_t tnode_t;
typedef list_t tree_t;

tree_t *tree_init(tree_t *tree, uint cap, size_t size);
void tree_free(tree_t *tree);

tnode_t tree_add(tree_t *tree);
void tree_remove(tree_t *tree, tnode_t node);

tnode_t tree_add_child(tree_t *tree, tnode_t node);
tnode_t tree_get_child(const tree_t *tree, tnode_t node);
bool tree_has_child(const tree_t *tree, tnode_t node);

tnode_t tree_add_next(tree_t *tree, tnode_t node);
tnode_t tree_get_next(const tree_t *tree, tnode_t node);

void *tree_get_data(const tree_t *tree, tnode_t node);

typedef int (*tree_iterate_cb)(const tree_t *tree, tnode_t node, void *value, int ret, int depth, int last, void *priv);
int tree_iterate_pre(const tree_t *tree, tnode_t node, tree_iterate_cb cb, int ret, void *priv);

typedef int (*tree_iterate_childs_cb)(const tree_t *tree, tnode_t node, void *value, int ret, int last, void *priv);
int tree_iterate_childs(const tree_t *tree, tnode_t node, tree_iterate_childs_cb cb, int ret, void *priv);

typedef int (*tree_print_cb)(FILE *file, void *data, int ret);
int tree_print(const tree_t *tree, tnode_t node, FILE *file, tree_print_cb cb, int ret);

typedef struct tree_it {
	const tree_t *tree;
	tnode_t stack[16];
	int top;
} tree_it;

tree_it tree_it_begin(const tree_t *tree, tnode_t node);
void tree_it_next(tree_it *it);

#define tree_foreach(_tree, _start, _node, _depth) \
	for (tree_it _it = tree_it_begin(_tree, _start); ((_depth = _it.top - 1) >= 0) && ((_node = _it.stack[_it.top - 1]) != -1); tree_it_next(&_it))

#define tree_foreach_all(_tree, _node) for (_node = 0; _node < (_tree)->cnt; _node++)

#define tree_foreach_child(_tree, _parent, _node) for (_node = tree_get_child(_tree, _parent); _node != -1; _node = tree_get_next(_tree, _node))

#endif
