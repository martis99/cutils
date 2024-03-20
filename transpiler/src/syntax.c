#include "syntax.h"

#include "log.h"

#include <stdarg.h>

stx_t *stx_init(stx_t *stx, uint rules_cap, uint terms_cap)
{
	if (stx == NULL) {
		return NULL;
	}

	if (arr_init(&stx->rules, rules_cap, sizeof(stx_rule_data_t)) == NULL) {
		log_error("cutils", "syntax", NULL, "failed to initialize rules array");
		return NULL;
	}

	if (list_init(&stx->terms, terms_cap, sizeof(stx_term_data_t)) == NULL) {
		log_error("cutils", "syntax", NULL, "failed to initialize terms list");
		return NULL;
	}

	stx->root = STX_RULE_END;

	return stx;
}

void stx_free(stx_t *stx)
{
	if (stx == NULL) {
		return;
	}

	stx_rule_data_t *rule;
	arr_foreach(&stx->rules, rule)
	{
		str_free(&rule->name);
	}

	arr_free(&stx->rules);

	stx_term_data_t *term;
	list_foreach_all(&stx->terms, term)
	{
		if (term->type == STX_TERM_LITERAL) {
			str_free(&term->val.literal);
		}
	}

	list_free(&stx->terms);

	stx->root = STX_RULE_END;
}

stx_rule_t stx_add_rule(stx_t *stx, str_t name)
{
	if (stx == NULL) {
		return STX_RULE_END;
	}

	const stx_rule_t rule = arr_add(&stx->rules);
	if (stx->root == STX_RULE_END) {
		stx->root = rule;
	}

	stx_rule_data_t *data = arr_get(&stx->rules, rule);
	if (data == NULL) {
		log_error("cutils", "syntax", NULL, "failed to add rule");
		return STX_RULE_END;
	}

	*data = (stx_rule_data_t){
		.name  = name,
		.terms = STX_TERM_END,
	};

	return rule;
}

stx_rule_data_t *stx_get_rule_data(const stx_t *stx, stx_rule_t rule)
{
	if (stx == NULL) {
		return NULL;
	}

	stx_rule_data_t *data = arr_get(&stx->rules, rule);

	if (data == NULL) {
		log_warn("cutils", "syntax", NULL, "invalid rule: %d", rule);
		return NULL;
	}

	return data;
}

stx_term_t stx_create_term(stx_t *stx, stx_term_data_t term)
{
	if (stx == NULL) {
		return STX_TERM_END;
	}

	const stx_term_t child = list_add(&stx->terms);
	stx_term_data_t *data  = list_get_data(&stx->terms, child);

	if (data == NULL) {
		log_error("cutils", "syntax", NULL, "failed to create term");
		return STX_TERM_END;
	}

	*data = term;

	return child;
}

stx_term_data_t *stx_get_term_data(const stx_t *stx, stx_term_t term)
{
	if (stx == NULL) {
		return NULL;
	}

	stx_term_data_t *data = list_get_data(&stx->terms, term);

	if (data == NULL) {
		log_warn("cutils", "syntax", NULL, "invalid term: %d", term);
		return NULL;
	}

	return data;
}

stx_term_t stx_rule_add_term_id(stx_t *stx, stx_rule_t rule, stx_term_t term)
{
	stx_rule_data_t *data = stx_get_rule_data(stx, rule);

	if (data == NULL) {
		log_error("cutils", "syntax", NULL, "failed to get rule: %d", rule);
		return STX_TERM_END;
	}

	return list_set_next_node(&stx->terms, data->terms, term);
}

stx_term_t stx_rule_add_term(stx_t *stx, stx_rule_t rule, stx_term_data_t term)
{
	if (stx_get_rule_data(stx, rule) == NULL) {
		log_error("cutils", "syntax", NULL, "failed to get rule: %d", rule);
		return STX_TERM_END;
	}

	return stx_rule_add_term_id(stx, rule, stx_create_term(stx, term));
}

static stx_term_t stx_term_add_term_id(stx_t *stx, stx_term_t term, stx_term_t next)
{
	return list_set_next_node(&stx->terms, term, next);
}

stx_term_t stx_term_add_term(stx_t *stx, stx_term_t term, stx_term_data_t next)
{
	if (stx_get_term_data(stx, term) == NULL) {
		log_error("cutils", "syntax", NULL, "failed to get term: %d", term);
		return STX_TERM_END;
	}

	return stx_term_add_term_id(stx, term, stx_create_term(stx, next));
}

stx_term_t stx_rule_add_or(stx_t *stx, stx_rule_t rule, size_t n, ...)
{
	if (n < 1) {
		return STX_TERM_END;
	}

	va_list args;
	va_start(args, n);

	stx_term_t term = va_arg(args, stx_term_t);

	for (size_t i = 1; i < n; i++) {
		term = stx_create_term(stx, STX_TERM_OR(term, va_arg(args, stx_term_t)));
	}

	va_end(args);

	return stx_rule_add_term_id(stx, rule, term);
}

stx_term_t stx_rule_add_arr(stx_t *stx, stx_rule_t rule, stx_term_data_t term, stx_term_data_t sep)
{
	const stx_term_t l = stx_create_term(stx, term);
	if (sep.type != (stx_term_t)-1) {
		stx_term_add_term(stx, l, sep);
	}
	stx_term_add_term(stx, l, STX_TERM_RULE(rule));
	const stx_term_t r = stx_create_term(stx, term);
	return stx_rule_add_term(stx, rule, STX_TERM_OR(l, r));
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int stx_compile(stx_t *stx)
{
	if (stx == NULL) {
		return 1;
	}

	int ret = 0;

	stx->max_rule_len = 0;

	stx_rule_data_t *rule;
	arr_foreach(&stx->rules, rule)
	{
		stx->max_rule_len = MAX(stx->max_rule_len, rule->name.len);
	}

	return ret;
}

static int stx_print_terms(const stx_t *stx, const stx_term_t terms, print_dst_t dst)
{
	int off = dst.off;

	const stx_term_data_t *term;
	list_foreach(&stx->terms, terms, term)
	{
		switch (term->type) {
		case STX_TERM_RULE: {
			const stx_rule_data_t *data = stx_get_rule_data(stx, term->val.rule);
			if (data == NULL) {
				log_error("cutils", "syntax", NULL, "failed to get rule: %d", term->val.rule);
				break;
			}
			dst.off += c_print_exec(dst, " <%.*s>", data->name.len, data->name.data);
			break;
		}
		case STX_TERM_TOKEN: {
			const str_t token = token_type_str(term->val.token);
			dst.off += c_print_exec(dst, " %.*s", token.len, token.data);
			break;
		}
		case STX_TERM_LITERAL:
			if (str_eq(term->val.literal, STR("'"))) {
				dst.off += c_print_exec(dst, " \"%.*s\"", term->val.literal.len, term->val.literal.data);
			} else {
				dst.off += c_print_exec(dst, " \'%.*s\'", term->val.literal.len, term->val.literal.data);
			}
			break;
		case STX_TERM_OR:
			dst.off += stx_print_terms(stx, term->val.orv.l, dst);
			dst.off += c_print_exec(dst, " |");
			dst.off += stx_print_terms(stx, term->val.orv.r, dst);
			break;
		default: log_warn("cutils", "syntax", NULL, "unknown term type: %d", term->type); break;
		}
	}

	return dst.off - off;
}

int stx_print(const stx_t *stx, print_dst_t dst)
{
	if (stx == NULL) {
		return 0;
	}

	int off = dst.off;

	const stx_rule_data_t *rule;
	arr_foreach(&stx->rules, rule)
	{
		dst.off += c_print_exec(dst, "<%.*s>%*s ::=", rule->name.len, rule->name.data, MAX(stx->max_rule_len - rule->name.len, 0), "");
		dst.off += stx_print_terms(stx, rule->terms, dst);
		dst.off += c_print_exec(dst, "\n");
	}

	return dst.off - off;
}
