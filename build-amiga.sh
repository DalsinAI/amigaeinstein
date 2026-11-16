#!/bin/sh
# Builds Einstein for GFOS (AmigaOS 3.2.3 with GFOpenUp, on the GFOpenKick
# ROM or a Kickstart) with a stove: 68040 with its FPU (GFOpenGPU's SDL 2 kit
# is hard float), on the stove's SDL 2, SDL2_ttf and SDL2_mixer. Einstein is
# written for SDL 1.2: compat/SDL/ gives it the 1.2 calls it makes, on SDL 2.
#   STOVE=.../stoves/os32-gcc16-open6 OUT=out ./build-amiga.sh
# Einstein is fetched at the commit einstein.lock gives, with patches/ on
# it; nothing third-party is kept here.
# OUT gets Parts/Einstein: a GFOpenUp part (its Part file, the Einstein
# drawer for SYS:Games and its icon; it needs the GFOpenGPU part), installed
# with
#   GFOpenUpTool INSTALL Parts/Einstein
# and unpacked/Games, the same drawer for a lab copy.
# Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Einstein.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
. "$HERE/einstein.lock"
STOVE=${STOVE:-"$HOME/AmiCradle/kitchen/stoves/os32-gcc16-open6"}
if [ -x "$STOVE/prefix/bin/m68k-amigaos-gcc" ]; then TC="$STOVE/prefix"; else TC="$STOVE"; fi
# outside the checkout: Cradle cleans it (git clean -ffdx) before every job
WORK="${WORK:-$HOME/.cache/amigaeinstein/build}"; OUT="${OUT:-$WORK/out}"; JOBS=${JOBS:-8}
PATH="$TC/bin:$PATH"; export PATH
mkdir -p "$WORK" "$OUT"; OUT=$(cd "$OUT" && pwd)
t0=$(date +%s)

# --- Einstein at its pinned commit, with patches/ -------------------------------
SRC="$WORK/einstein"
PSUM=$(cat "$HERE"/patches/*.patch | sha256sum | cut -c1-16)
if [ "$(cat "$SRC/.patched" 2>/dev/null)" != "$EIN_COMMIT-$PSUM" ]; then
    rm -rf "$SRC"; mkdir -p "$SRC"
    git -C "$SRC" init -q
    git -C "$SRC" fetch -q --depth 1 "$EIN_GIT" "$EIN_COMMIT"
    git -C "$SRC" checkout -q FETCH_HEAD
    [ "$(git -C "$SRC" rev-parse HEAD)" = "$EIN_COMMIT" ] || { echo "commit mismatch"; exit 1; }
    for p in "$HERE"/patches/*.patch; do
        patch -s -d "$SRC" -p1 < "$p" || { echo "patch failed: $p"; exit 1; }
    done
    echo "$EIN_COMMIT-$PSUM" > "$SRC/.patched"
    rm -rf "$WORK/obj"
fi

# --- SDL 2 and zlib from the stove's SDK ------------------------------------------
SDK="$TC/m68k-amigaos"; DEPS="$SDK/port-deps"
for f in include/SDL2/SDL.h include/SDL2/SDL_ttf.h include/SDL2/SDL_mixer.h lib/libSDL2.a lib/libSDL2_ttf.a lib/libSDL2_mixer.a port-deps/lib/libz.a port-deps/include/zlib.h; do
    [ -f "$SDK/$f" ] || { echo "the stove has no $f"; exit 2; }
done

# --- one Makefile -----------------------------------------------------------------
# Every *.cpp upstream builds (its Makefile's OBJECTS; tests/test-patches.sh
# checks this list against upstream's tree).
SRCS="buffer.cpp conf.cpp convert.cpp descr.cpp font.cpp formatter.cpp game.cpp horhints.cpp i18n.cpp iconset.cpp lexal.cpp main.cpp menu.cpp messages.cpp opensave.cpp options.cpp puzgen.cpp puzzle.cpp random.cpp regstorage.cpp resources.cpp rules.cpp screen.cpp sound.cpp storage.cpp streams.cpp table.cpp tablestorage.cpp tokenizer.cpp topscores.cpp unicode.cpp utils.cpp verthints.cpp widgets.cpp"
# -fno-tree-loop-distribute-patterns: GCC 16's loop miscompile (the kit's
# sdl2-config gives it too).
CF="-noixemul -m68040 -m68881 -O2 -fomit-frame-pointer -fno-tree-loop-distribute-patterns"
O="$WORK/obj"; mkdir -p "$O"
M="$O/Makefile"
{
    echo "CXXF = $CF -std=gnu++11 -include $HERE/compat/prelude.h -I$HERE/compat -I$SDK/include/SDL2 -I$DEPS/include -I$SRC -DPREFIX=L\\\"PROGDIR:\\\" -Wno-deprecated-declarations"
    A=""
    for f in $SRCS; do o="${f%.cpp}.o"; A="$A $o"; printf '%s: %s\n\tm68k-amigaos-g++ $(CXXF) -MMD -c $< -o $@\n' "$o" "$SRC/$f"; done
    printf 'sdl12on2.o: %s\n\tm68k-amigaos-gcc %s -I%s -I%s -c $< -o $@\n' "$HERE/compat/sdl12on2.c" "$CF" "$HERE/compat" "$SDK/include/SDL2"
    printf 'wcstod.o: %s\n\tm68k-amigaos-gcc %s -c $< -o $@\n' "$HERE/compat/wcstod.c" "$CF"
    printf 'latin1.o: %s\n\tm68k-amigaos-gcc %s -c $< -o $@\n' "$HERE/compat/latin1.c" "$CF"
    printf 'wmem.o: %s\n\tm68k-amigaos-gcc %s -fno-builtin -c $< -o $@\n' "$HERE/compat/wmem.c" "$CF"
    printf 'wide.o: %s\n\tm68k-amigaos-gcc %s -c $< -o $@\n' "$HERE/compat/wide.c" "$CF"
    printf 'amiga_start.o: %s\n\tm68k-amigaos-gcc %s -c $< -o $@\n' "$HERE/amiga/amiga_start.c" "$CF"
    echo "OBJS =$A sdl12on2.o wcstod.o latin1.o wmem.o wide.o amiga_start.o"
    echo "all: \$(OBJS)"
    echo "-include *.d"
} > "$M"
set +e
(cd "$O" && nice -n 19 make -k -j"$JOBS" all > make.log 2>&1)
rc=$?
set -e
grep -E 'error|Error' "$O/make.log" | head -60
[ "$rc" = 0 ] || { tail -20 "$O/make.log"; exit "$rc"; }
echo "compile: $(( $(date +%s) - t0 )) s"

# --- the link ---------------------------------------------------------------------
OBJS=$(cd "$O" && for f in $(sed -n 's/^OBJS =//p' Makefile); do echo "$O/$f"; done)
# shellcheck disable=SC2086
m68k-amigaos-g++ -noixemul -m68040 -m68881 -o "$O/Einstein.debug" $OBJS \
    -L"$SDK/lib" -L"$DEPS/lib" -lSDL2_ttf -lSDL2_mixer -lSDL2 -lz -lm -lpthread -latomic > "$O/link.log" 2>&1 || { head -60 "$O/link.log"; exit 1; }

# --- the GFOpenUp part: Parts/Einstein ----------------------------------------------
P="$OUT/Parts/Einstein"; rm -rf "$P"; D="$P/Games/Einstein"; mkdir -p "$D/res"
m68k-amigaos-strip -o "$D/Einstein" "$O/Einstein.debug"
cp "$SRC/einstein.res" "$D/res/einstein.res"
cp "$HERE/README.md" "$D/ReadMe"
cp "$SRC/LICENSE" "$D/COPYING.txt"
I="$HERE/amiga/icons"
cp "$I/Einstein.info" "$D/Einstein.info"
for f in ReadMe COPYING.txt; do cp "$I/ReadMe.info" "$D/$f.info"; done
cp "$I/Data.info" "$D/res.info"
cp "$I/Einstein-drawer.info" "$P/Games/Einstein.info"
sh "$HERE/scripts/write-part.sh" "$P" "$EIN_VERSION"
rm -rf "$OUT/unpacked"; mkdir -p "$OUT/unpacked"; cp -R "$P/Games" "$OUT/unpacked/"
ls -l "$D/Einstein"
echo "part: $(grep -c '^copy' "$P/Part") files, $(du -sk "$P" | cut -f1) KB"
echo "total: $(( $(date +%s) - t0 )) s"
