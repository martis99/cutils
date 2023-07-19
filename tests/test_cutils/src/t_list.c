#include "t_list.h"

#include "list.h"

#include "test.h"

TEST(init_free)
{
	START;

	list_t list = { 0 };

	list_init(&list, 1, sizeof(int));

	EXPECT_NE(list.data, NULL);
	EXPECT_EQ(list.cap, 1);
	EXPECT_EQ(list.cnt, 0);
	EXPECT_NE(list.size, 0);

	list_free(&list);

	EXPECT_EQ(list.data, NULL);
	EXPECT_EQ(list.cap, 0);
	EXPECT_EQ(list.cnt, 0);
	EXPECT_EQ(list.size, 0);

	END;
}

TEST(set_data)
{
	START;

	list_t list = { 0 };

	list_init(&list, 1, sizeof(int));

	const lnode_t node = list_add(&list);

	*(int *)list_get_data(&list, node) = 8;

	EXPECT_EQ(list.cnt, 1);
	EXPECT_EQ(list.cap, 1);
	EXPECT_EQ(*(int *)list_get_data(&list, node), 8);

	list_free(&list);

	END;
}

TEST(add)
{
	START;

	list_t list = { 0 };

	list_init(&list, 1, sizeof(int));

	const lnode_t node = list_add(&list);

	EXPECT_EQ(node, 0);
	EXPECT_EQ(list.cnt, 1);
	EXPECT_EQ(list.cap, 1);

	list_free(&list);

	END;
}

TEST(adds)
{
	START;

	list_t list = { 0 };

	list_init(&list, 2, sizeof(int));

	const lnode_t n1 = list_add(&list);
	const lnode_t n2 = list_add(&list);

	EXPECT_EQ(n1, 0);
	EXPECT_EQ(n2, 1);
	EXPECT_EQ(list.cnt, 2);
	EXPECT_EQ(list.cap, 2);

	list_free(&list);

	END;
}

TEST(add_next)
{
	START;

	list_t list = { 0 };

	list_init(&list, 2, sizeof(int));

	const lnode_t node = list_add(&list);
	const lnode_t next = list_add_next(&list, node);

	EXPECT_EQ(next, 1);
	EXPECT_EQ(list.cnt, 2);
	EXPECT_EQ(list.cap, 2);

	list_free(&list);

	END;
}

TEST(add_nexts)
{
	START;

	list_t list = { 0 };

	list_init(&list, 3, sizeof(int));

	const lnode_t node  = list_add(&list);
	const lnode_t next1 = list_add_next(&list, node);
	const lnode_t next2 = list_add_next(&list, node);

	EXPECT_EQ(next1, 1);
	EXPECT_EQ(next2, 2);
	EXPECT_EQ(list.cnt, 3);
	EXPECT_EQ(list.cap, 3);

	list_free(&list);

	END;
}

TEST(add_and_next)
{
	START;

	list_t list = { 0 };

	list_init(&list, 4, sizeof(int));

	const lnode_t n1 = list_add(&list);
	const lnode_t n2 = list_add(&list);

	const lnode_t next1 = list_add_next(&list, n1);
	const lnode_t next2 = list_add_next(&list, n2);

	EXPECT_EQ(next1, 2);
	EXPECT_EQ(next2, 3);
	EXPECT_EQ(list.cnt, 4);
	EXPECT_EQ(list.cap, 4);

	list_free(&list);

	END;
}

TEST(add_realloc)
{
	START;

	list_t list = { 0 };

	list_init(&list, 1, sizeof(int));

	const lnode_t n0 = list_add(&list);
	const lnode_t n1 = list_add(&list);

	*(int *)list_get_data(&list, n0) = 1;
	*(int *)list_get_data(&list, n1) = 2;

	EXPECT_EQ(n0, 0);
	EXPECT_EQ(n1, 1);
	EXPECT_EQ(list.cnt, 2);
	EXPECT_EQ(list.cap, 2);
	EXPECT_EQ(*(int *)list_get_data(&list, n0), 1);
	EXPECT_EQ(*(int *)list_get_data(&list, n1), 2);

	list_free(&list);

	END;
}

TEST(add_next_realloc)
{
	START;

	list_t list = { 0 };

	list_init(&list, 1, sizeof(int));

	const lnode_t node = list_add(&list);
	const lnode_t next = list_add_next(&list, node);

	*(int *)list_get_data(&list, node) = 1;
	*(int *)list_get_data(&list, next) = 2;

	EXPECT_EQ(node, 0);
	EXPECT_EQ(next, 1);
	EXPECT_EQ(list.cnt, 2);
	EXPECT_EQ(list.cap, 2);
	EXPECT_EQ(*(int *)list_get_data(&list, node), 1);
	EXPECT_EQ(*(int *)list_get_data(&list, next), 2);

	list_free(&list);

	END;
}

TEST(get_next)
{
	START;

	list_t list = { 0 };

	list_init(&list, 3, sizeof(int));

	const lnode_t node = list_add(&list);
	const lnode_t n1   = list_add_next(&list, node);
	const lnode_t n2   = list_add_next(&list, node);

	const lnode_t next1 = list_get_next(&list, node);
	const lnode_t next2 = list_get_next(&list, next1);

	EXPECT_EQ(n1, 1);
	EXPECT_EQ(n2, 2);
	EXPECT_EQ(next1, 1);
	EXPECT_EQ(next2, 2);

	list_free(&list);

	END;
}

TEST(remove_middle)
{
	START;

	list_t list = { 0 };

	list_init(&list, 3, sizeof(int));

	const lnode_t node = list_add(&list);
	const lnode_t n1   = list_add_next(&list, node);
	const lnode_t n2   = list_add_next(&list, node);

	list_remove(&list, n1);

	EXPECT_EQ(list_get_next(&list, node), n2);

	list_free(&list);

	END;
}

TEST(remove_last)
{
	START;

	list_t list = { 0 };

	list_init(&list, 3, sizeof(int));

	const lnode_t node = list_add(&list);
	const lnode_t n1   = list_add_next(&list, node);
	const lnode_t n2   = list_add_next(&list, node);

	list_remove(&list, n2);

	EXPECT_EQ(list_get_next(&list, n1), -1);

	list_free(&list);

	END;
}

TEST(foreach)
{
	START;

	list_t list = { 0 };

	list_init(&list, 3, sizeof(int));

	lnode_t node;

	*(int *)list_get_data(&list, node = list_add(&list))	 = 0;
	*(int *)list_get_data(&list, list_add_next(&list, node)) = 1;
	*(int *)list_get_data(&list, list_add_next(&list, node)) = 2;

	int *value;

	int i = 0;
	list_foreach(&list, node, value)
	{
		EXPECT_EQ(*value, i);
		i++;
	}

	EXPECT_EQ(i, 3);

	list_free(&list);

	END;
}

TEST(foreach_all)
{
	START;

	list_t list = { 0 };

	list_init(&list, 3, sizeof(int));

	lnode_t node;

	*(int *)list_get_data(&list, node = list_add(&list))	 = 0;
	*(int *)list_get_data(&list, list_add_next(&list, node)) = 1;
	*(int *)list_get_data(&list, list_add(&list))		 = 2;

	int *value;

	int i = 0;
	list_foreach_all(&list, value)
	{
		EXPECT_EQ(*value, i);
		i++;
	}

	EXPECT_EQ(i, 3);

	list_free(&list);

	END;
}

TEST(addt)
{
	SSTART;
	RUN(add);
	RUN(adds);
	RUN(add_next);
	RUN(add_nexts);
	RUN(add_and_next);
	RUN(add_realloc);
	RUN(add_next_realloc);
	SEND;
}

TEST(get)
{
	SSTART;
	RUN(get_next);
	SEND;
}

TEST(t_remove)
{
	SSTART;
	RUN(remove_middle);
	RUN(remove_last);
	SEND;
}

TEST(t_foreach)
{
	SSTART;
	RUN(foreach);
	RUN(foreach_all);
	SEND;
}

STEST(t_list)
{
	SSTART;
	RUN(init_free);
	RUN(set_data);
	RUN(addt);
	RUN(get);
	RUN(t_remove);
	RUN(t_foreach);
	SEND;
}
