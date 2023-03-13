#include "tree.h"

#include "mem.h"

typedef struct header_s {
	unsigned int child;
	unsigned int next;
} header_t;

typedef struct node_s {
	header_t header;
	void *data;
} node_t;

tree_t *tree_init(tree_t *tree, unsigned int cap, size_t size)
{
	tree->nodes = m_calloc(cap, sizeof(header_t) + size);
	if (tree->nodes == NULL) {
		return NULL;
	}

	tree->cap  = cap;
	tree->cnt  = 0;
	tree->size = size;

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
