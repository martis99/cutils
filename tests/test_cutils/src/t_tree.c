#include "t_tree.h"

#include "tree.h"

#include "test.h"

TEST(init_free)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 1, sizeof(int));

	EXPECT_NE(tree.data, NULL);
	EXPECT_EQ(tree.cap, 1);
	EXPECT_EQ(tree.cnt, 0);
	EXPECT_NE(tree.size, 0);

	tree_free(&tree);

	EXPECT_EQ(tree.data, NULL);
	EXPECT_EQ(tree.cap, 0);
	EXPECT_EQ(tree.cnt, 0);
	EXPECT_EQ(tree.size, 0);

	END;
}

TEST(add)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 1, sizeof(int));

	tnode_t node = tree_add(&tree);

	EXPECT_EQ(tree.cnt, 1);
	EXPECT_EQ(tree.cap, 1);
	EXPECT_EQ(node, 0);

	tree_free(&tree);

	END;
}

TEST(add_child)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 2, sizeof(int));

	const tnode_t child = tree_add_child(&tree, tree_add(&tree));

	EXPECT_EQ(child, 1);
	EXPECT_EQ(tree.cnt, 2);
	EXPECT_EQ(tree.cap, 2);

	tree_free(&tree);

	END;
}

TEST(add_childs)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 3, sizeof(int));

	const tnode_t n1 = tree_add_child(&tree, tree_add(&tree));
	const tnode_t n2 = tree_add_child(&tree, 0);

	EXPECT_EQ(n1, 1);
	EXPECT_EQ(n2, 2);
	EXPECT_EQ(tree.cnt, 3);
	EXPECT_EQ(tree.cap, 3);

	tree_free(&tree);

	END;
}

TEST(add_child_realloc)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 1, sizeof(int));

	*(int *)tree_get_data(&tree, tree_add(&tree)) = 1;

	tnode_t child;

	*(int *)tree_get_data(&tree, (child = tree_add_child(&tree, 0))) = 2;

	EXPECT_EQ(child, 1);
	EXPECT_EQ(tree.cnt, 2);
	EXPECT_EQ(tree.cap, 2);
	EXPECT_EQ(*(int *)tree_get_data(&tree, 0), 1);
	EXPECT_EQ(*(int *)tree_get_data(&tree, child), 2);

	tree_free(&tree);

	END;
}

TEST(add_next)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 2, sizeof(int));

	tnode_t next = tree_add_next(&tree, tree_add(&tree));

	EXPECT_EQ(next, 1);
	EXPECT_EQ(tree.cnt, 2);
	EXPECT_EQ(tree.cap, 2);

	tree_free(&tree);

	END;
}

TEST(add_nexts)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 3, sizeof(int));

	const tnode_t n1 = tree_add_next(&tree, tree_add(&tree));
	const tnode_t n2 = tree_add_next(&tree, 0);

	EXPECT_EQ(n1, 1);
	EXPECT_EQ(n2, 2);
	EXPECT_EQ(tree.cnt, 3);
	EXPECT_EQ(tree.cap, 3);

	tree_free(&tree);

	END;
}

TEST(add_next_realloc)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 1, sizeof(int));

	*(int *)tree_get_data(&tree, tree_add(&tree)) = 1;

	tnode_t next;

	*(int *)tree_get_data(&tree, (next = tree_add_next(&tree, 0))) = 2;

	EXPECT_EQ(next, 1);
	EXPECT_EQ(tree.cnt, 2);
	EXPECT_EQ(tree.cap, 2);
	EXPECT_EQ(*(int *)tree_get_data(&tree, 0), 1);
	EXPECT_EQ(*(int *)tree_get_data(&tree, next), 2);

	tree_free(&tree);

	END;
}

TEST(add_grand_child)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 3, sizeof(int));

	const tnode_t child  = tree_add_child(&tree, tree_add(&tree));
	const tnode_t gchild = tree_add_child(&tree, child);

	EXPECT_EQ(child, 1);
	EXPECT_EQ(gchild, 2);
	EXPECT_EQ(tree.cnt, 3);
	EXPECT_EQ(tree.cap, 3);

	tree_free(&tree);

	END;
}

TEST(add_multiple_childs)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 4, sizeof(int));

	const tnode_t n1  = tree_add_child(&tree, tree_add(&tree));
	const tnode_t n2  = tree_add_child(&tree, 0);
	const tnode_t n12 = tree_add_child(&tree, n1);

	EXPECT_EQ(n1, 1);
	EXPECT_EQ(n2, 2);
	EXPECT_EQ(n12, 3);
	EXPECT_EQ(tree.cnt, 4);
	EXPECT_EQ(tree.cap, 4);

	tree_free(&tree);

	END;
}

TEST(get_data)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 1, sizeof(int));

	*(int *)tree_get_data(&tree, tree_add(&tree)) = 1;

	EXPECT_EQ(*(int *)tree_get_data(&tree, 0), 1);

	tree_free(&tree);

	END;
}

TEST(get_child)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 4, sizeof(int));

	const tnode_t n1  = tree_add_child(&tree, tree_add(&tree));
	const tnode_t n2  = tree_add_child(&tree, 0);
	const tnode_t n12 = tree_add_child(&tree, n1);

	const tnode_t child1 = tree_get_child(&tree, 0);

	EXPECT_EQ(n1, 1);
	EXPECT_EQ(child1, 1);

	tree_free(&tree);

	END;
}

TEST(get_child_data)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 2, sizeof(int));

	*(int *)tree_get_data(&tree, tree_add(&tree))	       = 1;
	*(int *)tree_get_data(&tree, tree_add_child(&tree, 0)) = 2;

	EXPECT_EQ(*(int *)tree_get_data(&tree, 0), 1);
	EXPECT_EQ(*(int *)tree_get_data(&tree, 1), 2);

	tree_free(&tree);

	END;
}

TEST(get_next)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 3, sizeof(int));

	const tnode_t n1 = tree_add_next(&tree, tree_add(&tree));
	const tnode_t n2 = tree_add_next(&tree, 0);

	const tnode_t next1 = tree_get_next(&tree, 0);
	const tnode_t next2 = tree_get_next(&tree, next1);

	EXPECT_EQ(n1, 1);
	EXPECT_EQ(n2, 2);
	EXPECT_EQ(next1, 1);
	EXPECT_EQ(next2, 2);

	tree_free(&tree);

	END;
}

static int iterate_pre_root_cb(const tree_t *tree, tnode_t node, void *value, int ret, int depth, int last, void *priv)
{
	START;

	EXPECT_EQ(node, 0);
	EXPECT_EQ(depth, 0);
	EXPECT_EQ(last, 0);

	*(int *)priv += 1;

	return ret + RES;
}

TEST(iterate_pre_root)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 1, sizeof(int));

	tree_add(&tree);

	int cnt = 0;

	int ret = tree_iterate_pre(&tree, 0, iterate_pre_root_cb, 0, &cnt);

	EXPECT_EQ(cnt, 1);
	EXPECT_EQ(ret, 0);

	tree_free(&tree);

	END;
}

static int iterate_pre_child_cb(const tree_t *tree, tnode_t node, void *value, int ret, int depth, int last, void *priv)
{
	START;

	switch (node) {
	case 0:
		EXPECT_EQ(depth, 0);
		EXPECT_EQ(last, 0);
		break;
	case 1:
		EXPECT_EQ(depth, 1);
		EXPECT_EQ(last, 1);
		break;
	default: EXPECT_FAIL("%s", "Unknown node"); break;
	}

	*(int *)priv += 1;

	return ret + RES;
}

TEST(iterate_pre_child)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 2, sizeof(int));

	const tnode_t child = tree_add_child(&tree, tree_add(&tree));

	int cnt = 0;

	int ret = tree_iterate_pre(&tree, 0, iterate_pre_child_cb, 0, &cnt);

	EXPECT_EQ(child, 1);
	EXPECT_EQ(cnt, 2);
	EXPECT_EQ(ret, 0);

	tree_free(&tree);

	END;
}

static int iterate_pre_childs_cb(const tree_t *tree, tnode_t node, void *value, int ret, int depth, int last, void *priv)
{
	START;

	switch (node) {
	case 0:
		EXPECT_EQ(depth, 0);
		EXPECT_EQ(last, 0);
		break;
	case 1:
		EXPECT_EQ(depth, 1);
		EXPECT_EQ(last, 0);
		break;
	case 2:
		EXPECT_EQ(depth, 1);
		EXPECT_EQ(last, 1);
		break;
	default: EXPECT_FAIL("%s", "Unknown node"); break;
	}

	*(int *)priv += 1;

	return ret + RES;
}

TEST(iterate_pre_childs)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 3, sizeof(int));

	const tnode_t n1 = tree_add_child(&tree, tree_add(&tree));
	const tnode_t n2 = tree_add_child(&tree, 0);

	int cnt = 0;

	int ret = tree_iterate_pre(&tree, 0, iterate_pre_childs_cb, 0, &cnt);

	EXPECT_EQ(n1, 1);
	EXPECT_EQ(n2, 2);
	EXPECT_EQ(cnt, 3);
	EXPECT_EQ(ret, 0);

	tree_free(&tree);

	END;
}

static int iterate_pre_grand_child_cb(const tree_t *tree, tnode_t node, void *value, int ret, int depth, int last, void *priv)
{
	START;

	switch (node) {
	case 0:
		EXPECT_EQ(depth, 0);
		EXPECT_EQ(last, 0);
		break;
	case 1:
		EXPECT_EQ(depth, 1);
		EXPECT_EQ(last, 1);
		break;
	case 2:
		EXPECT_EQ(depth, 2);
		EXPECT_EQ(last, 0b11);
		break;
	default: EXPECT_FAIL("%s", "Unknown node"); break;
	}

	*(int *)priv += 1;

	return ret + RES;
}

TEST(iterate_pre_grand_child)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 3, sizeof(int));

	const tnode_t n1 = tree_add_child(&tree, tree_add(&tree));
	const tnode_t n2 = tree_add_child(&tree, n1);

	int cnt = 0;

	int ret = tree_iterate_pre(&tree, 0, iterate_pre_grand_child_cb, 0, &cnt);

	EXPECT_EQ(n1, 1);
	EXPECT_EQ(n2, 2);
	EXPECT_EQ(cnt, 3);
	EXPECT_EQ(ret, 0);

	tree_free(&tree);

	return ret + RES;
}

static int iterate_childs_root_cb(const tree_t *tree, tnode_t node, void *value, int ret, int last, void *priv)
{
	START;

	EXPECT_FAIL("%s", "Root has child");

	*(int *)priv += 1;

	return ret + RES;
}

TEST(iterate_childs_root)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 1, sizeof(int));

	tree_add(&tree);

	int cnt = 0;

	int ret = tree_iterate_childs(&tree, 0, iterate_childs_root_cb, 0, &cnt);

	EXPECT_EQ(cnt, 0);
	EXPECT_EQ(ret, 0);

	tree_free(&tree);

	END;
}

static int iterate_childs_child_cb(const tree_t *tree, tnode_t node, void *value, int ret, int last, void *priv)
{
	START;

	EXPECT_EQ(node, 1);
	EXPECT_EQ(last, 1);

	*(int *)priv += 1;

	return ret + RES;
}

TEST(iterate_childs_child)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 2, sizeof(int));

	const tnode_t child = tree_add_child(&tree, tree_add(&tree));

	int cnt = 0;

	int ret = tree_iterate_childs(&tree, 0, iterate_childs_child_cb, 0, &cnt);

	EXPECT_EQ(child, 1);
	EXPECT_EQ(cnt, 1);
	EXPECT_EQ(ret, 0);

	tree_free(&tree);

	END;
}

static int iterate_childs_childs_cb(const tree_t *tree, tnode_t node, void *value, int ret, int last, void *priv)
{
	START;

	switch (node) {
	case 1: EXPECT_EQ(last, 0); break;
	case 2: EXPECT_EQ(last, 1); break;
	default: EXPECT_FAIL("%s", "Unknown node"); break;
	}

	*(int *)priv += 1;

	return ret + RES;
}

TEST(iterate_childs_childs)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 3, sizeof(int));

	const tnode_t n1 = tree_add_child(&tree, tree_add(&tree));
	const tnode_t n2 = tree_add_child(&tree, 0);

	int cnt = 0;

	int ret = tree_iterate_childs(&tree, 0, iterate_childs_childs_cb, 0, &cnt);

	EXPECT_EQ(n1, 1);
	EXPECT_EQ(n2, 2);
	EXPECT_EQ(cnt, 2);
	EXPECT_EQ(ret, 0);

	tree_free(&tree);

	END;
}

static int iterate_childs_grand_child_cb(const tree_t *tree, tnode_t node, void *value, int ret, int last, void *priv)
{
	START;

	switch (node) {
	case 1: EXPECT_EQ(last, 0); break;
	case 2: EXPECT_EQ(last, 1); break;
	default: EXPECT_FAIL("%s", "Unknown node"); break;
	}

	*(int *)priv += 1;

	return ret + RES;
}

TEST(iterate_childs_grand_child)
{
	START;

	tree_t tree = { 0 };

	tree_init(&tree, 4, sizeof(int));

	const tnode_t n1 = tree_add_child(&tree, tree_add(&tree));
	const tnode_t n2 = tree_add_child(&tree, 0);
	const tnode_t n3 = tree_add_child(&tree, n1);

	int cnt = 0;

	int ret = tree_iterate_childs(&tree, 0, iterate_childs_grand_child_cb, 0, &cnt);

	EXPECT_EQ(n1, 1);
	EXPECT_EQ(n2, 2);
	EXPECT_EQ(n3, 3);
	EXPECT_EQ(cnt, 2);
	EXPECT_EQ(ret, 0);

	tree_free(&tree);

	END;
}

TEST(addt)
{
	SSTART;
	RUN(add);
	RUN(add_child);
	RUN(add_childs);
	RUN(add_child_realloc);
	RUN(add_next);
	RUN(add_nexts);
	RUN(add_next_realloc);
	RUN(add_grand_child);
	RUN(add_multiple_childs);
	SEND;
}

TEST(get)
{
	SSTART;
	RUN(get_data);
	RUN(get_child);
	RUN(get_child_data);
	RUN(get_next);
	SEND;
}

TEST(iterate_pre)
{
	SSTART;
	RUN(iterate_pre_root);
	RUN(iterate_pre_child);
	RUN(iterate_pre_childs);
	RUN(iterate_pre_grand_child);
	SEND;
}

TEST(iterate_childs)
{
	SSTART;
	RUN(iterate_childs_root);
	RUN(iterate_childs_child);
	RUN(iterate_childs_childs);
	RUN(iterate_childs_grand_child);
	SEND;
}

STEST(tree)
{
	SSTART;
	RUN(init_free);
	RUN(addt);
	RUN(get);
	RUN(iterate_pre);
	RUN(iterate_childs);
	SEND;
}
