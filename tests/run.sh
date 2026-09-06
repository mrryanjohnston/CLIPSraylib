#!/bin/sh
# Runs the CLIPSraylib test suite.
#
# Every tests/cases/<name>.bat is run to completion with "clips -f2", and its
# standard output has to be exactly tests/expected/<name>.out. Anything on
# standard error fails the case too: CLIPS reports a refused call there, so
# a case that starts doing that has stopped working whatever it printed.
#
# A case named window-*.bat opens a window. It is run under xvfb-run when
# that is installed, on the display DISPLAY names otherwise, and skipped
# (not failed) when there is neither. raylib writes its INFO: log lines to
# stdout, and what they say depends on the machine, so those lines are
# stripped from every case's output before it is compared.
#
# Usage:
#   ./tests/run.sh                        against ./clips
#   CLIPS=/path/to/clips ./tests/run.sh   against another binary
set -u

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$root" || exit 1

CLIPS=${CLIPS:-./clips}

if [ ! -x "$CLIPS" ]; then
    echo "no clips binary at $CLIPS -- run 'make' first," >&2
    echo "or point at one with CLIPS=/path/to/clips" >&2
    exit 1
fi

rm -rf tests/tmp
mkdir -p tests/tmp

passed=0
failed=0
skipped=0

if command -v xvfb-run >/dev/null 2>&1; then
    display="xvfb-run -a"
elif [ -n "${DISPLAY:-}" ]; then
    display=
else
    display=none
fi

for case in tests/cases/*.bat; do
    [ -f "$case" ] || continue
    name=$(basename "$case" .bat)
    expected=tests/expected/$name.out
    out=tests/tmp/$name.out
    err=tests/tmp/$name.err

    if [ ! -f "$expected" ]; then
        echo "FAILED: $name has no $expected"
        failed=$((failed + 1))
        continue
    fi

    case "$name" in
    window-*)
        if [ "$display" = none ]; then
            echo "skipped: $name (no display: install xvfb, or set DISPLAY)"
            skipped=$((skipped + 1))
            continue
        fi
        $display "$CLIPS" -f2 "$case" >"$out.raw" 2>"$err"
        status=$?
        ;;
    *)
        "$CLIPS" -f2 "$case" >"$out.raw" 2>"$err"
        status=$?
        ;;
    esac
    grep -v '^INFO: ' "$out.raw" > "$out"

    if [ "$status" -ne 0 ]; then
        echo "FAILED: $name exited $status"
        sed 's/^/    /' "$err"
        failed=$((failed + 1))
        continue
    fi

    if [ -s "$err" ]; then
        echo "FAILED: $name wrote to stderr"
        sed 's/^/    /' "$err"
        failed=$((failed + 1))
        continue
    fi

    if ! diff -u "$expected" "$out" > "tests/tmp/$name.diff"; then
        echo "FAILED: $name did not print what $expected says"
        sed 's/^/    /' "tests/tmp/$name.diff"
        failed=$((failed + 1))
        continue
    fi

    echo "passed: $name"
    passed=$((passed + 1))
done

rm -rf tests/tmp

echo
echo "$passed passed, $failed failed, $skipped skipped"
if [ "$failed" -gt 0 ] || [ "$passed" -eq 0 ]; then
    echo "FAILED"
    exit 1
fi
echo "PASSED"
