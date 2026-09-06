# Which CLIPS:
#
#   make                        the 6.4.2 release tarball (the default)
#   make CLIPS_VERSION=svn-6x   branches/64x of the CLIPS Subversion repository
#   make CLIPS_VERSION=svn-7x   branches/70x
#
# Specify RAYLIB_VERSION to change the version of raylib used.
# You can combine the two:
#
#   make CLIPS_VERSION=svn-7x RAYLIB_VERSION=6.0
#
# "make help" lists the targets and what each variable accepts.

.DEFAULT_GOAL := all

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  CLIPS_OS := DARWIN
else
  CLIPS_OS := LINUX
endif

# ---------------------------------------------------------------------------
# Which CLIPS to build against.
#
#   6.4.2    the 6.4.2 release tarball from SourceForge (the default)
#   svn-6x   branches/64x of the CLIPS Subversion repository
#   svn-7x   branches/70x
#
# The two branches are pinned to a revision with CLIPS_SVN_REV:
#
#   make CLIPS_VERSION=svn-7x
#   make CLIPS_VERSION=svn-7x CLIPS_SVN_REV=978
#   make CLIPS_VERSION=svn-7x CLIPS_SVN_REV=HEAD
#
# Each version is fetched into vendor/clips-source/<tag> and built in a copy of
# it under vendor/clips-build/<tag>.
# ---------------------------------------------------------------------------

CLIPS_VERSION  ?= 6.4.2
CLIPS_VERSIONS := 6.4.2 svn-6x svn-7x

ARCHIVE     := clips_core_source_642.tar.gz
ARCHIVE_URL ?= https://sourceforge.net/projects/clipsrules/files/CLIPS/6.4.2/$(ARCHIVE)

CLIPS_SVN_ROOT   ?= https://svn.code.sf.net/p/clipsrules/code
CLIPS_SVN_6X_REV ?= 967
CLIPS_SVN_7X_REV ?= 978

# The digest SourceForge publishes for clips_core_source_642.tar.gz. It applies
# to the release only; a branch export has no such file to check, and the
# variable is empty there, which tells the fetch script to skip the check.
CLIPS_SHA256_6.4.2 := 608a1eb2fc6e9caff30d63d684095f0bca7108f2294d21ee6f5617427c10455a
CLIPS_SHA256 ?= $(CLIPS_SHA256_$(CLIPS_VERSION))
export CLIPS_SHA256

ifeq ($(CLIPS_VERSION),6.4.2)
  CLIPS_TAG    := 6.4.2
  CLIPS_FETCH   = tarball "$(ARCHIVE_URL)" "$(ARCHIVE)"
  CLIPS_ORIGIN := the 6.4.2 release tarball
else ifeq ($(CLIPS_VERSION),svn-6x)
  CLIPS_SVN_URL ?= $(CLIPS_SVN_ROOT)/branches/64x/core
  CLIPS_SVN_REV ?= $(CLIPS_SVN_6X_REV)
  CLIPS_TAG     := svn-6x-r$(CLIPS_SVN_REV)
  CLIPS_FETCH    = svn "$(CLIPS_SVN_URL)" "$(CLIPS_SVN_REV)"
  CLIPS_ORIGIN  := branches/64x at r$(CLIPS_SVN_REV)
else ifeq ($(CLIPS_VERSION),svn-7x)
  CLIPS_SVN_URL ?= $(CLIPS_SVN_ROOT)/branches/70x/core
  CLIPS_SVN_REV ?= $(CLIPS_SVN_7X_REV)
  CLIPS_TAG     := svn-7x-r$(CLIPS_SVN_REV)
  CLIPS_FETCH    = svn "$(CLIPS_SVN_URL)" "$(CLIPS_SVN_REV)"
  CLIPS_ORIGIN  := branches/70x at r$(CLIPS_SVN_REV)
else
  $(error CLIPS_VERSION is '$(CLIPS_VERSION)': expected one of $(CLIPS_VERSIONS))
endif

CLIPS_SRC_DIR := vendor/clips-source/$(CLIPS_TAG)
BUILD_DIR     := vendor/clips-build/$(CLIPS_TAG)

CLIPS_SRC_STAMP := $(CLIPS_SRC_DIR)/.clips-source
BUILD_STAMP     := $(BUILD_DIR)/.clips-source
CONFIG_STAMP    := $(BUILD_DIR)/.build-config

# The executable lands in the root of the repository, where the README, the
# examples and the tests look for it.
BINARY := $(CURDIR)/clips

# Which binary the tests run. Empty means the one this build produces.
CLIPS   ?=
CLIPS_BIN = $(if $(strip $(CLIPS)),$(CLIPS),$(BINARY))

# ---------------------------------------------------------------------------
# Which raylib to build against.
#
#     make                          raylib 6.0 (the default)
#     make RAYLIB_VERSION=5.5
#     make RAYLIB_VERSION=main      a shallow clone of the main branch
#
# A release that is not in the table needs its URL and digest passed with it:
#
#     make RAYLIB_VERSION=5.0 \
#          RAYLIB_URL=https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz \
#          RAYLIB_SHA256=<the digest of that file>
#
# The wrappers use raylib 5.5 API (DrawRectangleRoundedLinesEx, ColorLerp), so
# a release older than 5.5 will not compile without changes.
# ---------------------------------------------------------------------------

RAYLIB_VERSION  ?= 6.0
RAYLIB_VERSIONS := 5.5 6.0 main

RAYLIB_URL_5.5    := https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
RAYLIB_SHA256_5.5 := aea98ecf5bc5c5e0b789a76de0083a21a70457050ea4cc2aec7566935f5e258e
RAYLIB_URL_6.0    := https://github.com/raysan5/raylib/archive/refs/tags/6.0.tar.gz
RAYLIB_SHA256_6.0 := 2b3ee1e2120c7a0796b33062c7e9a694dd8a8caa56a96319ac8c8ecf54a90d0b
RAYLIB_GIT_main   := https://github.com/raysan5/raylib.git

RAYLIB_URL    ?= $(RAYLIB_URL_$(RAYLIB_VERSION))
RAYLIB_SHA256 ?= $(RAYLIB_SHA256_$(RAYLIB_VERSION))
RAYLIB_GIT    ?= $(RAYLIB_GIT_$(RAYLIB_VERSION))

ifneq ($(RAYLIB_GIT),)
  RAYLIB_FETCH  = git "$(RAYLIB_GIT)"
  RAYLIB_ORIGIN = the main branch of $(RAYLIB_GIT)
else ifneq ($(RAYLIB_URL),)
  RAYLIB_FETCH  = tarball "$(RAYLIB_URL)" "$(RAYLIB_SHA256)"
  RAYLIB_ORIGIN = the $(RAYLIB_VERSION) release tarball
else
  $(error RAYLIB_VERSION is '$(RAYLIB_VERSION)': expected one of $(RAYLIB_VERSIONS), or pass RAYLIB_URL and RAYLIB_SHA256 with it)
endif

RAYLIB_DIR := $(CURDIR)/vendor/raylib-$(RAYLIB_VERSION)
RAYLIB_SRC := $(RAYLIB_DIR)/src
RAYLIB_LIB := $(RAYLIB_DIR)/libraylib.a

RAYLIB_SRC_STAMP := $(RAYLIB_DIR)/.raylib-source
RAYLIB_FLAGS     := $(RAYLIB_DIR)/.build-flags

# ---------------------------------------------------------------------------
# The rename.
#
# raylib and the CLIPS core both export a function called GetFileName.
# Every name
# in RAYLIB_RENAMES is compiled in raylib and in raylib_glue.c with a
# Raylib prefix in front of it. "make check-symbols" compares the two symbol
# tables and fails on any clash that is not in this list.
# ---------------------------------------------------------------------------

RAYLIB_RENAMES      := GetFileName
RAYLIB_RENAME_FLAGS := $(foreach name,$(RAYLIB_RENAMES),-D$(name)=Raylib$(name))

# Extra flags for compiling raylib and raylib_glue.c, for a machine that keeps
# its X11 or OpenGL headers somewhere the compiler does not look by default:
#
#     make RAYLIB_CFLAGS=-I/opt/x11/include
RAYLIB_CFLAGS ?=

# What raylib needs from the system at link time, on top of the archive.
ifeq ($(UNAME_S),Darwin)
  RAYLIB_SYSLIBS := -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
else
  RAYLIB_SYSLIBS := -lGL -lm -lpthread -ldl -lrt -lX11
endif

RAYLIB_LDLIBS := $(RAYLIB_LIB) $(RAYLIB_SYSLIBS)

RAYLIB_MAKE_ARGS := PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=STATIC \
                    RAYLIB_RELEASE_PATH="$(RAYLIB_DIR)" \
                    CUSTOM_CFLAGS="$(RAYLIB_RENAME_FLAGS) $(RAYLIB_CFLAGS)"

SOURCES := userfunctions.c raylib_glue.c raylib_glue.h raylib_standalone.c raygui.h

SUBMAKE = $(MAKE) -C $(BUILD_DIR) BINARY="$(BINARY)" CLIPS_OS=$(CLIPS_OS) \
          RAYLIB_SRC="$(RAYLIB_SRC)" RAYLIB_LIB="$(RAYLIB_LIB)" \
          RAYLIB_LDLIBS="$(RAYLIB_LDLIBS)" \
          RAYLIB_GLUE_CFLAGS="$(RAYLIB_RENAME_FLAGS) $(RAYLIB_CFLAGS)"

BUILD_CONFIG := raylib=$(RAYLIB_VERSION) renames=$(RAYLIB_RENAMES) \
                cflags=$(RAYLIB_CFLAGS) ldlibs=$(RAYLIB_LDLIBS)

.PHONY: all clips debug clips-source stage raylib raylib-source \
        check-symbols test clean distclean help FORCE \
        print-clips print-clips-versions print-raylib print-raylib-versions \
        print-raylib-ldlibs print-raylib-renames

$(CLIPS_SRC_STAMP): | scripts/fetch-clips.sh
	./scripts/fetch-clips.sh $(CLIPS_FETCH) "$(CLIPS_SRC_DIR)"

$(BUILD_STAMP): $(CLIPS_SRC_STAMP)
	mkdir -p "$(BUILD_DIR)"
	cp -R "$(CLIPS_SRC_DIR)/." "$(BUILD_DIR)/"
	touch "$@"

clips-source: $(CLIPS_SRC_STAMP) ## Fetch the selected CLIPS source and stop
	@cat "$(CLIPS_SRC_STAMP)"

$(RAYLIB_SRC_STAMP): | scripts/fetch-raylib.sh
	./scripts/fetch-raylib.sh $(RAYLIB_FETCH) "$(RAYLIB_DIR)"

raylib-source: $(RAYLIB_SRC_STAMP) ## Fetch the selected raylib source and stop
	@cat "$(RAYLIB_SRC_STAMP)"

FORCE:

$(RAYLIB_FLAGS): $(RAYLIB_SRC_STAMP) FORCE
	@printf '%s\n' '$(RAYLIB_MAKE_ARGS)' | cmp -s - $@ 2>/dev/null || \
	    printf '%s\n' '$(RAYLIB_MAKE_ARGS)' > $@

$(RAYLIB_LIB): $(RAYLIB_SRC_STAMP) $(RAYLIB_FLAGS)
	rm -f "$(RAYLIB_SRC)"/*.o "$@"
	$(MAKE) -C "$(RAYLIB_SRC)" $(RAYLIB_MAKE_ARGS)
	@test -f "$@" || { echo "raylib built but $@ is not there" >&2; exit 1; }

raylib: $(RAYLIB_LIB) ## Fetch, verify and build the selected raylib only
	@echo "$(RAYLIB_LIB)"

define check_build_config
	@if [ ! -f "$(CONFIG_STAMP)" ] || \
	   [ "$$(cat '$(CONFIG_STAMP)')" != '$(BUILD_CONFIG)' ]; then \
		if [ -f "$(CONFIG_STAMP)" ]; then \
			echo "the build configuration changed, so this build starts from clean"; \
			$(SUBMAKE) clean >/dev/null 2>&1 || true; \
		fi; \
		printf '%s\n' '$(BUILD_CONFIG)' > "$(CONFIG_STAMP)"; \
	fi
endef

stage: $(BUILD_STAMP) $(RAYLIB_LIB)
	@cp -p $(SOURCES) $(BUILD_DIR)/
	@cp -p clips.mk $(BUILD_DIR)/makefile
	$(check_build_config)

all: stage ## Build clips against the selected CLIPS and raylib (the default)
	$(SUBMAKE) release

clips: all ## Synonym for the default build

debug: stage ## Build with debugging symbols and no optimisation
	$(SUBMAKE) debug

check-symbols: all ## Fail on a name defined by both the CLIPS core and raylib
	./scripts/check-symbols.sh "$(BUILD_DIR)" "$(RAYLIB_SRC)" "$(RAYLIB_RENAMES)"

test: all ## Run the test suite against the current build
	CLIPS="$(CLIPS_BIN)" ./tests/run.sh

print-clips: ## Say which CLIPS this build uses and where it is
	@echo 'CLIPS_VERSION $(CLIPS_VERSION)'
	@echo 'origin        $(CLIPS_ORIGIN)'
	@echo 'source        $(CLIPS_SRC_DIR)'
	@echo 'build         $(BUILD_DIR)'
	@echo 'binary        $(BINARY)'
	@[ -f "$(CLIPS_SRC_STAMP)" ] && printf 'fetched       ' && cat "$(CLIPS_SRC_STAMP)" || true

print-clips-versions:
	@echo '$(CLIPS_VERSIONS)'

print-raylib: ## Say which raylib this build uses and where it is
	@echo 'RAYLIB_VERSION $(RAYLIB_VERSION)'
	@echo 'origin         $(RAYLIB_ORIGIN)'
	@echo 'source         $(RAYLIB_DIR)'
	@echo 'archive        $(RAYLIB_LIB)'
	@echo 'renames        $(RAYLIB_RENAMES)'
	@[ -f "$(RAYLIB_SRC_STAMP)" ] && printf 'fetched        ' && cat "$(RAYLIB_SRC_STAMP)" || true

print-raylib-versions:
	@echo '$(RAYLIB_VERSIONS)'

print-raylib-ldlibs: ## Print the raylib archive and the system libraries it needs
	@echo '$(RAYLIB_LDLIBS)'

print-raylib-renames:
	@echo '$(RAYLIB_RENAMES)'

clean: ## Remove the build trees, the binary and the raylib archive
	-rm -rf vendor/clips-build "$(BINARY)" tests/tmp
	-rm -f vendor/raylib-*/libraylib.a vendor/raylib-*/src/*.o vendor/raylib-*/.build-flags

distclean: clean ## Also remove the fetched CLIPS and raylib sources
	rm -rf vendor
	rm -f "$(ARCHIVE)"

help: ## List these targets
	@printf 'CLIPSraylib targets:\n\n'
	@grep -E '^[a-zA-Z0-9_%-]+:.*## ' $(MAKEFILE_LIST) \
		| awk -F':.*## ' '{ printf "  %-20s %s\n", $$1, $$2 }' \
		| sort
	@printf '\nThis build uses CLIPS %s: %s.\n' '$(CLIPS_VERSION)' '$(CLIPS_ORIGIN)'
	@printf '\n  CLIPS_VERSION=6.4.2   the release tarball from SourceForge\n'
	@printf '  CLIPS_VERSION=svn-6x  branches/64x, pinned at r%s\n' '$(CLIPS_SVN_6X_REV)'
	@printf '  CLIPS_VERSION=svn-7x  branches/70x, pinned at r%s\n' '$(CLIPS_SVN_7X_REV)'
	@printf '  CLIPS_SVN_REV=        build a branch at another revision, or at\n'
	@printf '                        HEAD (needs svn installed)\n'
	@printf '\nIt uses raylib %s: %s.\n' '$(RAYLIB_VERSION)' '$(RAYLIB_ORIGIN)'
	@printf '\n  RAYLIB_VERSION=       one of: %s\n' '$(RAYLIB_VERSIONS)'
	@printf '  RAYLIB_URL= RAYLIB_SHA256=\n'
	@printf '                        a release the makefile does not know\n'
	@printf '  RAYLIB_CFLAGS=        extra flags for raylib and raylib_glue.c\n'
	@printf '\nOther variables: CLIPS (which binary the tests run).\n'
