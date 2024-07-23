#include "test_cutils.h"

#include "cutils.h"
#include "test.h"
#include "test_test.h"

STEST(t_args);
STEST(t_arr);
STEST(t_bnf);
STEST(t_cstr);
STEST(t_cplatform);
STEST(t_cutils);
STEST(t_dict);
STEST(t_ebnf);
STEST(t_eparser);
STEST(t_esyntax);
STEST(t_file);
STEST(t_ini);
STEST(t_ini_parse);
STEST(t_json);
STEST(t_lexer);
STEST(t_list);
STEST(t_log);
STEST(t_md5);
STEST(t_mem);
STEST(t_parser);
STEST(t_path);
STEST(t_print);
STEST(t_str);
STEST(t_syntax);
STEST(t_time);
STEST(t_token);
STEST(t_tree);
STEST(t_type);
STEST(t_xml);

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
