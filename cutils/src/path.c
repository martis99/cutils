#include "path.h"

#include "mem.h"

#include "platform.h"

#if defined(C_WIN)
	#define SEP '\\'
#else
	#define SEP '/'
#endif

path_t *path_init(path_t *path, const char *dir, size_t len)
{
	if (path == NULL || dir == NULL || len + 1 > P_MAX_PATH) {
		return NULL;
	}

	mem_cpy(path->path, len, dir, len);
	path->len = len;

	path->path[path->len] = '\0';

	return path;
}

path_t *path_child_s(path_t *path, const char *dir, size_t len, char s)
{
	if (path == NULL || dir == NULL || path->len + len + 2 > P_MAX_PATH) {
		return NULL;
	}

	if (s != '\0' && path->len > 0 && path->path[path->len - 1] != '/' && path->path[path->len - 1] != '\\') {
		path->path[path->len++] = s;
	}
	mem_cpy(path->path + path->len, len, dir, len);
	path->len += len;

	path->path[path->len] = '\0';

	return path;
}

path_t *path_child(path_t *path, const char *dir, size_t len)
{
	return path_child_s(path, dir, len, SEP);
}

path_t *path_parent(path_t *path)
{
	if (path == NULL) {
		return NULL;
	}

	size_t len = path->len;

	while (len > 0 && path->path[len] != '\\' && path->path[len] != '/')
		len--;

	if (len == 0) {
		return NULL;
	}

	path->len = len;

	path->path[path->len] = '\0';

	return path;
}

path_t *path_set_len(path_t *path, size_t len)
{
	if (path == NULL) {
		return NULL;
	}

	path->len	      = len;
	path->path[path->len] = '\0';

	return path;
}

int path_ends(const path_t *path, const char *str, size_t len)
{
	if (path == NULL) {
		return 0;
	}

	return path->len > len && mem_cmp(path->path + path->len - len, str, (size_t)len) == 0;
}

int path_calc_rel(const char *path, size_t path_len, const char *dest, size_t dest_len, path_t *out)
{
	if (path == NULL || dest == NULL || out == NULL) {
		return 1;
	}

	int same = path_len == dest_len;

	size_t prefix_len = -1;

	for (size_t i = 0; i < path_len && i < dest_len; i++) {
		if (path[i] != dest[i]) {
			same = 0;
			break;
		}

		if (path[i] == '/' || path[i] == '\\') {
			prefix_len = i;
		}
	}

	out->len = 0;
	if (same) {
		out->path[0] = '\0';
		return 0;
	}

	for (size_t i = prefix_len + 1; i < path_len; i++) {
		if (path[i] == '/' || path[i] == '\\') {
			out->path[out->len++] = '.';
			out->path[out->len++] = '.';
			out->path[out->len++] = SEP;
		}
	}

	path_child(out, &dest[prefix_len + 1], dest_len - prefix_len - 1);
	return 0;
}

pathv_t *pathv_path(pathv_t *pathv, const path_t *path)
{
	if (pathv == NULL || path == NULL) {
		return NULL;
	}

	*pathv = (pathv_t){ .path = path->path, .len = path->len };
	return pathv;
}

pathv_t *pathv_sub(pathv_t *pathv, const path_t *l, const path_t *r)
{
	if (pathv == NULL || l == NULL || r == NULL) {
		return NULL;
	}

	*pathv = (pathv_t){
		.path = l->path + r->len + 1,
		.len  = l->len - r->len - 1,
	};
	return pathv;
}

pathv_t *pathv_folder(pathv_t *pathv, const path_t *path)
{
	if (pathv == NULL || path == NULL) {
		return NULL;
	}

	size_t len = path->len;

	while (len > 0 && path->path[len] != '\\' && path->path[len] != '/')
		len--;

	*pathv = (pathv_t){
		.path = path->path + len + 1,
		.len  = path->len - len - 1,
	};

	return pathv;
}
