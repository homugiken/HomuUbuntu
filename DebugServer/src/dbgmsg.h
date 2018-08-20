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
#define DBGMSG_OPTL_SRC_NAME            "name"
#define DBGMSG_OPTS_SRC_NAME            "msg source name"
/*························································*/
#define DBGMSG_TYPE_DEBUG               0x01
/*························································*/
#define DBGMSG_SRC_NAME_LEN             32
#define DBGMSG_TEXT_LEN                 1024
/*························································*/
#define DBGMSG_KEY_PATH_LEN             1024
#define DBGMSG_KEY_PATH_DFT             "."
/*························································*/
#define DBGMSG_KEY_ID_MIN               0x01
#define DBGMSG_KEY_ID_MAX               0xFF
#define DBGMSG_KEY_ID_DFT               DBGMSG_KEY_ID_MIN
/*························································*/
#define DBGMSG_MSG_FMT                  "%04d%02d%02d-%02d%02d%02d|P%04d|%s|%s\r\n"
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
    char                                src_name[DBGMSG_SRC_NAME_LEN];
} DBGMSG_CFG;
/*························································*/
typedef struct DBGMSG_CTL {
    DBGMSG_CFG *                        cfg;
    int                                 qid;
    bool                                ready;
    va_list                             vargs;
    pid_t                               src_pid;
} DBGMSG_CTL;
/*························································*/
void dbgmsg_printf (DBGMSG_CTL * const ctl, const char * fmt, ...);
int dbgmsg_fprintf(FILE * const fp, const DBGMSG_MSG * const msg);
int dbgmsg_msgrcv(const DBGMSG_CTL * const ctl, DBGMSG_MSG * const msg);
static int dbgmsg_msgget(DBGMSG_CTL * const ctl);
static void dbgmsg_config_show (DBGMSG_CFG * const cfg);
static int dbgmsg_config (DBGMSG_CTL * const ctl, const int argc, const char * const argv[]);
void dbgmsg_help (void);
void dbgmsg_release (DBGMSG_CTL * const ctl);
int dbgmsg_init (DBGMSG_CTL * const ctl, const int argc, const char * const argv[]);

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#endif /* DBGMSG_H_ */
