#include "t_cutils_c.h"

#include "bnf.h"
#include "cstr.h"
#include "mem.h"

TEST(t_bnf_init_free)
{
	START;

	bnf_t bnf = { 0 };

	EXPECT_EQ(bnf_init(NULL), NULL);
	EXPECT_EQ(bnf_init(&bnf), &bnf);

	bnf_free(&bnf);
	bnf_free(NULL);

	END;
}

TEST(t_bnf_get_stx)
{
	START;

	bnf_t bnf = { 0 };
	stx_t stx = { 0 };

	EXPECT_EQ(bnf_get_stx(NULL, NULL), NULL);
	mem_oom(1);
	EXPECT_EQ(bnf_get_stx(&bnf, &stx), NULL);
	mem_oom(0);
	EXPECT_EQ(bnf_get_stx(&bnf, &stx), &stx);

	char buf[1024] = { 0 };
	EXPECT_EQ(stx_print(&stx, PRINT_DST_BUF(buf, sizeof(buf), 0)), 902);
	EXPECT_STR(buf, "<file>        ::= <bnf> EOF\n"
			"<bnf>         ::= <rules>\n"
			"<rules>       ::= <rule> <rules> | <rule>\n"
			"<rule>        ::= '<' <rule-name> '>' <spaces> '::=' <space> <expression> NL\n"
			"<rule-name>   ::= LOWER <rule-chars> | LOWER\n"
			"<rule-chars>  ::= <rule-char> <rule-chars> | <rule-char>\n"
			"<rule-char>   ::= LOWER | '-'\n"
			"<expression>  ::= <terms> <space> '|' <space> <expression> | <terms>\n"
			"<terms>       ::= <term> <space> <terms> | <term>\n"
			"<term>        ::= <literal> | <token> | '<' <rule-name> '>'\n"
			"<literal>     ::= \"'\" <text-double> \"'\" | '\"' <text-single> '\"'\n"
			"<token>       ::= UPPER <token> | UPPER\n"
			"<text-double> ::= <char-double> <text-double> | <char-double>\n"
			"<text-single> ::= <char-single> <text-single> | <char-single>\n"
			"<char-double> ::= <character> | '\"'\n"
			"<char-single> ::= <character> | \"'\"\n"
			"<character>   ::= ALPHA | DIGIT | SYMBOL | <space>\n"
			"<spaces>      ::= <space> <spaces> | <space>\n"
			"<space>       ::= ' '\n");

	stx_free(&stx);

	END;
}

TEST(t_stx_from_bnf)
{
	START;

	bnf_t bnf = { 0 };
	stx_t stx = { 0 };

	bnf_get_stx(&bnf, &stx);

	str_t sbnf = STR("<file>        ::= <bnf> EOF\n"
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
	lex_init(&lex, 100);
	lex_tokenize(&lex, sbnf);

	prs_t prs = { 0 };
	prs_init(&prs, 100);

	prs_parse(&prs, &stx, &lex);

	stx_t new_stx = { 0 };
	stx_init(&new_stx, 10, 10);
	stx_from_bnf(&bnf, &new_stx, &prs);

	lex_free(&lex);
	prs_free(&prs);
	stx_free(&new_stx);
	stx_free(&stx);

	END;
}

TEST(t_stx_from_bnf_custom)
{
	START;

	bnf_t bnf = { 0 };
	stx_t stx = { 0 };

	bnf_get_stx(&bnf, &stx);

	prs_t prs = { 0 };
	prs_init(&prs, 100);

	stx_t new_stx = { 0 };
	stx_init(&new_stx, 10, 10);

	prs.stx = &stx;

	prs.root	   = prs_add_node(&prs, prs.root, PRS_NODE_RULE(bnf.file));
	prs_node_t nbnf	   = prs_add_node(&prs, prs.root, PRS_NODE_RULE(bnf.bnf));
	prs_node_t rules   = prs_add_node(&prs, nbnf, PRS_NODE_RULE(bnf.rules));
	prs_node_t rule	   = prs_add_node(&prs, rules, PRS_NODE_RULE(bnf.rule));
	prs_node_t expr	   = prs_add_node(&prs, rule, PRS_NODE_RULE(bnf.expression));
	prs_node_t terms0  = prs_add_node(&prs, expr, PRS_NODE_RULE(bnf.terms));
	prs_node_t term0   = prs_add_node(&prs, terms0, PRS_NODE_RULE(bnf.term));
	prs_node_t literal = prs_add_node(&prs, term0, PRS_NODE_RULE(bnf.literal));
	prs_node_t terms1  = prs_add_node(&prs, terms0, PRS_NODE_RULE(bnf.terms));
	prs_node_t term1   = prs_add_node(&prs, terms1, PRS_NODE_RULE(bnf.term));

	stx_from_bnf(&bnf, &new_stx, &prs);

	prs_free(&prs);
	stx_free(&new_stx);
	stx_free(&stx);

	END;
}

STEST(t_bnf)
{
	SSTART;

	RUN(t_bnf_init_free);
	RUN(t_bnf_get_stx);
	RUN(t_stx_from_bnf);
	RUN(t_stx_from_bnf_custom);

	SEND;
}
