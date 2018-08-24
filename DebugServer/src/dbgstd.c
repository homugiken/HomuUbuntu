/*____________________________________________________________________________*/
/* INCLUDE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#include "debug.h"

/*____________________________________________________________________________*/
/* DBGSTD */
/*------------------------------------*/
void
dbgstd_printf (
    const char *                        fmt,
    ...)
{
    static time_t                       _time_now, * const time_now = &_time_now;
    static struct tm *                  time_local;
    static va_list                      vargs;

    time(time_now);
    time_local = localtime(time_now);
    fprintf(stdout, GENERAL_STR_FMT_DATE_TIME"|",
            LOCAL_YEAR, LOCAL_MON, LOCAL_DAY,
            LOCAL_HOUR, LOCAL_MIN, LOCAL_SEC);

    va_start(vargs, fmt);
    vfprintf(stdout, fmt, vargs);
    va_end(vargs);

    return;
}

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
