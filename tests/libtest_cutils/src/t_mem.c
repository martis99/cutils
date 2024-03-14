#include "t_cutils_c.h"

#include "file.h"
#include "mem.h"

#define TEST_FILE "t_mem.txt"

TESTP(t_mem_init, mem_t *mem)
{
	START;

	EXPECT_EQ(mem_init(NULL), NULL);
	EXPECT_EQ(mem_init(mem), mem);

	END;
}

TEST(t_mem_get_stats)
{
	START;

	EXPECT_NE(mem_get(), NULL);

	END;
}

TESTP(t_mem_print, FILE *file)
{
	START;

	mem_t *mem = (mem_t *)mem_get();

	size_t mem_max = mem->mem_max;

	mem->mem_max = 10;

	EXPECT_EQ(mem_print(NULL), 0);

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_GT(mem_print(file), 0);
	}

	mem->mem_max = 1024 + 10;

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_GT(mem_print(file), 0);
	}

	mem->mem_max = 1024 * 1024 + 10;

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_GT(mem_print(file), 0);
	}

	mem->mem_max = mem_max;

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

	EXPECT_EQ(mem_realloc(NULL, 0, 0), NULL);

	void *ptr  = mem_alloc(0);
	void *prev = ptr;
	EXPECT_EQ(ptr = mem_realloc(ptr, 0, 0), prev);
	EXPECT_NE(ptr = mem_realloc(ptr, 1, 0), NULL);
	EXPECT_NE(ptr = mem_realloc(ptr, 0, 1), NULL);
	EXPECT_NE(ptr = mem_realloc(ptr, 1, 0), NULL);
	EXPECT_NE(ptr = mem_realloc(ptr, 1, 1), NULL);
	EXPECT_NE(ptr = mem_realloc(ptr, 2, 1), NULL);
	EXPECT_NE(ptr = mem_realloc(ptr, 1, 2), NULL);
	mem_free(ptr, 1);

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
	EXPECT_EQ(mem_cpy(dst, 0, src, 1), NULL);
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

	const mem_t *mem = mem_get();

	mem_t mm = { 0 };
	mem_init(&mm);

	RUNP(t_mem_init, &mm);
	RUN(t_mem_get_stats);
	RUNP(t_mem_print, file);
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

	mem_sset((mem_t *)mem);

	SEND;
}
