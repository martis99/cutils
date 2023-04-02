#include "cstr.h"

#include "mem.h"
#include "platform.h"

#include <string.h>

unsigned int cstr_len(const char *str)
{
	return (unsigned int)strlen(str);
}

int cstr_cmp(const char *str1, unsigned int str1_len, const char *str2, unsigned int str2_len)
{
	return str1_len == str2_len && m_memcmp(str1, str2, str1_len * sizeof(char)) == 0;
}

int cstrn_cmp(const char *str1, unsigned int str1_len, const char *str2, unsigned int str2_len, unsigned int len)
{
	return str1_len >= len && str2_len >= len && m_memcmp(str1, str2, len * sizeof(char)) == 0;
}

void *cstr_cpy(char *dst, unsigned int dst_len, const char *src, unsigned int src_len)
{
	return m_memcpy(dst, dst_len, src, src_len * sizeof(char));
}

char *cstr_chr(const char *str, char c)
{
	return strchr(str, c);
}

char *cstr_str(const char *str, const char *s)
{
	return strstr(str, s);
}

int cstr_replace(const char *src, unsigned int src_len, char *dst, unsigned int dst_len, const char *from, unsigned int from_len, const char *to, unsigned int to_len)
{
	src_len	 = src_len == 0 ? cstr_len(src) : src_len;
	from_len = from_len == 0 ? cstr_len(from) : from_len;
	to_len	 = to_len == 0 ? cstr_len(to) : to_len;

	unsigned int dst_i = 0;
	for (unsigned int i = 0; i < src_len; i++) {
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

int cstr_replaces(const char *src, unsigned int src_len, char *dst, unsigned int dst_len, const char *const *from, const char *const *to, unsigned int len)
{
	src_len = src_len == 0 ? cstr_len(src) : src_len;

	unsigned int dst_i = 0;
	for (unsigned int i = 0; i < src_len; i++) {
		int found = 0;

		for (unsigned int j = 0; j < len; j++) {
			if (!from[j] || !to[j]) {
				continue;
			}

			unsigned int from_len = cstr_len(from[j]);
			unsigned int to_len   = cstr_len(to[j]);

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

void wcstrn_cat(wchar_t *dst, unsigned int size, const wchar_t *src, unsigned int cnt)
{
#if defined(C_WIN)
	wcsncat_s(dst, size, src, cnt);
#else
	wcsncat(dst, src, cnt);
#endif
}
