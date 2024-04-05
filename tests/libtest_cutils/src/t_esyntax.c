#include "t_cutils_c.h"

#include "cstr.h"
#include "esyntax.h"
#include "mem.h"

TEST(t_estx_init_free)
{
	START;

	estx_t estx = { 0 };

	EXPECT_EQ(estx_init(NULL, 0, 0), NULL);
	mem_oom(1);
	EXPECT_EQ(estx_init(&estx, 1, 0), NULL);
	EXPECT_EQ(estx_init(&estx, 0, 1), NULL);
	mem_oom(0);
	EXPECT_EQ(estx_init(&estx, 0, 0), &estx);

	EXPECT_NE(estx.rules.data, NULL);
	EXPECT_NE(estx.terms.data, NULL);

	estx_free(&estx);
	estx_free(NULL);

	EXPECT_EQ(estx.rules.data, NULL);
	EXPECT_EQ(estx.terms.data, NULL);

	END;
}

TEST(t_estx_add_rule)
{
	START;

	estx_t estx = { 0 };
	estx_init(&estx, 0, 0);

	EXPECT_EQ(estx_add_rule(NULL, str_null()), ESTX_RULE_END);
	mem_oom(1);
	EXPECT_EQ(estx_add_rule(&estx, str_null()), ESTX_RULE_END);
	mem_oom(0);
	EXPECT_EQ(estx_add_rule(&estx, STRH("")), 0);

	estx_free(&estx);

	END;
}

TEST(t_estx_get_rule)
{
	START;

	estx_t estx = { 0 };
	estx_init(&estx, 0, 0);

	estx_add_rule(&estx, STR("rule0"));
	estx_rule_t rule1 = estx_add_rule(&estx, STR("rule1"));

	EXPECT_EQ(estx_get_rule(NULL, str_null()), ESTX_RULE_END);
	EXPECT_EQ(estx_get_rule(&estx, STR("none")), ESTX_RULE_END);
	EXPECT_EQ(estx_get_rule(&estx, STR("rule1")), rule1);

	estx_free(&estx);

	END;
}

TEST(t_estx_create_term)
{
	START;

	estx_t estx = { 0 };
	estx_init(&estx, 0, 0);

	EXPECT_EQ(ESTX_TERM_RULE(NULL, -1, 0), ESTX_TERM_END);
	mem_oom(1);
	EXPECT_EQ(ESTX_TERM_RULE(&estx, -1, 0), ESTX_TERM_END);
	mem_oom(0);
	EXPECT_EQ(ESTX_TERM_RULE(&estx, -1, 0), 0);

	estx_free(&estx);

	END;
}

TEST(t_estx_rule_set_term)
{
	START;

	estx_t estx = { 0 };
	estx_init(&estx, 0, 0);

	estx_rule_t rule = estx_add_rule(&estx, STRH(""));

	EXPECT_EQ(estx_rule_set_term(NULL, ESTX_RULE_END, -1), ESTX_TERM_END);
	EXPECT_EQ(estx_rule_set_term(&estx, ESTX_RULE_END, -1), ESTX_TERM_END);
	EXPECT_EQ(estx_rule_set_term(&estx, rule, 0), 0);

	estx_free(&estx);

	END;
}

TEST(t_estx_term_add_term)
{
	START;

	estx_t estx = { 0 };
	estx_init(&estx, 0, 0);

	estx_term_t term = ESTX_TERM_RULE(&estx, -1, 0);

	EXPECT_EQ(estx_term_add_term(NULL, ESTX_TERM_END, ESTX_TERM_RULE(&estx, -1, 0)), ESTX_TERM_END);
	EXPECT_EQ(estx_term_add_term(&estx, ESTX_TERM_END, ESTX_TERM_RULE(&estx, -1, 0)), ESTX_TERM_END);
	EXPECT_EQ(estx_term_add_term(&estx, term, ESTX_TERM_RULE(&estx, -1, 0)), 3);

	estx_free(&estx);

	END;
}

TEST(t_estx_compile_print)
{
	START;

	estx_t estx = { 0 };
	estx_init(&estx, 0, 0);

	estx_term_t file = estx_rule_set_term(&estx, estx_add_rule(&estx, STRH("file")), ESTX_TERM_CON(&estx));
	estx_term_t line = estx_rule_set_term(&estx, estx_add_rule(&estx, STRH("line")), ESTX_TERM_CON(&estx));

	estx_term_add_term(&estx, file, ESTX_TERM_RULE(&estx, -1, 0));
	estx_term_add_term(&estx, file, ESTX_TERM_RULE(&estx, line, 0));

	estx_term_add_term(&estx, line, ESTX_TERM_TOKEN(&estx, -1, ESTX_TERM_OCC_OPT));
	estx_term_add_term(&estx, line, ESTX_TERM_TOKEN(&estx, TOKEN_ALPHA, ESTX_TERM_OCC_REP));
	estx_term_add_term(&estx, line, ESTX_TERM_LITERAL(&estx, STRH(";"), ESTX_TERM_OCC_OPT | ESTX_TERM_OCC_REP));
	estx_term_add_term(&estx, line, ESTX_TERM_LITERAL(&estx, STRH("'"), 0));

	const estx_term_t group = estx_term_add_term(&estx, line, ESTX_TERM_GROUP(&estx, 0));
	const estx_term_t a	= ESTX_TERM_LITERAL(&estx, STRH("A"), 0);
	const estx_term_t b	= ESTX_TERM_LITERAL(&estx, STRH("B"), 0);
	const estx_term_t alt	= estx_term_add_term(&estx, group, ESTX_TERM_ALT(&estx));
	estx_term_add_term(&estx, alt, a);
	estx_term_add_term(&estx, alt, b);

	EXPECT_EQ(estx_compile(NULL), 1);
	EXPECT_EQ(estx_compile(&estx), 0);

	char buf[256] = { 0 };
	EXPECT_EQ(estx_print(NULL, PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);

	EXPECT_EQ(estx_print(&estx, PRINT_DST_BUF(buf, sizeof(buf), 0)), 58);
	EXPECT_STR(buf, "file = line\n"
			"line = UNKNOWN? ALPHA+ ';'* \"'\" ( 'A' | 'B' )\n");

	EXPECT_EQ(estx_print_tree(&estx, PRINT_DST_BUF(buf, sizeof(buf), 0)), 144);
	EXPECT_STR(buf, "<file>\n"
			"con\n"
			"├─\n"
			"└─<line>\n"
			"\n"
			"<line>\n"
			"con\n"
			"├─UNKNOWN?\n"
			"├─ALPHA+\n"
			"├─';'*\n"
			"├─\"'\"\n"
			"└─group\n"
			"  └─alt\n"
			"    ├─'A'\n"
			"    └─'B'\n");

	estx_term_add_term(&estx, line, estx_create_term(&estx, (estx_term_data_t){ .type = -1 }));
	//estx_rule_add_term(&estx, line, ESTX_TERM_OR(-1, -1)); //TODO

	EXPECT_EQ(estx_compile(&estx), 0);

	EXPECT_EQ(estx_print(&estx, PRINT_DST_BUF(buf, sizeof(buf), 0)), 58);
	EXPECT_STR(buf, "file = line\n"
			"line = UNKNOWN? ALPHA+ ';'* \"'\" ( 'A' | 'B' )\n");

	estx_free(&estx);

	END;
}

TEST(t_estx_print_tree)
{
	START;

	estx_t estx = { 0 };
	estx_init(&estx, 0, 0);

	estx_term_t file = estx_rule_set_term(&estx, estx_add_rule(&estx, STRH("file")), ESTX_TERM_CON(&estx));

	estx_term_add_term(&estx, file, estx_create_term(&estx, (estx_term_data_t){ .type = -1 }));

	char buf[64] = { 0 };
	EXPECT_EQ(estx_print_tree(NULL, PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);
	EXPECT_EQ(estx_print_tree(&estx, PRINT_DST_BUF(buf, sizeof(buf), 0)), 18);

	estx_free(&estx);

	END;
}

STEST(t_esyntax)
{
	SSTART;

	RUN(t_estx_init_free);
	RUN(t_estx_add_rule);
	RUN(t_estx_get_rule);
	RUN(t_estx_create_term);
	RUN(t_estx_rule_set_term);
	RUN(t_estx_term_add_term);
	RUN(t_estx_compile_print);
	RUN(t_estx_print_tree);

	SEND;
}
