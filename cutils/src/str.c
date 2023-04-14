#include "str.h"

#include "cstr.h"

int str_chr(const str_t *str, str_t *l, str_t *r, char c)
{
	const char *res = cstr_chr(str->data, c);

	if (!res || res >= str->data + str->len) {
		return 1;
	}

	const char *str_data = str->data;
	size_t str_len	     = str->len;

	l->data = str_data;
	l->len	= (size_t)(res - l->data);

	r->data = res + 1;
	r->len	= (size_t)(str_data + str_len - r->data);

	return 0;
}

int str_cstr(const str_t *str, str_t *l, str_t *r, const char *s, size_t s_len)
{
	const char *res = cstr_str(str->data, s);

	if (!res || res >= str->data + str->len) {
		return 1;
	}

	const char *str_data = str->data;
	size_t str_len	     = str->len;

	l->data = str_data;
	l->len	= (size_t)(res - l->data);

	r->data = res + s_len;
	r->len	= (size_t)(str_data + str_len - r->data);

	return 0;
}

int str_str(const str_t *str, str_t *l, str_t *r, const str_t *s)
{
	return str_cstr(str, l, r, s->data, s->len);
}

int str_eq_cstr(const str_t *str, const char *s, size_t s_len)
{
	return cstr_cmp(str->data, str->len, s, s_len);
}

int str_eq_str(const str_t *str, const str_t *s)
{
	return str_eq_cstr(str, s->data, s->len);
}
