/* Host test for compat/latin1.c (named amiga_* here, so it doesn't meet the
 * host's own): whole strings, the limit, Latin-1 and the character that
 * can't be written.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Einstein. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#define mbstowcs amiga_mbstowcs
#define wcstombs amiga_wcstombs
#define wctomb amiga_wctomb
#include "../compat/latin1.c"

static int fails;
#define CHECK(c) do { if (!(c)) { printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); fails++; } } while (0)

int main(void)
{
    const char *msg = "Resource 'cursor.bmp' not found";
    wchar_t w[64];
    char c[64];
    size_t len = strlen(msg);

    /* Einstein's fromMbcs(): limit = length, then terminates itself */
    CHECK(mbstowcs(w, msg, len) == len);
    w[len] = 0;
    CHECK(w[0] == L'R' && w[len - 1] == L'd');
    CHECK(mbstowcs(NULL, msg, 0) == len);
    /* Einstein's toMbcs(): limit = MB_CUR_MAX * length */
    CHECK(wcstombs(c, w, len) == len);
    c[len] = 0;
    CHECK(strcmp(c, msg) == 0);
    CHECK(wcstombs(NULL, w, 0) == len);
    /* room for the terminator: it is written */
    memset(c, 'x', sizeof c);
    CHECK(wcstombs(c, L"PROGDIR:res", sizeof c) == 11 && c[11] == 0);
    CHECK(mbstowcs(w, "PROGDIR:res", 64) == 11 && w[11] == 0);
    /* Latin-1 both ways */
    CHECK(mbstowcs(w, "\xe9t\xe9", 3) == 3 && w[0] == 0xe9);
    CHECK(wcstombs(c, L"\x00e9t\x00e9", 3) == 3 && (unsigned char)c[0] == 0xe9);
    /* past Latin-1 */
    CHECK(wcstombs(c, L"a\x20ac", 8) == (size_t)-1);
    CHECK(wctomb(c, 0x20ac) == -1);
    CHECK(wctomb(c, L'z') == 1 && c[0] == 'z');
    CHECK(wctomb(NULL, L'z') == 0);
    if (!fails)
        printf("latin1: all passed\n");
    return fails != 0;
}
