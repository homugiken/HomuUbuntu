
#ifndef DEBUGSERVER_H_
#define DEBUGSERVER_H_
/*____________________________________________________________________________*/
/* INCLUDE */
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
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

/*____________________________________________________________________________*/
/* GENERAL */
#define DOWHILE(JOB)                    do {JOB} while (0)
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

/*____________________________________________________________________________*/
/* DEBUG MACROS */
/*------------------------------------*/
/* ENABLE */
#define DEBUGSERVER_DBG_ENABLE          1
#define DEBUGSERVER_ERR_ENABLE          1
#define DEBUGSERVER_WRN_ENABLE          1
#define DEBUGSERVER_LOG_ENABLE          1
#define DEBUGSERVER_INF_ENABLE          0
#define DEBUGSERVER_TAG_ENABLE          0

#if DEBUGSERVER_DBG_ENABLE
#define DEBUGSERVER_DBGSTD_ENABLE       1
#define DEBUGSERVER_DBGMSG_ENABLE       1
#else
#define DEBUGSERVER_DBGSTD_ENABLE       0
#define DEBUGSERVER_DBGMSG_ENABLE       0
#endif
/*------------------------------------*/
/* DBGSTD */
#if DEBUGSERVER_DBGSTD_ENABLE
#define DBGSTD(fmt,...)                 dbgstd_printf("#%04d|%s:"fmt, __LINE__,__FUNCTION__,##__VA_ARGS__)
#else
#define DBGSTD(fmt,...)
#endif
/*------------------------------------*/
/* DBGMSG */
#if DEBUGSERVER_DBGMSG_ENABLE
#define DBGMSG(fmt,...)
#else
#define DBGMSG(fmt,...)
#endif
/*------------------------------------*/
/* DBG */
#if DEBUGSERVER_DBG_ENABLE
#define DBG(fmt,...)                    DOWHILE(DBGSTD(fmt,##__VA_ARGS__); DBGMSG(fmt,##__VA_ARGS__);)
#else
#define DBG(fmt,...)
#endif
/*------------------------------------*/
/* ERR */
#if DEBUGSERVER_ERR_ENABLE
#define ERR(fmt,...)                    DBG("ERR!"fmt,##__VA_ARGS__)
#else
#define ERR(fmt,...)
#endif
/*------------------------------------*/
/* WRN */
#if DEBUGSERVER_WRN_ENABLE
#define WRN(fmt,...)                    DBG("WRN!"fmt,##__VA_ARGS__)
#else
#define WRN(fmt,...)
#endif
/*------------------------------------*/
/* LOG */
#if DEBUGSERVER_LOG_ENABLE
#define LOG(fmt,...)                    DBG(fmt,##__VA_ARGS__)
#else
#define LOG(fmt,...)
#endif
/*------------------------------------*/
/* INF */
#if DEBUGSERVER_INF_ENABLE
#define INF(fmt,...)                    DBG("INF:"fmt,##__VA_ARGS__)
#else
#define INF(fmt,...)
#endif
/*------------------------------------*/
/* TAG */
#if DEBUGSERVER_TAG_ENABLE
#define TAG(NAME)                       DBG("<%s>",NAME)
#else
#define TAG(NAME)
#endif
/*------------------------------------*/
/* CHK */
#define CHK_NULL(CHK,PRT,ARG)           CHK(ARG==NULL, PRT("%s NULL", #ARG))
#define CHK_NZERO(CHK,PRT,ARG)          CHK(ARG!=0, PRT("%s=%d !=0", #ARG,ARG))
#define CHK_NEG(CHK,PRT,ARG)            CHK(ARG<0, PRT("%s=%d <0", #ARG,ARG))
#define CHK_NPOS(CHK,PRT,ARG)           CHK(ARG<=0, PRT("%s=%d !>0", #ARG,ARG))
#define CHK_FALSE(CHK,PRT,ARG)          CHK(ARG!=true, PRT("%s=%d FALSE", #ARG,ARG))
#define CHK_RANGE(CHK,PRT,ARG,MIN,MAX)  CHK((ARG<MIN)||(ARG>MAX), PRT("%s=%d !<%d,%d>", #ARG,ARG,MIN,MAX))
/*------------------------------------*/
/* ERR_CHK */
#define ERR_CHK(CASE,PRT)               DOWHILE(if(CASE){PRT; goto error;})
#define ERR_NULL(ARG)                   CHK_NULL(ERR_CHK,ERR,ARG)
#define ERR_NZERO(ARG)                  CHK_NZERO(ERR_CHK,ERR,ARG)
#define ERR_NEG(ARG)                    CHK_NEG(ERR_CHK,ERR,ARG)
#define ERR_NPOS(ARG)                   CHK_NPOS(ERR_CHK,ERR,ARG)
#define ERR_FALSE(ARG)                  CHK_FALSE(ERR_CHK,ERR,ARG)
#define ERR_RANGE(ARG,MIN,MAX)          CHK_RANGE(ERR_CHK,ERR,ARG,MIN,MAX)
/*------------------------------------*/
/* WRN_CHK */
#define WRN_CHK(CASE,PRT)               DOWHILE(if(CASE){PRT;})
#define WRN_NULL(ARG)                   CHK_NULL(ERR_CHK,ERR,ARG)
#define WRN_NZERO(ARG)                  CHK_NZERO(ERR_CHK,ERR,ARG)
#define WRN_NEG(ARG)                    CHK_NEG(ERR_CHK,ERR,ARG)
#define WRN_NPOS(ARG)                   CHK_NPOS(ERR_CHK,ERR,ARG)
#define WRN_FALSE(ARG)                  CHK_FALSE(ERR_CHK,ERR,ARG)
#define WRN_RANGE(ARG,MIN,MAX)          CHK_RANGE(ERR_CHK,ERR,ARG,MIN,MAX)
/*------------------------------------*/
/* TAG */
#define ENTR()                          TAG("ENTR")
#define EXIT()                          TAG("EXIT")
#define FAIL()                          ERR("<FAIL>")
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

/*____________________________________________________________________________*/
/* UTILITY MACROS */
#define TRYWHILE(TRY,FUNC) \
DOWHILE(int iter=0; while(iter++<TRY){ret=FUNC; if(ret==0){break;} WRN_NZERO(iter);})
#define PAUSE(fmt,...) \
DOWHILE(TAG("PAUSE",fmt,##__VA_ARGS__); while(getchar()!='g'){usleep(100000);} )
#define MEMZERO(PTR,SIZE) \
memset(PTR,0,SIZE)
#define MALLOCZERO(PTR,TYPE) \
DOWHILE(PTR=malloc(sizeof(TYPE)); if(PTR!=NULL){MEMZERO(PTR,sizeof(TYPE));})
#define STRBOOL(VAL)                    VAL ? "true" : "false"
#define ERR_ERRNO()                     ERR("errno=%d %s", errno, strerror(errno))
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

/*____________________________________________________________________________*/
/* DBGSTD */
/*------------------------------------*/
#define DBGSTD_TEXT_LEN                 4096
#define DBGSTD_TIME_STAMP_LEN           32

typedef struct DBGSTD_CTL {
        time_t                          tnow;
        struct tm *                     local;
        char                            time_stamp[DBGSTD_TIME_STAMP_LEN];
        va_list                         vargs;
        char                            text[DBGSTD_TEXT_LEN];
} DBGSTD_CTL;
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

/*____________________________________________________________________________*/
/* DBGMSG_SERVER */
/*------------------------------------*/
#define DBGMSG_SERVER_MSG_NAME_LEN      32
#define DBGMSG_SERVER_MSG_TEXT_LEN      2048

#define DBGMSG_SERVER_KEY_PATH_LEN      1024
#define DBGMSG_SERVER_KEY_PATH_DFT      "."
#define DBGMSG_SERVER_KEY_ID_DFT        0x01

typedef struct DBGMSG_SERVER_MSG {
        long                            msg_type;
        time_t                          msg_time;
        pid_t                           msg_pid;
        char *                          msg_name[DBGMSG_SERVER_MSG_NAME_LEN];
        char *                          msg_text[DBGMSG_SERVER_MSG_TEXT_LEN];
} DBGMSG_SERVER_MSG;

typedef struct DBGMSG_SERVER_CFG {
        key_t                           key;
        char                            key_path[DBGMSG_SERVER_KEY_PATH_LEN];
        int                             key_id;
} DBGMSG_SERVER_CFG;

typedef struct DBGMSG_SERVER_CTL {
        DBGMSG_SERVER_CFG               CFG;
        DBGMSG_SERVER_CFG *             cfg;
        key_t                           msg_key;
        int                             msg_qid;
        bool                            ready;
        time_t                          tnow;
        struct tm *                     local;
} DBGMSG_SERVER_CTL;
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

/*____________________________________________________________________________*/
/* DBG_SERVER */
/*------------------------------------*/
#define DBG_SERVER_LOG_PATH_LEN         1024
#define DBG_SERVER_LOG_NAME_LEN         1024
#define DBG_SERVER_LOG_PATH_NAME_LEN    (DBG_SERVER_LOG_PATH_LEN + DBG_SERVER_LOG_NAME_LEN)
#define DBG_SERVER_LOG_PATH_DFT         "./dbg"

#define DBG_SERVER_LOG_SIZE_MIN         2
#define DBG_SERVER_LOG_SIZE_MAX         100
#define DBG_SERVER_LOG_SIZE_DFT         DBG_SERVER_LOG_SIZE_MIN

#define DBG_SERVER_LOG_COUNT_MIN        2
#define DBG_SERVER_LOG_COUNT_MAX        100
#define DBG_SERVER_LOG_COUNT_DFT        DBG_SERVER_LOG_COUNT_MIN

typedef struct DBG_SERVER_STAT {
        pid_t                           pid;
        pid_t                           ppid;
} DBG_SERVER_STAT;

typedef struct DBG_SERVER_CFG {
        char                            log_path[DBG_SERVER_LOG_PATH_LEN];
        int                             log_size;                               /* MB */
        int                             log_count;
        DBGMSG_SERVER_CFG               DBGMSG_SERVER;
        DBGMSG_SERVER_CFG *             dbgmsg_server;
        bool                            dbgmsg_server_enable;
} DBG_SERVER_CFG;

typedef struct DBG_SERVER_CTL {
        DBGMSG_SERVER_CTL               DBGMSG_SERVER;
        DBGMSG_SERVER_CTL *             dbgmsg_server;
        char                            log_name[DBG_SERVER_LOG_NAME_LEN];
        char                            log_path_name[DBG_SERVER_LOG_PATH_NAME_LEN];
        int                             log_fd;
        int                             log_index;
        int                             log_index_top;
        int                             log_index_bottom;
} DBG_SERVER_CTL;

static int dbg_server_loop (void);
static int dbg_server_mkdir (void);
static int dbg_server_reset (void);
static void dbg_server_exit (int ret);
static void dbg_server_help (char * const name);
static int dbg_server_init (const int argc, char * const argv[]);
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/


#endif /* DEBUGSERVER_H_ */
