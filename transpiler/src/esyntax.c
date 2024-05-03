#include "esyntax.h"

#include "log.h"

#include <stdarg.h>

estx_t *estx_init(estx_t *estx, uint rules_cap, uint terms_cap)
{
	if (estx == NULL) {
		return NULL;
	}

	if (arr_init(&estx->rules, rules_cap, sizeof(estx_rule_data_t)) == NULL) {
		log_error("cutils", "esyntax", NULL, "failed to initialize rules array");
		return NULL;
	}

	if (tree_init(&estx->terms, terms_cap, sizeof(estx_term_data_t)) == NULL) {
		log_error("cutils", "esyntax", NULL, "failed to initialize terms tree");
		return NULL;
	}

	estx->root = ESTX_RULE_END;

	return estx;
}

void estx_free(estx_t *estx)
{
	if (estx == NULL) {
		return;
	}

	estx_rule_data_t *rule;
	arr_foreach(&estx->rules, rule)
	{
		str_free(&rule->name);
	}

	arr_free(&estx->rules);

	estx_term_t term;
	tree_foreach_all(&estx->terms, term)
	{
		estx_term_data_t *data = tree_get_data(&estx->terms, term);
		if (data->type == ESTX_TERM_LITERAL) {
			str_free(&data->val.literal);
		}
	}

	tree_free(&estx->terms);

	estx->root = ESTX_RULE_END;
}

estx_rule_t estx_add_rule(estx_t *estx, str_t name)
{
	if (estx == NULL) {
		return ESTX_RULE_END;
	}

	const estx_rule_t rule = arr_add(&estx->rules);
	if (estx->root == ESTX_RULE_END) {
		estx->root = rule;
	}

	estx_rule_data_t *data = arr_get(&estx->rules, rule);
	if (data == NULL) {
		log_error("cutils", "esyntax", NULL, "failed to add rule");
		return ESTX_RULE_END;
	}

	*data = (estx_rule_data_t){
		.name  = name,
		.terms = ESTX_TERM_END,
	};

	return rule;
}

estx_rule_t estx_get_rule(const estx_t *estx, str_t name)
{
	if (estx == NULL) {
		return ESTX_RULE_END;
	}

	const estx_rule_data_t *rule;
	arr_foreach(&estx->rules, rule)
	{
		if (str_eq(rule->name, name)) {
			return _i;
		}
	}

	return ESTX_RULE_END;
}

estx_rule_data_t *estx_get_rule_data(const estx_t *estx, estx_rule_t rule)
{
	if (estx == NULL) {
		return NULL;
	}

	estx_rule_data_t *data = arr_get(&estx->rules, rule);

	if (data == NULL) {
		log_warn("cutils", "esyntax", NULL, "invalid rule: %d", rule);
		return NULL;
	}

	return data;
}

estx_term_t estx_create_term(estx_t *estx, estx_term_data_t term)
{
	if (estx == NULL) {
		return ESTX_TERM_END;
	}

	const estx_term_t child = tree_add(&estx->terms);
	estx_term_data_t *data	= tree_get_data(&estx->terms, child);

	if (data == NULL) {
		log_error("cutils", "esyntax", NULL, "failed to create term");
		return ESTX_TERM_END;
	}

	*data = term;

	return child;
}

estx_term_data_t *estx_get_term_data(const estx_t *estx, estx_term_t term)
{
	if (estx == NULL) {
		return NULL;
	}

	estx_term_data_t *data = tree_get_data(&estx->terms, term);

	if (data == NULL) {
		log_warn("cutils", "esyntax", NULL, "invalid term: %d", term);
		return NULL;
	}

	return data;
}

estx_term_t estx_rule_set_term(estx_t *estx, estx_rule_t rule, estx_term_t term)
{
	estx_rule_data_t *data = estx_get_rule_data(estx, rule);

	if (data == NULL) {
		log_error("cutils", "esyntax", NULL, "failed to get rule: %d", rule);
		return ESTX_TERM_END;
	}

	return data->terms = term;
}

estx_term_t estx_term_add_term(estx_t *estx, estx_term_t term, estx_term_t child)
{
	if (estx_get_term_data(estx, term) == NULL) {
		log_error("cutils", "esyntax", NULL, "failed to get term: %d", term);
		return ESTX_TERM_END;
	}

	return tree_set_child_node(&estx->terms, term, child);
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int estx_compile(estx_t *estx)
{
	if (estx == NULL) {
		return 1;
	}

	int ret = 0;

	estx->max_rule_len = 0;

	estx_rule_data_t *rule;
	arr_foreach(&estx->rules, rule)
	{
		estx->max_rule_len = MAX(estx->max_rule_len, rule->name.len);
	}

	return ret;
}

static int estx_term_occ_print(estx_term_occ_t occ, print_dst_t dst)
{
	if ((occ & ESTX_TERM_OCC_OPT) && (occ & ESTX_TERM_OCC_REP)) {
		return dprintf(dst, "*");
	}

	if (occ & ESTX_TERM_OCC_OPT) {
		return dprintf(dst, "?");
	}

	if (occ & ESTX_TERM_OCC_REP) {
		return dprintf(dst, "+");
	}

	return 0;
}

static int estx_term_print(const estx_t *estx, const estx_term_t term, print_dst_t dst)
{
	int off = dst.off;

	const estx_term_data_t *data = estx_get_term_data(estx, term);
	switch (data->type) {
	case ESTX_TERM_RULE: {
		const estx_rule_data_t *rule = estx_get_rule_data(estx, data->val.rule);
		if (rule == NULL) {
			log_error("cutils", "esyntax", NULL, "failed to get rule: %d", data->val.rule);
			break;
		}
		dst.off += dprintf(dst, " %.*s", rule->name.len, rule->name.data);
		dst.off += estx_term_occ_print(data->occ, dst);
		break;
	}
	case ESTX_TERM_TOKEN: {
		const str_t token = token_type_str(data->val.token);
		dst.off += dprintf(dst, " %.*s", token.len, token.data);
		dst.off += estx_term_occ_print(data->occ, dst);
		break;
	}
	case ESTX_TERM_LITERAL:
		if (str_eq(data->val.literal, STR("'"))) {
			dst.off += dprintf(dst, " \"%.*s\"", data->val.literal.len, data->val.literal.data);
		} else {
			dst.off += dprintf(dst, " \'%.*s\'", data->val.literal.len, data->val.literal.data);
		}
		dst.off += estx_term_occ_print(data->occ, dst);
		break;
	case ESTX_TERM_ALT: {
		estx_term_t child;
		int first = 1;
		tree_foreach_child(&estx->terms, term, child)
		{
			if (!first) {
				dst.off += dprintf(dst, " |");
			}

			dst.off += estx_term_print(estx, child, dst);
			first = 0;
		}
		break;
	}
	case ESTX_TERM_CON: {
		estx_term_t child;
		tree_foreach_child(&estx->terms, term, child)
		{
			dst.off += estx_term_print(estx, child, dst);
		}
		break;
	}
	case ESTX_TERM_GROUP: {
		estx_term_t child;
		dst.off += dprintf(dst, " (");
		tree_foreach_child(&estx->terms, term, child)
		{
			dst.off += estx_term_print(estx, child, dst);
		}
		dst.off += dprintf(dst, " )");
		dst.off += estx_term_occ_print(data->occ, dst);
		break;
	}
	default: log_warn("cutils", "esyntax", NULL, "unknown term type: %d", data->type); break;
	}

	return dst.off - off;
}

int estx_print(const estx_t *estx, print_dst_t dst)
{
	if (estx == NULL) {
		return 0;
	}

	int off = dst.off;

	const estx_rule_data_t *rule;
	arr_foreach(&estx->rules, rule)
	{
		dst.off += dprintf(dst, "%.*s%*s =", rule->name.len, rule->name.data, MAX(estx->max_rule_len - rule->name.len, 0), "");
		if (rule->terms >= estx->terms.cnt) {
			log_error("cutils", "esyntax", NULL, "failed to get rule '%.*s' terms", rule->name.len, rule->name.data);
		} else {
			dst.off += estx_term_print(estx, rule->terms, dst);
		}
		dst.off += dprintf(dst, "\n");
	}

	return dst.off - off;
}

int term_print_cb(void *data, print_dst_t dst, const void *priv)
{
	const estx_term_data_t *term = data;
	const estx_t *estx	     = priv;

	int off = dst.off;

	switch (term->type) {
	case ESTX_TERM_RULE: {
		const estx_rule_data_t *rule = estx_get_rule_data(estx, term->val.rule);
		if (rule == NULL) {
			log_error("cutils", "esyntax", NULL, "failed to get rule: %d", term->val.rule);
			break;
		}
		dst.off += dprintf(dst, "<%.*s>", rule->name.len, rule->name.data);
		dst.off += estx_term_occ_print(term->occ, dst);
		break;
	}
	case ESTX_TERM_TOKEN: {
		const str_t token = token_type_str(term->val.token);
		dst.off += dprintf(dst, "%.*s", token.len, token.data);
		dst.off += estx_term_occ_print(term->occ, dst);
		break;
	}
	case ESTX_TERM_LITERAL:
		if (str_eq(term->val.literal, STR("'"))) {
			dst.off += dprintf(dst, "\"%.*s\"", term->val.literal.len, term->val.literal.data);
		} else {
			dst.off += dprintf(dst, "\'%.*s\'", term->val.literal.len, term->val.literal.data);
		}
		dst.off += estx_term_occ_print(term->occ, dst);
		break;
	case ESTX_TERM_ALT: {
		dst.off += dprintf(dst, "alt");
		break;
	}
	case ESTX_TERM_CON: {
		dst.off += dprintf(dst, "con");
		break;
	}
	case ESTX_TERM_GROUP: {
		dst.off += dprintf(dst, "group");
		dst.off += estx_term_occ_print(term->occ, dst);
		break;
	}
	default: log_warn("cutils", "esyntax", NULL, "unknown term type: %d", term->type); break;
	}

	dst.off += dprintf(dst, "\n");

	return dst.off - off;
}

int estx_print_tree(const estx_t *estx, print_dst_t dst)
{
	if (estx == NULL) {
		return 0;
	}

	int off = dst.off;

	estx_rule_data_t *data;
	int first = 1;
	arr_foreach(&estx->rules, data)
	{
		if (!first) {
			dst.off += dprintf(dst, "\n");
		}
		dst.off += dprintf(dst, "<%.*s>\n", data->name.len, data->name.data);
		dst.off += tree_print(&estx->terms, data->terms, term_print_cb, dst, estx);
		first = 0;
	}

	return dst.off - off;
}
