/*******************************************************************
 * File:        t_check
 * Purpose:     A tiny test harness shared by the C test programs.
 *              Counts passes, failures and skips, and reports them.
 * Author:      Gerph
 ******************************************************************/

#ifndef T_CHECK_H
#define T_CHECK_H

/* Called as CHECK(x == y, ("x was %d", x)) or CHECK(x == y, ("")).
 * The message is a parenthesised argument list because the compiler
 * has no variadic macros (C89). */
#define CHECK(cond, msg) \
    do \
    { \
        if (t_check_result((cond) ? 1 : 0, __FILE__, __LINE__)) \
        { \
            t_check_message msg; \
        } \
    } while (0)

/*******************************************************************
 Function:      t_check_result
 Description:   Record the outcome of one check
 Parameters:    ok = non-zero if the check passed
                file = source file of the check
                line = source line of the check
 Returns:       non-zero if the check failed (the caller then prints
                its message)
 ******************************************************************/
int t_check_result(int ok, const char *file, int line);

/*******************************************************************
 Function:      t_check_message
 Description:   Print the message that goes with a failed check
 Parameters:    fmt = printf-style format, then its arguments
 Returns:       none
 ******************************************************************/
void t_check_message(const char *fmt, ...);

/*******************************************************************
 Function:      t_check_skip
 Description:   Record a test that was not run, with the reason, so
                that skipped tests are counted and never silent
 Parameters:    what = name of the test
                why = reason it was skipped
 Returns:       none
 ******************************************************************/
void t_check_skip(const char *what, const char *why);

/*******************************************************************
 Function:      t_check_finish
 Description:   Print the totals for the run
 Parameters:    name = name of the test program
 Returns:       exit status for the program: 0 if all checks passed,
                1 otherwise
 ******************************************************************/
int t_check_finish(const char *name);

#endif
