#!/bin/sh
# Host test: every patch in patches/ applies to Einstein at the commit
# einstein.lock pins; on AmigaOS nothing reads $HOME, the settings, saved
# games and resources are in PROGDIR:, no SDL 1.2-only key field is left,
# and the build compiles every upstream source file.
# Needs git and the network (GitHub). Copyright (c) 2026 AmiCradle Team.
# GPL 2 or later, like Einstein.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
. "$HERE/../einstein.lock"
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
git -C "$T" init -q
git -C "$T" fetch -q --depth 1 "$EIN_GIT" "$EIN_COMMIT"
git -C "$T" checkout -q FETCH_HEAD
for p in "$HERE"/../patches/*.patch; do
    patch -s -d "$T" -p1 --dry-run < "$p" >/dev/null || { echo "FAIL: $p does not apply"; exit 1; }
    patch -s -d "$T" -p1 < "$p"
done
fail=0
# each getenv("HOME") is now on the non-Amiga side of an #ifdef __amigaos__
for f in main.cpp opensave.cpp tablestorage.cpp; do
    grep -q '__amigaos__' "$T/$f" || { echo "FAIL: $f has no AmigaOS path"; fail=1; }
done
grep -q 'L"PROGDIR:res"' "$T/main.cpp" || { echo "FAIL: resources not in PROGDIR:res"; fail=1; }
grep -q 'L"PROGDIR:User"' "$T/main.cpp" || { echo "FAIL: PROGDIR:User not made"; fail=1; }
grep -q 'L"PROGDIR:User/save"' "$T/opensave.cpp" || { echo "FAIL: saves not in PROGDIR:User/save"; fail=1; }
grep -q 'L"PROGDIR:User/einsteinrc"' "$T/tablestorage.cpp" || { echo "FAIL: settings not in PROGDIR:User"; fail=1; }
! grep -n 'keysym.unicode\|SDLK_NUMLOCK\b\|SDLK_COMPOSE' "$T"/*.cpp || { echo "FAIL: SDL 1.2 key names left"; fail=1; }
# every SDL 1.2 header Einstein includes has a compat header
for h in $(grep -ho '#include [<"]SDL/[A-Za-z_]*\.h[>"]' "$T"/*.cpp "$T"/*.h | sed 's/.*SDL\/\([A-Za-z_]*\.h\).*/\1/' | sort -u); do
    [ -f "$HERE/../compat/SDL/$h" ] || { echo "FAIL: no compat/SDL/$h"; fail=1; }
done
# the build's sources are exactly upstream's
want=$(cd "$T" && ls *.cpp | tr '\n' ' ' | sed 's/ $//')
have=$(sed -n 's/^SRCS="\(.*\)"$/\1/p' "$HERE/../build-amiga.sh")
[ "$want" = "$have" ] || { echo "FAIL: SRCS differs from upstream's *.cpp"; fail=1; }
[ -f "$T/einstein.res" ] || { echo "FAIL: no einstein.res upstream"; fail=1; }
[ $fail = 0 ] && echo "patches: all apply, paths and sources match"
exit $fail
