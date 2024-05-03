#include "t_cutils_c.h"

#include "cstr.h"
#include "ebnf.h"
#include "eparser.h"
#include "file.h"
#include "mem.h"

#include <stdlib.h>

TEST(t_eprs_init_free)
{
	START;

	eprs_t eprs = { 0 };

	EXPECT_EQ(eprs_init(NULL, 0), NULL);
	mem_oom(1);
	EXPECT_EQ(eprs_init(&eprs, 1), NULL);
	mem_oom(0);
	EXPECT_EQ(eprs_init(&eprs, 0), &eprs);

	EXPECT_NE(eprs.nodes.data, NULL);

	eprs_free(&eprs);
	eprs_free(NULL);

	EXPECT_EQ(eprs.nodes.data, NULL);

	END;
}

TEST(t_eprs_add_node)
{
	START;

	eprs_t eprs = { 0 };
	eprs_init(&eprs, 0);

	EXPECT_EQ(eprs_add(NULL, (eprs_node_data_t){ .type = EPRS_NODE_UNKNOWN }), EPRS_NODE_END);
	mem_oom(1);
	EXPECT_EQ(eprs_add(&eprs, (eprs_node_data_t){ .type = EPRS_NODE_UNKNOWN }), EPRS_NODE_END);
	mem_oom(0);

	eprs_node_t parent = eprs_add(&eprs, (eprs_node_data_t){ .type = EPRS_NODE_UNKNOWN });
	eprs_node_t child  = eprs_add(&eprs, (eprs_node_data_t){ .type = EPRS_NODE_UNKNOWN });

	EXPECT_EQ(eprs_add_node(NULL, EPRS_NODE_END, EPRS_NODE_END), EPRS_NODE_END);
	EXPECT_EQ(eprs_add_node(&eprs, EPRS_NODE_END, EPRS_NODE_END), EPRS_NODE_END);
	EXPECT_EQ(eprs_add_node(&eprs, parent, EPRS_NODE_END), EPRS_NODE_END);
	EXPECT_EQ(eprs_add_node(&eprs, parent, child), child);

	eprs_free(&eprs);

	END;
}

TEST(t_eprs_remove_node)
{
	START;

	eprs_t eprs = { 0 };
	eprs_init(&eprs, 0);

	eprs_node_t node = eprs_add_node(&eprs, EPRS_NODE_END, eprs_add(&eprs, (eprs_node_data_t){ .type = EPRS_NODE_UNKNOWN }));

	EXPECT_EQ(eprs_remove_node(NULL, EPRS_NODE_END), 1);
	EXPECT_EQ(eprs_remove_node(&eprs, EPRS_NODE_END), 1);
	EXPECT_EQ(eprs_remove_node(&eprs, node), 0);

	eprs_free(&eprs);

	END;
}

TEST(t_eprs_get_rule)
{
	START;

	eprs_t eprs = { 0 };
	eprs_init(&eprs, 0);

	estx_t estx = { 0 };
	estx_init(&estx, 1, 1);

	estx_rule_t rule0 = estx_add_rule(&estx, STR("rule0"));

	eprs_node_t root = eprs_add_node(&eprs, EPRS_NODE_END, EPRS_NODE_LITERAL(&eprs, ""));

	eprs_add_node(&eprs, root, EPRS_NODE_LITERAL(&eprs, ""));
	eprs_node_t node = eprs_add_node(&eprs, root, EPRS_NODE_RULE(&eprs, rule0));

	EXPECT_EQ(eprs_get_rule(NULL, EPRS_NODE_END, ESTX_RULE_END), EPRS_NODE_END);
	EXPECT_EQ(eprs_get_rule(&eprs, EPRS_NODE_END, ESTX_RULE_END), EPRS_NODE_END);
	EXPECT_EQ(eprs_get_rule(&eprs, root, ESTX_RULE_END), EPRS_NODE_END);
	EXPECT_EQ(eprs_get_rule(&eprs, root, rule0), node);

	estx_free(&estx);
	eprs_free(&eprs);

	END;
}

TEST(t_eprs_get_str)
{
	START;

	eprs_t eprs = { 0 };
	eprs_init(&eprs, 0);

	estx_t estx = { 0 };
	estx_init(&estx, 1, 1);

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

	estx_rule_t rule0 = estx_add_rule(&estx, STR("rule0"));

	eprs_node_t root = eprs_add_node(&eprs, EPRS_NODE_END, EPRS_NODE_RULE(&eprs, rule0));

	eprs_add_node(&eprs, root, EPRS_NODE_LITERAL(&eprs, STR("a")));
	eprs_add_node(&eprs, root, EPRS_NODE_TOKEN(&eprs, token));
	eprs_node_t node = eprs_add_node(&eprs, root, EPRS_NODE_RULE(&eprs, rule0));

	str_t str = strz(16);

	EXPECT_EQ(eprs_get_str(NULL, EPRS_NODE_END, NULL), 1);
	EXPECT_EQ(eprs_get_str(&eprs, EPRS_NODE_END, NULL), 1);
	EXPECT_EQ(eprs_get_str(&eprs, root, &str), 0);
	EXPECT_STR(str.data, "a");
	str.len	 = 0;
	eprs.lex = &lex;
	EXPECT_EQ(eprs_get_str(&eprs, root, &str), 0);

	EXPECT_STR(str.data, "ab");

	str_free(&str);
	lex_free(&lex);
	estx_free(&estx);
	eprs_free(&eprs);

	END;
}

TEST(t_eprs_parse_cache)
{
	START;

	eprs_t eprs = { 0 };
	eprs_init(&eprs, 100);

	lex_t lex = { 0 };
	lex_init(&lex, 100);

	estx_t estx = { 0 };
	estx_init(&estx, 10, 10);

	const estx_rule_t file = estx_add_rule(&estx, STR("file"));
	const estx_rule_t line = estx_add_rule(&estx, STR("line"));
	const estx_rule_t ra   = estx_add_rule(&estx, STR("ra"));

	estx_rule_set_term(&estx, file, ESTX_TERM_RULE(&estx, line, ESTX_TERM_OCC_ONE));
	estx_term_add_term(&estx, file, ESTX_TERM_TOKEN(&estx, TOKEN_EOF, ESTX_TERM_OCC_ONE));
	estx_rule_set_term(&estx, line, ESTX_TERM_RULE(&estx, ra, ESTX_TERM_OCC_OPT | ESTX_TERM_OCC_REP));
	estx_rule_set_term(&estx, ra, ESTX_TERM_LITERAL(&estx, STR("a"), ESTX_TERM_OCC_ONE));

	str_t bnf = STR("a");

	lex_tokenize(&lex, bnf);
	lex.tokens.cnt--; //Remove EOF

	eprs_node_t root = eprs_parse(&eprs, &estx, file, &lex);
	EXPECT_EQ(root, 0);

	char buf[512] = { 0 };
	EXPECT_EQ(eprs_print(&eprs, root, PRINT_DST_BUF(buf, sizeof(buf), 0)), 41);
	EXPECT_STR(buf, "file\n"
			"└─line\n"
			"  └─ra\n"
			"    └─'a'\n");

	eprs_free(&eprs);
	lex_free(&lex);
	estx_free(&estx);

	END;
}

TEST(t_eprs_parse_loop)
{
	START;

	eprs_t eprs = { 0 };
	eprs_init(&eprs, 100);

	lex_t lex = { 0 };
	lex_init(&lex, 100);

	estx_t estx = { 0 };
	estx_init(&estx, 10, 10);

	const estx_rule_t file = estx_add_rule(&estx, STR("file"));
	const estx_rule_t line = estx_add_rule(&estx, STR("line"));
	const estx_rule_t val  = estx_add_rule(&estx, STR("val"));

	estx_rule_set_term(&estx, file, ESTX_TERM_RULE(&estx, line, ESTX_TERM_OCC_OPT | ESTX_TERM_OCC_REP));
	estx_term_add_term(&estx, file, ESTX_TERM_TOKEN(&estx, TOKEN_EOF, ESTX_TERM_OCC_ONE));
	estx_rule_set_term(&estx, line, ESTX_TERM_RULE(&estx, val, ESTX_TERM_OCC_OPT));
	estx_rule_set_term(&estx, val, ESTX_TERM_LITERAL(&estx, STR("a"), ESTX_TERM_OCC_ONE));

	str_t bnf = STR("a");

	lex_tokenize(&lex, bnf);
	lex.tokens.cnt--; //Remove EOF

	eprs_node_t root = eprs_parse(&eprs, &estx, file, &lex);
	EXPECT_EQ(root, 0);

	char buf[512] = { 0 };
	EXPECT_EQ(eprs_print(&eprs, root, PRINT_DST_BUF(buf, sizeof(buf), 0)), 57);
	EXPECT_STR(buf, "file\n"
			"├─line\n"
			"│ └─val\n"
			"│   └─'a'\n"
			"└─line\n");

	eprs_free(&eprs);
	lex_free(&lex);
	estx_free(&estx);

	END;
}

TEST(t_eprs_parse_ebnf)
{
	START;

	eprs_t eprs = { 0 };
	eprs_init(&eprs, 256);

	EXPECT_EQ(eprs_parse(NULL, NULL, ESTX_RULE_END, NULL), EPRS_NODE_END);

	{
		str_t bnf = STR("<file> ::= <");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, bnf);

		estx_t estx = { 0 };
		estx_init(&estx, 0, 0);

		estx_rule_t rule = estx_add_rule(&estx, STRH(""));
		estx_rule_set_term(&estx, rule, estx_create_term(&estx, (estx_term_data_t){ .type = -1 }));

		EXPECT_EQ(eprs_parse(&eprs, &estx, rule, &lex), EPRS_NODE_END);

		estx_free(&estx);
		lex_free(&lex);
	}

	{
		str_t bnf = STR("<file> ::= <");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, bnf);

		estx_t estx = { 0 };
		estx_init(&estx, 0, 0);

		estx_rule_t rule = estx_add_rule(&estx, STRH(""));
		EXPECT_EQ(estx_rule_set_term(&estx, rule, ESTX_TERM_RULE(&estx, -1, 0)), 0);

		EXPECT_EQ(eprs_parse(&eprs, &estx, rule, &lex), EPRS_NODE_END);

		estx_free(&estx);
		lex_free(&lex);
	}

	{
		str_t bnf = STR("::");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, bnf);

		estx_t estx = { 0 };
		estx_init(&estx, 0, 0);

		estx_rule_t rule = estx_add_rule(&estx, STRH(""));
		estx_rule_set_term(&estx, rule, ESTX_TERM_LITERAL(&estx, STRH("::="), ESTX_TERM_OCC_ONE));

		EXPECT_EQ(eprs_parse(&eprs, &estx, rule, &lex), EPRS_NODE_END);

		estx_free(&estx);
		lex_free(&lex);
	}

	estx_t estx = { 0 };
	lex_t lex   = { 0 };
	prs_t prs   = { 0 };
	estx_rule_t estx_root;

	{
		ebnf_t ebnf = { 0 };
		ebnf_get_stx(&ebnf);

		str_t sbnf = STR("file    = ebnf EOF\n"
				 "ebnf    = rule+\n"
				 "rule    = rname spaces '= ' alt NL\n"
				 "rname   = LOWER (LOWER | '_')*\n"
				 "alt     = concat (' | ' concat)*\n"
				 "concat  = factor (' ' factor)*\n"
				 "factor  = term (opt | rep | opt_rep)?\n"
				 "opt     = '?'\n"
				 "rep     = '+'\n"
				 "opt_rep = '*'\n"
				 "term    = literal | token | rname | group\n"
				 "literal = \"'\" (char | '\"')+ \"'\" | '\"' (char | \"'\")+ '\"'\n"
				 "token   = UPPER+\n"
				 "group   = '(' alt ')'\n"
				 "char    = ALPHA | DIGIT | SYMBOL | ' '\n"
				 "spaces  = ' '+\n");

		lex_init(&lex, 100);
		lex_tokenize(&lex, sbnf);

		prs_init(&prs, 100);

		prs_node_t prs_root = prs_parse(&prs, &ebnf.stx, ebnf.file, &lex);

		estx_init(&estx, 10, 10);
		estx_root = estx_from_ebnf(&ebnf, &prs, prs_root, &estx);
		ebnf_free(&ebnf);
	}

	{
		str_t sbnf = STR("<file> ::= ");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, sbnf);
		lex.tokens.cnt--; //Remove EOF

		EXPECT_EQ(eprs_parse(&eprs, &estx, estx_root, &lex), EPRS_NODE_END);

		lex_free(&lex);
	}

	{
		str_t sebnf = STR("");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, sebnf);
		lex.tokens.cnt--; //Remove EOF

		EXPECT_EQ(eprs_parse(&eprs, &estx, estx_root, &lex), EPRS_NODE_END);

		lex_free(&lex);
	}

	{
		str_t sbnf = STR("file    = ebnf EOF\n"
				 "ebnf    = rule+\n"
				 "rule    = rname spaces '= ' alt NL\n"
				 "rname   = LOWER (LOWER | '_')*\n"
				 "alt     = concat (' | ' concat)*\n"
				 "concat  = factor (' ' factor)*\n"
				 "factor  = term (opt | rep | opt_rep)?\n"
				 "opt     = '?'\n"
				 "rep     = '+'\n"
				 "opt_rep = '*'\n"
				 "term    = literal | token | rname | group\n"
				 "literal = \"'\" (char | '\"')+ \"'\" | '\"' (char | \"'\")+ '\"'\n"
				 "token   = UPPER+\n"
				 "group   = '(' alt ')'\n"
				 "char    = ALPHA | DIGIT | SYMBOL | ' '\n"
				 "spaces  = ' '+\n");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, sbnf);

		eprs_free(&eprs);
		eprs_init(&eprs, 20000);

		eprs_node_t root = eprs_parse(&eprs, &estx, estx_root, &lex);
		EXPECT_EQ(root, 0);
		char *buf = malloc(80000);
		EXPECT_EQ(eprs_print(&eprs, root, PRINT_DST_BUF(buf, 80000, 0)), 33054);
		free(buf);

		lex_free(&lex);
	}

	estx_free(&estx);
	lex_free(&lex);
	prs_free(&prs);
	eprs_free(&eprs);

	END;
}

TEST(t_eprs_parse)
{
	SSTART;

	RUN(t_eprs_parse_cache);
	RUN(t_eprs_parse_loop);
	RUN(t_eprs_parse_ebnf);

	SEND;
}

TEST(t_eprs_print)
{
	START;

	eprs_t eprs = { 0 };
	eprs_init(&eprs, 0);

	estx_t estx = { 0 };
	estx_init(&estx, 1, 1);
	estx_rule_t rule = estx_add_rule(&estx, STR("rule"));

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

	eprs.estx = &estx;
	eprs.lex  = &lex;

	eprs_node_t root = eprs_add_node(&eprs, EPRS_NODE_END, EPRS_NODE_RULE(&eprs, rule));
	eprs_add_node(&eprs, root, EPRS_NODE_TOKEN(&eprs, token));
	eprs_add_node(&eprs, root, EPRS_NODE_LITERAL(&eprs, STR("L")));
	eprs_add_node(&eprs, root, eprs_add(&eprs, (eprs_node_data_t){ .type = EPRS_NODE_UNKNOWN }));

	char buf[64] = { 0 };
	EXPECT_EQ(eprs_print(NULL, EPRS_NODE_END, PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);

	EXPECT_EQ(eprs_print(&eprs, root, PRINT_DST_BUF(buf, sizeof(buf), 0)), 31);
	EXPECT_STR(buf, "rule\n"
			"├─'T'\n"
			"├─'L'\n"
			"└─");

	lex_free(&lex);
	estx_free(&estx);
	eprs_free(&eprs);

	END;
}

STEST(t_eparser)
{
	SSTART;

	RUN(t_eprs_init_free);
	RUN(t_eprs_add_node);
	RUN(t_eprs_remove_node);
	RUN(t_eprs_get_rule);
	RUN(t_eprs_get_str);
	RUN(t_eprs_parse);
	RUN(t_eprs_print);

	SEND;
}
