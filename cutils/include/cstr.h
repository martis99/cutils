#ifndef CSTR_H
#define CSTR_H

#include "type.h"

size_t cstr_len(const char *str);
int cstr_cmp(const char *str1, size_t str1_len, const char *str2, size_t str2_len);
int cstrn_cmp(const char *str1, size_t str1_len, const char *str2, size_t str2_len, size_t len);
void *cstr_cpy(char *dst, size_t dst_len, const char *src, size_t src_len);
char *cstr_chr(const char *str, char c);
char *cstr_rchr(const char *str, char c);
char *cstr_str(const char *str, const char *s);

size_t cstr_replace(const char *src, size_t src_len, char *dst, size_t dst_len, const char *from, size_t from_len, const char *to, size_t to_len);
size_t cstr_replaces(const char *src, size_t src_len, char *dst, size_t dst_len, const char *const *from, const char *const *to, size_t len);
size_t cstr_inplace(char *str, size_t str_size, size_t str_len, const char *old, size_t old_len, const char *new, size_t new_len);
size_t cstr_inplaces(char *str, size_t str_size, size_t str_len, const char *const *old, const char *const *new, size_t cnt);

void wcstrn_cat(wchar_t *dst, size_t size, const wchar_t *src, size_t cnt);

#define CSTR(_str) _str, sizeof(_str) - 1

#endif
