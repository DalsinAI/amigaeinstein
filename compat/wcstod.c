/* wcstod(), which libnix hasn't got: Einstein reads numbers from its
 * resource and settings files with it (convert.cpp strToDouble). The
 * number is ASCII, so it is narrowed and given to strtod().
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Einstein. */
#include <stdlib.h>
#include <wchar.h>

double wcstod(const wchar_t *s, wchar_t **end)
{
    char buf[64];
    char *e;
    size_t i = 0, skip = 0;
    double v;

    while (s[skip] == L' ' || s[skip] == L'\t' || s[skip] == L'\n' || s[skip] == L'\r')
        skip++;
    while (i < sizeof buf - 1 && s[skip + i] > 0 && s[skip + i] < 128) {
        buf[i] = (char)s[skip + i];
        i++;
    }
    buf[i] = 0;
    v = strtod(buf, &e);
    if (end)
        *end = (wchar_t *)(e == buf ? s : s + skip + (e - buf));
    return v;
}
