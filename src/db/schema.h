#ifndef SPM_SCHEMA_H
#define SPM_SCHEMA_H

#include "../utils/error.h"
#include "db.h"

#define SCHEMA_VERSION 1

SpmError schema_init(Db *db);
int      schema_version(Db *db);

#endif // SPM_SCHEMA_H
