#ifndef PRINT_H
#define PRINT_H

#include "type.h"

#include <stdarg.h>
#include <stdio.h>

int c_printv(const char *fmt, va_list args);
int c_printf(const char *fmt, ...);

int c_fprintv(FILE *file, const char *fmt, va_list args);
int c_fprintf(FILE *file, const char *fmt, ...);

int c_sprintv(char *buf, size_t size, const char *fmt, va_list args);
int c_sprintf(char *buf, size_t size, const char *fmt, ...);

int c_wprintv(const wchar *fmt, va_list args);
int c_wprintf(const wchar *fmt, ...);

int c_swprintv(wchar *buf, size_t size, const wchar *fmt, va_list args);
int c_swprintf(wchar *buf, size_t size, const wchar *fmt, ...);

int c_fflush(FILE *file);

int c_set_u16(FILE *file);
int c_unset_u16(FILE *file, int mode);

int c_ur(FILE *file);
int c_v(FILE *file);
int c_vr(FILE *file);

typedef int (*c_printv_cb)(void *priv, size_t size, int off, const char *fmt, va_list args);
int c_sprintv_cb(void *buf, size_t size, int off, const char *fmt, va_list args);
int c_fprintv_cb(void *file, size_t size, int off, const char *fmt, va_list args);

typedef int (*c_printf_cb)(void *priv, size_t size, int off, const char *fmt, ...);
int c_sprintf_cb(void *buf, size_t size, int off, const char *fmt, ...);
int c_fprintf_cb(void *file, size_t size, int off, const char *fmt, ...);

#endif
