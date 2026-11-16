/* mbstowcs(), wcstombs() and wctomb() for Latin-1, the Amiga's own
 * character set. libnix's mbstowcs() and wcstombs() stop early when given
 * a limit of one byte a character, which is what Einstein's fromMbcs() and
 * toMbcs() pass, so resource names, paths and messages came out cut short
 * ("Resou" for "Resource 'cursor.bmp' not found"). These convert every
 * character up to the limit; a wide character past 255 can't be written.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Einstein. */
#include <stdlib.h>
#include <wchar.h>

size_t mbstowcs(wchar_t *dst, const char *src, size_t n)
{
    size_t i = 0;

    if (!dst) {
        while (src[i])
            i++;
        return i;
    }
    for (; i < n; i++) {
        dst[i] = (wchar_t)(unsigned char)src[i];
        if (!src[i])
            return i;
    }
    return i;
}

size_t wcstombs(char *dst, const wchar_t *src, size_t n)
{
    size_t i = 0;

    for (;; i++) {
        if (src[i] < 0 || src[i] > 255)
            return (size_t)-1;
        if (!dst) {
            if (!src[i])
                return i;
            continue;
        }
        if (i >= n)
            return i;
        dst[i] = (char)src[i];
        if (!src[i])
            return i;
    }
}

int wctomb(char *s, wchar_t wc)
{
    if (!s)
        return 0;               /* no shift states */
    if (wc < 0 || wc > 255)
        return -1;
    *s = (char)wc;
    return 1;
}
