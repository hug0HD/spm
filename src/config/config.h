#ifndef SPM_CONFIG_H
#define SPM_CONFIG_H

#include "../utils/cJSON.h"
#include "../utils/error.h"
#include "../utils/memory.h"
#include "../utils/util.h"
#include <sys/stat.h>

#define SPM_DEFAULT_SESSION_TIMEOUT 1800
#define SPM_DEFAULT_FLASH_DURATION 30
#define SPM_DEFAULT_LOGIN_METHOD "system"
#define SPM_CONFIG_PATH "~/.config/spm/config.json"
#define SPM_DB_PATH "~/.config/spm/credentials.db"
#define SPM_MAX_PASSWORD_LEN 256
#define SPM_MAX_NAME_LEN 128

typedef struct {
  int version;
  char username[SPM_MAX_NAME_LEN];
  int session_timeout;
  char default_login_method[32];
  char database_path[256];
  char config_path[256];
  int flash_duration;
} Config;

SpmError config_load(const char *path, Config *config);

SpmError config_save(const char *path, const Config *config);

SpmError config_init_defaults(Config *config);

#endif
