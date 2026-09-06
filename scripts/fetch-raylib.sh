#!/bin/sh
# Fetches one raylib source tree into a directory.
#
# Two places a tree can come from: a release tarball from GitHub, checked
# against the SHA-256 the makefile pins for it, or a git clone of the main
# branch for building against what raylib is about to become.
#
# Usage:
#   scripts/fetch-raylib.sh tarball <url> <sha256> <dest-dir>
#   scripts/fetch-raylib.sh git     <url> <dest-dir>
#
# The tarball is kept beside the destination as <dest-dir>.tar.gz, so a
# second fetch of the same release reads the cached file (and checks it
# again) rather than downloading. A git clone is shallow and is not updated
# once it exists: remove the directory to fetch main again.
#
# On success the tree is in <dest-dir>, with src/raylib.h in it, and
# <dest-dir>/.raylib-source says what it is.
set -eu

kind=${1:?usage: fetch-raylib.sh <tarball|git> <url> [<sha256>] <dest-dir>}

die() { echo "fetch-raylib: $*" >&2; exit 1; }

fetch() {
    if command -v curl >/dev/null 2>&1; then
        curl -fsSL -o "$2" "$1"
    elif command -v wget >/dev/null 2>&1; then
        wget -q -O "$2" "$1"
    else
        die "neither curl nor wget is installed"
    fi
}

sha256() {
    if command -v sha256sum >/dev/null 2>&1; then
        sha256sum "$1" | cut -d' ' -f1
    elif command -v shasum >/dev/null 2>&1; then
        shasum -a 256 "$1" | cut -d' ' -f1
    else
        die "no SHA-256 tool found: need sha256sum or shasum"
    fi
}

case "$kind" in
tarball)
    url=${2:?usage: fetch-raylib.sh tarball <url> <sha256> <dest-dir>}
    want=${3:?usage: fetch-raylib.sh tarball <url> <sha256> <dest-dir>}
    dest=${4:?usage: fetch-raylib.sh tarball <url> <sha256> <dest-dir>}
    ;;
git)
    url=${2:?usage: fetch-raylib.sh git <url> <dest-dir>}
    dest=${3:?usage: fetch-raylib.sh git <url> <dest-dir>}
    want=
    ;;
*)
    die "unknown source kind '$kind': expected tarball or git"
    ;;
esac

# A half-written tree is worse than no tree, because make would take it for
# a finished one. Everything lands beside the destination and is moved into
# place only once it is whole.
staging="$dest.incoming"
rm -rf "$staging"
mkdir -p "$(dirname "$dest")"
trap 'rm -rf "$staging"' EXIT INT TERM

case "$kind" in
tarball)
    [ -n "$want" ] || die "no SHA-256 given for $url: a release is only built
   from a tarball whose digest the makefile pins. Pass RAYLIB_SHA256=<digest>
   with RAYLIB_URL for a release the makefile does not know."

    archive="$dest.tar.gz"
    if [ ! -f "$archive" ]; then
        echo "fetch-raylib: downloading $url"
        fetch "$url" "$archive.incoming" || die "download failed: $url"
        mv "$archive.incoming" "$archive"
    else
        echo "fetch-raylib: using the archive already at $archive"
    fi

    # Checked on the cached archive as well as on a fresh download, so a
    # file that went bad on disk is caught too.
    got=$(sha256 "$archive")
    if [ "$got" != "$want" ]; then
        die "SHA-256 mismatch for $archive
   expected $want
   actual   $got
   Nothing has been unpacked. Either the download was corrupted -- remove
   that file and build again -- or the file GitHub is serving is no longer
   the one this project was tested against, which is worth understanding
   before building."
    fi
    echo "fetch-raylib: SHA-256 $got verified"

    mkdir -p "$staging"
    # raylib-5.5/src/raylib.h -> src/raylib.h
    tar --strip-components=1 -xzf "$archive" -C "$staging" \
        || die "could not unpack $archive"
    description="tarball $url sha256 $got"
    ;;

git)
    command -v git >/dev/null 2>&1 || die "git is not installed: it is needed
   to build against raylib's main branch. Building against a release
   instead needs no git: run make with RAYLIB_VERSION=6.0, the default."

    echo "fetch-raylib: cloning $url"
    git clone --quiet --depth 1 "$url" "$staging" \
        || die "git clone failed: $url"
    commit=$(git -C "$staging" rev-parse HEAD)
    rm -rf "$staging/.git"
    description="git $url at $commit"
    ;;
esac

[ -f "$staging/src/raylib.h" ] || die "no src/raylib.h in the tree fetched from $url"

printf '%s\n' "$description" > "$staging/.raylib-source"

rm -rf "$dest"
mv "$staging" "$dest"
trap - EXIT INT TERM

echo "fetch-raylib: $description -> $dest"
