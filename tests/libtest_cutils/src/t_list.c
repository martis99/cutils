#include "t_cutils_c.h"

#include "file.h"
#include "list.h"
#include "mem.h"
#include "print.h"

#define TEST_FILE "t_list.txt"

TEST(t_list_init_free)
{
	START;

	list_t list = { 0 };

	EXPECT_EQ(list_init(NULL, 0, sizeof(int)), NULL);
	mem_oom(1);
	EXPECT_EQ(list_init(&list, 1, sizeof(int)), NULL);
	mem_oom(0);
	EXPECT_EQ(list_init(&list, 1, sizeof(int)), &list);

	EXPECT_NE(list.data, NULL);
	EXPECT_EQ(list.cap, 1);
	EXPECT_EQ(list.cnt, 0);
	EXPECT_NE(list.size, 0);

	list_free(&list);
	list_free(NULL);

	EXPECT_EQ(list.data, NULL);
	EXPECT_EQ(list.cap, 0);
	EXPECT_EQ(list.cnt, 0);
	EXPECT_EQ(list.size, 0);

	END;
}

TEST(t_list_add)
{
	START;

	list_t list = { 0 };
	list_init(&list, 1, sizeof(int));

	EXPECT_EQ(list_add(NULL), LIST_END);
	EXPECT_EQ(list_add(&list), 0);

	EXPECT_EQ(list.cnt, 1);
	EXPECT_EQ(list.cap, 1);

	list_free(&list);

	END;
}

TEST(t_list_adds)
{
	START;

	list_t list = { 0 };
	list_init(&list, 1, sizeof(int));

	EXPECT_EQ(list_add(&list), 0);
	EXPECT_EQ(list_add(&list), 1);

	EXPECT_EQ(list.cnt, 2);
	EXPECT_EQ(list.cap, 2);

	list_free(&list);

	END;
}

TEST(t_list_remove)
{
	START;

	list_t list = { 0 };
	list_init(&list, 1, sizeof(int));

	const lnode_t node = list_add(&list);

	EXPECT_EQ(list_remove(NULL, LIST_END), 1);
	EXPECT_EQ(list_remove(&list, LIST_END), 1);
	EXPECT_EQ(list_remove(&list, node), 0);

	EXPECT_EQ(list.cnt, 1);

	list_free(&list);

	END;
}

TEST(t_list_remove_middle)
{
	START;

	list_t list = { 0 };
	list_init(&list, 1, sizeof(int));

	const lnode_t node = list_add(&list);
	const lnode_t n1   = list_add_next(&list, node);
	const lnode_t n2   = list_add_next(&list, node);

	list_remove(&list, n1);

	EXPECT_EQ(list_get_next(&list, node), n2);

	list_free(&list);

	END;
}

TEST(t_list_remove_last)
{
	START;

	list_t list = { 0 };
	list_init(&list, 1, sizeof(int));

	const lnode_t node = list_add(&list);
	const lnode_t n1   = list_add_next(&list, node);
	const lnode_t n2   = list_add_next(&list, node);

	list_remove(&list, n2);

	EXPECT_EQ(list_get_next(&list, n1), LIST_END);

	list_free(&list);

	END;
}

TEST(t_list_add_remove)
{
	SSTART;
	RUN(t_list_add);
	RUN(t_list_adds);
	RUN(t_list_remove);
	RUN(t_list_remove_middle);
	RUN(t_list_remove_last);
	SEND;
}

TEST(t_list_add_next)
{
	START;

	list_t list = { 0 };
	list_init(&list, 1, sizeof(int));

	EXPECT_EQ(list_add_next(NULL, LIST_END), LIST_END);
	EXPECT_EQ(list_add_next(&list, LIST_END), LIST_END);
	EXPECT_EQ(list_add_next(&list, list_add(&list)), 1);

	EXPECT_EQ(list.cnt, 2);
	EXPECT_EQ(list.cap, 2);

	list_free(&list);

	END;
}

TEST(t_list_add_nexts)
{
	START;

	list_t list = { 0 };
	list_init(&list, 1, sizeof(int));

	const lnode_t node  = list_add(&list);
	const lnode_t next1 = list_add_next(&list, node);
	const lnode_t next2 = list_add_next(&list, node);

	EXPECT_EQ(next1, 1);
	EXPECT_EQ(next2, 2);
	EXPECT_EQ(list.cnt, 3);
	EXPECT_EQ(list.cap, 4);

	list_free(&list);

	END;
}

TEST(t_list_add_and_next)
{
	START;

	list_t list = { 0 };
	list_init(&list, 1, sizeof(int));

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

TEST(t_list_set_next)
{
	START;

	list_t list = { 0 };
	list_init(&list, 1, sizeof(int));

	lnode_t node = list_add(&list);

	EXPECT_EQ(list_set_next(NULL, LIST_END, LIST_END), LIST_END);
	EXPECT_EQ(list_set_next(&list, LIST_END, LIST_END), LIST_END);
	EXPECT_EQ(list_set_next(&list, node, LIST_END), LIST_END);
	EXPECT_EQ(list_set_next(&list, node, list_add(&list)), 1);

	EXPECT_EQ(list.cnt, 2);
	EXPECT_EQ(list.cap, 2);

	list_free(&list);

	END;
}

TEST(t_list_get_next)
{
	START;

	list_t list = { 0 };
	list_init(&list, 1, sizeof(int));

	const lnode_t node = list_add(&list);
	const lnode_t next = list_add_next(&list, node);

	EXPECT_EQ(list_get_next(NULL, LIST_END), LIST_END);
	EXPECT_EQ(list_get_next(&list, LIST_END), LIST_END);
	EXPECT_EQ(list_get_next(&list, node), next);

	list_free(&list);

	END;
}

TEST(t_list_next)
{
	SSTART;
	RUN(t_list_add_next);
	RUN(t_list_add_nexts);
	RUN(t_list_add_and_next);
	RUN(t_list_set_next);
	RUN(t_list_get_next);
	SEND;
}

TEST(t_list_get_data)
{
	START;

	list_t list = { 0 };
	list_init(&list, 1, sizeof(int));

	const lnode_t node = list_add(&list);

	EXPECT_EQ(list_get_data(NULL, LIST_END), NULL);
	EXPECT_EQ(list_get_data(&list, LIST_END), NULL);
	*(int *)list_get_data(&list, node) = 8;

	EXPECT_EQ(list.cnt, 1);
	EXPECT_EQ(list.cap, 1);
	EXPECT_EQ(*(int *)list_get_data(&list, node), 8);

	list_free(&list);

	END;
}

TEST(t_list_foreach)
{
	START;

	list_t list = { 0 };
	list_init(&list, 1, sizeof(int));

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

TEST(t_list_foreach_all)
{
	START;

	list_t list = { 0 };
	list_init(&list, 1, sizeof(int));

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

TEST(t_list_foreachs)
{
	SSTART;
	RUN(t_list_foreach);
	RUN(t_list_foreach_all);
	SEND;
}

static int print_list(FILE *file, void *data, int ret)
{
	c_fprintf(file, "%d\n", *(int *)data);
	return ret;
}

TESTP(t_list_print, FILE *file)
{
	START;

	list_t list = { 0 };
	list_init(&list, 1, sizeof(int));

	const lnode_t node = list_add(&list);

	*(int *)list_get_data(&list, node)			 = 0;
	*(int *)list_get_data(&list, list_add_next(&list, node)) = 1;
	*(int *)list_get_data(&list, list_add_next(&list, node)) = 2;

	EXPECT_EQ(list_print(NULL, LIST_END, NULL, NULL, 0), 0);
	EXPECT_EQ(list_print(&list, LIST_END, NULL, NULL, 0), 0);
	EXPECT_EQ(list_print(&list, node, NULL, NULL, 0), 0);

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(list_print(&list, node, file, NULL, 0), 0);
		EXPECT_EQ(list_print(&list, node, file, print_list, 0), 0);

		char buf[128] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "0\n"
				   "1\n"
				   "2\n";
		EXPECT_STR(buf, exp);
	}

	list_free(&list);

	END;
}

STEST(t_list)
{
	SSTART;

	FILE *file = file_open(TEST_FILE, "wb+");

	RUN(t_list_init_free);
	RUN(t_list_add_remove);
	RUN(t_list_next);
	RUN(t_list_get_data);
	RUN(t_list_foreachs);
	RUNP(t_list_print, file);

	file_close(file);
	file_delete(TEST_FILE);

	SEND;
}
