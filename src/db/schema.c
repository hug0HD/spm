#include "schema.h"
#include <stdlib.h>

#define SQL_CREATE_CREDENTIALS                                                 \
  "CREATE TABLE IF NOT EXISTS credentials ("                                   \
  "  id          INTEGER PRIMARY KEY AUTOINCREMENT,"                           \
  "  name        TEXT    NOT NULL,"                                            \
  "  password    TEXT,"                                                        \
  "  username    TEXT,"                                                        \
  "  email       TEXT,"                                                        \
  "  notes       TEXT,"                                                        \
  "  pin         TEXT,"                                                        \
  "  created_at  INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),"            \
  "  updated_at  INTEGER NOT NULL DEFAULT (strftime('%s', 'now'))"             \
  ");"

#define SQL_CREATE_META                                                         \
  "CREATE TABLE IF NOT EXISTS meta ("                                           \
  "  key   TEXT PRIMARY KEY,"                                                   \
  "  value TEXT NOT NULL"                                                       \
  ");"

#define SQL_INSERT_VERSION                                                      \
  "INSERT OR IGNORE INTO meta (key, value) VALUES ('schema_version', '1');"

SpmError schema_init(Db *db) {
  if (!db || !db->handle) {
    return SPM_ERR_DB;
  }

  SpmError err;

  err = db_exec(db, SQL_CREATE_CREDENTIALS);
  if (err != SPM_OK) return err;

  err = db_exec(db, SQL_CREATE_META);
  if (err != SPM_OK) return err;

  err = db_exec(db, SQL_INSERT_VERSION);
  if (err != SPM_OK) return err;

  return SPM_OK;
}

static void version_cb(DbRow *row, void *ctx) {
  int *out = (int *)ctx;
  if (row->col_count > 0 && row->col_values[0]) {
    *out = atoi(row->col_values[0]);
  }
}

int schema_version(Db *db) {
  if (!db || !db->handle) {
    return -1;
  }

  int version = -1;
  SpmError err = db_query(
    db,
    "SELECT value FROM meta WHERE key = 'schema_version';",
    version_cb,
    &version
  );

  if (err != SPM_OK) {
    return -1;
  }

  return version;
}
