#include "file.h"

#include "cstr.h"
#include "path.h"
#include "print.h"

#include "platform.h"

#include <string.h>

#if defined(P_LINUX)
	#include <dirent.h>
	#include <errno.h>
	#include <sys/stat.h>
#endif

FILE *file_open(const char *path, const char *mode)
{
	FILE *file = NULL;

#if defined(P_WIN)
	fopen_s(&file, path, mode);
#else
	file = fopen(path, mode);
#endif
	return file;
}

FILE *file_open_v(const char *format, const char *mode, va_list args)
{
	char path[P_MAX_PATH] = { 0 };

	if (p_vsnprintf(path, sizeof(path) / sizeof(char), format, args) == -1) {
		return NULL;
	}

	return file_open(path, mode);
}

FILE *file_open_f(const char *format, const char *mode, ...)
{
	va_list args;
	va_start(args, mode);
	FILE *file = file_open_v(format, mode, args);
	va_end(args);
	return file;
}

size_t file_read(FILE *file, size_t size, char *data, size_t data_size)
{
	size_t cnt;
#if defined(P_WIN)
	cnt = fread_s(data, data_size, size, 1, file);
#else
	cnt  = fread(data, size, 1, file);
#endif
	if (cnt != 1) {
		return 1;
	}

	return 0;
}

size_t file_read_t(const char *path, char *data, size_t data_size)
{
	FILE *file = file_open(path, "rb");

	if (file_read(file, file_size(file), data, data_size)) {
		return -1;
	}

	size_t len = 0;
	for (size_t i = 0; i < data_size; i++) {
		data[len] = data[i];
		if (data[i] != '\r') {
			len++;
		}
	}

	file_close(file);

	return len;
}

size_t file_size(FILE *file)
{
	fseek(file, 0L, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	return size;
}

int file_close(FILE *file)
{
	return fclose(file);
}

int file_exists(const char *path)
{
#if defined(P_WIN)
	int dwAttrib = GetFileAttributesA(path);
	return dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
#else
	struct stat buffer;
	if (stat(path, &buffer)) {
		return 0;
	} else {
		return 1;
	}
#endif
}

int file_exists_v(const char *format, va_list args)
{
	char path[P_MAX_PATH] = { 0 };

	if (p_vsnprintf(path, sizeof(path) / sizeof(char), format, args) == -1) {
		return 0;
	}

	return file_exists(path);
}

int file_exists_f(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	int exists = file_exists_v(format, args);
	va_end(args);
	return exists;
}

int folder_exists(const char *path)
{
#if defined(P_WIN)
	int dwAttrib = GetFileAttributesA(path);
	return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
#else
	DIR *dir = opendir(path);
	if (dir) {
		closedir(dir);
		return 1;
	} else {
		return 0;
	}
#endif
}

int folder_exists_v(const char *format, va_list args)
{
	char path[P_MAX_PATH] = { 0 };

	if (p_vsnprintf(path, sizeof(path) / sizeof(char), format, args) == -1) {
		return 0;
	}

	return folder_exists(path);
}

int folder_exists_f(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	int exists = folder_exists_v(format, args);
	va_end(args);
	return exists;
}

int folder_create(const char *path)
{
#if defined(P_WIN)
	return CreateDirectoryA(path, NULL) || ERROR_ALREADY_EXISTS == GetLastError();
#else
	struct stat st = { 0 };
	if (stat(path, &st) == -1) {
		mkdir(path, 0700);
	}
	return 1;
#endif
}

int files_foreach(const path_t *path, files_foreach_cb on_folder, files_foreach_cb on_file, void *priv)
{
#if defined(P_WIN)
	WIN32_FIND_DATA file = { 0 };
	HANDLE find	     = NULL;

	path_t child_path = *path;
	path_child(&child_path, "*.*", 3);

	if ((find = FindFirstFileA(child_path.path, (LPWIN32_FIND_DATAA)&file)) == INVALID_HANDLE_VALUE) {
		return 1;
	}
	child_path.len = path->len;

	do {
		if (strcmp((char *)file.cFileName, ".") == 0 || strcmp((char *)file.cFileName, "..") == 0) {
			continue;
		}

		files_foreach_cb cb = file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? on_folder : on_file;
		if (cb) {
			if (path_child(&child_path, (char *)file.cFileName, cstr_len((char *)file.cFileName))) {
				child_path.len = path->len;
				continue;
			}

			int ret = cb(&child_path, (char *)file.cFileName, priv);

			child_path.len = path->len;
			if (ret < 0) {
				return ret;
			}
		}
	} while (FindNextFileA(find, (LPWIN32_FIND_DATAA)&file));

	FindClose(find);

#else

	DIR *dir = opendir(path->path);
	if (!dir) {
		return 1;
	}

	path_t child_path = *path;
	struct dirent *dp;

	while ((dp = readdir(dir))) {
		struct stat stbuf = { 0 };

		if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
			goto next;
		}

		if (path_child(&child_path, dp->d_name, cstr_len(dp->d_name))) {
			goto next;
		}

		if (stat(child_path.path, &stbuf)) {
			goto next;
		}

		files_foreach_cb cb = folder_exists(child_path.path) ? on_folder : on_file;
		if (cb) {
			int ret = cb(&child_path, dp->d_name, priv);

			child_path.len = path->len;
			if (ret < 0) {
				return ret;
			}
		}
next:
		child_path.len = path->len;
	}

#endif

	return 0;
}
