#include "syntax.h"

#include "cstr.h"
#include "mem.h"
#include "test.h"

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

TEST(t_stx_get_rule)
{
	START;

	stx_t stx = { 0 };
	stx_init(&stx, 0, 0);

	stx_add_rule(&stx, STR("rule0"));
	stx_rule_t rule1 = stx_add_rule(&stx, STR("rule1"));

	EXPECT_EQ(stx_get_rule(NULL, str_null()), STX_RULE_END);
	EXPECT_EQ(stx_get_rule(&stx, STR("none")), STX_RULE_END);
	EXPECT_EQ(stx_get_rule(&stx, STR("rule1")), rule1);

	stx_free(&stx);

	END;
}

TEST(t_stx_create_term)
{
	START;

	stx_t stx = { 0 };
	stx_init(&stx, 0, 0);

	EXPECT_EQ(STX_TERM_RULE(NULL, -1), STX_TERM_END);
	mem_oom(1);
	EXPECT_EQ(STX_TERM_RULE(&stx, -1), STX_TERM_END);
	mem_oom(0);
	EXPECT_EQ(STX_TERM_RULE(&stx, -1), 0);

	stx_free(&stx);

	END;
}

TEST(t_stx_rule_set_term)
{
	START;

	stx_t stx = { 0 };
	stx_init(&stx, 0, 0);

	stx_rule_t rule = stx_add_rule(&stx, STRH(""));

	EXPECT_EQ(stx_rule_set_term(NULL, STX_RULE_END, -1), STX_TERM_END);
	EXPECT_EQ(stx_rule_set_term(&stx, STX_RULE_END, -1), STX_TERM_END);
	EXPECT_EQ(stx_rule_set_term(&stx, rule, 0), 0);

	stx_free(&stx);

	END;
}

TEST(t_stx_rule_add_term)
{
	START;

	stx_t stx = { 0 };
	stx_init(&stx, 0, 0);

	stx_rule_t rule = stx_add_rule(&stx, STRH(""));

	EXPECT_EQ(stx_rule_add_term(NULL, STX_RULE_END, STX_TERM_RULE(&stx, -1)), STX_TERM_END);
	EXPECT_EQ(stx_rule_add_term(&stx, STX_RULE_END, STX_TERM_RULE(&stx, -1)), STX_TERM_END);
	EXPECT_EQ(stx_rule_add_term(&stx, rule, STX_TERM_RULE(&stx, -1)), 2);

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
	stx_term_t term = STX_TERM_LITERAL(&stx, STR("T"));
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

	stx_term_t term = STX_TERM_LITERAL(&stx, STR("T"));

	EXPECT_EQ(stx_rule_add_arr(NULL, STX_RULE_END, STX_TERM_TOKEN(&stx, TOKEN_UPPER), STX_TERM_NONE(&stx)), STX_TERM_END);
	EXPECT_EQ(stx_rule_add_arr(&stx, STX_RULE_END, STX_TERM_TOKEN(&stx, TOKEN_UPPER), STX_TERM_RULE(&stx, rule)), STX_TERM_END);

	stx_free(&stx);

	END;
}

TEST(t_stx_term_add_term)
{
	START;

	stx_t stx = { 0 };
	stx_init(&stx, 0, 0);

	stx_term_t term = STX_TERM_RULE(&stx, -1);

	EXPECT_EQ(stx_term_add_term(NULL, STX_TERM_END, STX_TERM_RULE(&stx, -1)), STX_TERM_END);
	EXPECT_EQ(stx_term_add_term(&stx, STX_TERM_END, STX_TERM_RULE(&stx, -1)), STX_TERM_END);
	EXPECT_EQ(stx_term_add_term(&stx, term, STX_TERM_RULE(&stx, -1)), 3);

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

	stx_rule_add_term(&stx, file, STX_TERM_RULE(&stx, -1));
	stx_rule_add_term(&stx, file, STX_TERM_RULE(&stx, line));

	stx_rule_add_term(&stx, line, STX_TERM_TOKEN(&stx, -1));
	stx_rule_add_term(&stx, line, STX_TERM_TOKEN(&stx, TOKEN_ALPHA));
	stx_rule_add_term(&stx, line, STX_TERM_LITERAL(&stx, STRH(";")));
	stx_rule_add_term(&stx, line, STX_TERM_LITERAL(&stx, STRH("'")));

	const stx_term_t a = STX_TERM_LITERAL(&stx, STRH("A"));
	const stx_term_t b = STX_TERM_LITERAL(&stx, STRH("B"));
	stx_rule_add_term(&stx, line, STX_TERM_OR(&stx, a, b));

	EXPECT_EQ(stx_compile(NULL), 1);
	EXPECT_EQ(stx_compile(&stx), 0);

	char buf[64] = { 0 };
	EXPECT_EQ(stx_print(NULL, PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);

	EXPECT_EQ(stx_print(&stx, PRINT_DST_BUF(buf, sizeof(buf), 0)), 61);
	EXPECT_STR(buf, "<file> ::= <line>\n"
			"<line> ::= UNKNOWN ALPHA ';' \"'\" 'A' | 'B'\n");

	stx_rule_add_term(&stx, line, stx_create_term(&stx, (stx_term_data_t){ .type = -1 }));
	//stx_rule_add_term(&stx, line, STX_TERM_OR(-1, -1)); //TODO

	EXPECT_EQ(stx_compile(&stx), 0);

	EXPECT_EQ(stx_print(&stx, PRINT_DST_BUF(buf, sizeof(buf), 0)), 61);
	EXPECT_STR(buf, "<file> ::= <line>\n"
			"<line> ::= UNKNOWN ALPHA ';' \"'\" 'A' | 'B'\n");

	stx_free(&stx);

	END;
}

TEST(t_stx_print_tree)
{
	START;

	stx_t stx = { 0 };

	stx_init(&stx, 10, 10);

	const stx_rule_t file	    = stx_add_rule(&stx, STR("file"));
	const stx_rule_t functions  = stx_add_rule(&stx, STR("functions"));
	const stx_rule_t function   = stx_add_rule(&stx, STR("function"));
	const stx_rule_t identifier = stx_add_rule(&stx, STR("identifier"));
	const stx_rule_t chars	    = stx_add_rule(&stx, STR("chars"));

	stx_rule_add_term(&stx, file, STX_TERM_RULE(&stx, functions));
	stx_rule_add_term(&stx, file, STX_TERM_TOKEN(&stx, TOKEN_EOF));

	stx_rule_add_arr(&stx, functions, STX_TERM_RULE(&stx, function), STX_TERM_NONE(&stx));

	stx_rule_add_term(&stx, function, STX_TERM_RULE(&stx, identifier));

	stx_rule_add_arr(&stx, identifier, STX_TERM_RULE(&stx, chars), STX_TERM_NONE(&stx));

	const stx_term_t or = stx_rule_add_or(&stx, chars, 4, STX_TERM_TOKEN(&stx, TOKEN_ALPHA), STX_TERM_TOKEN(&stx, TOKEN_DIGIT), STX_TERM_LITERAL(&stx, STR("_")),
					      STX_TERM_LITERAL(&stx, STR("'")));

	stx_compile(&stx);

	char buf[512] = { 0 };

	EXPECT_EQ(stx_print_tree(NULL, PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);

	EXPECT_EQ(stx_print_tree(&stx, PRINT_DST_BUF(buf, sizeof(buf), 0)), 299);
	EXPECT_STR(buf, "<file>\n"
			"├─<functions>\n"
			"└─EOF\n"
			"\n"
			"<functions>\n"
			"or┬─<function>\n"
			"│ └─<functions>\n"
			"└───<function>\n"
			"\n"
			"<function>\n"
			"└─<identifier>\n"
			"\n"
			"<identifier>\n"
			"or┬─<chars>\n"
			"│ └─<identifier>\n"
			"└───<chars>\n"
			"\n"
			"<chars>\n"
			"or──ALPHA\n"
			"└─or──DIGIT\n"
			"  └─or──'_'\n"
			"    └───\"'\"\n");

	stx_rule_add_term(&stx, file, stx_create_term(&stx, (stx_term_data_t){ .type = -1 }));
	EXPECT_EQ(stx_print_tree(&stx, PRINT_DST_BUF(buf, sizeof(buf), 0)), 299);

	stx_free(&stx);

	END;
}

STEST(t_syntax)
{
	SSTART;

	RUN(t_stx_init_free);
	RUN(t_stx_add_rule);
	RUN(t_stx_get_rule);
	RUN(t_stx_create_term);
	RUN(t_stx_rule_set_term);
	RUN(t_stx_rule_add_term);
	RUN(t_stx_rule_add_or);
	RUN(t_stx_rule_add_arr);
	RUN(t_stx_term_add_term);
	RUN(t_stx_compile_print);
	RUN(t_stx_print_tree);

	SEND;
}
