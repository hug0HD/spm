# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
make          # debug build (default)
make release  # optimised build (-O2 -DNDEBUG)
make clean    # remove build/ and binary
make info     # print detected platform, compiler, flags
make valgrind # run binary under valgrind (full leak check)
make install  # install to /usr/local/bin
```

No test framework is set up yet — there are no automated tests to run.

### Linux dependencies

```bash
sudo apt install libsecret-1-dev libsqlite3-dev libssl-dev libncurses-dev
```

### macOS dependencies

OpenSSL is expected at `/opt/homebrew/opt/openssl@3` (Homebrew). libsecret is not needed; the Security framework is linked automatically.

## Architecture

The project is a CLI password manager in C11. Most module headers are stubs — active development is filling them in incrementally. The `src/` tree:

- **`main.c`** — entry point (currently minimal placeholder)
- **`cli/`** — arg parsing, command dispatch, user input, ncurses UI
- **`config/`** — `Config` struct, JSON load/save via cJSON, path defaults. Config file lives at `~/.config/spm/config.json`; DB at `~/.config/spm/credentials.db`
- **`crypto/`** — OpenSSL-backed encryption and key management (stubs)
- **`db/`** — SQLite credential storage and schema (stubs)
- **`session/`** — session state and timeout logic (stubs)
- **`keyring/`** — platform-specific backends: `keyring_linux.c` (libsecret), `keyring_macos.c` (Security.framework), `keyring_windows.c` (Crypt32). Selected at compile time via `PLATFORM_LINUX` / `PLATFORM_MACOS` / `PLATFORM_WINDOWS` defines
- **`utils/`** — shared primitives used by all modules:
  - `error.h` — `SpmError` enum + `CHECK()` macro (propagates errors), `LOG_*` macros
  - `memory.h` — `secure_alloc`/`secure_free`/`secure_lock`/`secure_unlock` (uses `mmap` on POSIX for memory locking)
  - `util.h` — `safe_strcpy`, `trim`/`ltrim`/`rtrim`, `file_exists`, `create_directory`
  - `cJSON.h/.c` — embedded JSON library (vendored)

## Conventions

- **Error handling**: functions return `SpmError`. Use the `CHECK(expr)` macro to propagate errors early-return style.
- **Secure memory**: use `secure_alloc`/`secure_free` (not `malloc`/`free`) for any buffer holding sensitive data. Always pass the allocation size to `secure_free`.
- **String copies**: always use `safe_strcpy` instead of `strcpy`/`strncpy`.
- **Include paths**: the Makefile adds `-Isrc`, so headers are included as `"config/config.h"`, `"utils/error.h"`, etc. from any `.c` file.
- **Platform guards**: keyring and memory-locking code uses `#if defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX)` guards — keep platform-specific code inside these blocks.
- **Known bugs in `error.h`**: `LOG_INFO` has a malformed format string, and `CHECK` uses `!=` written as `!= =`. These will cause compile errors — fix them before using those macros.
- **Known bug in `config.c`**: `config_save` calls `cJSON_AddNumberToObject(json, "username", config->version)` before `cJSON_AddStringToObject(json, "username", ...)`, adding the key twice. The `version` field is never serialised under the correct key.
