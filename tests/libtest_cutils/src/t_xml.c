#include "t_cutils_c.h"

#include "cstr.h"
#include "mem.h"
#include "xml.h"

TEST(t_xml_init_free)
{
	START;

	xml_t xml = { 0 };

	EXPECT_EQ(xml_init(NULL, 0, 0), NULL);
	mem_soom(0);
	EXPECT_EQ(xml_init(&xml, 1, 0), NULL);
	EXPECT_EQ(xml_init(&xml, 0, 1), NULL);
	mem_eoom();
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
	mem_soom(0);
	EXPECT_EQ(xml_add_tag_val(&xml, XML_END, STRH(""), STRH("")), XML_END);
	mem_eoom();
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
	mem_soom(0);
	EXPECT_EQ(xml_add_attr(&xml, xml_add_tag(&xml, XML_END, STRH("")), STRH(""), STRH("")), LIST_END);
	mem_eoom();
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

TEST(t_xml_print)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	char buf[64] = { 0 };
	EXPECT_EQ(xml_print(NULL, XML_END, PRINT_DST_BUF(buf, sizeof(buf), 0)), 39);
	EXPECT_EQ(xml_print(&xml, XML_END, PRINT_DST_BUF(buf, sizeof(buf), 0)), 39);

	xml_tag_t tag = xml_add_tag(&xml, XML_END, STRH(""));
	EXPECT_EQ(xml_print(&xml, tag, PRINT_DST_BUF(buf, sizeof(buf), 0)), 44);

	EXPECT_STR(buf, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			"< />\n");

	xml_free(&xml);

	END;
}

TEST(t_xml_print_tag)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	xml_add_tag(&xml, XML_END, STRH("Project"));

	char buf[64] = { 0 };
	EXPECT_EQ(xml_print(&xml, 0, PRINT_DST_BUF(buf, sizeof(buf), 0)), 51);

	EXPECT_STR(buf, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			"<Project />\n");

	xml_free(&xml);

	END;
}

TEST(t_xml_print_tag_val)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	xml_add_tag_val(&xml, XML_END, STRH("Project"), STRH("Name"));

	char buf[128] = { 0 };
	EXPECT_EQ(xml_print(&xml, 0, PRINT_DST_BUF(buf, sizeof(buf), 0)), 63);

	EXPECT_STR(buf, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			"<Project>Name</Project>\n");

	xml_free(&xml);

	END;
}

TEST(t_xml_print_tag_val_nl)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	xml_add_tag_val(&xml, XML_END, STRH("Project"), STRH("Name\n"));

	char buf[128] = { 0 };
	EXPECT_EQ(xml_print(&xml, 0, PRINT_DST_BUF(buf, sizeof(buf), 0)), 64);

	EXPECT_STR(buf, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			"<Project>Name\n"
			"</Project>\n");

	xml_free(&xml);

	END;
}

TEST(t_xml_print_tag_attr)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	const xml_tag_t project = xml_add_tag(&xml, XML_END, STRH("Project"));
	xml_add_attr(&xml, project, STRH("Name"), STRH("Project1"));

	char buf[128] = { 0 };
	EXPECT_EQ(xml_print(&xml, 0, PRINT_DST_BUF(buf, sizeof(buf), 0)), 67);

	EXPECT_STR(buf, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			"<Project Name=\"Project1\" />\n");

	xml_free(&xml);

	END;
}

TEST(t_xml_print_tag_val_attr)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	const xml_tag_t project = xml_add_tag_val(&xml, XML_END, STRH("Project"), STRH("Value"));
	xml_add_attr(&xml, project, STRH("Name"), STRH("Project1"));

	char buf[128] = { 0 };
	EXPECT_EQ(xml_print(&xml, 0, PRINT_DST_BUF(buf, sizeof(buf), 0)), 80);

	EXPECT_STR(buf, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			"<Project Name=\"Project1\">Value</Project>\n");

	xml_free(&xml);

	END;
}

TEST(t_xml_print_tag_child)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	const xml_tag_t project = xml_add_tag(&xml, XML_END, STRH("Project"));
	xml_add_tag(&xml, project, STRH("Child"));

	char buf[128] = { 0 };
	EXPECT_EQ(xml_print(&xml, 0, PRINT_DST_BUF(buf, sizeof(buf), 0)), 72);

	EXPECT_STR(buf, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			"<Project>\n"
			"  <Child />\n"
			"</Project>\n");

	xml_free(&xml);

	END;
}

TEST(t_xml_print_remove_tag)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	const xml_tag_t project = xml_add_tag(&xml, XML_END, STRH("Project"));
	const xml_tag_t child	= xml_add_tag(&xml, project, STRH("Child"));

	xml_remove_tag(&xml, child);

	char buf[64] = { 0 };
	EXPECT_EQ(xml_print(&xml, 0, PRINT_DST_BUF(buf, sizeof(buf), 0)), 51);

	EXPECT_STR(buf, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			"<Project />\n");

	xml_free(&xml);

	END;
}

TEST(t_xml_print_remove_tag_attr)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	const xml_tag_t project = xml_add_tag(&xml, XML_END, STRH("Project"));
	const xml_tag_t child	= xml_add_tag(&xml, project, STRH("Child"));
	xml_add_attr(&xml, child, STRH("Name"), STRH("Project3"));

	xml_remove_tag(&xml, child);

	char buf[64] = { 0 };
	EXPECT_EQ(xml_print(&xml, 0, PRINT_DST_BUF(buf, sizeof(buf), 0)), 51);

	EXPECT_STR(buf, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			"<Project />\n");

	xml_free(&xml);

	END;
}

TEST(t_xml_print_remove_tag_middle)
{
	START;

	xml_t xml = { 0 };
	xml_init(&xml, 1, 1);

	const xml_tag_t project = xml_add_tag(&xml, XML_END, STRH("Project"));
	xml_add_tag(&xml, project, STRH("Child1"));
	const xml_tag_t child2 = xml_add_tag(&xml, project, STRH("Child2"));
	xml_add_tag(&xml, project, STRH("Child3"));

	xml_remove_tag(&xml, child2);

	char buf[128] = { 0 };
	EXPECT_EQ(xml_print(&xml, 0, PRINT_DST_BUF(buf, sizeof(buf), 0)), 86);

	EXPECT_STR(buf, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			"<Project>\n"
			"  <Child1 />\n"
			"  <Child3 />\n"
			"</Project>\n");

	xml_free(&xml);

	END;
}

TEST(t_xml_prints)
{
	SSTART;
	RUN(t_xml_print);
	RUN(t_xml_print_tag);
	RUN(t_xml_print_tag_val);
	RUN(t_xml_print_tag_val_nl);
	RUN(t_xml_print_tag_attr);
	RUN(t_xml_print_tag_val_attr);
	RUN(t_xml_print_tag_child);
	RUN(t_xml_print_remove_tag);
	RUN(t_xml_print_remove_tag_attr);
	RUN(t_xml_print_remove_tag_middle);
	SEND;
}

STEST(t_xml)
{
	SSTART;

	RUN(t_xml_init_free);
	RUN(t_xml_add);
	RUN(t_xml_prints);

	SEND;
}
