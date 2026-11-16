/* wcslen() and the wmem*() functions for a 4-byte wchar_t. libnix's copy
 * moves 2 bytes a character, so libstdc++'s std::wstring kept only half of
 * every string it copied ("Resou" for "Resource 'cursor.bmp' not found"),
 * and resource names, paths and messages broke. These work on whole
 * wchar_t, as the compiler's wchar_t is.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Einstein. */
#include <stddef.h>
#include <wchar.h>

size_t wcslen(const wchar_t *s)
{
    const wchar_t *p = s;

    while (*p)
        p++;
    return (size_t)(p - s);
}

wchar_t *wmemcpy(wchar_t *d, const wchar_t *s, size_t n)
{
    size_t i;

    for (i = 0; i < n; i++)
        d[i] = s[i];
    return d;
}

wchar_t *wmemmove(wchar_t *d, const wchar_t *s, size_t n)
{
    size_t i;

    if (d < s) {
        for (i = 0; i < n; i++)
            d[i] = s[i];
    } else if (d > s) {
        for (i = n; i > 0; i--)
            d[i - 1] = s[i - 1];
    }
    return d;
}

wchar_t *wmemset(wchar_t *d, wchar_t c, size_t n)
{
    size_t i;

    for (i = 0; i < n; i++)
        d[i] = c;
    return d;
}

int wmemcmp(const wchar_t *a, const wchar_t *b, size_t n)
{
    size_t i;

    for (i = 0; i < n; i++)
        if (a[i] != b[i])
            return a[i] < b[i] ? -1 : 1;
    return 0;
}

wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t n)
{
    size_t i;

    for (i = 0; i < n; i++)
        if (s[i] == c)
            return (wchar_t *)&s[i];
    return NULL;
}
