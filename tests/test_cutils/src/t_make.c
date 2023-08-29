#include "t_make.h"

#include "file.h"
#include "make.h"

#include "test.h"

#define TEST_FILE "Makefile.m"

TEST(t_init_free)
{
	START;

	make_t make = { 0 };

	EXPECT_EQ(make_init(NULL, 8, 8), NULL);
	EXPECT_NE(make_init(&make, 8, 8), NULL);

	EXPECT_NE(make.acts.data, NULL);
	EXPECT_NE(make.strs.data, NULL);
	EXPECT_EQ(make.g_acts, LIST_END);

	make_free(&make);
	make_free(NULL);

	EXPECT_EQ(make.acts.data, NULL);
	EXPECT_EQ(make.strs.data, NULL);
	EXPECT_EQ(make.g_acts, LIST_END);

	END;
}

TEST(t_make_add_act)
{
	START;

	EXPECT_EQ(make_add_act(NULL, LIST_END), LIST_END);

	END;
}

TEST(t_make_create_var, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 8, 8);

	EXPECT_EQ(make_create_var(NULL, STR("TARGET"), MAKE_VAR_INST), LIST_END);
	make_var_t target = make_add_act(&make, make_create_var(&make, STR("TARGET"), MAKE_VAR_INST));
	EXPECT_EQ(make_var_add_val(NULL, target, MSTR("test")), LIST_END);
	make_var_add_val(&make, target, MSTR("test"));

	file_reopen(TEST_FILE, "wb+", file);
	EXPECT_EQ(make_print(&make, file), 0);

	char buf[64] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "TARGET := test\n";

	EXPECT_STR(buf, exp);

	make_free(&make);

	END;
}

TEST(t_make_create_rule, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 8, 8);

	EXPECT_EQ(make_create_rule(NULL, MSTR("target")), LIST_END);
	make_var_t compile = make_add_act(&make, make_create_rule(&make, MSTR("target")));

	file_reopen(TEST_FILE, "wb+", file);
	EXPECT_EQ(make_print(&make, file), 0);

	char buf[64] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "target:\n"
			   "\n"
			   "\n";

	EXPECT_STR(buf, exp);

	make_free(&make);

	END;
}

TEST(t_make_create_cmd, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 8, 8);

	make_var_t compile = make_add_act(&make, make_create_rule(&make, MSTR("target")));

	EXPECT_EQ(make_create_cmd(NULL, STR("prepare")), LIST_END);
	make_rule_add_act(&make, compile, make_create_cmd(&make, STR("prepare")));

	file_reopen(TEST_FILE, "wb+", file);
	EXPECT_EQ(make_print(&make, file), 0);

	char buf[64] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "target:\n"
			   "\tprepare\n"
			   "\n"
			   "\n";

	EXPECT_STR(buf, exp);

	make_free(&make);

	END;
}

TEST(t_make_create_if, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 8, 8);

	make_var_t platform = make_add_act(&make, make_create_var_ext(&make, STR("PLATFORM"), MAKE_VAR_INST));

	EXPECT_EQ(make_create_if(NULL, MVAR(platform), MSTR("x64")), LIST_END);
	make_if_t if_bits = make_add_act(&make, make_create_if(&make, MVAR(platform), MSTR("x64")));

	EXPECT_EQ(make_if_add_true_act(NULL, if_bits, LIST_END), LIST_END);
	make_var_t bits64 = make_if_add_true_act(&make, if_bits, make_create_var(&make, STR("BITS"), MAKE_VAR_INST));
	make_var_add_val(&make, bits64, MSTR("64"));

	file_reopen(TEST_FILE, "wb+", file);
	EXPECT_EQ(make_print(&make, file), 0);

	char buf[64] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "ifeq ($(PLATFORM), x64)\n"
			   "BITS := 64\n"
			   "endif\n";

	EXPECT_STR(buf, exp);

	make_free(&make);

	END;
}

TEST(t_create, FILE *file)
{
	SSTART;
	RUN(t_make_add_act);
	RUN(t_make_create_var, file);
	RUN(t_make_create_rule, file);
	RUN(t_make_create_cmd, file);
	RUN(t_make_create_if, file);
	SEND;
}

TEST(t_make_rule_add_depend, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 8, 8);

	make_var_t compile = make_add_act(&make, make_create_rule(&make, MSTR("target")));
	EXPECT_EQ(make_rule_add_depend(NULL, compile, MSTR("depend")), LIST_END);
	make_rule_add_depend(&make, compile, MSTR("depend"));

	file_reopen(TEST_FILE, "wb+", file);
	EXPECT_EQ(make_print(&make, file), 0);

	char buf[64] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "target: depend\n"
			   "\n"
			   "\n";

	EXPECT_STR(buf, exp);

	make_free(&make);

	END;
}

TEST(t_make_if_add_false_act, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 8, 8);

	make_var_t platform = make_add_act(&make, make_create_var_ext(&make, STR("PLATFORM"), MAKE_VAR_INST));

	make_if_t if_bits = make_add_act(&make, make_create_if(&make, MVAR(platform), MSTR("x64")));

	make_var_t bits64 = make_if_add_true_act(&make, if_bits, make_create_var(&make, STR("BITS"), MAKE_VAR_INST));
	make_var_add_val(&make, bits64, MSTR("64"));

	EXPECT_EQ(make_if_add_false_act(NULL, if_bits, LIST_END), LIST_END);
	make_var_t bits32 = make_if_add_false_act(&make, if_bits, make_create_var(&make, STR("BITS"), MAKE_VAR_INST));
	make_var_add_val(&make, bits32, MSTR("32"));

	file_reopen(TEST_FILE, "wb+", file);
	EXPECT_EQ(make_print(&make, file), 0);

	char buf[128] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "ifeq ($(PLATFORM), x64)\n"
			   "BITS := 64\n"
			   "else\n"
			   "BITS := 32\n"
			   "endif\n";

	EXPECT_STR(buf, exp);

	make_free(&make);

	END;
}

TEST(t_add, FILE *file)
{
	SSTART;
	RUN(t_make_rule_add_depend, file);
	RUN(t_make_if_add_false_act, file);
	SEND;
}

TEST(t_make_expand_var_var, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 8, 8);

	make_var_t slndir = make_add_act(&make, make_create_var(&make, STR("SLNDIR"), MAKE_VAR_INST));
	make_var_add_val(&make, slndir, MSTR("./"));

	make_var_t outdir = make_add_act(&make, make_create_var(&make, STR("OUTDIR"), MAKE_VAR_INST));
	make_var_add_val(&make, outdir, MVAR(slndir));

	EXPECT_EQ(make_expand(NULL), 1);
	EXPECT_EQ(make_expand(&make), 0);

	make_get_expanded(NULL, STR("OUTDIR"));
	str_t outdir_path = make_get_expanded(&make, STR("OUTDIR"));

	EXPECT_STRN(outdir_path.data, "./", outdir_path.len);

	file_reopen(TEST_FILE, "wb+", file);
	EXPECT_EQ(make_print(&make, file), 0);

	char buf[512] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "SLNDIR := ./\n"
			   "OUTDIR := $(SLNDIR)\n";

	EXPECT_STR(buf, exp);

	make_free(&make);

	END;
}

TEST(t_make_expand_var_ext, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 8, 8);

	make_var_t slndir = make_add_act(&make, make_create_var_ext(&make, STR("SLNDIR"), MAKE_VAR_INST));

	make_var_t outdir = make_add_act(&make, make_create_var(&make, STR("OUTDIR"), MAKE_VAR_INST));
	make_var_add_val(&make, outdir, MSTR("$(SLNDIR)"));

	make_set_ext(&make, STR("SLNDIR"), MSTR("./"));

	EXPECT_EQ(make_expand(&make), 0);

	str_t outdir_path = make_get_expanded(&make, STR("OUTDIR"));

	EXPECT_STRN(outdir_path.data, "./", outdir_path.len);

	file_reopen(TEST_FILE, "wb+", file);
	EXPECT_EQ(make_print(&make, file), 0);

	char buf[512] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "OUTDIR := $(SLNDIR)\n";

	EXPECT_STR(buf, exp);

	make_free(&make);

	END;
}

TEST(t_make_expand_var_app, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 8, 8);

	make_var_t slndir = make_add_act(&make, make_create_var(&make, STR("SLNDIR"), MAKE_VAR_INST));
	make_var_add_val(&make, slndir, MSTR("./"));
	make_var_t slndir_a = make_add_act(&make, make_create_var(&make, STR("SLNDIR"), MAKE_VAR_APP));
	make_var_add_val(&make, slndir_a, MSTR("./"));

	make_var_t outdir = make_add_act(&make, make_create_var(&make, STR("OUTDIR"), MAKE_VAR_INST));
	make_var_add_val(&make, outdir, MVAR(slndir));
	make_var_t outdir_a = make_add_act(&make, make_create_var(&make, STR("OUTDIR"), MAKE_VAR_APP));
	make_var_add_val(&make, outdir_a, MVAR(slndir));

	EXPECT_EQ(make_expand(&make), 0);

	str_t outdir_path = make_get_expanded(&make, STR("OUTDIR"));

	EXPECT_STRN(outdir_path.data, "./ ./ ./ ./", outdir_path.len);

	file_reopen(TEST_FILE, "wb+", file);
	EXPECT_EQ(make_print(&make, file), 0);

	char buf[512] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "SLNDIR := ./\n"
			   "SLNDIR += ./\n"
			   "OUTDIR := $(SLNDIR)\n"
			   "OUTDIR += $(SLNDIR)\n";

	EXPECT_STR(buf, exp);

	make_free(&make);

	END;
}

TEST(t_make_expand_if, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 8, 8);

	make_var_t platform = make_add_act(&make, make_create_var_ext(&make, STR("PLATFORM"), MAKE_VAR_INST));

	make_if_t if_bits = make_add_act(&make, make_create_if(&make, MVAR(platform), MSTR("x64")));

	make_var_t bits64 = make_if_add_true_act(&make, if_bits, make_create_var(&make, STR("BITS"), MAKE_VAR_INST));
	make_var_add_val(&make, bits64, MSTR("64"));

	make_var_t bits32 = make_if_add_false_act(&make, if_bits, make_create_var(&make, STR("BITS"), MAKE_VAR_INST));
	make_var_add_val(&make, bits32, MSTR("32"));

	make_set_ext(&make, STR("PLATFORM"), MSTR("x64"));

	EXPECT_EQ(make_expand(&make), 0);

	str_t bits_exp = make_get_expanded(&make, STR("BITS"));

	EXPECT_STRN(bits_exp.data, "64", bits_exp.len);

	make_set_ext(&make, STR("PLATFORM"), MSTR("x32"));

	EXPECT_EQ(make_expand(&make), 0);

	bits_exp = make_get_expanded(&make, STR("BITS"));

	EXPECT_STRN(bits_exp.data, "32", bits_exp.len);

	file_reopen(TEST_FILE, "wb+", file);
	EXPECT_EQ(make_print(&make, file), 0);

	char buf[512] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "ifeq ($(PLATFORM), x64)\n"
			   "BITS := 64\n"
			   "else\n"
			   "BITS := 32\n"
			   "endif\n";

	EXPECT_STR(buf, exp);

	make_free(&make);

	END;
}

TEST(t_make_expand, FILE *file)
{
	START;

	make_t make = { 0 };
	make_init(&make, 8, 8);

	make_var_t slndir   = make_add_act(&make, make_create_var_ext(&make, STR("SLNDIR"), MAKE_VAR_INST));
	make_var_t config   = make_add_act(&make, make_create_var_ext(&make, STR("CONFIG"), MAKE_VAR_INST));
	make_var_t platform = make_add_act(&make, make_create_var_ext(&make, STR("PLATFORM"), MAKE_VAR_INST));
	make_var_t tool	    = make_add_act(&make, make_create_var_ext(&make, STR("TOOL"), MAKE_VAR_INST));

	make_var_t outdir = make_add_act(&make, make_create_var(&make, STR("OUTDIR"), MAKE_VAR_INST));
	make_var_add_val(&make, outdir, MSTR("$(SLNDIR)bin/$(CONFIG)-$(PLATFORM)/test/"));

	make_if_t if_bits = make_add_act(&make, make_create_if(&make, MVAR(platform), MSTR("x64")));

	make_var_t bits64 = make_if_add_true_act(&make, if_bits, make_create_var(&make, STR("BITS"), MAKE_VAR_INST));
	make_var_add_val(&make, bits64, MSTR("64"));

	make_var_t bits32 = make_if_add_false_act(&make, if_bits, make_create_var(&make, STR("BITS"), MAKE_VAR_INST));
	make_var_add_val(&make, bits32, MSTR("32"));

	make_var_t target = make_add_act(&make, make_create_var(&make, STR("TARGET"), MAKE_VAR_INST));
	make_var_add_val(&make, target, MSTR("$(OUTDIR)test$(BITS)"));

	make_rule_t compile = make_add_act(&make, make_create_rule(&make, MVAR(target)));
	make_rule_add_depend(&make, compile, MSTR("depend"));

	make_rule_add_act(&make, compile, make_create_cmd(&make, STR("prepare")));

	make_rule_t if_gcc = make_rule_add_act(&make, compile, make_create_if(&make, MVAR(tool), MSTR("gcc")));

	make_var_t tool_gcc = make_if_add_true_act(&make, if_gcc, make_create_cmd(&make, STR("gcc")));

	make_var_t tool_clang = make_if_add_false_act(&make, if_gcc, make_create_cmd(&make, STR("clang")));

	make_rule_add_act(&make, compile, make_create_cmd(&make, STR("cleanup")));

	make_set_ext(&make, STR("SLNDIR"), MSTR("./"));
	make_set_ext(&make, STR("CONFIG"), MSTR("Debug"));
	make_set_ext(&make, STR("PLATFORM"), MSTR("x64"));
	make_set_ext(&make, STR("TOOL"), MSTR("cland"));

	EXPECT_EQ(make_expand(&make), 0);

	str_t target_path = make_get_expanded(&make, STR("TARGET"));

	EXPECT_STRN(target_path.data, "./bin/Debug-x64/test/test64", target_path.len);

	file_reopen(TEST_FILE, "wb+", file);
	EXPECT_EQ(make_print(&make, file), 0);

	char buf[512] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "OUTDIR := $(SLNDIR)bin/$(CONFIG)-$(PLATFORM)/test/\n"
			   "ifeq ($(PLATFORM), x64)\n"
			   "BITS := 64\n"
			   "else\n"
			   "BITS := 32\n"
			   "endif\n"
			   "TARGET := $(OUTDIR)test$(BITS)\n"
			   "$(TARGET): depend\n"
			   "\tprepare\n"
			   "ifeq ($(TOOL), gcc)\n"
			   "\tgcc\n"
			   "else\n"
			   "\tclang\n"
			   "endif\n"
			   "\tcleanup\n"
			   "\n"
			   "\n";

	EXPECT_STR(buf, exp);

	file_reopen(TEST_FILE, "wb+", file);
	EXPECT_EQ(make_dbg(&make, file), 0);

	make_free(&make);

	END;
}

TEST(t_expand, FILE *file)
{
	SSTART;
	RUN(t_make_expand_var_var, file);
	RUN(t_make_expand_var_ext, file);
	RUN(t_make_expand_var_app, file);
	RUN(t_make_expand_if, file);
	RUN(t_make_expand, file);
	SEND;
}

STEST(t_make)
{
	SSTART;

	FILE *file = file_open(TEST_FILE, "wb+");

	RUN(t_init_free);
	RUN(t_create, file);
	RUN(t_add, file);
	RUN(t_expand, file);

	file_close(file);
	file_delete(TEST_FILE);

	SEND;
}
