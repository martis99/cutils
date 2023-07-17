#ifndef STR_H
#define STR_H

#include "type.h"

#include <stdarg.h>

typedef struct str_s {
	const char *data;
	size_t size;
	size_t len;
	bool ref;
} str_t;

str_t str_init(size_t size);
str_t str_cstr(const char *cstr, size_t len);
str_t str_cstrn(const char *cstr, size_t len, size_t size);
str_t str_cstrv(const char *fmt, va_list args);
str_t str_cstrf(const char *fmt, ...);
str_t str_buf(const char *buf, size_t size, size_t len);
str_t str_ref();

void str_free(str_t *str);

int str_resize(str_t *str, size_t size);

str_t *str_catc(str_t *str, const char *cstr, size_t len);
str_t *str_catn(str_t *str, str_t src, size_t len);
str_t *str_cat(str_t *str, str_t src);

int str_cmpnc(str_t str, const char *cstr, size_t cstr_len, size_t len);
int str_cmpc(str_t str, const char *cstr, size_t cstr_len);
int str_cmpn(str_t str, str_t src, size_t len);
int str_cmp(str_t str, str_t src);

int str_eqc(str_t str, const char *cstr, size_t cstr_len);
int str_eq(str_t str, str_t s);

str_t str_cpy(str_t src);

int str_split(str_t str, char c, str_t *l, str_t *r);
int str_rsplit(str_t str, char c, str_t *l, str_t *r);

// clang-format off
#define STR(_str) { .data = _str, .len = sizeof(_str) - 1 }
// clang-format on
#endif
