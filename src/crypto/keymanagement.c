#include "keymanagement.h"
#include "../utils/memory.h"
#include <openssl/rand.h>

SpmError keygen_generate(uint8_t *buf, size_t buf_len) {
  if (!buf || buf_len < CRYPTO_KEY_LEN) {
    return SPM_ERR_CRYPTO;
  }

  if (RAND_bytes(buf, CRYPTO_KEY_LEN) != 1) {
    LOG_ERR("RAND_bytes failed during key generation");
    return SPM_ERR_CRYPTO;
  }

  return SPM_OK;
}

void keygen_destroy(uint8_t *buf, size_t buf_len) {
  secure_free(buf, buf_len);
}
