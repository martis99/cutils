#ifndef PATH_H
#define PATH_H

#include "platform.h"
#include "type.h"

typedef struct path_s {
	size_t len;
	char path[P_MAX_PATH];
} path_t;

typedef struct pathv_s {
	size_t len;
	const char *path;
} pathv_t;

path_t *path_init(path_t *path, const char *dir, size_t len);
path_t *path_child_s(path_t *path, const char *dir, size_t len, char s);
path_t *path_child(path_t *path, const char *dir, size_t len);
path_t *path_parent(path_t *path);
path_t *path_set_len(path_t *path, size_t len);
int path_ends(const path_t *path, const char *str, size_t len);
int path_calc_rel(const char *path, size_t path_len, const char *dest, size_t dest_len, path_t *out);

pathv_t *pathv_path(pathv_t *pathv, const path_t *path);
pathv_t *pathv_sub(pathv_t *pathv, const path_t *l, const path_t *r);
pathv_t *pathv_folder(pathv_t *pathv, const path_t *path);

#endif
