#include "t_cutils_c.h"

#include "file.h"
#include "log.h"

TEST(t_log_init)
{
	START;

	const log_t *log = log_get();
	log_t tmp = *log;

	EXPECT_EQ(log_init(NULL), NULL);
	EXPECT_EQ(log_init(&tmp), &tmp);

	log_set((log_t *)log);

	END;
}

TEST(t_log_stdout)
{
	START;

	int quiet  = log_set_quiet(0);
	int level  = log_set_level(LOG_DEBUG);
	int header = log_set_header(1);

	log_debug("test", "log", NULL, "stdout test");
	log_debug("test", "log", "stdout", "stdout test");

	log_set_header(0);

	log_debug("test", "log", NULL, "stdout test");
	log_debug("test", "log", "stdout", "stdout test");

	log_set_quiet(quiet);
	log_set_level(level);
	log_set_header(header);

	END;
}

TEST(t_log_print_header)
{
	START;

	const log_t *log = log_get();

	log_t tmp = *log;
	log_set(&tmp);

	char buf[1024] = { 0 };
	log_add_print(c_sprintv_cb, sizeof(buf), 0, buf, LOG_DEBUG, 1);

	log_debug("test", "log", NULL, "file test %d", 1);
	uint exp_line = __LINE__ - 1;

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u DEBUG [test:log] t_log_print_header:%u: file test %u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	log_set((log_t *)log);

	END;
}

TEST(t_log_print_header_tag)
{
	START;

	const log_t *log = log_get();

	log_t tmp = *log;
	log_set(&tmp);

	char buf[1024] = { 0 };
	log_add_print(c_sprintv_cb, sizeof(buf), 0, buf, LOG_DEBUG, 1);

	log_debug("test", "log", "file", "file test %d", 2);
	uint exp_line = __LINE__ - 1;

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u DEBUG [test:log] t_log_print_header_tag:%u: [file] file test %u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 2);

	log_set((log_t *)log);

	END;
}

TEST(t_log_print_no_header)
{
	START;

	const log_t *log = log_get();

	log_t tmp = *log;
	log_set(&tmp);

	char buf[1024] = { 0 };
	log_add_print(c_sprintv_cb, sizeof(buf), 0, buf, LOG_DEBUG, 0);

	log_debug("test", "log", NULL, "file test %d", 3);

	uint x;

	EXPECT_FMT(buf, 1, "file test %u\n", &x);

	EXPECT_EQ(x, 3);

	log_set((log_t *)log);

	END;
}

TEST(t_log_print_no_header_tag)
{
	START;

	const log_t *log = log_get();

	log_t tmp = *log;
	log_set(&tmp);

	char buf[1024] = { 0 };
	log_add_print(c_sprintv_cb, sizeof(buf), 0, buf, LOG_DEBUG, 0);

	log_debug("test", "log", "file", "file test %d", 4);

	uint x;

	EXPECT_FMT(buf, 1, "[file] file test %u\n", &x);

	EXPECT_EQ(x, 4);

	log_set((log_t *)log);

	END;
}

TEST(t_log_print)
{
	SSTART;
	RUN(t_log_print_header);
	RUN(t_log_print_header_tag);
	RUN(t_log_print_no_header);
	RUN(t_log_print_no_header_tag);
	SEND;
}

TEST(t_log_level_str)
{
	START;

	EXPECT_STR(log_level_str(LOG_TRACE), "TRACE");
	EXPECT_STR(log_level_str(LOG_DEBUG), "DEBUG");

	END;
}

TEST(t_log_set_level)
{
	START;

	const log_t *log = log_get();

	log_set(NULL);
	EXPECT_EQ(log_set_level(LOG_WARN), LOG_WARN);

	log_t tmp = *log;
	log_set(&tmp);

	EXPECT_EQ(log_set_level(log_set_level(LOG_DEBUG)), LOG_DEBUG);

	log_set((log_t *)log);

	END;
}

TEST(t_log_set_quiet)
{
	START;

	const log_t *log = log_get();

	log_set(NULL);
	EXPECT_EQ(log_set_quiet(0), 0);

	log_t tmp = *log;
	log_set(&tmp);

	EXPECT_EQ(log_set_quiet(log_set_quiet(1)), 1);

	log_set((log_t *)log);

	END;
}

TEST(t_log_set_header)
{
	START;

	const log_t *log = log_get();

	log_set(NULL);
	EXPECT_EQ(log_set_header(1), 1);

	log_t tmp = *log;
	log_set(&tmp);

	log_set((log_t *)log);
	EXPECT_EQ(log_set_header(log_set_header(0)), 0);

	log_set((log_t *)log);

	END;
}

static int print_callback(log_event_t *ev)
{
	(void)ev;
	return 0;
}

TEST(t_log_add_callback)
{
	START;

	const log_t *log = log_get();

	log_set(NULL);
	EXPECT_EQ(log_add_callback(print_callback, NULL, LOG_TRACE, 1), 1);

	log_t tmp = *log;
	log_set(&tmp);

	for (int i = 0; i < LOG_MAX_CALLBACKS; i++) {
		EXPECT_EQ(log_add_callback(print_callback, NULL, LOG_TRACE, 1), 0);
	}

	EXPECT_EQ(log_add_callback(print_callback, NULL, LOG_TRACE, 1), 1);

	log_debug("test", "log", NULL, "trace");

	log_set((log_t *)log);

	END;
}

TEST(t_log_log)
{
	START;

	const log_t *log = log_get();

	log_set(NULL);
	EXPECT_EQ(log_log(LOG_TRACE, "test", "log", NULL, 0, NULL, NULL), 1);

	log_t tmp = *log;
	log_set(&tmp);

	EXPECT_EQ(log_log(LOG_TRACE, "test", "log", NULL, 0, NULL, NULL), 1);

	log_set((log_t *)log);

	END;
}

TEST(t_log_trace)
{
	START;

	const log_t *log = log_get();

	log_t tmp = *log;
	log_set(&tmp);

	char buf[1024] = { 0 };
	log_add_print(c_sprintv_cb, sizeof(buf), 0, buf, LOG_TRACE, 1);

	log_trace("test", "log", NULL, "trace%d", 1);
	uint exp_line = __LINE__ - 1;

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u TRACE [test:log] t_log_trace:%u: trace%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	log_set((log_t *)log);

	END;
}

TEST(t_log_debug)
{
	START;

	const log_t *log = log_get();

	log_t tmp = *log;
	log_set(&tmp);

	char buf[1024] = { 0 };
	log_add_print(c_sprintv_cb, sizeof(buf), 0, buf, LOG_DEBUG, 1);

	log_debug("test", "log", NULL, "debug%d", 1);
	uint exp_line = __LINE__ - 1;

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u DEBUG [test:log] t_log_debug:%u: debug%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	log_set((log_t *)log);

	END;
}

TEST(t_log_info)
{
	START;

	const log_t *log = log_get();

	log_t tmp = *log;
	log_set(&tmp);

	char buf[1024] = { 0 };
	log_add_print(c_sprintv_cb, sizeof(buf), 0, buf, LOG_INFO, 1);

	log_info("test", "log", NULL, "info%d", 1);
	uint exp_line = __LINE__ - 1;

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u INFO  [test:log] t_log_info:%u: info%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	log_set((log_t *)log);

	END;
}

TEST(t_log_warn)
{
	START;

	const log_t *log = log_get();

	log_t tmp = *log;
	log_set(&tmp);

	char buf[1024] = { 0 };
	log_add_print(c_sprintv_cb, sizeof(buf), 0, buf, LOG_WARN, 1);

	log_warn("test", "log", NULL, "warn%d", 1);
	uint exp_line = __LINE__ - 1;

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u WARN  [test:log] t_log_warn:%u: warn%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	log_set((log_t *)log);

	END;
}

TEST(t_log_error)
{
	START;

	const log_t *log = log_get();

	log_t tmp = *log;
	log_set(&tmp);

	char buf[1024] = { 0 };
	log_add_print(c_sprintv_cb, sizeof(buf), 0, buf, LOG_ERROR, 1);

	log_error("test", "log", NULL, "error%d", 1);
	uint exp_line = __LINE__ - 1;

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u ERROR [test:log] t_log_error:%u: error%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	log_set((log_t *)log);

	END;
}

TEST(t_log_fatal)
{
	START;

	const log_t *log = log_get();

	log_t tmp = *log;
	log_set(&tmp);

	char buf[1024] = { 0 };
	log_add_print(c_sprintv_cb, sizeof(buf), 0, buf, LOG_FATAL, 1);

	log_fatal("test", "log", NULL, "fatal%d", 1);
	uint exp_line = __LINE__ - 1;

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u FATAL [test:log] t_log_fatal:%u: fatal%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	log_set((log_t *)log);

	END;
}

STEST(t_log)
{
	SSTART;

	RUN(t_log_init);
	RUN(t_log_stdout);
	RUN(t_log_print);
	RUN(t_log_level_str);
	RUN(t_log_set_level);
	RUN(t_log_set_quiet);
	RUN(t_log_set_header);
	RUN(t_log_add_callback);
	RUN(t_log_log);
	RUN(t_log_trace);
	RUN(t_log_debug);
	RUN(t_log_info);
	RUN(t_log_warn);
	RUN(t_log_error);
	RUN(t_log_fatal);

	SEND;
}
