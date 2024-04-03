#include "t_cutils_c.h"

#include "bnf.h"
#include "cstr.h"
#include "file.h"
#include "mem.h"
#include "parser.h"

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

	EXPECT_EQ(prs_add_node(NULL, PRS_NODE_END, PRS_NODE_RULE(STX_RULE_END)), PRS_NODE_END);
	mem_oom(1);
	EXPECT_EQ(prs_add_node(&prs, PRS_NODE_END, PRS_NODE_RULE(STX_RULE_END)), PRS_NODE_END);
	mem_oom(0);
	EXPECT_EQ(prs_add_node(&prs, PRS_NODE_END, PRS_NODE_RULE(STX_RULE_END)), 0);

	prs_free(&prs);

	END;
}

TEST(t_prs_set_node)
{
	START;

	prs_t prs = { 0 };
	prs_init(&prs, 0);

	prs_node_t parent = prs_add_node(&prs, PRS_NODE_END, PRS_NODE_RULE(STX_RULE_END));
	prs_node_t child  = prs_add_node(&prs, PRS_NODE_END, PRS_NODE_RULE(STX_RULE_END));

	EXPECT_EQ(prs_set_node(NULL, PRS_NODE_END, PRS_NODE_END), PRS_NODE_END);
	EXPECT_EQ(prs_set_node(&prs, PRS_NODE_END, PRS_NODE_END), PRS_NODE_END);
	EXPECT_EQ(prs_set_node(&prs, parent, PRS_NODE_END), PRS_NODE_END);
	EXPECT_EQ(prs_set_node(&prs, parent, child), child);

	prs_free(&prs);

	END;
}

TEST(t_prs_remove_node)
{
	START;

	prs_t prs = { 0 };
	prs_init(&prs, 0);

	prs_node_t node = prs_add_node(&prs, PRS_NODE_END, PRS_NODE_RULE(STX_RULE_END));

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

	prs.root       = prs_add_node(&prs, prs.root, PRS_NODE_LITERAL(""));
	prs_node_t alt = prs_add_node(&prs, prs.root, PRS_NODE_ALT(0));

	prs_add_node(&prs, alt, PRS_NODE_LITERAL(""));
	prs_add_node(&prs, alt, PRS_NODE_RULE(rule0));
	prs_node_t node = prs_add_node(&prs, alt, PRS_NODE_RULE(rule1));

	EXPECT_EQ(prs_get_rule(NULL, PRS_NODE_END, STX_RULE_END), PRS_NODE_END);
	EXPECT_EQ(prs_get_rule(&prs, PRS_NODE_END, STX_RULE_END), PRS_NODE_END);
	EXPECT_EQ(prs_get_rule(&prs, prs.root, STX_RULE_END), PRS_NODE_END);
	EXPECT_EQ(prs_get_rule(&prs, prs.root, rule1), node);

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

	prs.root       = prs_add_node(&prs, prs.root, PRS_NODE_RULE(rule0));
	prs_node_t alt = prs_add_node(&prs, prs.root, PRS_NODE_ALT(0));

	prs_add_node(&prs, alt, PRS_NODE_LITERAL(STR("a")));
	prs_add_node(&prs, alt, PRS_NODE_TOKEN(token));
	prs_node_t node = prs_add_node(&prs, alt, PRS_NODE_RULE(rule1));

	str_t str = strz(16);

	EXPECT_EQ(prs_get_str(NULL, PRS_NODE_END, NULL), 1);
	EXPECT_EQ(prs_get_str(&prs, PRS_NODE_END, NULL), 1);
	EXPECT_EQ(prs_get_str(&prs, prs.root, &str), 0);
	EXPECT_STR(str.data, "a");
	str.len = 0;
	prs.lex = &lex;
	EXPECT_EQ(prs_get_str(&prs, prs.root, &str), 0);

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

	EXPECT_EQ(prs_parse(&prs, &stx, &lex), 0);

	char buf[512] = { 0 };
	EXPECT_EQ(prs_print(&prs, PRINT_DST_BUF(buf, sizeof(buf), 0)), 72);
	EXPECT_STR(buf, "file\n"
			"├─line\n"
			"│ └─1\n"
			"│   └─ra\n"
			"│     └─'a'\n"
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
	prs_init(&prs, 20000);

	EXPECT_EQ(prs_parse(NULL, NULL, NULL), 1);

	{
		str_t bnf = STR("<file> ::= <");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, bnf);

		stx_t stx = { 0 };
		stx_init(&stx, 0, 0);

		stx_rule_t rule = stx_add_rule(&stx, STRH(""));
		stx_rule_add_term(&stx, rule, stx_create_term(&stx, (stx_term_data_t){ .type = -1 }));

		EXPECT_EQ(prs_parse(&prs, &stx, &lex), 1);

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

		EXPECT_EQ(prs_parse(&prs, &stx, &lex), 1);

		stx_free(&stx);
		lex_free(&lex);
	}

	bnf_t bnf	 = { 0 };
	const stx_t *stx = bnf_get_stx(&bnf);

	{
		str_t bnf = STR("<file> ::= <");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, bnf);
		lex.tokens.cnt--; //Remove EOF

		EXPECT_EQ(prs_parse(&prs, stx, &lex), 1);

		lex_free(&lex);
	}

	{
		str_t bnf = STR("<file> ::= ");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, bnf);
		lex.tokens.cnt--; //Remove EOF

		EXPECT_EQ(prs_parse(&prs, stx, &lex), 1);

		lex_free(&lex);
	}

	{
		str_t bnf = STR("<file>    ::= <bnf> EOF\n"
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
				"<char>    ::= ALPHA | DIGIT | SYMBOL | <space>\n"
				"<spaces>  ::= <space> <spaces> | <space>\n"
				"<space>   ::= ' '\n");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, bnf);

		EXPECT_EQ(prs_parse(&prs, stx, &lex), 0);
		char *buf = malloc(320000);
		EXPECT_EQ(prs_print(&prs, PRINT_DST_BUF(buf, 320000, 0)), 223672);
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

	prs.root = prs_add_node(&prs, PRS_NODE_END, PRS_NODE_RULE(rule));
	prs_add_node(&prs, prs.root, PRS_NODE_TOKEN(token));
	prs_add_node(&prs, prs.root, PRS_NODE_LITERAL(STR("L")));
	prs_add_node(&prs, prs.root, PRS_NODE_ALT(0));
	prs_add_node(&prs, prs.root, (prs_node_data_t){ .type = -1 });

	char buf[64] = { 0 };
	EXPECT_EQ(prs_print(NULL, PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);
	EXPECT_EQ(prs_print_node(NULL, 0, PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);

	EXPECT_EQ(prs_print(&prs, PRINT_DST_BUF(buf, sizeof(buf), 0)), 39);
	EXPECT_STR(buf, "rule\n"
			"├─'T'\n"
			"├─'L'\n"
			"├─0\n"
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
	RUN(t_prs_set_node);
	RUN(t_prs_remove_node);
	RUN(t_prs_get_rule);
	RUN(t_prs_get_str);
	RUN(t_prs_parse);
	RUN(t_prs_print);

	SEND;
}
