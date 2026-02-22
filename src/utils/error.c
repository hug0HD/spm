#include "error.h"

const char *spm_strerror(SpmError err) {
  switch (err) {
  case SPM_OK:
    return "OK";
  case SPM_ERR:
    return "General error";
  case SPM_ERR_DB:
    return "Database error";
  case SPM_ERR_CRYPTO:
    return "Crypto error";
  case SPM_ERR_KEYRING:
    return "Keyring error";
  case SPM_ERR_SESSION:
    return "Session error";
  case SPM_ERR_CONFIG:
    return "Config error";
  case SPM_ERR_MEMORY:
    return "Memory error";
  case SPM_ERR_NOT_FOUND:
    return "Not found";
  default:
    return "Unknown error";
  }
}
