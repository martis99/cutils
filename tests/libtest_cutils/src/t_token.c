#include "t_cutils_c.h"

#include "cstr.h"
#include "file.h"
#include "token.h"

#define TEST_FILE "t_token.txt"

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

TESTP(t_loken_dbg, FILE *file)
{
	START;

	{
		token_t token = {
			.value = STR("\t\r\n\0a"),
			.line  = 0,
			.col   = 0,
			.type  = 1 << TOKEN_ALPHA | 1 << TOKEN_LOWER,
		};


		file_reopen(TEST_FILE, "wb+", file);
		token_dbg(token, file);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "ALPHA | LOWER ( 0,  0) \"\\t\\r\\n\\0a\"";
		EXPECT_STR(buf, exp);
	}

	END;
}

STEST(t_token)
{
	SSTART;

	FILE *file = file_open(TEST_FILE, "wb+");

	RUN(t_token_type_str);
	RUN(t_token_type_enum);
	RUNP(t_loken_dbg, file);

	file_close(file);
	file_delete(TEST_FILE);

	SEND;
}
