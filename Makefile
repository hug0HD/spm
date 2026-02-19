# ==============================================================================
# SPM - Simple Password Manager
# Cross-platform Makefile (macOS, Linux, Windows/MinGW)
# ==============================================================================

# --- Project Config ---
TARGET  = spm
SRC_DIR = src
OBJ_DIR = build

# --- Detect Platform ---
UNAME := $(shell uname 2>/dev/null || echo Windows)

ifeq ($(UNAME), Darwin)
    PLATFORM = macos
else ifeq ($(UNAME), Linux)
    PLATFORM = linux
else
    PLATFORM = windows
endif

# --- Compiler ---
ifeq ($(PLATFORM), windows)
    CC = gcc   # MinGW gcc on Windows
else
    CC = clang  # clang on macOS/Linux (can change to gcc if preferred)
endif

# --- Find all .c source files recursively ---
SRCS := $(shell find $(SRC_DIR) -name '*.c')

# --- Convert src paths to build object paths ---
# e.g. src/db/db.c -> build/db/db.o
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# --- Common Flags ---
CFLAGS  = -Wall -Wextra -Wpedantic -g -std=c11
CFLAGS += -I$(SRC_DIR)   # so you can #include "db/db.h" from anywhere

# --- Common Libraries ---
LDFLAGS = -lsqlite3 -lssl -lcrypto -lncurses

# --- Platform-Specific Flags ---
ifeq ($(PLATFORM), macos)
    CFLAGS  += -DPLATFORM_MACOS
    LDFLAGS += -framework Security   # macOS Keychain

else ifeq ($(PLATFORM), linux)
    CFLAGS  += -DPLATFORM_LINUX
    # libsecret for GNOME Keyring - install with: sudo apt install libsecret-1-dev
    LDFLAGS += $(shell pkg-config --libs libsecret-1 2>/dev/null || echo "")
    CFLAGS  += $(shell pkg-config --cflags libsecret-1 2>/dev/null || echo "")

else ifeq ($(PLATFORM), windows)
    CFLAGS  += -DPLATFORM_WINDOWS
    LDFLAGS += -lcrypt32   # Windows Credential Manager
endif

# --- Release Build (make release) ---
RELEASE_FLAGS = -O2 -DNDEBUG

# --- Install Path ---
PREFIX      = /usr/local
INSTALL_DIR = $(PREFIX)/bin

# ==============================================================================
# TARGETS
# ==============================================================================

# Default target - build the binary
all: $(TARGET)

# Link all object files into final binary
$(TARGET): $(OBJS)
	@echo "[LINK] $@"
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build complete -> ./$(TARGET)"

# Compile each .c file into a .o object file
# The | $(OBJ_DIR)/... ensures the directory exists before compiling
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "[CC]   $<"
	$(CC) $(CFLAGS) -c $< -o $@

# --- Release Build ---
release: CFLAGS += $(RELEASE_FLAGS)
release: clean $(TARGET)

# --- Run with Valgrind (memory leak check) ---
valgrind: $(TARGET)
	valgrind \
		--leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--verbose \
		./$(TARGET)

# --- Install binary to /usr/local/bin ---
install: $(TARGET)
	@echo "[INSTALL] $(INSTALL_DIR)/$(TARGET)"
	install -m 755 $(TARGET) $(INSTALL_DIR)

# --- Uninstall ---
uninstall:
	@echo "[UNINSTALL] $(INSTALL_DIR)/$(TARGET)"
	rm -f $(INSTALL_DIR)/$(TARGET)

# --- Clean build artifacts ---
clean:
	@echo "[CLEAN]"
	rm -rf $(OBJ_DIR) $(TARGET)

# --- Print detected platform and flags (useful for debugging) ---
info:
	@echo "Platform : $(PLATFORM)"
	@echo "Compiler : $(CC)"
	@echo "CFLAGS   : $(CFLAGS)"
	@echo "LDFLAGS  : $(LDFLAGS)"
	@echo "Sources  : $(SRCS)"
	@echo "Objects  : $(OBJS)"

# Tell make these aren't real files
.PHONY: all release valgrind install uninstall clean info
