#include "parser.h"

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

prs_t *prs_init(prs_t *prs, uint nodes_cap)
{
	if (prs == NULL) {
		return NULL;
	}

	if (tree_init(&prs->nodes, nodes_cap, sizeof(prs_node_data_t)) == NULL) {
		log_error("cutils", "parser", NULL, "failed to initialize nodes tree");
		return NULL;
	}

	return prs;
}

void prs_free(prs_t *prs)
{
	if (prs == NULL) {
		return;
	}

	tree_free(&prs->nodes);
}

prs_node_t prs_add(prs_t *prs, prs_node_data_t node)
{
	if (prs == NULL) {
		return PRS_NODE_END;
	}

	prs_node_t child = tree_add(&prs->nodes);

	prs_node_data_t *data = tree_get_data(&prs->nodes, child);
	if (data == NULL) {
		log_error("cutils", "parser", NULL, "failed to add node");
		return PRS_NODE_END;
	}

	*data = node;

	return child;
}

prs_node_t prs_add_node(prs_t *prs, prs_node_t parent, prs_node_t node)
{
	if (prs == NULL) {
		return PRS_NODE_END;
	}

	return tree_set_child_node(&prs->nodes, parent, node);
}

int prs_remove_node(prs_t *prs, prs_node_t node)
{
	if (prs == NULL) {
		return 1;
	}

	return tree_remove(&prs->nodes, node);
}

prs_node_t prs_get_rule(const prs_t *prs, prs_node_t parent, stx_rule_t rule)
{
	if (prs == NULL) {
		return PRS_NODE_END;
	}

	prs_node_t child;
	tree_foreach_child(&prs->nodes, parent, child)
	{
		prs_node_data_t *cdata = tree_get_data(&prs->nodes, child);
		switch (cdata->type) {
		case PRS_NODE_RULE:
			if (cdata->val.rule == rule) {
				return child;
			}
			break;
		default: break;
		}
	}

	return PRS_NODE_END;
}

int prs_get_str(const prs_t *prs, prs_node_t parent, str_t *out)
{
	if (prs == NULL || out == NULL) {
		return 1;
	}

	prs_node_t child;
	tree_foreach_child(&prs->nodes, parent, child)
	{
		prs_node_data_t *data = tree_get_data(&prs->nodes, child);
		switch (data->type) {
		case PRS_NODE_RULE: prs_get_str(prs, child, out); break;
		case PRS_NODE_TOKEN: {
			const token_t *token = lex_get_token(prs->lex, data->token);
			if (token == NULL) {
				log_warn("cutils", "parser", NULL, "token not found: %d", data->token);
				break;
			}

			str_cat(out, token->value);
			break;
		}
		case PRS_NODE_LITERAL: str_cat(out, data->val.literal); break;
		}
	}

	return 0;
}

typedef struct prs_parse_err_s {
	stx_rule_t rule;
	lex_token_t tok;
	stx_term_t exp;
} prs_parse_err_t;

static int prs_parse_rule(prs_t *prs, stx_rule_t rule_id, lex_token_t *off, prs_node_t node, prs_parse_err_t *err);
static int prs_parse_terms(prs_t *prs, stx_rule_t rule, stx_term_t terms, lex_token_t *off, prs_node_t node, prs_parse_err_t *err);

static int prs_parse_term(prs_t *prs, stx_rule_t rule, stx_term_t term_id, lex_token_t *off, prs_node_t node, prs_parse_err_t *err)
{
	const stx_term_data_t *term = stx_get_term_data(prs->stx, term_id);

	switch (term->type) {
	case STX_TERM_RULE: {
		uint nodes_cnt	 = prs->nodes.cnt;
		lex_token_t cur	 = *off;
		prs_node_t child = PRS_NODE_RULE(prs, term->val.rule);
		if (prs_parse_rule(prs, term->val.rule, off, child, err)) {
			prs->nodes.cnt = nodes_cnt;
			*off	       = cur;
			return 1;
		}

		prs_add_node(prs, node, child);
		return 0;
	}
	case STX_TERM_TOKEN: {
		const token_type_t token_type = term->val.token;

		const str_t token_str = token_type_str(token_type);

		if (*off >= prs->lex->tokens.cnt) {
			err->rule = rule;
			err->tok  = *off;
			err->exp  = term_id;
			log_trace("cutils", "parser", NULL, "%.*s: failed: end of tokens", token_str.len, token_str.data);
			return 1;
		}

		const token_t *token = lex_get_token(prs->lex, *off);
		if (token->type & (1 << token_type)) {
			prs_add_node(prs, node, PRS_NODE_TOKEN(prs, *off));
			log_trace("cutils", "parser", NULL, "%.*s: success +%d", token_str.len, token_str.data, token->value.len);
			*off += token->value.len;
			return 0;
		}

		if (err->tok == LEX_TOKEN_END || *off >= err->tok) {
			err->rule = rule;
			err->tok  = *off;
			err->exp  = term_id;
		}
		log_trace("cutils", "parser", NULL, "%.*s: failed: got: " BYTE_TO_BIN_PATTERN, token_str.len, token_str.data, BYTE_TO_BIN(token->type));
		return 1;
	}
	case STX_TERM_LITERAL: {
		const str_t literal = term->val.literal;

		for (size_t i = 0; i < literal.len; i++) {
			if (*off + i >= prs->lex->tokens.cnt) {
				//TODO: Test
				err->rule = rule;
				err->tok  = *off + i;
				err->exp  = term_id;
				log_trace("cutils", "parser", NULL, "\'%*s\': failed: end of tokens", literal.len, literal.data);
				return 1;
			}

			const token_t *token = lex_get_token(prs->lex, *off + i);

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

		prs_add_node(prs, node, PRS_NODE_LITERAL(prs, literal));
		log_trace("cutils", "parser", NULL, "\'%*s\': success +%d", literal.len, literal.data, literal.len);
		*off += literal.len;
		return 0;
	}
	case STX_TERM_OR: {
		stx_rule_t cache_rule = STX_RULE_END;
		lex_token_t cache_app = 0;
		int from_cache	      = 0;

		uint nodes_cnt	= prs->nodes.cnt;
		lex_token_t cur = *off;
		if (!prs_parse_terms(prs, rule, term->val.orv.l, off, node, err)) {
			log_trace("cutils", "parser", NULL, "left: success");
			return 0;
		}

		log_trace("cutils", "parser", NULL, "left: failed");
		tree_set_cnt(&prs->nodes, nodes_cnt);

		if (!prs_parse_terms(prs, rule, term->val.orv.r, off, node, err)) {
			log_trace("cutils", "parser", NULL, "right: success");
			return 0;
		}

		log_trace("cutils", "parser", NULL, "right: failed");
		tree_set_cnt(&prs->nodes, nodes_cnt);
		*off = cur;
		return 1;
	}
	default: log_warn("cutils", "parser", NULL, "unknown term type: %d", term->type); break;
	}

	return 1;
}

static int prs_parse_terms(prs_t *prs, stx_rule_t rule, stx_term_t terms, lex_token_t *off, prs_node_t node, prs_parse_err_t *err)
{
	lex_token_t cur = *off;
	const stx_term_data_t *term;
	stx_term_foreach(&prs->stx->terms, terms, term)
	{
		if (prs_parse_term(prs, rule, _i, off, node, err)) {
			*off = cur;
			return 1;
		}
	}

	return 0;
}

static int prs_parse_rule(prs_t *prs, const stx_rule_t rule_id, lex_token_t *off, prs_node_t node, prs_parse_err_t *err)
{
	const stx_rule_data_t *rule = stx_get_rule_data(prs->stx, rule_id);
	if (rule == NULL) {
		log_error("cutils", "parser", NULL, "rule not found: %d", rule_id);
		return 1;
	}

	log_trace("cutils", "parser", NULL, "<%*s>", rule->name.len, rule->name.data);

	lex_token_t cur = *off;
	if (prs_parse_terms(prs, rule_id, rule->terms, off, node, err)) {
		log_trace("cutils", "parser", NULL, "<%*s>: failed", rule->name.len, rule->name.data);
		*off = cur;
		return 1;
	}

	log_trace("cutils", "parser", NULL, "<%*s>: success +%d", rule->name.len, rule->name.data, *off - cur);
	return 0;
}

prs_node_t prs_parse(prs_t *prs, const stx_t *stx, stx_rule_t rule, const lex_t *lex)
{
	if (prs == NULL || stx == NULL || lex == NULL) {
		return PRS_NODE_END;
	}

	prs->stx = stx;
	prs->lex = lex;

	prs_parse_err_t err = {
		.rule = STX_RULE_END,
		.tok  = LEX_TOKEN_END,
		.exp  = STX_TERM_END,
	};

	prs_node_t root	   = prs_add_node(prs, PRS_NODE_END, PRS_NODE_RULE(prs, rule));
	lex_token_t parsed = lex->root;
	if (prs_parse_rule(prs, rule, &parsed, root, &err) || parsed != lex->tokens.cnt) {
		if (err.exp != STX_TERM_END) {
			const stx_term_data_t *term = stx_get_term_data(stx, err.exp);
			if (term != NULL) {
				if (term->type == STX_TERM_TOKEN) {
					const stx_rule_data_t *rule = stx_get_rule_data(prs->stx, err.rule);
					const str_t exp_token	    = token_type_str(term->val.token);
					log_error("cutils", "parser", NULL, "%.*s parsing failed: expected: %.*s", rule->name.len, rule->name.data, exp_token.len,
						  exp_token.data);
				} else if (term->type == STX_TERM_LITERAL) {
					const stx_rule_data_t *rule = stx_get_rule_data(prs->stx, err.rule);
					const str_t exp_str	    = term->val.literal;
					log_error("cutils", "parser", NULL, "%.*s parsing failed: expected: \'%.*s\'", rule->name.len, rule->name.data, exp_str.len,
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
				log_error("cutils", "parser", NULL, "%d: %.*s", token->line, len, buf);
				log_error("cutils", "parser", NULL, "%d: %*s^", token->line, token->col + off, "");
			}
		}
		return PRS_NODE_END;
	}

	log_trace("cutils", "parser", NULL, "success");
	return root;
}

static int print_nodes(void *data, print_dst_t dst, const void *priv)
{
	const prs_t *prs = priv;

	int off = dst.off;

	const prs_node_data_t *node = data;
	switch (node->type) {
	case PRS_NODE_RULE: {
		const stx_rule_data_t *rule = stx_get_rule_data(prs->stx, node->val.rule);
		dst.off += dprintf(dst, "%.*s\n", rule->name.len, rule->name.data);
		break;
	}
	case PRS_NODE_TOKEN: {
		const token_t *token = lex_get_token(prs->lex, node->token);

		char buf[256] = { 0 };
		const int len = str_print(token->value, PRINT_DST_BUF(buf, sizeof(buf), 0));
		dst.off += dprintf(dst, "\'%.*s\'\n", len, buf);
		break;
	}
	case PRS_NODE_LITERAL: {
		const str_t literal = node->val.literal;
		dst.off += dprintf(dst, "\'%.*s\'\n", literal.len, literal.data);
		break;
	}
	default: break;
	}

	return dst.off - off;
}

int prs_print(const prs_t *prs, prs_node_t node, print_dst_t dst)
{
	if (prs == NULL) {
		return 0;
	}
	return tree_print(&prs->nodes, node, print_nodes, dst, prs);
}
