#ifndef DBGSTD_H_
#define DBGSTD_H_
/*____________________________________________________________________________*/
/* INCLUDE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#include "debug.h"

/*____________________________________________________________________________*/
/* DBGSTD */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define DBGSTD_TIME_STR_LEN             32
#define DBGSGT_TIME_STR_FMT             "%04d%02d%02d-%02d%02d%02d"
/*························································*/
#define DBGSTD_TEXT_LEN                 1024
/*························································*/
typedef struct DBGSTD_CTL {
    va_list                             vargs;
    time_t                              time_now;
    struct tm *                         time_local;
    char                                time_str[DBGSTD_TIME_STR_LEN];
    char                                text[DBGSTD_TEXT_LEN];
} DBGSTD_CTL;
/*························································*/
void dbgstd_printf (const char * fmt, ...);

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#endif /* DBGSTD_H_ */
