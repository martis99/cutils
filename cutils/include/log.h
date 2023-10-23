#ifndef LOG_H
#define LOG_H

#include "c_time.h"
#include "print.h"

typedef struct log_event_s {
	va_list ap;
	const char *pkg;
	const char *file;
	const char *func;
	const char *tag;
	const char *fmt;
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

#define log_trace(_pkg, _file, _tag, ...) log_log(LOG_TRACE, _pkg, _file, __func__, __LINE__, _tag, __VA_ARGS__)
#define log_debug(_pkg, _file, _tag, ...) log_log(LOG_DEBUG, _pkg, _file, __func__, __LINE__, _tag, __VA_ARGS__)
#define log_info(_pkg, _file, _tag, ...)  log_log(LOG_INFO, _pkg, _file, __func__, __LINE__, _tag, __VA_ARGS__)
#define log_warn(_pkg, _file, _tag, ...)  log_log(LOG_WARN, _pkg, _file, __func__, __LINE__, _tag, __VA_ARGS__)
#define log_error(_pkg, _file, _tag, ...) log_log(LOG_ERROR, _pkg, _file, __func__, __LINE__, _tag, __VA_ARGS__)
#define log_fatal(_pkg, _file, _tag, ...) log_log(LOG_FATAL, _pkg, _file, __func__, __LINE__, _tag, __VA_ARGS__)

log_t *log_init(log_t *log);
const log_t *log_get();

const char *log_level_str(int level);
int log_set_level(int level);
int log_set_quiet(int enable);
int log_add_callback(log_LogFn fn, void *udata, int level);
int log_add_fp(FILE *fp, int level);

int log_log(int level, const char *pkg, const char *file, const char *func, int line, const char *tag, const char *fmt, ...);

const char *log_strerror(int errnum);

#endif
