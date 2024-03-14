#include "arr.h"
#include "cutils.h"
#include "json.h"
#include "list.h"
#include "log.h"
#include "make.h"
#include "print.h"
#include "tree.h"
#include "xml.h"

#define SEP "\n------%s------\n\n"

static int print_arr(FILE *file, void *data, int ret)
{
	c_fprintf(file, "%d\n", *(int *)data);
	return ret;
}

static void example_arr()
{
	c_printf(SEP, __func__);

	arr_t arr = { 0 };

	arr_init(&arr, 4, sizeof(int));

	*(int *)arr_get(&arr, arr_add(&arr)) = 0;
	*(int *)arr_get(&arr, arr_add(&arr)) = 1;
	*(int *)arr_get(&arr, arr_add(&arr)) = 2;
	*(int *)arr_get(&arr, arr_add(&arr)) = 3;

	arr_print(&arr, stdout, print_arr, 0);

	arr_free(&arr);
}

static void example_json()
{
	c_printf(SEP, __func__);

	json_t json = { 0 };
	json_init(&json, 1);

	const json_val_t root = json_add_val(&json, JSON_END, str_null(), JSON_OBJ());
	json_add_val(&json, root, STRH("int"), JSON_INT(1));
	json_add_val(&json, root, STRH("float"), JSON_FLOAT(2.1234567f));
	json_add_val(&json, root, STRH("double"), JSON_DOUBLE(3.123456789012345));
	json_add_val(&json, root, STRH("bool"), JSON_BOOL(1));
	json_add_val(&json, root, STRH("str"), JSON_STR(STRH("str")));
	const json_val_t arr = json_add_val(&json, root, STRH("arr"), JSON_ARR());
	json_add_val(&json, arr, str_null(), JSON_INT(0));
	json_add_val(&json, arr, str_null(), JSON_INT(1));
	json_add_val(&json, arr, str_null(), JSON_INT(2));
	const json_val_t obj = json_add_val(&json, root, STRH("obj"), JSON_OBJ());
	json_add_val(&json, obj, STRH("int"), JSON_INT(4));

	json_print(&json, root, "\t", stdout);

	json_free(&json);
}

static int print_list(FILE *file, void *data, int ret)
{
	c_fprintf(file, "%d\n", *(int *)data);
	return ret;
}

static void example_list()
{
	c_printf(SEP, __func__);

	list_t list = { 0 };

	list_init(&list, 13, sizeof(int));

	*(int *)list_get_data(&list, list_add(&list)) = 0;

	lnode_t n1, n2, n3, n11, n12, n13, n21, n22, n23, n31, n32, n33;

	*(int *)list_get_data(&list, (n1 = list_add(&list))) = 1;
	*(int *)list_get_data(&list, (n2 = list_add(&list))) = 2;
	*(int *)list_get_data(&list, (n3 = list_add(&list))) = 3;

	*(int *)list_get_data(&list, (n11 = list_add_next(&list, n1))) = 11;
	*(int *)list_get_data(&list, (n12 = list_add_next(&list, n1))) = 12;
	*(int *)list_get_data(&list, (n13 = list_add_next(&list, n1))) = 13;

	*(int *)list_get_data(&list, (n21 = list_add_next(&list, n2))) = 21;
	*(int *)list_get_data(&list, (n22 = list_add_next(&list, n2))) = 22;
	*(int *)list_get_data(&list, (n23 = list_add_next(&list, n2))) = 23;

	*(int *)list_get_data(&list, (n31 = list_add_next(&list, n3))) = 31;
	*(int *)list_get_data(&list, (n32 = list_add_next(&list, n3))) = 32;
	*(int *)list_get_data(&list, (n33 = list_add_next(&list, n3))) = 33;

	list_print(&list, n1, stdout, print_list, 0);
	list_print(&list, n2, stdout, print_list, 0);
	list_print(&list, n3, stdout, print_list, 0);

	list_free(&list);
}

static void example_log()
{
	c_printf(SEP, __func__);

	log_trace("example", "cutils", NULL, "trace");
	log_debug("example", "cutils", NULL, "debug");
	log_info("example", "cutils", NULL, "info");
	log_warn("example", "cutils", NULL, "warn");
	log_error("example", "cutils", NULL, "error");
	log_fatal("example", "cutils", NULL, "fatal");
}

static void example_make()
{
	c_printf(SEP, __func__);

	make_t make = { 0 };
	make_init(&make, 8, 8, 8);

	make_add_act(&make, make_create_var_ext(&make, STR("SLNDIR"), MAKE_VAR_INST));
	make_add_act(&make, make_create_var_ext(&make, STR("CONFIG"), MAKE_VAR_INST));

	make_var_t platform = make_add_act(&make, make_create_var_ext(&make, STR("PLATFORM"), MAKE_VAR_INST));
	make_var_t tool	    = make_add_act(&make, make_create_var_ext(&make, STR("TOOL"), MAKE_VAR_INST));

	make_var_t outdir = make_add_act(&make, make_create_var(&make, STR("OUTDIR"), MAKE_VAR_INST));
	make_var_add_val(&make, outdir, MSTR(STR("$(SLNDIR)bin/$(CONFIG)-$(PLATFORM)/test/")));

	make_if_t if_bits = make_add_act(&make, make_create_if(&make, MVAR(platform), MSTR(STR("x64"))));

	make_var_t bits64 = make_if_add_true_act(&make, if_bits, make_create_var(&make, STR("BITS"), MAKE_VAR_INST));
	make_var_add_val(&make, bits64, MSTR(STR("64")));

	make_var_t bits32 = make_if_add_false_act(&make, if_bits, make_create_var(&make, STR("BITS"), MAKE_VAR_INST));
	make_var_add_val(&make, bits32, MSTR(STR("32")));

	make_var_t target = make_add_act(&make, make_create_var(&make, STR("TARGET"), MAKE_VAR_INST));
	make_var_add_val(&make, target, MSTR(STR("$(OUTDIR)test$(BITS)")));

	make_rule_t compile = make_add_act(&make, make_create_rule(&make, MRULE(MVAR(target)), 1));
	make_rule_add_depend(&make, compile, MRULE(MSTR(STR("depend"))));

	make_rule_add_act(&make, compile, make_create_cmd(&make, MCMD(STR("prepare"))));

	make_rule_t if_gcc = make_rule_add_act(&make, compile, make_create_if(&make, MVAR(tool), MSTR(STR("gcc"))));

	make_if_add_true_act(&make, if_gcc, make_create_cmd(&make, MCMD(STR("gcc"))));

	make_if_add_false_act(&make, if_gcc, make_create_cmd(&make, MCMD(STR("clang"))));

	make_rule_add_act(&make, compile, make_create_cmd(&make, MCMD(STR("cleanup"))));

	make_print(&make, stdout);

	make_ext_set_val(&make, STR("SLNDIR"), MSTR(STR("./")));
	make_ext_set_val(&make, STR("CONFIG"), MSTR(STR("Debug")));
	make_ext_set_val(&make, STR("PLATFORM"), MSTR(STR("x64")));
	make_ext_set_val(&make, STR("TOOL"), MSTR(STR("cland")));

	make_expand(&make);

	str_t target_path = make_var_get_expanded(&make, STR("TARGET"));

	printf("TARGET: %.*s\n\n", (int)target_path.len, target_path.data);

	make_free(&make);
}

static int print_tree(FILE *file, void *data, int ret, const void *priv)
{
	(void)priv;
	c_fprintf(file, "%d\n", *(int *)data);
	return ret;
}

static void example_tree()
{
	c_printf(SEP, __func__);

	tree_t tree = { 0 };

	tree_init(&tree, 20, sizeof(int));

	*(int *)tree_get_data(&tree, tree_add(&tree)) = 0;

	tnode_t n1, n2, n3, n11, n12, n13, n21, n22, n23, n31, n32, n33, n111, n131, n212, n222, n232, n333, n1111;

	*(int *)tree_get_data(&tree, (n1 = tree_add_child(&tree, 0))) = 1;
	*(int *)tree_get_data(&tree, (n2 = tree_add_child(&tree, 0))) = 2;
	*(int *)tree_get_data(&tree, (n3 = tree_add_child(&tree, 0))) = 3;

	*(int *)tree_get_data(&tree, (n11 = tree_add_child(&tree, n1))) = 11;
	*(int *)tree_get_data(&tree, (n12 = tree_add_child(&tree, n1))) = 12;
	*(int *)tree_get_data(&tree, (n13 = tree_add_child(&tree, n1))) = 13;

	*(int *)tree_get_data(&tree, (n21 = tree_add_child(&tree, n2))) = 21;
	*(int *)tree_get_data(&tree, (n22 = tree_add_child(&tree, n2))) = 22;
	*(int *)tree_get_data(&tree, (n23 = tree_add_child(&tree, n2))) = 23;

	*(int *)tree_get_data(&tree, (n31 = tree_add_child(&tree, n3))) = 31;
	*(int *)tree_get_data(&tree, (n32 = tree_add_child(&tree, n3))) = 32;
	*(int *)tree_get_data(&tree, (n33 = tree_add_child(&tree, n3))) = 33;

	*(int *)tree_get_data(&tree, (n111 = tree_add_child(&tree, n11))) = 111;
	*(int *)tree_get_data(&tree, (n131 = tree_add_child(&tree, n13))) = 131;

	*(int *)tree_get_data(&tree, (n212 = tree_add_child(&tree, n21))) = 212;
	*(int *)tree_get_data(&tree, (n222 = tree_add_child(&tree, n22))) = 222;
	*(int *)tree_get_data(&tree, (n232 = tree_add_child(&tree, n23))) = 232;

	*(int *)tree_get_data(&tree, (n333 = tree_add_child(&tree, n33))) = 333;

	*(int *)tree_get_data(&tree, (n1111 = tree_add_child(&tree, n111))) = 1111;

	tree_print(&tree, 0, stdout, print_tree, 0, NULL);

	tree_free(&tree);
}

static void example_xml()
{
	c_printf(SEP, __func__);

	xml_t xml = { 0 };
	xml_init(&xml, 8, 8);

	xml_tag_t parent = xml_add_tag(&xml, XML_END, STR("Parent"));

	xml_add_tag(&xml, parent, STR("EmptyChild"));
	xml_add_tag_val(&xml, parent, STR("Child"), STR("Value"));
	xml_tag_t child3 = xml_add_tag(&xml, parent, STR("Child"));
	xml_tag_t child4 = xml_add_tag(&xml, parent, STR("Child"));
	xml_add_tag_val(&xml, parent, STR("Child"), strf("Value: %d", 5));

	xml_add_attr(&xml, child3, STR("Name"), STR("Child3"));

	xml_add_attr(&xml, child4, STR("Name"), STR("Child4"));
	xml_add_attr(&xml, child4, STR("Age"), STR("45"));
	xml_add_attr(&xml, child4, STR("Settings"), strf("Format:%s", "True"));

	xml_add_tag(&xml, child4, STR("Child41"));
	xml_add_tag(&xml, child4, STR("Child42"));

	xml_print(&xml, parent, stdout);

	xml_free(&xml);
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	cutils_t cutils = { 0 };
	c_init(&cutils);

	example_arr();
	example_json();
	example_list();
	example_log();
	example_make();
	example_tree();
	example_xml();

	c_printf(SEP, "----------");

	c_free(&cutils, stdout);

	return 0;
}
