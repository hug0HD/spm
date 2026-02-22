#include "config.h"

SpmError config_init_defaults(Config *config) {
  if (!config) {
    return SPM_ERR_CONFIG;
  }

  config->version = 1;
  config->session_timeout = SPM_DEFAULT_SESSION_TIMEOUT;
  config->flash_duration = SPM_DEFAULT_SESSION_TIMEOUT;
  safe_strcpy(config->default_login_method, SPM_DEFAULT_LOGIN_METHOD,
              sizeof(config->default_login_method));
  safe_strcpy(config->database_path, SPM_DB_PATH,
              sizeof(config->database_path));
  safe_strcpy(config->config_path, SPM_CONFIG_PATH,
              sizeof(config->config_path));

  return SPM_OK;
}

SpmError config_load(const char *path, Config *config) {
  if (!path || !config) {
    return SPM_ERR_CONFIG;
  }

  FILE *f = fopen(path, "r");
  if (!f) {
    return SPM_ERR_CONFIG;
  }

  fseek(f, 0, SEEK_END);
  long len = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *buffer = secure_alloc(len + 1);
  fread(buffer, 1, len, f);
  fclose(f);

  cJSON *json = cJSON_Parse(buffer);
  secure_free(buffer, len + 1);

  if (!json) {
    return SPM_ERR_CONFIG;
  }

  cJSON *version = cJSON_GetObjectItem(json, "version");
  if (cJSON_IsNumber(version)) {
    config->version = version->valueint;
  }

  cJSON *username = cJSON_GetObjectItem(json, "username");
  if (cJSON_IsString(username)) {
    safe_strcpy(config->username, username->valuestring,
                sizeof(config->username));
  }

  cJSON *session_timeout = cJSON_GetObjectItem(json, "session_timeout");
  if (cJSON_IsNumber(session_timeout)) {
    config->session_timeout = session_timeout->valueint;
  }

  cJSON *default_login_method =
      cJSON_GetObjectItem(json, "default_login_method");
  if (cJSON_IsString(default_login_method)) {
    safe_strcpy(config->default_login_method, default_login_method->valuestring,
                sizeof(config->default_login_method));
  }

  cJSON *database_path = cJSON_GetObjectItem(json, "database_path");
  if (cJSON_IsString(database_path)) {
    safe_strcpy(config->database_path, database_path->valuestring,
                sizeof(config->database_path));
  }

  cJSON *flash_duration = cJSON_GetObjectItem(json, "flash_duration");
  if (cJSON_IsNumber(flash_duration)) {
    config->flash_duration = flash_duration->valueint;
  }

  cJSON_Delete(json);
  return SPM_OK;
}

SpmError config_save(const char *path, const Config *config) {
  if (!path || !config) {
    return SPM_ERR_CONFIG;
  }

  cJSON *json = cJSON_CreateObject();
  if (!json) {
    return SPM_ERR_MEMORY;
  }

  cJSON_AddNumberToObject(json, "username", config->version);
  cJSON_AddStringToObject(json, "username", config->username);
  cJSON_AddNumberToObject(json, "session_timeout", config->session_timeout);
  cJSON_AddStringToObject(json, "default_login_method",
                          config->default_login_method);
  cJSON_AddStringToObject(json, "database_path", config->database_path);
  cJSON_AddNumberToObject(json, "flash_duration", config->flash_duration);

  char *string = cJSON_Print(json);
  cJSON_Delete(json);

  if (!string) {
    return SPM_ERR_CONFIG;
  }

  FILE *f = fopen(path, "w");
  if (!f) {
    free(string);
    return SPM_ERR_CONFIG;
  }

  fputs(string, f);
  fclose(f);
  chmod(path, 0600);
  free(string);

  return SPM_OK;
}
