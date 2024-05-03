#include "test_cutils.h"

#include "t_cutils_c.h"

#include "cutils.h"

#include "test_test.h"

TEST(tests)
{
	SSTART;
	RUN(t_args);
	RUN(t_arr);
	RUN(t_bnf);
	RUN(t_cplatform);
	RUN(t_cstr);
	RUN(t_cutils);
	RUN(t_dict);
	RUN(t_ebnf);
	RUN(t_eparser);
	RUN(t_esyntax);
	RUN(t_file);
	RUN(t_ini);
	RUN(t_ini_parse);
	RUN(t_json);
	RUN(t_lexer);
	RUN(t_list);
	RUN(t_log);
	RUN(t_make);
	RUN(t_md5);
	RUN(t_mem);
	RUN(t_parser);
	RUN(t_path);
	RUN(t_print);
	RUN(t_str);
	RUN(t_syntax);
	RUN(t_time);
	RUN(t_token);
	RUN(t_tree);
	RUN(t_type);
	RUN(t_xml);
	SEND;
}

int test_cutils()
{
	t_run(test_test, 0);
	tests();

	return 0;
}
