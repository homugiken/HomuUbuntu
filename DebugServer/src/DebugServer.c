
/*____________________________________________________________________________*/
/* INCLUDE */
#include "DebugServer.h"
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

/*____________________________________________________________________________*/
/* GLOBAL */
#define GLOBAL_STAT                     DBG_SERVER_STAT
#define GLOBAL_CFG                      DBG_SERVER_CFG
#define GLOBAL_CTL                      DBG_SERVER_CTL

static GLOBAL_STAT                      GSTAT;
static GLOBAL_STAT * const              gstat = &GSTAT;
static GLOBAL_CFG                       GCFG;
static GLOBAL_CFG * const               gcfg = &GCFG;
static GLOBAL_CTL                       GCTL;
static GLOBAL_CTL * const               gctl = &GCTL;
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

/*____________________________________________________________________________*/
/* DBGMSG_SERVER*/
/*------------------------------------*/
static int dbgmsg_server_reset (DBGMSG_SERVER_CTL * const ctl);
static void dbgmsg_server_release (DBGMSG_SERVER_CTL * const ctl);
static int dbgmsg_server_init (DBGMSG_SERVER_CTL * const ctl, DBGMSG_SERVER_CFG * const cfg);
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

/*____________________________________________________________________________*/
/* DBGSTD */
/*------------------------------------*/
static void
dbgstd_printf (
        const char *                    fmt,
        ...
        )
{
        static DBGSTD_CTL               CTL = {0};
        static DBGSTD_CTL * const       ctl = &CTL;

        MEMZERO(ctl->text, DBGSTD_TEXT_LEN);
        ctl->tnow = time(NULL);
        ctl->local = localtime(&(ctl->tnow));
        snprintf(ctl->time_stamp, DBGSTD_TIME_STAMP_LEN, "%04d%02d%02d-%02d%02d%02d",
                 (ctl->local->tm_year + 1900), (ctl->local->tm_mon + 1), ctl->local->tm_mday,
                 ctl->local->tm_hour, ctl->local->tm_min, ctl->local->tm_sec);

        va_start(ctl->vargs, fmt);
        vsnprintf(ctl->text, DBGSTD_TEXT_LEN, fmt, ctl->vargs);
        va_end(ctl->vargs);

        fprintf(stdout, "%s|%s\r\n", ctl->time_stamp, ctl->text);

        return;
}       /* dbgstd_printf */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

/*____________________________________________________________________________*/
/* DBGMSG */
/*------------------------------------*/
static int
dbgmsg_server_printf (
        DBGMSG_SERVER_CTL * const       ctl)
{       ENTR();
        int                             ret = -1;
        ERR_NULL(ctl);

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* dbgmsg_server_printf */
/*------------------------------------*/
static int
dbgmsg_server_send (
        DBGMSG_SERVER_CTL * const       ctl)
{       ENTR();
        int                             ret = -1;
        ERR_NULL(ctl);

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* dbgmsg_server_send */
/*------------------------------------*/
static int
dbgmsg_server_recv (
        DBGMSG_SERVER_CTL * const       ctl)
{       ENTR();
        int                             ret = -1;
        ERR_NULL(ctl);

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* dbgmsg_server_recv */
/*------------------------------------*/
static int
dbgmsg_server_reset (
        DBGMSG_SERVER_CTL * const       ctl)
{       ENTR();
        int                             ret = -1;
        DBGMSG_SERVER_CFG *                    cfg = NULL;
        ERR_NULL(ctl);

        if (ctl->cfg != NULL)
        {
                MALLOCZERO(cfg, DBGMSG_SERVER_CFG); ERR_NULL(cfg);
                memcpy(cfg, ctl->cfg, sizeof(DBGMSG_SERVER_CFG));
                INF("original cfg backup");
        }

        dbgmsg_server_release(ctl);

        ret = dbgmsg_server_init(ctl, cfg); ERR_NZERO(ret);

        ret = 0;
exit:   EXIT();
        free(cfg);
        return(ret);
error:  FAIL();
        ERR_ERRNO();
        goto exit;
}       /* dbgmsg_server_reset */
/*------------------------------------*/
static void
dbgmsg_server_release (
        DBGMSG_SERVER_CTL * const       ctl)
{       ENTR();
        ERR_NULL(ctl);

        // if (ctl->msg_qid >= 0)
        // {
        //         if (msgctl(ctl->msg_qid, IPC_RMID, NULL) < 0)
        //         {
        //                 ERR_ERRNO();
        //         }
        // }
        MEMZERO(ctl, sizeof(DBGMSG_SERVER_CTL));

exit:   EXIT();
        return;
error:  FAIL();
        goto exit;
}       /* dbgmsg_server_release */
/*------------------------------------*/
static int
dbgmsg_server_init (
        DBGMSG_SERVER_CTL * const       ctl,
        DBGMSG_SERVER_CFG * const       cfg)
{       ENTR();
        int                             ret = -1;
        ERR_NULL(ctl);

        // if (ctl->cfg != NULL)
        // {
        //         dbgmsg_server_release(ctl);
        // }
        // ctl->cfg = &(ctl->CFG);

        // if (cfg == NULL)
        // {       /* use default cfg */
        //         snprintf(ctl->cfg->key_path, DBGMSG_SERVER_KEY_PATH_LEN, DBGMSG_SERVER_KEY_PATH_DFT);
        //         INF("key_path=\"%s\"", ctl->cfg->key_path);
        //         ctl->cfg->key_id = DBGMSG_SERVER_KEY_ID_DFT;
        //         INF("key_id=0x%X", ctl->cfg->key_id);
        //         ctl->cfg->log_size = DBGMSG_SERVER_LOG_SIZE_DFT;
        //         INF("log_size=0x%X", ctl->cfg->log_size);
        //         ctl->cfg->log_count = DBGMSG_SERVER_LOG_COUNT_DFT;
        //         INF("log_count=0x%X", ctl->cfg->log_count);
        // }
        // else
        // {
        //         if (cfg->key != 0)
        //         {       /* use given key */
        //                 ctl->cfg->key = cfg->key;
        //                 INF("key=0x%X", ctl->cfg->key);
        //         }
        //         else
        //         {
        //                 if (strnlen(cfg->key_path, DBGMSG_SERVER_KEY_PATH_LEN) < 1)
        //                 {       /* use default key_path */
        //                         snprintf(ctl->cfg->key_path, DBGMSG_SERVER_KEY_PATH_LEN, DBGMSG_SERVER_KEY_PATH_DFT);
        //                 }
        //                 else
        //                 {       /* use given key_path */
        //                         snprintf(ctl->cfg->key_path, DBGMSG_SERVER_KEY_PATH_LEN, "%s", cfg->key_path);
        //                 }
        //                 INF("key_path=\"%s\"", ctl->cfg->key_path);

        //                 if ((cfg->key_id < 0x01) || (cfg->key_id > 0xFF))
        //                 {       /* use default key_id */
        //                         ctl->cfg->key_id = DBGMSG_SERVER_KEY_ID_DFT;
        //                 }
        //                 else
        //                 {       /* use given key_id */
        //                         ctl->cfg->key_id = cfg->key_id;
        //                 }
        //                 INF("key_id=%d", ctl->cfg->key_id);
        //         }

        //         if (cfg->log_size == 0)
        //         {
        //                 ctl->cfg->log_size = DBGMSG_SERVER_LOG_SIZE_DFT;
        //         }
        //         else
        //         {
        //                 ctl->cfg->log_size = cfg->log_size;
        //         }
        //         INF("log_size=0x%X", ctl->cfg->log_size);

        //         if (cfg->log_count == 0)
        //         {
        //                 ctl->cfg->log_count = DBGMSG_SERVER_LOG_COUNT_DFT;
        //         }
        //         else
        //         {
        //                 ctl->cfg->log_count = cfg->log_count;
        //         }
        //         INF("log_count=0x%X", ctl->cfg->log_count);
        // }

        // if (ctl->cfg->key == 0)
        // {       /* generate key from key_path and key_id */
        //         ctl->cfg->key = ftok(ctl->cfg->key_path, ctl->cfg->key_id); ERR_NEG(ctl->cfg->key);
        //         INF("key=0x%X", ctl->cfg->key);
        // }
        // ctl->msg_key = ctl->cfg->key;
        // LOG("msg_key=0x%X", ctl->msg_key);

        // ctl->msg_qid = msgget(ctl->msg_key, (IPC_CREAT | IPC_EXCL));
        // if ((ctl->msg_qid < 0) && (errno == EEXIST))
        // {       /* remove old msgq and generate new msgq */
        //         ctl->msg_qid = msgget(ctl->msg_key, 0); ERR_NEG(ctl->msg_qid);
        //         ret = msgctl(ctl->msg_qid, IPC_RMID, NULL); ERR_NEG(ret);
        //         INF("msg_qid=0x%X removed", ctl->msg_qid);
        //         ctl->msg_qid = msgget(ctl->msg_key, IPC_CREAT); ERR_NEG(ctl->msg_qid);
        // }
        // ERR_NEG(ctl->msg_qid);
        // LOG("msg_qid=0x%X", ctl->msg_qid);

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        ERR_ERRNO();
        goto exit;
}       /* dbgmsg_server_init */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

/*____________________________________________________________________________*/
/* DBG_SERVER */
/*------------------------------------*/
static int
dbg_server_loop (void)
{       ENTR();
        int                             ret = -1;

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* dbg_server_loop */
/*------------------------------------*/
static int
dbg_server_scan (
        const char *                    fpath,
        const struct stat *             sb,
        int                             typeflag)
{       ENTR();
        int                             ret = -1;
        ERR_NULL(fpath); ERR_NULL(sb);

        if (typeflag != FTW_F)
        {
                ret = 0;
                goto exit;
        }
        INF("fpath=\"%s\"", fpath);


        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* dbg_server_scan */
/*------------------------------------*/
static int
dbg_server_ftw (void)
{       ENTR();
        int                             ret = -1;

        ret = ftw(gcfg->log_path, dbg_server_scan, 2); ERR_NZERO(ret);

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* dbg_server_ftw */
/*------------------------------------*/
static int
dbg_server_mkdir (void)
{       ENTR();
        int                             ret = -1;
        int                             i;
        int                             len;
        char                            strdir[DBG_SERVER_LOG_PATH_LEN] = {'0'};

        len = strlen(gcfg->log_path);
        for (i = 0; i < len; i++)
        {
                if (((gcfg->log_path[i] == '/') && (i > 0)) || (i == (len - 1)))
                {
                        snprintf(strdir, (i + 2), "%s", gcfg->log_path);
                        INF("mkdir=\"%s\"", strdir);
                        ret = mkdir(strdir, DEFFILEMODE);
                        if ((ret < 0) && (errno != EEXIST))
                        {
                                ERR_NZERO(ret);
                        }
                }
        }

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* dbg_server_mkdir */
/*------------------------------------*/
static int
dbg_server_reset (void)
{       ENTR();
        int                             ret = -1;

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* dbg_server_reset */
/*------------------------------------*/
static void
dbg_server_exit (
        int                             ret)
{       ENTR();

        if (gctl->dbgmsg_server)
        {
                dbgmsg_server_release(gctl->dbgmsg_server);
        }

exit:   EXIT();
        exit(ret);
        return;
}       /* dbg_server_exit */
/*------------------------------------*/
#define OPTL_HELP                       "help"
#define OPTC_HELP                       'h'
#define OPTL_LOG_PATH                   "path"
#define OPTC_LOG_PATH                   'p'
#define OPTL_LOG_SIZE                   "size"
#define OPTC_LOG_SIZE                   's'
#define OPTL_LOG_COUNT                  "count"
#define OPTC_LOG_COUNT                  'c'
/* DBGMSG */
#define OPTL_DBGMSG_ENABLE              "msg"
#define OPTC_DBGMSG_ENABLE              'm'
#define OPTL_DBGMSG_KEY                 "key"
#define OPTC_DBGMSG_KEY                 'k'
#define OPTL_DBGMSG_KEY_PATH            "keypath"
#define OPTL_DBGMSG_KEY_ID              "keyid"

extern char *                           optarg;
static struct option                    optlist[] =
{
        {OPTL_HELP,             required_argument,      0, OPTC_HELP},
        {OPTL_LOG_PATH,         required_argument,      0, OPTC_LOG_PATH},
        {OPTL_LOG_SIZE,         required_argument,      0, OPTC_LOG_SIZE},
        {OPTL_LOG_COUNT,        required_argument,      0, OPTC_LOG_COUNT},
        /* DBGMSG */
        {OPTL_DBGMSG_ENABLE,    no_argument,            0, OPTC_DBGMSG_ENABLE},
        {OPTL_DBGMSG_KEY,       required_argument,      0, OPTC_DBGMSG_KEY},
        {OPTL_DBGMSG_KEY_PATH,  required_argument,      0, 0},
        {OPTL_DBGMSG_KEY_ID,    required_argument,      0, 0},
        {0, 0, 0, 0}
};

#define OPTARG_INVALID(ARG)             (ARG==NULL)||(ARG[0]=='-')||(strlen(ARG)<1)
#define ERR_OPTARG_INVALID() \
DOWHILE(if(OPTARG_INVALID(optarg)){ERR("option --%s need argument", optlist[optindex].name); goto error;})
/*------------------------------------*/
static void
dbg_server_help (
        char * const                    name)
{       ENTR();

        LOG("%s options:", name);
        LOG("--%s\t\tshow help info", OPTL_HELP);
        LOG("--%s\t\tlog file storage path", OPTL_LOG_PATH);
        LOG("--%s\t\tlog file size limit in MB", OPTL_LOG_SIZE);
        LOG("--%s\t\tlog file count limit", OPTL_LOG_COUNT);
        LOG("--%s\t\tdbgmsg server enable", OPTL_DBGMSG_ENABLE);
        LOG("--%s\t\tdbgmsg server key, used to fetch dbgmsg qid", OPTL_DBGMSG_KEY);
        LOG("--%s\tdbgmsg server key path, used to generate key when not assigned", OPTL_DBGMSG_KEY_PATH);
        LOG("--%s\t\tdbgmsg server key id, used to generate key when not assigned", OPTL_DBGMSG_KEY_ID);

exit:   EXIT();
        return;
}       /* dbg_server_help */
/*------------------------------------*/
static int
dbg_server_init (
        const int                       argc,
        char * const                    argv[])
{       ENTR();
        int                             ret = -1;
        int                             optchar;
        int                             optindex;
        ERR_NPOS(argc); ERR_NULL(argv);

        ERR_NULL(gstat);
        MEMZERO(gstat, sizeof(GLOBAL_STAT));

        ERR_NULL(gcfg);
        MEMZERO(gcfg, sizeof(GLOBAL_CFG));
        gcfg->dbgmsg_server = &(gcfg->DBGMSG_SERVER);

        ERR_NULL(gctl);
        MEMZERO(gctl, sizeof(GLOBAL_CTL));

        gstat->pid  = getpid();
        gstat->ppid = getppid();
        LOG("pid=%d ppid=%d argc=%d argv=%p", gstat->pid, gstat->ppid, argc, argv);
        for (int i = 0; i < argc; i++)
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
                        snprintf(gcfg->log_path, DBG_SERVER_LOG_PATH_LEN, "%s", optarg);
                        INF("log_path=\"%s\"", gcfg->log_path);
                        break;
                case OPTC_LOG_SIZE:
                        ERR_OPTARG_INVALID();
                        gcfg->log_size = strtol(optarg, NULL, 0);
                        ERR_RANGE(gcfg->log_size, DBG_SERVER_LOG_SIZE_MIN, DBG_SERVER_LOG_SIZE_MAX);
                        INF("log_size=%dMB", gcfg->log_size);
                        break;
                case OPTC_LOG_COUNT:
                        ERR_OPTARG_INVALID();
                        gcfg->log_count = strtol(optarg, NULL, 0);
                        ERR_RANGE(gcfg->log_count, DBG_SERVER_LOG_COUNT_MIN, DBG_SERVER_LOG_COUNT_MAX);
                        INF("log_count=%d", gcfg->log_count);
                        break;
                /* DBGMSG */
                case OPTC_DBGMSG_ENABLE:
                        gcfg->dbgmsg_server_enable = true;
                        INF("dbgmsg_server_enable=%s", STRBOOL(gcfg->dbgmsg_server_enable));
                        break;
                case OPTC_DBGMSG_KEY:
                        ERR_OPTARG_INVALID();
                        gcfg->dbgmsg_server->key = strtol(optarg, NULL, 0);
                        INF("key=0x%X", gcfg->dbgmsg_server->key);
                        break;
                case 0:
                /* DBGMSG */
                        if (strncmp(optlist[optindex].name, OPTL_DBGMSG_KEY_PATH, strlen(OPTL_DBGMSG_KEY_PATH)) == 0)
                        {
                                ERR_OPTARG_INVALID();
                                snprintf(gcfg->dbgmsg_server->key_path, DBGMSG_SERVER_KEY_PATH_LEN, "%s", optarg);
                                INF("key_path=\"%s\"", gcfg->dbgmsg_server->key_path);
                        }
                        else if (strncmp(optlist[optindex].name, OPTL_DBGMSG_KEY_ID, strlen(OPTL_DBGMSG_KEY_ID)) == 0)
                        {
                                ERR_OPTARG_INVALID();
                                gcfg->dbgmsg_server->key_id = strtol(optarg, NULL, 0);
                                ERR_RANGE(gcfg->dbgmsg_server->key_id, 0x01, 0xFF);
                                INF("key_id=%d", gcfg->dbgmsg_server->key_id);
                        }
                default:
                        break;
                }
        }

        if (strnlen(gcfg->log_path, DBG_SERVER_LOG_PATH_LEN) < 1)
        {       /* use default log_path */
                snprintf(gcfg->log_path, DBG_SERVER_LOG_PATH_LEN, "%s", DBG_SERVER_LOG_PATH_DFT);
        }
        LOG("log_path=%s", gcfg->log_path);

        if (gcfg->log_size < 1)
        {       /* use default log_size */
                gcfg->log_size = DBG_SERVER_LOG_SIZE_DFT;
        }
        ERR_RANGE(gcfg->log_size, DBG_SERVER_LOG_SIZE_MIN, DBG_SERVER_LOG_SIZE_MAX);
        LOG("log_size=%dMB", gcfg->log_size);

        if (gcfg->log_count < 1)
        {       /* use default log_count */
                gcfg->log_count = DBG_SERVER_LOG_COUNT_DFT;
        }
        ERR_RANGE(gcfg->log_count, DBG_SERVER_LOG_COUNT_MIN, DBG_SERVER_LOG_COUNT_MAX);
        LOG("log_count=%d", gcfg->log_count);

        ret = dbg_server_mkdir(); ERR_NZERO(ret);

        /* dbg_server_log_scan */
        ret = dbg_server_ftw(); ERR_NZERO(ret);
        /* dbg_server_log_create */



        if (gcfg->dbgmsg_server_enable)
        {
                LOG("dbgmsg_server_enable=%s", STRBOOL(gcfg->dbgmsg_server_enable));
                gctl->dbgmsg_server = &(gctl->DBGMSG_SERVER);
                ret = dbgmsg_server_init(gctl->dbgmsg_server, gcfg->dbgmsg_server); ERR_NZERO(ret);
        }

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        ERR_ERRNO();
        goto exit;
}       /* dbg_server_init */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

/*____________________________________________________________________________*/
/* MAIN */
/*------------------------------------*/
int
main (
        const int                       argc,
        char * const                    argv[])
{       ENTR();
        int                             ret = -1;
        ERR_NPOS(argc); ERR_NULL(argv);

        if (argc < 2)
        {
                dbg_server_help(argv[0]);
                ret = 0;
                goto exit;
        }

        ret = dbg_server_init(argc, argv); ERR_NZERO(ret);

        ret = dbg_server_loop(); ERR_NZERO(ret);

        ret = 0;
exit:   EXIT();
        dbg_server_exit(ret);
        return(ret);
error:  FAIL();
        goto exit;
}       /* main */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
