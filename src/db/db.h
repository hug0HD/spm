#ifndef SPM_DB_H
#define SPM_DB_H

#include "../utils/error.h"
#include <sqlite3.h>

typedef struct {
  sqlite3 *handle;
} Db;

typedef struct {
  int    col_count;
  char **col_names;
  char **col_values;
} DbRow;

typedef void (*DbRowCallback)(DbRow *row, void *ctx);

SpmError db_open(Db *db, const char *path);
void     db_close(Db *db);
SpmError db_exec(Db *db, const char *sql);
SpmError db_query(Db *db, const char *sql, DbRowCallback row_cb, void *ctx);

#endif // SPM_DB_H
