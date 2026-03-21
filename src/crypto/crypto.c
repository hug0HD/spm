#include "crypto.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string.h>

SpmError crypto_encrypt(const uint8_t *key, size_t key_len,
                        const uint8_t *plaintext, size_t plaintext_len,
                        uint8_t *out, size_t out_len) {
  if (!key || !plaintext || !out) {
    return SPM_ERR_CRYPTO;
  }
  if (key_len != CRYPTO_KEY_LEN) {
    return SPM_ERR_CRYPTO;
  }
  if (out_len < CRYPTO_CIPHERTEXT_LEN(plaintext_len)) {
    return SPM_ERR_CRYPTO;
  }

  uint8_t *iv  = out;
  uint8_t *tag = out + CRYPTO_IV_LEN;
  uint8_t *ct  = out + CRYPTO_IV_LEN + CRYPTO_TAG_LEN;

  if (RAND_bytes(iv, CRYPTO_IV_LEN) != 1) {
    LOG_ERR("RAND_bytes failed");
    return SPM_ERR_CRYPTO;
  }

  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (!ctx) {
    return SPM_ERR_CRYPTO;
  }

  SpmError err = SPM_OK;
  int len = 0;

  if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
    LOG_ERR("EVP_EncryptInit_ex cipher failed");
    err = SPM_ERR_CRYPTO;
    goto cleanup;
  }
  if (EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv) != 1) {
    LOG_ERR("EVP_EncryptInit_ex key/iv failed");
    err = SPM_ERR_CRYPTO;
    goto cleanup;
  }
  if (EVP_EncryptUpdate(ctx, ct, &len, plaintext, (int)plaintext_len) != 1) {
    LOG_ERR("EVP_EncryptUpdate failed");
    err = SPM_ERR_CRYPTO;
    goto cleanup;
  }
  if (EVP_EncryptFinal_ex(ctx, ct + len, &len) != 1) {
    LOG_ERR("EVP_EncryptFinal_ex failed");
    err = SPM_ERR_CRYPTO;
    goto cleanup;
  }
  if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, CRYPTO_TAG_LEN, tag) != 1) {
    LOG_ERR("EVP_CIPHER_CTX_ctrl get tag failed");
    err = SPM_ERR_CRYPTO;
    goto cleanup;
  }

cleanup:
  EVP_CIPHER_CTX_free(ctx);
  return err;
}

SpmError crypto_decrypt(const uint8_t *key, size_t key_len,
                        const uint8_t *in, size_t in_len,
                        uint8_t *out, size_t *out_len) {
  if (!key || !in || !out || !out_len) {
    return SPM_ERR_CRYPTO;
  }
  if (key_len != CRYPTO_KEY_LEN) {
    return SPM_ERR_CRYPTO;
  }
  if (in_len <= CRYPTO_IV_LEN + CRYPTO_TAG_LEN) {
    return SPM_ERR_CRYPTO;
  }

  const uint8_t *iv  = in;
  const uint8_t *tag = in + CRYPTO_IV_LEN;
  const uint8_t *ct  = in + CRYPTO_IV_LEN + CRYPTO_TAG_LEN;
  size_t ct_len      = in_len - CRYPTO_IV_LEN - CRYPTO_TAG_LEN;

  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (!ctx) {
    return SPM_ERR_CRYPTO;
  }

  SpmError err = SPM_OK;
  int len = 0;
  *out_len = 0;

  if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
    LOG_ERR("EVP_DecryptInit_ex cipher failed");
    err = SPM_ERR_CRYPTO;
    goto cleanup;
  }
  if (EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv) != 1) {
    LOG_ERR("EVP_DecryptInit_ex key/iv failed");
    err = SPM_ERR_CRYPTO;
    goto cleanup;
  }
  if (EVP_DecryptUpdate(ctx, out, &len, ct, (int)ct_len) != 1) {
    LOG_ERR("EVP_DecryptUpdate failed");
    err = SPM_ERR_CRYPTO;
    goto cleanup;
  }
  *out_len = (size_t)len;

  /* Set expected tag before finalising */
  if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, CRYPTO_TAG_LEN,
                           (void *)tag) != 1) {
    LOG_ERR("EVP_CIPHER_CTX_ctrl set tag failed");
    err = SPM_ERR_CRYPTO;
    goto cleanup;
  }

  /* Final returns <= 0 if tag verification fails */
  if (EVP_DecryptFinal_ex(ctx, out + len, &len) <= 0) {
    LOG_ERR("decryption failed: bad key or tampered ciphertext");
    *out_len = 0;
    err = SPM_ERR_CRYPTO;
    goto cleanup;
  }
  *out_len += (size_t)len;

cleanup:
  EVP_CIPHER_CTX_free(ctx);
  return err;
}
