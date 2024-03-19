#include "t_cutils_c.h"

#include "bnf.h"
#include "cstr.h"
#include "mem.h"

TEST(t_bnf_init_free)
{
	START;

	bnf_t bnf = { 0 };

	EXPECT_EQ(bnf_init(NULL, 0), NULL);
	mem_oom(1);
	EXPECT_EQ(bnf_init(&bnf, 1), NULL);
	mem_oom(0);
	EXPECT_EQ(bnf_init(&bnf, 0), &bnf);

	bnf_free(&bnf);
	bnf_free(NULL);

	END;
}

TEST(t_bnf_get_stx)
{
	START;

	stx_t stx = { 0 };

	EXPECT_EQ(bnf_get_stx(NULL), NULL);
	mem_oom(1);
	EXPECT_EQ(bnf_get_stx(&stx), NULL);
	mem_oom(0);
	EXPECT_EQ(bnf_get_stx(&stx), &stx);

	{
		char buf[1024] = { 0 };
		EXPECT_EQ(stx_print(&stx, PRINT_DST_BUF(buf, sizeof(buf), 0)), 902);

		const char exp[] = "<file>        ::= <bnf> EOF\n"
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
				   "<space>       ::= ' '\n";
		EXPECT_STR(buf, exp);
	}

	stx_free(&stx);

	END;
}

STEST(t_bnf)
{
	SSTART;

	RUN(t_bnf_init_free);
	RUN(t_bnf_get_stx);

	SEND;
}
