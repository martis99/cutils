#include "json.h"

#include "cstr.h"
#include "mem.h"
#include "test.h"

TEST(t_json_init_free)
{
	START;

	json_t json = { 0 };

	EXPECT_EQ(json_init(NULL, 0), NULL);
	mem_oom(1);
	EXPECT_EQ(json_init(&json, 1), NULL);
	mem_oom(0);
	EXPECT_EQ(json_init(&json, 0), &json);

	EXPECT_NE(json.values.data, NULL);

	json_free(&json);
	json_free(NULL);

	EXPECT_EQ(json.values.data, NULL);

	END;
}

TEST(t_json_add_val)
{
	START;

	json_t json = { 0 };
	json_init(&json, 0);

	EXPECT_EQ(json_add_val(NULL, JSON_END, str_null(), JSON_INT(0)), JSON_END);
	mem_oom(1);
	EXPECT_EQ(json_add_val(&json, JSON_END, str_null(), JSON_INT(0)), JSON_END);
	mem_oom(0);
	EXPECT_NE(json_add_val(&json, JSON_END, STRH(""), JSON_INT(0)), JSON_END);
	EXPECT_EQ(json_add_val(&json, 5, str_null(), JSON_INT(0)), JSON_END);
	EXPECT_EQ(json_add_val(&json, 0, str_null(), JSON_INT(0)), JSON_END);

	json_free(&json);

	END;
}

TEST(t_json_print)
{
	START;

	json_t json = { 0 };
	json_init(&json, 1);

	char buf[8] = { 0 };
	EXPECT_EQ(json_print(NULL, JSON_END, PRINT_DST_BUF(buf, sizeof(buf), 0), "\t"), 0);
	EXPECT_EQ(json_print(&json, JSON_END, PRINT_DST_BUF(buf, sizeof(buf), 0), "\t"), 0);

	json_free(&json);

	END;
}

TEST(t_json_print_all)
{
	START;

	json_t json = { 0 };
	json_init(&json, 1);

	const json_val_t root = json_add_val(&json, JSON_END, str_null(), JSON_OBJ());
	json_add_val(&json, root, STRH("int"), JSON_INT(1));
	json_add_val(&json, root, STRH("float"), JSON_FLOAT(2.1234567f));
	json_add_val(&json, root, STRH("double"), JSON_DOUBLE(3.123456789012345));
	json_add_val(&json, root, STRH("bool"), JSON_BOOL(1));
	json_add_val(&json, root, STRH("str"), JSON_STR(STRH("str")));
	const json_val_t arr = json_add_val(&json, root, STRH("arr"), JSON_ARR());
	json_add_val(&json, arr, str_null(), JSON_INT(0));
	json_add_val(&json, arr, str_null(), JSON_INT(1));
	json_add_val(&json, arr, str_null(), JSON_INT(2));
	json_add_val(&json, arr, str_null(), JSON_OBJ());
	json_add_val(&json, arr, str_null(), JSON_ARR());
	const json_val_t obj = json_add_val(&json, root, STRH("obj"), JSON_OBJ());
	json_add_val(&json, obj, STRH("int"), JSON_INT(4));

	char buf[256] = { 0 };
	EXPECT_EQ(json_print(&json, root, PRINT_DST_BUF(buf, sizeof(buf), 0), "\t"), 159);
	EXPECT_STR(buf, "{\n"
			"\t\"int\": 1,\n"
			"\t\"float\": 2.1234567,\n"
			"\t\"double\": 3.123456789012345,\n"
			"\t\"bool\": true,\n"
			"\t\"str\": \"str\",\n"
			"\t\"arr\": [\n"
			"\t\t0,\n"
			"\t\t1,\n"
			"\t\t2,\n"
			"\t\t{},\n"
			"\t\t[]\n"
			"\t],\n"
			"\t\"obj\": {\n"
			"\t\t\"int\": 4\n"
			"\t}\n"
			"}");

	json_add_val(&json, root, STRH("invalid"), (json_val_data_t){ .type = -1 });

	EXPECT_EQ(json_print(&json, root, PRINT_DST_BUF(buf, sizeof(buf), 0), "\t"), 161);

	json_free(&json);

	END;
}

TEST(t_json_prints)
{
	SSTART;
	RUN(t_json_print);
	RUN(t_json_print_all);
	SEND;
}

STEST(t_json)
{
	SSTART;

	RUN(t_json_init_free);
	RUN(t_json_add_val);
	RUN(t_json_prints);

	SEND;
}
