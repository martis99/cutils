#include "arr.h"
#include "mem.h"
#include "print.h"

static int print_arr(FILE *file, void *data, int ret)
{
	p_fprintf(file, "%d\n", *(int *)data);
	return ret;
}

int main(int argc, char **argv)
{
	m_stats_t m_stats = { 0 };
	m_init(&m_stats);

	arr_t arr = { 0 };

	arr_init(&arr, 4, sizeof(int));

	*(int *)arr_get(&arr, arr_add(&arr)) = 0;
	*(int *)arr_get(&arr, arr_add(&arr)) = 1;
	*(int *)arr_get(&arr, arr_add(&arr)) = 2;
	*(int *)arr_get(&arr, arr_add(&arr)) = 3;

	arr_print(&arr, stdout, print_arr, 0);

	arr_free(&arr);

	m_print(stdout);

	return 0;
}
