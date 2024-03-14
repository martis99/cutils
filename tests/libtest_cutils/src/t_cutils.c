#include "t_cutils_c.h"

#include "cutils.h"
#include "file.h"

#define TEST_FILE "t_cutils.txt"

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

TESTP(t_c_free, FILE *file)
{
	START;

	cutils_t cutils = { 0 };

	EXPECT_EQ(c_free(NULL, NULL), 1);

	file_reopen(TEST_FILE, "r", file);
	EXPECT_EQ(c_free(&cutils, file), 1);

	file_reopen(TEST_FILE, "wb+", file);
	EXPECT_EQ(c_free(&cutils, file), 0);

	END;
}

STEST(t_cutils)
{
	SSTART;

	FILE *file = file_open(TEST_FILE, "wb+");

	const mem_t *mem = mem_get();
	const log_t *log = log_get();

	cutils_t cu = { 0 };
	c_init(&cu);
	cu.log = *log;

	RUN(t_c_init);
	RUNP(t_c_free, file);

	file_close(file);
	file_delete(TEST_FILE);

	mem_sset((mem_t *)mem);
	log_set((log_t *)log);

	SEND;
}
