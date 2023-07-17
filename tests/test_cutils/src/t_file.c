#include "t_file.h"

#include "cstr.h"
#include "file.h"
#include "print.h"

#include "test.h"

#define TEST_FILE "test_file.txt"

TEST(file_open_close)
{
	START;

	FILE *file = file_open(TEST_FILE, "w+");

	EXPECT_NE(file, 0);

	const int ret = file_close(file);

	EXPECT_EQ(ret, 0);

	file_delete(TEST_FILE);

	END;
}

TEST(file_open_f_test)
{
	START;

	FILE *file = file_open_f("%s", "w+", TEST_FILE);

	EXPECT_NE(file, 0);

	file_close(file);

	file_delete(TEST_FILE);

	END;
}

TEST(file_exists_false)
{
	START;

	int ret = file_exists(TEST_FILE);

	EXPECT_EQ(ret, 0);

	END;
}

TEST(file_exists_true)
{
	START;

	FILE *file = file_open(TEST_FILE, "w+");

	const int ret = file_exists(TEST_FILE);

	EXPECT_EQ(ret, 1);

	file_close(file);
	file_delete(TEST_FILE);

	END;
}

TEST(file_delete_test)
{
	START;

	FILE *file = file_open(TEST_FILE, "w+");

	file_close(file);

	const int ret = file_delete(TEST_FILE);

	EXPECT_EQ(ret, 0);

	END;
}

TEST(file_size_test)
{
	START;

	FILE *file = file_open(TEST_FILE, "w+");

	c_fprintf(file, "Test");

	const size_t size = file_size(file);

	EXPECT_EQ(size, 4);

	file_close(file);

	file_delete(TEST_FILE);

	END;
}

TEST(file_read_test)
{
	START;

	FILE *file = file_open(TEST_FILE, "wb+");

	c_fprintf(file, "Test");

	char data[64] = { 0 };

	const size_t size = file_read(file, 10, data, sizeof(data));

	EXPECT_EQ(size, 4);
	EXPECT_STR(data, "Test");

	file_close(file);

	file_delete(TEST_FILE);

	END;
}

TEST(file_read_ft_test)
{
	START;

	FILE *file = file_open(TEST_FILE, "w+");

	c_fprintf(file, "Test");

	char data[64] = { 0 };

	size_t len = file_read_ft(file, data, sizeof(data));

	EXPECT_EQ(len, 4);
	EXPECT_STR(data, "Test");

	file_close(file);

	file_delete(TEST_FILE);

	END;
}

TEST(file_read_ft_r_test)
{
	START;

	FILE *file = file_open(TEST_FILE, "wb+");

	c_fprintf(file, "Test\r\nTest");

	char data[64] = { 0 };

	size_t len = file_read_ft(file, data, sizeof(data));

	EXPECT_EQ(len, 9);
	EXPECT_STR(data, "Test\nTest");

	file_close(file);

	file_delete(TEST_FILE);

	END;
}

TEST(folder)
{
	START;

	EXPECT_EQ(folder_exists_f("%s", "temp"), 0);

	folder_create_f("%s", "temp");

	EXPECT_EQ(folder_exists_f("%s", "temp"), 1);

	folder_delete_f("%s", "temp");

	EXPECT_EQ(folder_exists_f("%s", "temp"), 0);

	END;
}

STEST(file)
{
	SSTART;
	RUN(file_open_close);
	RUN(file_open_f_test);
	RUN(file_exists_false);
	RUN(file_exists_true);
	RUN(file_delete_test);
	RUN(file_size_test);
	RUN(file_read_test);
	RUN(file_read_ft_test);
	RUN(file_read_ft_r_test);
	RUN(folder);
	SEND;
}
