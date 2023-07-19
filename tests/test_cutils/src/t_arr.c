#include "t_arr.h"

#include "arr.h"

#include "test.h"

TEST(init_free)
{
	START;

	arr_t arr = { 0 };

	arr_init(&arr, 1, sizeof(int));

	EXPECT_NE(arr.data, NULL);
	EXPECT_EQ(arr.cap, 1);
	EXPECT_EQ(arr.cnt, 0);
	EXPECT_EQ(arr.size, sizeof(int));

	arr_free(&arr);

	EXPECT_EQ(arr.data, NULL);
	EXPECT_EQ(arr.cap, 0);
	EXPECT_EQ(arr.cnt, 0);
	EXPECT_EQ(arr.size, 0);

	END;
}

TEST(add)
{
	START;

	arr_t arr = { 0 };

	arr_init(&arr, 1, sizeof(int));

	EXPECT_EQ(arr_add(&arr), 0);
	EXPECT_EQ(arr.cnt, 1);
	EXPECT_EQ(arr.cap, 1);

	arr_free(&arr);

	END;
}

TEST(get)
{
	START;

	arr_t arr = { 0 };

	arr_init(&arr, 1, sizeof(int));

	*(int *)arr_get(&arr, arr_add(&arr)) = 1;

	EXPECT_EQ(*(int *)arr_get(&arr, 0), 1);

	arr_free(&arr);

	END;
}

TEST(get_invalid)
{
	START;

	arr_t arr = { 0 };

	arr_init(&arr, 1, sizeof(int));

	EXPECT_EQ(arr_get(&arr, 0), NULL);

	arr_free(&arr);

	END;
}

TEST(set)
{
	START;

	arr_t arr = { 0 };

	arr_init(&arr, 1, sizeof(int));

	int value = 1;

	arr_set(&arr, arr_add(&arr), &value);

	EXPECT_EQ(*(int *)arr_get(&arr, 0), value);

	arr_free(&arr);

	END;
}

TEST(set_invalid_index)
{
	START;

	arr_t arr = { 0 };

	arr_init(&arr, 1, sizeof(int));

	int value = 0;

	EXPECT_EQ(arr_set(&arr, 0, &value), NULL);

	arr_free(&arr);

	END;
}

TEST(set_invalid_value)
{
	START;

	arr_t arr = { 0 };

	arr_init(&arr, 1, sizeof(int));

	EXPECT_EQ(arr_set(&arr, arr_add(&arr), NULL), NULL);

	arr_free(&arr);

	END;
}

TEST(adds)
{
	START;

	arr_t arr = { 0 };

	arr_init(&arr, 2, sizeof(int));

	EXPECT_EQ(arr_add(&arr), 0);
	EXPECT_EQ(arr_add(&arr), 1);
	EXPECT_EQ(arr.cnt, 2);
	EXPECT_EQ(arr.cap, 2);

	arr_free(&arr);

	END;
}

TEST(add_realloc)
{
	START;

	arr_t arr = { 0 };

	arr_init(&arr, 1, sizeof(int));

	*(int *)arr_get(&arr, arr_add(&arr)) = 0;
	*(int *)arr_get(&arr, arr_add(&arr)) = 1;

	EXPECT_EQ(arr.cnt, 2);
	EXPECT_EQ(arr.cap, 2);
	EXPECT_EQ(*(int *)arr_get(&arr, 0), 0);
	EXPECT_EQ(*(int *)arr_get(&arr, 1), 1);

	arr_free(&arr);

	END;
}

TEST(app)
{
	START;

	arr_t arr = { 0 };

	arr_init(&arr, 2, sizeof(int));

	const int v0 = 1;
	const int v1 = 2;

	uint i0 = arr_app(&arr, &v0);
	uint i1 = arr_app(&arr, &v1);

	EXPECT_EQ(i0, 0);
	EXPECT_EQ(i1, 1);
	EXPECT_EQ(*(int *)arr_get(&arr, 0), 1);
	EXPECT_EQ(*(int *)arr_get(&arr, 1), 2);
	EXPECT_EQ(arr.cnt, 2);
	EXPECT_EQ(arr.cap, 2);

	arr_free(&arr);

	END;
}

TEST(index)
{
	START;

	arr_t arr = { 0 };

	arr_init(&arr, 2, sizeof(int));

	const int value0 = 0;
	const int value1 = 1;

	*(int *)arr_get(&arr, arr_add(&arr)) = value0;
	*(int *)arr_get(&arr, arr_add(&arr)) = value1;

	EXPECT_EQ(arr_index(&arr, &value0), 0);
	EXPECT_EQ(arr_index(&arr, &value1), 1);

	arr_free(&arr);

	END;
}

static int index_cmp_cb(const void *value1, const void *value2)
{
	return *(int *)value1 == *(int *)value2;
}

TEST(index_cmp)
{
	START;

	arr_t arr = { 0 };

	arr_init(&arr, 2, sizeof(int));

	const int value0 = 0;
	const int value1 = 1;

	*(int *)arr_get(&arr, arr_add(&arr)) = value0;
	*(int *)arr_get(&arr, arr_add(&arr)) = value1;

	EXPECT_EQ(arr_index_cmp(&arr, &value0, index_cmp_cb), 0);
	EXPECT_EQ(arr_index_cmp(&arr, &value1, index_cmp_cb), 1);

	arr_free(&arr);

	END;
}

TEST(add_all)
{
	START;

	arr_t arr  = { 0 };
	arr_t arr0 = { 0 };
	arr_t arr1 = { 0 };

	arr_init(&arr, 4, sizeof(int));
	arr_init(&arr0, 2, sizeof(int));
	arr_init(&arr1, 2, sizeof(int));

	*(int *)arr_get(&arr0, arr_add(&arr0)) = 0;
	*(int *)arr_get(&arr0, arr_add(&arr0)) = 1;
	*(int *)arr_get(&arr1, arr_add(&arr1)) = 1;
	*(int *)arr_get(&arr1, arr_add(&arr1)) = 2;

	arr_add_all(&arr, &arr0);
	arr_add_all(&arr, &arr1);

	EXPECT_EQ(arr.cnt, 4);
	EXPECT_EQ(arr.cap, 4);
	EXPECT_EQ(*(int *)arr_get(&arr, 0), 0);
	EXPECT_EQ(*(int *)arr_get(&arr, 1), 1);
	EXPECT_EQ(*(int *)arr_get(&arr, 2), 1);
	EXPECT_EQ(*(int *)arr_get(&arr, 3), 2);

	arr_free(&arr);
	arr_free(&arr0);
	arr_free(&arr1);

	END;
}

TEST(add_unique)
{
	START;

	arr_t arr  = { 0 };
	arr_t arr0 = { 0 };
	arr_t arr1 = { 0 };

	arr_init(&arr, 3, sizeof(int));
	arr_init(&arr0, 2, sizeof(int));
	arr_init(&arr1, 2, sizeof(int));

	*(int *)arr_get(&arr0, arr_add(&arr0)) = 0;
	*(int *)arr_get(&arr0, arr_add(&arr0)) = 1;
	*(int *)arr_get(&arr1, arr_add(&arr1)) = 1;
	*(int *)arr_get(&arr1, arr_add(&arr1)) = 2;

	arr_add_unique(&arr, &arr0);
	arr_add_unique(&arr, &arr1);

	EXPECT_EQ(arr.cnt, 3);
	EXPECT_EQ(arr.cap, 3);
	EXPECT_EQ(*(int *)arr_get(&arr, 0), 0);
	EXPECT_EQ(*(int *)arr_get(&arr, 1), 1);
	EXPECT_EQ(*(int *)arr_get(&arr, 2), 2);

	arr_free(&arr);
	arr_free(&arr0);
	arr_free(&arr1);

	END;
}

TEST(merge_all)
{
	START;

	arr_t arr  = { 0 };
	arr_t arr0 = { 0 };
	arr_t arr1 = { 0 };

	arr_init(&arr0, 2, sizeof(int));
	arr_init(&arr1, 2, sizeof(int));

	*(int *)arr_get(&arr0, arr_add(&arr0)) = 0;
	*(int *)arr_get(&arr0, arr_add(&arr0)) = 1;
	*(int *)arr_get(&arr1, arr_add(&arr1)) = 1;
	*(int *)arr_get(&arr1, arr_add(&arr1)) = 2;

	arr_merge_all(&arr, &arr0, &arr1);

	EXPECT_EQ(arr.cnt, 4);
	EXPECT_EQ(arr.cap, 4);
	EXPECT_EQ(*(int *)arr_get(&arr, 0), 0);
	EXPECT_EQ(*(int *)arr_get(&arr, 1), 1);
	EXPECT_EQ(*(int *)arr_get(&arr, 2), 1);
	EXPECT_EQ(*(int *)arr_get(&arr, 3), 2);

	arr_free(&arr);
	arr_free(&arr0);
	arr_free(&arr1);

	END;
}

TEST(merge_unique)
{
	START;

	arr_t arr  = { 0 };
	arr_t arr0 = { 0 };
	arr_t arr1 = { 0 };

	arr_init(&arr0, 2, sizeof(int));
	arr_init(&arr1, 2, sizeof(int));

	*(int *)arr_get(&arr0, arr_add(&arr0)) = 0;
	*(int *)arr_get(&arr0, arr_add(&arr0)) = 1;
	*(int *)arr_get(&arr1, arr_add(&arr1)) = 1;
	*(int *)arr_get(&arr1, arr_add(&arr1)) = 2;

	arr_merge_unique(&arr, &arr0, &arr1);

	EXPECT_EQ(arr.cnt, 3);
	EXPECT_EQ(arr.cap, 4);
	EXPECT_EQ(*(int *)arr_get(&arr, 0), 0);
	EXPECT_EQ(*(int *)arr_get(&arr, 1), 1);
	EXPECT_EQ(*(int *)arr_get(&arr, 2), 2);

	arr_free(&arr);
	arr_free(&arr0);
	arr_free(&arr1);

	END;
}

TEST(foreach)
{
	START;

	arr_t arr = { 0 };

	arr_init(&arr, 2, sizeof(int));
	*(int *)arr_get(&arr, arr_add(&arr)) = 0;
	*(int *)arr_get(&arr, arr_add(&arr)) = 1;

	int *value = NULL;

	int i = 0;
	arr_foreach(&arr, value)
	{
		EXPECT_EQ(*value, i);
		i++;
	}

	EXPECT_EQ(i, 2);

	arr_free(&arr);

	END;
}

STEST(t_arr)
{
	SSTART;
	RUN(init_free);
	RUN(add);
	RUN(get);
	RUN(get_invalid);
	RUN(set);
	RUN(set_invalid_index);
	RUN(set_invalid_value);
	RUN(adds);
	RUN(add_realloc);
	RUN(app);
	RUN(index);
	RUN(index_cmp);
	RUN(add_all);
	RUN(add_unique);
	RUN(merge_all);
	RUN(merge_unique);
	RUN(foreach);
	SEND;
}
