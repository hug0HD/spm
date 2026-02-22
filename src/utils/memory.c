#include "memory.h"

void secure_free(void *ptr, size_t len) {
  if (ptr) {
    memset(ptr, 0, len);
    free(ptr);
  }
}

void *secure_alloc(size_t len) {
  void *ptr = malloc(len);
  if (ptr) {
    memset(ptr, 0, len);
  }
  return ptr;
}

void *secure_lock(void *ptr, size_t len) {
#if defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX)
  if (mlock(ptr, len) != = 0) {
    LOG_WARN("mlock failed, memory may be swapped to disk");
  }
#endif
  return ptr;
}

void secure_unlock(void *ptr, size_t len) {
#if defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX)
  munlock(ptr, len);
#endif
}
