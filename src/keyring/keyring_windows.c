#include "keyring.h"

#ifdef PLATFORM_WINDOWS

#include <string.h>
#include <windows.h>
#include <wincred.h>

SpmError keyring_store_key(const char *label, const uint8_t *key, size_t key_len) {
  if (!label || !key || key_len == 0) {
    return SPM_ERR_KEYRING;
  }

  CREDENTIALA cred = {0};
  cred.Type             = CRED_TYPE_GENERIC;
  cred.TargetName       = (LPSTR)label;
  cred.CredentialBlob     = (LPBYTE)key;
  cred.CredentialBlobSize = (DWORD)key_len;
  cred.Persist          = CRED_PERSIST_LOCAL_MACHINE;

  return CredWriteA(&cred, 0) ? SPM_OK : SPM_ERR_KEYRING;
}

SpmError keyring_retrieve_key(const char *label, uint8_t *key, size_t key_len) {
  if (!label || !key || key_len == 0) {
    return SPM_ERR_KEYRING;
  }

  PCREDENTIALA cred = NULL;
  if (!CredReadA(label, CRED_TYPE_GENERIC, 0, &cred)) {
    return SPM_ERR_KEYRING;
  }

  if (cred->CredentialBlobSize != (DWORD)key_len) {
    CredFree(cred);
    return SPM_ERR_KEYRING;
  }

  memcpy(key, cred->CredentialBlob, key_len);
  CredFree(cred);
  return SPM_OK;
}

SpmError keyring_delete_key(const char *label) {
  if (!label) {
    return SPM_ERR_KEYRING;
  }

  /* CredDeleteA returns FALSE if not found — that's fine */
  CredDeleteA(label, CRED_TYPE_GENERIC, 0);
  return SPM_OK;
}

#endif // PLATFORM_WINDOWS
