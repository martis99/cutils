#include "lexer.h"

#include "log.h"

lex_t *lex_init(lex_t *lex, uint tokens_cap)
{
	if (lex == NULL) {
		return NULL;
	}

	if (arr_init(&lex->tokens, tokens_cap, sizeof(token_t)) == NULL) {
		log_error("cutils", "lexer", NULL, "failed to intialize tokens list");
		return NULL;
	}

	lex->root = LEX_TOKEN_END;

	return lex;
}

void lex_free(lex_t *lex)
{
	if (lex == NULL) {
		return;
	}

	arr_free(&lex->tokens);
	str_free(&lex->str);

	lex->root = LEX_TOKEN_END;
}

lex_token_t lex_add_token(lex_t *lex)
{
	if (lex == NULL) {
		return LEX_TOKEN_END;
	}

	const lex_token_t id = arr_add(&lex->tokens);
	if (id >= lex->tokens.cnt) {
		log_error("cutils", "lexer", NULL, "failed to add token");
		return LEX_TOKEN_END;
	}

	if (lex->root == LEX_TOKEN_END) {
		lex->root = id;
	}

	return id;
}

token_t *lex_get_token(const lex_t *lex, lex_token_t token)
{
	if (lex == NULL) {
		return NULL;
	}

	return arr_get(&lex->tokens, (uint)token);
}

lex_t *lex_tokenize(lex_t *lex, str_t str)
{
	if (lex == NULL) {
		return NULL;
	}

	lex->str	= str;
	lex->tokens.cnt = 0;

	uint line = 0;
	uint col  = 0;

	uint line_start = 0;

	for (size_t i = 0; i <= str.len; i++) {
		token_type_t type = TOKEN_UNKNOWN;

		char c;

		if (i == str.len) {
			c = '\0';
			type |= 1 << TOKEN_EOF;
		} else {
			c = str.data[i];

			if (c >= 'A' && c <= 'Z') {
				type |= 1 << TOKEN_ALPHA;
				type |= 1 << TOKEN_UPPER;
			} else if (c >= 'a' && c <= 'z') {
				type |= 1 << TOKEN_ALPHA;
				type |= 1 << TOKEN_LOWER;
			} else if (c >= '0' && c <= '9') {
				type |= 1 << TOKEN_DIGIT;
			} else if (c == '\'') {
				type |= 1 << TOKEN_SINGLE_QUOTE;
			} else if (c == '"') {
				type |= 1 << TOKEN_DOUBLE_QUOTE;
			} else if (c == ' ') {
				type |= 1 << TOKEN_WS;
				type |= 1 << TOKEN_SPACE;
			} else if (c == '\t') {
				type |= 1 << TOKEN_WS;
				type |= 1 << TOKEN_TAB;
			} else if (c == '\r') {
				type |= 1 << TOKEN_WS;
				type |= 1 << TOKEN_CR;
			} else if (c == '\n') {
				type |= 1 << TOKEN_WS;
				type |= 1 << TOKEN_NL;
			} else if (c == '\0') {
				type |= 1 << TOKEN_NULL;
			} else {
				type |= 1 << TOKEN_SYMBOL;
			}
		}

		const lex_token_t token = lex_add_token(lex);

		token_t *data = lex_get_token(lex, token);
		if (data == NULL) {
			return NULL;
		}

		*data = (token_t){
			.value	    = strc(&str.data[i], 1),
			.line_start = i == 0 ? (uint)token : line_start,
			.line	    = line,
			.col	    = col,
			.type	    = type,
		};

		if (c == '\n') {
			line++;
			col	   = 0;
			line_start = (uint)token + 1;
		} else {
			col++;
		}
	}

	return lex;
}

int lex_print_line(const lex_t *lex, lex_token_t token, print_dst_t dst)
{
	const token_t *data = lex_get_token(lex, token);

	if (data == NULL) {
		return 0;
	}

	int off = dst.off;

	uint i = data->line_start;
	const token_t *line_token;
	arr_foreach_i(&lex->tokens, line_token, i)
	{
		dst.off += str_print(line_token->value, dst);

		if (line_token->type & (1 << TOKEN_NL)) {
			break;
		}
	}

	return dst.off - off;
}

int lex_dbg(const lex_t *lex, print_dst_t dst)
{
	if (lex == NULL) {
		return 0;
	}

	int off = dst.off;

	const token_t *token;
	arr_foreach(&lex->tokens, token)
	{
		dst.off += token_dbg(*token, dst);
		dst.off += c_print_exec(dst, "\n");
	}

	return dst.off - off;
}
