#include "cstr.h"

#include "mem.h"
#include "platform.h"

#include <string.h>

size_t cstr_len(const char *str)
{
	return strlen(str);
}

int cstr_cmp(const char *str1, size_t str1_len, const char *str2, size_t str2_len)
{
	return str1_len == str2_len && m_memcmp(str1, str2, str1_len) == 0;
}

int cstrn_cmp(const char *str1, size_t str1_len, const char *str2, size_t str2_len, size_t len)
{
	return str1_len >= len && str2_len >= len && m_memcmp(str1, str2, len) == 0;
}

void *cstr_cpy(char *dst, size_t dst_len, const char *src, size_t src_len)
{
	return m_memcpy(dst, dst_len, src, src_len * sizeof(char));
}

char *cstr_chr(const char *str, char c)
{
	return strchr(str, c);
}

char *cstr_rchr(const char *str, char c)
{
	return strrchr(str, c);
}

char *cstr_str(const char *str, const char *s)
{
	return strstr(str, s);
}

size_t cstr_replace(const char *src, size_t src_len, char *dst, size_t dst_len, const char *from, size_t from_len, const char *to, size_t to_len)
{
	src_len	 = src_len == 0 ? cstr_len(src) : src_len;
	from_len = from_len == 0 ? cstr_len(from) : from_len;
	to_len	 = to_len == 0 ? cstr_len(to) : to_len;

	size_t dst_i = 0;
	for (size_t i = 0; i < src_len; i++) {
		if (src_len - i > from_len && cstr_cmp(&src[i], from_len, from, from_len)) {
			cstr_cpy(&dst[dst_i], dst_len, to, to_len);
			i += from_len - 1;
			dst_i += to_len;
		} else {
			dst[dst_i++] = src[i];
		}
	}

	return dst_i;
}

size_t cstr_replaces(const char *src, size_t src_len, char *dst, size_t dst_len, const char *const *from, const char *const *to, size_t len)
{
	src_len = src_len == 0 ? cstr_len(src) : src_len;

	size_t dst_i = 0;
	for (size_t i = 0; i < src_len; i++) {
		bool found = 0;

		for (size_t j = 0; j < len; j++) {
			if (!from[j] || !to[j]) {
				continue;
			}

			size_t from_len = cstr_len(from[j]);
			size_t to_len	= cstr_len(to[j]);

			if (src_len - i >= from_len && cstr_cmp(&src[i], from_len, from[j], from_len)) {
				cstr_cpy(&dst[dst_i], dst_len, to[j], to_len);
				i += from_len - 1;
				dst_i += to_len;
				found = 1;
				break;
			}
		}

		if (!found) {
			dst[dst_i++] = src[i];
		}
	}

	return dst_i;
}

size_t cstr_inplace(char *str, size_t str_size, size_t str_len, const char *old, size_t old_len, const char *new, size_t new_len)
{
	str_len = str_len == 0 ? cstr_len(str) : str_len;
	old_len = old_len == 0 ? cstr_len(old) : old_len;
	new_len = new_len == 0 ? cstr_len(new) : new_len;

	if (str_len < old_len) {
		return str_len;
	}

	for (size_t i = 0; i <= str_len - old_len; i++) {
		if (i > str_size) {
			return 0;
		}

		if (!cstrn_cmp(&str[i], str_len, old, old_len, old_len)) {
			continue;
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

size_t cstr_inplaces(char *str, size_t str_size, size_t str_len, const char *const *old, const char *const *new, size_t cnt)
{
	for (size_t i = 0; i < cnt; i++) {
		if (!old[i] || !new[i]) {
			continue;
		}

		str_len = cstr_inplace(str, str_size, str_len, old[i], 0, new[i], 0);
	}

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
