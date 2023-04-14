#ifndef PRINT_H
#define PRINT_H

#include "type.h"

#include <stdarg.h>
#include <stdio.h>

size_t p_fprintf(FILE *file, const char *fmt, ...);
size_t p_vfprintf(FILE *file, const char *fmt, va_list args);

size_t p_sprintf(char *buf, size_t size, const char *fmt, ...);
size_t p_vsprintf(char *buf, size_t size, const char *fmt, va_list args);
size_t p_vsnprintf(char *buf, size_t size, const char *fmt, va_list args);

size_t p_swprintf(wchar *buf, size_t size, const wchar *fmt, ...);
size_t p_vswprintf(wchar *buf, size_t size, const wchar *fmt, va_list args);

int p_set_u16(FILE *file);
int p_unset_u16(FILE *file, int mode);

void p_ur(FILE *file);
void p_v(FILE *file);
void p_vr(FILE *file);

#endif
