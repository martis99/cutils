#include "args.h"
#include "arr.h"
#include "bnf.h"
#include "cutils.h"
#include "json.h"
#include "lexer.h"
#include "list.h"
#include "log.h"
#include "make.h"
#include "parser.h"
#include "print.h"
#include "syntax.h"
#include "tree.h"
#include "xml.h"

#define SEP "\n------%s------\n\n"

static int print_arr(void *data, print_dst_t dst, const void *priv)
{
	(void)priv;
	return dprintf(dst, "%d\n", *(int *)data);
}

static int handle_dir(const char *param, void *ret)
{
	(void)param;
	(void)ret;
	return 0;
}

static void example_args()
{
	c_printf(SEP, __func__);

	arg_t args[] = {
		[0] = ARG('T', "test", PARAM_NONE, "", "Run tests", NULL),
		[1] = ARG('B', "dir", PARAM_STR, "<dir>", "Set directory", handle_dir),
		[2] = ARG('S', "dir", PARAM_STR, "<dir>", "Set directory", NULL),
		[3] = ARG('D', "debug", PARAM_SWITCH, "<0/1>", "Turn on/off debug messages (default: 0)", NULL),
		[4] = ARG('M', "mode", PARAM_MODE, "<mode>", "Set mode (default: A)", NULL),
	};

	mode_t mode_modes[] = {
		[0] = { .c = 'A', .desc = "A mode" },
		[1] = { .c = 'B', .desc = "B mode" },
	};

	mode_desc_t modes[] = {
		{ .name = "Modes", .modes = mode_modes, .len = 2 },
	};

	char *dir = NULL;
	int debug = 0;

	void *params[] = {
		[2] = &dir,
		[3] = &debug,
	};

	const char *argv[] = {
		"example",
		"-H",
	};

	args_handle("example", "example program", args, sizeof(args), modes, sizeof(modes), sizeof(argv) / sizeof(char *), argv, params, PRINT_DST_STD());
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

	arr_print(&arr, print_arr, PRINT_DST_STD(), NULL);

	arr_free(&arr);
}

static void example_bnf()
{
	c_printf(SEP, __func__);

	bnf_t bnf = { 0 };
	bnf_get_stx(&bnf);

	str_t sbnf = STR("<file>    ::= <bnf> EOF\n"
			 "<bnf>     ::= <rules>\n"
			 "<rules>   ::= <rule> <rules> | <rule>\n"
			 "<rule>    ::= '<' <rname> '>' <spaces> '::=' <space> <expr> NL\n"
			 "<rname>   ::= LOWER <rchars> | LOWER\n"
			 "<rchars>  ::= <rchar> <rchars> | <rchar>\n"
			 "<rchar>   ::= LOWER | '-'\n"
			 "<expr>    ::= <terms> <space> '|' <space> <expr> | <terms>\n"
			 "<terms>   ::= <term> <space> <terms> | <term>\n"
			 "<term>    ::= <literal> | <token> | '<' <rname> '>'\n"
			 "<literal> ::= \"'\" <tdouble> \"'\" | '\"' <tsingle> '\"'\n"
			 "<token>   ::= UPPER <token> | UPPER\n"
			 "<tdouble> ::= <cdouble> <tdouble> | <cdouble>\n"
			 "<tsingle> ::= <csingle> <tsingle> | <csingle>\n"
			 "<cdouble> ::= <char> | '\"'\n"
			 "<csingle> ::= <char> | \"'\"\n"
			 "<char>    ::= ALPHA | DIGIT | SYMBOL | <space>\n"
			 "<spaces>  ::= <space> <spaces> | <space>\n"
			 "<space>   ::= ' '\n");

	lex_t lex = { 0 };
	lex_init(&lex, 100);
	lex_tokenize(&lex, sbnf);

	prs_t prs = { 0 };
	prs_init(&prs, 100);

	prs_node_t prs_root = prs_parse(&prs, &bnf.stx, bnf.file, &lex);

	stx_t new_stx = { 0 };
	stx_init(&new_stx, 10, 10);
	stx_from_bnf(&bnf, &prs, prs_root, &new_stx);

	stx_print(&new_stx, PRINT_DST_STD());

	stx_free(&new_stx);
	lex_free(&lex);
	prs_free(&prs);
	bnf_free(&bnf);
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

	json_print(&json, root, PRINT_DST_STD(), "\t");

	json_free(&json);
}

static int print_list(void *data, print_dst_t dst, const void *priv)
{
	(void)priv;
	return dprintf(dst, "%d\n", *(int *)data);
}

static void example_lexer()
{
	c_printf(SEP, __func__);

	lex_t lex = { 0 };

	lex_init(&lex, 10);

	str_t src = STR("int main() {\n"
			"	return 0;\n"
			"}\n");

	str_print(src, PRINT_DST_STD());

	printf("\n\n");

	lex_tokenize(&lex, src);

	lex_dbg(&lex, PRINT_DST_STD());

	lex_free(&lex);
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

	list_print(&list, n1, print_list, PRINT_DST_STD(), NULL);
	list_print(&list, n2, print_list, PRINT_DST_STD(), NULL);
	list_print(&list, n3, print_list, PRINT_DST_STD(), NULL);

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

	make_print(&make, PRINT_DST_STD());

	make_ext_set_val(&make, STR("SLNDIR"), MSTR(STR("./")));
	make_ext_set_val(&make, STR("CONFIG"), MSTR(STR("Debug")));
	make_ext_set_val(&make, STR("PLATFORM"), MSTR(STR("x64")));
	make_ext_set_val(&make, STR("TOOL"), MSTR(STR("cland")));

	make_expand(&make);

	str_t target_path = make_var_get_expanded(&make, STR("TARGET"));

	printf("TARGET: %.*s\n", (int)target_path.len, target_path.data);

	make_free(&make);
}

static void example_parser()
{
	c_printf(SEP, __func__);

	prs_t prs = { 0 };
	prs_init(&prs, 100);

	lex_t lex = { 0 };
	lex_init(&lex, 100);

	stx_t stx = { 0 };

	lex_token_t a		= lex_add_token(&lex);
	*lex_get_token(&lex, a) = (token_t){
		.value = STR("a"),
	};

	lex_token_t b		= lex_add_token(&lex);
	*lex_get_token(&lex, b) = (token_t){
		.value = STR("b"),
	};

	stx_init(&stx, 10, 10);

	prs.stx = &stx;
	prs.lex = &lex;

	stx_rule_t file_rule = stx_add_rule(&stx, STR("file"));

	prs_node_t root	  = prs_add_node(&prs, PRS_NODE_END, PRS_NODE_RULE(file_rule));
	prs_node_t alt0	  = prs_add_node(&prs, root, PRS_NODE_ALT(0));
	prs_node_t child0 = prs_add_node(&prs, alt0, PRS_NODE_TOKEN(a));
	prs_node_t alt1	  = prs_add_node(&prs, child0, PRS_NODE_ALT(1));
	prs_add_node(&prs, alt1, PRS_NODE_TOKEN(b));

	prs_print(&prs, root, PRINT_DST_STD());

	prs_free(&prs);
	lex_free(&lex);
	stx_free(&stx);
}

static void example_syntax()
{
	c_printf(SEP, __func__);

	stx_t stx = { 0 };

	stx_init(&stx, 10, 10);

	const stx_rule_t file	     = stx_add_rule(&stx, STR("file"));
	const stx_rule_t c	     = stx_add_rule(&stx, STR("c"));
	const stx_rule_t functions   = stx_add_rule(&stx, STR("functions"));
	const stx_rule_t function    = stx_add_rule(&stx, STR("function"));
	const stx_rule_t type	     = stx_add_rule(&stx, STR("type"));
	const stx_rule_t name	     = stx_add_rule(&stx, STR("name"));
	const stx_rule_t identifier  = stx_add_rule(&stx, STR("identifier"));
	const stx_rule_t chars	     = stx_add_rule(&stx, STR("chars"));
	const stx_rule_t expressions = stx_add_rule(&stx, STR("expressions"));
	const stx_rule_t expression  = stx_add_rule(&stx, STR("expression"));

	stx_rule_add_term(&stx, file, STX_TERM_RULE(&stx, c));
	stx_rule_add_term(&stx, file, STX_TERM_TOKEN(&stx, TOKEN_EOF));

	stx_rule_add_term(&stx, c, STX_TERM_RULE(&stx, functions));

	stx_rule_add_arr(&stx, functions, STX_TERM_RULE(&stx, function), STX_TERM_NONE());

	stx_rule_add_term(&stx, function, STX_TERM_RULE(&stx, type));
	stx_rule_add_term(&stx, function, STX_TERM_RULE(&stx, name));
	stx_rule_add_term(&stx, function, STX_TERM_LITERAL(&stx, STR("(")));
	stx_rule_add_term(&stx, function, STX_TERM_LITERAL(&stx, STR(")")));
	stx_rule_add_term(&stx, function, STX_TERM_TOKEN(&stx, TOKEN_WS));
	stx_rule_add_term(&stx, function, STX_TERM_LITERAL(&stx, STR("{")));
	stx_rule_add_term(&stx, function, STX_TERM_TOKEN(&stx, TOKEN_NL));
	stx_rule_add_term(&stx, function, STX_TERM_RULE(&stx, expressions));
	stx_rule_add_term(&stx, function, STX_TERM_LITERAL(&stx, STR("}")));

	stx_rule_add_term(&stx, type, STX_TERM_LITERAL(&stx, STR("int")));

	stx_rule_add_term(&stx, name, STX_TERM_RULE(&stx, identifier));

	stx_rule_add_arr(&stx, identifier, STX_TERM_RULE(&stx, chars), STX_TERM_NONE(&stx));

	stx_rule_add_or(&stx, chars, 3, STX_TERM_TOKEN(&stx, TOKEN_ALPHA), STX_TERM_TOKEN(&stx, TOKEN_DIGIT), STX_TERM_LITERAL(&stx, STR("_")));

	stx_rule_add_arr(&stx, expressions, STX_TERM_RULE(&stx, expression), STX_TERM_TOKEN(&stx, TOKEN_NL));

	stx_rule_add_term(&stx, expression, STX_TERM_LITERAL(&stx, STR("return 0;")));

	stx_compile(&stx);

	stx_print(&stx, PRINT_DST_STD());

	printf("\n");

	stx_print_tree(&stx, file, PRINT_DST_STD());

	stx_free(&stx);
}
static int print_tree(void *data, print_dst_t dst, const void *priv)
{
	(void)priv;
	return dprintf(dst, "%d\n", *(int *)data);
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

	tree_print(&tree, 0, print_tree, PRINT_DST_STD(), NULL);

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

	xml_print(&xml, parent, PRINT_DST_STD());

	xml_free(&xml);
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	cutils_t cutils = { 0 };
	c_init(&cutils);

	example_args();
	example_arr();
	example_bnf();
	example_json();
	example_lexer();
	example_list();
	example_log();
	example_make();
	example_parser();
	example_syntax();
	example_tree();
	example_xml();

	c_printf(SEP, "----------");

	c_free(&cutils, PRINT_DST_STD());

	return 0;
}
