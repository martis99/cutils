#include "t_tree.h"

#include "file.h"
#include "print.h"
#include "tree.h"

#include "test.h"

#define TEST_FILE "t_tree.txt"

TEST(t_tree_init_free)
{
	START;

	tree_t tree = { 0 };

	EXPECT_EQ(tree_init(NULL, 0, sizeof(int)), NULL);
	EXPECT_EQ(tree_init(&tree, 0, sizeof(int)), NULL);
	EXPECT_NE(tree_init(&tree, 1, sizeof(int)), NULL);

	EXPECT_NE(tree.data, NULL);
	EXPECT_EQ(tree.cap, 1);
	EXPECT_EQ(tree.cnt, 0);
	EXPECT_NE(tree.size, 0);

	tree_free(&tree);
	tree_free(NULL);

	EXPECT_EQ(tree.data, NULL);
	EXPECT_EQ(tree.cap, 0);
	EXPECT_EQ(tree.cnt, 0);
	EXPECT_EQ(tree.size, 0);

	END;
}

TEST(t_tree_add)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	EXPECT_EQ(tree_add(NULL), TREE_END);
	EXPECT_NE(tree_add(&tree), TREE_END);

	EXPECT_EQ(tree.cnt, 1);
	EXPECT_EQ(tree.cap, 1);

	tree_free(&tree);

	END;
}

TEST(t_tree_add_child)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	EXPECT_EQ(tree_add_child(NULL, TREE_END), TREE_END);
	EXPECT_EQ(tree_add_child(&tree, TREE_END), TREE_END);
	EXPECT_NE(tree_add_child(&tree, tree_add(&tree)), TREE_END);

	EXPECT_EQ(tree.cnt, 2);
	EXPECT_EQ(tree.cap, 2);

	tree_free(&tree);

	END;
}

TEST(t_tree_add_childs)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	const tnode_t n1 = tree_add_child(&tree, tree_add(&tree));
	const tnode_t n2 = tree_add_child(&tree, 0);

	EXPECT_EQ(n1, 1);
	EXPECT_EQ(n2, 2);
	EXPECT_EQ(tree.cnt, 3);
	EXPECT_EQ(tree.cap, 4);

	tree_free(&tree);

	END;
}

TEST(t_tree_add_child_realloc)
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

TEST(t_tree_add_next)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	EXPECT_EQ(tree_add_next(NULL, TREE_END), TREE_END);
	EXPECT_EQ(tree_add_next(&tree, TREE_END), TREE_END);
	EXPECT_NE(tree_add_next(&tree, tree_add(&tree)), TREE_END);

	EXPECT_EQ(tree.cnt, 2);
	EXPECT_EQ(tree.cap, 2);

	tree_free(&tree);

	END;
}

TEST(t_tree_add_nexts)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	const tnode_t n1 = tree_add_next(&tree, tree_add(&tree));
	const tnode_t n2 = tree_add_next(&tree, 0);

	EXPECT_EQ(n1, 1);
	EXPECT_EQ(n2, 2);
	EXPECT_EQ(tree.cnt, 3);
	EXPECT_EQ(tree.cap, 4);

	tree_free(&tree);

	END;
}

TEST(t_tree_add_next_realloc)
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

TEST(t_tree_add_grand_child)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	const tnode_t child  = tree_add_child(&tree, tree_add(&tree));
	const tnode_t gchild = tree_add_child(&tree, child);

	EXPECT_EQ(child, 1);
	EXPECT_EQ(gchild, 2);
	EXPECT_EQ(tree.cnt, 3);
	EXPECT_EQ(tree.cap, 4);

	tree_free(&tree);

	END;
}

TEST(t_tree_add_multiple_childs)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

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

TEST(t_tree_adds)
{
	SSTART;
	RUN(t_tree_add);
	RUN(t_tree_add_child);
	RUN(t_tree_add_childs);
	RUN(t_tree_add_child_realloc);
	RUN(t_tree_add_next);
	RUN(t_tree_add_nexts);
	RUN(t_tree_add_next_realloc);
	RUN(t_tree_add_grand_child);
	RUN(t_tree_add_multiple_childs);
	SEND;
}

TEST(t_tree_get_data)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	EXPECT_EQ(tree_get_data(NULL, TREE_END), NULL);
	EXPECT_EQ(tree_get_data(&tree, TREE_END), NULL);

	*(int *)tree_get_data(&tree, tree_add(&tree)) = 1;
	EXPECT_EQ(*(int *)tree_get_data(&tree, 0), 1);

	tree_free(&tree);

	END;
}

TEST(t_tree_has_child)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	EXPECT_EQ(tree_has_child(NULL, TREE_END), 0);
	EXPECT_EQ(tree_has_child(&tree, TREE_END), 0);

	const tnode_t n1  = tree_add_child(&tree, tree_add(&tree));
	const tnode_t n2  = tree_add_child(&tree, 0);
	const tnode_t n12 = tree_add_child(&tree, n1);

	EXPECT_EQ(tree_has_child(&tree, 0), 1);

	tree_free(&tree);

	END;
}

TEST(t_tree_get_child)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	EXPECT_EQ(tree_get_child(NULL, TREE_END), TREE_END);
	EXPECT_EQ(tree_get_child(&tree, TREE_END), TREE_END);

	const tnode_t n1  = tree_add_child(&tree, tree_add(&tree));
	const tnode_t n2  = tree_add_child(&tree, 0);
	const tnode_t n12 = tree_add_child(&tree, n1);

	EXPECT_NE(tree_get_child(&tree, 0), TREE_END);

	tree_free(&tree);

	END;
}

TEST(t_tree_get_child_data)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	*(int *)tree_get_data(&tree, tree_add(&tree))	       = 1;
	*(int *)tree_get_data(&tree, tree_add_child(&tree, 0)) = 2;

	EXPECT_EQ(*(int *)tree_get_data(&tree, 0), 1);
	EXPECT_EQ(*(int *)tree_get_data(&tree, 1), 2);

	tree_free(&tree);

	END;
}

TEST(t_tree_get_next)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

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

TEST(t_tree_get)
{
	SSTART;
	RUN(t_tree_get_data);
	RUN(t_tree_has_child);
	RUN(t_tree_get_child);
	RUN(t_tree_get_child_data);
	RUN(t_tree_get_next);
	SEND;
}

TEST(t_tree_remove)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	EXPECT_EQ(tree_remove(NULL, TREE_END), 1);
	EXPECT_EQ(tree_remove(&tree, TREE_END), 1);

	tree_free(&tree);

	END;
}

TEST(t_tree_remove_next)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	const tnode_t n1 = tree_add_child(&tree, tree_add(&tree));
	const tnode_t n2 = tree_add_child(&tree, 0);
	const tnode_t n3 = tree_add_child(&tree, 0);

	EXPECT_EQ(tree_remove(&tree, n2), 0);

	EXPECT_EQ(tree_get_next(&tree, n1), n3);

	tree_free(&tree);

	END;
}

TEST(t_tree_remove_child)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	const tnode_t n1 = tree_add_child(&tree, tree_add(&tree));
	const tnode_t n2 = tree_add_child(&tree, 0);
	const tnode_t n3 = tree_add_child(&tree, 0);

	EXPECT_EQ(tree_remove(&tree, n1), 0);

	EXPECT_EQ(tree_get_child(&tree, 0), n2);

	tree_free(&tree);

	END;
}

TEST(t_tree_removes)
{
	SSTART;
	RUN(t_tree_remove);
	RUN(t_tree_remove_next);
	RUN(t_tree_remove_child);
	SEND;
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

TEST(t_tree_iterate_pre_root)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	tree_add(&tree);

	int cnt = 0;

	EXPECT_EQ(tree_iterate_pre(NULL, TREE_END, NULL, 0, &cnt), 0);
	EXPECT_EQ(tree_iterate_pre(&tree, TREE_END, NULL, 0, &cnt), 0);
	EXPECT_EQ(tree_iterate_pre(&tree, 0, NULL, 0, &cnt), 0);
	EXPECT_EQ(tree_iterate_pre(&tree, 0, iterate_pre_root_cb, 0, &cnt), 0);

	EXPECT_EQ(cnt, 1);

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

TEST(t_tree_iterate_pre_child)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

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

TEST(t_tree_iterate_pre_childs)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

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

TEST(t_tree_iterate_pre_grand_child)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

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

TEST(t_tree_iterate_childs_root)
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

TEST(t_tree_iterate_childs_child)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

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

TEST(t_tree_iterate_childs_childs)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

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

TEST(t_tree_iterate_childs_grand_child)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

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

TEST(t_tree_iterate)
{
	SSTART;
	RUN(t_tree_iterate_pre_root);
	RUN(t_tree_iterate_pre_child);
	RUN(t_tree_iterate_pre_childs);
	RUN(t_tree_iterate_pre_grand_child);
	RUN(t_tree_iterate_childs_root);
	RUN(t_tree_iterate_childs_child);
	RUN(t_tree_iterate_childs_childs);
	RUN(t_tree_iterate_childs_grand_child);
	SEND;
}

TEST(t_tree_it_begin)
{
	SSTART;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	tnode_t node = { 0 };

	tree_it_begin(NULL, TREE_END);
	tree_it_begin(&tree, TREE_END);
	tree_it_begin(&tree, node);

	tree_free(&tree);

	SEND;
}

TEST(t_tree_it_next)
{
	SSTART;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	tree_it it = { 0 };

	tree_it_next(NULL);
	tree_it_next(&it);

	tree_free(&tree);

	SEND;
}

TEST(t_tree_foreach_root)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	tnode_t root;
	*(int *)tree_get_data(&tree, root = tree_add(&tree)) = 0;

	tnode_t node;
	int depth;

	int i = 0;
	tree_foreach(NULL, TREE_END, node, depth)
	{
		i++;
	}

	tree_foreach(&tree, TREE_END, node, depth)
	{
		i++;
	}

	tree_foreach(&tree, 0, node, depth)
	{
		const int *value = tree_get_data(&tree, node);
		EXPECT_EQ(*value, i);
		EXPECT_EQ(depth, 0);
		i++;
	}

	EXPECT_EQ(i, 1);

	tree_free(&tree);

	END;
}

TEST(t_tree_foreach_child)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	tnode_t root;
	*(int *)tree_get_data(&tree, root = tree_add(&tree))	  = 0;
	*(int *)tree_get_data(&tree, tree_add_child(&tree, root)) = 1;

	tnode_t node;
	int depth;

	int i = 0;
	tree_foreach(&tree, 0, node, depth)
	{
		const int *value = tree_get_data(&tree, node);
		EXPECT_EQ(*value, i);
		EXPECT_EQ(depth, i);
		i++;
	}

	EXPECT_EQ(i, 2);

	tree_free(&tree);

	END;
}

TEST(t_tree_foreach_childs)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	tnode_t root;
	*(int *)tree_get_data(&tree, root = tree_add(&tree))	  = 0;
	*(int *)tree_get_data(&tree, tree_add_child(&tree, root)) = 1;
	*(int *)tree_get_data(&tree, tree_add_child(&tree, root)) = 2;

	tnode_t node;
	int depth;

	int i = 0;
	tree_foreach(&tree, 0, node, depth)
	{
		const int *value = tree_get_data(&tree, node);
		EXPECT_EQ(*value, i);
		EXPECT_EQ(depth, i != 0);
		i++;
	}

	EXPECT_EQ(i, 3);

	tree_free(&tree);

	END;
}

TEST(t_tree_foreach_grand_child)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	tnode_t root, n1;
	*(int *)tree_get_data(&tree, root = tree_add(&tree))	       = 0;
	*(int *)tree_get_data(&tree, n1 = tree_add_child(&tree, root)) = 1;
	*(int *)tree_get_data(&tree, tree_add_child(&tree, n1))	       = 2;

	tnode_t node;
	int depth;

	int i = 0;
	tree_foreach(&tree, 0, node, depth)
	{
		const int *value = tree_get_data(&tree, node);
		EXPECT_EQ(*value, i);
		EXPECT_EQ(depth, i);
		i++;
	}

	EXPECT_EQ(i, 3);

	tree_free(&tree);

	END;
}

TEST(t_tree_foreach_child_root)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	tree_add(&tree);

	tnode_t node;

	int i = 0;
	tree_foreach_child(&tree, 0, node)
	{
		const int *value = tree_get_data(&tree, node);
		EXPECT_EQ(*value, i);
		i++;
	}

	EXPECT_EQ(i, 0);

	tree_free(&tree);

	END;
}

TEST(t_tree_foreach_child_child)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	*(int *)tree_get_data(&tree, tree_add_child(&tree, tree_add(&tree))) = 0;

	tnode_t node;

	int i = 0;
	tree_foreach_child(&tree, 0, node)
	{
		const int *value = tree_get_data(&tree, node);
		EXPECT_EQ(*value, i);
		i++;
	}

	EXPECT_EQ(i, 1);

	tree_free(&tree);

	END;
}

TEST(t_tree_foreach_child_childs)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	*(int *)tree_get_data(&tree, tree_add_child(&tree, tree_add(&tree))) = 0;
	*(int *)tree_get_data(&tree, tree_add_child(&tree, 0))		     = 1;

	tnode_t node;

	int i = 0;
	tree_foreach_child(&tree, 0, node)
	{
		const int *value = tree_get_data(&tree, node);
		EXPECT_EQ(*value, i);
		i++;
	}

	EXPECT_EQ(i, 2);

	tree_free(&tree);

	END;
}

TEST(t_tree_foreach_child_grand_child)
{
	START;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	tnode_t n1;
	*(int *)tree_get_data(&tree, n1 = tree_add_child(&tree, tree_add(&tree))) = 0;
	*(int *)tree_get_data(&tree, tree_add_child(&tree, 0))			  = 1;
	*(int *)tree_get_data(&tree, tree_add_child(&tree, n1))			  = 2;

	tnode_t node;

	int i = 0;
	tree_foreach_child(&tree, 0, node)
	{
		const int *value = tree_get_data(&tree, node);
		EXPECT_EQ(*value, i);
		i++;
	}

	EXPECT_EQ(i, 2);

	tree_free(&tree);

	END;
}

TEST(t_tree_foreach)
{
	SSTART;
	RUN(t_tree_it_begin);
	RUN(t_tree_it_next);
	RUN(t_tree_foreach_root);
	RUN(t_tree_foreach_child);
	RUN(t_tree_foreach_childs);
	RUN(t_tree_foreach_grand_child);
	RUN(t_tree_foreach_child_root);
	RUN(t_tree_foreach_child_child);
	RUN(t_tree_foreach_child_childs);
	RUN(t_tree_foreach_child_grand_child);
	SEND;
}

static int print_tree(FILE *file, void *data, int ret)
{
	c_fprintf(file, "%d\n", *(int *)data);
	return ret;
}

TEST(t_tree_print, FILE *file)
{
	SSTART;

	tree_t tree = { 0 };
	tree_init(&tree, 1, sizeof(int));

	tnode_t root;
	*(int *)tree_get_data(&tree, root = tree_add(&tree)) = 0;

	tnode_t n1, n2, n11, n111;

	*(int *)tree_get_data(&tree, (n1 = tree_add_child(&tree, 0))) = 1;
	*(int *)tree_get_data(&tree, (n2 = tree_add_child(&tree, 0))) = 2;

	*(int *)tree_get_data(&tree, (n11 = tree_add_child(&tree, n1))) = 11;

	*(int *)tree_get_data(&tree, (n111 = tree_add_child(&tree, n11))) = 111;

	EXPECT_EQ(tree_print(NULL, TREE_END, NULL, NULL, 0), 0);
	EXPECT_EQ(tree_print(&tree, TREE_END, NULL, NULL, 0), 0);
	EXPECT_EQ(tree_print(&tree, root, NULL, NULL, 0), 0);

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(tree_print(&tree, root, file, NULL, 0), 0);
		EXPECT_EQ(tree_print(&tree, root, file, print_tree, 0), 0);

		char buf[128] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "0\n"
				   "├─1\n"
				   "│ └─11\n"
				   "│   └─111\n"
				   "└─2\n";
		EXPECT_STR(buf, exp);
	}

	tree_free(&tree);

	SEND;
}

STEST(t_tree)
{
	SSTART;

	FILE *file = file_open(TEST_FILE, "wb+");

	RUN(t_tree_init_free);
	RUN(t_tree_adds);
	RUN(t_tree_get);
	RUN(t_tree_removes);
	RUN(t_tree_iterate);
	RUN(t_tree_foreach);
	RUN(t_tree_print, file);

	file_close(file);
	file_delete(TEST_FILE);

	SEND;
}
