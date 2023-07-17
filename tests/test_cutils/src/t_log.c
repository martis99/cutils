#include "t_time.h"

#include "file.h"
#include "log.h"

#include "test.h"

#define TEST_FILE "test_log.log"

TEST(log_trace_test, FILE *file)
{
	START;

	file_reopen(TEST_FILE, "wb+", file);

	log_trace("trace%d", 1);
	uint exp_line = __LINE__ - 1;

	char buf[1024] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u TRACE log_trace_test:%u: trace%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	END;
}

TEST(log_debug_test, FILE *file)
{
	START;

	file_reopen(TEST_FILE, "wb+", file);

	log_debug("debug%d", 1);
	uint exp_line = __LINE__ - 1;

	char buf[1024] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u DEBUG log_debug_test:%u: debug%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	END;
}

TEST(log_info_test, FILE *file)
{
	START;

	file_reopen(TEST_FILE, "wb+", file);

	log_info("info%d", 1);
	uint exp_line = __LINE__ - 1;

	char buf[1024] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u INFO  log_info_test:%u: info%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	END;
}

TEST(log_warn_test, FILE *file)
{
	START;

	file_reopen(TEST_FILE, "wb+", file);

	log_warn("warn%d", 1);
	uint exp_line = __LINE__ - 1;

	char buf[1024] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u WARN  log_warn_test:%u: warn%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	END;
}

TEST(log_error_test, FILE *file)
{
	START;

	file_reopen(TEST_FILE, "wb+", file);

	log_error("error%d", 1);
	uint exp_line = __LINE__ - 1;

	char buf[1024] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u ERROR log_error_test:%u: error%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	END;
}

TEST(log_fatal_test, FILE *file)
{
	START;

	file_reopen(TEST_FILE, "wb+", file);

	log_fatal("fatal%d", 1);
	uint exp_line = __LINE__ - 1;

	char buf[1024] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u FATAL log_fatal_test:%u: fatal%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	END;
}

STEST(t_log)
{
	SSTART;

	log_t lg = { 0 };
	log_init(&lg);

	log_set_quiet(1);

	FILE *file = file_open(TEST_FILE, "wb+");
	log_add_fp(file, 0);

	RUN(log_trace_test, file);
	RUN(log_debug_test, file);
	RUN(log_info_test, file);
	RUN(log_warn_test, file);
	RUN(log_error_test, file);
	RUN(log_fatal_test, file);

	file_close(file);

	file_delete(TEST_FILE);

	SEND;
}
