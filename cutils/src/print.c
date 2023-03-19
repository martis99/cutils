#include "print.h"

#include "platform.h"

#include <stdio.h>
#include <fcntl.h>
#include <wchar.h>

#if defined(P_WIN)
	#include <io.h>
#endif

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

int p_set_u16(FILE *f)
{
#if defined(P_WIN)
	return _setmode(_fileno(f), _O_U16TEXT);
#else
	return 0;
#endif
}

int p_unset_u16(FILE *f, int mode)
{
#if defined(P_WIN)
	return _setmode(_fileno(f), mode);
#else
	return 0;
#endif
}

void p_ur(FILE *f)
{
#if defined(P_WIN)
	int mode = p_set_u16(f);
	fwprintf_s(f, L"\u2514\u2500");
	p_unset_u16(f, mode);
#else
	fprintf(f, "└─");
#endif

}

void p_v(FILE *f)
{
#if defined(P_WIN)
	int mode = p_set_u16(f);
	fwprintf_s(f, L"\u2502 ");
	p_unset_u16(f, mode);
#else
	fprintf(f, "│ ");
#endif
}

void p_vr(FILE *f)
{
#if defined(P_WIN)
	int mode = p_set_u16(f);
	fwprintf_s(f, L"\u251C\u2500");
	p_unset_u16(f, mode);
#else
	fprintf(f, "├─");
#endif
}
