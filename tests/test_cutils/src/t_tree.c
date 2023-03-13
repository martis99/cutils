#include "t_tree.h"

#include "tree.h"

#include "test.h"

TEST(init_free)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 10, sizeof(int));

	EXPECT_NE(tree.nodes, NULL);
	EXPECT_EQ(tree.cap, 10);
	EXPECT_EQ(tree.cnt, 0);
	EXPECT_EQ(tree.size, sizeof(int));

	tree_free(&tree);

	EXPECT_EQ(tree.nodes, 0);
	EXPECT_EQ(tree.cap, 0);
	EXPECT_EQ(tree.cnt, 0);
	EXPECT_EQ(tree.size, 0);

	END;
}

STEST(tree)
{
	SSTART;
	RUN(init_free);
	SEND;
}
