#!/bin/sh
# Host test for scripts/write-part.sh: the Part file GFOpenUpTool reads.
# Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Einstein.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
mkdir -p "$T/Games/Einstein/res"
: > "$T/Games/Einstein/Einstein"; : > "$T/Games/Einstein.info"; : > "$T/Games/Einstein/res/einstein.res"
sh "$HERE/../scripts/write-part.sh" "$T" 2.0
fail=0
expect() { grep -qxF "$1" "$T/Part" || { echo "FAIL: no line: $1"; fail=1; }; }
expect 'part Einstein 2.0'
expect 'needs 68040'
expect 'needs part GFOpenGPU'
expect 'copy "Games/Einstein.info" "SYS:Games/Einstein.info"'
expect 'copy "Games/Einstein/Einstein" "SYS:Games/Einstein/Einstein"'
expect 'copy "Games/Einstein/res/einstein.res" "SYS:Games/Einstein/res/einstein.res"'
[ "$(head -1 "$T/Part")" = 'part Einstein 2.0' ] || { echo "FAIL: first line"; fail=1; }
[ "$(grep -c '^copy' "$T/Part")" = 3 ] || { echo "FAIL: copy count (the Part file itself must not be listed)"; fail=1; }
sh "$HERE/../scripts/write-part.sh" "$T" 2.0   # a second run: same file
[ "$(grep -c '^copy' "$T/Part")" = 3 ] || { echo "FAIL: second run lists the Part file"; fail=1; }
[ $fail = 0 ] && echo "Part file: all checks passed"
exit $fail
