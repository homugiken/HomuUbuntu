
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
#define LOCAL_YEAR                      time_local->tm_year + 1900
#define LOCAL_MON                       time_local->tm_mon + 1
#define LOCAL_DAY                       time_local->tm_mday
#define LOCAL_HOUR                      time_local->tm_hour
#define LOCAL_MIN                       time_local->tm_min
#define LOCAL_SEC                       time_local->tm_sec

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
#define DBG_ENABLE_TAG                  1   /* TAG */
/*························································*/
#define DBG_VERBOSE_ERR                 1   /* ERR */
#define DBG_VERBOSE_WRN                 2   /* WRN */
#define DBG_VERBOSE_LOG                 3   /* LOG */
#define DBG_VERBOSE_INF                 4   /* INF */
#define DBG_VERBOSE_TAG                 5   /* TAG */
#define DBG_VERBOSE_MIN                 DBG_VERBOSE_ERR
#define DBG_VERBOSE_MAX                 DBG_VERBOSE_TAG
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
#define ERR(fmt,...)                    DOWHILE(if(gverbose>=DBG_VERBOSE_ERR){DBG("ERR!"fmt,##__VA_ARGS__);})
#else
#define ERR(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_ALL && DBG_ENABLE_WRN    /* WRN */
#define WRN(fmt,...)                    DOWHILE(if(gverbose>=DBG_VERBOSE_WRN){DBG("WRN!"fmt,##__VA_ARGS__);})
#else
#define WRN(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_ALL && DBG_ENABLE_LOG    /* LOG */
#define LOG(fmt,...)                    DOWHILE(if(gverbose>=DBG_VERBOSE_LOG){DBG(fmt,##__VA_ARGS__);})
#else
#define LOG(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_ALL && DBG_ENABLE_INF    /* INF */
#define INF(fmt,...)                    DOWHILE(if(gverbose>=DBG_VERBOSE_INF){DBG(fmt,##__VA_ARGS__);})
#else
#define INF(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_ALL && DBG_ENABLE_TAG    /* TAG */
#define TAG(NAME)                       DOWHILE(if(gverbose>=DBG_VERBOSE_TAG){DBG("<%s>",NAME);})
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
#define DBGSTD_TIME_STR_LEN             32
#define DBGSTD_TEXT_LEN                 2048
/*························································*/
typedef struct DBGSTD_CTL {
    va_list                             vargs;
    time_t                              time_now;
    struct tm *                         time_local;
    char                                time_str[DBGSTD_TIME_STR_LEN];
    char                                text[DBGSTD_TEXT_LEN];
} DBGSTD_CTL;
/*························································*/
static void dbgstd_printf (const char * fmt, ...);

/*____________________________________________________________________________*/
/* DBGMSG */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define DBGMSG_OPTL_KEY                 "key"
#define DBGMSG_OPTC_KEY                 'k'
#define DBGMSG_OPTS_KEY                 "msgkey"
#define DBGMSG_OPTL_KEY_PATH            "keypath"
#define DBGMSG_OPTS_KEY_PATH            "msgkey path"
#define DBGMSG_OPTL_KEY_ID              "keyid"
#define DBGMSG_OPTS_KEY_ID              "msgkey id"
/*························································*/
#define DBGMSG_TYPE_DEBUG               0x01
/*························································*/
#define DBGMSG_SRC_NAME_LEN             32
#define DBGMSG_TEXT_LEN                 2048
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
    pid_t                               pid;
    int                                 qid;
    bool                                ready;
    va_list                             vargs;
} DBGMSG_CTL;
/*························································*/
static void dbgmsg_printf (const char * fmt, ...);
static void dbgmsg_cfg_show (DBGMSG_CFG * const cfg);
static int dbgmsg_cfg (DBGMSG_CFG * const cfg, const int argc, char * const argv[]);
static void dbgmsg_help (void);
static void dbgmsg_release (DBGMSG_CTL * const ctl);
static int dbgmsg_init (DBGMSG_CTL * const ctl, DBGMSG_CFG * const cfg);

/*____________________________________________________________________________*/
/* DBGMSG_SVR */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define DBGMSG_SVR_MSG_BUF_SIZE         10
#define DBGMSG_SVR_MSG_COUNT_MAX        (DBGMSG_SVR_MSG_BUF_SIZE - 1)
/*························································*/
typedef struct DBGMSG_SVR_CFG {
    DBGMSG_CFG *                        dbgmsg;
} DBGMSG_SVR_CFG;
/*························································*/
typedef struct DBGMSG_SVR_CTL {
    DBGMSG_SVR_CFG *                    cfg;
    DBGMSG_CTL *                        dbgmsg;
    bool                                ready;
    DBGMSG_MSG                          msg_buf[DBGMSG_SVR_MSG_BUF_SIZE];
    uint32_t                            msg_count;
    time_t                              time_msg;
    struct tm *                         time_local;
} DBGMSG_SVR_CTL;
/*························································*/
static void dbgmsg_svr_cfg_show (DBGMSG_SVR_CFG * const cfg);
static int dbgmsg_svr_cfg (DBGMSG_SVR_CFG * const cfg, const int argc, char * const argv[]);
static void dbgmsg_svr_help (void);
static void dbgmsg_svr_release (DBGMSG_SVR_CTL * const ctl);
static int dbgmsg_svr_init (DBGMSG_SVR_CTL * const ctl, DBGMSG_SVR_CFG * const cfg);

/*____________________________________________________________________________*/
/* DBG_SVR */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define DBG_SVR_VERBOSE_MIN             DBG_VERBOSE_ERR
#define DBG_SVR_VERBOSE_MAX             DBG_VERBOSE_TAG
#define DBG_SVR_VERBOSE_DFT             DBG_VERBOSE_MAX
/*························································*/
#define DBG_SVR_OPTL_LOG_PATH           "path"
#define DBG_SVR_OPTC_LOG_PATH           'p'
#define DBG_SVR_OPTS_LOG_PATH           "log file storage path"
#define DBG_SVR_OPTL_LOG_SIZE           "size"
#define DBG_SVR_OPTC_LOG_SIZE           's'
#define DBG_SVR_OPTS_LOG_SIZE           "log file size limit (MB)"
#define DBG_SVR_OPTL_LOG_COUNT          "count"
#define DBG_SVR_OPTC_LOG_COUNT          'c'
#define DBG_SVR_OPTS_LOG_COUNT          "log file quantity limit "
#define DBG_SVR_OPTL_DBGMSG_SVR         "msg"
#define DBG_SVR_OPTC_DBGMSG_SVR         'm'
#define DBG_SVR_OPTS_DBGMSG_SVR         "dbgmsg server enable"
/*························································*/
#define DBG_SVR_DIR_PATH_LEN            1024
#define DBG_SVR_DIR_PATH_DFT            "./dbg"
/*························································*/
#define DBG_SVR_LOG_NAME_LEN            1024
#define DBG_SVR_LOG_NAME_FMT            "dbg-%04d-%04d%02d%02d-%02d%02d%02d.txt"
#define DBG_SVR_LOG_PATH_NAME_LEN       (DBG_SVR_DIR_PATH_LEN + DBG_SVR_LOG_NAME_LEN)
/*························································*/
#define DBG_SVR_LOG_SIZE_MIN            1
#define DBG_SVR_LOG_SIZE_MAX            10
#define DBG_SVR_LOG_SIZE_DFT            DBG_SVR_LOG_SIZE_MIN
/*························································*/
#define DBG_SVR_LOG_COUNT_MIN           10
#define DBG_SVR_LOG_COUNT_MAX           100
#define DBG_SVR_LOG_COUNT_DFT           DBG_SVR_LOG_COUNT_MIN
/*························································*/
#define DBG_SVR_IDX_NAME_LEN            32
#define DBG_SVR_IDX_NAME_FMT            "dbg-idx.txt"
#define DBG_SVR_IDX_PATH_NAME_LEN       (DBG_SVR_DIR_PATH_LEN + DBG_SVR_IDX_NAME_LEN)
/*························································*/
#define DBG_SVR_IDX_FMT                 "[IDX|CREATE=%04u|DELETE=%04u]"
#define DBG_SVR_IDX_MIN                 0
#define DBG_SVR_IDX_MAX                 9999
/*························································*/
typedef struct DBG_SVR_CFG {
    char                                dir_path[DBG_SVR_DIR_PATH_LEN];
    uint32_t                            log_size;           /* MB */
    uint32_t                            log_count;
    bool                                dbgmsg_svr_enable;
    DBGMSG_SVR_CFG                      _dbgmsg_svr, * dbgmsg_svr;
} DBG_SVR_CFG;
/*························································*/
typedef struct DBG_SVR_CTL {
    DBG_SVR_CFG *                       cfg;
    pid_t                               pid;
    pid_t                               ppid;
    bool                                ready;
    time_t                              time_now;
    struct tm *                         time_local;
    char                                log_name[DBG_SVR_LOG_NAME_LEN];
    char                                log_path_name[DBG_SVR_LOG_PATH_NAME_LEN];
    FILE *                              log_fp;
    uint32_t                            log_size_current;   /* MB */
    char                                idx_path_name[DBG_SVR_IDX_PATH_NAME_LEN];
    FILE *                              idx_fp;
    uint32_t                            idx_create;
    uint32_t                            idx_delete;
    DIR *                               dir;
    DBGMSG_SVR_CTL                      _dbgmsg_svr, * dbgmsg_svr;
} DBG_SVR_CTL;
/*························································*/
static int dbg_svr_loop_job (DBG_SVR_CTL * const ctl);
static int dbg_svr_rcv (DBG_SVR_CTL * const ctl);
/*························································*/
static int dbg_svr_log_shift (DBG_SVR_CTL * const ctl);
static void dbg_svr_log_flush (DBG_SVR_CTL * const ctl);
static int dbg_svr_log_fprintf (DBG_SVR_CTL * const ctl);
static int dbg_svr_log_write_trailer (DBG_SVR_CTL * const ctl);
static int dbg_svr_log_write_header (DBG_SVR_CTL * const ctl);
static int dbg_svr_log_name_new (DBG_SVR_CTL * const ctl);
static void dbg_svr_log_close (DBG_SVR_CTL * const ctl);
static int dbg_svr_log_open (DBG_SVR_CTL * const ctl);
/*························································*/
static int dbg_svr_idx_delete_increase (DBG_SVR_CTL * const ctl);
static int dbg_svr_idx_create_increase (DBG_SVR_CTL * const ctl);
static void dbg_svr_idx_flush (DBG_SVR_CTL * const ctl);
static int dbg_svr_idx_write (DBG_SVR_CTL * const ctl);
static int dbg_svr_idx_read (DBG_SVR_CTL * const ctl);
static void dbg_svr_idx_close (DBG_SVR_CTL * const ctl);
static int dbg_svr_idx_open_write (DBG_SVR_CTL * const ctl);
static int dbg_svr_idx_open_read (DBG_SVR_CTL * const ctl);
/*························································*/
static int dbg_svr_mkdir (DBG_SVR_CTL * const ctl);
static void dbg_svr_closedir (DBG_SVR_CTL * const ctl);
static int dbg_svr_opendir (DBG_SVR_CTL * const ctl);
/*························································*/
static void dbg_svr_cfg_show (DBG_SVR_CFG * const cfg);
static int dbg_svr_cfg (DBG_SVR_CFG * const cfg, const int argc, char * const argv[]);
static void dbg_svr_help (void);
static void dbg_svr_release (DBG_SVR_CTL * const ctl);
static int dbg_svr_init (DBG_SVR_CTL * const ctl, DBG_SVR_CFG * const cfg);

/*____________________________________________________________________________*/
/* MAIN */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define GOPTL_HELP                      "help"
#define GOPTC_HELP                      'h'
#define GOPTS_HELP                      "show help info"
#define GOPTL_VERBOSE                   "verbose"
#define GOPTC_VERBOSE                   'v'
#define GOPTS_VERBOSE                   "set verbosity level"

static int main_loop (void);
static void main_cfg_show (void);
static int main_cfg (const int argc, char * const argv[]);
static void main_help (char * const name);
static void main_exit (int ret);
static int main_init (void);

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#endif /* DEBUGSERVER_H_ */
