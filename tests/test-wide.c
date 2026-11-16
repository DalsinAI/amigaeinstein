/* Host test for compat/wide.c (named amiga_* here, so it doesn't meet the
 * host's own): the formats and numbers Einstein uses.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Einstein. */
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#define vswprintf amiga_vswprintf
#define swprintf amiga_swprintf
#define wcstol amiga_wcstol
#include "../compat/wide.c"

static int fails;
#define CHECK(c) do { if (!(c)) { printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); fails++; } } while (0)

int main(void)
{
    wchar_t buf[50], *end;

    CHECK(swprintf(buf, 29, L"%i", 1234) == 4 && wcscmp(buf, L"1234") == 0);
    CHECK(swprintf(buf, 29, L"%i", -7) == 2 && wcscmp(buf, L"-7") == 0);
    CHECK(swprintf(buf, 50, L"%02i:%02i:%02i", 1, 2, 3) == 8 && wcscmp(buf, L"01:02:03") == 0);
    CHECK(swprintf(buf, 3, L"%i", 12345) == -1 && wcscmp(buf, L"12") == 0);
    CHECK(swprintf(buf, 10, L"\x00e9%i", 1) == -1);
    CHECK(wcstol(L"42", &end, 10) == 42 && *end == 0);
    CHECK(wcstol(L"-3x", &end, 10) == -3 && *end == L'x');
    CHECK(wcstol(L"abc", &end, 10) == 0 && *end == L'a');
    CHECK(wcstol(L"17", NULL, 10) == 17);
    if (!fails)
        printf("wide: all passed\n");
    return fails != 0;
}
