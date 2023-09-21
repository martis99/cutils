#include "t_make.h"

#include "file.h"
#include "make.h"
#include "mem.h"

#include "test.h"

#define TEST_FILE "t_make.txt"

TEST(t_make_init_free)
{
	START;

	make_t make = { 0 };

	EXPECT_EQ(make_init(NULL, 0, 0), NULL);
	mem_oom(1);
	EXPECT_EQ(make_init(&make, 1, 0), NULL);
	EXPECT_EQ(make_init(&make, 0, 1), NULL);
	mem_oom(0);
	EXPECT_EQ(make_init(&make, 1, 1), &make);

	EXPECT_NE(make.acts.data, NULL);
	EXPECT_NE(make.strs.data, NULL);
	EXPECT_EQ(make.g_acts, MAKE_END);

	make_free(&make);
	make_free(NULL);

	EXPECT_EQ(make.acts.data, NULL);
	EXPECT_EQ(make.strs.data, NULL);
	EXPECT_EQ(make.g_acts, MAKE_END);

	END;
}

TEST(t_make_create_empty)
{
	START;

	make_t make = { 0 };
	make_init(&make, 0, 0);

	EXPECT_EQ(make_create_empty(NULL), MAKE_END);
	mem_oom(1);
	EXPECT_EQ(make_create_empty(&make), MAKE_END);
	mem_oom(0);
	EXPECT_EQ(make_create_empty(&make), 0);

	make_free(&make);
	END;
}

TEST(t_make_create_var)
{
	START;

	make_t make = { 0 };
	make_init(&make, 0, 0);

	EXPECT_EQ(make_create_var(NULL, STR(""), -1), MAKE_END);
	mem_oom(1);
	EXPECT_EQ(make_create_var(&make, STR(""), -1), MAKE_END);
	mem_oom(0);
	EXPECT_EQ(make_create_var(&make, STR(""), -1), 0);
	EXPECT_EQ(make_create_var(&make, STR(""), MAKE_VAR_INST), 1);
	EXPECT_EQ(make_create_var(&make, STR(""), MAKE_VAR_APP), 2);

	make_free(&make);

	END;
}

TEST(t_make_create_var_ext)
{
	START;

	make_t make = { 0 };
	make_init(&make, 0, 0);

	EXPECT_EQ(make_create_var_ext(NULL, STR(""), -1), MAKE_END);
	mem_oom(1);
	EXPECT_EQ(make_create_var(&make, STR(""), -1), MAKE_END);
	mem_oom(0);
	EXPECT_EQ(make_create_var_ext(&make, STR(""), -1), 0);
	EXPECT_EQ(make_create_var_ext(&make, STR(""), MAKE_VAR_INST), 1);
	EXPECT_EQ(make_create_var_ext(&make, STR(""), MAKE_VAR_APP), 2);

	make_free(&make);

	END;
}

TEST(t_make_create_rule)
{
	START;

	make_t make = { 0 };
	make_init(&make, 0, 0);

	EXPECT_EQ(make_create_rule(NULL, MVAR(MAKE_END)), MAKE_END);
	EXPECT_EQ(make_create_rule(&make, (make_str_data_t){ .type = -1, .var = MAKE_END }), MAKE_END);
	EXPECT_EQ(make_create_rule(&make, MVAR(MAKE_END)), MAKE_END);
	mem_oom(1);
	EXPECT_EQ(make_create_rule(&make, MSTR("")), MAKE_END);
	mem_oom(0);
	EXPECT_EQ(make_create_rule(&make, MSTR("")), 0);

	make_free(&make);

	END;
}

TEST(t_make_create_cmd)
{
	START;

	make_t make = { 0 };
	make_init(&make, 0, 0);

	EXPECT_EQ(make_create_cmd(NULL, STR("")), MAKE_END);
	mem_oom(1);
	EXPECT_EQ(make_create_cmd(&make, STR("")), MAKE_END);
	mem_oom(0);
	EXPECT_EQ(make_create_cmd(&make, STR("")), 0);

	make_free(&make);

	END;
}

TEST(t_make_create_if)
{
	START;

	make_t make = { 0 };
	make_init(&make, 0, 0);

	EXPECT_EQ(make_create_if(NULL, MVAR(MAKE_END), MVAR(MAKE_END)), MAKE_END);
	EXPECT_EQ(make_create_if(&make, MVAR(MAKE_END), MVAR(MAKE_END)), MAKE_END);
	mem_oom(1);
	EXPECT_EQ(make_create_if(&make, MSTR(""), MSTR("")), MAKE_END);
	mem_oom(0);
	EXPECT_EQ(make_create_if(&make, MSTR(""), MSTR("")), 0);

	make_free(&make);

	END;
}

TEST(t_make_create)
{
	SSTART;
	RUN(t_make_create_empty);
	RUN(t_make_create_var);
	RUN(t_make_create_var_ext);
	RUN(t_make_create_rule);
	RUN(t_make_create_cmd);
	RUN(t_make_create_if);
	SEND;
}

TEST(t_make_add_act)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	EXPECT_EQ(make_add_act(NULL, MAKE_END), MAKE_END);
	EXPECT_EQ(make_add_act(&make, MAKE_END), MAKE_END);
	EXPECT_EQ(make_add_act(&make, make_create_empty(&make)), 0);

	make_free(&make);

	END;
}

TEST(t_make_var_add_val)
{
	START;

	make_t make = { 0 };
	make_init(&make, 0, 0);

	const make_empty_t empty = make_create_empty(&make);
	const make_rule_t var	 = make_create_var(&make, STR(""), MAKE_VAR_INST);

	EXPECT_EQ(make_var_add_val(NULL, MAKE_END, MVAR(MAKE_END)), MAKE_END);
	EXPECT_EQ(make_var_add_val(&make, MAKE_END, MVAR(MAKE_END)), MAKE_END);
	EXPECT_EQ(make_var_add_val(&make, empty, MVAR(MAKE_END)), MAKE_END);
	EXPECT_EQ(make_var_add_val(&make, make_create_var_ext(&make, STR(""), MAKE_VAR_INST), MVAR(MAKE_END)), MAKE_END);
	EXPECT_EQ(make_var_add_val(&make, var, MVAR(MAKE_END)), MAKE_END);
	mem_oom(1);
	EXPECT_EQ(make_var_add_val(&make, var, MSTR("")), MAKE_END);
	mem_oom(0);
	EXPECT_EQ(make_var_add_val(&make, var, MSTR("")), 1);

	make_free(&make);

	END;
}

TEST(t_make_rule_add_depend)
{
	START;

	make_t make = { 0 };
	make_init(&make, 0, 0);

	const make_empty_t empty = make_create_empty(&make);
	const make_rule_t rule	 = make_create_rule(&make, MSTR(""));

	EXPECT_EQ(make_rule_add_depend(NULL, MAKE_END, MVAR(MAKE_END)), MAKE_END);
	EXPECT_EQ(make_rule_add_depend(&make, MAKE_END, MVAR(MAKE_END)), MAKE_END);
	EXPECT_EQ(make_rule_add_depend(&make, empty, MVAR(MAKE_END)), MAKE_END);
	EXPECT_EQ(make_rule_add_depend(&make, rule, MVAR(MAKE_END)), MAKE_END);
	mem_oom(1);
	EXPECT_EQ(make_rule_add_depend(&make, rule, MSTR("")), MAKE_END);
	mem_oom(0);
	EXPECT_EQ(make_rule_add_depend(&make, rule, MSTR("")), 0);

	make_free(&make);

	END;
}

TEST(t_make_rule_add_act)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	const make_empty_t empty = make_create_empty(&make);
	const make_rule_t rule	 = make_create_rule(&make, MSTR(""));

	EXPECT_EQ(make_rule_add_act(NULL, MAKE_END, MAKE_END), MAKE_END);
	EXPECT_EQ(make_rule_add_act(&make, MAKE_END, MAKE_END), MAKE_END);
	EXPECT_EQ(make_rule_add_act(&make, empty, MAKE_END), MAKE_END);
	EXPECT_EQ(make_rule_add_act(&make, rule, MAKE_END), MAKE_END);
	EXPECT_EQ(make_rule_add_act(&make, rule, empty), 0);

	make_free(&make);

	END;
}

TEST(t_make_if_add_true_act)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	const make_empty_t empty = make_create_empty(&make);
	const make_if_t mif	 = make_create_if(&make, MSTR(""), MSTR(""));

	EXPECT_EQ(make_if_add_true_act(NULL, MAKE_END, MAKE_END), MAKE_END);
	EXPECT_EQ(make_if_add_true_act(&make, MAKE_END, MAKE_END), MAKE_END);
	EXPECT_EQ(make_if_add_true_act(&make, empty, MAKE_END), MAKE_END);
	EXPECT_EQ(make_if_add_true_act(&make, mif, MAKE_END), MAKE_END);
	EXPECT_EQ(make_if_add_true_act(&make, mif, empty), 0);

	make_free(&make);

	END;
}

TEST(t_make_if_add_false_act)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	const make_empty_t empty = make_create_empty(&make);
	const make_if_t mif	 = make_create_if(&make, MSTR(""), MSTR(""));

	EXPECT_EQ(make_if_add_false_act(NULL, MAKE_END, MAKE_END), MAKE_END);
	EXPECT_EQ(make_if_add_false_act(&make, MAKE_END, MAKE_END), MAKE_END);
	EXPECT_EQ(make_if_add_false_act(&make, empty, MAKE_END), MAKE_END);
	EXPECT_EQ(make_if_add_false_act(&make, mif, MAKE_END), MAKE_END);
	EXPECT_EQ(make_if_add_false_act(&make, mif, empty), 0);

	make_free(&make);

	END;
}

TEST(t_make_add)
{
	SSTART;
	RUN(t_make_add_act);
	RUN(t_make_var_add_val);
	RUN(t_make_rule_add_depend);
	RUN(t_make_rule_add_act);
	RUN(t_make_if_add_true_act);
	RUN(t_make_if_add_false_act);
	SEND;
}

TEST(t_make_ext_set_val)
{
	START;

	make_t make = { 0 };
	make_init(&make, 0, 0);

	make_create_var_ext(&make, STR("EXT"), MAKE_VAR_INST);

	EXPECT_EQ(make_ext_set_val(NULL, STR(""), MVAR(MAKE_END)), MAKE_END);
	EXPECT_EQ(make_ext_set_val(&make, STR(""), MVAR(MAKE_END)), MAKE_END);
	EXPECT_EQ(make_ext_set_val(&make, STR("EXT"), MVAR(MAKE_END)), MAKE_END);
	mem_oom(1);
	EXPECT_EQ(make_ext_set_val(&make, STR("EXT"), MSTR("")), MAKE_END);
	mem_oom(0);
	EXPECT_EQ(make_ext_set_val(&make, STR("EXT"), MSTR("")), 0);

	make_free(&make);

	END;
}

TEST(t_make_expand)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	EXPECT_EQ(make_expand(NULL), 1);
	EXPECT_EQ(make_expand(&make), 0);

	make_free(&make);

	END;
}

TEST(t_make_var_get_expanded)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	EXPECT_EQ(make_var_get_expanded(NULL, STR("")).data, NULL);
	EXPECT_EQ(make_var_get_expanded(&make, STR("")).data, NULL);

	make_free(&make);

	END;
}

TEST(t_make_print, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	EXPECT_EQ(make_print(NULL, NULL), 1);
	EXPECT_EQ(make_print(&make, NULL), 0);

	make_add_act(&make, make_create_empty(&make));

	EXPECT_EQ(make_print(&make, NULL), 1);
	EXPECT_EQ(make_print(&make, file), 0);

	make_add_act(&make, make_create_var(&make, STR(""), -1));
	EXPECT_EQ(make_print(&make, file), 1);

	make_free(&make);

	END;
}

TEST(t_make_dbg, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	EXPECT_EQ(make_dbg(NULL, NULL), 1);
	EXPECT_EQ(make_dbg(&make, NULL), 0);

	make_add_act(&make, make_create_empty(&make));

	EXPECT_EQ(make_dbg(&make, NULL), 1);
	EXPECT_EQ(make_dbg(&make, file), 0);

	make_free(&make);

	END;
}

TEST(t_make_expand_print_empty, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	make_add_act(&make, make_create_empty(&make));

	EXPECT_EQ(make_expand(&make), 0);

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_print(&make, file), 0);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "\n";
		EXPECT_STR(buf, exp);
	}
	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_dbg(&make, file), 0);
	}

	make_free(&make);

	END;
}

TEST(t_make_expand_print_var_inst_empty, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	make_add_act(&make, make_create_var(&make, STR("VAR"), MAKE_VAR_INST));

	{
		EXPECT_EQ(make_expand(&make), 0);

		str_t var_exp = make_var_get_expanded(&make, STR("VAR"));
		EXPECT_STRN(var_exp.data, "", var_exp.len);
	}
	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_print(&make, file), 0);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "VAR :=\n";
		EXPECT_STR(buf, exp);
	}
	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_dbg(&make, file), 0);
	}

	make_free(&make);

	END;
}

TEST(t_make_expand_print_var_inst, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	make_var_add_val(&make, make_add_act(&make, make_create_var(&make, STR("VAR"), MAKE_VAR_INST)), MSTR("VAL"));

	{
		EXPECT_EQ(make_expand(&make), 0);

		str_t var_exp = make_var_get_expanded(&make, STR("VAR"));
		EXPECT_STRN(var_exp.data, "VAL", var_exp.len);
	}
	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_print(&make, file), 0);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "VAR := VAL\n";
		EXPECT_STR(buf, exp);
	}

	make_free(&make);

	END;
}

TEST(t_make_expand_print_var_inst2, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	make_var_t var = make_add_act(&make, make_create_var(&make, STR("VAR"), MAKE_VAR_INST));
	make_var_add_val(&make, var, MSTR("VAL1"));
	make_var_add_val(&make, var, MSTR("VAL2"));

	{
		EXPECT_EQ(make_expand(&make), 0);

		str_t var_exp = make_var_get_expanded(&make, STR("VAR"));
		EXPECT_STRN(var_exp.data, "VAL1 VAL2", var_exp.len);
	}
	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_print(&make, file), 0);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "VAR := VAL1 VAL2\n";
		EXPECT_STR(buf, exp);
	}

	make_free(&make);

	END;
}

TEST(t_make_expand_print_var_app, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	make_var_add_val(&make, make_add_act(&make, make_create_var(&make, STR("VAR"), MAKE_VAR_INST)), MSTR("VAL1"));
	make_var_add_val(&make, make_add_act(&make, make_create_var(&make, STR("VAR"), MAKE_VAR_APP)), MSTR("VAL2"));

	{
		EXPECT_EQ(make_expand(&make), 0);

		str_t var_exp = make_var_get_expanded(&make, STR("VAR"));
		EXPECT_STRN(var_exp.data, "VAL1 VAL2", var_exp.len);
	}
	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_print(&make, file), 0);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "VAR := VAL1\n"
				   "VAR += VAL2\n";
		EXPECT_STR(buf, exp);
	}
	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_dbg(&make, file), 0);
	}

	make_free(&make);

	END;
}

TEST(t_make_expand_print_var_ext_inst, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	make_var_add_val(&make, make_add_act(&make, make_create_var_ext(&make, STR("VAR"), MAKE_VAR_INST)), MSTR("VAL"));

	{
		EXPECT_EQ(make_expand(&make), 0);

		str_t var_exp = make_var_get_expanded(&make, STR("VAR"));
		EXPECT_STRN(var_exp.data, "VAL", var_exp.len);
	}
	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_print(&make, file), 0);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "";
		EXPECT_STR(buf, exp);
	}

	make_free(&make);

	END;
}

TEST(t_make_expand_print_if_empty, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	make_add_act(&make, make_create_if(&make, MSTR("L"), MSTR("R")));

	EXPECT_EQ(make_expand(&make), 0);

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_print(&make, file), 0);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "ifeq (L, R)\n"
				   "endif\n";
		EXPECT_STR(buf, exp);
	}
	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_dbg(&make, file), 0);
	}

	make_free(&make);

	END;
}

TEST(t_make_expand_print_var_if_true, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	const make_var_t cond	= make_create_var_ext(&make, STR("COND"), MAKE_VAR_INST);
	const make_if_t if_cond = make_add_act(&make, make_create_if(&make, MVAR(cond), MSTR("A")));
	make_if_add_true_act(&make, if_cond, make_var_add_val(&make, make_create_var(&make, STR("VAR"), MAKE_VAR_INST), MSTR("VAL")));

	{
		make_ext_set_val(&make, STR("COND"), MSTR("A"));

		EXPECT_EQ(make_expand(&make), 0);

		str_t var_exp = make_var_get_expanded(&make, STR("VAR"));
		EXPECT_STRN(var_exp.data, "VAL", var_exp.len);
	}
	{
		make_ext_set_val(&make, STR("COND"), MSTR("B"));

		EXPECT_EQ(make_expand(&make), 0);

		str_t var_exp = make_var_get_expanded(&make, STR("VAR"));
		EXPECT_STRN(var_exp.data, NULL, var_exp.len);
	}
	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_print(&make, file), 0);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "ifeq ($(COND), A)\n"
				   "VAR := VAL\n"
				   "endif\n";
		EXPECT_STR(buf, exp);
	}
	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_dbg(&make, file), 0);
	}

	make_free(&make);

	END;
}

TEST(t_make_expand_print_var_if_false, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	const make_var_t cond	= make_create_var_ext(&make, STR("COND"), MAKE_VAR_INST);
	const make_if_t if_cond = make_add_act(&make, make_create_if(&make, MVAR(cond), MSTR("A")));
	make_if_add_true_act(&make, if_cond, make_var_add_val(&make, make_create_var(&make, STR("VAR"), MAKE_VAR_INST), MSTR("VAL1")));
	make_if_add_false_act(&make, if_cond, make_var_add_val(&make, make_create_var(&make, STR("VAR"), MAKE_VAR_INST), MSTR("VAL2")));

	{
		make_ext_set_val(&make, STR("COND"), MSTR("A"));

		EXPECT_EQ(make_expand(&make), 0);

		str_t var_exp = make_var_get_expanded(&make, STR("VAR"));
		EXPECT_STRN(var_exp.data, "VAL1", var_exp.len);
	}
	{
		make_ext_set_val(&make, STR("COND"), MSTR("B"));

		EXPECT_EQ(make_expand(&make), 0);

		str_t var_exp = make_var_get_expanded(&make, STR("VAR"));
		EXPECT_STRN(var_exp.data, "VAL2", var_exp.len);
	}
	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_print(&make, file), 0);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "ifeq ($(COND), A)\n"
				   "VAR := VAL1\n"
				   "else\n"
				   "VAR := VAL2\n"
				   "endif\n";
		EXPECT_STR(buf, exp);
	}
	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_dbg(&make, file), 0);
	}

	make_free(&make);

	END;
}

TEST(t_make_print_rule_empty, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	make_add_act(&make, make_create_rule(&make, MSTR("rule")));

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_print(&make, file), 0);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "rule:\n"
				   "\n";
		EXPECT_STR(buf, exp);
	}
	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_dbg(&make, file), 0);
	}

	make_free(&make);

	END;
}

TEST(t_make_print_rule_depend, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	make_rule_t rule = make_add_act(&make, make_create_rule(&make, MSTR("rule")));
	make_rule_add_depend(&make, rule, MSTR("depend"));

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_print(&make, file), 0);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "rule: depend\n"
				   "\n";
		EXPECT_STR(buf, exp);
	}
	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_dbg(&make, file), 0);
	}

	make_free(&make);

	END;
}

TEST(t_make_print_rule_depends, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	make_rule_t rule = make_add_act(&make, make_create_rule(&make, MSTR("rule")));
	make_rule_add_depend(&make, rule, MSTR("depend1"));
	make_rule_add_depend(&make, rule, MSTR("depend2"));

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_print(&make, file), 0);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "rule: depend1 depend2\n"
				   "\n";
		EXPECT_STR(buf, exp);
	}
	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_dbg(&make, file), 0);
	}

	make_free(&make);

	END;
}

TEST(t_make_print_rule_acts, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 1, 1);

	make_rule_t rule = make_add_act(&make, make_create_rule(&make, MSTR("rule")));
	make_rule_add_act(&make, rule, make_create_cmd(&make, STR("cmd1")));
	make_rule_add_act(&make, rule, make_create_cmd(&make, STR("cmd2")));

	make_rule_t if_rule = make_rule_add_act(&make, rule, make_create_if(&make, MSTR("L"), MSTR("R")));
	make_if_add_true_act(&make, if_rule, make_create_cmd(&make, STR("cmd3")));
	make_if_add_false_act(&make, if_rule, make_create_cmd(&make, STR("cmd4")));

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(make_print(&make, file), 0);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "rule:\n"
				   "\tcmd1\n"
				   "\tcmd2\n"
				   "ifeq (L, R)\n"
				   "\tcmd3\n"
				   "else\n"
				   "\tcmd4\n"
				   "endif\n"
				   "\n";
		EXPECT_STR(buf, exp);
	}

	make_free(&make);

	END;
}

TEST(t_make_expand_print, FILE *file)
{
	SSTART;
	RUN(t_make_ext_set_val);
	RUN(t_make_expand);
	RUN(t_make_var_get_expanded);
	RUN(t_make_print, file);
	RUN(t_make_dbg, file);
	RUN(t_make_expand_print_empty, file);
	RUN(t_make_expand_print_var_inst_empty, file);
	RUN(t_make_expand_print_var_inst, file);
	RUN(t_make_expand_print_var_inst2, file);
	RUN(t_make_expand_print_var_app, file);
	RUN(t_make_expand_print_var_ext_inst, file);
	RUN(t_make_expand_print_if_empty, file);
	RUN(t_make_expand_print_var_if_true, file);
	RUN(t_make_expand_print_var_if_false, file);
	RUN(t_make_print_rule_empty, file);
	RUN(t_make_print_rule_depend, file);
	RUN(t_make_print_rule_depends, file);
	RUN(t_make_print_rule_acts, file);
	SEND;
}

STEST(t_make)
{
	SSTART;

	FILE *file = file_open(TEST_FILE, "wb+");

	RUN(t_make_init_free);
	RUN(t_make_create);
	RUN(t_make_add);
	RUN(t_make_expand_print, file);

	file_close(file);
	file_delete(TEST_FILE);

	SEND;
}
