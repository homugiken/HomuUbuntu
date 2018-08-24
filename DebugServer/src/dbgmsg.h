#ifndef DBGMSG_H_
#define DBGMSG_H_
/*____________________________________________________________________________*/
/* INCLUDE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#include "debug.h"

/*____________________________________________________________________________*/
/* DBGMSG */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define DBGMSG_OPTL_KEY                 "key"
#define DBGMSG_OPTC_KEY                 'k'
#define DBGMSG_OPTS_KEY                 "msg key"
#define DBGMSG_OPTL_KEY_PATH            "keypath"
#define DBGMSG_OPTS_KEY_PATH            "msg key path"
#define DBGMSG_OPTL_KEY_ID              "keyid"
#define DBGMSG_OPTS_KEY_ID              "msg key id"
/*························································*/
#define DBGMSG_TYPE_DEBUG               0x01
/*························································*/
#define DBGMSG_SRC_NAME_LEN             32
/*························································*/
#define DBGMSG_TEXT_LEN                 1024
/*························································*/
#define DBGMSG_KEY_PATH_LEN             1024
#define DBGMSG_KEY_PATH_DFT             "."
/*························································*/
#define DBGMSG_KEY_ID_MIN               0x01
#define DBGMSG_KEY_ID_MAX               0xFF
#define DBGMSG_KEY_ID_DFT               DBGMSG_KEY_ID_MIN
/*························································*/
#define DBGMSG_MSG_FMT                  GENERAL_STR_FMT_DATE_TIME"|P%04d|%s|%s\r\n"
/*························································*/
typedef struct DBGMSG_MSG {
    long                                type;
    time_t                              src_time;
    pid_t                               src_pid;
    char                                src_name[DBGMSG_SRC_NAME_LEN];
    char                                text[DBGMSG_TEXT_LEN];
} DBGMSG_MSG;
/*························································*/
typedef struct DBGMSG_CFG {
    key_t                               key;
    char                                key_path[DBGMSG_KEY_PATH_LEN];
    uint32_t                            key_id;
} DBGMSG_CFG;
/*························································*/
typedef struct DBGMSG_CTL {
    bool                                ready;
    DBGMSG_CFG *                        cfg;
    int                                 qid;
} DBGMSG_CTL;
/*························································*/
static int dbgmsg_fprintf (FILE * const fp, DBGMSG_MSG * const msg);
static int dbgmsg_recv (DBGMSG_CTL * const ctl, DBGMSG_MSG * const msg);
static void dbgmsg_config_show (DBGMSG_CFG * const cfg);
static int dbgmsg_config (DBGMSG_CTL * const ctl, const int argc, char * const argv[]);
static void dbgmsg_help (void);
static void dbgmsg_release (DBGMSG_CTL * const ctl);
static int dbgmsg_init (DBGMSG_CTL * const ctl, const int argc, char * const argv[]);

/*____________________________________________________________________________*/
/* DBGMSG_SVR */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define DBGMSG_SVR_MSG_BUF_SIZE         10
/*························································*/
typedef struct DBGMSG_SVR_CTL {
    bool                                ready;
    DBGMSG_CTL                          _dbgmsg, * dbgmsg;
    DBGMSG_MSG                          msg_buf[DBGMSG_SVR_MSG_BUF_SIZE];
    uint32_t                            msg_count;
} DBGMSG_SVR_CTL;
/*························································*/
int dbgmsg_svr_fprintf (DBGMSG_SVR_CTL * const ctl, FILE * const fp);
int dbgmsg_svr_recv (DBGMSG_SVR_CTL * const ctl);
void dbgmsg_svr_help (void);
void dbgmsg_svr_release (DBGMSG_SVR_CTL * const ctl);
int dbgmsg_svr_init (DBGMSG_SVR_CTL * const ctl, const int argc, char * const argv[]);

/*____________________________________________________________________________*/
/* DBGMSG_SVR */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define DBGMSG_CLNT_SRC_NAME_DFT        "DBG_CLNT"
/*························································*/
typedef struct DBGMSG_CLNT_CTL {
    bool                                ready;
    va_list                             vargs;
    DBGMSG_CTL                          _dbgmsg, * dbgmsg;
    pid_t                               src_pid;
    char                                src_name[DBGMSG_SRC_NAME_LEN];
} DBGMSG_CLNT_CTL;
/*························································*/
void dbgmsg_clnt_printf (DBGMSG_CLNT_CTL * const ctl, const char * fmt, ...);
int dbgmsg_clnt_set_src_name (DBGMSG_CLNT_CTL * const ctl, char * const name);
void dbgmsg_clnt_help (void);
void dbgmsg_clnt_release (DBGMSG_CLNT_CTL * const ctl);
int dbgmsg_clnt_init (DBGMSG_CLNT_CTL * const ctl, const int argc, char * const argv[]);

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#endif /* DBGMSG_H_ */
