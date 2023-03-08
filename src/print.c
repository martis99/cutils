#include "print.h"

#include "platform.h"

int p_fprintf(FILE *f, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret;
#if defined(P_WIN)
	ret = vfprintf_s(f, fmt, args);
#else
	ret = vfprintf(f, fmt, args);
#endif
	va_end(args);
	return ret;
}

int p_sprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = p_vsprintf(buf, size, fmt, args);
	va_end(args);
	return ret;
}

int p_vsprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	va_list copy;
	va_copy(copy, args);
	int ret;
#if defined(P_WIN)
	ret = vsprintf_s(buf, size, fmt, args);
#else
	ret = vsprintf(buf, fmt, args);
#endif
	va_end(copy);
	return ret;
}

int p_vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	va_list copy;
	va_copy(copy, args);
	int ret;
#if defined(P_WIN)
	ret = vsnprintf(buf, size, fmt, copy);
#else
	ret = vsnprintf(buf, size, fmt, copy);
#endif
	va_end(copy);
	return ret;
}

int p_swprintf(wchar_t *buf, size_t size, const wchar_t *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = p_vswprintf(buf, size, fmt, args);
	va_end(args);
	return ret;
}

int p_vswprintf(wchar_t *buf, size_t size, const wchar_t *fmt, va_list args)
{
	va_list copy;
	va_copy(copy, args);
	int ret;
#if defined(P_WIN)
	ret = vswprintf_s(buf, size, fmt, args);
#else
	ret = vswprintf(buf, size, fmt, args);
#endif
	va_end(copy);
	return ret;
}
