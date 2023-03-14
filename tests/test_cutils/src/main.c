#include "t_tree.h"

#include "tree.h"

#include "test.h"

#include "mem.h"

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

	m_print(stdout);

	return 0;
}
