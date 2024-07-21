#include "mem.h"

#include "test.h"

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

TEST(t_mem_print)
{
	START;

	mem_t *mem = (mem_t *)mem_get();

	size_t peak = mem->peak;

	char buf[64] = { 0 };

	mem->peak = 10;
	EXPECT_GT(mem_print(PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);

	mem->peak = 1024 + 10;
	EXPECT_GT(mem_print(PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);

	mem->peak = 1024 * 1024 + 10;
	EXPECT_GT(mem_print(PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);

	mem->peak = peak;

	mem_sset(NULL);
	EXPECT_EQ(mem_print(PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);
	mem_sset(mem);

	END;
}

TEST(t_mem_check)
{
	START;

	mem_t *mem = (mem_t *)mem_get();

	size_t m = mem->mem;

	EXPECT_EQ(mem_check(), 0);
	mem->mem = 1;
	EXPECT_EQ(mem_check(), 1);
	mem->mem = m;

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

	const mem_t *mem = mem_get();

	mem_t mm = { 0 };
	mem_init(&mm);

	RUNP(t_mem_init, &mm);
	RUN(t_mem_get_stats);
	RUN(t_mem_print);
	RUN(t_mem_check);
	RUN(t_mem_alloc);
	RUN(t_mem_calloc);
	RUN(t_mem_realloc);
	RUN(t_mem_set);
	RUN(t_mem_cpy);
	RUN(t_mem_cmp);
	RUN(t_mem_swap);
	RUN(t_mem_oom);

	mem_sset((mem_t *)mem);

	SEND;
}
