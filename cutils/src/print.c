#include "print.h"

#include "platform.h"

#include <fcntl.h>
#include <stdio.h>
#include <wchar.h>

#if defined(C_WIN)
	#include <io.h>
#else
	#include <locale.h>
#endif

size_t c_printv(const char *fmt, va_list args)
{
	if (fmt == NULL) {
		return 0;
	}

	va_list copy;
	va_copy(copy, args);
	size_t ret = vprintf(fmt, copy);
	va_end(copy);
	return ret;
}

size_t c_printf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	size_t ret = c_printv(fmt, args);
	va_end(args);
	return ret;
}

size_t c_fprintv(FILE *file, const char *fmt, va_list args)
{
	if (file == NULL || fmt == NULL) {
		return 0;
	}

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

size_t c_fprintf(FILE *file, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	size_t ret = c_fprintv(file, fmt, args);
	va_end(args);
	return ret;
}

size_t c_sprintv(char *buf, size_t size, const char *fmt, va_list args)
{
	if ((buf == NULL && size > 0) || fmt == NULL) {
		return 0;
	}

	va_list copy;
	va_copy(copy, args);
	size_t ret;
#if defined(C_WIN)
	ret = vsnprintf(buf, size / sizeof(char), fmt, copy);
#else
	ret = vsnprintf(buf, size / sizeof(char), fmt, copy);
#endif
	va_end(copy);

	if (size > 0 && ret > size) {
		return 0;
	}

	return ret;
}

size_t c_sprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	size_t ret = c_sprintv(buf, size, fmt, args);
	va_end(args);
	return ret;
}

size_t c_wprintv(const wchar *fmt, va_list args)
{
	if (fmt == NULL) {
		return 0;
	}

	va_list copy;
	va_copy(copy, args);
	size_t ret = vwprintf(fmt, copy);
	if (ret == -1) {
		ret = 0;
	}
	va_end(copy);
	return ret;
}

size_t c_wprintf(const wchar *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	size_t ret = c_wprintv(fmt, args);
	va_end(args);
	return ret;
}

size_t c_swprintv(wchar *buf, size_t size, const wchar *fmt, va_list args)
{
	if ((buf == NULL && size > 0) || fmt == NULL) {
		return 0;
	}

	va_list copy;
	va_copy(copy, args);
	size_t ret;
#if defined(C_WIN)
	ret = vswprintf_s(buf, size / sizeof(wchar_t), fmt, copy);
#else
	ret = vswprintf(buf, size / sizeof(wchar_t), fmt, copy);
#endif
	va_end(copy);
	return ret;
}

size_t c_swprintf(wchar *buf, size_t size, const wchar *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	size_t ret = c_swprintv(buf, size, fmt, args);
	va_end(args);
	return ret;
}

int c_set_u16(FILE *file)
{
	if (file == NULL) {
		return 0;
	}

#if defined(C_WIN)
	fflush(file);
	return _setmode(_fileno(file), _O_U16TEXT);
#else
	return 0;
#endif
}

int c_unset_u16(FILE *file, int mode)
{
	if (file == NULL) {
		return 0;
	}

#if defined(C_WIN)
	fflush(file);
	return _setmode(_fileno(file), mode);
#else
	return 0;
#endif
}

int c_ur(FILE *file)
{
	if (file == NULL) {
		return 0;
	}

	int ret;

#if defined(C_WIN)
	if (file == stdout || file == stderr) {
		int mode = c_set_u16(file);
		ret	 = fwprintf_s(file, L"\u2514\u2500");
		c_unset_u16(file, mode);
	} else {
		ret = fprintf_s(file, "└─");
	}
#else
	ret = fprintf(file, "└─");
#endif
	return ret;
}

int c_v(FILE *file)
{
	if (file == NULL) {
		return 0;
	}

	int ret;

#if defined(C_WIN)
	if (file == stdout || file == stderr) {
		int mode = c_set_u16(file);
		ret	 = fwprintf_s(file, L"\u2502 ");
		c_unset_u16(file, mode);
	} else {
		ret = fprintf_s(file, "│ ");
	}
#else
	ret = fprintf(file, "│ ");
#endif
	return ret;
}

int c_vr(FILE *file)
{
	if (file == NULL) {
		return 0;
	}

	int ret;

#if defined(C_WIN)
	if (file == stdout || file == stderr) {
		int mode = c_set_u16(file);
		ret	 = fwprintf_s(file, L"\u251C\u2500");
		c_unset_u16(file, mode);
	} else {
		ret = fprintf_s(file, "├─");
	}
#else
	ret = fprintf(file, "├─");
#endif
	return ret;
}
