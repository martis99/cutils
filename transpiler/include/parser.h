#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "syntax.h"
#include "tree.h"

#define PRS_NODE_END TREE_END

typedef tnode_t prs_node_t;

typedef enum prs_node_type_e {
	PRS_NODE_UNKNOWN,
	PRS_NODE_RULE,
	PRS_NODE_TOKEN,
	PRS_NODE_LITERAL,
} prs_node_type_t;

typedef struct prs_node_data_s {
	prs_node_type_t type;
	lex_token_t token;
	union {
		stx_rule_t rule;
		str_t literal;
	} val;
} prs_node_data_t;

typedef struct prs_s {
	const stx_t *stx;
	const lex_t *lex;
	tree_t nodes;
} prs_t;

prs_t *prs_init(prs_t *prs, uint nodes_cap);
void prs_free(prs_t *prs);

prs_node_t prs_add(prs_t *prs, prs_node_data_t node);

prs_node_t prs_add_node(prs_t *prs, prs_node_t parent, prs_node_t node);
int prs_remove_node(prs_t *prs, prs_node_t node);

prs_node_t prs_get_rule(const prs_t *prs, prs_node_t parent, stx_rule_t rule);
int prs_get_str(const prs_t *prs, prs_node_t parent, str_t *out);

prs_node_t prs_parse(prs_t *prs, const stx_t *stx, stx_rule_t rule, const lex_t *lex);

int prs_print(const prs_t *prs, prs_node_t node, print_dst_t dst);

#define PRS_NODE_RULE(_prs, _rule)	 prs_add(_prs, (prs_node_data_t){ .type = PRS_NODE_RULE, .val.rule = _rule })
#define PRS_NODE_TOKEN(_prs, _token)	 prs_add(_prs, (prs_node_data_t){ .type = PRS_NODE_TOKEN, .token = _token })
#define PRS_NODE_LITERAL(_prs, _literal) prs_add(_prs, (prs_node_data_t){ .type = PRS_NODE_LITERAL, .val.literal = _literal })

#endif
