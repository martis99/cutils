#include "ini.h"

#include "mem.h"
#include "test.h"

TEST(t_ini_init_free)
{
	START;

	ini_t ini = { 0 };

	EXPECT_EQ(ini_init(NULL, 0, 0, 0), NULL);
	mem_oom(1);
	EXPECT_EQ(ini_init(&ini, 1, 0, 0), NULL);
	EXPECT_EQ(ini_init(&ini, 0, 1, 0), NULL);
	EXPECT_EQ(ini_init(&ini, 0, 0, 1), NULL);
	mem_oom(0);
	EXPECT_EQ(ini_init(&ini, 1, 1, 1), &ini);

	EXPECT_NE(ini.secs.data, NULL);
	EXPECT_NE(ini.pairs.data, NULL);
	EXPECT_NE(ini.vals.data, NULL);

	ini_free(&ini);
	ini_free(NULL);

	EXPECT_EQ(ini.secs.data, NULL);
	EXPECT_EQ(ini.pairs.data, NULL);
	EXPECT_EQ(ini.vals.data, NULL);

	END;
}

TEST(t_ini_add_sec)
{
	START;

	ini_t ini = { 0 };
	ini_init(&ini, 0, 0, 0);

	EXPECT_EQ(ini_add_sec(NULL, str_null()), INI_SEC_END);
	mem_oom(1);
	EXPECT_EQ(ini_add_sec(&ini, str_null()), INI_SEC_END);
	mem_oom(0);
	EXPECT_EQ(ini_add_sec(&ini, str_null()), 0);

	ini_free(&ini);

	END;
}

TEST(t_ini_add_pair)
{
	START;

	ini_t ini = { 0 };
	ini_init(&ini, 0, 0, 0);

	ini_sec_t sec = ini_add_sec(&ini, str_null());

	EXPECT_EQ(ini_add_pair(NULL, INI_SEC_END, str_null()), INI_PAIR_END);
	EXPECT_EQ(ini_add_pair(&ini, INI_SEC_END, str_null()), INI_SEC_END);
	mem_oom(1);
	EXPECT_EQ(ini_add_pair(&ini, sec, str_null()), INI_SEC_END);
	mem_oom(0);
	EXPECT_EQ(ini_add_pair(&ini, sec, str_null()), 0);

	ini_free(&ini);

	END;
}

TEST(t_ini_add_val)
{
	START;

	ini_t ini = { 0 };
	ini_init(&ini, 0, 0, 0);

	ini_sec_t sec	= ini_add_sec(&ini, str_null());
	ini_pair_t pair = ini_add_pair(&ini, sec, str_null());

	EXPECT_EQ(ini_add_val(NULL, INI_PAIR_END, str_null()), INI_VAL_END);
	EXPECT_EQ(ini_add_val(&ini, INI_PAIR_END, str_null()), INI_VAL_END);
	mem_oom(1);
	EXPECT_EQ(ini_add_val(&ini, pair, str_null()), INI_VAL_END);
	mem_oom(0);
	EXPECT_EQ(ini_add_val(&ini, pair, str_null()), 0);

	ini_free(&ini);

	END;
}

TEST(t_ini_print)
{
	START;

	ini_t ini = { 0 };
	ini_init(&ini, 1, 1, 1);

	ini_sec_t sec	 = ini_add_sec(&ini, str_null());
	ini_sec_t sec2	 = ini_add_sec(&ini, STRH("sec2"));
	ini_pair_t pair	 = ini_add_pair(&ini, sec, str_null());
	ini_pair_t pair2 = ini_add_pair(&ini, sec2, str_null());
	ini_pair_t pair3 = ini_add_pair(&ini, sec2, STRH("key2"));
	ini_add_val(&ini, pair, str_null());
	ini_add_val(&ini, pair2, STRH("val2"));
	ini_add_val(&ini, pair3, STRH("val3"));

	char buf[32] = { 0 };
	EXPECT_EQ(ini_print(NULL, PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);
	EXPECT_EQ(ini_print(&ini, PRINT_DST_BUF(buf, sizeof(buf), 0)), 26);
	EXPECT_STR(buf, "\n"
			"\n"
			"[sec2]\n"
			"val2\n"
			"key2 = val3\n");
	ini_free(&ini);

	END;
}

STEST(t_ini)
{
	SSTART;

	RUN(t_ini_init_free);
	RUN(t_ini_add_sec);
	RUN(t_ini_add_pair);
	RUN(t_ini_add_val);
	RUN(t_ini_print);

	SEND;
}
