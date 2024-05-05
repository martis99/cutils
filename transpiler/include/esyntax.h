#ifndef ESYNTAX_H
#define ESYNTAX_H

#include "arr.h"
#include "print.h"
#include "str.h"
#include "token.h"
#include "tree.h"

#define ESTX_RULE_END ARR_END
#define ESTX_TERM_END TREE_END

typedef uint estx_rule_t;
typedef tnode_t estx_term_t;

typedef enum estx_term_type_e {
	ESTX_TERM_NONE,
	ESTX_TERM_RULE,
	ESTX_TERM_TOKEN,
	ESTX_TERM_LITERAL,
	ESTX_TERM_ALT,
	ESTX_TERM_CON,
	ESTX_TERM_GROUP,
} estx_term_type_t;

typedef enum estx_term_occ_e {
	ESTX_TERM_OCC_ONE = 0,
	ESTX_TERM_OCC_OPT = 1 << 0,
	ESTX_TERM_OCC_REP = 1 << 1,
} estx_term_occ_t;

typedef struct estx_term_data_s {
	estx_term_type_t type;
	estx_term_occ_t occ;
	union {
		estx_rule_t rule;
		token_type_t token;
		str_t literal;
	} val;
} estx_term_data_t;

typedef struct estx_rule_data_s {
	str_t name;
	estx_term_t terms;
} estx_rule_data_t;

typedef struct estx_s {
	arr_t rules;
	tree_t terms;
	estx_rule_t root;
	size_t max_rule_len;
} estx_t;

estx_t *estx_init(estx_t *estx, uint rules_cap, uint terms_cap);
void estx_free(estx_t *estx);

estx_rule_t estx_add_rule(estx_t *estx, str_t name);
estx_rule_t estx_get_rule(const estx_t *estx, str_t name);
estx_rule_data_t *estx_get_rule_data(const estx_t *estx, estx_rule_t rule);

estx_term_t estx_create_term(estx_t *estx, estx_term_data_t term);
estx_term_data_t *estx_get_term_data(const estx_t *estx, estx_term_t term);

estx_term_t estx_rule_set_term(estx_t *estx, estx_rule_t rule, estx_term_t term);
estx_term_t estx_term_add_term(estx_t *estx, estx_term_t term, estx_term_t child);

int estx_compile(estx_t *estx);

int estx_print(const estx_t *estx, print_dst_t dst);
int estx_print_tree(const estx_t *estx, print_dst_t dst);

#define ESTX_TERM_NONE(_estx)			 ESTX_TERM_END
#define ESTX_TERM_RULE(_estx, _rule, _occ)	 estx_create_term(_estx, (estx_term_data_t){ .type = ESTX_TERM_RULE, .val.rule = _rule, .occ = _occ })
#define ESTX_TERM_TOKEN(_estx, _token, _occ)	 estx_create_term(_estx, (estx_term_data_t){ .type = ESTX_TERM_TOKEN, .val.token = _token, .occ = _occ })
#define ESTX_TERM_LITERAL(_estx, _literal, _occ) estx_create_term(_estx, (estx_term_data_t){ .type = ESTX_TERM_LITERAL, .val.literal = _literal, .occ = _occ })
#define ESTX_TERM_ALT(_estx)			 estx_create_term(_estx, (estx_term_data_t){ .type = ESTX_TERM_ALT })
#define ESTX_TERM_CON(_estx)			 estx_create_term(_estx, (estx_term_data_t){ .type = ESTX_TERM_CON })
#define ESTX_TERM_GROUP(_estx, _occ)		 estx_create_term(_estx, (estx_term_data_t){ .type = ESTX_TERM_GROUP, .occ = _occ })

#define estx_rule_foreach arr_foreach
#define estx_term_foreach tree_foreach_child

#endif
