/*******************************************************************
 * File:        t_check
 * Purpose:     Test harness shared by the C test programs
 * Author:      Gerph
 ******************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include "t_check.h"

static int checks = 0;
static int failures = 0;
static int skips = 0;

int t_check_result(int ok, const char *file, int line)
{
    checks++;
    if (ok)
    {
        return 0;
    }
    failures++;
    printf("FAIL %s:%d: ", file, line);
    return 1;
}

void t_check_message(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

void t_check_skip(const char *what, const char *why)
{
    skips++;
    printf("SKIP %s: %s\n", what, why);
}

int t_check_finish(const char *name)
{
    printf("%s: %d checks, %d failed, %d skipped\n",
           name, checks, failures, skips);
    return failures ? 1 : 0;
}
