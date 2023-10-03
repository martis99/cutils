#include "t_args.h"
#include "t_arr.h"
#include "t_cstr.h"
#include "t_cutils.h"
#include "t_dict.h"
#include "t_file.h"
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

#include "test.h"

#include "test_test.h"

#include "cutils.h"

TEST(test_cutils)
{
	SSTART;
	RUN(t_args);
	RUN(t_arr);
	RUN(t_cstr);
	RUN(t_cutils);
	RUN(t_dict);
	RUN(t_file);
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

int main(int argc, char **argv)
{
	cutils_t cutils = { 0 };
	c_init(&cutils);

	t_init(80);

	log_set_level(LOG_FATAL);

	test_test();
	test_cutils();
	const int ret = t_finish();

	c_free(&cutils, stdout);

	return ret;
}
