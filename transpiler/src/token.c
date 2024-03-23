#include "token.h"

#include <stdarg.h>

static str_t token_type_strs[] = {
	[TOKEN_UNKNOWN]	     = STRS("UNKNOWN"),
	[TOKEN_ALPHA]	     = STRS("ALPHA"),
	[TOKEN_UPPER]	     = STRS("UPPER"),
	[TOKEN_LOWER]	     = STRS("LOWER"),
	[TOKEN_DIGIT]	     = STRS("DIGIT"),
	[TOKEN_SINGLE_QUOTE] = STRS("SINGLE_QUOTE"),
	[TOKEN_DOUBLE_QUOTE] = STRS("DOUBLE_QUOTE"),
	[TOKEN_WS]	     = STRS("WS"),
	[TOKEN_SPACE]	     = STRS("SPACE"),
	[TOKEN_TAB]	     = STRS("TAB"),
	[TOKEN_CR]	     = STRS("CR"),
	[TOKEN_NL]	     = STRS("NL"),
	[TOKEN_NULL]	     = STRS("NULL"),
	[TOKEN_SYMBOL]	     = STRS("SYMBOL"),
	[TOKEN_EOF]	     = STRS("EOF"),
};

str_t token_type_str(token_type_t type)
{
	return token_type_strs[type >= TOKEN_UNKNOWN && type < __TOKEN_MAX ? type : TOKEN_UNKNOWN];
}

token_type_t token_type_enum(str_t str)
{
	for (token_type_t type = TOKEN_UNKNOWN; type < __TOKEN_MAX; type++) {
		if (str_eq(str, token_type_strs[type])) {
			return type;
		}
	}

	return TOKEN_UNKNOWN;
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int token_dbg(token_t token, print_dst_t dst)
{
	int first = 1;
	int off	  = dst.off;

	for (token_type_t type = TOKEN_UNKNOWN; type < __TOKEN_MAX; type++) {
		if (token.type & (1 << type)) {
			if (first == 0) {
				dst.off += dprintf(dst, " | ");
			}
			dst.off += dprintf(dst, "%-5s", token_type_str(type).data);
			first = 0;
		}
	}

	dst.off += dprintf(dst, "%*s (%2d, %2d) \"", MAX(13 - (dst.off - off), 0), "", token.line, token.col, token.value.len, token.value.data);
	dst.off += str_print(token.value, dst);
	dst.off += dprintf(dst, "\"");

	return dst.off - off;
}
