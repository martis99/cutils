#include "list.h"
#include "mem.h"
#include "print.h"

static int print_list(FILE *file, void *data, int ret)
{
	c_fprintf(file, "%d\n", *(int *)data);
	return ret;
}

int main(int argc, char **argv)
{
	mem_stats_t mem_stats = { 0 };
	mem_init(&mem_stats);

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

	list_print(&list, n1, stdout, print_list, 0);
	list_print(&list, n2, stdout, print_list, 0);
	list_print(&list, n3, stdout, print_list, 0);

	list_free(&list);

	mem_print(stdout);

	return 0;
}
