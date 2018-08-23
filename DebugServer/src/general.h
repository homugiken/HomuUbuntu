#ifndef GENERAL_H_
#define GENERAL_H_
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

#include "debug.h"

/*____________________________________________________________________________*/
/* UTILITY */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
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
/*························································*/
#define DO(JOB)                         do {JOB} while (0)
#define MEMZ(PTR,SIZE)                  memset(PTR,0,SIZE)
#define MALLOCZ(PTR,TYPE)               DO(PTR=malloc(sizeof(TYPE));if(PTR!=NULL){MEMZ(PTR,sizeof(TYPE));})
#define STRBOOL(VAL)                    VAL ? "true" : "false"
#define REGULATE(ARG,MIN,MAX)           DO(if(ARG<MIN){ARG=MIN;}else if(ARG>MAX){ARG=MAX;})
#define TRY(RETRY,FUNC)                 DO(int iter=0;while(iter++<RETRY){if((ret=FUNC)==0){break;}})
#define PAUSE()                         DO(TAG("PAUSE");while(getchar()!='g'){usleep(100000);})

/*____________________________________________________________________________*/
/* OPTION */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
extern int                              optind;
extern char *                           optarg;
#define OPTARG_INVALID(ARG)             (ARG==NULL)||(ARG[0]=='-')||(strlen(ARG)<1)

/*____________________________________________________________________________*/
/* STR_FMT */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define STR_FMT_DATE_TIME               "%04d%02d%02d-%02d%02d%02d"

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#endif /* GENERAL_H_ */
