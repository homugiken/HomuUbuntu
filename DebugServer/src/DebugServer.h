
#ifndef DEBUGSERVER_H_
#define DEBUGSERVER_H_
/*____________________________________________________________________________*/
/* INCLUDE */
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
#define DEBUGSERVER_INF_ENABLE          1
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
#define DBGSTD(fmt,...)                 fprintf(stdout, "%04d|%s:"fmt"\r\n", __LINE__,__FUNCTION__,##__VA_ARGS__)
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
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

/*____________________________________________________________________________*/
/* DEBUGSERVER */
/*------------------------------------*/
#define DEBUGSERVER_PATH_LEN            1024
#define DEBUGSERVER_LOG_PATH_DFT        "./dbg"
#define DEBUGSERVER_DBGMSG_KEYID_DFT    0x01

typedef struct DEBUGSERVER_STAT {
        pid_t                           pid;
        pid_t                           ppid;
        bool                            dbgmsg_ready;
} DEBUGSERVER_STAT;

typedef struct DEBUGSERVER_CFG {
        char                            log_path[DEBUGSERVER_PATH_LEN];
        bool                            dbgmsg_enable;
        key_t                           dbgmsg_key;
        char                            dbgmsg_key_path[DEBUGSERVER_PATH_LEN];
        int                             dbgmsg_key_id;
} DEBUGSERVER_CFG;

typedef struct DEBUGSERVER_CTL {
        key_t                           dbgmsg_key;
        int                             dbgmsg_qid;
} DEBUGSERVER_CTL;
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#endif /* DEBUGSERVER_H_ */
