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
    static DBGSTD_CTL                   _ctl, * const ctl = &_ctl;

    MEMZ(ctl, sizeof(DBGSTD_CTL));
    ctl->time_now = time(NULL);
    ctl->time_local = localtime(&(ctl->time_now));
    snprintf(ctl->time_str, DBGSTD_TIME_STR_LEN, DBGSGT_TIME_STR_FMT,
             (ctl->LOCAL_YEAR), (ctl->LOCAL_MON), ctl->LOCAL_DAY,
             ctl->LOCAL_HOUR, ctl->LOCAL_MIN, ctl->LOCAL_SEC);

    va_start(ctl->vargs, fmt);
    vsnprintf(ctl->text, DBGSTD_TEXT_LEN, fmt, ctl->vargs);
    va_end(ctl->vargs);

    fprintf(stdout, "%s|%s\r\n", ctl->time_str, ctl->text);

    return;
}

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
