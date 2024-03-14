#include "bnf.h"

#include "log.h"
#include "token.h"

bnf_t *bnf_init(bnf_t *bnf, uint tokens_cap)
{
	if (bnf == NULL) {
		return NULL;
	}

	if (lex_init(&bnf->lex, tokens_cap) == NULL) {
		log_error("cutils", "bnf", NULL, "failed to intialize lexer");
		return NULL;
	}

	return bnf;
}

void bnf_free(bnf_t *bnf)
{
	if (bnf == NULL) {
		return;
	}

	lex_free(&bnf->lex);
}

stx_t *bnf_get_stx(stx_t *stx)
{
	if (stx == NULL) {
		return NULL;
	}

	if (stx_init(stx, 10, 20) == NULL) {
		log_error("cutils", "bnf", NULL, "failed to intialize syntax");
		return NULL;
	}

	const stx_rule_t file	     = stx_add_rule(stx, STR("file"));
	const stx_rule_t bnf	     = stx_add_rule(stx, STR("bnf"));
	const stx_rule_t rules	     = stx_add_rule(stx, STR("rules"));
	const stx_rule_t rule	     = stx_add_rule(stx, STR("rule"));
	const stx_rule_t rule_name   = stx_add_rule(stx, STR("rule-name"));
	const stx_rule_t rule_chars  = stx_add_rule(stx, STR("rule-chars"));
	const stx_rule_t rule_char   = stx_add_rule(stx, STR("rule-char"));
	const stx_rule_t expression  = stx_add_rule(stx, STR("expression"));
	const stx_rule_t terms	     = stx_add_rule(stx, STR("terms"));
	const stx_rule_t term	     = stx_add_rule(stx, STR("term"));
	const stx_rule_t literal     = stx_add_rule(stx, STR("literal"));
	const stx_rule_t token	     = stx_add_rule(stx, STR("token"));
	const stx_rule_t text_double = stx_add_rule(stx, STR("text-double"));
	const stx_rule_t text_single = stx_add_rule(stx, STR("text-single"));
	const stx_rule_t char_double = stx_add_rule(stx, STR("char-double"));
	const stx_rule_t char_single = stx_add_rule(stx, STR("char-single"));
	const stx_rule_t character   = stx_add_rule(stx, STR("character"));
	const stx_rule_t spaces	     = stx_add_rule(stx, STR("spaces"));
	const stx_rule_t space	     = stx_add_rule(stx, STR("space"));

	stx_rule_add_term(stx, file, STX_TERM_RULE(bnf));
	stx_rule_add_term(stx, file, STX_TERM_TOKEN(TOKEN_EOF));

	stx_rule_add_term(stx, bnf, STX_TERM_RULE(rules));

	stx_rule_add_arr(stx, rules, STX_TERM_RULE(rule), STX_TERM_NONE());

	stx_rule_add_term(stx, rule, STX_TERM_LITERAL(STR("<")));
	stx_rule_add_term(stx, rule, STX_TERM_RULE(rule_name));
	stx_rule_add_term(stx, rule, STX_TERM_LITERAL(STR(">")));
	stx_rule_add_term(stx, rule, STX_TERM_RULE(spaces));
	stx_rule_add_term(stx, rule, STX_TERM_LITERAL(STR("::=")));
	stx_rule_add_term(stx, rule, STX_TERM_RULE(space));
	stx_rule_add_term(stx, rule, STX_TERM_RULE(expression));
	stx_rule_add_term(stx, rule, STX_TERM_TOKEN(TOKEN_NL));

	const stx_term_t rule_name_l = stx_create_term(stx, STX_TERM_TOKEN(TOKEN_LOWER));
	stx_term_add_term(stx, rule_name_l, STX_TERM_RULE(rule_chars));
	const stx_term_t rule_name_r = stx_create_term(stx, STX_TERM_TOKEN(TOKEN_LOWER));
	stx_rule_add_term(stx, rule_name, STX_TERM_OR(rule_name_l, rule_name_r));

	stx_rule_add_arr(stx, rule_chars, STX_TERM_RULE(rule_char), STX_TERM_NONE());

	const stx_term_t rule_char_lower = stx_create_term(stx, STX_TERM_TOKEN(TOKEN_LOWER));
	const stx_term_t rule_char_dash	 = stx_create_term(stx, STX_TERM_LITERAL(STR("-")));
	stx_rule_add_term(stx, rule_char, STX_TERM_OR(rule_char_lower, rule_char_dash));

	const stx_term_t expression_l = stx_create_term(stx, STX_TERM_RULE(terms));
	stx_term_add_term(stx, expression_l, STX_TERM_RULE(space));
	stx_term_add_term(stx, expression_l, STX_TERM_LITERAL(STR("|")));
	stx_term_add_term(stx, expression_l, STX_TERM_RULE(space));
	stx_term_add_term(stx, expression_l, STX_TERM_RULE(expression));
	const stx_term_t epxression_r = stx_create_term(stx, STX_TERM_RULE(terms));
	stx_rule_add_term(stx, expression, STX_TERM_OR(expression_l, epxression_r));

	stx_rule_add_arr(stx, terms, STX_TERM_RULE(term), STX_TERM_RULE(space));

	const stx_term_t term_literal = stx_create_term(stx, STX_TERM_RULE(literal));
	const stx_term_t term_token = stx_create_term(stx, STX_TERM_RULE(token));
	const stx_term_t term_rule    = stx_create_term(stx, STX_TERM_LITERAL(STR("<")));
	stx_term_add_term(stx, term_rule, STX_TERM_RULE(rule_name));
	stx_term_add_term(stx, term_rule, STX_TERM_LITERAL(STR(">")));
	stx_rule_add_or(stx, term, 3, term_literal, term_token, term_rule);

	const stx_term_t literal_single = stx_create_term(stx, STX_TERM_LITERAL(STR("'")));
	stx_term_add_term(stx, literal_single, STX_TERM_RULE(text_double));
	stx_term_add_term(stx, literal_single, STX_TERM_LITERAL(STR("'")));
	const stx_term_t literal_double = stx_create_term(stx, STX_TERM_LITERAL(STR("\"")));
	stx_term_add_term(stx, literal_double, STX_TERM_RULE(text_single));
	stx_term_add_term(stx, literal_double, STX_TERM_LITERAL(STR("\"")));
	stx_rule_add_term(stx, literal, STX_TERM_OR(literal_single, literal_double));

	stx_rule_add_arr(stx, token, STX_TERM_TOKEN(TOKEN_UPPER), STX_TERM_NONE());

	stx_rule_add_arr(stx, text_double, STX_TERM_RULE(char_double), STX_TERM_NONE());
	stx_rule_add_arr(stx, text_single, STX_TERM_RULE(char_single), STX_TERM_NONE());

	const stx_term_t char_double_char  = stx_create_term(stx, STX_TERM_RULE(character));
	const stx_term_t char_double_quote = stx_create_term(stx, STX_TERM_LITERAL(STR("\"")));
	stx_rule_add_term(stx, char_double, STX_TERM_OR(char_double_char, char_double_quote));

	const stx_term_t char_single_char  = stx_create_term(stx, STX_TERM_RULE(character));
	const stx_term_t char_single_quote = stx_create_term(stx, STX_TERM_LITERAL(STR("'")));
	stx_rule_add_term(stx, char_single, STX_TERM_OR(char_single_char, char_single_quote));

	const stx_term_t char_alpha	  = stx_create_term(stx, STX_TERM_TOKEN(TOKEN_ALPHA));
	const stx_term_t char_digit	  = stx_create_term(stx, STX_TERM_TOKEN(TOKEN_DIGIT));
	const stx_term_t char_symbol	  = stx_create_term(stx, STX_TERM_TOKEN(TOKEN_SYMBOL));
	const stx_term_t char_space	  = stx_create_term(stx, STX_TERM_RULE(space));
	stx_rule_add_or(stx, character, 4, char_alpha, char_digit, char_symbol, char_space);

	stx_rule_add_arr(stx, spaces, STX_TERM_RULE(space), STX_TERM_NONE());

	stx_rule_add_term(stx, space, STX_TERM_LITERAL(STR(" ")));

	stx_compile(stx);

	return stx;
}
