
#ifndef DEBUGSERVER_H_
#define DEBUGSERVER_H_
/*____________________________________________________________________________*/
/* INCLUDE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>

/*____________________________________________________________________________*/
/* GENERAL */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define DOWHILE(JOB)                    do {JOB} while (0)
#define MEMZ(PTR,SIZE)                  memset(PTR,0,SIZE)
#define MALLOCZ(PTR,TYPE)               DOWHILE(PTR=malloc(sizeof(TYPE));if(PTR!=NULL){MEMZ(PTR,sizeof(TYPE));})
#define STRBOOL(VAL)                    VAL ? "true" : "false"
#define REGULATE(ARG,MIN,MAX)           DOWHILE(if(ARG<MIN){ARG=MIN;}else if(ARG>MAX){ARG=MAX;})
/*························································*/
#define LERROR                          error: FAIL()
#define LEXIT                           exit: EXIT()
#define GOERROR                         goto error
#define GOEXIT                          goto exit
/*························································*/
#define LOCAL_YEAR                      local->tm_year + 1900
#define LOCAL_MON                       local->tm_mon + 1
#define LOCAL_DAY                       local->tm_mday
#define LOCAL_HOUR                      local->tm_hour
#define LOCAL_MIN                       local->tm_min
#define LOCAL_SEC                       local->tm_sec

/*____________________________________________________________________________*/
/* DEBUG */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define DBG_ENABLE_DBGSTD               1   /* DBGSTD */
#define DBG_ENABLE_DBGMSG               0   /* DBGMSG */
#define DBG_ENABLE_ALL                  1   /* ALL */
#define DBG_ENABLE_ERR                  1   /* ERR */
#define DBG_ENABLE_WRN                  1   /* WRN */
#define DBG_ENABLE_LOG                  1   /* LOG */
#define DBG_ENABLE_INF                  1   /* INF */
#define DBG_ENABLE_TAG                  0   /* TAG */
/*························································*/
#define DBG_VERBOSE_MIN                 0
#define DBG_VERBOSE_ERR                 0   /* ERR */
#define DBG_VERBOSE_WRN                 1   /* WRN */
#define DBG_VERBOSE_LOG                 2   /* LOG */
#define DBG_VERBOSE_INF                 3   /* INF */
#define DBG_VERBOSE_TAG                 4   /* TAG */
#define DBG_VERBOSE_MAX                 4
#define DBG_VERBOSE_DFT                 DBG_VERBOSE_MAX
/*························································*/
#if DBG_ENABLE_DBGSTD && DBG_ENABLE_ALL
#define DBGSTD(fmt,...)                 dbgstd_printf("#%04d|%s:"fmt,__LINE__,__FUNCTION__,##__VA_ARGS__)
#else
#define DBGSTD(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_DBGMSG && DBG_ENABLE_ALL
#define DBGMSG(fmt,...)                 dbgmsg_printf("#%04d|%s:"fmt,__LINE__,__FUNCTION__,##__VA_ARGS__)
#else
#define DBGMSG(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_ALL                      /* DBG */
#define DBG(fmt,...)                    DOWHILE(DBGSTD(fmt,##__VA_ARGS__);DBGMSG(fmt,##__VA_ARGS__);)
#else
#define DBG(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_ALL && DBG_ENABLE_ERR    /* ERR */
#define ERR(fmt,...)                    DOWHILE(if(gcfg->verbose>=DBG_VERBOSE_ERR){DBG("ERR!"fmt,##__VA_ARGS__);})
#else
#define ERR(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_ALL && DBG_ENABLE_WRN    /* WRN */
#define WRN(fmt,...)                    DOWHILE(if(gcfg->verbose>=DBG_VERBOSE_WRN){DBG("WRN!"fmt,##__VA_ARGS__);})
#else
#define WRN(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_ALL && DBG_ENABLE_LOG    /* LOG */
#define LOG(fmt,...)                    DOWHILE(if(gcfg->verbose>=DBG_VERBOSE_LOG){DBG(fmt,##__VA_ARGS__);})
#else
#define LOG(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_ALL && DBG_ENABLE_INF    /* INF */
#define INF(fmt,...)                    DOWHILE(if(gcfg->verbose>=DBG_VERBOSE_INF){DBG(fmt,##__VA_ARGS__);})
#else
#define INF(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_ALL && DBG_ENABLE_TAG    /* TAG */
#define TAG(NAME)                       DOWHILE(if(gcfg->verbose>=DBG_VERBOSE_TAG){DBG("<%s>",NAME);})
#else
#define TAG(NAME)
#endif
/*························································*/
#define CHK_NULL(CHK,PRT,ARG)           CHK(ARG==NULL,PRT("%s=NULL",#ARG))
#define CHK_NZERO(CHK,PRT,ARG)          CHK(ARG!=0,PRT("%s=%d!=0",#ARG,ARG))
#define CHK_NEG(CHK,PRT,ARG)            CHK(ARG<0,PRT("%s=%d<0",#ARG,ARG))
#define CHK_NPOS(CHK,PRT,ARG)           CHK(ARG<=0,PRT("%s=%d!>0",#ARG,ARG))
#define CHK_FALSE(CHK,PRT,ARG)          CHK(ARG!=true,PRT("%s=%d!=true",#ARG,ARG))
#define CHK_RANGE(CHK,PRT,ARG,MIN,MAX)  CHK((ARG<MIN)||(ARG>MAX),PRT("%s=%d!<%d,%d>",#ARG,ARG,MIN,MAX))
/*························································*/
#define ERR_CHK(CASE,PRT)               DOWHILE(if(CASE){PRT;GOERROR;})
#define ERR_NULL(ARG)                   CHK_NULL(ERR_CHK,ERR,ARG)
#define ERR_NZERO(ARG)                  CHK_NZERO(ERR_CHK,ERR,ARG)
#define ERR_NEG(ARG)                    CHK_NEG(ERR_CHK,ERR,ARG)
#define ERR_NPOS(ARG)                   CHK_NPOS(ERR_CHK,ERR,ARG)
#define ERR_FALSE(ARG)                  CHK_FALSE(ERR_CHK,ERR,ARG)
#define ERR_RANGE(ARG,MIN,MAX)          CHK_RANGE(ERR_CHK,ERR,ARG,MIN,MAX)
/*························································*/
#define WRN_CHK(CASE,PRT)               DOWHILE(if(CASE){PRT;})
#define WRN_NULL(ARG)                   CHK_NULL(WRN_CHK,WRN,ARG)
#define WRN_NZERO(ARG)                  CHK_NZERO(WRN_CHK,WRN,ARG)
#define WRN_NEG(ARG)                    CHK_NEG(WRN_CHK,WRN,ARG)
#define WRN_NPOS(ARG)                   CHK_NPOS(WRN_CHK,WRN,ARG)
#define WRN_FALSE(ARG)                  CHK_FALSE(WRN_CHK,WRN,ARG)
#define WRN_RANGE(ARG,MIN,MAX)          CHK_RANGE(WRN_CHK,WRN,ARG,MIN,MAX)
/*························································*/
#define FAIL()                          ERR("<FAIL>")
#define ENTR()                          TAG("ENTR")
#define EXIT()                          TAG("EXIT")

/*____________________________________________________________________________*/
/* UTILITY */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define TRY(TRY,FUNC)                   DOWHILE(int iter=0;while(iter++<TRY){if((ret=FUNC)==0){break;}})
#define PAUSE()                         DOWHILE(TAG("PAUSE");while(getchar()!='g'){usleep(100000);})
#define ERR_ERRNO()                     ERR("errno=%d %s", errno, strerror(errno))

/*____________________________________________________________________________*/
/* DBGSTD */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define DBGSTD_TEXT_LEN                 2048
#define DBGSTD_TIME_LEN                 32

typedef struct DBGSTD_CTL {
    va_list                             vargs;
    time_t                              t_now;
    struct tm *                         local;
    char                                time[DBGSTD_TIME_LEN];
    char                                text[DBGSTD_TEXT_LEN];
} DBGSTD_CTL;

/*____________________________________________________________________________*/
/* DBGMSG_SVR */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define DBGMSG_SVR_OPTL_ENABLE          "msg"
#define DBGMSG_SVR_OPTC_ENABLE          'm'
#define DBGMSG_SVR_OPTS_ENABLE          "dbgmsg server enable"
#define DBGMSG_SVR_OPTL_KEY             "key"
#define DBGMSG_SVR_OPTC_KEY             'k'
#define DBGMSG_SVR_OPTS_KEY             "dbgmsg server msgkey"
#define DBGMSG_SVR_OPTL_KEY_PATH        "keypath"
#define DBGMSG_SVR_OPTS_KEY_PATH        "dbgmsg server msgkey path, used when msgkey not assigned"
#define DBGMSG_SVR_OPTL_KEY_ID          "keyid"
#define DBGMSG_SVR_OPTS_KEY_ID          "dbgmsg server msgkey id, used when msgkey not assigned"
/*························································*/
#define DBGMSG_SVR_MSG_NAME_LEN         32
#define DBGMSG_SVR_MSG_TEXT_LEN         2048
/*························································*/
#define DBGMSG_SVR_KEY_PATH_LEN         1024
#define DBGMSG_SVR_KEY_PATH_DFT         "."
#define DBGMSG_SVR_KEY_ID_DFT           0x01
/*························································*/
typedef struct DBGMSG_SVR_MSG {
    long                                src_type;
    time_t                              src_time;
    pid_t                               src_pid;
    char *                              src_name[DBGMSG_SVR_MSG_NAME_LEN];
    char *                              src_text[DBGMSG_SVR_MSG_TEXT_LEN];
} DBGMSG_SVR_MSG;

typedef struct DBGMSG_SVR_CFG {
    key_t                               key;
    char                                key_path[DBGMSG_SVR_KEY_PATH_LEN];
    int                                 key_id;
} DBGMSG_SVR_CFG;

typedef struct DBGMSG_SVR_CTL {
    // DBGMSG_SVR_CFG                      CFG;
    // DBGMSG_SVR_CFG *                    cfg;
    // key_t                               key;
    int                                 qid;
    FILE *                              log_fp;
    bool                                ready;
    // time_t                              tnow;
    // struct tm *                         local;
} DBGMSG_SVR_CTL;

/*____________________________________________________________________________*/
/* DBG_SVR */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define DBG_SVR_OPTL_HELP               "help"
#define DBG_SVR_OPTC_HELP               'h'
#define DBG_SVR_OPTS_HELP               "show help info"
#define DBG_SVR_OPTL_VERBOSE            "verbose"
#define DBG_SVR_OPTC_VERBOSE            'v'
#define DBG_SVR_OPTS_VERBOSE            "set verbosity level"
#define DBG_SVR_OPTL_LOG_PATH           "path"
#define DBG_SVR_OPTC_LOG_PATH           'p'
#define DBG_SVR_OPTS_LOG_PATH           "log file storage path"
#define DBG_SVR_OPTL_LOG_SIZE           "size"
#define DBG_SVR_OPTC_LOG_SIZE           's'
#define DBG_SVR_OPTS_LOG_SIZE           "log file size limit (MB)"
#define DBG_SVR_OPTL_LOG_COUNT          "count"
#define DBG_SVR_OPTC_LOG_COUNT          'c'
#define DBG_SVR_OPTS_LOG_COUNT          "log file quantity limit "
/*························································*/
#define DBG_SVR_LOG_PATH_LEN            1024
#define DBG_SVR_LOG_NAME_LEN            1024
#define DBG_SVR_LOG_PATH_NAME_LEN       (DBG_SVR_LOG_PATH_LEN + DBG_SVR_LOG_NAME_LEN)
#define DBG_SVR_LOG_PATH_DFT            "./dbg"
#define DBG_SVR_LOG_SIZE_MIN            10
#define DBG_SVR_LOG_SIZE_MAX            100
#define DBG_SVR_LOG_SIZE_DFT            DBG_SVR_LOG_SIZE_MIN
#define DBG_SVR_LOG_COUNT_MIN           10
#define DBG_SVR_LOG_COUNT_MAX           1000
#define DBG_SVR_LOG_COUNT_DFT           DBG_SVR_LOG_COUNT_MAX
/*························································*/
#define DBG_SVR_IDX_NAME_LEN            32
#define DBG_SVR_IDX_PATH_NAME_LEN       (DBG_SVR_LOG_PATH_LEN + DBG_SVR_IDX_NAME_LEN)
#define DBG_SVR_IDX_NAME_DFT            "dbg_svr_idx.txt"
#define DBG_SVR_IDX_MIN                 0
#define DBG_SVR_IDX_MAX                 DBG_SVR_LOG_COUNT_MAX
/*························································*/
typedef struct DBG_SVR_CFG {
    char                                log_path[DBG_SVR_LOG_PATH_LEN];
    int                                 log_size;   /* MB */
    int                                 log_count;
    bool                                dbgmsg_enable;
    DBGMSG_SVR_CFG                      _dbgmsg, * dbgmsg;
} DBG_SVR_CFG;

typedef struct DBG_SVR_CTL {
    DBG_SVR_CFG *                       cfg;
    pid_t                               pid;
    pid_t                               ppid;
    time_t                              t_now;
    struct tm *                         local;
    char                                log_name[DBG_SVR_LOG_NAME_LEN];
    char                                log_path_name[DBG_SVR_LOG_PATH_NAME_LEN];
    FILE *                              log_fp;
    char                                idx_path_name[DBG_SVR_IDX_PATH_NAME_LEN];
    FILE *                              idx_fp;
    int                                 idx_create;
    int                                 idx_delete;
    DBGMSG_SVR_CTL                      _dbgmsg, * dbgmsg;
} DBG_SVR_CTL;

// static int dbg_server_loop (void);
// static int dbg_server_idx_update (void);
// static int dbg_server_idx_fetch (void);
// static int dbg_server_mkdir (void);
// static int dbg_server_reset (void);
// static void dbg_server_exit (int ret);
// static void dbg_server_help (char * const name);
// static int dbg_server_init (const int argc, char * const argv[]);
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#endif /* DEBUGSERVER_H_ */
