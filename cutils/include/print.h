#ifndef PRINT_H
#define PRINT_H

#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>

int p_fprintf(FILE *f, const char *fmt, ...);

int p_sprintf(char *buf, size_t size, const char *fmt, ...);
int p_vsprintf(char *buf, size_t size, const char *fmt, va_list args);
int p_vsnprintf(char *buf, size_t size, const char *fmt, va_list args);

int p_swprintf(wchar_t *buf, size_t size, const wchar_t *fmt, ...);
int p_vswprintf(wchar_t *buf, size_t size, const wchar_t *fmt, va_list args);

#endif
