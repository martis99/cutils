#include "cstr.h"

#include "mem.h"
#include "platform.h"
#include "print.h"

#include <string.h>

size_t cstrv(char *cstr, size_t size, const char *fmt, va_list args)
{
	return c_sprintv(cstr, size, fmt, args);
}

size_t cstrf(char *cstr, size_t size, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	size_t ret = cstrv(cstr, size, fmt, args);
	va_end(args);
	return ret;
}

void cstr_zero(char *cstr, size_t size)
{
	m_memset(cstr, 0, size);
}

size_t cstr_len(const char *cstr)
{
	if (cstr == NULL) {
		return 0;
	}

	return strlen(cstr);
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

size_t cstr_catn(char *cstr, size_t size, size_t cstr_len, const char *src, size_t src_len, size_t len)
{
	if (cstr == NULL) {
		return 0;
	}

	if (src == NULL) {
		return cstr_len;
	}

	size_t cat_len = MIN(src_len, len);

	if (cstr_len + cat_len > size) {
		return cstr_len;
	}

#if defined(C_WIN)
	strncat_s(cstr, size, src, cat_len);
#else
	strncat(cstr, src, cat_len);
#endif
	return cstr_len + cat_len;
}

size_t cstr_cat(char *cstr, size_t size, size_t cstr_len, const char *src, size_t src_len)
{
	return cstr_catn(cstr, size, cstr_len, src, src_len, src_len);
}

int cstr_cmpn(const char *cstr, size_t cstr_len, const char *src, size_t src_len, size_t len)
{
	if (cstr == NULL && src == NULL) {
		return 0;
	} else if (cstr == NULL) {
		return -1;
	} else if (src == NULL) {
		return 1;
	}

	int ret = m_memcmp(cstr, src, MIN(MIN(cstr_len, src_len), len));

	if (ret != 0 || (cstr_len >= len && src_len >= len)) {
		return ret;
	}

	return cstr_len < src_len ? -1 : 1;
}

int cstr_cmp(const char *cstr, size_t cstr_len, const char *src, size_t src_len)
{
	return cstr_cmpn(cstr, cstr_len, src, src_len, MAX(cstr_len, src_len));
}

int cstr_eq(const char *cstr, size_t cstr_len, const char *src, size_t src_len)
{
	return cstr_cmp(cstr, cstr_len, src, src_len) == 0;
}

char *cstr_chr(const char *cstr, char c)
{
	if (cstr == NULL) {
		return NULL;
	}

	return strchr(cstr, c);
}

char *cstr_rchr(const char *cstr, char c)
{
	if (cstr == NULL) {
		return NULL;
	}

	return strrchr(cstr, c);
}

char *cstr_cstr(const char *cstr, const char *src)
{
	if (cstr == NULL || src == NULL) {
		return NULL;
	}

	return strstr(cstr, src);
}

void *cstr_cpy(char *cstr, size_t size, const char *src, size_t len)
{
	if (cstr == NULL || src == NULL) {
		return NULL;
	}

	if (len > size) {
		return NULL;
	}

	return m_memcpy(cstr, size, src, len * sizeof(char));
}

size_t cstr_replace(char *str, size_t str_size, size_t str_len, const char *old, size_t old_len, const char *new, size_t new_len, int *found)
{
	if (found) {
		*found = 0;
	}

	if (str == NULL) {
		return 0;
	}

	if (old == NULL || new == NULL) {
		return str_len;
	}

	str_len = str_len == 0 ? cstr_len(str) : str_len;
	old_len = old_len == 0 ? cstr_len(old) : old_len;
	new_len = new_len == 0 ? cstr_len(new) : new_len;

	if (str_len < old_len) {
		return str_len;
	}

	for (size_t i = 0; str_len >= old_len && i <= str_len - old_len; i++) {
		if (i > str_size) {
			return 0;
		}

		if (cstr_cmpn(&str[i], str_len, old, old_len, old_len)) {
			continue;
		}

		if (found) {
			*found = 1;
		}

		if (new_len < old_len) {
			for (size_t j = i + new_len, k = i + old_len; k <= str_len; j++, k++) {
				str[j] = str[k];
			}
		} else if (new_len > old_len) {
			if (str_len + new_len - old_len > str_size) {
				return 0;
			}
			for (size_t j = str_len + new_len - old_len, k = str_len; k >= i + old_len; j--, k--) {
				str[j] = str[k];
			}
		}

		if (i + new_len > str_size) {
			return 0;
		}

		for (size_t j = 0; j < new_len; j++) {
			str[i + j] = new[j];
		}

		str_len += new_len - old_len;
		i = i + new_len - 1;

		if (str_len > str_size) {
			return 0;
		}
	}

	return str_len;
}

size_t cstr_replaces(char *str, size_t str_size, size_t str_len, const char *const *old, const char *const *new, size_t cnt, int *found)
{
	for (size_t i = 0; i < cnt; i++) {
		str_len = cstr_replace(str, str_size, str_len, old[i], 0, new[i], 0, found);
	}

	return str_len;
}

size_t cstr_rreplaces(char *str, size_t str_size, size_t str_len, const char *const *old, const char *const *new, size_t cnt)
{
	int found = 0;
	do {
		str_len = cstr_replaces(str, str_size, str_len, old, new, cnt, &found);
	} while (found);

	return str_len;
}

void wcstrn_cat(wchar_t *dst, size_t size, const wchar_t *src, size_t cnt)
{
#if defined(C_WIN)
	wcsncat_s(dst, size, src, cnt);
#else
	wcsncat(dst, src, cnt);
#endif
}
