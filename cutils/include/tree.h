#ifndef TREE_H
#define TREE_H

#include <stdio.h>

typedef unsigned int tnode_t;

typedef struct tree_s {
	void *nodes;
	unsigned int cap;
	unsigned int cnt;
	size_t size;
} tree_t;

tree_t *tree_init(tree_t *tree, unsigned int cap, size_t size);
void tree_free(tree_t *tree);

tnode_t tree_add_child(tree_t *tree, tnode_t node);
tnode_t tree_get_child(const tree_t *tree, tnode_t node);

tnode_t tree_add_next(tree_t *tree, tnode_t node);
tnode_t tree_get_next(const tree_t *tree, tnode_t node);

void *tree_get_data(const tree_t *tree, tnode_t node);

typedef int (*tree_iterate_cb)(const tree_t *tree, tnode_t node, int depth, int last, void *priv);
int tree_iterate_pre(const tree_t *tree, tnode_t node, tree_iterate_cb cb, void *priv);

typedef int (*tree_iterate_childs_cb)(const tree_t *tree, tnode_t node, int last, void *priv);
int tree_iterate_childs(const tree_t *tree, tnode_t node, tree_iterate_childs_cb cb, void *priv);

typedef int (*tree_print_cb)(FILE *file, void *data);
int tree_print(const tree_t *tree, tnode_t node, FILE *file, tree_print_cb cb);

#endif
