#include "t_cutils_c.h"

#include "cplatform.h"

TEST(t_cplatform_init)
{
	START;

	cplatform_t cplatform = { 0 };
	EXPECT_EQ(cplatform_init(NULL), NULL);
	EXPECT_EQ(cplatform_init(&cplatform), &cplatform);

	END;
}

TEST(t_cplatform_free)
{
	START;

	cplatform_t cplatform = { 0 };
	EXPECT_EQ(cplatform_free(NULL), 1);
	EXPECT_EQ(cplatform_free(&cplatform), 0);

	END;
}

STEST(t_cplatform)
{
	SSTART;

	const log_t *log = log_get();

	RUN(t_cplatform_init);
	RUN(t_cplatform_free);

	log_set((log_t *)log);

	SEND;
}
