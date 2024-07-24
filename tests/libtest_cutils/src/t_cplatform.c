#include "cplatform.h"

#include "test.h"

TEST(t_cplatform_init)
{
	START;

	const log_t *log = log_get();
	const mem_t *mem = mem_get();

	cplatform_t cplatform = { 0 };
	EXPECT_EQ(cplatform_init(NULL), NULL);
	EXPECT_EQ(cplatform_init(&cplatform), &cplatform);

	log_set((log_t *)log);
	mem_sset((mem_t *)mem);

	END;
}

TEST(t_cplatform_free)
{
	START;

	const log_t *log = log_get();
	const mem_t *mem = mem_get();

	cplatform_t cplatform = { 0 };
	EXPECT_EQ(cplatform_free(NULL), 1);
	EXPECT_EQ(cplatform_free(&cplatform), 0);

	log_set((log_t *)log);
	mem_sset((mem_t *)mem);

	END;
}

STEST(t_cplatform)
{
	SSTART;

	RUN(t_cplatform_init);
	RUN(t_cplatform_free);

	SEND;
}
