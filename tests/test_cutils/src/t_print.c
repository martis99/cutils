#include "t_print.h"

#include "print.h"

#include "test.h"

TEST(t_printf_null)
{
	START;

	EXPECT_EQ(c_printf(NULL), 0);

	END;
}

TEST(t_fprintf_null)
{
	START;

	EXPECT_EQ(c_fprintf(NULL, ""), 0);
	EXPECT_EQ(c_fprintf(NULL, NULL), 0);

	END;
}

TEST(t_sprintf_null)
{
	START;

	char buf[1] = { 0 };

	EXPECT_EQ(c_sprintf(NULL, 1, ""), 0);
	EXPECT_EQ(c_sprintf(buf, 1, NULL), 0);

	END;
}

TEST(t_wprintf_null)
{
	START;

	EXPECT_EQ(c_wprintf(NULL), 0);

	END;
}

TEST(t_swprintf_null)
{
	START;

	wchar buf[1] = { 0 };

	EXPECT_EQ(c_swprintf(NULL, 1, L""), 0);
	EXPECT_EQ(c_swprintf(buf, 1, NULL), 0);

	END;
}

TEST(t_set_u16_null)
{
	START;

	EXPECT_EQ(c_set_u16(NULL), 0);

	END;
}

TEST(t_unset_u16_null)
{
	START;

	EXPECT_EQ(c_unset_u16(NULL, 0), 0);

	END;
}

TEST(t_ur_null)
{
	START;

	c_ur(NULL);

	END;
}

TEST(t_v_null)
{
	START;

	c_v(NULL);

	END;
}

TEST(t_vr_null)
{
	START;

	c_vr(NULL);

	END;
}

TEST(t_sprintf)
{
	START;

	char buf[16] = { 0 };
	c_sprintf(buf, sizeof(buf), "%s", "a");

	EXPECT_STR(buf, "a");

	END;
}

TEST(t_swprintf)
{
	START;

	wchar buf[16] = { 0 };
	c_swprintf(buf, sizeof(buf), L"%s", L"a");

	EXPECT_WSTR(buf, L"a");

	END;
}

TEST(t_print_null)
{
	SSTART;
	RUN(t_printf_null);
	RUN(t_fprintf_null);
	RUN(t_sprintf_null);
	RUN(t_wprintf_null);
	RUN(t_swprintf_null);
	RUN(t_set_u16_null);
	RUN(t_unset_u16_null);
	RUN(t_ur_null);
	RUN(t_v_null);
	RUN(t_vr_null);
	SEND;
}

STEST(t_print)
{
	SSTART;
	RUN(t_print_null);
	RUN(t_sprintf);
	RUN(t_swprintf);
	SEND;
}
