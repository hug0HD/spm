#ifndef SPM_KEYRING_H
#define SPM_KEYRING_H

#include "../utils/error.h"
#include <stddef.h>
#include <stdint.h>

/*
 * Store a binary key in the system keyring under the given label.
 * If an entry for label already exists it is overwritten.
 */
SpmError keyring_store_key(const char *label, const uint8_t *key, size_t key_len);

/*
 * Retrieve a binary key from the system keyring into caller-allocated buf.
 * key_len must match the length used when the key was stored.
 */
SpmError keyring_retrieve_key(const char *label, uint8_t *key, size_t key_len);

/*
 * Remove a key from the system keyring.
 * Returns SPM_OK if the item was deleted or did not exist.
 */
SpmError keyring_delete_key(const char *label);

#endif // SPM_KEYRING_H
