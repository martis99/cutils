#include "t_tree.h"

#include "tree.h"

#include "test.h"

#include <stdio.h>

TEST(init_free)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 10, sizeof(int));

	EXPECT_NE(tree.nodes, NULL);
	EXPECT_EQ(tree.cap, 10);
	EXPECT_EQ(tree.cnt, 1);
	EXPECT_EQ(tree.size, sizeof(int));

	tree_free(&tree);

	EXPECT_EQ(tree.nodes, 0);
	EXPECT_EQ(tree.cap, 0);
	EXPECT_EQ(tree.cnt, 0);
	EXPECT_EQ(tree.size, 0);

	END;
}

TEST(add_child)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 10, sizeof(int));

	*(int *)tree_get_data(&tree, 0) = 0;

	tnode_t n1, n2, n3, n11, n12, n13, n21, n22, n23, n31, n32, n33, n111, n131, n212, n222, n232, n333, n1111;

	*(int *)tree_get_data(&tree, (n1 = tree_add_child(&tree, 0))) = 1;
	*(int *)tree_get_data(&tree, (n2 = tree_add_child(&tree, 0))) = 2;

	*(int *)tree_get_data(&tree, (n12 = tree_add_child(&tree, n1))) = 12;

	EXPECT_EQ(tree.cnt, 4);

	tree_free(&tree);

	END;
}

STEST(tree)
{
	SSTART;
	RUN(init_free);
	RUN(add_child);
	SEND;
}
