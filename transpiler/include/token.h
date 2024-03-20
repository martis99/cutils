#ifndef TOKEN_H
#define TOKEN_H

#include "print.h"
#include "str.h"

typedef enum token_type_e {
	TOKEN_UNKNOWN,

	TOKEN_ALPHA,
	TOKEN_UPPER,
	TOKEN_LOWER,

	TOKEN_DIGIT,

	TOKEN_SINGLE_QUOTE,
	TOKEN_DOUBLE_QUOTE,

	TOKEN_WS,
	TOKEN_SPACE,
	TOKEN_TAB,
	TOKEN_CR,
	TOKEN_NL,

	TOKEN_NULL,

	TOKEN_SYMBOL,

	TOKEN_EOF,

	__TOKEN_MAX,
} token_type_t;

typedef struct token_s {
	str_t value;
	uint line_start;
	uint line;
	uint col;
	uint type;
} token_t;

str_t token_type_str(token_type_t type);
token_type_t token_type_enum(str_t str);

int token_dbg(token_t token, print_dst_t dst);

#endif
