#include "test_cutils.h"

#include "t_args.h"
#include "t_arr.h"
#include "t_cstr.h"
#include "t_cutils.h"
#include "t_dict.h"
#include "t_file.h"
#include "t_json.h"
#include "t_list.h"
#include "t_log.h"
#include "t_make.h"
#include "t_md5.h"
#include "t_mem.h"
#include "t_path.h"
#include "t_print.h"
#include "t_str.h"
#include "t_time.h"
#include "t_tree.h"
#include "t_type.h"
#include "t_xml.h"

#include "cutils.h"

#include "test.h"
#include "test_test.h"

TEST(tests)
{
	SSTART;
	RUN(t_args);
	RUN(t_arr);
	RUN(t_cstr);
	RUN(t_cutils);
	RUN(t_dict);
	RUN(t_file);
	RUN(t_json);
	RUN(t_list);
	RUN(t_log);
	RUN(t_make);
	RUN(t_md5);
	RUN(t_mem);
	RUN(t_path);
	RUN(t_print);
	RUN(t_str);
	RUN(t_time);
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
