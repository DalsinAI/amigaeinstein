/* swprintf() and wcstol() for a 4-byte wchar_t. libnix's wide functions
 * step through the text 2 bytes at a time, as for a 2-byte wchar_t, and
 * Einstein's numbers (convert.cpp numToStr and strToInt, utils.cpp's
 * game time) came out wrong. Einstein only formats ASCII ("%i",
 * "%02i:%02i:%02i") and reads ASCII numbers, so both narrow the text,
 * use the C library's narrow call and widen the result.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Einstein. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

int vswprintf(wchar_t *dst, size_t n, const wchar_t *fmt, va_list ap)
{
    char f[128], out[256];
    size_t i;
    int len;

    for (i = 0; fmt[i] && i < sizeof f - 1; i++) {
        if (fmt[i] < 0 || fmt[i] > 127)
            return -1;
        f[i] = (char)fmt[i];
    }
    if (fmt[i])
        return -1;
    f[i] = 0;
    len = vsnprintf(out, sizeof out, f, ap);
    if (len < 0 || (size_t)len >= sizeof out || n == 0)
        return -1;
    for (i = 0; i < (size_t)len && i < n - 1; i++)
        dst[i] = (wchar_t)(unsigned char)out[i];
    dst[i] = 0;
    return (size_t)len < n ? len : -1;
}

int swprintf(wchar_t *dst, size_t n, const wchar_t *fmt, ...)
{
    va_list ap;
    int r;

    va_start(ap, fmt);
    r = vswprintf(dst, n, fmt, ap);
    va_end(ap);
    return r;
}

long wcstol(const wchar_t *s, wchar_t **end, int base)
{
    char buf[64];
    char *e;
    size_t i = 0;
    long v;

    while (i < sizeof buf - 1 && s[i] > 0 && s[i] < 128) {
        buf[i] = (char)s[i];
        i++;
    }
    buf[i] = 0;
    v = strtol(buf, &e, base);
    if (end)
        *end = (wchar_t *)s + (e - buf);
    return v;
}
