#include "t_cutils_c.h"

#include "cstr.h"
#include "ebnf.h"
#include "mem.h"

TEST(t_ebnf_init_free)
{
	START;

	ebnf_t ebnf = { 0 };

	EXPECT_EQ(ebnf_init(NULL), NULL);
	EXPECT_EQ(ebnf_init(&ebnf), &ebnf);

	ebnf_free(&ebnf);
	ebnf_free(NULL);

	END;
}

TEST(t_ebnf_get_stx)
{
	START;

	ebnf_t ebnf = { 0 };

	EXPECT_EQ(ebnf_get_stx(NULL), NULL);
	mem_oom(1);
	EXPECT_EQ(ebnf_get_stx(&ebnf), NULL);
	mem_oom(0);
	EXPECT_NE(ebnf_get_stx(&ebnf), NULL);

	char buf[1024] = { 0 };
	EXPECT_EQ(stx_print(&ebnf.stx, PRINT_DST_BUF(buf, sizeof(buf), 0)), 894);
	EXPECT_STR(buf, "<file>    ::= <ebnf> EOF\n"
			"<ebnf>    ::= <rules>\n"
			"<rules>   ::= <rule> <rules> | <rule>\n"
			"<rule>    ::= <rname> <spaces> '=' <space> <alt> NL\n"
			"<rname>   ::= LOWER <rchars> | LOWER\n"
			"<spaces>  ::= <space> <spaces> | <space>\n"
			"<space>   ::= ' '\n"
			"<alt>     ::= <concat> <space> '|' <space> <alt> | <concat>\n"
			"<rchars>  ::= <rchar> <rchars> | <rchar>\n"
			"<rchar>   ::= LOWER | '_'\n"
			"<concat>  ::= <factor> <space> <concat> | <factor>\n"
			"<factor>  ::= <term> <opt> | <term> <rep> | <term> <opt-rep> | <term>\n"
			"<term>    ::= <literal> | <token> | <rname> | <group>\n"
			"<opt>     ::= '?'\n"
			"<rep>     ::= '+'\n"
			"<opt-rep> ::= '*'\n"
			"<literal> ::= \"'\" <tdouble> \"'\" | '\"' <tsingle> '\"'\n"
			"<token>   ::= UPPER <token> | UPPER\n"
			"<group>   ::= '(' <alt> ')'\n"
			"<tdouble> ::= <cdouble> <tdouble> | <cdouble>\n"
			"<tsingle> ::= <csingle> <tsingle> | <csingle>\n"
			"<cdouble> ::= <char> | '\"'\n"
			"<csingle> ::= <char> | \"'\"\n"
			"<char>    ::= ALPHA | DIGIT | SYMBOL | ' '\n");

	ebnf_free(&ebnf);

	END;
}

TEST(t_stx_from_ebnf)
{
	START;

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

	lex_t lex = { 0 };
	lex_init(&lex, 100);
	lex_tokenize(&lex, sbnf);

	prs_t prs = { 0 };
	prs_init(&prs, 100);

	prs_node_t prs_root = prs_parse(&prs, &ebnf.stx, ebnf.file, &lex);

	estx_t new_stx = { 0 };
	estx_init(&new_stx, 10, 10);
	EXPECT_EQ(estx_from_ebnf(&ebnf, &prs, prs_root, &new_stx), 0);

	estx_free(&new_stx);
	lex_free(&lex);
	prs_free(&prs);
	ebnf_free(&ebnf);

	END;
}

TEST(t_stx_from_ebnf_custom)
{
	START;

	ebnf_t ebnf = { 0 };
	ebnf_get_stx(&ebnf);

	prs_t prs = { 0 };
	prs_init(&prs, 100);

	estx_t new_stx = { 0 };
	estx_init(&new_stx, 10, 10);

	prs.stx = &ebnf.stx;

	prs_node_t file	   = prs_add_node(&prs, PRS_NODE_END, PRS_NODE_RULE(ebnf.file));
	prs_node_t pebnf   = prs_add_node(&prs, file, PRS_NODE_RULE(ebnf.ebnf));
	prs_node_t rs_lit  = prs_add_node(&prs, pebnf, PRS_NODE_RULE(ebnf.rules));
	prs_node_t r_lit   = prs_add_node(&prs, rs_lit, PRS_NODE_RULE(ebnf.rule));
	prs_node_t a_lit   = prs_add_node(&prs, r_lit, PRS_NODE_RULE(ebnf.alt));
	prs_node_t c_lit   = prs_add_node(&prs, a_lit, PRS_NODE_RULE(ebnf.concat));
	prs_node_t f_lit   = prs_add_node(&prs, c_lit, PRS_NODE_RULE(ebnf.factor));
	prs_node_t t_lit   = prs_add_node(&prs, f_lit, PRS_NODE_RULE(ebnf.term));
	prs_node_t literal = prs_add_node(&prs, t_lit, PRS_NODE_RULE(ebnf.literal));

	prs_node_t rs_grp = prs_add_node(&prs, rs_lit, PRS_NODE_RULE(ebnf.rules));
	prs_node_t r_grp  = prs_add_node(&prs, rs_grp, PRS_NODE_RULE(ebnf.rule));
	prs_node_t a_grp  = prs_add_node(&prs, r_grp, PRS_NODE_RULE(ebnf.alt));
	prs_node_t c_grp  = prs_add_node(&prs, a_grp, PRS_NODE_RULE(ebnf.concat));
	prs_node_t f_grp  = prs_add_node(&prs, c_grp, PRS_NODE_RULE(ebnf.factor));
	prs_node_t t_grp  = prs_add_node(&prs, f_grp, PRS_NODE_RULE(ebnf.term));
	prs_node_t group  = prs_add_node(&prs, t_grp, PRS_NODE_RULE(ebnf.group));

	prs_add_node(&prs, f_grp, PRS_NODE_RULE(ebnf.opt_rep));

	EXPECT_EQ(estx_from_ebnf(&ebnf, &prs, file, &new_stx), 0);

	estx_free(&new_stx);
	prs_free(&prs);
	ebnf_free(&ebnf);

	END;
}

STEST(t_ebnf)
{
	SSTART;

	RUN(t_ebnf_init_free);
	RUN(t_ebnf_get_stx);
	RUN(t_stx_from_ebnf);
	RUN(t_stx_from_ebnf_custom);

	SEND;
}
