#include "cstr.h"
#include "make.h"
#include "mem.h"
#include "print.h"

int main(int argc, char **argv)
{
	mem_stats_t mem_stats = { 0 };
	mem_init(&mem_stats);

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

	make_print(&make, stdout);

	make_ext_set_val(&make, STR("SLNDIR"), MSTR("./"));
	make_ext_set_val(&make, STR("CONFIG"), MSTR("Debug"));
	make_ext_set_val(&make, STR("PLATFORM"), MSTR("x64"));
	make_ext_set_val(&make, STR("TOOL"), MSTR("cland"));

	make_expand(&make);

	str_t target_path = make_var_get_expanded(&make, STR("TARGET"));

	printf("TARGET: %.*s\n\n", (int)target_path.len, target_path.data);

	make_free(&make);

	mem_print(stdout);

	return 0;
}
