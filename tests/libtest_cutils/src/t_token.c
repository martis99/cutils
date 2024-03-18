#include "t_cutils_c.h"

#include "cstr.h"
#include "token.h"

TEST(t_token_type_str)
{
	START;

	token_type_str(TOKEN_ALPHA);

	END;
}

TEST(t_token_type_enum)
{
	START;

	EXPECT_EQ(token_type_enum(STR("ALPHA")), TOKEN_ALPHA);
	EXPECT_EQ(token_type_enum(STR("")), TOKEN_UNKNOWN);

	END;
}

TEST(t_token_dbg)
{
	START;

	{
		token_t token = {
			.value = STR("\t\r\n\0a"),
			.line  = 0,
			.col   = 0,
			.type  = 1 << TOKEN_ALPHA | 1 << TOKEN_LOWER,
		};

		char buf[64] = { 0 };
		EXPECT_EQ(token_dbg(token, PRINT_DST_BUF(buf, sizeof(buf), 0)), 34);

		const char exp[] = "ALPHA | LOWER ( 0,  0) \"\\t\\r\\n\\0a\"";
		EXPECT_STR(buf, exp);
	}

	END;
}

STEST(t_token)
{
	SSTART;

	RUN(t_token_type_str);
	RUN(t_token_type_enum);
	RUN(t_token_dbg);

	SEND;
}
