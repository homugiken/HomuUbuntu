
/*____________________________________________________________________________*/
/* INCLUDE */
#include "DebugServer.h"
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

/*____________________________________________________________________________*/
/* GLOBAL */
#define GLOBAL_STAT                     DEBUGSERVER_STAT
#define GLOBAL_CFG                      DEBUGSERVER_CFG
#define GLOBAL_CTL                      DEBUGSERVER_CTL

static GLOBAL_STAT                      GSTAT;
static GLOBAL_STAT * const              gstat = &GSTAT;
static GLOBAL_CFG                       GCFG;
static GLOBAL_CFG * const               gcfg = &GCFG;
static GLOBAL_CTL                       GCTL;
static GLOBAL_CTL * const               gctl = &GCTL;
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

/*____________________________________________________________________________*/
/* MAIN */
/*------------------------------------*/
static int
main_loop (void)
{       ENTR();
        int                             ret = -1;

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* main_loop */
/*------------------------------------*/
static int
main_reset (void)
{       ENTR();
        int                             ret = -1;

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* main_reset */
/*------------------------------------*/
static void
main_exit (void)
{       ENTR();
        int                             ret = -1;

        ret = 0;
exit:   EXIT();
        exit(ret);
        return;
}       /* main_exit */
/*------------------------------------*/
#define OPTL_LOG_PATH                   "path"
#define OPTC_LOG_PATH                   'p'
#define OPTL_DBGMSG_ENABLE              "msg"
#define OPTC_DBGMSG_ENABLE              'm'
#define OPTL_DBGMSG_KEY                 "key"
#define OPTC_DBGMSG_KEY                 'k'
#define OPTL_DBGMSG_KEY_PATH            "keypath"
#define OPTL_DBGMSG_KEY_ID              "keyid"

extern char *                           optarg;
static struct option                    optlist[] =
{
        {OPTL_LOG_PATH, required_argument, 0, OPTC_LOG_PATH},
        {OPTL_DBGMSG_ENABLE, no_argument, 0, OPTC_DBGMSG_ENABLE},
        {OPTL_DBGMSG_KEY, required_argument, 0, OPTC_DBGMSG_KEY},
        {OPTL_DBGMSG_KEY_PATH, required_argument, 0, 0},
        {OPTL_DBGMSG_KEY_ID, required_argument, 0, 0},
        {0, 0, 0, 0}
};

#define OPTARG_INVALID(ARG)             (ARG==NULL)||(ARG[0]=='-')||(strlen(ARG)<1)
#define ERR_OPTARG_INVALID() \
DOWHILE(if(OPTARG_INVALID(optarg)){ERR("option --%s need argument", optlist[optindex].name); goto error;})

static int
main_init (
        const int                       argc,
        char * const                    argv[])
{       ENTR();
        int                             ret = -1;
        int                             i;
        int                             optchar;
        int                             optindex;
        ERR_NPOS(argc); ERR_NULL(argv);

        ERR_NULL(gctl); MEMZERO(gctl, sizeof(GLOBAL_CTL));
        ERR_NULL(gcfg); MEMZERO(gcfg, sizeof(GLOBAL_CFG));
        ERR_NULL(gstat); MEMZERO(gstat, sizeof(GLOBAL_STAT));

        gstat->pid = getpid();
        gstat->ppid = getppid();
        LOG("pid=%d ppid=%d", gstat->pid, gstat->ppid);
        LOG("argc=%d argv=%p", argc, argv);
        for (i = 0; i < argc; i++)
        {
                LOG("argv[%d]=\"%s\"", i, argv[i]);
        }

        while (1)
        {
                optchar = getopt_long(argc, argv, "", optlist, &optindex);
                if (optchar == -1) { break; }
                switch (optchar)
                {
                case OPTC_LOG_PATH:
                        ERR_OPTARG_INVALID();
                        snprintf(gcfg->log_path, DEBUGSERVER_PATH_LEN, "%s", optarg);
                        INF("log_path=\"%s\"", gcfg->log_path);
                        break;
                case OPTC_DBGMSG_ENABLE:
                        gcfg->dbgmsg_enable = true;
                        INF("dbgmsg_enable=%s", STRBOOL(gcfg->dbgmsg_enable));
                        break;
                case OPTC_DBGMSG_KEY:
                        ERR_OPTARG_INVALID();
                        gcfg->dbgmsg_key = strtol(optarg, NULL, 0);
                        INF("dbgmsg_key=0x%X", gcfg->dbgmsg_key);
                        break;
                case 0:
                        if (strncmp(optlist[optindex].name, OPTL_DBGMSG_KEY_PATH, strlen(OPTL_DBGMSG_KEY_PATH)) == 0)
                        {
                                ERR_OPTARG_INVALID();
                                snprintf(gcfg->dbgmsg_key_path, DEBUGSERVER_PATH_LEN, "%s", optarg);
                                INF("dbgmsg_key_path=\"%s\"", gcfg->dbgmsg_key_path);
                        }
                        else if (strncmp(optlist[optindex].name, OPTL_DBGMSG_KEY_ID, strlen(OPTL_DBGMSG_KEY_ID)) == 0)
                        {
                                ERR_OPTARG_INVALID();
                                gcfg->dbgmsg_key_id = strtol(optarg, NULL, 0);
                                ERR_RANGE(gcfg->dbgmsg_key_id, 0x01, 0xFF);
                                INF("dbgmsg_key_id=%d", gcfg->dbgmsg_key_id);
                        }
                default:
                        break;
                }
        }

        if (strnlen(gcfg->log_path, DEBUGSERVER_PATH_LEN) < 1)
        {
                snprintf(gcfg->log_path, DEBUGSERVER_PATH_LEN, "%s", DEBUGSERVER_LOG_PATH_DFT);
        }
        LOG("log_path=%s", gcfg->log_path);

        LOG("dbgmsg_enable=%s", STRBOOL(gcfg->dbgmsg_enable));
        if ((gcfg->dbgmsg_enable) && (gcfg->dbgmsg_key != 0))
        {
                LOG("dbtgmsg_key=0x%X", gcfg->dbgmsg_key);
        }
        else if (gcfg->dbgmsg_enable)
        {
                if (strnlen(gcfg->dbgmsg_key_path, DEBUGSERVER_PATH_LEN) < 1)
                {
                        snprintf(gcfg->dbgmsg_key_path, DEBUGSERVER_PATH_LEN, "%s", argv[0]);
                }
                LOG("dbgmsg_key_path=\"%s\"", gcfg->dbgmsg_key_path);
                if ((gcfg->dbgmsg_key_id < 0x01) || (gcfg->dbgmsg_key_id > 0xFF))
                {
                        gcfg->dbgmsg_key_id = DEBUGSERVER_DBGMSG_KEYID_DFT;
                }
                LOG("dbgmsg_key_id=0x%02X", gcfg->dbgmsg_key_id);
        }

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* main_init */
/*------------------------------------*/
int
main (
        const int                       argc,
        char * const                    argv[])
{       ENTR();
        int                             ret = -1;
        ERR_NPOS(argc); ERR_NULL(argv);

        ret = main_init(argc, argv); ERR_NZERO(ret);

        ret = main_loop(); ERR_NZERO(ret);

        ret = 0;
exit:   EXIT();
        main_exit();
        return(ret);
error:  FAIL();
        goto exit;
}       /* main */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
