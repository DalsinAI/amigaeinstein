/* Host test for compat/wcstod.c (named amiga_wcstod here, so it doesn't
 * meet the host's own): the numbers Einstein's files hold.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Einstein. */
#include <stdio.h>
#include <math.h>
#include <wchar.h>
#define wcstod amiga_wcstod
#include "../compat/wcstod.c"
#undef wcstod

static int fail;
static void check(const wchar_t *s, double want, size_t used)
{
    wchar_t *end;
    double v = amiga_wcstod(s, &end);
    if (fabs(v - want) > 1e-9 || (size_t)(end - s) != used) {
        printf("FAIL: %ls -> %g, %zu used (want %g, %zu)\n", s, v, (size_t)(end - s), want, used);
        fail = 1;
    }
}

int main(void)
{
    check(L"0.2", 0.2, 3);
    check(L"  -12.5x", -12.5, 7);
    check(L"100", 100, 3);
    check(L"abc", 0, 0);
    check(L"1e3 ", 1000, 3);
    if (!fail) puts("wcstod: all checks passed");
    return fail;
}
