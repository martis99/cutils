#include "t_tree.h"

#include "test.h"

#include "mem.h"

#include <stdio.h>

TEST(test)
{
	SSTART;
	RUN(tree);
	SEND;
}

int main(int argc, char **argv)
{
	m_stats_t m_stats = { 0 };
	m_init(&m_stats);

	t_init(4);

	test();

	t_results();
	t_free();

	printf("mem:      %zd\n", m_stats.mem);
	printf("max mem:  %zd\n", m_stats.max_mem);
	printf("allocs:   %d\n", m_stats.allocs);
	printf("reallocs: %d\n", m_stats.reallocs);

	return 0;
}
