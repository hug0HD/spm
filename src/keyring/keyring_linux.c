#include "keyring.h"

#ifdef PLATFORM_LINUX

#include "../crypto/crypto.h"
#include <libsecret/secret.h>
#include <stdio.h>
#include <string.h>

static const SecretSchema SPM_SCHEMA = {
  "org.spm.keyring",
  SECRET_SCHEMA_NONE,
  {
    { "label", SECRET_SCHEMA_ATTRIBUTE_STRING },
    { NULL, 0 }
  },
  0, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

/* Encode binary key as lowercase hex string */
static void key_to_hex(const uint8_t *key, size_t key_len, char *out) {
  for (size_t i = 0; i < key_len; i++) {
    sprintf(out + (i * 2), "%02x", key[i]);
  }
  out[key_len * 2] = '\0';
}

/* Decode hex string back into binary key */
static SpmError hex_to_key(const char *hex, uint8_t *key, size_t key_len) {
  if (strlen(hex) != key_len * 2) {
    return SPM_ERR_KEYRING;
  }
  for (size_t i = 0; i < key_len; i++) {
    unsigned int byte;
    if (sscanf(hex + (i * 2), "%02x", &byte) != 1) {
      return SPM_ERR_KEYRING;
    }
    key[i] = (uint8_t)byte;
  }
  return SPM_OK;
}

SpmError keyring_store_key(const char *label, const uint8_t *key, size_t key_len) {
  if (!label || !key || key_len == 0) {
    return SPM_ERR_KEYRING;
  }

  /* Fixed-size hex buffer: CRYPTO_KEY_LEN * 2 + null terminator */
  char hex[CRYPTO_KEY_LEN * 2 + 1];
  key_to_hex(key, key_len, hex);

  GError *error = NULL;
  gboolean ok = secret_password_store_sync(
    &SPM_SCHEMA, SECRET_COLLECTION_DEFAULT,
    label, hex, NULL, &error,
    "label", label,
    NULL
  );

  if (error) {
    LOG_ERR(error->message);
    g_error_free(error);
  }

  memset(hex, 0, sizeof(hex));
  return ok ? SPM_OK : SPM_ERR_KEYRING;
}

SpmError keyring_retrieve_key(const char *label, uint8_t *key, size_t key_len) {
  if (!label || !key || key_len == 0) {
    return SPM_ERR_KEYRING;
  }

  GError *error = NULL;
  gchar *hex = secret_password_lookup_sync(
    &SPM_SCHEMA, NULL, &error,
    "label", label,
    NULL
  );

  if (error) {
    LOG_ERR(error->message);
    g_error_free(error);
    return SPM_ERR_KEYRING;
  }
  if (!hex) {
    return SPM_ERR_NOT_FOUND;
  }

  SpmError err = hex_to_key(hex, key, key_len);
  secret_password_free(hex);
  return err;
}

SpmError keyring_delete_key(const char *label) {
  if (!label) {
    return SPM_ERR_KEYRING;
  }

  GError *error = NULL;
  secret_password_clear_sync(
    &SPM_SCHEMA, NULL, &error,
    "label", label,
    NULL
  );

  if (error) {
    LOG_ERR(error->message);
    g_error_free(error);
    return SPM_ERR_KEYRING;
  }

  return SPM_OK;
}

#endif // PLATFORM_LINUX
