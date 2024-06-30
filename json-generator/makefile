# Define variables
WORKSPACE_FOLDER := $(CURDIR)
BUILD_TYPE ?= debug

# Build flavors
BUILD_FLAVORS := debug release tests

# Directories
BUILD_DIR := $(WORKSPACE_FOLDER)/build/$(BUILD_TYPE)
SRC_DIR := $(WORKSPACE_FOLDER)/src
INCLUDE_DIR := $(WORKSPACE_FOLDER)/include
TREE_SITTER_DIR := $(WORKSPACE_FOLDER)/../tree-sitter
TESTS_DIR := $(WORKSPACE_FOLDER)/tests
OSX_LIB_DIR := $(WORKSPACE_FOLDER)/lib/osx

# Compiler and linker
ifeq ($(OS),Windows_NT)
    CC := $(MINGWPATH)/gcc
else
    CC := cc
endif

CFLAGS := -std=c99 \
          -I$(INCLUDE_DIR) \
          -I$(SRC_DIR) \
          -I$(TESTS_DIR) \
          -I$(TREE_SITTER_DIR)\lib\src \
          -I$(TREE_SITTER_DIR)\lib\include
LDFLAGS :=

# Source files
ifeq ($(OS),Windows_NT)
    define FIND_FILES
    for /r $(1) %i in (*.c) do @echo %i
    endef
    SRC_FILES := $(shell cmd /c $(call FIND_FILES,$(SRC_DIR)))
    TEST_FILES := $(shell cmd /c $(call FIND_FILES,$(TESTS_DIR)))
else
    SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
    TEST_FILES := $(shell find $(TESTS_DIR) -name '*.c')
endif

# Common settings
COMMON_ARGS := -std=c99

# Arguments for debug build
DEBUG_ARGS := -g \
              -O0 \
              -fdiagnostics-color=always \
              -Wno-missing-braces \
              -Wall \

# Arguments for release build
RELEASE_ARGS := -O2

# Arguments for unit test build
TESTS_ARGS := -g \
              -O0 \
              -fdiagnostics-color=always \
              -Wno-missing-braces \
              -Wall \
              -DUNIT_TEST \

# OS-specific settings
ifeq ($(OS),Windows_NT)
    OUTPUT := $(BUILD_DIR)\json-generator.exe
    PLATFORM_ARGS := -Llib\windows \
                     -ltree-sitter \
                     -ltree-sitter-c \
    
    MKDIR := if not exist "$(subst /,\,$(BUILD_DIR))" mkdir "$(subst /,\,$(BUILD_DIR))"
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        OUTPUT := $(BUILD_DIR)/json-generator
        PLATFORM_ARGS := -L$(OSX_LIB_DIR) \
                         -ltree-sitter \
                         -Wl,-rpath,$(OSX_LIB_DIR)
        MKDIR := mkdir -p $(BUILD_DIR)
    else
        $(error Unsupported OS)
    endif
endif

# Set build-specific arguments
ifeq ($(BUILD_TYPE),debug)
    BUILD_ARGS := $(DEBUG_ARGS)
    FILES := $(SRC_FILES)
else ifeq ($(BUILD_TYPE),release)
    BUILD_ARGS := $(RELEASE_ARGS)
    FILES := $(SRC_FILES)
else ifeq ($(BUILD_TYPE),tests)
    BUILD_ARGS := $(TESTS_ARGS)
    FILES := $(TEST_FILES)
else
    $(error Unknown build type $(BUILD_TYPE))
endif

# Targets
.PHONY: all clean $(BUILD_FLAVORS) clean-debug clean-release clean-tests

all: $(OUTPUT)

$(OUTPUT): $(FILES)
	@$(MKDIR)
	$(CC) $(COMMON_ARGS) $(BUILD_ARGS) $(CFLAGS) $(FILES) -o $(OUTPUT) $(PLATFORM_ARGS)

clean: clean-debug clean-release clean-tests

clean-debug:
ifeq ($(OS),Windows_NT)
	@if exist "$(subst /,\,$(WORKSPACE_FOLDER)/build/debug)" rmdir /s /q "$(subst /,\,$(WORKSPACE_FOLDER)/build/debug)" 2>nul
else
	@rm -rf "$(WORKSPACE_FOLDER)/build/debug"
endif

clean-release:
ifeq ($(OS),Windows_NT)
	@if exist "$(subst /,\,$(WORKSPACE_FOLDER)/build/release)" rmdir /s /q "$(subst /,\,$(WORKSPACE_FOLDER)/build/release)" 2>nul
else
	@rm -rf "$(WORKSPACE_FOLDER)/build/release"
endif

clean-tests:
ifeq ($(OS),Windows_NT)
	@if exist "$(subst /,\,$(WORKSPACE_FOLDER)/build/tests)" rmdir /s /q "$(subst /,\,$(WORKSPACE_FOLDER)/build/tests)" 2>nul
else
	@rm -rf "$(WORKSPACE_FOLDER)/build/tests"
endif

$(BUILD_FLAVORS):
	$(MAKE) clean BUILD_TYPE=$@
	$(MAKE) all BUILD_TYPE=$@
