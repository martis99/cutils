#ifndef BNF_H
#define BNF_H

#include "lexer.h"
#include "syntax.h"

typedef struct bnf_s {
	lex_t lex;
} bnf_t;

bnf_t *bnf_init(bnf_t *bnf, uint tokens_cap);
void bnf_free(bnf_t *bnf);

stx_t *bnf_get_stx(stx_t *stx);

#endif
