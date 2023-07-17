#ifndef PRINT_H
#define PRINT_H

#include "type.h"

#include <stdarg.h>
#include <stdio.h>

size_t c_printv(const char *fmt, va_list args);
size_t c_printf(const char *fmt, ...);

size_t c_fprintv(FILE *file, const char *fmt, va_list args);
size_t c_fprintf(FILE *file, const char *fmt, ...);

size_t c_sprintv(char *buf, size_t size, const char *fmt, va_list args);
size_t c_sprintf(char *buf, size_t size, const char *fmt, ...);

size_t c_wprintv(const wchar *fmt, va_list args);
size_t c_wprintf(const wchar *fmt, ...);

size_t c_swprintv(wchar *buf, size_t size, const wchar *fmt, va_list args);
size_t c_swprintf(wchar *buf, size_t size, const wchar *fmt, ...);

int c_set_u16(FILE *file);
int c_unset_u16(FILE *file, int mode);

void c_ur(FILE *file);
void c_v(FILE *file);
void c_vr(FILE *file);

#endif
