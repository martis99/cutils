#include "bnf.h"

#include "log.h"
#include "token.h"

bnf_t *bnf_init(bnf_t *bnf)
{
	if (bnf == NULL) {
		return NULL;
	}

	return bnf;
}

void bnf_free(bnf_t *bnf)
{
	if (bnf == NULL) {
		return;
	}
}

stx_t *bnf_get_stx(bnf_t *bnf, stx_t *stx)
{
	if (bnf == NULL) {
		return NULL;
	}

	if (stx_init(stx, 10, 20) == NULL) {
		log_error("cutils", "bnf", NULL, "failed to intialize syntax");
		return NULL;
	}

	bnf->file		     = stx_add_rule(stx, STR("file"));
	bnf->bnf		     = stx_add_rule(stx, STR("bnf"));
	bnf->rules		     = stx_add_rule(stx, STR("rules"));
	bnf->rule		     = stx_add_rule(stx, STR("rule"));
	bnf->rule_name		     = stx_add_rule(stx, STR("rule-name"));
	const stx_rule_t rule_chars  = stx_add_rule(stx, STR("rule-chars"));
	const stx_rule_t rule_char   = stx_add_rule(stx, STR("rule-char"));
	bnf->expression		     = stx_add_rule(stx, STR("expression"));
	bnf->terms		     = stx_add_rule(stx, STR("terms"));
	bnf->term		     = stx_add_rule(stx, STR("term"));
	bnf->literal		     = stx_add_rule(stx, STR("literal"));
	bnf->token		     = stx_add_rule(stx, STR("token"));
	bnf->text_double	     = stx_add_rule(stx, STR("text-double"));
	bnf->text_single	     = stx_add_rule(stx, STR("text-single"));
	const stx_rule_t char_double = stx_add_rule(stx, STR("char-double"));
	const stx_rule_t char_single = stx_add_rule(stx, STR("char-single"));
	const stx_rule_t character   = stx_add_rule(stx, STR("character"));
	const stx_rule_t spaces	     = stx_add_rule(stx, STR("spaces"));
	const stx_rule_t space	     = stx_add_rule(stx, STR("space"));

	stx_rule_add_term(stx, bnf->file, STX_TERM_RULE(bnf->bnf));
	stx_rule_add_term(stx, bnf->file, STX_TERM_TOKEN(TOKEN_EOF));

	stx_rule_add_term(stx, bnf->bnf, STX_TERM_RULE(bnf->rules));

	stx_rule_add_arr(stx, bnf->rules, STX_TERM_RULE(bnf->rule), STX_TERM_NONE());

	stx_rule_add_term(stx, bnf->rule, STX_TERM_LITERAL(STR("<")));
	stx_rule_add_term(stx, bnf->rule, STX_TERM_RULE(bnf->rule_name));
	stx_rule_add_term(stx, bnf->rule, STX_TERM_LITERAL(STR(">")));
	stx_rule_add_term(stx, bnf->rule, STX_TERM_RULE(spaces));
	stx_rule_add_term(stx, bnf->rule, STX_TERM_LITERAL(STR("::=")));
	stx_rule_add_term(stx, bnf->rule, STX_TERM_RULE(space));
	stx_rule_add_term(stx, bnf->rule, STX_TERM_RULE(bnf->expression));
	stx_rule_add_term(stx, bnf->rule, STX_TERM_TOKEN(TOKEN_NL));

	const stx_term_t rule_name_l = stx_create_term(stx, STX_TERM_TOKEN(TOKEN_LOWER));
	stx_term_add_term(stx, rule_name_l, STX_TERM_RULE(rule_chars));
	const stx_term_t rule_name_r = stx_create_term(stx, STX_TERM_TOKEN(TOKEN_LOWER));
	stx_rule_add_term(stx, bnf->rule_name, STX_TERM_OR(rule_name_l, rule_name_r));

	stx_rule_add_arr(stx, rule_chars, STX_TERM_RULE(rule_char), STX_TERM_NONE());

	const stx_term_t rule_char_lower = stx_create_term(stx, STX_TERM_TOKEN(TOKEN_LOWER));
	const stx_term_t rule_char_dash	 = stx_create_term(stx, STX_TERM_LITERAL(STR("-")));
	stx_rule_add_term(stx, rule_char, STX_TERM_OR(rule_char_lower, rule_char_dash));

	const stx_term_t expression_l = stx_create_term(stx, STX_TERM_RULE(bnf->terms));
	stx_term_add_term(stx, expression_l, STX_TERM_RULE(space));
	stx_term_add_term(stx, expression_l, STX_TERM_LITERAL(STR("|")));
	stx_term_add_term(stx, expression_l, STX_TERM_RULE(space));
	stx_term_add_term(stx, expression_l, STX_TERM_RULE(bnf->expression));
	const stx_term_t epxression_r = stx_create_term(stx, STX_TERM_RULE(bnf->terms));
	stx_rule_add_term(stx, bnf->expression, STX_TERM_OR(expression_l, epxression_r));

	stx_rule_add_arr(stx, bnf->terms, STX_TERM_RULE(bnf->term), STX_TERM_RULE(space));

	const stx_term_t term_literal = stx_create_term(stx, STX_TERM_RULE(bnf->literal));
	const stx_term_t term_token   = stx_create_term(stx, STX_TERM_RULE(bnf->token));
	const stx_term_t term_rule    = stx_create_term(stx, STX_TERM_LITERAL(STR("<")));
	stx_term_add_term(stx, term_rule, STX_TERM_RULE(bnf->rule_name));
	stx_term_add_term(stx, term_rule, STX_TERM_LITERAL(STR(">")));
	stx_rule_add_or(stx, bnf->term, 3, term_literal, term_token, term_rule);

	const stx_term_t literal_single = stx_create_term(stx, STX_TERM_LITERAL(STR("'")));
	stx_term_add_term(stx, literal_single, STX_TERM_RULE(bnf->text_double));
	stx_term_add_term(stx, literal_single, STX_TERM_LITERAL(STR("'")));
	const stx_term_t literal_double = stx_create_term(stx, STX_TERM_LITERAL(STR("\"")));
	stx_term_add_term(stx, literal_double, STX_TERM_RULE(bnf->text_single));
	stx_term_add_term(stx, literal_double, STX_TERM_LITERAL(STR("\"")));
	stx_rule_add_term(stx, bnf->literal, STX_TERM_OR(literal_single, literal_double));

	stx_rule_add_arr(stx, bnf->token, STX_TERM_TOKEN(TOKEN_UPPER), STX_TERM_NONE());

	stx_rule_add_arr(stx, bnf->text_double, STX_TERM_RULE(char_double), STX_TERM_NONE());
	stx_rule_add_arr(stx, bnf->text_single, STX_TERM_RULE(char_single), STX_TERM_NONE());

	const stx_term_t char_double_char  = stx_create_term(stx, STX_TERM_RULE(character));
	const stx_term_t char_double_quote = stx_create_term(stx, STX_TERM_LITERAL(STR("\"")));
	stx_rule_add_term(stx, char_double, STX_TERM_OR(char_double_char, char_double_quote));

	const stx_term_t char_single_char  = stx_create_term(stx, STX_TERM_RULE(character));
	const stx_term_t char_single_quote = stx_create_term(stx, STX_TERM_LITERAL(STR("'")));
	stx_rule_add_term(stx, char_single, STX_TERM_OR(char_single_char, char_single_quote));

	const stx_term_t char_alpha  = stx_create_term(stx, STX_TERM_TOKEN(TOKEN_ALPHA));
	const stx_term_t char_digit  = stx_create_term(stx, STX_TERM_TOKEN(TOKEN_DIGIT));
	const stx_term_t char_symbol = stx_create_term(stx, STX_TERM_TOKEN(TOKEN_SYMBOL));
	const stx_term_t char_space  = stx_create_term(stx, STX_TERM_RULE(space));
	stx_rule_add_or(stx, character, 4, char_alpha, char_digit, char_symbol, char_space);

	stx_rule_add_arr(stx, spaces, STX_TERM_RULE(space), STX_TERM_NONE());

	stx_rule_add_term(stx, space, STX_TERM_LITERAL(STR(" ")));

	stx_compile(stx);

	return stx;
}

static stx_term_t term_from_bnf(const prs_t *prs, bnf_t *bnf, prs_node_t parent, stx_t *stx)
{
	const prs_node_t prs_rule_name = prs_get_rule(prs, parent, bnf->rule_name);
	if (prs_rule_name < prs->nodes.cnt) {
		str_t rule_name = strz(16);
		prs_get_str(prs, prs_rule_name, &rule_name);

		stx_rule_t term_rule = stx_get_rule(stx, rule_name);
		if (term_rule >= stx->rules.cnt) {
			term_rule = stx_add_rule(stx, rule_name);
		} else {
			str_free(&rule_name);
		}

		return stx_create_term(stx, STX_TERM_RULE(term_rule));
	}

	const prs_node_t prs_literal = prs_get_rule(prs, parent, bnf->literal);
	if (prs_literal < prs->nodes.cnt) {
		const prs_node_t prs_text_double = prs_get_rule(prs, prs_literal, bnf->text_double);
		if (prs_text_double < prs->nodes.cnt) {
			str_t literal = strz(16);
			prs_get_str(prs, prs_text_double, &literal);
			return stx_create_term(stx, STX_TERM_LITERAL(literal));
		}

		const prs_node_t prs_text_single = prs_get_rule(prs, prs_literal, bnf->text_single);
		if (prs_text_single < prs->nodes.cnt) {
			str_t literal = strz(16);
			prs_get_str(prs, prs_text_single, &literal);
			return stx_create_term(stx, STX_TERM_LITERAL(literal));
		}

		return STX_TERM_END;
	}

	const prs_node_t prs_token = prs_get_rule(prs, parent, bnf->token);
	if (prs_token < prs->nodes.cnt) {
		str_t token = strz(16);
		prs_get_str(prs, prs_token, &token);
		const stx_term_t term = stx_create_term(stx, STX_TERM_TOKEN(token_type_enum(token)));
		str_free(&token);
		return term;
	}

	return STX_TERM_END;
}

static stx_term_t terms_from_bnf(const prs_t *prs, bnf_t *bnf, prs_node_t parent, stx_t *stx)
{
	const prs_node_t prs_term = prs_get_rule(prs, parent, bnf->term);
	const stx_term_t term	  = term_from_bnf(prs, bnf, prs_term, stx);

	const prs_node_t prs_terms = prs_get_rule(prs, parent, bnf->terms);
	if (prs_terms < prs->nodes.cnt) {
		stx_term_add_term_id(stx, term, terms_from_bnf(prs, bnf, prs_terms, stx));
	}

	return term;
}

static stx_term_t exprs_from_bnf(const prs_t *prs, bnf_t *bnf, prs_node_t parent, stx_t *stx)
{
	const prs_node_t prs_expr = prs_get_rule(prs, parent, bnf->expression);
	if (prs_expr < prs->nodes.cnt) {
		const prs_node_t prs_terms = prs_get_rule(prs, parent, bnf->terms);

		const stx_term_t left  = terms_from_bnf(prs, bnf, prs_terms, stx);
		const stx_term_t right = exprs_from_bnf(prs, bnf, prs_expr, stx);

		return stx_create_term(stx, STX_TERM_OR(left, right));
	}

	const prs_node_t prs_terms = prs_get_rule(prs, parent, bnf->terms);
	return terms_from_bnf(prs, bnf, prs_terms, stx);
}

static void rules_from_bnf(const prs_t *prs, bnf_t *bnf, prs_node_t parent, stx_t *stx)
{
	const prs_node_t prs_rule      = prs_get_rule(prs, parent, bnf->rule);
	const prs_node_t prs_rule_name = prs_get_rule(prs, prs_rule, bnf->rule_name);

	str_t rule_name = strz(16);
	prs_get_str(prs, prs_rule_name, &rule_name);
	stx_rule_t rule = stx_get_rule(stx, rule_name);
	if (rule >= stx->rules.cnt) {
		rule = stx_add_rule(stx, rule_name);
	} else {
		str_free(&rule_name);
	}

	const prs_node_t prs_expr = prs_get_rule(prs, prs_rule, bnf->expression);
	const stx_term_t term	  = exprs_from_bnf(prs, bnf, prs_expr, stx);
	stx_rule_set_term(stx, rule, term);

	const prs_node_t rules = prs_get_rule(prs, parent, bnf->rules);
	if (rules >= prs->nodes.cnt) {
		return;
	}

	rules_from_bnf(prs, bnf, rules, stx);
}

stx_t *stx_from_bnf(bnf_t *bnf, stx_t *stx, prs_t *prs)
{
	prs_node_t file = prs->root;

	prs_node_t fbnf	 = prs_get_rule(prs, file, bnf->bnf);
	prs_node_t rules = prs_get_rule(prs, fbnf, bnf->rules);

	rules_from_bnf(prs, bnf, rules, stx);

	stx_compile(stx);

	return stx;
}
