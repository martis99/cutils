#ifndef EPARSER_H
#define EPARSER_H

#include "esyntax.h"
#include "lexer.h"
#include "tree.h"

#define EPRS_NODE_END TREE_END

typedef tnode_t eprs_node_t;

typedef enum eprs_node_type_e {
	EPRS_NODE_UNKNOWN,
	EPRS_NODE_RULE,
	EPRS_NODE_TOKEN,
	EPRS_NODE_LITERAL,
	EPRS_NODE_ALT,
	EPRS_NODE_CON,
	EPRS_NODE_GROUP,
} eprs_node_type_t;

typedef struct eprs_node_data_s {
	eprs_node_type_t type;
	lex_token_t token;
	union {
		estx_rule_t rule;
		str_t literal;
		int alt;
	} val;
} eprs_node_data_t;

typedef struct eprs_s {
	const estx_t *estx;
	const lex_t *lex;
	tree_t nodes;
} eprs_t;

eprs_t *eprs_init(eprs_t *eprs, uint nodes_cap);
void eprs_free(eprs_t *eprs);

eprs_node_t eprs_add(eprs_t *eprs, eprs_node_data_t node);

eprs_node_t eprs_add_node(eprs_t *eprs, eprs_node_t parent, eprs_node_t node);
int eprs_remove_node(eprs_t *eprs, eprs_node_t node);

eprs_node_t eprs_get_rule(const eprs_t *eprs, eprs_node_t parent, estx_rule_t rule);
int eprs_get_str(const eprs_t *eprs, eprs_node_t parent, str_t *out);

eprs_node_t eprs_parse(eprs_t *eprs, const estx_t *stx, estx_rule_t rule, const lex_t *lex);

int eprs_print(const eprs_t *eprs, eprs_node_t node, print_dst_t dst);

#define EPRS_NODE_RULE(_prs, _rule)	  eprs_add(_prs, (eprs_node_data_t){ .type = EPRS_NODE_RULE, .val.rule = _rule })
#define EPRS_NODE_TOKEN(_prs, _token)	  eprs_add(_prs, (eprs_node_data_t){ .type = EPRS_NODE_TOKEN, .token = _token })
#define EPRS_NODE_LITERAL(_prs, _literal) eprs_add(_prs, (eprs_node_data_t){ .type = EPRS_NODE_LITERAL, .val.literal = _literal })
#define EPRS_NODE_ALT(_prs, _alt)	  eprs_add(_prs, (eprs_node_data_t){ .type = EPRS_NODE_ALT, .val.alt = _alt })
#define EPRS_NODE_CON(_prs)		  eprs_add(_prs, (eprs_node_data_t){ .type = EPRS_NODE_CON })
#define EPRS_NODE_GROUP(_prs)		  eprs_add(_prs, (eprs_node_data_t){ .type = EPRS_NODE_GROUP })

#endif
