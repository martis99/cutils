#include "eparser.h"

#include "log.h"

#define BYTE_TO_BIN_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"

// clang-format off
#define BYTE_TO_BIN(byte)  \
  (byte & 0x8000 ? '1' : '0'), \
  (byte & 0x4000 ? '1' : '0'), \
  (byte & 0x2000 ? '1' : '0'), \
  (byte & 0x1000 ? '1' : '0'), \
  (byte & 0x0800 ? '1' : '0'), \
  (byte & 0x0400 ? '1' : '0'), \
  (byte & 0x0200 ? '1' : '0'), \
  (byte & 0x0100 ? '1' : '0'), \
  (byte & 0x0080 ? '1' : '0'), \
  (byte & 0x0040 ? '1' : '0'), \
  (byte & 0x0020 ? '1' : '0'), \
  (byte & 0x0010 ? '1' : '0'), \
  (byte & 0x0008 ? '1' : '0'), \
  (byte & 0x0004 ? '1' : '0'), \
  (byte & 0x0002 ? '1' : '0'), \
  (byte & 0x0001 ? '1' : '0')
// clang-format on

eprs_t *eprs_init(eprs_t *eprs, uint nodes_cap)
{
	if (eprs == NULL) {
		return NULL;
	}

	if (tree_init(&eprs->nodes, nodes_cap, sizeof(eprs_node_data_t)) == NULL) {
		log_error("cutils", "eparser", NULL, "failed to initialize nodes tree");
		return NULL;
	}

	return eprs;
}

void eprs_free(eprs_t *eprs)
{
	if (eprs == NULL) {
		return;
	}

	tree_free(&eprs->nodes);
}

eprs_node_t eprs_add(eprs_t *eprs, eprs_node_data_t node)
{
	if (eprs == NULL) {
		return EPRS_NODE_END;
	}

	eprs_node_t child = tree_add(&eprs->nodes);

	eprs_node_data_t *data = tree_get_data(&eprs->nodes, child);
	if (data == NULL) {
		log_error("cutils", "eparser", NULL, "failed to add node");
		return EPRS_NODE_END;
	}

	*data = node;

	return child;
}

eprs_node_t eprs_add_node(eprs_t *eprs, eprs_node_t parent, eprs_node_t node)
{
	if (eprs == NULL) {
		return EPRS_NODE_END;
	}

	return tree_set_child_node(&eprs->nodes, parent, node);
}

int eprs_remove_node(eprs_t *eprs, eprs_node_t node)
{
	if (eprs == NULL) {
		return 1;
	}

	return tree_remove(&eprs->nodes, node);
}

eprs_node_t eprs_get_rule(const eprs_t *eprs, eprs_node_t parent, estx_rule_t rule)
{
	if (eprs == NULL) {
		return EPRS_NODE_END;
	}

	eprs_node_t child;
	tree_foreach_child(&eprs->nodes, parent, child)
	{
		eprs_node_data_t *cdata = tree_get_data(&eprs->nodes, child);
		switch (cdata->type) {
		case EPRS_NODE_ALT:
		case EPRS_NODE_CON:
		case EPRS_NODE_GROUP: {
			eprs_node_t ret = eprs_get_rule(eprs, child, rule);
			if (ret < eprs->nodes.cnt) {
				return ret;
			}
			break;
		}
		case EPRS_NODE_RULE:
			if (cdata->val.rule == rule) {
				return child;
			}
			break;
		default: break;
		}
	}

	return EPRS_NODE_END;
}

int eprs_get_str(const eprs_t *eprs, eprs_node_t parent, str_t *out)
{
	if (eprs == NULL || out == NULL) {
		return 1;
	}

	eprs_node_t child;
	tree_foreach_child(&eprs->nodes, parent, child)
	{
		eprs_node_data_t *data = tree_get_data(&eprs->nodes, child);
		switch (data->type) {
		case EPRS_NODE_RULE:
		case EPRS_NODE_ALT:
		case EPRS_NODE_CON:
		case EPRS_NODE_GROUP: eprs_get_str(eprs, child, out); break;
		case EPRS_NODE_TOKEN: {
			const token_t *token = lex_get_token(eprs->lex, data->token);
			if (token == NULL) {
				log_warn("cutils", "eparser", NULL, "token not found: %d", data->token);
				break;
			}

			str_cat(out, token->value);
			break;
		}
		case EPRS_NODE_LITERAL: str_cat(out, data->val.literal); break;
		}
	}

	return 0;
}

typedef struct eprs_parse_err_s {
	estx_rule_t rule;
	lex_token_t tok;
	estx_term_t exp;
} eprs_parse_err_t;

static int eprs_parse_rule(eprs_t *prs, const estx_rule_t rule_id, lex_token_t *off, eprs_node_t node, eprs_parse_err_t *err);
static int eprs_parse_terms(eprs_t *eprs, estx_rule_t rule, estx_term_t term_id, lex_token_t *off, eprs_node_t node, eprs_parse_err_t *err);

static int eprs_parse_term(eprs_t *eprs, estx_rule_t rule, estx_term_t term_id, lex_token_t *off, eprs_node_t node, eprs_parse_err_t *err)
{
	const estx_term_data_t *term = estx_get_term_data(eprs->estx, term_id);

	switch (term->type) {
	case ESTX_TERM_RULE: {
		eprs_node_t child = EPRS_NODE_RULE(eprs, term->val.rule);
		lex_token_t cur	  = *off;
		if (eprs_parse_rule(eprs, term->val.rule, off, child, err)) {
			*off = cur;
			return 1;
		}
		eprs_add_node(eprs, node, child);
		return 0;
	}
	case ESTX_TERM_TOKEN: {
		const token_type_t token_type = term->val.token;

		const str_t token_str = token_type_str(token_type);

		if (*off >= eprs->lex->tokens.cnt) {
			err->rule = rule;
			err->tok  = *off;
			err->exp  = term_id;
			log_trace("cutils", "eparser", NULL, "%.*s: failed: end of tokens", token_str.len, token_str.data);
			return 1;
		}

		const token_t *token = lex_get_token(eprs->lex, *off);
		if (token->type & (1 << token_type)) {
			eprs_add_node(eprs, node, EPRS_NODE_TOKEN(eprs, *off));
			log_trace("cutils", "eparser", NULL, "%.*s: success +%d", token_str.len, token_str.data, token->value.len);
			*off += token->value.len;
			return 0;
		}

		if (err->tok == LEX_TOKEN_END || *off >= err->tok) {
			err->rule = rule;
			err->tok  = *off;
			err->exp  = term_id;
		}
		log_trace("cutils", "eparser", NULL, "%.*s: failed: got: " BYTE_TO_BIN_PATTERN, token_str.len, token_str.data, BYTE_TO_BIN(token->type));
		return 1;
	}
	case ESTX_TERM_LITERAL: {
		const str_t literal = term->val.literal;

		for (size_t i = 0; i < literal.len; i++) {
			if (*off + i >= eprs->lex->tokens.cnt) {
				//TODO: Test
				err->rule = rule;
				err->tok  = *off + i;
				err->exp  = term_id;
				log_trace("cutils", "eparser", NULL, "\'%*s\': failed: end of tokens", literal.len, literal.data);
				return 1;
			}

			const token_t *token = lex_get_token(eprs->lex, *off + i);

			str_t c = strc(&literal.data[i], 1);
			if (!str_eq(token->value, c)) {
				if (err->tok == LEX_TOKEN_END || *off + i >= err->tok) {
					err->rule = rule;
					err->tok  = *off + i;
					err->exp  = term_id;
				}

				char buf[256] = { 0 };
				const int len = str_print(token->value, PRINT_DST_BUF(buf, sizeof(buf), 0));

				log_trace("cutils", "parser", NULL, "\'%*s\': failed: got: \'%.*s\'", literal.len, literal.data, len, buf);
				return 1;
			}
		}

		eprs_add_node(eprs, node, EPRS_NODE_LITERAL(eprs, literal));
		log_trace("cutils", "parser", NULL, "\'%*s\': success +%d", literal.len, literal.data, literal.len);
		*off += literal.len;
		return 0;
	}
	case ESTX_TERM_ALT: {
		uint node_cnt = eprs->nodes.cnt;

		estx_term_t child_id;
		tree_foreach_child(&eprs->estx->terms, term_id, child_id)
		{
			eprs_node_t child0 = EPRS_NODE_ALT(eprs, 0);
			lex_token_t cur	   = *off;
			if (eprs_parse_terms(eprs, rule, child_id, off, child0, err)) {
				log_trace("cutils", "eparser", NULL, "alt: failed");
				eprs->nodes.cnt = node_cnt;
				*off		= cur;
			} else {
				eprs_add_node(eprs, node, child0);
				log_trace("cutils", "parser", NULL, "alt: success");
				return 0;
			}
		}
		break;
	}
	case ESTX_TERM_CON: {
		lex_token_t cur = *off;
		estx_term_t child_id;
		tree_foreach_child(&eprs->estx->terms, term_id, child_id)
		{
			eprs_node_t child0 = EPRS_NODE_CON(eprs);
			if (eprs_parse_terms(eprs, rule, child_id, off, child0, err)) {
				log_trace("cutils", "eparser", NULL, "con: failed");
				*off = cur;
				return 1;
			} else {
				eprs_add_node(eprs, node, child0);
				log_trace("cutils", "parser", NULL, "con: success");
			}
		}
		return 0;
	}
	case ESTX_TERM_GROUP: {
		lex_token_t cur = *off;
		estx_term_t child_id;
		tree_foreach_child(&eprs->estx->terms, term_id, child_id)
		{
			eprs_node_t child0 = EPRS_NODE_GROUP(eprs);
			if (eprs_parse_terms(eprs, rule, child_id, off, child0, err)) {
				log_trace("cutils", "eparser", NULL, "group: failed");
				*off = cur;
				return 1;
			} else {
				eprs_add_node(eprs, node, child0);
				log_trace("cutils", "parser", NULL, "group: success");
			}
		}
		return 0;
	}
	default: log_warn("cutils", "eparser", NULL, "unknown term type: %d", term->type); break;
	}

	return 1;
}

static int eprs_parse_terms(eprs_t *eprs, estx_rule_t rule, estx_term_t term_id, lex_token_t *off, eprs_node_t node, eprs_parse_err_t *err)
{
	const estx_term_data_t *term = estx_get_term_data(eprs->estx, term_id);

	lex_token_t cur = *off;

	int ret = eprs_parse_term(eprs, rule, term_id, off, node, err);
	int one = term->occ == ESTX_TERM_OCC_ONE;

	if (term->type == ESTX_TERM_ALT || term->type == ESTX_TERM_CON || one) {
		return ret;
	}

	bool opt = (term->occ & ESTX_TERM_OCC_OPT) && !(term->occ & ESTX_TERM_OCC_REP);
	bool rep = !(term->occ & ESTX_TERM_OCC_OPT) && (term->occ & ESTX_TERM_OCC_REP);

	if (ret && opt) {
		*off = cur;
		return 0;
	}

	if (ret && rep) {
		log_trace("cutils", "eparser", NULL, "rep: failed");
		*off = cur;
		return ret;
	}

	while (ret == 0) {
		if (cur == *off) {
			log_warn("cutils", "eparser", NULL, "loop detected");
			break;
		}
		cur = *off;
		ret = eprs_parse_term(eprs, rule, term_id, off, node, err);
	}

	*off = cur;
	return 0;
}

static int eprs_parse_rule(eprs_t *prs, const estx_rule_t rule_id, lex_token_t *off, eprs_node_t node, eprs_parse_err_t *err)
{
	const estx_rule_data_t *rule = estx_get_rule_data(prs->estx, rule_id);
	if (rule == NULL) {
		log_error("cutils", "eparser", NULL, "rule not found: %d", rule_id);
		return 1;
	}

	log_trace("cutils", "eparser", NULL, "<%*s>", rule->name.len, rule->name.data);

	lex_token_t cur = *off;
	if (eprs_parse_terms(prs, rule_id, rule->terms, off, node, err)) {
		log_trace("cutils", "eparser", NULL, "<%*s>: failed", rule->name.len, rule->name.data);
		*off = cur;
		return 1;
	}

	log_trace("cutils", "eparser", NULL, "<%*s>: success +%d", rule->name.len, rule->name.data, *off - cur);
	return 0;
}

eprs_node_t eprs_parse(eprs_t *eprs, const estx_t *estx, estx_rule_t rule, const lex_t *lex)
{
	if (eprs == NULL || estx == NULL || lex == NULL) {
		return EPRS_NODE_END;
	}

	eprs->estx = estx;
	eprs->lex  = lex;

	eprs_parse_err_t err = {
		.rule = ESTX_RULE_END,
		.tok  = LEX_TOKEN_END,
		.exp  = ESTX_TERM_END,
	};

	eprs_node_t root   = eprs_add_node(eprs, EPRS_NODE_END, EPRS_NODE_RULE(eprs, rule));
	lex_token_t parsed = lex->root;
	if (eprs_parse_rule(eprs, rule, &parsed, root, &err) || parsed != lex->tokens.cnt) {
		if (err.exp != ESTX_TERM_END) {
			const estx_term_data_t *term = estx_get_term_data(estx, err.exp);
			if (term != NULL) {
				if (term->type == ESTX_TERM_TOKEN) {
					const estx_rule_data_t *rule = estx_get_rule_data(eprs->estx, err.rule);
					const str_t exp_token	     = token_type_str(term->val.token);
					log_error("cutils", "eparser", NULL, "%.*s parsing failed: expected: %.*s", rule->name.len, rule->name.data, exp_token.len,
						  exp_token.data);
				} else if (term->type == ESTX_TERM_LITERAL) {
					const estx_rule_data_t *rule = estx_get_rule_data(eprs->estx, err.rule);
					const str_t exp_str	     = term->val.literal;
					log_error("cutils", "eparser", NULL, "%.*s parsing failed: expected: \'%.*s\'", rule->name.len, rule->name.data, exp_str.len,
						  exp_str.data);
				}
			}
		}

		if (err.tok != LEX_TOKEN_END) {
			char buf[256] = { 0 };
			const int off = err.tok == lex->tokens.cnt ? 1 : 0;

			const token_t *token = lex_get_token(lex, err.tok - off);
			if (token != NULL) {
				const int len = lex_print_line(lex, err.tok - off, PRINT_DST_BUF(buf, sizeof(buf), 0));
				log_error("cutils", "eparser", NULL, "%d: %.*s", token->line, len, buf);
				log_error("cutils", "eparser", NULL, "%d: %*s^", token->line, token->col + off, "");
			}
		}

		return EPRS_NODE_END;
	}

	log_trace("cutils", "eparser", NULL, "success");
	return root;
}

static int print_nodes(void *data, print_dst_t dst, const void *priv)
{
	const eprs_t *eprs = priv;

	int off = dst.off;

	const eprs_node_data_t *node = data;
	switch (node->type) {
	case EPRS_NODE_RULE: {
		const estx_rule_data_t *rule = estx_get_rule_data(eprs->estx, node->val.rule);
		dst.off += dprintf(dst, "%.*s\n", rule->name.len, rule->name.data);
		break;
	}
	case EPRS_NODE_TOKEN: {
		const token_t *token = lex_get_token(eprs->lex, node->token);

		char buf[256] = { 0 };
		const int len = str_print(token->value, PRINT_DST_BUF(buf, sizeof(buf), 0));
		dst.off += dprintf(dst, "\'%.*s\'\n", len, buf);
		break;
	}
	case EPRS_NODE_LITERAL: {
		const str_t literal = node->val.literal;
		dst.off += dprintf(dst, "\'%.*s\'\n", literal.len, literal.data);
		break;
	}
	case EPRS_NODE_ALT: {
		dst.off += dprintf(dst, "alt\n");
		break;
	}
	case EPRS_NODE_CON: {
		dst.off += dprintf(dst, "con\n");
		break;
	}
	case EPRS_NODE_GROUP: {
		dst.off += dprintf(dst, "grp\n");
		break;
	}
	default: break;
	}

	return dst.off - off;
}

int eprs_print(const eprs_t *eprs, eprs_node_t node, print_dst_t dst)
{
	if (eprs == NULL) {
		return 0;
	}
	return tree_print(&eprs->nodes, node, print_nodes, dst, eprs);
}
