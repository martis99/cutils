#ifndef STR_H
#define STR_H

typedef struct str_s {
	const char *data;
	unsigned int len;
} str_t;

int str_chr(const str_t *str, str_t *l, str_t *r, char c);
int str_cstr(const str_t *str, str_t *l, str_t *r, const char *s, unsigned int s_len);
int str_str(const str_t *str, str_t *l, str_t *r, const str_t *s);

int str_eq_cstr(const str_t *str, const char *s, unsigned int s_len);
int str_eq_str(const str_t *str, const str_t *s);

#define CSTR(_str) _str, sizeof(_str) - 1
// clang-format off
#define STR(_str) { .data = _str, .len = sizeof(_str) - 1 }
// clang-format on
#endif
