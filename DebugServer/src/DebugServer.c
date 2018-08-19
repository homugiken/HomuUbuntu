
/*____________________________________________________________________________*/
/* INCLUDE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#include "DebugServer.h"

/*____________________________________________________________________________*/
/* GLOBAL */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
typedef struct GLOBAL_CFG {
    DBG_SVR_CFG                         _dbg_svr, * dbg_svr;
} GLOBAL_CFG;

typedef struct GLOBAL_CTL {
    DBG_SVR_CTL                         _dbg_svr, * dbg_svr;
} GLOBAL_CTL;

static GLOBAL_CFG                       _gcfg, * const gcfg = &_gcfg;
static GLOBAL_CTL                       _gctl, * const gctl = &_gctl;
static uint32_t                         gverbose = DBG_SVR_VERBOSE_DFT;

/*____________________________________________________________________________*/
/* OPTION */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
extern int                              optind;
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
/* DBGSTD */
/*------------------------------------*/
static void
dbgstd_printf (
    const char *                        fmt,
    ...)
{
    static DBGSTD_CTL                   _ctl, * const ctl = &_ctl;


    ctl->time_str[0] = '\0';
    ctl->time_now = time(NULL);
    ctl->time_local = localtime(&(ctl->time_now));
    snprintf(ctl->time_str, DBGSTD_TIME_STR_LEN, "%04d%02d%02d-%02d%02d%02d",
             (ctl->LOCAL_YEAR), (ctl->LOCAL_MON), ctl->LOCAL_DAY,
             ctl->LOCAL_HOUR, ctl->LOCAL_MIN, ctl->LOCAL_SEC);

    ctl->text[0] = '\0';
    va_start(ctl->vargs, fmt);
    vsnprintf(ctl->text, DBGSTD_TEXT_LEN, fmt, ctl->vargs);
    va_end(ctl->vargs);

    fprintf(stdout, "%s|%s\r\n", ctl->time_str, ctl->text);

    return;
}

/*____________________________________________________________________________*/
/* DBGMSG */
/*------------------------------------*/
static void
dbgmsg_printf (
    DBGMSG_CTL * const                  ctl,
    const char *                        fmt,
    ...)
{
    int                                 ret = -1;
    DBGMSG_MSG                          _msg, * const msg = &_msg;

    if ((ctl == NULL) || (ctl->qid < 0) || (ctl->ready != true))
    {
        return;
    }

    msg->type = DBGMSG_TYPE_DEBUG;
    msg->src_time = time(NULL);
    msg->src_pid = ctl->pid;

    msg->src_name[0] = '\0';
    snprintf(msg->src_name, DBGMSG_SRC_NAME_LEN, "%s", ctl->src_name);

    msg->text[0] = '\0';
    va_start(ctl->vargs, fmt);
    vsnprintf(msg->text, DBGSTD_TEXT_LEN, fmt, ctl->vargs);
    va_end(ctl->vargs);

    msgsnd(ctl->qid, msg, (sizeof(time_t) + sizeof(pid_t) + DBGMSG_SRC_NAME_LEN + strlen(msg->text) + 1), IPC_NOWAIT);
}


static void dbgmsg_cfg_show (DBGMSG_CFG * const cfg);
static int dbgmsg_cfg (DBGMSG_CFG * const cfg, const int argc, char * const argv[]);
static void dbgmsg_help (void);
static void dbgmsg_release (DBGMSG_CTL * const ctl);
static int dbgmsg_init (DBGMSG_CTL * const ctl, DBGMSG_CFG * const cfg);






/*____________________________________________________________________________*/
/* DBGMSG_SVR */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
static int
dbgmsg_svr_fprintf (
    DBGMSG_SVR_CTL * const              ctl,
    FILE * const                        fp)
{   ENTR();
    int                                 ret = -1;
    DBGMSG_MSG *                    msg = NULL;

    ERR_NULL(ctl); ERR_NULL(fp);
    ERR_FALSE(ctl->ready);

    if (ctl->msg_count < 1)
    {
        ret = 0;
        GOEXIT;
    }

    for (uint32_t i = 0; i < ctl->msg_count; i++)
    {
        msg = &(ctl->msg_buf[i]);
        ctl->time_msg = msg->src_time;
        ctl->time_local = localtime(&(ctl->time_msg));

        ret = fprintf(fp, DBGMSG_SVR_MSG_FMT,
                      ctl->LOCAL_YEAR, ctl->LOCAL_MON, ctl->LOCAL_DAY,
                      ctl->LOCAL_HOUR, ctl->LOCAL_MIN, ctl->LOCAL_SEC,
                      msg->src_pid, msg->src_name, msg->text); WRN_NPOS(ret);

        ret = fprintf(stdout, DBGMSG_SVR_MSG_FMT,
                      ctl->LOCAL_YEAR, ctl->LOCAL_MON, ctl->LOCAL_DAY,
                      ctl->LOCAL_HOUR, ctl->LOCAL_MIN, ctl->LOCAL_SEC,
                      msg->src_pid, msg->src_name, msg->text); WRN_NPOS(ret);
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static int
dbgmsg_svr_rcv (
    DBGMSG_SVR_CTL * const              ctl)
{   ENTR();
    int                                 ret = -1;

    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready);
    ERR_NEG(ctl->qid);

    if (ctl->msg_count > 0)
    {
        MEMZ(ctl->msg_buf, (ctl->msg_count * sizeof(DBGMSG_SVR_MSG)));
    }
    ctl->msg_count = 0;

    for (int i = 0; i < DBGMSG_SVR_MSG_BUF_SIZE; i++)
    {
        ret = msgrcv(ctl->qid,
                     &(ctl->msg_buf[i]),
                     (sizeof(DBGMSG_SVR_MSG) - sizeof(long)),
                     0,
                     (IPC_NOWAIT | MSG_NOERROR)); WRN_NPOS(ret);
        if (ret <= 0)
        {
            break;
        }
        ctl->msg_count++;
    }
    if (ctl->msg_count > 0)
    {
        INF("msg_count=%u", ctl->msg_count);
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static void
dbgmsg_svr_cfg_show (
    DBGMSG_SVR_CFG * const              cfg)
{   ENTR();

    ERR_NULL(cfg);

    LOG("key=0x%08X", cfg->key);
    LOG("key_path=\"%s\"", cfg->key_path);
    LOG("key_id=0x%02X", cfg->key_id);

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
    optind = 0;
    while (1)
    {
        optchar = getopt_long_only(argc, argv, "", optlist, &optindex);
        if (optchar == -1) { break; }

        switch (optchar)
        {
        case DBGMSG_SVR_OPTC_KEY:
            ERR_OPTARG_INVALID();
            cfg->key = strtoul(optarg, NULL, 0);
            INF("key=0x%08X", cfg->key);
            break;
        case 0:
            if (strncmp(optlist[optindex].name, DBGMSG_SVR_OPTL_KEY_PATH, strlen(DBGMSG_SVR_OPTL_KEY_PATH)) == 0)
            {
                ERR_OPTARG_INVALID();
                snprintf(cfg->key_path, DBGMSG_SVR_KEY_PATH_LEN, "%s", optarg);
                INF("key_path=\"%s\"", cfg->key_path);
            }
            else if (strncmp(optlist[optindex].name, DBGMSG_SVR_OPTL_KEY_ID, strlen(DBGMSG_SVR_OPTL_KEY_ID)) == 0)
            {
                ERR_OPTARG_INVALID();
                cfg->key_id = strtoul(optarg, NULL, 0);
                INF("key_id=0x%02X", cfg->key_id);
            }
            break;
        default:
                break;
        }
    }

    if (strlen(cfg->key_path) < 2)
    {
        snprintf(cfg->key_path, DBGMSG_SVR_KEY_PATH_LEN, "%s", DBGMSG_SVR_KEY_PATH_DFT);
        INF("key_path=\"%s\"", cfg->key_path);
    }

    if ((cfg->key_id < DBGMSG_SVR_KEY_ID_MIN) || (cfg->key_id > DBGMSG_SVR_KEY_ID_MAX))
    {
        cfg->key_id = DBGMSG_SVR_KEY_ID_DFT;
        INF("key_id=0x%02X", cfg->key_id);
    }
    ERR_RANGE(cfg->key_id, DBGMSG_SVR_KEY_ID_MIN, DBGMSG_SVR_KEY_ID_MAX);

    if (cfg->key < 1)
    {
        cfg->key = ftok(cfg->key_path, cfg->key_id); ERR_NEG(cfg->key);
        INF("key=0x%X", cfg->key);
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
    int                                 ret = -1;

    ERR_NULL(ctl);

    if (ctl->qid >=0)
    {
        ret = msgctl(ctl->qid, IPC_RMID, NULL); WRN_NZERO(ret);
        ctl->ready = false;
    }

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

    if (ctl->cfg != NULL)
    {
        dbgmsg_svr_release(ctl);
    }
    ctl->cfg = cfg;

    ERR_NPOS(ctl->cfg->key);
    ctl->qid = msgget(ctl->cfg->key, (IPC_CREAT | IPC_EXCL));
    if ((ctl->qid < 0) && (errno == EEXIST))
    {
        ctl->qid = msgget(ctl->cfg->key, 0); WRN_NEG(ctl->qid);
    }
    ERR_NEG(ctl->qid);
    LOG("qid=%d", ctl->qid);

    ctl->ready = true;
    LOG("ready=%s", STRBOOL(ctl->ready));

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
dbg_svr_loop_job (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    ret = dbg_svr_rcv(ctl); WRN_NZERO(ret);

    ret = dbg_svr_log_fprintf(ctl); WRN_NZERO(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static int
dbg_svr_rcv (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    if (ctl->cfg->dbgmsg_svr_enable == true)
    {
        WRN_NULL(ctl->dbgmsg_svr);
        if (ctl->dbgmsg_svr != NULL)
        {
            WRN_FALSE(ctl->dbgmsg_svr->ready);
            if (ctl->dbgmsg_svr->ready == true)
            {
                ret = dbgmsg_svr_rcv(ctl->dbgmsg_svr); WRN_NZERO(ret);
            }
        }
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static int
dbg_svr_log_shift (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    if (ctl->dir == NULL)
    {
        ret = dbg_svr_mkdir(ctl); ERR_NZERO(ret);
    }
    if (ctl->log_fp != NULL)
    {
        dbg_svr_log_close(ctl);
    }

    ret = dbg_svr_log_name_new(ctl); ERR_NZERO(ret);
    ret = dbg_svr_log_open(ctl); ERR_NZERO(ret);
    ret = dbg_svr_idx_create_increase(ctl); ERR_NZERO(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static int
dbg_svr_log_fprintf (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    if (ctl->log_fp == NULL)
    {
        ret = dbg_svr_log_open(ctl); ERR_NZERO(ret);
    }

    if (ctl->cfg->dbgmsg_svr_enable)
    {
        WRN_NULL(ctl->dbgmsg_svr);
        if (ctl->dbgmsg_svr != NULL)
        {
            WRN_FALSE(ctl->dbgmsg_svr->ready);
            if (ctl->dbgmsg_svr->ready == true)
            {
                ret = dbgmsg_svr_fprintf(ctl->dbgmsg_svr, ctl->log_fp); WRN_NZERO(ret);
            }
        }
    }

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
    ERR_NULL(ctl); ERR_NULL(ctl->log_fp);

    ctl->time_now = time(NULL);
    ctl->time_local = localtime(&(ctl->time_now));
    ret = fprintf(ctl->log_fp, "[TIME|END=%04d/%02d/%02d-%02d:%02d:%02d]\r\n",
                  ctl->LOCAL_YEAR, ctl->LOCAL_MON, ctl->LOCAL_DAY,
                  ctl->LOCAL_HOUR, ctl->LOCAL_MIN, ctl->LOCAL_SEC); WRN_NPOS(ret);

    ret = fflush(ctl->log_fp); WRN_NZERO(ret);

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
    ERR_NULL(ctl->cfg);

    if (ctl->log_fp == NULL)
    {
        ret = dbg_svr_log_open(ctl); ERR_NZERO(ret);
    }

    ret = fprintf(ctl->log_fp, "[LOG|PATH=%s|NAME=%s]\r\n", ctl->cfg->dir_path, ctl->log_name); WRN_NPOS(ret);
    ret = fprintf(ctl->log_fp, DBG_SVR_IDX_FMT"\r\n", ctl->idx_create, ctl->idx_delete); WRN_NPOS(ret);

    ctl->time_now = time(NULL);
    ctl->time_local = localtime(&(ctl->time_now));
    ret = fprintf(ctl->log_fp, "[TIME|START=%04d/%02d/%02d-%02d:%02d:%02d]\r\n",
                  ctl->LOCAL_YEAR, ctl->LOCAL_MON, ctl->LOCAL_DAY,
                  ctl->LOCAL_HOUR, ctl->LOCAL_MIN, ctl->LOCAL_SEC); WRN_NPOS(ret);

    ret = fflush(ctl->log_fp); WRN_NZERO(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static int
dbg_svr_log_name_new (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    MEMZ(ctl->log_name, DBG_SVR_LOG_NAME_LEN);
    ctl->time_now = time(NULL);
    ctl->time_local = localtime(&(ctl->time_now));
    ret = snprintf(ctl->log_name, DBG_SVR_LOG_NAME_LEN, DBG_SVR_LOG_NAME_FMT,
                   ctl->idx_create, ctl->LOCAL_YEAR, ctl->LOCAL_MON, ctl->LOCAL_DAY,
                   ctl->LOCAL_HOUR, ctl->LOCAL_MIN, ctl->LOCAL_SEC); ERR_NPOS(ret);
    INF("log_name=\"%s\"", ctl->log_name);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static void
dbg_svr_log_flush (
    DBG_SVR_CTL *                       const ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_NULL(ctl->log_fp);

    ret = fflush(ctl->log_fp); WRN_NZERO(ret);

LEXIT;
    return;
LERROR;
    GOEXIT;
}

static void
dbg_svr_log_close (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    if (ctl->log_fp != NULL)
    {
        ret = dbg_svr_log_write_trailer(ctl); WRN_NZERO(ret);

        ret = fflush(ctl->log_fp); WRN_NZERO(ret);
        fclose(ctl->log_fp);
        ctl->log_fp = NULL;
        LOG("close=\"%s\"", ctl->log_path_name);
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
    ERR_NPOS(strlen(ctl->cfg->dir_path));
    ERR_NPOS(strlen(ctl->log_name));

    if (ctl->dir == NULL)
    {
        ret = dbg_svr_mkdir(ctl); ERR_NZERO(ret);
    }
    if (ctl->log_fp != NULL)
    {
        dbg_svr_log_close(ctl);
    }
    if (strlen(ctl->log_name) < strlen("dbg.txt"))
    {
        ret = dbg_svr_log_name_new(ctl); ERR_NZERO(ret);
    }

    snprintf(ctl->log_path_name, DBG_SVR_LOG_PATH_NAME_LEN, "%s/%s", ctl->cfg->dir_path, ctl->log_name);
    ctl->log_fp = fopen(ctl->log_path_name, "w+"); ERR_NULL(ctl->log_fp);
    LOG("open=\"%s\"", ctl->log_path_name);

    ret = dbg_svr_log_write_header(ctl); ERR_NZERO(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_idx_delete_increase (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    if (ctl->idx_delete >= DBG_SVR_IDX_MAX)
    {
        ctl->idx_delete = 0;
    }
    else
    {
        ctl->idx_delete++;
    }
    INF("idx_delete=%04u", ctl->idx_delete);

    ret = dbg_svr_idx_write(ctl); ERR_NZERO(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static int
dbg_svr_idx_create_increase (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    if (ctl->idx_create >= DBG_SVR_IDX_MAX)
    {
        ctl->idx_create = 0;
    }
    else
    {
        ctl->idx_create++;
    }
    INF("idx_create=%04u", ctl->idx_create);

    ret = dbg_svr_idx_write(ctl); ERR_NZERO(ret);

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
    ERR_NULL(ctl->cfg);

    ret = dbg_svr_idx_open_write(ctl); ERR_NZERO(ret);

    ret = fprintf(ctl->idx_fp, DBG_SVR_IDX_FMT"\r\n", ctl->idx_create, ctl->idx_delete); ERR_NPOS(ret);
    INF("idx_create=%04u idx_delete=%04u", ctl->idx_create, ctl->idx_delete);

    dbg_svr_idx_close(ctl);

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
    ERR_NULL(ctl->cfg);

    ret = dbg_svr_idx_open_read(ctl); WRN_NZERO(ret);
    if (ret != 0)
    {
        ctl->idx_create = DBG_SVR_IDX_MIN;
        ctl->idx_delete = DBG_SVR_IDX_MIN;
        ret = dbg_svr_idx_write(ctl); ERR_NZERO(ret);
    }
    ret = dbg_svr_idx_open_read(ctl); ERR_NZERO(ret);

    ret = fscanf(ctl->idx_fp, DBG_SVR_IDX_FMT, &(ctl->idx_create), &(ctl->idx_delete)); WRN_NPOS(ret);
    if (ret < 1)
    {
        ctl->idx_create = DBG_SVR_IDX_MIN;
        ctl->idx_delete = DBG_SVR_IDX_MIN;
        ret = dbg_svr_idx_write(ctl); ERR_NZERO(ret);
    }
    else
    {
        INF("idx_create=%04u idx_delete=%04u", ctl->idx_create, ctl->idx_delete);
    }

    dbg_svr_idx_close(ctl);

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
    int                                 ret = -1;

    ERR_NULL(ctl);

    if (ctl->idx_fp != NULL)
    {

        ret = fflush(ctl->idx_fp); WRN_NZERO(ret);
        fclose(ctl->idx_fp);
        ctl->idx_fp = NULL;
    }

LEXIT;
    return;
LERROR;
    GOEXIT;
}

static int
dbg_svr_idx_open_write (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_NULL(ctl->cfg);

    if (ctl->idx_fp != NULL)
    {
        dbg_svr_idx_close(ctl);
    }

    snprintf(ctl->idx_path_name, DBG_SVR_IDX_PATH_NAME_LEN, "%s/"DBG_SVR_IDX_NAME_FMT, ctl->cfg->dir_path);
    ctl->idx_fp = fopen(ctl->idx_path_name, "w+"); ERR_NULL(ctl->idx_fp);
    rewind(ctl->idx_fp);
    INF("open=\"%s\"", ctl->idx_path_name);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static int
dbg_svr_idx_open_read (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_NULL(ctl->cfg);

    if (ctl->idx_fp != NULL)
    {
        dbg_svr_idx_close(ctl);
    }

    snprintf(ctl->idx_path_name, DBG_SVR_IDX_PATH_NAME_LEN, "%s/"DBG_SVR_IDX_NAME_FMT, ctl->cfg->dir_path);
    ctl->idx_fp = fopen(ctl->idx_path_name, "r"); ERR_NULL(ctl->idx_fp);
    rewind(ctl->idx_fp);
    INF("open=\"%s\"", ctl->idx_path_name);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_mkdir (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    int                                 len;
    char                                str_dir[DBG_SVR_DIR_PATH_LEN];
    ERR_NULL(ctl);
    ERR_NULL(ctl->cfg);

    ret = dbg_svr_opendir(ctl); WRN_NZERO(ret);
    if (ret == 0)
    {
        GOEXIT;
    }

    len = strlen(ctl->cfg->dir_path);
    for (int i = 0; i < len; i++)
    {
        if (((ctl->cfg->dir_path[i] == '/') && (i > 0)) || (i == (len - 1)))
        {
            MEMZ(str_dir, DBG_SVR_DIR_PATH_LEN);
            snprintf(str_dir, (i + 2), "%s", ctl->cfg->dir_path);
            ret = mkdir(str_dir, DEFFILEMODE);
            if ((ret < 0) && (errno != EEXIST))
            {
                ERR_NZERO(ret);
            }
            INF("mkdir=\"%s\"", str_dir);
        }
    }

    ret = dbg_svr_opendir(ctl); ERR_NZERO(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

static void
dbg_svr_closedir (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    if (ctl->dir != NULL)
    {
        ret = closedir(ctl->dir); WRN_NZERO(ret);
        ctl->dir = NULL;
    }

LEXIT;
    return;
LERROR;
    GOEXIT;
}

static int
dbg_svr_opendir (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    int                                 len;
    ERR_NULL(ctl);
    ERR_NULL(ctl->cfg);

    len = strlen(ctl->cfg->dir_path); ERR_NPOS(len);

    if (ctl->dir != NULL)
    {
        if (ctl->dir != opendir(ctl->cfg->dir_path))
        {
            dbg_svr_closedir(ctl);
        }
        else
        {
            ret = 0;
            GOEXIT;
        }
    }

    ctl->dir = opendir(ctl->cfg->dir_path); ERR_NULL(ctl->dir);
    INF("opendir=\"%s\"", ctl->cfg->dir_path);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static void
dbg_svr_cfg_show (
    DBG_SVR_CFG * const                 cfg)
{   ENTR();

    ERR_NULL(cfg);

    LOG("dir_path=\"%s\"", cfg->dir_path);
    LOG("log_size=%u", cfg->log_size);
    LOG("log_count=%u", cfg->log_count);
    LOG("dbgmsg_svr_enable=%s", STRBOOL(cfg->dbgmsg_svr_enable));

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
    optind = 0;
    while (1)
    {
        optchar = getopt_long_only(argc, argv, "", optlist, &optindex);
        if (optchar == -1) { break; }

        switch (optchar)
        {
        case DBG_SVR_OPTC_LOG_PATH:
            ERR_OPTARG_INVALID();
            snprintf(cfg->dir_path, DBG_SVR_DIR_PATH_LEN, "%s", optarg);
            INF("dir_path=\"%s\"", cfg->dir_path);
            break;
        case DBG_SVR_OPTC_LOG_SIZE:
            ERR_OPTARG_INVALID();
            cfg->log_size = strtoul(optarg, NULL, 0);
            INF("log_size=%u", cfg->log_size);
            break;
        case DBG_SVR_OPTC_LOG_COUNT:
            ERR_OPTARG_INVALID();
            cfg->log_count = strtoul(optarg, NULL, 0);
            INF("log_count=%u", cfg->log_count);
            break;
        case DBG_SVR_OPTC_DBGMSG_SVR:
            cfg->dbgmsg_svr_enable = true;
            INF("dbgmsg_svr_enable=%s", STRBOOL(cfg->dbgmsg_svr_enable));
            break;
        default:
                break;
        }
    }

    if (strnlen(cfg->dir_path, DBG_SVR_DIR_PATH_LEN) < 2)
    {
        snprintf(cfg->dir_path, DBG_SVR_DIR_PATH_LEN, "%s", DBG_SVR_DIR_PATH_DFT);
        INF("dir_path=\"%s\"", cfg->dir_path);
    }

    if ((cfg->log_size < DBG_SVR_LOG_SIZE_MIN) || (cfg->log_size > DBG_SVR_LOG_SIZE_MAX))
    {
        cfg->log_size = DBG_SVR_LOG_SIZE_DFT;
        INF("log_size=%u", cfg->log_size);
    }
    ERR_RANGE(cfg->log_size, DBG_SVR_LOG_SIZE_MIN, DBG_SVR_LOG_SIZE_MAX);

    if ((cfg->log_count < DBG_SVR_LOG_COUNT_MIN) || (cfg->log_count > DBG_SVR_LOG_COUNT_MAX))
    {
        cfg->log_count = DBG_SVR_LOG_COUNT_DFT;
        INF("log_count=%u", cfg->log_count);
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
    dbg_svr_closedir(ctl);

    if (ctl->dbgmsg_svr != NULL)
    {
        dbgmsg_svr_release(ctl->dbgmsg_svr);
    }

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

    ret = dbg_svr_mkdir(ctl); ERR_NZERO(ret);
    ret = dbg_svr_idx_read(ctl); ERR_NZERO(ret);
    ret = dbg_svr_log_shift(ctl); ERR_NZERO(ret);

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
/* DBG_CLNT */
/*------------------------------------*/




/*____________________________________________________________________________*/
/* MAIN */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
static int
main_loop (void)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(gctl); ERR_NULL(gcfg);

    while (1)
    {
        ERR_NULL(gctl->dbg_svr);
        WRN_FALSE(gctl->dbg_svr->ready);
        if (gctl->dbg_svr->ready == true)
        {
            ret = dbg_svr_loop_job(gctl->dbg_svr); WRN_NZERO(ret);
        }
    }

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

    optind = 0;
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
            gverbose = strtoul(optarg, NULL, 0);
            INF("gverbose=%u", gverbose);
            break;
        default:
            break;
        }
    }

    if ((gverbose < DBG_SVR_VERBOSE_MIN) || (gverbose > DBG_SVR_VERBOSE_MAX))
    {
        gverbose = DBG_SVR_VERBOSE_DFT;
        INF("gverbose=%u", gverbose);
    }
    ERR_RANGE(gverbose, DBG_SVR_VERBOSE_MIN, DBG_SVR_VERBOSE_MAX);

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

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
