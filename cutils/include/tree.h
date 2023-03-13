#ifndef TREE_H
#define TREE_H

#include <stddef.h>

typedef struct tree_s {
	void *nodes;
	unsigned int cap;
	unsigned int cnt;
        size_t size;
} tree_t;

tree_t *tree_init(tree_t *tree, unsigned int cap, size_t size);
void tree_free(tree_t *tree);

#endif
