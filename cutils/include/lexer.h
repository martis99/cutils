#ifndef LEXER_H
#define LEXER_H

#include "arr.h"
#include "print.h"
#include "str.h"
#include "token.h"

#define LEX_TOKEN_END ARR_END

typedef size_t lex_token_t;

typedef struct lex_s {
	str_t str;
	arr_t tokens;
	lex_token_t root;
} lex_t;

lex_t *lex_init(lex_t *lex, uint token_cap);
void lex_free(lex_t *lex);

lex_token_t lex_add_token(lex_t *lex);
token_t *lex_get_token(const lex_t *lex, lex_token_t token);

lex_t *lex_tokenize(lex_t *lex, str_t str);

int lex_print_line(const lex_t *lex, lex_token_t token, c_printv_fn cb, size_t size, int off, void *priv);

int lex_dbg(const lex_t *lex, FILE *file);

#endif
