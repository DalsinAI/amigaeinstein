Einstein for GFOS
=================

A port of Einstein 2.0, Alexander Babichev's free remake of the DOS game
Sherlock, the logic puzzle after Einstein's riddle, from
https://github.com/lksj/einstein-puzzle at commit
db34f7a51c174095dda31f54ecbbd4f1a325e16a, for GFOS: AmigaOS 3.2 with
GFOpenUp, on a 68040 or 68060 with an FPU, on GFOpenUp's SDL 2.

Open all 36 cards of the 6 x 6 square. Each row holds one kind of card
(digits, letters, Roman numerals, dice, shapes, signs); the hints below
and beside the square say which cards share a column, sit side by side,
or lie left of or between others. Rule them out until only one is left.

Status: built in Cradle's os32-gcc16-open6 stove; not yet run in a lab.


What you need
-------------

- GFOS: AmigaOS 3.2 with GFOpenUp and its GFOpenGPU part, which installs
  LIBS:GFOpenGPU/SDL2.module and SDL2_mixer.module.
- A 68040 or 68060 with an FPU (not an LC040 or EC040), an RTG screen that
  shows 800 x 600 (or the AmiCradle lab), and about 4 MB of free memory.


Installing
----------

The release is a GFOpenUp part:

    GFOpenUpTool INSTALL Parts/Einstein

puts the Einstein drawer in SYS:Games. GFOpenUpTool REMOVE Parts/Einstein
takes it out again. Double-click Einstein, or from a Shell:

    cd SYS:Games/Einstein
    Einstein

It opens an 800 x 600 screen of its own; Options switches it to a window.
The settings (einsteinrc), the saved games (save) and the high scores are
kept in the drawer's User drawer, made the first time it runs.


Playing
-------

The left mouse button on a small picture opens that card; the right
button rules it out. The right button on a hint you no longer need puts
it away. Rules in the menu explains the hints with pictures.


Building
--------

    STOVE=/path/to/os32-gcc16-open6 OUT=out ./build-amiga.sh

or, in Cradle, stove_build with the os32-gcc16-open6 stove and target "."
(build-amiga.sh). It fetches Einstein at the commit einstein.lock pins,
puts patches/ on it, builds with the stove's SDL 2, SDL2_ttf and SDL2_mixer
link stubs and the stove's zlib, and writes OUT/Parts/Einstein (the part:
its Part file, Games/Einstein.info and the Games/Einstein drawer) and
OUT/unpacked/Games (the same drawer, for a lab copy). make -C tests test
runs the host tests: the Part file, that every patch applies, that the
paths are the Amiga ones and that every upstream source file is built.


What was changed
----------------

Einstein is written for SDL 1.2, SDL_ttf and SDL_mixer; GFOpenUp has
SDL 2. compat/SDL/ stands in for the SDL 1.2 headers Einstein includes:
one SDL 2 window whose surface is the 1.2 "screen" (SDL_SetVideoMode,
SDL_UpdateRects, SDL_DisplayFormat, SDL_WarpMouse, SDL_WM_SetCaption), and
SDL2_ttf and SDL2_mixer, whose calls Einstein makes are the same.

- patches/0001: the resources come from PROGDIR:res, and the settings, the
  saved games and the high scores go to PROGDIR:User instead of
  $HOME/.einstein.
- patches/0002: SDL 2 keys. The typed letter comes from the key itself
  (SDL 2 has no keysym.unicode), and "press any key" ignores the SDL 2
  modifier keys.
- compat/prelude.h is read before each source file: the C++ streams before
  <unistd.h> (for the stove's pthread.h), and <ctime>.
- amiga/amiga_start.c: a 256 KB stack and the version string.
- The icons (amiga/icons) are GlowIcons made with amicradle's
  scripts/ports/port_icons.py from four of Einstein's own cards.


Licences
--------

Einstein and its pictures and sounds are GPL 2 (LICENSE, COPYING.txt in
the drawer). The three fonts packed in einstein.res come with Einstein
under their own free terms: Luxi Mono Bold (luximb.ttf) under Bigelow &
Holmes' Luxi font licence, which lets it be passed on unchanged, and
nova.ttf and laudcn2.ttf as Einstein's upstream ships them. The Amiga files here are The AmiCradle Team's, under the
GPL 2 or later like the game.

Designed, tested and delivered by The AmiCradle Team.
