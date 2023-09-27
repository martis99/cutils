#include "t_mem.h"

#include "file.h"
#include "mem.h"

#include "test.h"

#define TEST_FILE "t_mem.txt"

TEST(t_mem_get_stats)
{
	START;

	EXPECT_NE(mem_get_stats(), NULL);

	END;
}

TEST(t_mem_print, FILE *file)
{
	START;

	mem_stats_t *mem_stats = (mem_stats_t *)mem_get_stats();

	size_t mem_max = mem_stats->mem_max;

	mem_stats->mem_max = 10;

	{
		file_reopen(TEST_FILE, "wb+", file);
		mem_print(file);
	}

	mem_stats->mem_max = 1024 + 10;

	{
		file_reopen(TEST_FILE, "wb+", file);
		mem_print(file);
	}

	mem_stats->mem_max = 1024 * 1024 + 10;

	{
		file_reopen(TEST_FILE, "wb+", file);
		mem_print(file);
	}

	mem_stats->mem_max = mem_max;

	END;
}

TEST(t_mem_alloc)
{
	START;

	void *data = mem_alloc(0);

	EXPECT_NE(data, NULL);

	mem_free(data, 0);

	END;
}

TEST(t_mem_calloc)
{
	START;

	void *data = mem_calloc(0, 0);

	EXPECT_NE(data, NULL);

	mem_free(data, 0);

	END;
}

TEST(t_mem_realloc)
{
	START;

	EXPECT_NE(mem_realloc(NULL, 0, 0), NULL);

	END;
}

TEST(t_mem_set)
{
	START;

	void *data = mem_alloc(0);

	EXPECT_EQ(mem_set(NULL, 0, 0), NULL);
	EXPECT_NE(mem_set(data, 0, 0), NULL);

	mem_free(data, 0);

	END;
}

TEST(t_mem_cpy)
{
	START;

	void *src = mem_alloc(0);
	void *dst = mem_alloc(0);

	EXPECT_EQ(mem_cpy(NULL, 0, NULL, 0), NULL);
	EXPECT_NE(mem_cpy(dst, 0, src, 0), NULL);

	mem_free(src, 0);
	mem_free(dst, 0);

	END;
}

TEST(t_mem_cmp)
{
	START;

	void *l = mem_alloc(0);
	void *r = mem_alloc(0);

	EXPECT_EQ(mem_cmp(NULL, NULL, 0), 0);
	EXPECT_EQ(mem_cmp(l, r, 0), 0);

	mem_free(l, 0);
	mem_free(r, 0);

	END;
}

TEST(t_mem_swap)
{
	START;

	char a[] = "abc";
	char b[] = "def";

	EXPECT_EQ(mem_swap(NULL, NULL, 0), 1);
	EXPECT_EQ(mem_swap(a, NULL, 0), 1);
	EXPECT_EQ(mem_swap(a, b, sizeof(a)), 0);

	EXPECT_STR(a, "def");
	EXPECT_STR(b, "abc");

	END;
}

TEST(t_mem_oom)
{
	START;

	mem_oom(1);
	mem_oom(0);

	END;
}

STEST(t_mem)
{
	SSTART;

	FILE *file = file_open(TEST_FILE, "wb+");

	RUN(t_mem_get_stats);
	RUN(t_mem_print, file);
	RUN(t_mem_alloc);
	RUN(t_mem_calloc);
	RUN(t_mem_realloc);
	RUN(t_mem_set);
	RUN(t_mem_cpy);
	RUN(t_mem_cmp);
	RUN(t_mem_swap);
	RUN(t_mem_oom);

	file_close(file);
	file_delete(TEST_FILE);

	SEND;
}
