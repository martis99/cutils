#include "c_time.h"

#include "platform.h"
#include "print.h"

#include <stdio.h>
#include <time.h>

#if defined(C_WIN)
	#define gmtime(_tm, _time) gmtime_s(_tm, _time)
#else
	#include <sys/time.h>
	#include <unistd.h>
	#define gmtime(_tm, _time) gmtime_r(_time, _tm)
#endif

static struct timespec get_time()
{
#if defined(C_WIN)
	SYSTEMTIME st;
	GetSystemTime(&st);
	const struct timespec now = {
		.tv_sec	 = time(NULL),
		.tv_nsec = st.wMilliseconds * 1000,
	};
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	const struct timespec now = {
		.tv_sec	 = tv.tv_sec,
		.tv_nsec = tv.tv_usec,
	};
#endif
	return now;
}

u64 c_time()
{
	const struct timespec now = get_time();
	return now.tv_sec * 1000 + (u64)now.tv_nsec / 1000;
}

const char *c_time_str(char *buf)
{
	struct tm timeinfo;
	const struct timespec now = get_time();

	gmtime(&timeinfo, &now.tv_sec);
	strftime(buf, C_TIME_BUF_SIZE, "%Y-%m-%d %H:%M:%S", &timeinfo);
	p_sprintf(buf + 19, C_TIME_BUF_SIZE - 19, ".%03ld", now.tv_nsec / 1000);

	return buf;
}

int c_sleep(u32 milliseconds)
{
#if defined(C_WIN)
	Sleep((DWORD)milliseconds);
	return 0;
#else
	return usleep(milliseconds * 1000);
#endif
}
