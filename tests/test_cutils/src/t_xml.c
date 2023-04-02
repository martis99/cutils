#include "t_xml.h"

#include "cstr.h"
#include "file.h"
#include "mem.h"
#include "xml.h"

#include "test.h"

#define TEST_FILE "test_xml.xml"

TEST(init_free)
{
	START;

	xml_t xml = { 0 };

	xml_init(&xml, 4);

	EXPECT_NE(xml.tags.nodes, NULL);
	EXPECT_NE(xml.attrs.nodes, NULL);

	xml_free(&xml);

	EXPECT_EQ(xml.tags.nodes, NULL);
	EXPECT_EQ(xml.attrs.nodes, NULL);

	END;
}

TEST(add_child, FILE *file)
{
	START;

	xml_t xml = { 0 };

	xml_init(&xml, 4);

	xml_add_child(&xml, 0, CSTR("Project"));

	FILE *nf = file_reopen(TEST_FILE, "wb+", file);
	xml_print(&xml, file);

	char buf[1024] = { 0 };

	unsigned int len = (unsigned int)file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			   "<Project />\n";

	EXPECT(cstr_cmp(buf, len, exp, sizeof(exp) - 1));

	xml_free(&xml);

	END;
}

TEST(add_child_val, FILE *file)
{
	START;

	xml_t xml = { 0 };

	xml_init(&xml, 4);

	xml_add_child_val(&xml, 0, CSTR("Project"), CSTR("Name"));

	file_reopen(TEST_FILE, "wb+", file);
	xml_print(&xml, file);

	char buf[1024] = { 0 };

	unsigned int len = (unsigned int)file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			   "<Project>Name</Project>\n";

	EXPECT(cstr_cmp(buf, len, exp, sizeof(exp) - 1));

	xml_free(&xml);

	END;
}

TEST(add_child_val_c, FILE *file)
{
	START;

	xml_t xml = { 0 };

	xml_init(&xml, 4);

	xml_add_child_val_c(&xml, 0, CSTR("Project"), CSTR("Name"));

	file_reopen(TEST_FILE, "wb+", file);
	xml_print(&xml, file);

	char buf[1024] = { 0 };

	unsigned int len = (unsigned int)file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			   "<Project>Name</Project>\n";

	EXPECT(cstr_cmp(buf, len, exp, sizeof(exp) - 1));

	xml_free(&xml);

	END;
}

TEST(add_child_val_f, FILE *file)
{
	START;

	xml_t xml = { 0 };

	xml_init(&xml, 4);

	xml_add_child_val_f(&xml, 0, CSTR("Project"), "Name%d", 1);

	file_reopen(TEST_FILE, "wb+", file);
	xml_print(&xml, file);

	char buf[1024] = { 0 };

	unsigned int len = (unsigned int)file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			   "<Project>Name1</Project>\n";

	EXPECT(cstr_cmp(buf, len, exp, sizeof(exp) - 1));

	xml_free(&xml);

	END;
}

TEST(add_child_val_r_stack, FILE *file)
{
	START;

	xml_t xml = { 0 };

	xml_init(&xml, 4);

	xml_add_child_val_r(&xml, 0, CSTR("Project"), CSTR("Name"), 0);

	file_reopen(TEST_FILE, "wb+", file);
	xml_print(&xml, file);

	char buf[1024] = { 0 };

	unsigned int len = (unsigned int)file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			   "<Project>Name</Project>\n";

	EXPECT(cstr_cmp(buf, len, exp, sizeof(exp) - 1));

	xml_free(&xml);

	END;
}

TEST(add_child_val_r_heap, FILE *file)
{
	START;

	xml_t xml = { 0 };

	xml_init(&xml, 4);

	char val[] = "Name";

	char *name = m_malloc(sizeof(val));
	m_memcpy(name, sizeof(val), val, sizeof(val));

	xml_add_child_val_r(&xml, 0, CSTR("Project"), name, sizeof(val), 1);

	file_reopen(TEST_FILE, "wb+", file);
	xml_print(&xml, file);

	char buf[1024] = { 0 };

	unsigned int len = (unsigned int)file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			   "<Project>Name</Project>\n";

	EXPECT(cstr_cmp(buf, len, exp, sizeof(exp) - 1));

	xml_free(&xml);

	END;
}

TEST(add_attr, FILE *file)
{
	START;

	xml_t xml = { 0 };

	xml_init(&xml, 4);

	const xml_tag_t project = xml_add_child(&xml, 0, CSTR("Project"));
	xml_add_attr(&xml, project, CSTR("Name"), CSTR("Project1"));

	file_reopen(TEST_FILE, "wb+", file);
	xml_print(&xml, file);

	char buf[1024] = { 0 };

	unsigned int len = (unsigned int)file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			   "<Project Name=\"Project1\" />\n";

	EXPECT(cstr_cmp(buf, len, exp, sizeof(exp) - 1));

	xml_free(&xml);

	END;
}

TEST(add_attr_c, FILE *file)
{
	START;

	xml_t xml = { 0 };

	xml_init(&xml, 4);

	const xml_tag_t project = xml_add_child(&xml, 0, CSTR("Project"));
	xml_add_attr_c(&xml, project, CSTR("Name"), CSTR("Project1"));

	file_reopen(TEST_FILE, "wb+", file);
	xml_print(&xml, file);

	char buf[1024] = { 0 };

	unsigned int len = (unsigned int)file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			   "<Project Name=\"Project1\" />\n";

	EXPECT(cstr_cmp(buf, len, exp, sizeof(exp) - 1));

	xml_free(&xml);

	END;
}

TEST(add_attr_f, FILE *file)
{
	START;

	xml_t xml = { 0 };

	xml_init(&xml, 4);

	const xml_tag_t project = xml_add_child(&xml, 0, CSTR("Project"));
	xml_add_attr_f(&xml, project, CSTR("Name"), "Project%d", 2);

	file_reopen(TEST_FILE, "wb+", file);
	xml_print(&xml, file);

	char buf[1024] = { 0 };

	unsigned int len = (unsigned int)file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			   "<Project Name=\"Project2\" />\n";

	EXPECT(cstr_cmp(buf, len, exp, sizeof(exp) - 1));

	xml_free(&xml);

	END;
}

TEST(add_attr_r_stack, FILE *file)
{
	START;

	xml_t xml = { 0 };

	xml_init(&xml, 4);

	const xml_tag_t project = xml_add_child(&xml, 0, CSTR("Project"));
	xml_add_attr_r(&xml, project, CSTR("Name"), CSTR("Project1"), 0);

	file_reopen(TEST_FILE, "wb+", file);
	xml_print(&xml, file);

	char buf[1024] = { 0 };

	unsigned int len = (unsigned int)file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			   "<Project Name=\"Project1\" />\n";

	EXPECT(cstr_cmp(buf, len, exp, sizeof(exp) - 1));

	xml_free(&xml);

	END;
}

TEST(add_attr_r_heap, FILE *file)
{
	START;

	xml_t xml = { 0 };

	xml_init(&xml, 4);

	char val[] = "Project3";

	char *name = m_malloc(sizeof(val));
	m_memcpy(name, sizeof(val), val, sizeof(val));

	const xml_tag_t project = xml_add_child(&xml, 0, CSTR("Project"));
	xml_add_attr_r(&xml, project, CSTR("Name"), name, sizeof(val), 1);

	file_reopen(TEST_FILE, "wb+", file);
	xml_print(&xml, file);

	char buf[1024] = { 0 };

	unsigned int len = (unsigned int)file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			   "<Project Name=\"Project3\" />\n";

	EXPECT(cstr_cmp(buf, len, exp, sizeof(exp) - 1));

	xml_free(&xml);

	END;
}

TEST(add_child_val_attr, FILE *file)
{
	START;

	xml_t xml = { 0 };

	xml_init(&xml, 4);

	const xml_tag_t project = xml_add_child_val(&xml, 0, CSTR("Project"), CSTR("Value"));
	xml_add_attr(&xml, project, CSTR("Name"), CSTR("Project1"));

	file_reopen(TEST_FILE, "wb+", file);
	xml_print(&xml, file);

	char buf[1024] = { 0 };

	unsigned int len = (unsigned int)file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			   "<Project Name=\"Project1\">Value</Project>\n";

	EXPECT(cstr_cmp(buf, len, exp, sizeof(exp) - 1));

	xml_free(&xml);

	END;
}

TEST(add_child_child, FILE *file)
{
	START;

	xml_t xml = { 0 };

	xml_init(&xml, 4);

	const xml_tag_t project = xml_add_child(&xml, 0, CSTR("Project"));
	xml_add_child(&xml, project, CSTR("Child"));

	file_reopen(TEST_FILE, "wb+", file);
	xml_print(&xml, file);

	char buf[1024] = { 0 };

	unsigned int len = (unsigned int)file_read_ft(file, buf, sizeof(buf));

	const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			   "<Project>\n"
			   "  <Child />\n"
			   "</Project>\n";

	EXPECT(cstr_cmp(buf, len, exp, sizeof(exp) - 1));

	xml_free(&xml);

	END;
}

TEST(add_child_tests, FILE *file)
{
	SSTART;
	RUN(add_child, file);
	RUN(add_child_val, file);
	RUN(add_child_val_c, file);
	RUN(add_child_val_f, file);
	RUN(add_child_val_r_stack, file);
	RUN(add_child_val_r_heap, file);
	SEND;
}

TEST(add_attr_tests, FILE *file)
{
	SSTART;
	RUN(add_attr, file);
	RUN(add_attr_c, file);
	RUN(add_attr_f, file);
	RUN(add_attr_r_stack, file);
	RUN(add_attr_r_heap, file);
	SEND;
}

TEST(add, FILE *file)
{
	SSTART;
	RUN(add_child_tests, file);
	RUN(add_attr_tests, file);
	RUN(add_child_val_attr, file);
	RUN(add_child_child, file);
	SEND;
}

STEST(xml)
{
	SSTART;

	FILE *file = file_open(TEST_FILE, "wb+");

	RUN(init_free, file);
	RUN(add, file);

	file_close(file);

	file_delete(TEST_FILE);

	SEND;
}
