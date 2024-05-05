#include "t_cutils_c.h"

#include "ini_parse.h"
#include "mem.h"

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

	SEND;
}
