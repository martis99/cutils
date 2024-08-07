#include "cutils.h"

#include "test.h"

TEST(t_c_init)
{
	START;

	const mem_t *mem = mem_get();
	const log_t *log = log_get();

	EXPECT_EQ(c_init(NULL), NULL);

	cutils_t cu = { 0 };
	EXPECT_EQ(c_init(&cu), &cu);

	mem_sset((mem_t *)mem);
	log_set((log_t *)log);

	END;
}

TEST(t_c_free)
{
	START;

	cutils_t cutils = { 0 };

	char buf[256] = { 0 };
	EXPECT_EQ(c_free(NULL, PRINT_DST_BUF(buf, sizeof(buf), 0)), 1);

	EXPECT_EQ(c_free(&cutils, PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);

	mem_t *mem = (mem_t *)mem_get();

	size_t m = mem->mem;

	mem->mem = 1;
	EXPECT_EQ(c_free(&cutils, PRINT_DST_BUF(buf, sizeof(buf), 0)), 1);
	mem->mem = m;

	END;
}

STEST(t_cutils)
{
	SSTART;

	const mem_t *mem = mem_get();
	const log_t *log = log_get();

	cutils_t cu = { 0 };
	c_init(&cu);
	cu.cplatform.log = *log;

	RUN(t_c_init);
	RUN(t_c_free);

	mem_sset((mem_t *)mem);
	log_set((log_t *)log);

	SEND;
}
