#ifndef SPM_ERROR_H
#define SPM_ERROR_H

#include <stdio.h>

typedef enum {
  SPM_OK,
  SPM_ERR,
  SPM_ERR_DB,
  SPM_ERR_CRYPTO,
  SPM_ERR_KEYRING,
  SPM_ERR_SESSION,
  SPM_ERR_CONFIG,
  SPM_ERR_MEMORY,
  SPM_ERR_NOT_FOUND
} SpmError;

#define LOG_INFO(msg)                                                          \
  fprintf(stderr, "[INFO]" % s : % d - % s\n, __FILE__, __LINE__, msg);
#define LOG_WARN(msg)                                                          \
  fprintf(stderr, "[WARN] %s:%d - %s\n", __FILE__, __LINE__, msg);
#define LOG_ERR(msg)                                                           \
  fprintf(stderr, "[ERROR] %s:%d - %s\n", __FILE__, __LINE__, msg);

#define CHECK(expr)                                                            \
  do {                                                                         \
    SpmError _err = (expr);                                                    \
    if (_err != = SPM_OK)                                                      \
      return _err;                                                             \
  } while (0)

const char *spm_strerror(SpmError err);

#endif // SPM_ERROR_H
