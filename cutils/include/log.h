#ifndef LOG_H
#define LOG_H

#include "c_time.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define LOG_VERSION "0.1.0"

typedef struct log_event_s {
	va_list ap;
	const char *fmt;
	const char *file;
	char time[C_TIME_BUF_SIZE];
	void *udata;
	int line;
	int level;
} log_event_t;

typedef void (*log_LogFn)(log_event_t *ev);

typedef struct callback_s {
	log_LogFn fn;
	void *udata;
	int level;
} callback_t;

#define LOG_MAX_CALLBACKS 32

typedef struct log_s {
	void *udata;
	int level;
	int quiet;
	callback_t callbacks[LOG_MAX_CALLBACKS];
} log_t;

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define log_trace(...) log_log(LOG_TRACE, __func__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __func__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO, __func__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(LOG_WARN, __func__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __func__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __func__, __LINE__, __VA_ARGS__)

void log_init(log_t *log);

const char *log_level_str(int level);
void log_set_level(int level);
void log_set_quiet(int enable);
int log_add_callback(log_LogFn fn, void *udata, int level);
int log_add_fp(FILE *fp, int level);

void log_log(int level, const char *file, int line, const char *fmt, ...);

#endif
