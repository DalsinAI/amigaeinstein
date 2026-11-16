/* The Amiga port's stack and version string. Einstein makes PROGDIR:User
 * (the settings, the saved games and the high scores) itself.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Einstein. */

/* A Shell gives 4 KB of stack; SDL 2, the fonts and C++ exceptions want
 * more. libnix's startup sets this much up. */
unsigned long __stack = 256 * 1024;
static const char version[] __attribute__((used)) =
    "$VER: Einstein 2.0 (16.11.2026)";
