#include "path.h"

#include "mem.h"

#include "platform.h"

int path_init(path_t *path, const char *dir, unsigned int len)
{
	if (len + 1 > P_MAX_PATH) {
		return 1;
	}

	m_cpy(path->path, len, dir, len);
	path->len = (unsigned int)len;

	path->path[path->len] = '\0';

	return 0;
}

int path_child_s(path_t *path, const char *dir, unsigned int len, char s)
{
	if (path->len + len + 2 > P_MAX_PATH) {
		return 1;
	}

	if (s != 0) {
		path->path[path->len++] = s;
	}
	m_cpy(path->path + path->len, len, dir, len);
	path->len += (unsigned int)len;

	path->path[path->len] = '\0';

	return 0;
}

int path_child(path_t *path, const char *dir, unsigned int len)
{
	char c;
#if defined(P_WIN)
	c = '\\';
#else
	c = '/';
#endif
	return path_child_s(path, dir, len, c);
}

int path_parent(path_t *path)
{
	unsigned int len = path->len;

	while (len > 0 && path->path[len] != '\\' && path->path[len] != '/')
		len--;

	if (len == 0) {
		return 1;
	}

	path->len = len;

	path->path[path->len] = '\0';

	return 0;
}

int path_set_len(path_t *path, unsigned int len)
{
	path->len	      = len;
	path->path[path->len] = '\0';
	return 0;
}

int path_ends(const path_t *path, const char *str, unsigned int len)
{
	return path->len > len && m_cmp(path->path + path->len - len, str, (size_t)len) == 0;
}

int path_calc_rel(const char *path, unsigned int path_len, const char *dest, unsigned int dest_len, path_t *out)
{
	int dif	       = 0;
	int last_slash = 0;

	for (unsigned int i = 0; i < path_len; i++) {
		if (dif == 0 && (path[i] == '\\' || path[i] == '/')) {
			last_slash = i;
		}

		if (dif && (path[i] == '\\' || path[i] == '/')) {
			dif++;
		}

		if (dif == 0 && (i > dest_len || path[i] != dest[i])) {
			dif = 1;
		}
	}

	out->len = 0;
	for (int i = 0; i < dif; i++) {
		out->path[out->len++] = '.';
		out->path[out->len++] = '.';
		out->path[out->len++] = '\\';
	}

	if (out->len) {
		out->len--;
	}

	path_child(out, &dest[last_slash + 1], dest_len - last_slash - 1);
	return 0;
}

int pathv_path(pathv_t *pathv, const path_t *path)
{
	*pathv = (pathv_t){ .path = path->path, .len = path->len };
	return 0;
}

int pathv_sub(pathv_t *pathv, const path_t *l, const path_t *r)
{
	*pathv = (pathv_t){
		.path = l->path + r->len + 1,
		.len  = l->len - r->len - 1,
	};
	return 0;
}

int pathv_folder(pathv_t *pathv, const path_t *path)
{
	unsigned int len = path->len;

	while (len > 0 && path->path[len] != '\\' && path->path[len] != '/')
		len--;

	*pathv = (pathv_t){
		.path = path->path + len + 1,
		.len  = path->len - len - 1,
	};

	return 0;
}
