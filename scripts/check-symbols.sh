#!/bin/sh
# Fails if the CLIPS core and raylib define a function of the same name.
#
# The CLIPS core is built exactly as it comes, so any name it shares with
# raylib has to be moved on the raylib side, with -D<name>=Raylib<name>
# (RAYLIB_RENAMES in the makefile). This check keeps that list honest: it
# takes every global symbol the core objects define and every RLAPI function
# raylib.h and rlgl.h declare, and a name in both lists that is not in the
# rename list fails the check. Today the one match is GetFileName.
#
# Usage:
#   scripts/check-symbols.sh <clips-build-dir> <raylib-src-dir> "<renames>"
#
# The core objects are every .o in the build directory except main.o and the
# three this project adds: userfunctions.o, raylib_glue.o and
# raylib_standalone.o.
set -eu

build_dir=${1:?usage: check-symbols.sh <clips-build-dir> <raylib-src-dir> "<renames>"}
raylib_src=${2:?usage: check-symbols.sh <clips-build-dir> <raylib-src-dir> "<renames>"}
renames=${3:-}

die() { echo "check-symbols: $*" >&2; exit 1; }

command -v nm >/dev/null 2>&1 || die "nm is not installed"
[ -d "$build_dir" ] || die "no build directory at $build_dir: run make first"
[ -f "$raylib_src/raylib.h" ] || die "no raylib.h in $raylib_src: run make raylib first"

tmp=$(mktemp -d "${TMPDIR:-/tmp}/check-symbols.XXXXXX")
trap 'rm -rf "$tmp"' EXIT INT TERM

objects=
for object in "$build_dir"/*.o; do
    case "$(basename "$object")" in
    main.o|userfunctions.o|raylib_glue.o|raylib_standalone.o) continue ;;
    esac
    objects="$objects $object"
done
[ -n "$objects" ] || die "no core objects in $build_dir: run make first"

# nm prints "address type name" for a defined global; macOS puts a leading
# underscore on C names, which is not part of the name.
# shellcheck disable=SC2086
nm -g --defined-only $objects \
    | awk 'NF == 3 { print $3 }' \
    | sed 's/^_//' \
    | sort -u > "$tmp/core"

# Every RLAPI declaration, reduced to the identifier before its parameter
# list. "RLAPI const char *GetFileName(const char *filePath);" -> GetFileName.
cat "$raylib_src/raylib.h" "$raylib_src/rlgl.h" \
    | grep -E '^[[:space:]]*RLAPI[[:space:]]' \
    | grep -E '\(' \
    | sed -E 's/\(.*//; s/.*[[:space:]*]([A-Za-z_][A-Za-z0-9_]*)[[:space:]]*$/\1/' \
    | grep -E '^[A-Za-z_][A-Za-z0-9_]*$' \
    | sort -u > "$tmp/rlapi"

[ -s "$tmp/rlapi" ] || die "found no RLAPI declarations in $raylib_src/raylib.h"

comm -12 "$tmp/core" "$tmp/rlapi" > "$tmp/clashes"

printf '%s\n' $renames | sort -u > "$tmp/renamed"

# Clashes the rename list does not cover.
comm -23 "$tmp/clashes" "$tmp/renamed" > "$tmp/unhandled"
# Names in the rename list that clash with nothing any more.
comm -13 "$tmp/clashes" "$tmp/renamed" > "$tmp/stale"

core_count=$(wc -l < "$tmp/core" | tr -d ' ')
rlapi_count=$(wc -l < "$tmp/rlapi" | tr -d ' ')
echo "check-symbols: $core_count global symbols in the CLIPS core, $rlapi_count RLAPI functions in raylib"

if [ -s "$tmp/clashes" ]; then
    echo "check-symbols: defined by both:"
    while IFS= read -r name; do
        if grep -qx "$name" "$tmp/renamed"; then
            echo "  $name (renamed to Raylib$name)"
        else
            echo "  $name (NOT renamed)"
        fi
    done < "$tmp/clashes"
else
    echo "check-symbols: defined by both: none"
fi

if [ -s "$tmp/stale" ]; then
    echo "check-symbols: in RAYLIB_RENAMES but clashing with nothing:"
    sed 's/^/  /' "$tmp/stale"
fi

if [ -s "$tmp/unhandled" ]; then
    echo "check-symbols: FAILED. Add each name marked NOT renamed to RAYLIB_RENAMES" >&2
    echo "in the makefile, and check that raylib_glue.c calls it under its new name." >&2
    exit 1
fi

echo "check-symbols: PASSED"
