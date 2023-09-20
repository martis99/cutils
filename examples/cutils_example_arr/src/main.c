#include "arr.h"
#include "mem.h"
#include "print.h"

static int print_arr(FILE *file, void *data, int ret)
{
	c_fprintf(file, "%d\n", *(int *)data);
	return ret;
}

int main(int argc, char **argv)
{
	mem_stats_t mem_stats = { 0 };
	mem_init(&mem_stats);

	arr_t arr = { 0 };

	arr_init(&arr, 4, sizeof(int));

	*(int *)arr_get(&arr, arr_add(&arr)) = 0;
	*(int *)arr_get(&arr, arr_add(&arr)) = 1;
	*(int *)arr_get(&arr, arr_add(&arr)) = 2;
	*(int *)arr_get(&arr, arr_add(&arr)) = 3;

	arr_print(&arr, stdout, print_arr, 0);

	arr_free(&arr);

	mem_print(stdout);

	return 0;
}
