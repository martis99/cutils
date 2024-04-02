#ifndef BNF_H
#define BNF_H

#include "lexer.h"
#include "parser.h"
#include "syntax.h"

typedef struct bnf_s {
	stx_rule_t file;
	stx_rule_t bnf;
	stx_rule_t rules;
	stx_rule_t rule;
	stx_rule_t rule_name;
	stx_rule_t expression;
	stx_rule_t terms;
	stx_rule_t term;
	stx_rule_t literal;
	stx_rule_t token;
	stx_rule_t text_double;
	stx_rule_t text_single;
} bnf_t;

bnf_t *bnf_init(bnf_t *bnf);
void bnf_free(bnf_t *bnf);

stx_t *bnf_get_stx(bnf_t *bnf, stx_t *stx);

stx_t *stx_from_bnf(bnf_t *bnf, stx_t *stx, prs_t *prs);

#endif
