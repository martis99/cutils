#include "t_print.h"

#include "file.h"
#include "print.h"

#include "test.h"

#define TEST_FILE "t_print.txt"

TEST(t_printf)
{
	START;

	EXPECT_EQ(c_printf(NULL), 0);
	EXPECT_EQ(c_printf(""), 0);

	c_wprintf(L"");
	EXPECT_EQ(c_printf(" "), 1);

	END;
}

TEST(t_fprintf, FILE *file)
{
	START;

	EXPECT_EQ(c_fprintf(NULL, NULL), 0);

	file_reopen(TEST_FILE, "r", file);
	EXPECT_EQ(c_fprintf(file, ""), 0);

	{
		file_reopen(TEST_FILE, "wb+", file);

		EXPECT_EQ(c_fprintf(file, NULL), 0);
		EXPECT_EQ(c_fprintf(file, "test"), 4);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		EXPECT_STR(buf, "test");
	}

	END;
}

TEST(t_sprintf)
{
	START;

	char buf[16] = { 0 };

	EXPECT_EQ(c_sprintf(NULL, 1, ""), 0);
	EXPECT_EQ(c_sprintf(buf, 1, NULL), 0);
	EXPECT_EQ(c_sprintf(buf, 1, "abcd"), 0);
	EXPECT_EQ(c_sprintf(buf, sizeof(buf), "%s", "a"), 1);

	EXPECT_STR(buf, "a");

	END;
}

TEST(t_wprintf)
{
	START;

	EXPECT_EQ(c_wprintf(NULL), 0);
	EXPECT_EQ(c_wprintf(L""), 0);

	c_printf("");
	EXPECT_EQ(c_wprintf(L" "), 1);

	END;
}

TEST(t_swprintf)
{
	START;

	wchar buf[16] = { 0 };

	EXPECT_EQ(c_swprintf(NULL, 1, L""), 0);
	EXPECT_EQ(c_swprintf(buf, 1, NULL), 0);
	EXPECT_EQ(c_swprintf(buf, sizeof(buf), L"%ls", L"a"), 1);

	EXPECT_WSTR(buf, L"a");

	END;
}

TEST(t_fflush, FILE *file)
{
	START;

	EXPECT_EQ(c_fflush(NULL), 1);
	EXPECT_EQ(c_fflush(file), 0);

	END;
}

TEST(t_set_unset_u16)
{
	START;

	EXPECT_EQ(c_set_u16(NULL), 0);

	int mode = c_set_u16(stdout);

	EXPECT_EQ(c_unset_u16(NULL, 0), 0);
	c_unset_u16(stdout, mode);

	END;
}

TEST(t_ur, FILE *file)
{
	START;

	EXPECT_EQ(c_ur(NULL), 0);

	{
		file_reopen(TEST_FILE, "wb+", file);

		EXPECT_EQ(c_ur(file), sizeof("└─") - 1);

		char buf[16] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		EXPECT_STR(buf, "└─");
	}

	END;
}

TEST(t_v, FILE *file)
{
	START;

	EXPECT_EQ(c_v(NULL), 0);

	{
		file_reopen(TEST_FILE, "wb+", file);

		EXPECT_EQ(c_v(file), sizeof("│ ") - 1);

		char buf[16] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		EXPECT_STR(buf, "│ ");
	}

	END;
}

TEST(t_vr, FILE *file)
{
	START;

	EXPECT_EQ(c_vr(NULL), 0);

	{
		file_reopen(TEST_FILE, "wb+", file);

		EXPECT_EQ(c_vr(file), sizeof("├─") - 1);

		char buf[16] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		EXPECT_STR(buf, "├─");
	}

	END;
}

STEST(t_print)
{
	SSTART;

	FILE *file = file_open(TEST_FILE, "wb+");

	RUN(t_printf);
	RUN(t_fprintf, file);
	RUN(t_sprintf);
	RUN(t_wprintf);
	RUN(t_swprintf);
	RUN(t_fflush, file);
	RUN(t_set_unset_u16);
	RUN(t_ur, file);
	RUN(t_v, file);
	RUN(t_vr, file);

	file_close(file);
	file_delete(TEST_FILE);

	SEND;
}
