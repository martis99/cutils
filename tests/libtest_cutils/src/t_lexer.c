#include "t_cutils_c.h"

#include "cstr.h"
#include "file.h"
#include "lexer.h"
#include "mem.h"

#define TEST_FILE "t_lexer.txt"

TEST(t_lex_init_free)
{
	START;

	lex_t lex = { 0 };

	EXPECT_EQ(lex_init(NULL, 0), NULL);
	mem_oom(1);
	EXPECT_EQ(lex_init(&lex, 1), NULL);
	mem_oom(0);
	EXPECT_EQ(lex_init(&lex, 0), &lex);

	EXPECT_NE(lex.tokens.data, NULL);

	lex_free(&lex);
	lex_free(NULL);

	EXPECT_EQ(lex.tokens.data, NULL);

	END;
}

TEST(t_lex_add_token)
{
	START;

	lex_t lex = { 0 };
	lex_init(&lex, 0);

	EXPECT_EQ(lex_add_token(NULL), LEX_TOKEN_END);
	mem_oom(1);
	EXPECT_EQ(lex_add_token(&lex), LEX_TOKEN_END);
	mem_oom(0);
	EXPECT_EQ(lex_add_token(&lex), 0);

	lex_free(&lex);

	END;
}

TEST(t_lex_get_token)
{
	START;

	lex_t lex = { 0 };
	lex_init(&lex, 0);

	const lex_token_t token = lex_add_token(&lex);

	EXPECT_EQ(lex_get_token(NULL, LEX_TOKEN_END), NULL);
	EXPECT_EQ(lex_get_token(&lex, LEX_TOKEN_END), NULL);
	EXPECT_NE(lex_get_token(&lex, token), NULL);

	lex_free(&lex);

	END;
}

TEST(t_lex_tokenize)
{
	START;

	lex_t lex = { 0 };
	lex_init(&lex, 0);

	EXPECT_EQ(lex_tokenize(NULL, str_null()), NULL);

	str_t str = STR("#include <stdio.h>\n"
			"\n"
			"int main(int argc, char **argv) {\n"
			"\tconst char *str = \"ABC\";\r\n"
			"char c = 'a';\n"
			"\treturn 0;\n"
			"}\n\0");

	mem_oom(1);
	EXPECT_EQ(lex_tokenize(&lex, str), NULL);
	mem_oom(0);
	EXPECT_EQ(lex_tokenize(&lex, str), &lex);

	lex_free(&lex);

	END;
}

TEST(t_lex_print_line)
{
	START;

	lex_t lex = { 0 };
	lex_init(&lex, 0);

	lex_tokenize(&lex, STR("Test\nNew Line"));

	char buf[64] = { 0 };
	EXPECT_EQ(lex_print_line(NULL, 0, NULL, 0, 0, NULL), 0);
	EXPECT_EQ(lex_print_line(&lex, 0, NULL, 0, 0, NULL), 0);
	EXPECT_EQ(lex_print_line(&lex, 1, c_sprintv_cb, sizeof(buf), 0, buf), 6);

	EXPECT_STR(buf, "Test\\n");

	lex_free(&lex);

	END;
}

TESTP(t_lex_dbg, FILE *file)
{
	START;

	EXPECT_EQ(lex_dbg(NULL, NULL), 0);

	lex_t lex = { 0 };
	lex_init(&lex, 0);

	lex_tokenize(&lex, STR("A\nLN"));

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_NE(lex_dbg(&lex, file), 0);

		char buf[512] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "ALPHA | UPPER ( 0,  0) \"A\"\n"
				   "WS    | NL    ( 0,  1) \"\\n\"\n"
				   "ALPHA | UPPER ( 1,  0) \"L\"\n"
				   "ALPHA | UPPER ( 1,  1) \"N\"\n"
				   "EOF           ( 1,  2) \"\\0\"\n";
		EXPECT_STR(buf, exp);
	}

	lex_free(&lex);

	END;
}

STEST(t_lexer)
{
	SSTART;

	FILE *file = file_open(TEST_FILE, "wb+");

	RUN(t_lex_init_free);
	RUN(t_lex_add_token);
	RUN(t_lex_get_token);
	RUN(t_lex_tokenize);
	RUN(t_lex_print_line);
	RUNP(t_lex_dbg, file);

	file_close(file);
	file_delete(TEST_FILE);

	SEND;
}
