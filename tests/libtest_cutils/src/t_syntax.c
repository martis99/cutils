#include "t_cutils_c.h"

#include "cstr.h"
#include "mem.h"
#include "syntax.h"

TEST(t_stx_init_free)
{
	START;

	stx_t stx = { 0 };

	EXPECT_EQ(stx_init(NULL, 0, 0), NULL);
	mem_oom(1);
	EXPECT_EQ(stx_init(&stx, 1, 0), NULL);
	EXPECT_EQ(stx_init(&stx, 0, 1), NULL);
	mem_oom(0);
	EXPECT_EQ(stx_init(&stx, 0, 0), &stx);

	EXPECT_NE(stx.rules.data, NULL);
	EXPECT_NE(stx.terms.data, NULL);

	stx_free(&stx);
	stx_free(NULL);

	EXPECT_EQ(stx.rules.data, NULL);
	EXPECT_EQ(stx.terms.data, NULL);

	END;
}

TEST(t_stx_add_rule)
{
	START;

	stx_t stx = { 0 };
	stx_init(&stx, 0, 0);

	EXPECT_EQ(stx_add_rule(NULL, str_null()), STX_RULE_END);
	mem_oom(1);
	EXPECT_EQ(stx_add_rule(&stx, str_null()), STX_RULE_END);
	mem_oom(0);
	EXPECT_EQ(stx_add_rule(&stx, STRH("")), 0);

	stx_free(&stx);

	END;
}

TEST(t_stx_create_term)
{
	START;

	stx_t stx = { 0 };
	stx_init(&stx, 0, 0);

	EXPECT_EQ(stx_create_term(NULL, STX_TERM_RULE(-1)), STX_TERM_END);
	mem_oom(1);
	EXPECT_EQ(stx_create_term(&stx, STX_TERM_RULE(-1)), STX_TERM_END);
	mem_oom(0);
	EXPECT_EQ(stx_create_term(&stx, STX_TERM_RULE(-1)), 0);

	stx_free(&stx);

	END;
}

TEST(t_stx_rule_add_term)
{
	START;

	stx_t stx = { 0 };
	stx_init(&stx, 0, 0);

	stx_rule_t rule = stx_add_rule(&stx, STRH(""));

	EXPECT_EQ(stx_rule_add_term(NULL, STX_RULE_END, STX_TERM_RULE(-1)), STX_TERM_END);
	EXPECT_EQ(stx_rule_add_term(&stx, STX_RULE_END, STX_TERM_RULE(-1)), STX_TERM_END);
	EXPECT_EQ(stx_rule_add_term(&stx, rule, STX_TERM_RULE(-1)), 0);

	stx_free(&stx);

	END;
}

TEST(t_stx_rule_add_or)
{
	START;

	stx_t stx = { 0 };
	stx_init(&stx, 0, 0);

	stx_rule_t rule = stx_add_rule(&stx, STRH(""));

	EXPECT_EQ(stx_rule_add_or(NULL, STX_RULE_END, 0), STX_TERM_END);
	stx_term_t term = stx_create_term(&stx, STX_TERM_LITERAL(STR("T")));
	EXPECT_EQ(stx_rule_add_or(NULL, STX_RULE_END, 1, term), STX_TERM_END);
	EXPECT_EQ(stx_rule_add_or(NULL, STX_RULE_END, 2, term, term), STX_TERM_END);

	stx_free(&stx);

	END;
}

TEST(t_stx_rule_add_arr)
{
	START;

	stx_t stx = { 0 };
	stx_init(&stx, 0, 0);
	
	stx_rule_t rule = stx_add_rule(&stx, STRH(""));

	stx_term_t term = stx_create_term(&stx, STX_TERM_LITERAL(STR("T")));
	
	EXPECT_EQ(stx_rule_add_arr(NULL, STX_RULE_END, STX_TERM_TOKEN(TOKEN_UPPER), STX_TERM_NONE()), STX_TERM_END);
	EXPECT_EQ(stx_rule_add_arr(&stx, STX_RULE_END, STX_TERM_TOKEN(TOKEN_UPPER), STX_TERM_RULE(rule)), STX_TERM_END);

	stx_free(&stx);

	END;
}

TEST(t_stx_term_add_term)
{
	START;

	stx_t stx = { 0 };
	stx_init(&stx, 0, 0);

	stx_term_t term = stx_create_term(&stx, STX_TERM_RULE(-1));

	EXPECT_EQ(stx_term_add_term(NULL, STX_TERM_END, STX_TERM_RULE(-1)), STX_TERM_END);
	EXPECT_EQ(stx_term_add_term(&stx, STX_TERM_END, STX_TERM_RULE(-1)), STX_TERM_END);
	EXPECT_EQ(stx_term_add_term(&stx, term, STX_TERM_RULE(-1)), 1);

	stx_free(&stx);

	END;
}

TEST(t_stx_compile_print)
{
	START;

	stx_t stx = { 0 };
	stx_init(&stx, 0, 0);

	stx_rule_t file = stx_add_rule(&stx, STRH("file"));
	stx_rule_t line = stx_add_rule(&stx, STRH("line"));

	stx_rule_add_term(&stx, file, STX_TERM_RULE(-1));
	stx_rule_add_term(&stx, file, STX_TERM_RULE(line));

	stx_rule_add_term(&stx, line, STX_TERM_TOKEN(-1));
	stx_rule_add_term(&stx, line, STX_TERM_TOKEN(TOKEN_ALPHA));
	stx_rule_add_term(&stx, line, STX_TERM_LITERAL(STRH(";")));
	stx_rule_add_term(&stx, line, STX_TERM_LITERAL(STRH("'")));

	const stx_term_t a = stx_create_term(&stx, STX_TERM_LITERAL(STRH("A")));
	const stx_term_t b = stx_create_term(&stx, STX_TERM_LITERAL(STRH("B")));
	stx_rule_add_term(&stx, line, STX_TERM_OR(a, b));

	EXPECT_EQ(stx_compile(NULL), 1);
	EXPECT_EQ(stx_compile(&stx), 0);

	EXPECT_EQ(stx_print(NULL, PRINT_DST_NONE()), 0);

	{
		char buf[64] = { 0 };
		EXPECT_EQ(stx_print(&stx, PRINT_DST_BUF(buf, sizeof(buf), 0)), 61);

		const char exp[] = "<file> ::= <line>\n"
				   "<line> ::= UNKNOWN ALPHA ';' \"'\" 'A' | 'B'\n";
		EXPECT_STR(buf, exp);
	}

	stx_rule_add_term(&stx, line, (stx_term_data_t){ .type = -1 });
	//stx_rule_add_term(&stx, line, STX_TERM_OR(-1, -1)); //TODO

	EXPECT_EQ(stx_compile(&stx), 0);

	{
		char buf[64] = { 0 };
		EXPECT_EQ(stx_print(&stx, PRINT_DST_BUF(buf, sizeof(buf), 0)), 61);;

		const char exp[] = "<file> ::= <line>\n"
				   "<line> ::= UNKNOWN ALPHA ';' \"'\" 'A' | 'B'\n";
		EXPECT_STR(buf, exp);
	}

	stx_free(&stx);

	END;
}

STEST(t_syntax)
{
	SSTART;

	RUN(t_stx_init_free);
	RUN(t_stx_add_rule);
	RUN(t_stx_create_term);
	RUN(t_stx_rule_add_term);
	RUN(t_stx_rule_add_or);
	RUN(t_stx_rule_add_arr);
	RUN(t_stx_term_add_term);
	RUN(t_stx_compile_print);

	SEND;
}
