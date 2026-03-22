#include "keyring.h"

#ifdef PLATFORM_MACOS

#include <Security/Security.h>
#include <string.h>

SpmError keyring_store_key(const char *label, const uint8_t *key, size_t key_len) {
  if (!label || !key || key_len == 0) {
    return SPM_ERR_KEYRING;
  }

  /* Delete any existing entry so we can overwrite */
  keyring_delete_key(label);

  CFStringRef cf_label = CFStringCreateWithCString(kCFAllocatorDefault, label,
                                                   kCFStringEncodingUTF8);
  CFDataRef cf_data = CFDataCreate(kCFAllocatorDefault, key, (CFIndex)key_len);

  const void *keys[] = { kSecClass, kSecAttrLabel, kSecAttrAccount, kSecValueData };
  const void *vals[] = { kSecClassGenericPassword, cf_label, cf_label, cf_data };
  CFDictionaryRef attrs = CFDictionaryCreate(kCFAllocatorDefault, keys, vals, 4,
                                             &kCFTypeDictionaryKeyCallBacks,
                                             &kCFTypeDictionaryValueCallBacks);

  OSStatus status = SecItemAdd(attrs, NULL);

  CFRelease(cf_label);
  CFRelease(cf_data);
  CFRelease(attrs);

  return (status == errSecSuccess) ? SPM_OK : SPM_ERR_KEYRING;
}

SpmError keyring_retrieve_key(const char *label, uint8_t *key, size_t key_len) {
  if (!label || !key || key_len == 0) {
    return SPM_ERR_KEYRING;
  }

  CFStringRef cf_label = CFStringCreateWithCString(kCFAllocatorDefault, label,
                                                   kCFStringEncodingUTF8);

  const void *keys[] = { kSecClass, kSecAttrLabel, kSecAttrAccount,
                         kSecReturnData, kSecMatchLimit };
  const void *vals[] = { kSecClassGenericPassword, cf_label, cf_label,
                         kCFBooleanTrue, kSecMatchLimitOne };
  CFDictionaryRef query = CFDictionaryCreate(kCFAllocatorDefault, keys, vals, 5,
                                             &kCFTypeDictionaryKeyCallBacks,
                                             &kCFTypeDictionaryValueCallBacks);

  CFDataRef result = NULL;
  OSStatus status = SecItemCopyMatching(query, (CFTypeRef *)&result);

  CFRelease(cf_label);
  CFRelease(query);

  if (status != errSecSuccess || !result) {
    return SPM_ERR_KEYRING;
  }

  CFIndex len = CFDataGetLength(result);
  if ((size_t)len != key_len) {
    CFRelease(result);
    return SPM_ERR_KEYRING;
  }

  memcpy(key, CFDataGetBytePtr(result), key_len);
  CFRelease(result);
  return SPM_OK;
}

SpmError keyring_delete_key(const char *label) {
  if (!label) {
    return SPM_ERR_KEYRING;
  }

  CFStringRef cf_label = CFStringCreateWithCString(kCFAllocatorDefault, label,
                                                   kCFStringEncodingUTF8);

  const void *keys[] = { kSecClass, kSecAttrLabel, kSecAttrAccount };
  const void *vals[] = { kSecClassGenericPassword, cf_label, cf_label };
  CFDictionaryRef query = CFDictionaryCreate(kCFAllocatorDefault, keys, vals, 3,
                                             &kCFTypeDictionaryKeyCallBacks,
                                             &kCFTypeDictionaryValueCallBacks);

  OSStatus status = SecItemDelete(query);

  CFRelease(cf_label);
  CFRelease(query);

  return (status == errSecSuccess || status == errSecItemNotFound)
             ? SPM_OK
             : SPM_ERR_KEYRING;
}

#endif // PLATFORM_MACOS
