#include "t_cutils_c.h"

#include "bnf.h"
#include "cstr.h"
#include "file.h"
#include "mem.h"
#include "parser.h"

#define TEST_FILE "t_parser.txt"

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

TEST(t_prs_parse)
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
		stx_rule_add_term(&stx, rule, (stx_term_data_t){ .type = -1 });

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
		EXPECT_EQ(stx_rule_add_term(&stx, rule, STX_TERM_RULE(-1)), 0);

		EXPECT_EQ(prs_parse(&prs, &stx, &lex), 1);

		stx_free(&stx);
		lex_free(&lex);
	}

	stx_t stx = { 0 };
	bnf_get_stx(&stx);

	{
		str_t bnf = STR("<file> ::= <");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, bnf);
		lex.tokens.cnt--; //Remove EOF

		EXPECT_EQ(prs_parse(&prs, &stx, &lex), 1);

		lex_free(&lex);
	}

	{
		str_t bnf = STR("<file> ::= ");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, bnf);
		lex.tokens.cnt--; //Remove EOF

		EXPECT_EQ(prs_parse(&prs, &stx, &lex), 1);

		lex_free(&lex);
	}

	{
		str_t bnf = STR("<file>        ::= <bnf> EOF\n"
				"<bnf>         ::= <rules>\n"
				"<rules>       ::= <rule> <rules> | <rule>\n"
				"<rule>        ::= '<' <rule-name> '>' <spaces> '::=' <space> <expression> NL\n"
				"<rule-name>   ::= LOWER <rule-chars> | LOWER\n"
				"<rule-chars>  ::= <rule-char> <rule-chars> | <rule-char>\n"
				"<rule-char>   ::= LOWER | '-'\n"
				"<expression>  ::= <terms> <space> '|' <space> <expression> | <terms>\n"
				"<terms>       ::= <term> <terms> | <term>\n"
				"<term>        ::= <literal> | <token> | '<' <rule-name> '>'\n"
				"<literal>     ::= \"'\" <text-double> \"'\" | '\"' <text-single> '\"'\n"
				"<token>       ::= UPPER <token> | UPPER\n"
				"<text-double> ::= <char-double> <text-double> | <char-double>\n"
				"<text-single> ::= <char-single> <text-single> | <char-single>\n"
				"<char-double> ::= <character> | '\"'\n"
				"<char-single> ::= <character> | \"'\"\n"
				"<character>   ::= ALPHA | DIGIT | SYMBOL\n"
				"<spaces>      ::= <space> <spaces> | <space>\n"
				"<space>       ::= ' '\n");

		lex_t lex = { 0 };
		lex_init(&lex, 1);
		lex_tokenize(&lex, bnf);

		EXPECT_EQ(prs_parse(&prs, &stx, &lex), 0);

		lex_free(&lex);
	}

	stx_free(&stx);
	prs_free(&prs);

	END;
}

TESTP(t_prs_print, FILE *file)
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

	EXPECT_EQ(prs_print(NULL, NULL), 0);
	EXPECT_EQ(prs_print(&prs, NULL), 0);

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(prs_print(&prs, file), 0);

		char buf[128] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "rule\n"
				   "├─'T'\n"
				   "├─'L'\n"
				   "├─0\n"
				   "└─";
		EXPECT_STR(buf, exp);
	}

	lex_free(&lex);
	stx_free(&stx);
	prs_free(&prs);

	END;
}

STEST(t_parser)
{
	SSTART;

	FILE *file = file_open(TEST_FILE, "wb+");

	RUN(t_prs_init_free);
	RUN(t_prs_add_node);
	RUN(t_prs_set_node);
	RUN(t_prs_remove_node);
	RUN(t_prs_parse);
	RUNP(t_prs_print, file);

	file_close(file);
	file_delete(TEST_FILE);

	SEND;
}
