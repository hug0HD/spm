#ifndef SPM_UTIL_H
#define SPM_UTIL_H

#include <ctype.h>
#include <string.h>
#include <sys/stat.h>

char *ltrim(char *s);
char *rtrim(char *s);
char *trim(char *s);

void safe_strcpy(char *dest, const char *src, size_t size);

int file_exists(const char *path);

int create_directory(const char *path);

#endif
