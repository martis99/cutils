#include "t_cutils_c.h"

#include "cstr.h"
#include "file.h"
#include "mem.h"
#include "xml.h"

#define TEST_FILE "t_xml.txt"

TEST(t_xml_init_free)
{
	START;

	xml_t xml = { 0 };

	EXPECT_EQ(xml_init(NULL, 0, 0), NULL);
	mem_oom(1);
	EXPECT_EQ(xml_init(&xml, 1, 0), NULL);
	EXPECT_EQ(xml_init(&xml, 0, 1), NULL);
	mem_oom(0);
	EXPECT_EQ(xml_init(&xml, 0, 0), &xml);

	EXPECT_NE(xml.tags.data, NULL);
	EXPECT_NE(xml.attrs.data, NULL);

	xml_free(&xml);
	xml_free(NULL);

	EXPECT_EQ(xml.tags.data, NULL);
	EXPECT_EQ(xml.attrs.data, NULL);

	END;
}

TEST(t_xml_add_tag)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	EXPECT_EQ(xml_add_tag(NULL, XML_END, str_null()), XML_END);
	EXPECT_EQ(xml_add_tag(&xml, XML_END, STRH("")), 0);

	xml_free(&xml);

	END;
}

TEST(t_xml_add_tag_val)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 0, 0);

	EXPECT_EQ(xml_add_tag_val(NULL, XML_END, str_null(), str_null()), XML_END);
	mem_oom(1);
	EXPECT_EQ(xml_add_tag_val(&xml, XML_END, STRH(""), STRH("")), XML_END);
	mem_oom(0);
	EXPECT_EQ(xml_add_tag_val(&xml, XML_END, STRH(""), STRH("")), 0);

	xml_free(&xml);

	END;
}

TEST(t_xml_remove_tag)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	EXPECT_EQ(xml_remove_tag(NULL, XML_END), 1);
	EXPECT_EQ(xml_remove_tag(&xml, XML_END), 1);
	EXPECT_EQ(xml_remove_tag(&xml, xml_add_tag(&xml, XML_END, STRH(""))), 0);

	xml_free(&xml);

	END;
}

TEST(t_xml_has_child)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	EXPECT_EQ(xml_has_child(NULL, XML_END), 0);
	EXPECT_EQ(xml_has_child(&xml, XML_END), 0);

	xml_tag_t parent = xml_add_tag(&xml, XML_END, STRH(""));
	xml_add_tag(&xml, parent, STRH(""));
	EXPECT_EQ(xml_has_child(&xml, parent), 1);

	xml_free(&xml);

	END;
}

TEST(t_xml_add_attr)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 0, 0);

	EXPECT_EQ(xml_add_attr(NULL, XML_END, str_null(), str_null()), LIST_END);
	EXPECT_EQ(xml_add_attr(&xml, XML_END, str_null(), str_null()), LIST_END);
	mem_oom(1);
	EXPECT_EQ(xml_add_attr(&xml, xml_add_tag(&xml, XML_END, STRH("")), STRH(""), STRH("")), LIST_END);
	mem_oom(0);
	EXPECT_EQ(xml_add_attr(&xml, xml_add_tag(&xml, XML_END, STRH("")), STRH(""), STRH("")), 0);

	xml_free(&xml);

	END;
}

TEST(t_xml_add)
{
	SSTART;
	RUN(t_xml_add_tag);
	RUN(t_xml_add_tag_val);
	RUN(t_xml_remove_tag);
	RUN(t_xml_has_child);
	RUN(t_xml_add_attr);
	SEND;
}

TEST(t_xml_print, FILE *file)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	EXPECT_EQ(xml_print(NULL, XML_END, NULL), 1);
	EXPECT_EQ(xml_print(&xml, XML_END, NULL), 1);

	xml_tag_t tag = xml_add_tag(&xml, XML_END, STRH(""));
	EXPECT_EQ(xml_print(&xml, tag, NULL), 1);

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(xml_print(&xml, tag, file), 0);

		char buf[128] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				   "< />\n";
		EXPECT_STR(buf, exp);
	}

	xml_free(&xml);

	END;
}

TEST(t_xml_print_tag, FILE *file)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	xml_add_tag(&xml, XML_END, STRH("Project"));

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(xml_print(&xml, 0, file), 0);

		char buf[128] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				   "<Project />\n";
		EXPECT_STR(buf, exp);
	}

	xml_free(&xml);

	END;
}

TEST(t_xml_print_tag_val, FILE *file)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	xml_add_tag_val(&xml, XML_END, STRH("Project"), STRH("Name"));

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(xml_print(&xml, 0, file), 0);

		char buf[128] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				   "<Project>Name</Project>\n";
		EXPECT_STR(buf, exp);
	}

	xml_free(&xml);

	END;
}

TEST(t_xml_print_tag_val_nl, FILE *file)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	xml_add_tag_val(&xml, XML_END, STRH("Project"), STRH("Name\n"));

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(xml_print(&xml, 0, file), 0);

		char buf[128] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				   "<Project>Name\n"
				   "</Project>\n";
		EXPECT_STR(buf, exp);
	}

	xml_free(&xml);

	END;
}

TEST(t_xml_print_tag_attr, FILE *file)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	const xml_tag_t project = xml_add_tag(&xml, XML_END, STRH("Project"));
	xml_add_attr(&xml, project, STRH("Name"), STRH("Project1"));

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(xml_print(&xml, 0, file), 0);

		char buf[128] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				   "<Project Name=\"Project1\" />\n";
		EXPECT_STR(buf, exp);
	}

	xml_free(&xml);

	END;
}

TEST(t_xml_print_tag_val_attr, FILE *file)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	const xml_tag_t project = xml_add_tag_val(&xml, XML_END, STRH("Project"), STRH("Value"));
	xml_add_attr(&xml, project, STRH("Name"), STRH("Project1"));

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(xml_print(&xml, 0, file), 0);

		char buf[128] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				   "<Project Name=\"Project1\">Value</Project>\n";
		EXPECT_STR(buf, exp);
	}

	xml_free(&xml);

	END;
}

TEST(t_xml_print_tag_child, FILE *file)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	const xml_tag_t project = xml_add_tag(&xml, XML_END, STRH("Project"));
	xml_add_tag(&xml, project, STRH("Child"));

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(xml_print(&xml, 0, file), 0);

		char buf[128] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				   "<Project>\n"
				   "  <Child />\n"
				   "</Project>\n";
		EXPECT_STR(buf, exp);
	}

	xml_free(&xml);

	END;
}

TEST(t_xml_print_remove_tag, FILE *file)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	const xml_tag_t project = xml_add_tag(&xml, XML_END, STRH("Project"));
	const xml_tag_t child	= xml_add_tag(&xml, project, STRH("Child"));

	xml_remove_tag(&xml, child);

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(xml_print(&xml, 0, file), 0);

		char buf[128] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				   "<Project />\n";
		EXPECT_STR(buf, exp);
	}

	xml_free(&xml);

	END;
}

TEST(t_xml_print_remove_tag_attr, FILE *file)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	const xml_tag_t project = xml_add_tag(&xml, XML_END, STRH("Project"));
	const xml_tag_t child	= xml_add_tag(&xml, project, STRH("Child"));
	xml_add_attr(&xml, child, STRH("Name"), STRH("Project3"));

	xml_remove_tag(&xml, child);

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(xml_print(&xml, 0, file), 0);

		char buf[128] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				   "<Project />\n";
		EXPECT_STR(buf, exp);
	}

	xml_free(&xml);

	END;
}

TEST(t_xml_print_remove_tag_middle, FILE *file)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	const xml_tag_t project = xml_add_tag(&xml, XML_END, STRH("Project"));
	const xml_tag_t child1	= xml_add_tag(&xml, project, STRH("Child1"));
	const xml_tag_t child2	= xml_add_tag(&xml, project, STRH("Child2"));
	const xml_tag_t child3	= xml_add_tag(&xml, project, STRH("Child3"));

	xml_remove_tag(&xml, child2);

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(xml_print(&xml, 0, file), 0);

		char buf[128] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				   "<Project>\n"
				   "  <Child1 />\n"
				   "  <Child3 />\n"
				   "</Project>\n";
		EXPECT_STR(buf, exp);
	}

	xml_free(&xml);

	END;
}

TEST(t_xml_prints, FILE *file)
{
	SSTART;
	RUN(t_xml_print, file);
	RUN(t_xml_print_tag, file);
	RUN(t_xml_print_tag_val, file);
	RUN(t_xml_print_tag_val_nl, file);
	RUN(t_xml_print_tag_attr, file);
	RUN(t_xml_print_tag_val_attr, file);
	RUN(t_xml_print_tag_child, file);
	RUN(t_xml_print_remove_tag, file);
	RUN(t_xml_print_remove_tag_attr, file);
	RUN(t_xml_print_remove_tag_middle, file);
	SEND;
}

STEST(t_xml)
{
	SSTART;

	FILE *file = file_open(TEST_FILE, "wb+");

	RUN(t_xml_init_free);
	RUN(t_xml_add);
	RUN(t_xml_prints, file);

	file_close(file);
	file_delete(TEST_FILE);

	SEND;
}
