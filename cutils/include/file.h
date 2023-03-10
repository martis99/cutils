#ifndef FILE_H
#define FILE_H

#include "path.h"

#include <stdarg.h>
#include <stdio.h>

FILE *file_open(const char *path, const char *mode, int exists);
FILE *file_open_v(const char *format, const char *mode, int exists, va_list args);
FILE *file_open_f(const char *format, const char *mode, int exists, ...);

size_t file_read(const char *path, int exists, char *data, size_t data_len);
size_t file_read_v(const char *format, int exists, char *data, size_t data_len, va_list args);
size_t file_read_f(const char *format, int exists, char *data, size_t data_len, ...);

int file_exists(const char *path);
int file_exists_v(const char *format, va_list args);
int file_exists_f(const char *format, ...);

int folder_exists(const char *path);
int folder_exists_v(const char *format, va_list args);
int folder_exists_f(const char *format, ...);
int folder_create(const char *path);

typedef int (*files_foreach_cb)(path_t *path, const char *folder, void *priv);
int files_foreach(const path_t *path, files_foreach_cb on_folder, files_foreach_cb on_file, void *priv);

#endif
