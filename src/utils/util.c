#include "util.h"

char *ltrim(char *s) {
  if (!s || !*s) {
    return s;
  }
  while (isspace(*s))
    s++;
  return s;
}

char *rtrim(char *s) {
  if (!s || !*s) {
    return s;
  }
  char *back = s + strlen(s) - 1;
  while (back > s && isspace(*--back))
    ;
  *(back + 1) = '\0';
  return s;
}

char *trim(char *s) {
  if (!s || !*s) {
    return s;
  }
  return rtrim(ltrim(s));
}

void safe_strcpy(char *dest, const char *src, size_t size) {
  if (!dest || !src || size == 0) {
    return;
  }
  strncpy(dest, src, size - 1);
  dest[size - 1] = '\0';
}

int file_exists(const char *path) {
  if (!path) {
    return 0;
  }
  struct stat st;
  return stat(path, &st) == 0;
}

int create_directory(const char *path) {
  if (!path) {
    return -1;
  }
  char tmp[256];
  safe_strcpy(tmp, path, sizeof(tmp));
  for (char *p = tmp + 1; *p; p++) {
    if (*p == '/') {
      *p = '\0';
      mkdir(tmp, 0755);
      *p = '/';
    }
  }
  return mkdir(tmp, 0755);
}
