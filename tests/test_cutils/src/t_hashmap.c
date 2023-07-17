#include "t_hashmap.h"

#include "hashmap.h"

#include "test.h"

TEST(t_hashmap_init_null)
{
	START;

	EXPECT_EQ(hashmap_init(NULL, 0), NULL);
	hashmap_free(NULL);

	END;
}

TEST(t_hashmap_set_null)
{
	START;
	hashmap_set(NULL, NULL, 0, NULL);
	END;
}

TEST(t_hashmap_get_null)
{
	START;
	EXPECT_EQ(hashmap_get(NULL, NULL, 0, NULL), 1);
	END;
}

TEST(t_hashmap_set)
{
	START;

	hashmap_t hashmap = { 0 };

	EXPECT_EQ(hashmap_init(&hashmap, 4), &hashmap);

	hashmap_set(&hashmap, "one", 3, "1");

	char *val = NULL;

	EXPECT_EQ(hashmap_get(&hashmap, "one", 3, (void **)&val), 0);
	EXPECT_STR(val, "1");

	hashmap_free(&hashmap);

	END;
}

STEST(t_hashmap)
{
	SSTART;
	RUN(t_hashmap_init_null);
	RUN(t_hashmap_set_null);
	RUN(t_hashmap_get_null);
	RUN(t_hashmap_set);
	SEND;
}
