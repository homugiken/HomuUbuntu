
/*____________________________________________________________________________*/
/* INCLUDE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#include "DebugServer.h"

/*____________________________________________________________________________*/
/* GLOBAL */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
typedef struct GLOBAL_CFG {
    DBG_SVR_CFG                         _dbg_svr, * dbg_svr;
    uint8_t                             verbose;
} GLOBAL_CFG;

typedef struct GLOBAL_CTL {
    DBG_SVR_CTL                         _dbg_svr, * dbg_svr;
} GLOBAL_CTL;

static GLOBAL_CFG                       _gcfg, * const gcfg = &_gcfg;
static GLOBAL_CTL                       _gctl, * const gctl = &_gctl;
static uint8_t                          gverbose = DBG_SVR_VERBOSE_DFT;

/*____________________________________________________________________________*/
/* OPTION */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
extern char *                           optarg;
static struct option                    optlist[] =
{
    /* GLOBAL */
    {GOPTL_HELP,                        no_argument,        0,                  GOPTC_HELP},
    {GOPTL_VERBOSE,                     required_argument,  0,                  GOPTC_VERBOSE},
    /* DBG_SVR */
    {DBG_SVR_OPTL_LOG_PATH,             required_argument,  0,                  DBG_SVR_OPTC_LOG_PATH},
    {DBG_SVR_OPTL_LOG_SIZE,             required_argument,  0,                  DBG_SVR_OPTC_LOG_SIZE},
    {DBG_SVR_OPTL_LOG_COUNT,            required_argument,  0,                  DBG_SVR_OPTC_LOG_COUNT},
    {DBG_SVR_OPTL_DBGMSG_SVR,           no_argument,        0,                  DBG_SVR_OPTC_DBGMSG_SVR},
    /* DBGMSG_SVR */
    {DBGMSG_SVR_OPTL_KEY,               required_argument,  0,                  DBGMSG_SVR_OPTC_KEY},
    {DBGMSG_SVR_OPTL_KEY_PATH,          required_argument,  0,                  0},
    {DBGMSG_SVR_OPTL_KEY_ID,            required_argument,  0,                  0},
    {0, 0, 0, 0}
};

#define OPTARG_INVALID(ARG)             (ARG==NULL)||(ARG[0]=='-')||(strlen(ARG)<1)
#define ERR_OPTARG_INVALID() \
DOWHILE(if(OPTARG_INVALID(optarg)){ERR("option --%s need argument", optlist[optindex].name); goto error;})

/*____________________________________________________________________________*/
/* DECLARE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
static void dbgstd_printf (const char * fmt, ...);
static void dbgmsg_printf (const char * fmt, ...);

/*____________________________________________________________________________*/
/* DBGSTD */
/*------------------------------------*/
static void
dbgstd_printf (
    const char *                        fmt,
    ...)
{
    static DBGSTD_CTL                   _ctl, * const ctl = &_ctl;

#if 0
    MEMZ(ctl->text, DBGSTD_TEXT_LEN);
#else
    ctl->text[0] = '\0';
#endif

    ctl->t_now = time(NULL);
    ctl->local = localtime(&(ctl->t_now));
#if 0
    snprintf(ctl->time, DBGSTD_TIME_LEN, "%04d/%02d/%02d-%02d:%02d:%02d",
             (ctl->LOCAL_YEAR), (ctl->LOCAL_MON), ctl->LOCAL_DAY,
             ctl->LOCAL_HOUR, ctl->LOCAL_MIN, ctl->LOCAL_SEC);
#else
    snprintf(ctl->time, DBGSTD_TIME_LEN, "%04d%02d%02d-%02d%02d%02d",
             (ctl->LOCAL_YEAR), (ctl->LOCAL_MON), ctl->LOCAL_DAY,
             ctl->LOCAL_HOUR, ctl->LOCAL_MIN, ctl->LOCAL_SEC);
#endif

    va_start(ctl->vargs, fmt);
    vsnprintf(ctl->text, DBGSTD_TEXT_LEN, fmt, ctl->vargs);
    va_end(ctl->vargs);

    fprintf(stdout, "%s|%s\r\n", ctl->time, ctl->text);

    return;
}

/*____________________________________________________________________________*/
/* DBGMSG_SVR */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/


static void
dbgmsg_svr_cfg_show (
    DBGMSG_SVR_CFG * const              cfg)
{   ENTR();

    ERR_NULL(cfg);


LEXIT;
    return;
LERROR;
    GOEXIT;
}

static int
dbgmsg_svr_cfg (
    DBGMSG_SVR_CFG * const              cfg,
    const int                           argc,
    char * const                        argv[])
{   ENTR();

    int                                 ret = -1;
    int                                 optchar;
    int                                 optindex;

    ERR_NULL(cfg); ERR_NPOS(argc); ERR_NULL(argv);

    MEMZ(cfg, sizeof(DBGMSG_SVR_CFG));
    while (1)
    {
        optchar = getopt_long_only(argc, argv, "", optlist, &optindex);
        if (optchar == -1) { break; }

        switch (optchar)
        {
        case DBGMSG_SVR_OPTC_KEY:
            ERR_OPTARG_INVALID();
            cfg->key = strtol(optarg, NULL, 0);
            break;
        case 0:
            if (strncmp(optlist[optindex].name, DBGMSG_SVR_OPTL_KEY_PATH, strlen(DBGMSG_SVR_OPTL_KEY_PATH)) == 0)
            {
                ERR_OPTARG_INVALID();
                snprintf(cfg->key_path, DBGMSG_SVR_KEY_PATH_LEN, "%s", optarg);
            }
            else if (strncmp(optlist[optindex].name, DBGMSG_SVR_OPTL_KEY_ID, strlen(DBGMSG_SVR_OPTL_KEY_ID)) == 0)
            {
                ERR_OPTARG_INVALID();
                cfg->key_id = strtol(optarg, NULL, 0);
                ERR_RANGE(cfg->key_id, 0x01, 0xFF);
            }
            break;
        default:
                break;
        }
    }


    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static void
dbgmsg_svr_help (void)
{   ENTR();

    printf("-%c/--%s\t%s\r\n", DBGMSG_SVR_OPTC_KEY, DBGMSG_SVR_OPTL_KEY, DBGMSG_SVR_OPTS_KEY);
    printf("--%s\t%s\r\n", DBGMSG_SVR_OPTL_KEY_PATH, DBGMSG_SVR_OPTS_KEY_PATH);
    printf("--%s\t\t%s\r\n", DBGMSG_SVR_OPTL_KEY_ID, DBGMSG_SVR_OPTS_KEY_ID);

LEXIT;
    return;
}

static void
dbgmsg_svr_release (
    DBGMSG_SVR_CTL * const              ctl)
{   ENTR();

    ERR_NULL(ctl);

LEXIT;
    return;
LERROR;
    GOEXIT;
}

static int
dbgmsg_svr_init (
    DBGMSG_SVR_CTL * const              ctl,
    DBGMSG_SVR_CFG * const              cfg)
{   ENTR();
    int                                 ret = -1;

    ERR_NULL(ctl); ERR_NULL(cfg);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    dbgmsg_svr_release(ctl);
    GOEXIT;
}

/*____________________________________________________________________________*/
/* DBG_SVR */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
static int
dbg_svr_idx_delete_increase (
    DBG_SVR_CTL * const                 ctl,
    uint32_t                            num)
{   ENTR();
    int                                 ret = -1;

    ERR_NULL(ctl);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static int
dbg_svr_idx_create_increase (
    DBG_SVR_CTL * const                 ctl,
    uint32_t                            num)
{   ENTR();
    int                                 ret = -1;

    ERR_NULL(ctl);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static int
dbg_svr_idx_write (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;

    ERR_NULL(ctl);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static int
dbg_svr_idx_read (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;

    ERR_NULL(ctl);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static void
dbg_svr_idx_close (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    ERR_NULL(ctl);

    if (ctl->idx_fp != NULL)
    {
        fclose(ctl->idx_fp);
    }

LEXIT;
    return;
LERROR;
    GOEXIT;
}

static int
dbg_svr_idx_open (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;

    ERR_NULL(ctl);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*····································*/
static int
dbg_svr_log_shift (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;

    ERR_NULL(ctl);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static int
dbg_svr_log_write (
    DBG_SVR_CTL * const                 ctl,
    uint8_t * const                     buf,
    uint32_t                            len)
{   ENTR();
    int                                 ret = -1;

    ERR_NULL(ctl);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static int
dbg_svr_log_write_trailer (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;

    ERR_NULL(ctl);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static int
dbg_svr_log_write_header (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;

    ERR_NULL(ctl);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static void
dbg_svr_log_close (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    ERR_NULL(ctl);

    if (ctl->log_fp != NULL)
    {
        fclose(ctl->log_fp);
    }

LEXIT;
    return;
LERROR;
    GOEXIT;
}

static int
dbg_svr_log_open (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;

    ERR_NULL(ctl);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*····································*/
static int
dbg_svr_mkdir (
    DBG_SVR_CFG * const                 cfg)
{   ENTR();
    int                                 ret = -1;
    int                                 len;
    char                                strdir[DBG_SVR_LOG_PATH_LEN];

    len = strlen(cfg->log_path);
    for (int i = 0; i < len; i++)
    {
        if (((cfg->log_path[i] == '/') && (i > 0)) || (i == (len - 1)))
        {
            MEMZ(strdir, DBG_SVR_LOG_PATH_LEN);
            snprintf(strdir, (i + 2), "%s", cfg->log_path);
            INF("mkdir=\"%s\"", strdir);
            ret = mkdir(strdir, DEFFILEMODE);
            if ((ret < 0) && (errno != EEXIST))
            {
                ERR_NZERO(ret);
            }
        }
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static void
dbg_svr_cfg_show (
    DBG_SVR_CFG * const                 cfg)
{   ENTR();

    ERR_NULL(cfg);

    LOG("dbg_svr->cfg->log_path=\"%s\"", cfg->log_path);
    LOG("dbg_svr->cfg->log_size=%d", cfg->log_size);
    LOG("dbg_svr->cfg->log_count=%d", cfg->log_count);
    LOG("dbg_svr->cfg->dbgmsg_svr_enable=\"%s\"", STRBOOL(cfg->dbgmsg_svr_enable));

    if (cfg->dbgmsg_svr_enable == true)
    {
        dbgmsg_svr_cfg_show(cfg->dbgmsg_svr);
    }

LEXIT;
    return;
LERROR;
    GOEXIT;
}

static int
dbg_svr_cfg (
    DBG_SVR_CFG * const                 cfg,
    const int                           argc,
    char * const                        argv[])
{   ENTR();

    int                                 ret = -1;
    int                                 optchar;
    int                                 optindex;

    ERR_NULL(cfg); ERR_NPOS(argc); ERR_NULL(argv);

    MEMZ(cfg, sizeof(DBG_SVR_CFG));
    while (1)
    {
        optchar = getopt_long_only(argc, argv, "", optlist, &optindex);
        if (optchar == -1) { break; }

        switch (optchar)
        {
        case DBG_SVR_OPTC_LOG_PATH:
            ERR_OPTARG_INVALID();
            snprintf(cfg->log_path, DBG_SVR_LOG_PATH_LEN, "%s", optarg);
            break;
        case DBG_SVR_OPTC_LOG_SIZE:
            ERR_OPTARG_INVALID();
            cfg->log_size = strtol(optarg, NULL, 0);
            break;
        case DBG_SVR_OPTC_LOG_COUNT:
            ERR_OPTARG_INVALID();
            cfg->log_count = strtol(optarg, NULL, 0);
            break;
        case DBG_SVR_OPTC_DBGMSG_SVR:
            cfg->dbgmsg_svr_enable = true;
            break;
        default:
                break;
        }
    }

    if (strnlen(cfg->log_path, DBG_SVR_LOG_PATH_LEN) < 1)
    {
        snprintf(cfg->log_path, DBG_SVR_LOG_PATH_LEN, "%s", DBG_SVR_LOG_PATH_DFT);
    }

    if (cfg->log_size < 1)
    {
        cfg->log_size = DBG_SVR_LOG_SIZE_DFT;
    }
    ERR_RANGE(cfg->log_size, DBG_SVR_LOG_SIZE_MIN, DBG_SVR_LOG_SIZE_MAX);

    if (cfg->log_count < 1)
    {
        cfg->log_count = DBG_SVR_LOG_COUNT_DFT;
    }
    ERR_RANGE(cfg->log_count, DBG_SVR_LOG_COUNT_MIN, DBG_SVR_LOG_COUNT_MAX);

    if (cfg->dbgmsg_svr_enable == true)
    {
        cfg->dbgmsg_svr = &(cfg->_dbgmsg_svr);
        ret = dbgmsg_svr_cfg(cfg->dbgmsg_svr, argc, argv); ERR_NZERO(ret);
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static void
dbg_svr_help (void)
{   ENTR();

    printf("-%c/--%s\t%s\r\n", DBG_SVR_OPTC_LOG_PATH, DBG_SVR_OPTL_LOG_PATH, DBG_SVR_OPTS_LOG_PATH);
    printf("-%c/--%s\t%s <%d,%d>\r\n", DBG_SVR_OPTC_LOG_SIZE, DBG_SVR_OPTL_LOG_SIZE, DBG_SVR_OPTS_LOG_SIZE, DBG_SVR_LOG_SIZE_MIN, DBG_SVR_LOG_SIZE_MAX);
    printf("-%c/--%s\t%s <%d,%d>\r\n", DBG_SVR_OPTC_LOG_COUNT, DBG_SVR_OPTL_LOG_COUNT, DBG_SVR_OPTS_LOG_COUNT, DBG_SVR_LOG_COUNT_MIN, DBG_SVR_LOG_COUNT_MAX);
    printf("-%c/--%s\t%s\r\n", DBG_SVR_OPTC_DBGMSG_SVR, DBG_SVR_OPTL_DBGMSG_SVR, DBG_SVR_OPTS_DBGMSG_SVR);

    dbgmsg_svr_help();

LEXIT;
    return;
}

static void
dbg_svr_release (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();

    ERR_NULL(ctl);

    dbg_svr_log_close(ctl);
    dbg_svr_idx_close(ctl);

    dbgmsg_svr_release(ctl->dbgmsg_svr);

    MEMZ(ctl, sizeof(dbg_svr_release));

LEXIT;
    return;
LERROR;
    GOEXIT;
}

static int
dbg_svr_init (
    DBG_SVR_CTL * const                 ctl,
    DBG_SVR_CFG * const                 cfg)
{   ENTR();
    int                                 ret = -1;

    ERR_NULL(ctl); ERR_NULL(cfg);

    if (ctl->cfg != NULL)
    {
        dbg_svr_release(ctl);
    }
    ctl->cfg = cfg;

    ctl->pid = getpid();
    ctl->ppid = getppid();
    LOG("pid=%d ppid=%d", ctl->pid, ctl->ppid);

    ret = dbg_svr_mkdir(cfg); ERR_NZERO(ret);
    ret = dbg_svr_idx_open(ctl); ERR_NZERO(ret);
    ret = dbg_svr_idx_read(ctl); ERR_NZERO(ret);
    ret = dbg_svr_log_open(ctl); ERR_NZERO(ret);
    ret = dbg_svr_log_write_header(ctl); ERR_NZERO(ret);

    if (cfg->dbgmsg_svr_enable == true)
    {
        ctl->dbgmsg_svr = &(ctl->_dbgmsg_svr);
        cfg->dbgmsg_svr = &(cfg->_dbgmsg_svr);
        ret = dbgmsg_svr_init(ctl->dbgmsg_svr, cfg->dbgmsg_svr); ERR_NZERO(ret);
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    dbg_svr_release(ctl);
    GOEXIT;
}

/*____________________________________________________________________________*/
/* MAIN */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
static int
main_loop (void)
{   ENTR();
    int                                 ret = -1;


    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static void
main_cfg_show (void)
{   ENTR();

    LOG("gverbose=%d", gverbose);

    dbg_svr_cfg_show(gcfg->dbg_svr);

LEXIT;
    return;
}

static int
main_cfg (
    const int                           argc,
    char * const                        argv[])
{   ENTR();
    int                                 ret = -1;
    int                                 optchar;
    int                                 optindex;

    ERR_NPOS(argc); ERR_NULL(argv);

    if (argc < 2)
    {
        GOERROR;
    }

    LOG("argc=%d", argc);
    for (int i = 1; i < argc; i++)
    {
        LOG("argv[%d]=\"%s\"", i, argv[i]);
    }

    while (1)
    {
        optchar = getopt_long_only(argc, argv, "", optlist, &optindex);
        if (optchar == -1) { break; }

        switch (optchar)
        {
        case GOPTC_HELP:
            main_help(argv[0]);
            main_exit(0);
            break;
        case GOPTC_VERBOSE:
            ERR_OPTARG_INVALID();
            gverbose = strtol(optarg, NULL, 0);
            break;
        default:
            break;
        }
    }

    gcfg->dbg_svr = &(gcfg->_dbg_svr);
    ret = dbg_svr_cfg(gcfg->dbg_svr, argc, argv); ERR_NZERO(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static void
main_help (
    char * const                        name)
{   ENTR();

    ERR_NULL(name);

    printf("%s options:\r\n", name);
    printf("-%c/--%s\t%s\r\n", GOPTC_HELP, GOPTL_HELP, GOPTS_HELP);
    printf("-%c/--%s\t%s <%d,%d>\r\n", GOPTC_VERBOSE, GOPTL_VERBOSE, GOPTS_VERBOSE, DBG_VERBOSE_MIN, DBG_VERBOSE_MAX);

    dbg_svr_help();

LEXIT;
    return;
LERROR;
    GOEXIT;
}

static void
main_exit (
    int                                 ret)
{   ENTR();

    LOG("ret=%d", ret);

    if (gctl->dbg_svr != NULL)
    {
        dbg_svr_release(gctl->dbg_svr);
    }

    MEMZ(gcfg, sizeof(GLOBAL_CFG));
    MEMZ(gctl, sizeof(GLOBAL_CTL));

LEXIT;
    exit(ret);
}
static int
main_init (void)
{   ENTR();
    int                                 ret = -1;

    gctl->dbg_svr = &(gctl->_dbg_svr);
    ret = dbg_svr_init(gctl->dbg_svr, gcfg->dbg_svr); ERR_NZERO(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

int
main (
    const int                           argc,
    char * const                        argv[])
{   ENTR();
    int                                 ret = -1;

    ERR_NPOS(argc); ERR_NULL(argv);

    MEMZ(gcfg, sizeof(GLOBAL_CFG));
    MEMZ(gctl, sizeof(GLOBAL_CTL));

    if (argc < 2)
    {
        main_help(argv[0]);
        main_exit(0);
    }

    ret = main_cfg(argc, argv);
    main_cfg_show();
    ret = main_init(); ERR_NZERO(ret);
    ret = main_loop(); ERR_NZERO(ret);

    ret = 0;
LEXIT;
    main_exit(ret);
    return(ret);
LERROR;
    GOEXIT;
}


// /*____________________________________________________________________________*/
// /* DBGMSG_SERVER*/
// /*------------------------------------*/
// static int dbgmsg_svr_reset (DBGMSG_SERVER_CTL * const ctl);
// static void dbgmsg_svr_release (DBGMSG_SERVER_CTL * const ctl);
// static int dbgmsg_svr_init (DBGMSG_SERVER_CTL * const ctl, DBGMSG_SERVER_CFG * const cfg);
// /*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/


// /*____________________________________________________________________________*/
// /* DBGMSG */
// /*------------------------------------*/
// static int
// dbgmsg_server_printf (
//         DBGMSG_SERVER_CTL * const       ctl)
// {       ENTR();
//         int                             ret = -1;
//         ERR_NULL(ctl);

//         ret = 0;
// exit:   EXIT();
//         return(ret);
// error:  FAIL();
//         goto exit;
// }       /* dbgmsg_server_printf */
// /*------------------------------------*/
// static int
// dbgmsg_server_send (
//         DBGMSG_SERVER_CTL * const       ctl)
// {       ENTR();
//         int                             ret = -1;
//         ERR_NULL(ctl);

//         ret = 0;
// exit:   EXIT();
//         return(ret);
// error:  FAIL();
//         goto exit;
// }       /* dbgmsg_server_send */
// /*------------------------------------*/
// static int
// dbgmsg_server_recv (
//         DBGMSG_SERVER_CTL * const       ctl)
// {       ENTR();
//         int                             ret = -1;
//         ERR_NULL(ctl);

//         ret = 0;
// exit:   EXIT();
//         return(ret);
// error:  FAIL();
//         goto exit;
// }       /* dbgmsg_server_recv */
// /*------------------------------------*/
// static int
// dbgmsg_server_reset (
//         DBGMSG_SERVER_CTL * const       ctl)
// {       ENTR();
//         int                             ret = -1;
//         DBGMSG_SERVER_CFG *                    cfg = NULL;
//         ERR_NULL(ctl);

//         if (ctl->cfg != NULL)
//         {
//                 MALLOCZ(cfg, DBGMSG_SERVER_CFG); ERR_NULL(cfg);
//                 memcpy(cfg, ctl->cfg, sizeof(DBGMSG_SERVER_CFG));
//                 INF("original cfg backup");
//         }

//         dbgmsg_server_release(ctl);

//         ret = dbgmsg_server_init(ctl, cfg); ERR_NZERO(ret);

//         ret = 0;
// exit:   EXIT();
//         free(cfg);
//         return(ret);
// error:  FAIL();
//         ERR_ERRNO();
//         goto exit;
// }       /* dbgmsg_server_reset */
// /*------------------------------------*/
// static void
// dbgmsg_server_release (
//         DBGMSG_SERVER_CTL * const       ctl)
// {       ENTR();
//         ERR_NULL(ctl);

//         // if (ctl->msg_qid >= 0)
//         // {
//         //         if (msgctl(ctl->msg_qid, IPC_RMID, NULL) < 0)
//         //         {
//         //                 ERR_ERRNO();
//         //         }
//         // }
//         MEMZ(ctl, sizeof(DBGMSG_SERVER_CTL));

// exit:   EXIT();
//         return;
// error:  FAIL();
//         goto exit;
// }       /* dbgmsg_server_release */
// /*------------------------------------*/
// static int
// dbgmsg_server_init (
//         DBGMSG_SERVER_CTL * const       ctl,
//         DBGMSG_SERVER_CFG * const       cfg)
// {       ENTR();
//         int                             ret = -1;
//         ERR_NULL(ctl);

//         // if (ctl->cfg != NULL)
//         // {
//         //         dbgmsg_server_release(ctl);
//         // }
//         // ctl->cfg = &(ctl->CFG);

//         // if (cfg == NULL)
//         // {       /* use default cfg */
//         //         snprintf(ctl->cfg->key_path, DBGMSG_SERVER_KEY_PATH_LEN, DBGMSG_SERVER_KEY_PATH_DFT);
//         //         INF("key_path=\"%s\"", ctl->cfg->key_path);
//         //         ctl->cfg->key_id = DBGMSG_SERVER_KEY_ID_DFT;
//         //         INF("key_id=0x%X", ctl->cfg->key_id);
//         //         ctl->cfg->log_size = DBGMSG_SERVER_LOG_SIZE_DFT;
//         //         INF("log_size=0x%X", ctl->cfg->log_size);
//         //         ctl->cfg->log_count = DBGMSG_SERVER_LOG_COUNT_DFT;
//         //         INF("log_count=0x%X", ctl->cfg->log_count);
//         // }
//         // else
//         // {
//         //         if (cfg->key != 0)
//         //         {       /* use given key */
//         //                 ctl->cfg->key = cfg->key;
//         //                 INF("key=0x%X", ctl->cfg->key);
//         //         }
//         //         else
//         //         {
//         //                 if (strnlen(cfg->key_path, DBGMSG_SERVER_KEY_PATH_LEN) < 1)
//         //                 {       /* use default key_path */
//         //                         snprintf(ctl->cfg->key_path, DBGMSG_SERVER_KEY_PATH_LEN, DBGMSG_SERVER_KEY_PATH_DFT);
//         //                 }
//         //                 else
//         //                 {       /* use given key_path */
//         //                         snprintf(ctl->cfg->key_path, DBGMSG_SERVER_KEY_PATH_LEN, "%s", cfg->key_path);
//         //                 }
//         //                 INF("key_path=\"%s\"", ctl->cfg->key_path);

//         //                 if ((cfg->key_id < 0x01) || (cfg->key_id > 0xFF))
//         //                 {       /* use default key_id */
//         //                         ctl->cfg->key_id = DBGMSG_SERVER_KEY_ID_DFT;
//         //                 }
//         //                 else
//         //                 {       /* use given key_id */
//         //                         ctl->cfg->key_id = cfg->key_id;
//         //                 }
//         //                 INF("key_id=%d", ctl->cfg->key_id);
//         //         }

//         //         if (cfg->log_size == 0)
//         //         {
//         //                 ctl->cfg->log_size = DBGMSG_SERVER_LOG_SIZE_DFT;
//         //         }
//         //         else
//         //         {
//         //                 ctl->cfg->log_size = cfg->log_size;
//         //         }
//         //         INF("log_size=0x%X", ctl->cfg->log_size);

//         //         if (cfg->log_count == 0)
//         //         {
//         //                 ctl->cfg->log_count = DBGMSG_SERVER_LOG_COUNT_DFT;
//         //         }
//         //         else
//         //         {
//         //                 ctl->cfg->log_count = cfg->log_count;
//         //         }
//         //         INF("log_count=0x%X", ctl->cfg->log_count);
//         // }

//         // if (ctl->cfg->key == 0)
//         // {       /* generate key from key_path and key_id */
//         //         ctl->cfg->key = ftok(ctl->cfg->key_path, ctl->cfg->key_id); ERR_NEG(ctl->cfg->key);
//         //         INF("key=0x%X", ctl->cfg->key);
//         // }
//         // ctl->msg_key = ctl->cfg->key;
//         // LOG("msg_key=0x%X", ctl->msg_key);

//         // ctl->msg_qid = msgget(ctl->msg_key, (IPC_CREAT | IPC_EXCL));
//         // if ((ctl->msg_qid < 0) && (errno == EEXIST))
//         // {       /* remove old msgq and generate new msgq */
//         //         ctl->msg_qid = msgget(ctl->msg_key, 0); ERR_NEG(ctl->msg_qid);
//         //         ret = msgctl(ctl->msg_qid, IPC_RMID, NULL); ERR_NEG(ret);
//         //         INF("msg_qid=0x%X removed", ctl->msg_qid);
//         //         ctl->msg_qid = msgget(ctl->msg_key, IPC_CREAT); ERR_NEG(ctl->msg_qid);
//         // }
//         // ERR_NEG(ctl->msg_qid);
//         // LOG("msg_qid=0x%X", ctl->msg_qid);

//         ret = 0;
// exit:   EXIT();
//         return(ret);
// error:  FAIL();
//         ERR_ERRNO();
//         goto exit;
// }       /* dbgmsg_server_init */
// /*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

// /*____________________________________________________________________________*/
// /* DBG_SERVER */
// /*------------------------------------*/
// static int
// dbg_server_loop (void)
// {       ENTR();
//         int                             ret = -1;

//         ret = 0;
// exit:   EXIT();
//         return(ret);
// error:  FAIL();
//         goto exit;
// }       /* dbg_server_loop */
// /*------------------------------------*/
// static int
// dbg_server_log_create (void)
// {       ENTR();
//         int                             ret = -1;

//         gctl->t_now = time(NULL);
//         gctl->local = localtime(&(gctl->t_now));
//         snprintf(gctl->log_name, DBG_SERVER_LOG_NAME_LEN, "%04d-%04d%02d%02d-%02d%02d%02d-dbg.txt",
//                  gctl->idx_create, (gctl->local->tm_year + 1900), (gctl->local->tm_mon + 1), gctl->local->tm_mday,
//                  gctl->local->tm_hour, gctl->local->tm_min, gctl->local->tm_sec);
//         snprintf(gctl->log_path_name, DBG_SERVER_LOG_PATH_NAME_LEN, "%s/%s", gcfg->log_path, gctl->log_name);
//         INF("log_path_name=\"%s\"", gctl->log_path_name);

//         if (gctl->log_fp != NULL)
//         {
//                 fclose(gctl->log_fp);
//                 gctl->log_fp = NULL;
//         }
//         gctl->log_fp = fopen(gctl->log_path_name, "w+"); ERR_NULL(gctl->log_fp);
//         gctl->idx_create++;
//         ret = dbg_server_idx_update(); ERR_NZERO(ret);

//         ret = 0;
// exit:   EXIT();
//         return(ret);
// error:  FAIL();
//         goto exit;
// }       /* dbg_server_log_create */
// /*------------------------------------*/
// static int
// dbg_server_idx_update (void)
// {       ENTR();
//         int                             ret = -1;
//         int                             len;

//         len = strnlen(gctl->idx_path_name, DBG_SERVER_IDX_PATH_NAME_LEN);
//         if (len < 1)
//         {
//                 snprintf(gctl->idx_path_name, DBG_SERVER_IDX_PATH_NAME_LEN, "%s/%s", gcfg->log_path, DBG_SERVER_IDX_NAME_DFT);
//         }

//         gctl->idx_fp = fopen(gctl->idx_path_name, "w+"); ERR_NULL(gctl->idx_fp);
//         fprintf(gctl->idx_fp, "[create=%04d][delete=%04d]\r\n", gctl->idx_create, gctl->idx_delete);
//         INF("[create=%04d][delete=%04d]", gctl->idx_create, gctl->idx_delete);
//         ret = fclose(gctl->idx_fp); ERR_NZERO(ret);

//         ret = 0;
// exit:   EXIT();
//         return(ret);
// error:  FAIL();
//         ERR_ERRNO();
//         goto exit;
// }       /* dbg_server_idx_update */
// /*------------------------------------*/
// static int
// dbg_server_idx_fetch (void)
// {       ENTR();
//         int                             ret = -1;
//         int                             len;

//         len = strnlen(gctl->idx_path_name, DBG_SERVER_IDX_PATH_NAME_LEN);
//         if (len < 1)
//         {
//                 snprintf(gctl->idx_path_name, DBG_SERVER_IDX_PATH_NAME_LEN, "%s/%s", gcfg->log_path, DBG_SERVER_IDX_NAME_DFT);
//         }

//         gctl->idx_fp = fopen(gctl->idx_path_name, "a+"); ERR_NULL(gctl->idx_fp);
//         rewind(gctl->idx_fp);
//         ret = fscanf(gctl->idx_fp, "[create=%04d][delete=%04d]", &(gctl->idx_create), &(gctl->idx_delete));
//         if (ret < 1)
//         {
//                 gctl->idx_create = DBG_SERVER_IDX_MIN;
//                 gctl->idx_delete = DBG_SERVER_IDX_MIN;
//                 rewind(gctl->idx_fp);
//                 fprintf(gctl->idx_fp, "[create=%04d][delete=%04d]\r\n", gctl->idx_create, gctl->idx_delete);
//         }
//         ret = fclose(gctl->idx_fp); ERR_NZERO(ret);

//         ret = 0;
// exit:   EXIT();
//         return(ret);
// error:  FAIL();
//         ERR_ERRNO();
//         goto exit;
// }       /* dbg_server_idx_fetch */
// /*------------------------------------*/
// static int
// dbg_server_mkdir (void)
// {       ENTR();
//         int                             ret = -1;
//         int                             i;
//         int                             len;
//         DIR *                           dir;
//         char                            strdir[DBG_SERVER_LOG_PATH_LEN] = {'0'};

//         dir = opendir(gcfg->log_path);
//         if (dir != NULL)
//         {
//                 ret = closedir(dir); ERR_NZERO(ret);
//                 goto exit;
//         }

//         len = strlen(gcfg->log_path);
//         for (i = 0; i < len; i++)
//         {
//                 if (((gcfg->log_path[i] == '/') && (i > 0)) || (i == (len - 1)))
//                 {
//                         snprintf(strdir, (i + 2), "%s", gcfg->log_path);
//                         INF("mkdir=\"%s\"", strdir);
//                         ret = mkdir(strdir, DEFFILEMODE);
//                         if ((ret < 0) && (errno != EEXIST))
//                         {
//                                 ERR_NZERO(ret);
//                         }
//                 }
//         }

//         dir = opendir(gcfg->log_path);
//         if (dir != NULL)
//         {
//                 ret = closedir(dir); ERR_NZERO(ret);
//                 goto exit;
//         }
//         else
//         {
//                 ret = -1;
//                 goto error;
//         }

//         ret = 0;
// exit:   EXIT();
//         return(ret);
// error:  FAIL();
//         ERR_ERRNO();
//         goto exit;
// }       /* dbg_server_mkdir */
// /*------------------------------------*/
// static int
// dbg_server_reset (void)
// {       ENTR();
//         int                             ret = -1;

//         ret = 0;
// exit:   EXIT();
//         return(ret);
// error:  FAIL();
//         goto exit;
// }       /* dbg_server_reset */
// /*------------------------------------*/
// static void
// dbg_server_exit (
//         int                             ret)
// {       ENTR();

//         if (gctl->dbgmsg_server)
//         {
//                 dbgmsg_server_release(gctl->dbgmsg_server);
//         }

// exit:   EXIT();
//         exit(ret);
//         return;
// }       /* dbg_server_exit */
// /*------------------------------------*/


// #define OPTARG_INVALID(ARG)             (ARG==NULL)||(ARG[0]=='-')||(strlen(ARG)<1)
// #define ERR_OPTARG_INVALID() \
// DOWHILE(if(OPTARG_INVALID(optarg)){ERR("option --%s need argument", optlist[optindex].name); goto error;})
// /*------------------------------------*/

// /*------------------------------------*/
// static int
// dbg_server_init (
//         const int                       argc,
//         char * const                    argv[])
// {       ENTR();
//         int                             ret = -1;
//         int                             optchar;
//         int                             optindex;
//         ERR_NPOS(argc); ERR_NULL(argv);

//         ERR_NULL(gstat);
//         MEMZ(gstat, sizeof(GLOBAL_STAT));

//         ERR_NULL(gcfg);
//         MEMZ(gcfg, sizeof(GLOBAL_CFG));
//         gcfg->verbose = DBG_VERBOSE_DFT;
//         gcfg->dbgmsg_server = &(gcfg->DBGMSG_SERVER);

//         ERR_NULL(gctl);
//         MEMZ(gctl, sizeof(GLOBAL_CTL));

//         gstat->pid  = getpid();
//         gstat->ppid = getppid();
//         LOG("pid=%d ppid=%d argc=%d argv=%p", gstat->pid, gstat->ppid, argc, argv);
//         for (int i = 0; i < argc; i++)
//         {
//                 LOG("argv[%d]=\"%s\"", i, argv[i]);
//         }

//         while (1)
//         {
//                 optchar = getopt_long_only(argc, argv, "", optlist, &optindex);
//                 if (optchar == -1) { break; }

//                 switch (optchar)
//                 {
//                 case OPTC_HELP:
//                         dbg_server_help(argv[0]);
//                         break;
//                 case OPTC_VERBOSE:
//                         ERR_OPTARG_INVALID();
//                         gcfg->verbose = strtol(optarg, NULL, 0);
//                         INF("verbose=%d", gcfg->verbose);
//                         break;
//                 case OPTC_LOG_PATH:
//                         ERR_OPTARG_INVALID();
//                         snprintf(gcfg->log_path, DBG_SERVER_LOG_PATH_LEN, "%s", optarg);
//                         INF("log_path=\"%s\"", gcfg->log_path);
//                         break;
//                 case OPTC_LOG_SIZE:
//                         ERR_OPTARG_INVALID();
//                         gcfg->log_size = strtol(optarg, NULL, 0);
//                         ERR_RANGE(gcfg->log_size, DBG_SERVER_LOG_SIZE_MIN, DBG_SERVER_LOG_SIZE_MAX);
//                         INF("log_size=%dMB", gcfg->log_size);
//                         break;
//                 case OPTC_LOG_COUNT:
//                         ERR_OPTARG_INVALID();
//                         gcfg->log_count = strtol(optarg, NULL, 0);
//                         ERR_RANGE(gcfg->log_count, DBG_SERVER_LOG_COUNT_MIN, DBG_SERVER_LOG_COUNT_MAX);
//                         INF("log_count=%d", gcfg->log_count);
//                         break;
//                 case OPTC_DBGMSG_ENABLE:
//                         gcfg->dbgmsg_server_enable = true;
//                         INF("dbgmsg_server_enable=%s", STRBOOL(gcfg->dbgmsg_server_enable));
//                         break;
//                 case OPTC_DBGMSG_KEY:
//                         ERR_OPTARG_INVALID();
//                         gcfg->dbgmsg_server->key = strtol(optarg, NULL, 0);
//                         INF("key=0x%X", gcfg->dbgmsg_server->key);
//                         break;
//                 case 0:
//                         if (strncmp(optlist[optindex].name, OPTL_DBGMSG_KEY_PATH, strlen(OPTL_DBGMSG_KEY_PATH)) == 0)
//                         {
//                                 ERR_OPTARG_INVALID();
//                                 snprintf(gcfg->dbgmsg_server->key_path, DBGMSG_SERVER_KEY_PATH_LEN, "%s", optarg);
//                                 INF("key_path=\"%s\"", gcfg->dbgmsg_server->key_path);
//                         }
//                         else if (strncmp(optlist[optindex].name, OPTL_DBGMSG_KEY_ID, strlen(OPTL_DBGMSG_KEY_ID)) == 0)
//                         {
//                                 ERR_OPTARG_INVALID();
//                                 gcfg->dbgmsg_server->key_id = strtol(optarg, NULL, 0);
//                                 ERR_RANGE(gcfg->dbgmsg_server->key_id, 0x01, 0xFF);
//                                 INF("key_id=%d", gcfg->dbgmsg_server->key_id);
//                         }
//                         break;
//                 default:
//                         break;
//                 }
//         }

//         LOG("verbose=%d", gcfg->verbose);

//         if (strnlen(gcfg->log_path, DBG_SERVER_LOG_PATH_LEN) < 1)
//         {       /* default log_path */
//                 snprintf(gcfg->log_path, DBG_SERVER_LOG_PATH_LEN, "%s", DBG_SERVER_LOG_PATH_DFT);
//         }
//         LOG("log_path=%s", gcfg->log_path);

//         if (gcfg->log_size < 1)
//         {       /* default log_size */
//                 gcfg->log_size = DBG_SERVER_LOG_SIZE_DFT;
//         }
//         REGULATE_RANGE(gcfg->log_size, DBG_SERVER_LOG_SIZE_MIN, DBG_SERVER_LOG_SIZE_MAX);
//         LOG("log_size=%dMB", gcfg->log_size);

//         if (gcfg->log_count < 1)
//         {       /* default log_count */
//                 gcfg->log_count = DBG_SERVER_LOG_COUNT_DFT;
//         }
//         REGULATE_RANGE(gcfg->log_count, DBG_SERVER_LOG_COUNT_MIN, DBG_SERVER_LOG_COUNT_MAX);
//         LOG("log_count=%d", gcfg->log_count);

//         ret = dbg_server_mkdir(); ERR_NZERO(ret);
//         ret = dbg_server_idx_fetch(); ERR_NZERO(ret);
//         ret = dbg_server_log_create(); ERR_NZERO(ret);



//         if (gcfg->dbgmsg_server_enable)
//         {
//                 LOG("dbgmsg_server_enable=%s", STRBOOL(gcfg->dbgmsg_server_enable));
//                 gctl->dbgmsg_server = &(gctl->DBGMSG_SERVER);
//                 ret = dbgmsg_server_init(gctl->dbgmsg_server, gcfg->dbgmsg_server); ERR_NZERO(ret);
//         }

//         ret = 0;
// exit:   EXIT();
//         return(ret);
// error:  FAIL();
//         ERR_ERRNO();
//         goto exit;
// }       /* dbg_server_init */
// /*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/

