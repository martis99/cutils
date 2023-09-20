#include "t_dict.h"

#include "dict.h"

#include "test.h"

TEST(t_dict_init_free)
{
	START;

	dict_t dict = { 0 };

	EXPECT_EQ(dict_init(NULL, 0), NULL);
	EXPECT_EQ(dict_init(&dict, 0), &dict);

	dict_free(NULL);
	dict_free(&dict);

	END;
}

TEST(t_dict_set_get)
{
	START;

	dict_t dict = { 0 };

	EXPECT_EQ(dict_init(&dict, 2), &dict);

	dict_set(NULL, NULL, 0, NULL);
	dict_set(&dict, "one", 3, "1");
	dict_set(&dict, "two", 3, "2");
	dict_set(&dict, "three", 5, "3");
	dict_set(&dict, "three", 5, "3");
	dict_set(&dict, "abcdef", 6, "4");
	dict_set(&dict, "abcdefg", 7, "5");
	dict_set(&dict, "abcdefgh", 8, "6");

	char *val = NULL;

	EXPECT_EQ(dict_get(NULL, NULL, 0, NULL), 1);
	EXPECT_EQ(dict_get(&dict, "one", 3, (void **)&val), 0);
	EXPECT_STR(val, "1");

	dict_free(&dict);

	END;
}

TEST(t_dict_foreach)
{
	START;

	dict_t dict = { 0 };

	EXPECT_EQ(dict_init(&dict, 4), &dict);

	dict_set(&dict, "one", 3, "1");
	dict_set(&dict, "two", 3, "2");

	char *val = NULL;

	int i = 0;
	dict_foreach(&dict, pair)
	{
		const char *exp = NULL;
		switch (i) {
		case 0: exp = "1"; break;
		case 1: exp = "2"; break;
		}

		EXPECT_STR(pair->value, exp);

		i++;
	}

	EXPECT_EQ(i, 2);

	dict_free(&dict);

	END;
}

STEST(t_dict)
{
	SSTART;
	RUN(t_dict_init_free);
	RUN(t_dict_set_get);
	RUN(t_dict_foreach);
	SEND;
}
