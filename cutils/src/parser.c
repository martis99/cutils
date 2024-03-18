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

	prs->root = PRS_NODE_END;

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

prs_node_t prs_add_node(prs_t *prs, prs_node_t parent, prs_node_data_t node)
{
	return prs_set_node(prs, parent, prs_add(prs, node));
}

prs_node_t prs_set_node(prs_t *prs, prs_node_t parent, prs_node_t child)
{
	if (prs == NULL) {
		return PRS_NODE_END;
	}

	return tree_set_child_node(&prs->nodes, parent, child);
}

int prs_remove_node(prs_t *prs, prs_node_t node)
{
	if (prs == NULL) {
		return 1;
	}

	return tree_remove(&prs->nodes, node);
}

static lex_token_t prs_parse_rule(prs_t *prs, stx_rule_t rule_id, lex_token_t cur, prs_node_t node, lex_token_t *err, stx_term_t *exp);
static lex_token_t prs_parse_terms(prs_t *prs, stx_term_t terms, lex_token_t cur, prs_node_t node, lex_token_t *err, stx_term_t *exp, stx_rule_t *rule_cache,
				   lex_token_t *rule_app);

static lex_token_t prs_parse_term(prs_t *prs, stx_term_t term_id, lex_token_t cur, prs_node_t node, lex_token_t *err, stx_term_t *exp, stx_rule_t *rule_cache,
				  lex_token_t *rule_app)
{
	const stx_term_data_t *term = stx_get_term_data(prs->stx, term_id);

	switch (term->type) {
	case STX_TERM_RULE: {
		if (rule_cache) {
			if (*rule_cache == term->val.rule) {
				return *rule_app;
			} else {
				*rule_cache = term->val.rule;
			}
		}
		prs_node_t child = prs_add(prs, PRS_NODE_RULE(term->val.rule));
		lex_token_t app	 = prs_parse_rule(prs, term->val.rule, cur, child, err, exp);
		if (app > 0) {
			prs_set_node(prs, node, child);
		}
		if (rule_app) {
			*rule_app = app;
		}
		return app;
	}
	case STX_TERM_TOKEN: {
		const token_type_t token_type = term->val.token;

		const str_t token_str = token_type_str(token_type);

		if (cur >= prs->lex->tokens.cnt) {
			*err = cur;
			*exp = term_id;
			log_trace("cutils", "parser", NULL, "%.*s: failed: end of tokens", token_str.len, token_str.data);
			return 0;
		}

		const token_t *token = lex_get_token(prs->lex, cur);
		if (token->type & (1 << token_type)) {
			prs_add_node(prs, node, PRS_NODE_TOKEN(cur));
			log_trace("cutils", "parser", NULL, "%.*s: success +%d", token_str.len, token_str.data, token->value.len);
			return token->value.len;
		}

		if (*err == LEX_TOKEN_END || cur >= *err) {
			*err = cur;
			*exp = term_id;
		}
		log_trace("cutils", "parser", NULL, "%.*s: failed: got: " BYTE_TO_BIN_PATTERN, token_str.len, token_str.data, BYTE_TO_BIN(token->type));
		return 0;
	}
	case STX_TERM_LITERAL: {
		const str_t literal = term->val.literal;

		for (size_t i = 0; i < literal.len; i++) {
			if (cur + i >= prs->lex->tokens.cnt) {
				//TODO: Test
				*err = cur + i;
				*exp = term_id;
				log_trace("cutils", "parser", NULL, "\'%*s\': failed: end of tokens", literal.len, literal.data);
				return 0;
			}

			const token_t *token = lex_get_token(prs->lex, cur + i);

			str_t c = strc(&literal.data[i], 1);
			if (!str_eq(token->value, c)) {
				if (*err == LEX_TOKEN_END || cur + i >= *err) {
					*err = cur + i;
					*exp = term_id;
				}

				char buf[256] = { 0 };
				const int len = str_print(token->value, PRINT_DST_BUF(buf, sizeof(buf), 0));

				log_trace("cutils", "parser", NULL, "\'%*s\': failed: got: \'%.*s\'", literal.len, literal.data, len, buf);
				return 0;
			}
		}

		prs_add_node(prs, node, PRS_NODE_LITERAL(literal));
		log_trace("cutils", "parser", NULL, "\'%*s\': success +%d", literal.len, literal.data, literal.len);
		return literal.len;
	}
	case STX_TERM_OR: {
		stx_rule_t cache_rule = STX_RULE_END;
		lex_token_t cache_app = 0;

		prs_node_t child0 = prs_add(prs, PRS_NODE_ALT(0));
		lex_token_t l	  = prs_parse_terms(prs, term->val.orv.l, cur, child0, err, exp, &cache_rule, &cache_app);
		if (l == 0) {
			log_trace("cutils", "parser", NULL, "left: failed");
			prs_node_t child1 = prs_add(prs, PRS_NODE_ALT(1));
			lex_token_t r	  = prs_parse_terms(prs, term->val.orv.r, cur, child1, err, exp, &cache_rule, &cache_app);
			if (r == 0) {
				log_trace("cutils", "parser", NULL, "right: failed");
				return 0;
			} else {
				prs_set_node(prs, node, child1);
				log_trace("cutils", "parser", NULL, "right: success");
				return r;
			}
		} else {
			prs_set_node(prs, node, child0);
			log_trace("cutils", "parser", NULL, "left: success");
			return l;
		}
		break;
	}
	default: log_warn("cutils", "parser", NULL, "unknown term type: %d", term->type); break;
	}

	return 0;
}

static lex_token_t prs_parse_terms(prs_t *prs, stx_term_t terms, lex_token_t cur, prs_node_t node, lex_token_t *err, stx_term_t *exp, stx_rule_t *rule_cache,
				   lex_token_t *rule_app)
{
	lex_token_t ret = 0;

	const stx_term_data_t *term;
	stx_term_foreach(&prs->stx->terms, terms, term)
	{
		lex_token_t app = prs_parse_term(prs, _i, cur + ret, node, err, exp, _i == terms ? rule_cache : NULL, _i == terms ? rule_app : NULL);
		if (app == 0) {
			return 0;
		}

		ret += app;
	}

	return ret;
}

static lex_token_t prs_parse_rule(prs_t *prs, const stx_rule_t rule_id, lex_token_t cur, prs_node_t node, lex_token_t *err, stx_term_t *exp)
{
	(void)cur;
	(void)err;
	(void)exp;
	const stx_rule_data_t *rule = stx_get_rule_data(prs->stx, rule_id);
	if (rule == NULL) {
		log_error("cutils", "parser", NULL, "rule not found: %d", rule_id);
		return 0;
	}

	log_trace("cutils", "parser", NULL, "<%*s>", rule->name.len, rule->name.data);

	lex_token_t app = prs_parse_terms(prs, rule->terms, cur, node, err, exp, NULL, 0);
	if (app == 0) {
		log_trace("cutils", "parser", NULL, "<%*s>: failed", rule->name.len, rule->name.data);
	} else {
		log_trace("cutils", "parser", NULL, "<%*s>: success +%d", rule->name.len, rule->name.data, app);
	}

	return app;
}

int prs_parse(prs_t *prs, const stx_t *stx, const lex_t *lex)
{
	if (prs == NULL || stx == NULL || lex == NULL) {
		return 1;
	}

	prs->stx = stx;
	prs->lex = lex;

	lex_token_t err = LEX_TOKEN_END;
	stx_term_t exp	= STX_TERM_END;

	prs->root = prs_add_node(prs, prs->root, PRS_NODE_RULE(stx->root));

	lex_token_t ret = prs_parse_rule(prs, stx->root, lex->root, prs->root, &err, &exp);
	if (ret == lex->tokens.cnt) {
		log_trace("cutils", "parser", NULL, "success");
		return 0;
	} else {
		if (exp != STX_TERM_END) {
			const stx_term_data_t *term = stx_get_term_data(stx, exp);
			if (term != NULL) {
				if (term->type == STX_TERM_TOKEN) {
					const str_t exp_token = token_type_str(term->val.token);
					log_error("cutils", "parser", NULL, "parsing failed: expected: %.*s", exp_token.len, exp_token.data);
				} else if (term->type == STX_TERM_LITERAL) {
					const str_t exp_str = term->val.literal;
					log_error("cutils", "parser", NULL, "parsing failed: expected: \'%.*s\'", exp_str.len, exp_str.data);
				}
			}
		}

		if (err != LEX_TOKEN_END) {
			char buf[256] = { 0 };
			const int off = err == lex->tokens.cnt ? 1 : 0;

			const token_t *token = lex_get_token(lex, err - off);
			if (token != NULL) {
				const int len = lex_print_line(lex, err - off, PRINT_DST_BUF(buf, sizeof(buf), 0));
				log_error("cutils", "parser", NULL, "%d: %.*s", token->line, len, buf);
				log_error("cutils", "parser", NULL, "%d: %*s^", token->line, token->col + off, "");
			}
		}
	}

	return 1;
}

static int print_nodes(void *data, print_dst_t dst, const void *priv)
{
	const prs_t *prs = priv;

	int off = dst.off;

	const prs_node_data_t *node = data;
	switch (node->type) {
	case PRS_NODE_RULE: {
		const stx_rule_data_t *rule = stx_get_rule_data(prs->stx, node->val.rule);
		dst.off += c_print_exec(dst, "%.*s\n", rule->name.len, rule->name.data);
		break;
	}
	case PRS_NODE_TOKEN: {
		const token_t *token = lex_get_token(prs->lex, node->token);

		char buf[256] = { 0 };
		const int len = str_print(token->value, PRINT_DST_BUF(buf, sizeof(buf), 0));
		dst.off += c_print_exec(dst, "\'%.*s\'\n", len, buf);
		break;
	}
	case PRS_NODE_LITERAL: {
		const str_t literal = node->val.literal;
		dst.off += c_print_exec(dst, "\'%.*s\'\n", literal.len, literal.data);
		break;
	}
	case PRS_NODE_ALT: {
		const int alt = node->val.alt;
		dst.off += c_print_exec(dst, "%d\n", alt);
		break;
	}
	default: break;
	}

	return dst.off - off;
}

int prs_print(const prs_t *prs, print_dst_t dst)
{
	if (prs == NULL) {
		return 0;
	}
	return tree_print(&prs->nodes, prs->root, print_nodes, dst, prs);
}
