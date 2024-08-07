#include "ini_parse.h"

#include "mem.h"
#include "test.h"

TEST(t_ini_prs_init_free)
{
	START;

	ini_prs_t ini_prs = { 0 };

	EXPECT_EQ(ini_prs_init(NULL), NULL);
	mem_oom(1);
	EXPECT_EQ(ini_prs_init(&ini_prs), NULL);
	mem_oom(0);
	EXPECT_EQ(ini_prs_init(&ini_prs), &ini_prs);

	ini_prs_free(&ini_prs);
	ini_prs_free(NULL);

	END;
}

TEST(t_ini_prs_parse)
{
	START;

	ini_prs_t ini_prs = { 0 };
	ini_prs_init(&ini_prs);

	str_t str = STR("");

	ini_t ini = { 0 };
	ini_init(&ini, 1, 1, 1);

	EXPECT_EQ(ini_prs_parse(NULL, str_null(), NULL), NULL);
	EXPECT_EQ(ini_prs_parse(&ini_prs, str_null(), NULL), NULL);
	EXPECT_EQ(ini_prs_parse(&ini_prs, str, NULL), NULL);
	EXPECT_EQ(ini_prs_parse(&ini_prs, str, &ini), &ini);

	char buf[128] = { 0 };
	EXPECT_EQ(ini_print(&ini, PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);
	EXPECT_STR(buf, "");

	ini_prs_free(&ini_prs);
	ini_free(&ini);

	END;
}

TEST(t_ini_prs_parse_value)
{
	START;

	ini_prs_t ini_prs = { 0 };
	ini_prs_init(&ini_prs);

	str_t str = STR("value\n");

	ini_t ini = { 0 };
	ini_init(&ini, 1, 1, 1);

	EXPECT_EQ(ini_prs_parse(&ini_prs, str, &ini), &ini);

	char buf[128] = { 0 };
	EXPECT_EQ(ini_print(&ini, PRINT_DST_BUF(buf, sizeof(buf), 0)), 6);
	EXPECT_STR(buf, "value\n");

	ini_prs_free(&ini_prs);
	ini_free(&ini);

	END;
}

TEST(t_ini_prs_parse_key_value)
{
	START;

	ini_prs_t ini_prs = { 0 };
	ini_prs_init(&ini_prs);

	str_t str = STR("key = val\n");

	ini_t ini = { 0 };
	ini_init(&ini, 1, 1, 1);

	EXPECT_EQ(ini_prs_parse(&ini_prs, str, &ini), &ini);

	char buf[128] = { 0 };
	EXPECT_EQ(ini_print(&ini, PRINT_DST_BUF(buf, sizeof(buf), 0)), 10);
	EXPECT_STR(buf, "key = val\n");

	ini_prs_free(&ini_prs);
	ini_free(&ini);

	END;
}

TEST(t_ini_prs_parse_key_value_with_comma)
{
	START;

	ini_prs_t ini_prs = { 0 };
	ini_prs_init(&ini_prs);

	str_t str = STR("key = val1,val2\n");

	ini_t ini = { 0 };
	ini_init(&ini, 1, 1, 1);

	EXPECT_EQ(ini_prs_parse(&ini_prs, str, &ini), &ini);

	char buf[128] = { 0 };
	EXPECT_EQ(ini_print(&ini, PRINT_DST_BUF(buf, sizeof(buf), 0)), 16);
	EXPECT_STR(buf, "key = val1,val2\n");

	ini_prs_free(&ini_prs);
	ini_free(&ini);

	END;
}

TEST(t_ini_prs_parse_key_array)
{
	START;

	ini_prs_t ini_prs = { 0 };
	ini_prs_init(&ini_prs);

	str_t str = STR("key = val1, val2, val3\n");

	ini_t ini = { 0 };
	ini_init(&ini, 1, 1, 1);

	EXPECT_EQ(ini_prs_parse(&ini_prs, str, &ini), &ini);

	char buf[128] = { 0 };
	EXPECT_EQ(ini_print(&ini, PRINT_DST_BUF(buf, sizeof(buf), 0)), 23);
	EXPECT_STR(buf, "key = val1, val2, val3\n");

	ini_prs_free(&ini_prs);
	ini_free(&ini);

	END;
}

TEST(t_ini_prs_parse_section)
{
	START;

	ini_prs_t ini_prs = { 0 };
	ini_prs_init(&ini_prs);

	str_t str = STR("[sec]\n");

	ini_t ini = { 0 };
	ini_init(&ini, 1, 1, 1);

	EXPECT_EQ(ini_prs_parse(&ini_prs, str, &ini), &ini);

	char buf[128] = { 0 };
	EXPECT_EQ(ini_print(&ini, PRINT_DST_BUF(buf, sizeof(buf), 0)), 6);
	EXPECT_STR(buf, "[sec]\n");

	ini_prs_free(&ini_prs);
	ini_free(&ini);

	END;
}

TEST(t_ini_prs_parse_all)
{
	START;

	ini_prs_t ini_prs = { 0 };
	ini_prs_init(&ini_prs);

	str_t str = STR("name = Test\n"
			"type = Simple\n"
			"Value1\n"
			"\n"
			"[Section]\n"
			"Value2\n"
			"tests = Test1, Test2, Test3\n");

	ini_t ini = { 0 };
	ini_init(&ini, 1, 1, 1);

	EXPECT_EQ(ini_prs_parse(NULL, str_null(), NULL), NULL);
	EXPECT_EQ(ini_prs_parse(&ini_prs, str_null(), NULL), NULL);
	EXPECT_EQ(ini_prs_parse(&ini_prs, str, NULL), NULL);
	EXPECT_EQ(ini_prs_parse(&ini_prs, str, &ini), &ini);

	char buf[128] = { 0 };
	EXPECT_EQ(ini_print(&ini, PRINT_DST_BUF(buf, sizeof(buf), 0)), 79);
	EXPECT_STR(buf, "name = Test\n"
			"type = Simple\n"
			"Value1\n"
			"\n"
			"[Section]\n"
			"Value2\n"
			"tests = Test1, Test2, Test3\n");

	ini_prs_free(&ini_prs);
	ini_free(&ini);

	END;
}

STEST(t_ini_parse)
{
	SSTART;

	RUN(t_ini_prs_init_free);
	RUN(t_ini_prs_parse);
	RUN(t_ini_prs_parse_value);
	RUN(t_ini_prs_parse_key_value);
	RUN(t_ini_prs_parse_key_value_with_comma);
	RUN(t_ini_prs_parse_key_array);
	RUN(t_ini_prs_parse_section);
	RUN(t_ini_prs_parse_all);

	SEND;
}
