#ifndef SPM_CRYPTO_H
#define SPM_CRYPTO_H

#include "../utils/error.h"
#include <stddef.h>
#include <stdint.h>

#define CRYPTO_IV_LEN  12
#define CRYPTO_TAG_LEN 16
#define CRYPTO_KEY_LEN 32  /* AES-256 */

/* Required output buffer size for encryption */
#define CRYPTO_CIPHERTEXT_LEN(plaintext_len) \
  (CRYPTO_IV_LEN + CRYPTO_TAG_LEN + (plaintext_len))

/* Plaintext length recoverable from a ciphertext blob */
#define CRYPTO_PLAINTEXT_LEN(ciphertext_len) \
  ((ciphertext_len) - CRYPTO_IV_LEN - CRYPTO_TAG_LEN)

/*
 * Encrypt plaintext using AES-256-GCM.
 *
 * out must be CRYPTO_CIPHERTEXT_LEN(plaintext_len) bytes.
 * Blob layout: [ IV (12) | TAG (16) | ciphertext (n) ]
 */
SpmError crypto_encrypt(const uint8_t *key, size_t key_len,
                        const uint8_t *plaintext, size_t plaintext_len,
                        uint8_t *out, size_t out_len);

/*
 * Decrypt a blob produced by crypto_encrypt.
 *
 * out must be at least CRYPTO_PLAINTEXT_LEN(in_len) bytes.
 * *out_len is set to the number of bytes written on success.
 * Returns SPM_ERR_CRYPTO if the key is wrong or the blob is tampered.
 */
SpmError crypto_decrypt(const uint8_t *key, size_t key_len,
                        const uint8_t *in, size_t in_len,
                        uint8_t *out, size_t *out_len);

#endif // SPM_CRYPTO_H
