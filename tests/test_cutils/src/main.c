#include "t_file.h"
#include "t_list.h"
#include "t_log.h"
#include "t_md5.h"
#include "t_time.h"
#include "t_tree.h"
#include "t_type.h"
#include "t_xml.h"

#include "test.h"

#include "mem.h"

TEST(tests)
{
	SSTART;
	RUN(file);
	RUN(list);
	RUN(logt);
	RUN(md5);
	RUN(time);
	RUN(tree);
	RUN(type);
	RUN(xml);
	SEND;
}

int main(int argc, char **argv)
{
	m_stats_t m_stats = { 0 };
	m_init(&m_stats);

	t_init(80);
	tests();
	const int ret = t_finish();

	m_print(stdout);

	return ret;
}
