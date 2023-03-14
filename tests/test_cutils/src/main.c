#include "t_tree.h"

#include "tree.h"

#include "test.h"

#include "mem.h"

#include <stdio.h>

TEST(test)
{
	SSTART;
	RUN(tree);
	SEND;
}

static void print_node(const tree_t *tree, tnode_t node, int depth, int last, void *priv)
{
	for (int i = 0; i < depth - 1; i++) {
		printf((1 << i) & last ? "  " : "│ ");
	}

	if (depth > 0) {
		printf("%s", (1 << (depth - 1)) & last ? "└─" : "├─");
	}
	printf("%d\n", *(int *)tree_get_data(tree, node));
}

static void tree_ex(m_stats_t *m_stats)
{
	tree_t tree = { 0 };

	tree_init(&tree, 10, sizeof(int));

	*(int *)tree_get_data(&tree, 0) = 0;

	tnode_t n1, n2, n3, n11, n12, n13, n21, n22, n23, n31, n32, n33, n111, n131, n212, n222, n232, n333, n1111;

	*(int *)tree_get_data(&tree, (n1 = tree_add_child(&tree, 0))) = 1;
	*(int *)tree_get_data(&tree, (n2 = tree_add_child(&tree, 0))) = 2;
	*(int *)tree_get_data(&tree, (n3 = tree_add_child(&tree, 0))) = 3;

	*(int *)tree_get_data(&tree, (n11 = tree_add_child(&tree, n1))) = 11;
	*(int *)tree_get_data(&tree, (n12 = tree_add_child(&tree, n1))) = 12;
	*(int *)tree_get_data(&tree, (n13 = tree_add_child(&tree, n1))) = 13;

	*(int *)tree_get_data(&tree, (n21 = tree_add_child(&tree, n2))) = 21;
	*(int *)tree_get_data(&tree, (n22 = tree_add_child(&tree, n2))) = 22;
	*(int *)tree_get_data(&tree, (n23 = tree_add_child(&tree, n2))) = 23;

	*(int *)tree_get_data(&tree, (n31 = tree_add_child(&tree, n3))) = 31;
	*(int *)tree_get_data(&tree, (n32 = tree_add_child(&tree, n3))) = 32;
	*(int *)tree_get_data(&tree, (n33 = tree_add_child(&tree, n3))) = 33;

	*(int *)tree_get_data(&tree, (n111 = tree_add_child(&tree, n11))) = 111;
	*(int *)tree_get_data(&tree, (n131 = tree_add_child(&tree, n13))) = 131;

	*(int *)tree_get_data(&tree, (n212 = tree_add_child(&tree, n21))) = 212;
	*(int *)tree_get_data(&tree, (n222 = tree_add_child(&tree, n22))) = 222;
	*(int *)tree_get_data(&tree, (n232 = tree_add_child(&tree, n23))) = 232;

	*(int *)tree_get_data(&tree, (n333 = tree_add_child(&tree, n33))) = 333;

	*(int *)tree_get_data(&tree, (n1111 = tree_add_child(&tree, n111))) = 1111;

	tree_iterate_pre(&tree, 0, print_node, NULL);

	printf("mem: %zd\n", m_stats->mem);

	tree_free(&tree);
}

int main(int argc, char **argv)
{
	m_stats_t m_stats = { 0 };
	m_init(&m_stats);

	t_init(4);

	test();

	t_results();
	t_free();

	printf("\n");

	tree_ex(&m_stats);

	printf("mem:      %zd (max: %zd)\n", m_stats.mem, m_stats.mem_max);
	printf("allocs:   %d\n", m_stats.allocs);
	printf("reallocs: %d\n", m_stats.reallocs);

	return 0;
}
