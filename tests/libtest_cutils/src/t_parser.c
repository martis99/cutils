#include "parser.h"

#include "bnf.h"
#include "cstr.h"
#include "file.h"
#include "mem.h"
#include "test.h"

#include <stdlib.h>

TEST(t_prs_init_free)
{
	START;

	prs_t prs = { 0 };

	EXPECT_EQ(prs_init(NULL, 0), NULL);
	mem_oom(1);
	EXPECT_EQ(prs_init(&prs, 1), NULL);
	mem_oom(0);
	EXPECT_EQ(prs_init(&prs, 0), &prs);

	EXPECT_NE(prs.nodes.data, NULL);

	prs_free(&prs);
	prs_free(NULL);

	EXPECT_EQ(prs.nodes.data, NULL);

	END;
}

TEST(t_prs_add_node)
{
	START;

	prs_t prs = { 0 };
	prs_init(&prs, 0);

	EXPECT_EQ(prs_add(NULL, (prs_node_data_t){ .type = PRS_NODE_UNKNOWN }), PRS_NODE_END);
	mem_oom(1);
	EXPECT_EQ(prs_add(&prs, (prs_node_data_t){ .type = PRS_NODE_UNKNOWN }), PRS_NODE_END);
	mem_oom(0);

	prs_node_t parent = prs_add(&prs, (prs_node_data_t){ .type = PRS_NODE_UNKNOWN });
	prs_node_t child  = prs_add(&prs, (prs_node_data_t){ .type = PRS_NODE_UNKNOWN });

	EXPECT_EQ(prs_add_node(NULL, PRS_NODE_END, PRS_NODE_END), PRS_NODE_END);
	EXPECT_EQ(prs_add_node(&prs, PRS_NODE_END, PRS_NODE_END), PRS_NODE_END);
	EXPECT_EQ(prs_add_node(&prs, parent, PRS_NODE_END), PRS_NODE_END);
	EXPECT_EQ(prs_add_node(&prs, parent, child), child);

	prs_free(&prs);

	END;
}

TEST(t_prs_remove_node)
{
	START;

	prs_t prs = { 0 };
	prs_init(&prs, 0);

	prs_node_t node = prs_add_node(&prs, PRS_NODE_END, prs_add(&prs, (prs_node_data_t){ .type = PRS_NODE_UNKNOWN }));

	EXPECT_EQ(prs_remove_node(NULL, PRS_NODE_END), 1);
	EXPECT_EQ(prs_remove_node(&prs, PRS_NODE_END), 1);
	EXPECT_EQ(prs_remove_node(&prs, node), 0);

	prs_free(&prs);

	END;
}

TEST(t_prs_get_rule)
{
	START;

	prs_t prs = { 0 };
	prs_init(&prs, 0);

	stx_t stx = { 0 };
	stx_init(&stx, 1, 1);

	stx_rule_t rule0 = stx_add_rule(&stx, STR("rule0"));
	stx_rule_t rule1 = stx_add_rule(&stx, STR("rule1"));

	prs_node_t root = prs_add_node(&prs, PRS_NODE_END, PRS_NODE_LITERAL(&prs, ""));

	prs_add_node(&prs, root, PRS_NODE_LITERAL(&prs, ""));
	prs_add_node(&prs, root, PRS_NODE_RULE(&prs, rule0));
	prs_node_t node = prs_add_node(&prs, root, PRS_NODE_RULE(&prs, rule1));

	EXPECT_EQ(prs_get_rule(NULL, PRS_NODE_END, STX_RULE_END), PRS_NODE_END);
	EXPECT_EQ(prs_get_rule(&prs, PRS_NODE_END, STX_RULE_END), PRS_NODE_END);
	EXPECT_EQ(prs_get_rule(&prs, root, STX_RULE_END), PRS_NODE_END);
	EXPECT_EQ(prs_get_rule(&prs, root, rule1), node);

	stx_free(&stx);
	prs_free(&prs);

	END;
}

TEST(t_prs_get_str)
{
	START;

	prs_t prs = { 0 };
	prs_init(&prs, 0);

	stx_t stx = { 0 };
	stx_init(&stx, 1, 1);

	lex_t lex = { 0 };
	lex_init(&lex, 1);
	lex_token_t token = lex_add_token(&lex);

	*lex_get_token(&lex, token) = (token_t){
		.value	    = STR("b"),
		.line_start = 0,
		.line	    = 0,
		.col	    = 0,
		.type	    = (1 << TOKEN_ALPHA) | (1 << TOKEN_LOWER),
	};

	stx_rule_t rule0 = stx_add_rule(&stx, STR("rule0"));
	stx_rule_t rule1 = stx_add_rule(&stx, STR("rule1"));

	prs_node_t root = prs_add_node(&prs, PRS_NODE_END, PRS_NODE_RULE(&prs, rule0));

	prs_add_node(&prs, root, PRS_NODE_LITERAL(&prs, STR("a")));
	prs_add_node(&prs, root, PRS_NODE_TOKEN(&prs, token));
	prs_node_t node = prs_add_node(&prs, root, PRS_NODE_RULE(&prs, rule1));

	str_t str = strz(16);

	EXPECT_EQ(prs_get_str(NULL, PRS_NODE_END, NULL), 1);
	EXPECT_EQ(prs_get_str(&prs, PRS_NODE_END, NULL), 1);
	EXPECT_EQ(prs_get_str(&prs, root, &str), 0);
	EXPECT_STR(str.data, "a");
	str.len = 0;
	prs.lex = &lex;
	EXPECT_EQ(prs_get_str(&prs, root, &str), 0);

	EXPECT_STR(str.data, "ab");

	str_free(&str);
	lex_free(&lex);
	stx_free(&stx);
	prs_free(&prs);

	END;
}

TEST(t_prs_parse_cache)
{
	START;

	prs_t prs = { 0 };
	prs_init(&prs, 100);

	lex_t lex = { 0 };
	lex_init(&lex, 100);

	stx_t stx = { 0 };
	stx_init(&stx, 10, 10);

	const stx_rule_t file = stx_add_rule(&stx, STR("file"));
	const stx_rule_t line = stx_add_rule(&stx, STR("line"));
	const stx_rule_t ra   = stx_add_rule(&stx, STR("ra"));

	stx_rule_add_term(&stx, file, STX_TERM_RULE(&stx, line));
	stx_rule_add_term(&stx, file, STX_TERM_TOKEN(&stx, TOKEN_EOF));

	stx_rule_add_arr(&stx, line, STX_TERM_RULE(&stx, ra), STX_TERM_NONE(&stx));

	stx_rule_add_term(&stx, ra, STX_TERM_LITERAL(&stx, STR("a")));

	str_t bnf = STR("a");

	lex_tokenize(&lex, bnf);

	prs_node_t root = prs_parse(&prs, &stx, file, &lex);
	EXPECT_EQ(root, 0);

	char buf[512] = { 0 };
	EXPECT_EQ(prs_print(&prs, root, PRINT_DST_BUF(buf, sizeof(buf), 0)), 56);
	EXPECT_STR(buf, "file\n"
			"├─line\n"
			"│ └─ra\n"
			"│   └─'a'\n"
			"└─'\\0'\n");

	prs_free(&prs);
	lex_free(&lex);
	stx_free(&stx);

	END;
}

TEST(t_prs_parse_bnf)
{
	START;

	prs_t prs = { 0 };
	prs_init(&prs, 256);

	EXPECT_EQ(prs_parse(NULL, NULL, STX_RULE_END, NULL), PRS_NODE_END);

	{
		str_t bnf = STR("<file> ::= <");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, bnf);

		stx_t stx = { 0 };
		stx_init(&stx, 0, 0);

		stx_rule_t rule = stx_add_rule(&stx, STRH(""));
		stx_rule_add_term(&stx, rule, stx_create_term(&stx, (stx_term_data_t){ .type = -1 }));

		EXPECT_EQ(prs_parse(&prs, &stx, rule, &lex), PRS_NODE_END);

		stx_free(&stx);
		lex_free(&lex);
	}

	{
		str_t bnf = STR("<file> ::= <");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, bnf);

		stx_t stx = { 0 };
		stx_init(&stx, 0, 0);

		stx_rule_t rule = stx_add_rule(&stx, STRH(""));
		EXPECT_EQ(stx_rule_add_term(&stx, rule, STX_TERM_RULE(&stx, -1)), 0);

		EXPECT_EQ(prs_parse(&prs, &stx, rule, &lex), PRS_NODE_END);

		stx_free(&stx);
		lex_free(&lex);
	}

	bnf_t bnf = { 0 };
	bnf_get_stx(&bnf);

	{
		str_t sbnf = STR("<file> ::= <");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, sbnf);
		lex.tokens.cnt--; //Remove EOF

		EXPECT_EQ(prs_parse(&prs, &bnf.stx, bnf.file, &lex), PRS_NODE_END);

		lex_free(&lex);
	}

	{
		str_t sbnf = STR("<file> ::= ");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, sbnf);
		lex.tokens.cnt--; //Remove EOF

		EXPECT_EQ(prs_parse(&prs, &bnf.stx, bnf.file, &lex), PRS_NODE_END);

		lex_free(&lex);
	}

	{
		str_t sbnf = STR("<file>    ::= <bnf> EOF\n"
				 "<bnf>     ::= <rules>\n"
				 "<rules>   ::= <rule> <rules> | <rule>\n"
				 "<rule>    ::= '<' <rname> '>' <spaces> '::=' <space> <expr> NL\n"
				 "<rname>   ::= LOWER <rchars> | LOWER\n"
				 "<rchars>  ::= <rchar> <rchars> | <rchar>\n"
				 "<rchar>   ::= LOWER | '-'\n"
				 "<expr>    ::= <terms> <space> '|' <space> <expr> | <terms>\n"
				 "<terms>   ::= <term> <space> <terms> | <term>\n"
				 "<term>    ::= <literal> | <token> | '<' <rname> '>'\n"
				 "<literal> ::= \"'\" <tdouble> \"'\" | '\"' <tsingle> '\"'\n"
				 "<token>   ::= UPPER <token> | UPPER\n"
				 "<tdouble> ::= <cdouble> <tdouble> | <cdouble>\n"
				 "<tsingle> ::= <csingle> <tsingle> | <csingle>\n"
				 "<cdouble> ::= <char> | '\"'\n"
				 "<csingle> ::= <char> | \"'\"\n"
				 "<char>    ::= ALPHA | DIGIT | SYMBOL | COMMA | <space>\n"
				 "<spaces>  ::= <space> <spaces> | <space>\n"
				 "<space>   ::= ' '\n");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, sbnf);

		prs_free(&prs);
		prs_init(&prs, 20000);

		prs_node_t root = prs_parse(&prs, &bnf.stx, bnf.file, &lex);
		EXPECT_EQ(root, 0);
		char *buf = malloc(160000);
		EXPECT_EQ(prs_print(&prs, root, PRINT_DST_BUF(buf, 160000, 0)), 93372);
		free(buf);

		lex_free(&lex);
	}

	bnf_free(&bnf);
	prs_free(&prs);

	END;
}

TEST(t_prs_parse)
{
	SSTART;

	RUN(t_prs_parse_cache);
	RUN(t_prs_parse_bnf);

	SEND;
}

TEST(t_prs_print)
{
	START;

	prs_t prs = { 0 };
	prs_init(&prs, 0);

	stx_t stx = { 0 };
	stx_init(&stx, 1, 1);
	stx_rule_t rule = stx_add_rule(&stx, STR("rule"));

	lex_t lex = { 0 };
	lex_init(&lex, 1);
	lex_token_t token = lex_add_token(&lex);

	*lex_get_token(&lex, token) = (token_t){
		.value	    = STR("T"),
		.line_start = 0,
		.line	    = 0,
		.col	    = 0,
		.type	    = (1 << TOKEN_ALPHA) | (1 << TOKEN_UPPER),
	};

	prs.stx = &stx;
	prs.lex = &lex;

	prs_node_t root = prs_add_node(&prs, PRS_NODE_END, PRS_NODE_RULE(&prs, rule));
	prs_add_node(&prs, root, PRS_NODE_TOKEN(&prs, token));
	prs_add_node(&prs, root, PRS_NODE_LITERAL(&prs, STR("L")));
	prs_add_node(&prs, root, prs_add(&prs, (prs_node_data_t){ .type = PRS_NODE_UNKNOWN }));

	char buf[64] = { 0 };
	EXPECT_EQ(prs_print(NULL, PRS_NODE_END, PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);

	EXPECT_EQ(prs_print(&prs, root, PRINT_DST_BUF(buf, sizeof(buf), 0)), 31);
	EXPECT_STR(buf, "rule\n"
			"├─'T'\n"
			"├─'L'\n"
			"└─");

	lex_free(&lex);
	stx_free(&stx);
	prs_free(&prs);

	END;
}

STEST(t_parser)
{
	SSTART;

	RUN(t_prs_init_free);
	RUN(t_prs_add_node);
	RUN(t_prs_remove_node);
	RUN(t_prs_get_rule);
	RUN(t_prs_get_str);
	RUN(t_prs_parse);
	RUN(t_prs_print);

	SEND;
}
