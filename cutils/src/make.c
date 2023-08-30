#include "make.h"

#include "platform.h"
#include "print.h"

#define MAX_VAR_VALUE_LEN 256

typedef struct make_var_data_s {
	str_t name;
	make_var_type_t type;
	lnode_t values;
	str_t ref;
	str_t value;
	int ext : 1;
	int expanded : 1;
} make_var_data_t;

typedef struct make_rule_data_s {
	make_str_data_t target;
	lnode_t depends;
	make_act_t acts;
} make_rule_data_t;

typedef struct make_cmd_data_s {
	str_t cmd;
} make_cmd_data_t;

typedef struct make_if_data_s {
	make_str_data_t l;
	make_str_data_t r;
	make_act_t true_acts;
	make_act_t false_acts;
	str_t l_value;
	str_t r_value;
} make_if_data_t;

typedef enum make_act_type_e {
	MAKE_ACT_EMPTY,
	MAKE_ACT_VAR,
	MAKE_ACT_RULE,
	MAKE_ACT_CMD,
	MAKE_ACT_IF,
} make_act_type_t;

typedef struct make_act_data_s {
	make_act_type_t type;
	union {
		make_var_data_t var;
		make_rule_data_t rule;
		make_cmd_data_t cmd;
		make_if_data_t mif;
	};
} make_act_data_t;

static inline make_act_data_t *make_act_get(const make_t *make, make_act_t act)
{
	return list_get_data(&make->acts, act);
}

static inline make_var_data_t *make_var_get(const make_t *make, make_var_t var)
{
	make_act_data_t *act = make_act_get(make, var);
	if (act == NULL || act->type != MAKE_ACT_VAR) {
		return NULL;
	}

	return &act->var;
}

static inline make_rule_data_t *make_rule_get(const make_t *make, make_rule_t rule)
{
	make_act_data_t *act = make_act_get(make, rule);
	if (act == NULL || act->type != MAKE_ACT_RULE) {
		return NULL;
	}

	return &act->rule;
}

static inline make_if_data_t *make_if_get(const make_t *make, make_if_t mif)
{
	make_act_data_t *act = make_act_get(make, mif);
	if (act == NULL || act->type != MAKE_ACT_IF) {
		return NULL;
	}

	return &act->mif;
}

static make_var_t make_var_get_name(const make_t *make, str_t name)
{
	const make_act_data_t *act;
	list_foreach_all(&make->acts, act)
	{
		if (act->type == MAKE_ACT_VAR && act->var.type == MAKE_VAR_INST && str_eq(act->var.name, name)) {
			return _i;
		}
	}

	return MAKE_END;
}

static int make_str_check(const make_t *make, make_str_data_t str)
{
	switch (str.type) {
	case MAKE_STR_STR: return 0;
	case MAKE_STR_VAR: return make_var_get(make, str.var) == NULL ? 1 : 0;
	default: return 1;
	}
}

make_t *make_init(make_t *make, uint acts_cnt, uint strs_cnt)
{
	if (make == NULL) {
		return NULL;
	}

	if (list_init(&make->acts, acts_cnt, sizeof(make_act_data_t)) == NULL) {
		return NULL;
	}

	if (list_init(&make->strs, strs_cnt, sizeof(make_str_data_t)) == NULL) {
		return NULL;
	}

	make->g_acts = MAKE_END;

	return make;
}

static inline void make_var_free(make_var_data_t *var)
{
	str_free(&var->name);
	str_free(&var->ref);
	str_free(&var->value);
}

static inline void make_rule_free(make_rule_data_t *rule)
{
	if (rule->target.type == MAKE_STR_STR) {
		str_free(&rule->target.str);
	}
}

static inline void make_cmd_free(make_cmd_data_t *cmd)
{
	str_free(&cmd->cmd);
}

static inline void make_if_free(make_if_data_t *mif)
{
	if (mif->l.type == MAKE_STR_STR) {
		str_free(&mif->l.str);
	}
	str_free(&mif->l_value);

	if (mif->r.type == MAKE_STR_STR) {
		str_free(&mif->r.str);
	}
	str_free(&mif->r_value);
}

void make_free(make_t *make)
{
	if (make == NULL) {
		return;
	}

	make_act_data_t *act;
	list_foreach_all(&make->acts, act)
	{
		switch (act->type) {
		case MAKE_ACT_VAR: make_var_free(&act->var); break;
		case MAKE_ACT_RULE: make_rule_free(&act->rule); break;
		case MAKE_ACT_CMD: make_cmd_free(&act->cmd); break;
		case MAKE_ACT_IF: make_if_free(&act->mif); break;
		}
	}

	list_free(&make->acts);
	list_free(&make->strs);
	;

	make->g_acts = MAKE_END;
}

make_empty_t make_create_empty(make_t *make)
{
	if (make == NULL) {
		return MAKE_END;
	}

	const make_act_t act  = list_add(&make->acts);
	make_act_data_t *data = make_act_get(make, act);

	if (data == NULL) {
		return MAKE_END;
	}

	*data = (make_act_data_t){
		.type = MAKE_ACT_EMPTY,
	};

	return act;
}

static make_var_t make_create_var_r(make_t *make, str_t name, make_var_type_t type, int ext)
{
	if (make == NULL) {
		return MAKE_END;
	}

	const make_act_t act  = list_add(&make->acts);
	make_act_data_t *data = make_act_get(make, act);

	if (data == NULL) {
		return MAKE_END;
	}

	*data = (make_act_data_t) {
		.type = MAKE_ACT_VAR,
		.var  = {
			.name	 = name,
			.type	 = type,
			.values	 = MAKE_END,
			.ref	 = strf("$(%.*s)", name.len, name.data),
			.value	 = strz(MAX_VAR_VALUE_LEN),
			.ext	 = ext,
			.expanded = 0,
		},
	};

	return act;
}

make_var_t make_create_var(make_t *make, str_t name, make_var_type_t type)
{
	return make_create_var_r(make, name, type, 0);
}

make_var_t make_create_var_ext(make_t *make, str_t name, make_var_type_t type)
{
	return make_create_var_r(make, name, type, 1);
}

make_rule_t make_create_rule(make_t *make, make_str_data_t target)
{
	if (make == NULL || make_str_check(make, target)) {
		return MAKE_END;
	}

	const make_act_t act  = list_add(&make->acts);
	make_act_data_t *data = make_act_get(make, act);

	if (data == NULL) {
		return MAKE_END;
	}

	*data = (make_act_data_t){
		.type = MAKE_ACT_RULE,
		.rule = {
			.target  = target,
			.depends = MAKE_END,
			.acts    = MAKE_END,
		},
	};

	return act;
}

make_cmd_t make_create_cmd(make_t *make, str_t cmd)
{
	if (make == NULL) {
		return MAKE_END;
	}

	const make_act_t act  = list_add(&make->acts);
	make_act_data_t *data = make_act_get(make, act);

	if (data == NULL) {
		return MAKE_END;
	}

	*data = (make_act_data_t){
		.type = MAKE_ACT_CMD,
		.cmd = {
			.cmd  = cmd,
		},
	};

	return act;
}

make_if_t make_create_if(make_t *make, make_str_data_t l, make_str_data_t r)
{
	if (make == NULL || make_str_check(make, l) || make_str_check(make, r)) {
		return MAKE_END;
	}

	const make_act_t act  = list_add(&make->acts);
	make_act_data_t *data = make_act_get(make, act);

	if (data == NULL) {
		return MAKE_END;
	}

	*data = (make_act_data_t){
		.type = MAKE_ACT_IF,
		.mif = {
			.l = l,
			.r = r,
			.true_acts = MAKE_END,
			.false_acts = MAKE_END,
			.l_value = strz(MAX_VAR_VALUE_LEN),
			.r_value = strz(MAX_VAR_VALUE_LEN),
		},
	};

	return act;
}

make_act_t make_add_act(make_t *make, make_act_t act)
{
	if (make == NULL || make_act_get(make, act) == NULL) {
		return MAKE_END;
	}

	return list_set_next_node(&make->acts, make->g_acts, act);
}

make_var_t make_var_add_val(make_t *make, make_var_t var, make_str_data_t val)
{
	if (make == NULL) {
		return MAKE_END;
	}

	make_var_data_t *data = make_var_get(make, var);

	if (data == NULL || data->ext || make_str_check(make, val)) {
		return MAKE_END;
	}

	const make_str_t str	= list_add_next_node(&make->strs, make_var_get(make, var)->values);
	make_str_data_t *target = list_get_data(&make->strs, str);

	if (target == NULL) {
		return MAKE_END;
	}

	*target = val;

	return var;
}

make_str_t make_rule_add_depend(make_t *make, make_rule_t rule, make_str_data_t depend)
{
	if (make == NULL) {
		return MAKE_END;
	}

	make_rule_data_t *data = make_rule_get(make, rule);

	if (data == NULL || make_str_check(make, depend)) {
		return MAKE_END;
	}

	const make_str_t str	= list_add_next_node(&make->strs, make_rule_get(make, rule)->depends);
	make_str_data_t *target = list_get_data(&make->strs, str);

	if (target == NULL) {
		return MAKE_END;
	}

	*target = depend;

	return str;
}

make_act_t make_rule_add_act(make_t *make, make_rule_t rule, make_act_t act)
{
	if (make == NULL) {
		return MAKE_END;
	}

	make_rule_data_t *data = make_rule_get(make, rule);

	if (data == NULL || make_act_get(make, act) == NULL) {
		return MAKE_END;
	}

	return list_set_next_node(&make->acts, data->acts, act);
}

make_act_t make_if_add_true_act(make_t *make, make_if_t mif, make_act_t act)
{
	if (make == NULL) {
		return MAKE_END;
	}

	make_if_data_t *data = make_if_get(make, mif);

	if (data == NULL || make_act_get(make, act) == NULL) {
		return MAKE_END;
	}

	return list_set_next_node(&make->acts, data->true_acts, act);
}

make_act_t make_if_add_false_act(make_t *make, make_if_t mif, make_act_t act)
{
	if (make == NULL) {
		return MAKE_END;
	}

	make_if_data_t *data = make_if_get(make, mif);

	if (data == NULL || make_act_get(make, act) == NULL) {
		return MAKE_END;
	}

	return list_set_next_node(&make->acts, data->false_acts, act);
}

make_str_t make_ext_set_val(make_t *make, str_t name, make_str_data_t val)
{
	if (make == NULL) {
		return MAKE_END;
	}

	const make_act_t act  = make_var_get_name(make, name);
	make_act_data_t *data = make_act_get(make, act);

	if (data == NULL || !data->var.ext || make_str_check(make, val)) {
		return MAKE_END;
	}

	const make_str_t str	= list_add_node(&make->strs, make_var_get(make, act)->values);
	make_str_data_t *target = list_get_data(&make->strs, str);

	if (target == NULL) {
		return MAKE_END;
	}

	*target = val;

	return str;
}

static int replace_refs(make_t *make, str_t *res)
{
	int found = 0;
	const make_act_data_t *act;
	list_foreach_all(&make->acts, act)
	{
		if (act->type != MAKE_ACT_VAR || act->var.type != MAKE_VAR_INST) {
			continue;
		}

		found |= str_replace(res, act->var.ref, act->var.value);
	}
	return found;
}

static str_t make_str(const make_t *make, const make_str_data_t *str)
{
	switch (str->type) {
	case MAKE_STR_STR: return str->str;
	case MAKE_STR_VAR: {
		make_var_data_t *ref = make_var_get(make, str->var);
		if (ref == NULL) {
			return str_null();
		}

		return ref->ref;
	}
	default: return str_null();
	}
}

static int make_var_expand(make_t *make, make_var_data_t *var)
{
	str_zero(&var->value);
	const make_str_data_t *value;
	list_foreach(&make->strs, var->values, value)
	{
		str_cat(&var->value, make_str(make, value));

		if (list_get_next(&make->strs, _i) != LIST_END) {
			str_cat(&var->value, STR(" "));
		}
	}

	replace_refs(make, &var->value);

	switch (var->type) {
	case MAKE_VAR_APP: {
		make_var_data_t *inst = make_var_get(make, make_var_get_name(make, var->name));
		if (inst == NULL) {
			return 1;
		}

		str_cat(&inst->value, STR(" "));
		str_cat(&inst->value, var->value);
		break;
	}
	default: break;
	}

	var->expanded = 1;

	return 0;
}

static int make_acts_expand(make_t *make, make_act_t acts);

static inline int make_if_expand(make_t *make, make_if_data_t *mif)
{
	int ret = 0;

	str_zero(&mif->l_value);
	str_cat(&mif->l_value, make_str(make, &mif->l));

	str_zero(&mif->r_value);
	str_cat(&mif->r_value, make_str(make, &mif->r));

	replace_refs(make, &mif->l_value);
	replace_refs(make, &mif->r_value);

	if (str_eq(mif->l_value, mif->r_value)) {
		ret |= make_acts_expand(make, mif->true_acts);
	} else {
		ret |= make_acts_expand(make, mif->false_acts);
	}

	return ret;
}

static inline int make_expand_reset(make_t *make)
{
	make_act_data_t *act;
	list_foreach_all(&make->acts, act)
	{
		switch (act->type) {
		case MAKE_ACT_VAR: act->var.expanded = 0; break;
		default: break;
		}
	}

	return 0;
}

static inline int make_var_ext_expand(make_t *make)
{
	int ret = 0;
	make_act_data_t *act;
	list_foreach_all(&make->acts, act)
	{
		if (act->type == MAKE_ACT_VAR && act->var.ext) {
			ret |= make_var_expand(make, &act->var);
		}
	}

	return ret;
}

static int make_acts_expand(make_t *make, make_act_t acts)
{
	int ret = 0;
	make_act_data_t *act;
	list_foreach(&make->acts, acts, act)
	{
		switch (act->type) {
		case MAKE_ACT_VAR: ret |= make_var_expand(make, &act->var); break;
		case MAKE_ACT_IF: ret |= make_if_expand(make, &act->mif); break;
		default: break;
		}
	}
	return ret;
}

int make_expand(make_t *make)
{
	if (make == NULL) {
		return 1;
	}

	int ret = 0;

	ret |= make_expand_reset(make);
	ret |= make_var_ext_expand(make);
	ret |= make_acts_expand(make, make->g_acts);

	return ret;
}

str_t make_var_get_expanded(make_t *make, str_t var)
{
	if (make == NULL) {
		return str_null();
	}

	make_act_data_t *act;
	list_foreach_all(&make->acts, act)
	{
		if (act->type == MAKE_ACT_VAR && act->var.expanded && str_eq(act->var.name, var)) {
			return act->var.value;
		}
	}

	return str_null();
}

const char *make_var_type_to_str(make_var_type_t type)
{
	switch (type) {
	case MAKE_VAR_INST: return ":=";
	//case MAKE_VAR_REF: return "=";
	//case MAKE_VAR_COND: return "?=";
	case MAKE_VAR_APP: return "+=";
	default: return NULL;
	}
}

static int make_str_print(const make_t *make, const make_str_data_t *str, FILE *file)
{
	str_t val = make_str(make, str);
	return !c_fprintf(file, "%.*s", val.len, val.data);
}

static inline int make_var_print(const make_t *make, const make_var_data_t *var, FILE *file)
{
	int ret = 0;

	if (var->ext) {
		return ret;
	}

	const char *var_type_str = make_var_type_to_str(var->type);
	if (var_type_str == NULL) {
		return 1;
	}

	ret |= !c_fprintf(file, "%.*s %s", var->name.len, var->name.data, var_type_str);

	const make_str_data_t *value;
	list_foreach(&make->strs, var->values, value)
	{
		ret |= !c_fprintf(file, " ");
		ret |= make_str_print(make, value, file);
	}

	ret |= !c_fprintf(file, "\n");

	return ret;
}

static int make_acts_print(const make_t *make, const make_act_t acts, FILE *file);

static inline int make_rule_print(const make_t *make, const make_rule_data_t *rule, FILE *file)
{
	int ret = 0;
	ret |= make_str_print(make, &rule->target, file);
	ret |= !c_fprintf(file, ":");

	const make_str_data_t *depend;
	list_foreach(&make->strs, rule->depends, depend)
	{
		ret |= !c_fprintf(file, " ");
		ret |= make_str_print(make, depend, file);
	}

	ret |= !c_fprintf(file, "\n");
	ret |= make_acts_print(make, rule->acts, file);
	ret |= !c_fprintf(file, "\n");

	return ret;
}

static inline int make_cmd_print(const make_t *make, const make_cmd_data_t *cmd, FILE *file)
{
	return !c_fprintf(file, "\t%.*s\n", cmd->cmd.len, cmd->cmd.data);
}

static inline int make_if_print(const make_t *make, const make_if_data_t *mif, FILE *file)
{
	int ret = 0;

	ret |= !c_fprintf(file, "ifeq (");
	ret |= make_str_print(make, &mif->l, file);
	ret |= !c_fprintf(file, ", ");
	ret |= make_str_print(make, &mif->r, file);
	ret |= !c_fprintf(file, ")\n");

	ret |= make_acts_print(make, mif->true_acts, file);

	if (mif->false_acts != MAKE_END) {
		ret |= !c_fprintf(file, "else\n");
		ret |= make_acts_print(make, mif->false_acts, file);
	}

	ret |= !c_fprintf(file, "endif\n");

	return ret;
}

static int make_acts_print(const make_t *make, make_act_t acts, FILE *file)
{
	int ret = 0;
	const make_act_data_t *act;
	list_foreach(&make->acts, acts, act)
	{
		switch (act->type) {
		case MAKE_ACT_EMPTY: ret |= !c_fprintf(file, "\n"); break;
		case MAKE_ACT_VAR: ret |= make_var_print(make, &act->var, file); break;
		case MAKE_ACT_RULE: ret |= make_rule_print(make, &act->rule, file); break;
		case MAKE_ACT_CMD: ret |= make_cmd_print(make, &act->cmd, file); break;
		case MAKE_ACT_IF: ret |= make_if_print(make, &act->mif, file); break;
		}
	}

	return ret;
}

int make_print(const make_t *make, FILE *file)
{
	if (make == NULL) {
		return 1;
	}

	return make_acts_print(make, make->g_acts, file);
}

static inline int make_empty_dbg(const make_t *make, FILE *file)
{
	return !c_fprintf(file, "EMPTY\n\n");
}

static inline int make_var_dbg(const make_t *make, const make_var_data_t *var, FILE *file)
{
	int ret = 0;
	ret |= !c_fprintf(file,
			  "VAR\n"
			  "    NAME    : %.*s %s\n"
			  "    VALUES  :\n",
			  var->name.len, var->name.data, var->ext ? "(ext)" : "");
	const make_str_data_t *value;
	list_foreach(&make->strs, var->values, value)
	{
		ret |= !c_fprintf(file, "        ");
		ret |= make_str_print(make, value, file);
		ret |= !c_fprintf(file, "\n");
	}
	ret |= !c_fprintf(file,
			  "    REF     : %.*s\n"
			  "    VALUE   : %.*s\n"
			  "\n",
			  var->ref.len, var->ref.data, var->value.len, var->value.data);

	return ret;
}

static inline int make_if_dbg(const make_t *make, const make_if_data_t *mif, FILE *file)
{
	int ret = 0;
	ret |= !c_fprintf(file, "IF\n"
				"    L: ");
	ret |= make_str_print(make, &mif->l, file);
	ret |= !c_fprintf(file,
			  "\n"
			  "        VALUE   : %.*s\n"
			  "    R: ",
			  mif->l_value.len, mif->l_value.data);

	ret |= make_str_print(make, &mif->r, file);
	ret |= !c_fprintf(file,
			  "\n"
			  "        VALUE   : %.*s\n"
			  "\n",
			  mif->r_value.len, mif->r_value.data);

	return ret;
}

int make_dbg(const make_t *make, FILE *file)
{
	if (make == NULL) {
		return 1;
	}

	int ret = 0;
	const make_act_data_t *act;
	list_foreach_all(&make->acts, act)
	{
		switch (act->type) {
		case MAKE_ACT_EMPTY: ret |= make_empty_dbg(make, file); break;
		case MAKE_ACT_VAR: ret |= make_var_dbg(make, &act->var, file); break;
		case MAKE_ACT_IF: ret |= make_if_dbg(make, &act->mif, file); break;
		default: break;
		}
	}

	return ret;
}
