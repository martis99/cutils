#ifndef MAKE_H
#define MAKE_H

#include "arr.h"
#include "list.h"
#include "str.h"

#include <stdio.h>

typedef lnode_t make_str_t;

typedef lnode_t make_act_t;
typedef make_act_t make_var_t;
typedef make_act_t make_rule_t;
typedef make_act_t make_cmd_t;
typedef make_act_t make_if_t;

typedef enum make_str_type_e {
	MAKE_STR_STR,
	MAKE_STR_VAR,
} make_str_type_t;

typedef struct make_str_data_s {
	make_str_type_t type;
	union {
		str_t str;
		make_var_t var;
	};
} make_str_data_t;

typedef enum make_var_type_e {
	MAKE_VAR_INST, //:=
	MAKE_VAR_REF, //=
	MAKE_VAR_COND, //?=
	MAKE_VAR_APP, //+=
} make_var_type_t;

typedef struct make_s {
	list_t strs;
	list_t acts;
	make_act_t g_acts;
} make_t;

make_t *make_init(make_t *make, uint acts_cnt, uint strs_cnt);
void make_free(make_t *make);

make_var_t make_create_var_r(make_t *make, str_t name, make_var_type_t type, int ext);
make_var_t make_create_var(make_t *make, str_t name, make_var_type_t type);
make_var_t make_create_var_ext(make_t *make, str_t name, make_var_type_t type);
make_rule_t make_create_rule(make_t *make, make_str_data_t target);
make_cmd_t make_create_cmd(make_t *make, str_t cmd);
make_if_t make_create_if(make_t *make, make_str_data_t l, make_str_data_t r);

make_act_t make_add_act(make_t *make, make_act_t act);

make_str_t make_var_add_val(make_t *make, make_var_t var, make_str_data_t val);

make_str_t make_rule_add_depend(make_t *make, make_rule_t rule, make_str_data_t depend);
make_act_t make_rule_add_act(make_t *make, make_rule_t rule, make_act_t act);

make_act_t make_if_add_true_act(make_t *make, make_if_t mif, make_act_t act);
make_act_t make_if_add_false_act(make_t *make, make_if_t mif, make_act_t act);

make_str_t make_set_ext(make_t *make, str_t name, make_str_data_t val);

int make_expand(make_t *make);

str_t make_get_expanded(make_t *make, str_t var);

int make_print(const make_t *make, FILE *file);

int make_dbg(const make_t *make, FILE *file);

#define MSTR(_str)                                     \
	(make_str_data_t)                              \
	{                                              \
		.type = MAKE_STR_STR, .str = STR(_str) \
	}

#define MVAR(_var)                                \
	(make_str_data_t)                         \
	{                                         \
		.type = MAKE_STR_VAR, .var = _var \
	}

#endif
