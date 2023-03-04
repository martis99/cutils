#ifndef CSTR_H
#define CSTR_H

#include <wchar.h>

unsigned int cstr_len(const char *str);
unsigned int cstrn_len(const char *str, unsigned int max_len);
int cstr_cmp(const char *str1, unsigned int str1_len, const char *str2, unsigned int str2_len);
int cstrn_cmp(const char *str1, unsigned int str1_len, const char *str2, unsigned int str2_len, unsigned int len);
void *cstr_cpy(char *dst, unsigned int dst_len, const char *src, unsigned int src_len);
int cstr_replace(const char *src, unsigned int src_len, char *dst, unsigned int dst_len, const char *from, unsigned int from_len, const char *to, unsigned int to_len);
int cstr_replaces(const char *src, unsigned int src_len, char *dst, unsigned int dst_len, const char *const *from, const char *const *to, unsigned int len);

void wcstrn_cat(wchar_t *dst, unsigned int size, const wchar_t *src, unsigned int cnt);

#endif
