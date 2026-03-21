#ifndef SPM_KEYMANAGEMENT_H
#define SPM_KEYMANAGEMENT_H

#include "../utils/error.h"
#include "crypto.h"
#include <stdint.h>

/*
 * Generate a 32-byte (256-bit) random key into buf.
 * buf must be at least CRYPTO_KEY_LEN bytes.
 */
SpmError keygen_generate(uint8_t *buf, size_t buf_len);

/*
 * Securely wipe a key buffer from memory.
 * buf_len must match the allocation size passed to secure_alloc.
 */
void keygen_destroy(uint8_t *buf, size_t buf_len);

#endif // SPM_KEYMANAGEMENT_H
