#ifndef PRINT_H
#define PRINT_H

#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>

int p_fprintf(FILE *file, const char *fmt, ...);
int p_vfprintf(FILE *file, const char *fmt, va_list args);

int p_sprintf(char *buf, size_t size, const char *fmt, ...);
int p_vsprintf(char *buf, size_t size, const char *fmt, va_list args);
int p_vsnprintf(char *buf, size_t size, const char *fmt, va_list args);

int p_swprintf(wchar_t *buf, size_t size, const wchar_t *fmt, ...);
int p_vswprintf(wchar_t *buf, size_t size, const wchar_t *fmt, va_list args);

int p_set_u16(FILE *file);
int p_unset_u16(FILE *file, int mode);

void p_ur(FILE *file);
void p_v(FILE *file);
void p_vr(FILE *file);

#endif
