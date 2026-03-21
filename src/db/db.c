#include "db.h"

SpmError db_open(Db *db, const char *path) {
  if (!db || !path) {
    return SPM_ERR_DB;
  }

  if (sqlite3_open(path, &db->handle) != SQLITE_OK) {
    LOG_ERR(sqlite3_errmsg(db->handle));
    sqlite3_close(db->handle);
    db->handle = NULL;
    return SPM_ERR_DB;
  }

  /* WAL mode for better concurrent read access */
  SpmError err = db_exec(db, "PRAGMA journal_mode=WAL;");
  if (err != SPM_OK) {
    sqlite3_close(db->handle);
    db->handle = NULL;
    return err;
  }

  return SPM_OK;
}

void db_close(Db *db) {
  if (db && db->handle) {
    sqlite3_close(db->handle);
    db->handle = NULL;
  }
}

SpmError db_exec(Db *db, const char *sql) {
  if (!db || !db->handle || !sql) {
    return SPM_ERR_DB;
  }

  char *errmsg = NULL;
  if (sqlite3_exec(db->handle, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
    LOG_ERR(errmsg);
    sqlite3_free(errmsg);
    return SPM_ERR_DB;
  }

  return SPM_OK;
}

SpmError db_query(Db *db, const char *sql, DbRowCallback row_cb, void *ctx) {
  if (!db || !db->handle || !sql) {
    return SPM_ERR_DB;
  }

  sqlite3_stmt *stmt = NULL;
  if (sqlite3_prepare_v2(db->handle, sql, -1, &stmt, NULL) != SQLITE_OK) {
    LOG_ERR(sqlite3_errmsg(db->handle));
    return SPM_ERR_DB;
  }

  int col_count = sqlite3_column_count(stmt);
  int rc;

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    if (row_cb) {
      char *names[col_count];
      char *values[col_count];

      for (int i = 0; i < col_count; i++) {
        names[i]  = (char *)sqlite3_column_name(stmt, i);
        values[i] = (char *)sqlite3_column_text(stmt, i);
      }

      DbRow row = {
        .col_count  = col_count,
        .col_names  = names,
        .col_values = values,
      };

      row_cb(&row, ctx);
    }
  }

  sqlite3_finalize(stmt);

  if (rc != SQLITE_DONE) {
    LOG_ERR(sqlite3_errmsg(db->handle));
    return SPM_ERR_DB;
  }

  return SPM_OK;
}
