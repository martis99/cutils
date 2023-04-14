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

int path_init(path_t *path, const char *dir, size_t len);
int path_child_s(path_t *path, const char *dir, size_t len, char s);
int path_child(path_t *path, const char *dir, size_t len);
int path_parent(path_t *path);
int path_set_len(path_t *path, size_t len);
int path_ends(const path_t *path, const char *str, size_t len);
int path_calc_rel(const char *path, size_t path_len, const char *dest, size_t dest_len, path_t *out);

int pathv_path(pathv_t *pathv, const path_t *path);
int pathv_sub(pathv_t *pathv, const path_t *l, const path_t *r);
int pathv_folder(pathv_t *pathv, const path_t *path);

#endif
