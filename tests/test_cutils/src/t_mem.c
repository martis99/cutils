#include "t_mem.h"

#include "mem.h"

#include "test.h"

TEST(t_mem_get_stats)
{
	START;

	EXPECT_NE(mem_get_stats(), NULL);

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

STEST(t_mem)
{
	SSTART;
	RUN(t_mem_get_stats);
	RUN(t_mem_alloc);
	RUN(t_mem_calloc);
	RUN(t_mem_realloc);
	RUN(t_mem_set);
	RUN(t_mem_cpy);
	RUN(t_mem_cmp);
	SEND;
}
