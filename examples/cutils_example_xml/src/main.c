#include "cstr.h"
#include "mem.h"
#include "print.h"
#include "xml.h"

int main(int argc, char **argv)
{
	m_stats_t m_stats = { 0 };
	m_init(&m_stats);

	xml_t xml = { 0 };
	xml_init(&xml, 10);

	xml_tag_t parent = xml_add_child(&xml, 0, CSTR("Parent"));

	xml_tag_t child1 = xml_add_child(&xml, parent, CSTR("EmptyChild"));
	xml_tag_t child2 = xml_add_child_val(&xml, parent, CSTR("Child"), CSTR("Value"));
	xml_tag_t child3 = xml_add_child(&xml, parent, CSTR("Child"));
	xml_tag_t child4 = xml_add_child(&xml, parent, CSTR("Child"));
	xml_tag_t child5 = xml_add_child_val_f(&xml, parent, CSTR("Child"), "Value: %d", 5);

	xml_add_attr(&xml, child3, CSTR("Name"), CSTR("Child3"));

	xml_add_attr(&xml, child4, CSTR("Name"), CSTR("Child4"));
	xml_add_attr(&xml, child4, CSTR("Age"), CSTR("45"));
	xml_add_attr_f(&xml, child4, CSTR("Settings"), "Format:%s", "True");

	xml_tag_t child41 = xml_add_child(&xml, child4, CSTR("Child41"));
	xml_tag_t child42 = xml_add_child(&xml, child4, CSTR("Child42"));

	xml_print(&xml, stdout);

	xml_free(&xml);

	m_print(stdout);

	return 0;
}
