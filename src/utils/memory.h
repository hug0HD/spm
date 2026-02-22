#ifndef SPM_MEMORY_H
#define SPM_MEMORY_H

#include "error.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#if defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX)
#include <sys/mman.h>
#endif

void secure_free(void *ptr, size_t len);

void *secure_alloc(size_t len);

void *secure_lock(void *ptr, size_t len);

void secure_unlock(void *ptr, size_t len);

#endif // SPM_MEMORY_H
