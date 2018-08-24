#ifndef DEBUG_H_
#define DEBUG_H_
/*____________________________________________________________________________*/
/* INCLUDE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#include "general.h"
#include "dbgstd.h"
#include "dbgmsg.h"
#include "DebugServer.h"
#include "DebugClient.h"

/*____________________________________________________________________________*/
/* DEBUG */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define DBG_ENABLE_ALL                  1   /* ALL */
#define DBG_ENABLE_DBGSTD               1   /* DBGSTD */
#define DBG_ENABLE_DBG_CLNT             1   /* DBG_CLNT */
/*························································*/
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
#define DBG_VERBOSE_DFT                 DBG_VERBOSE_LOG
static uint32_t                         gverbose = DBG_VERBOSE_DFT;
/*························································*/
#if DBG_ENABLE_DBGSTD && DBG_ENABLE_ALL
#define DBGSTD(fmt,...)                 dbgstd_printf("#%04d|%s:"fmt"\r\n",__LINE__,__FUNCTION__,##__VA_ARGS__)
#else
#define DBGSTD(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_DBG_CLNT && DBG_ENABLE_ALL
static DBG_CLNT_CTL                     _gdbg_clnt = {0}, * const gdbg_clnt = &(_gdbg_clnt);
#define DBGCLNT(fmt,...)                dbg_clnt_printf("#%04d|%s:"fmt"\r\n",__LINE__,__FUNCTION__,##__VA_ARGS__)
#else
static DBG_CLNT_CTL * const             gdbg_clnt = NULL;
#define DBGCLNT(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_ALL                      /* DBG */
#define DBG(fmt,...)                    DO(DBGSTD(fmt,##__VA_ARGS__);DBGCLNT(fmt,##__VA_ARGS__);)
#else
#define DBG(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_ALL && DBG_ENABLE_ERR    /* ERR */
#define ERR(fmt,...)                    DO(if(gverbose>=DBG_VERBOSE_ERR){DBG("ERR!"fmt,##__VA_ARGS__);})
#else
#define ERR(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_ALL && DBG_ENABLE_WRN    /* WRN */
#define WRN(fmt,...)                    DO(if(gverbose>=DBG_VERBOSE_WRN){DBG("WRN!"fmt,##__VA_ARGS__);})
#else
#define WRN(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_ALL && DBG_ENABLE_LOG    /* LOG */
#define LOG(fmt,...)                    DO(if(gverbose>=DBG_VERBOSE_LOG){DBG(fmt,##__VA_ARGS__);})
#else
#define LOG(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_ALL && DBG_ENABLE_INF    /* INF */
#define INF(fmt,...)                    DO(if(gverbose>=DBG_VERBOSE_INF){DBG(fmt,##__VA_ARGS__);})
#else
#define INF(fmt,...)
#endif
/*························································*/
#if DBG_ENABLE_ALL && DBG_ENABLE_TAG    /* TAG */
#define TAG(NAME)                       DO(if(gverbose>=DBG_VERBOSE_TAG){DBG("<%s>",NAME);})
#else
#define TAG(NAME)
#endif
/*························································*/
#define FAIL()                          ERR("<FAIL>")
#define ENTR()                          TAG("ENTR")
#define EXIT()                          TAG("EXIT")
/*························································*/
#define CHK_NULL(CHK,PRT,ARG)           CHK(ARG==NULL,PRT("%s=NULL",#ARG))
#define CHK_NZERO(CHK,PRT,ARG)          CHK(ARG!=0,PRT("%s=%d!=0",#ARG,ARG))
#define CHK_NEG(CHK,PRT,ARG)            CHK(ARG<0,PRT("%s=%d<0",#ARG,ARG))
#define CHK_NPOS(CHK,PRT,ARG)           CHK(ARG<=0,PRT("%s=%d!>0",#ARG,ARG))
#define CHK_FALSE(CHK,PRT,ARG)          CHK(ARG!=true,PRT("%s=%d!=true",#ARG,ARG))
#define CHK_RANGE(CHK,PRT,ARG,MIN,MAX)  CHK((ARG<MIN)||(ARG>MAX),PRT("%s=%d!<%d,%d>",#ARG,ARG,MIN,MAX))
/*························································*/
#define ERR_CHK(CASE,PRT)               DO(if(CASE){PRT;GOERROR;})
#define ERR_NULL(ARG)                   CHK_NULL(ERR_CHK,ERR,ARG)
#define ERR_NZERO(ARG)                  CHK_NZERO(ERR_CHK,ERR,ARG)
#define ERR_NEG(ARG)                    CHK_NEG(ERR_CHK,ERR,ARG)
#define ERR_NPOS(ARG)                   CHK_NPOS(ERR_CHK,ERR,ARG)
#define ERR_FALSE(ARG)                  CHK_FALSE(ERR_CHK,ERR,ARG)
#define ERR_RANGE(ARG,MIN,MAX)          CHK_RANGE(ERR_CHK,ERR,ARG,MIN,MAX)
#define ERR_ERRNO()                     ERR("errno=%d %s", errno, strerror(errno))
#define ERR_OPTARG_INVALID()            DO(if(OPTARG_INVALID(optarg)){ERR("option --%s need argument", optlist[optindex].name); goto error;})
/*························································*/
#define WRN_CHK(CASE,PRT)               DO(if(CASE){PRT;})
#define WRN_NULL(ARG)                   CHK_NULL(WRN_CHK,WRN,ARG)
#define WRN_NZERO(ARG)                  CHK_NZERO(WRN_CHK,WRN,ARG)
#define WRN_NEG(ARG)                    CHK_NEG(WRN_CHK,WRN,ARG)
#define WRN_NPOS(ARG)                   CHK_NPOS(WRN_CHK,WRN,ARG)
#define WRN_FALSE(ARG)                  CHK_FALSE(WRN_CHK,WRN,ARG)
#define WRN_RANGE(ARG,MIN,MAX)          CHK_RANGE(WRN_CHK,WRN,ARG,MIN,MAX)

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#endif /* DEBUG_H_ */
