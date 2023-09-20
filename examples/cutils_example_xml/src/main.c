#include "cstr.h"
#include "mem.h"
#include "print.h"
#include "xml.h"

int main(int argc, char **argv)
{
	mem_stats_t mem_stats = { 0 };
	mem_init(&mem_stats);

	xml_t xml = { 0 };
	xml_init(&xml, 8);

	xml_tag_t parent = xml_add_tag(&xml, TREE_END, STR("Parent"));

	xml_tag_t child1 = xml_add_tag(&xml, parent, STR("EmptyChild"));
	xml_tag_t child2 = xml_add_tag_val(&xml, parent, STR("Child"), STR("Value"));
	xml_tag_t child3 = xml_add_tag(&xml, parent, STR("Child"));
	xml_tag_t child4 = xml_add_tag(&xml, parent, STR("Child"));
	xml_tag_t child5 = xml_add_tag_val(&xml, parent, STR("Child"), strf("Value: %d", 5));

	xml_add_attr(&xml, child3, STR("Name"), STR("Child3"));

	xml_add_attr(&xml, child4, STR("Name"), STR("Child4"));
	xml_add_attr(&xml, child4, STR("Age"), STR("45"));
	xml_add_attr(&xml, child4, STR("Settings"), strf("Format:%s", "True"));

	xml_tag_t child41 = xml_add_tag(&xml, child4, STR("Child41"));
	xml_tag_t child42 = xml_add_tag(&xml, child4, STR("Child42"));

	xml_print(&xml, parent, stdout);

	xml_free(&xml);

	mem_print(stdout);

	return 0;
}
