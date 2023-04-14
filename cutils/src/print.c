#include "print.h"

#include "platform.h"

#include <fcntl.h>
#include <stdio.h>
#include <wchar.h>

#if defined(C_WIN)
	#include <io.h>
#endif

size_t p_fprintf(FILE *file, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	size_t ret = p_vfprintf(file, fmt, args);
	va_end(args);
	return ret;
}

size_t p_vfprintf(FILE *file, const char *fmt, va_list args)
{
	va_list copy;
	va_copy(copy, args);
	size_t ret;
#if defined(C_WIN)
	ret = vfprintf_s(file, fmt, copy);
#else
	ret = vfprintf(file, fmt, copy);
#endif
	va_end(copy);
	return ret;
}

size_t p_sprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	size_t ret = p_vsprintf(buf, size, fmt, args);
	va_end(args);
	return ret;
}

size_t p_vsprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	va_list copy;
	va_copy(copy, args);
	size_t ret;
#if defined(C_WIN)
	ret = vsprintf_s(buf, size, fmt, copy);
#else
	ret = vsprintf(buf, fmt, copy);
#endif
	va_end(copy);
	return ret;
}

size_t p_vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	va_list copy;
	va_copy(copy, args);
	size_t ret;
#if defined(C_WIN)
	ret = vsnprintf(buf, size, fmt, copy);
#else
	ret = vsnprintf(buf, size, fmt, copy);
#endif
	va_end(copy);
	return ret;
}

size_t p_swprintf(wchar *buf, size_t size, const wchar *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	size_t ret = p_vswprintf(buf, size, fmt, args);
	va_end(args);
	return ret;
}

size_t p_vswprintf(wchar *buf, size_t size, const wchar *fmt, va_list args)
{
	va_list copy;
	va_copy(copy, args);
	size_t ret;
#if defined(C_WIN)
	ret = vswprintf_s(buf, size, fmt, args);
#else
	ret = vswprintf(buf, size, fmt, args);
#endif
	va_end(copy);
	return ret;
}

int p_set_u16(FILE *file)
{
#if defined(C_WIN)
	return _setmode(_fileno(file), _O_U16TEXT);
#else
	return 0;
#endif
}

int p_unset_u16(FILE *file, int mode)
{
#if defined(C_WIN)
	return _setmode(_fileno(file), mode);
#else
	return 0;
#endif
}

void p_ur(FILE *file)
{
#if defined(C_WIN)
	int mode = p_set_u16(file);
	fwprintf_s(file, L"\u2514\u2500");
	p_unset_u16(file, mode);
#else
	fprintf(f, "└─");
#endif
}

void p_v(FILE *file)
{
#if defined(C_WIN)
	int mode = p_set_u16(file);
	fwprintf_s(file, L"\u2502 ");
	p_unset_u16(file, mode);
#else
	fprintf(f, "│ ");
#endif
}

void p_vr(FILE *file)
{
#if defined(C_WIN)
	int mode = p_set_u16(file);
	fwprintf_s(file, L"\u251C\u2500");
	p_unset_u16(file, mode);
#else
	fprintf(f, "├─");
#endif
}
