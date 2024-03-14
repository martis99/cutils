#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "syntax.h"
#include "tree.h"

#define PRS_NODE_END TREE_END

typedef tnode_t prs_node_t;

typedef enum prs_node_type_e {
	PRS_NODE_RULE,
	PRS_NODE_TOKEN,
	PRS_NODE_LITERAL,
	PRS_NODE_ALT,
} prs_node_type_t;

typedef struct prs_node_data_s {
	prs_node_type_t type;
	lex_token_t token;
	union {
		stx_rule_t rule;
		str_t literal;
		int alt;
	} val;
} prs_node_data_t;

typedef struct prs_s {
	const stx_t *stx;
	const lex_t *lex;
	tree_t nodes;
	prs_node_t root;
} prs_t;

prs_t *prs_init(prs_t *prs, uint nodes_cap);
void prs_free(prs_t *prs);

prs_node_t prs_add(prs_t *prs, prs_node_data_t node);

prs_node_t prs_add_node(prs_t *prs, prs_node_t parent, prs_node_data_t node);
prs_node_t prs_set_node(prs_t *prs, prs_node_t parent, prs_node_t child);
int prs_remove_node(prs_t *prs, prs_node_t node);

int prs_parse(prs_t *prs, const stx_t *stx, const lex_t *lex);

int prs_print(const prs_t *prs, void *priv);

#define PRS_NODE_RULE(_rule)                              \
	(prs_node_data_t)                                 \
	{                                                 \
		.type = PRS_NODE_RULE, .val.rule = _rule, \
	}

#define PRS_NODE_TOKEN(_token)                           \
	(prs_node_data_t)                                \
	{                                                \
		.type = PRS_NODE_TOKEN, .token = _token, \
	}

#define PRS_NODE_LITERAL(_literal)                                 \
	(prs_node_data_t)                                          \
	{                                                          \
		.type = PRS_NODE_LITERAL, .val.literal = _literal, \
	}

#define PRS_NODE_ALT(_alt)                             \
	(prs_node_data_t)                              \
	{                                              \
		.type = PRS_NODE_ALT, .val.alt = _alt, \
	}

#endif
