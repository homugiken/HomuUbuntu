#ifndef DEBUGCLIENT_H_
#define DEBUGCLIENT_H_
/*____________________________________________________________________________*/
/* INCLUDE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#include "debug.h"

/*____________________________________________________________________________*/
/* DBG_CLNT */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define DBG_CLNT_OPTL_DBGMSG_CLNT       "dbgmsg"
#define DBG_CLNT_OPTC_DBGMSG_CLNT       'm'
#define DBG_CLNT_OPTS_DBGMSG_CLNT       "dbgmsg client enable"
/*························································*/
#define DBG_CLNT_PRINTF_STR_LEN         2048
/*························································*/
typedef struct DBG_CLNT_CFG {
    bool                                dbgmsg_clnt_enable;
} DBG_CLNT_CFG;
/*························································*/
typedef struct DBG_CLNT_CTL {
    bool                                ready;
    va_list                             vargs;
    DBG_CLNT_CFG                        * cfg;
    DBGMSG_CLNT_CTL                     _dbgmsg_clnt, * dbgmsg_clnt;
} DBG_CLNT_CTL;
/*························································*/
void dbg_clnt_printf (DBG_CLNT_CTL * const ctl, const char * fmt, ...);
int dbg_clnt_set_src_name (DBG_CLNT_CTL * const ctl, char * const name);
static void dbg_clnt_config_show (DBG_CLNT_CFG * const cfg);
static int dbg_clnt_config (DBG_CLNT_CTL * const ctl, const int argc, char * const argv[]);
void dbg_clnt_help (void);
void dbg_clnt_release (DBG_CLNT_CTL * const ctl);
int dbg_clnt_init (DBG_CLNT_CTL * const ctl, const int argc, char * const argv[]);

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#endif /* DEBUGCLIENT_H_ */
