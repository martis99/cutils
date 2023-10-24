#include "t_cutils_c.h"

#include "file.h"
#include "log.h"

#define TEST_FILE "t_log.txt"

TESTP(t_log_init, log_t *log)
{
	START;

	EXPECT_EQ(log_init(NULL), NULL);
	EXPECT_EQ(log_init(log), log);

	END;
}

TESTP(t_log_stdout, int quiet, int level)
{
	START;

	log_set_quiet(0);
	log_set_level(LOG_DEBUG);

	log_debug("test", "log", NULL, "t_log_stdout");
	log_debug("test", "log", "stdout", "t_log_stdout");

	log_set_quiet(quiet);
	log_set_level(level);

	END;
}

TEST(t_log_level_str)
{
	START;

	EXPECT_STR(log_level_str(LOG_TRACE), "TRACE");
	EXPECT_STR(log_level_str(LOG_DEBUG), "DEBUG");

	END;
}

TESTP(t_log_set_level, log_t *log)
{
	START;

	log_init(NULL);
	EXPECT_EQ(log_set_level(log->level), 1);

	log_init(log);
	EXPECT_EQ(log_set_level(log->level), 0);

	END;
}

TESTP(t_log_set_quiet, log_t *log)
{
	START;

	log_init(NULL);
	EXPECT_EQ(log_set_quiet(log->quiet), 1);

	log_init(log);
	EXPECT_EQ(log_set_quiet(log->quiet), 0);

	END;
}

static void file_callback(log_event_t *ev)
{
	(void)ev;
}

TESTP(t_log_add_callback, log_t *log)
{
	START;

	log_init(NULL);
	EXPECT_EQ(log_add_callback(file_callback, NULL, LOG_TRACE), 1);

	log_init(log);
	for (int i = 0; i < 31; i++) {
		EXPECT_EQ(log_add_callback(file_callback, NULL, LOG_TRACE), 0);
	}

	EXPECT_EQ(log_add_callback(file_callback, NULL, LOG_TRACE), 1);

	END;
}

TESTP(t_log_log, log_t *log)
{
	START;

	log_init(NULL);
	EXPECT_EQ(log_log(LOG_TRACE, "test", "log", NULL, 0, NULL, NULL), 1);

	log_init(log);
	EXPECT_EQ(log_log(LOG_TRACE, "test", "log", NULL, 0, NULL, NULL), 1);

	END;
}

TESTP(t_log_trace, FILE *file)
{
	START;

	file_reopen(TEST_FILE, "wb+", file);

	log_trace("test", "log", NULL, "trace%d", 1);
	uint exp_line = __LINE__ - 1;

	char buf[1024] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u TRACE [test:log] t_log_trace:%u: trace%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	END;
}

TESTP(t_log_debug, FILE *file)
{
	START;

	file_reopen(TEST_FILE, "wb+", file);

	log_debug("test", "log", NULL, "debug%d", 1);
	uint exp_line = __LINE__ - 1;

	char buf[1024] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u DEBUG [test:log] t_log_debug:%u: debug%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	END;
}

TESTP(t_log_info, FILE *file)
{
	START;

	file_reopen(TEST_FILE, "wb+", file);

	log_info("test", "log", NULL, "info%d", 1);
	uint exp_line = __LINE__ - 1;

	char buf[1024] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u INFO  [test:log] t_log_info:%u: info%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	END;
}

TESTP(t_log_warn, FILE *file)
{
	START;

	file_reopen(TEST_FILE, "wb+", file);

	log_warn("test", "log", NULL, "warn%d", 1);
	uint exp_line = __LINE__ - 1;

	char buf[1024] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u WARN  [test:log] t_log_warn:%u: warn%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	END;
}

TESTP(t_log_error, FILE *file)
{
	START;

	file_reopen(TEST_FILE, "wb+", file);

	log_error("test", "log", NULL, "error%d", 1);
	uint exp_line = __LINE__ - 1;

	char buf[1024] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u ERROR [test:log] t_log_error:%u: error%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	END;
}

TESTP(t_log_fatal, FILE *file)
{
	START;

	file_reopen(TEST_FILE, "wb+", file);

	log_fatal("test", "log", NULL, "fatal%d", 1);
	uint exp_line = __LINE__ - 1;

	char buf[1024] = { 0 };

	file_read_ft(file, buf, sizeof(buf));

	uint y, m, d, H, M, S, U, line, x;

	EXPECT_FMT(buf, 9, "%4u-%2u-%2u %2u:%2u:%2u.%3u FATAL [test:log] t_log_fatal:%u: fatal%u\n", &y, &m, &d, &H, &M, &S, &U, &line, &x);

	EXPECT_EQ(line, exp_line);
	EXPECT_EQ(x, 1);

	END;
}

STEST(t_log)
{
	SSTART;

	const log_t *log = log_get();

	log_t lg = { 0 };
	log_init(&lg);

	log_set_quiet(1);

	FILE *file = file_open(TEST_FILE, "wb+");
	log_add_fp(file, 0);

	RUNP(t_log_init, &lg);
	RUNP(t_log_stdout, lg.quiet, lg.level);
	RUN(t_log_level_str);
	RUNP(t_log_set_level, &lg);
	RUNP(t_log_set_quiet, &lg);
	RUNP(t_log_add_callback, &lg);
	RUNP(t_log_log, &lg);
	RUNP(t_log_trace, file);
	RUNP(t_log_debug, file);
	RUNP(t_log_info, file);
	RUNP(t_log_warn, file);
	RUNP(t_log_error, file);
	RUNP(t_log_fatal, file);

	file_close(file);
	file_delete(TEST_FILE);

	log_init((log_t *)log);

	SEND;
}
