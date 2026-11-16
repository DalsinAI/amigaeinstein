/* Host test for compat/wmem.c (named amiga_* here, so it doesn't meet the
 * host's own): whole 4-byte characters copied, moved, set and compared.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Einstein. */
#include <stdio.h>
#include <wchar.h>
#define wcslen amiga_wcslen
#define wmemcpy amiga_wmemcpy
#define wmemmove amiga_wmemmove
#define wmemset amiga_wmemset
#define wmemcmp amiga_wmemcmp
#define wmemchr amiga_wmemchr
#include "../compat/wmem.c"

static int fails;
#define CHECK(c) do { if (!(c)) { printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); fails++; } } while (0)

int main(void)
{
    const wchar_t *msg = L"Resource 'cursor.bmp' not found";
    wchar_t buf[64];

    CHECK(wcslen(msg) == 31);
    CHECK(wcslen(L"") == 0);
    wmemset(buf, L'z', 64);
    CHECK(wmemcpy(buf, msg, 32) == buf && wmemcmp(buf, msg, 32) == 0 && buf[30] == L'd');
    CHECK(buf[32] == L'z');
    wmemmove(buf + 2, buf, 10);               /* overlapping, forwards */
    CHECK(wmemcmp(buf + 2, L"Resource '", 10) == 0);
    wmemmove(buf, buf + 2, 10);               /* overlapping, backwards */
    CHECK(wmemcmp(buf, L"Resource '", 10) == 0);
    CHECK(wmemcmp(L"ab", L"ac", 2) < 0 && wmemcmp(L"ac", L"ab", 2) > 0 && wmemcmp(L"ab", L"ab", 2) == 0);
    CHECK(wmemcmp(L"\x10000", L"\x00ff", 1) > 0);
    CHECK(wmemchr(msg, L'\'', 31) == msg + 9 && wmemchr(msg, L'#', 31) == NULL);
    if (!fails)
        printf("wmem: all passed\n");
    return fails != 0;
}
