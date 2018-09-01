/*____________________________________________________________________________*/
/* INCLUDE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#include "debug.h"

/*____________________________________________________________________________*/
/* DBGMSG */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
static void
dbgmsg_config_show (
    DBGMSG_CFG * const                  cfg)
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
/*························································*/
static int
dbgmsg_config (
    DBGMSG_CTL * const                  ctl,
    const int                           argc,
    char * const                        argv[])
{   ENTR();
    int                                 ret = -1;
    DBGMSG_CFG *                        cfg = NULL;
    char *                              pwd = NULL;
    int                                 optchar, optindex;
    struct option                       optlist[] =
    {
        {DBGMSG_OPTL_KEY,               required_argument, 0, DBGMSG_OPTC_KEY},
        {DBGMSG_OPTL_KEY_PATH,          required_argument, 0, 0},
        {DBGMSG_OPTL_KEY_ID,            required_argument, 0, 0},
        {0, 0, 0, 0}
    };
    ERR_NULL(ctl); ERR_NPOS(argc); ERR_NULL(argv);

    if (ctl->cfg != &(ctl->_cfg))
    {
        ctl->cfg = &(ctl->_cfg);
    }
    MEMZ(ctl->cfg, sizeof(DBGMSG_CFG));
    cfg = ctl->cfg;

    opterr = 0;
    optind = 0;
    while (1)
    {
        optchar = getopt_long_only(argc, argv, "", optlist, &optindex);
        if (optchar == -1) { break; }

        switch (optchar)
        {
        case DBGMSG_OPTC_KEY:
            ERR_OPTARG_INVALID();
            cfg->key = strtoul(optarg, NULL, 0);
            INF("key=0x%08X", cfg->key);
            break;
        case 0:
            if (strncmp(optlist[optindex].name, DBGMSG_OPTL_KEY_PATH, strlen(DBGMSG_OPTL_KEY_PATH)) == 0)
            {
                ERR_OPTARG_INVALID();
                snprintf(cfg->key_path, DBGMSG_KEY_PATH_LEN, "%s", optarg);
                INF("key_path=\"%s\"", cfg->key_path);
            }
            else if (strncmp(optlist[optindex].name, DBGMSG_OPTL_KEY_ID, strlen(DBGMSG_OPTL_KEY_ID)) == 0)
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

    if (strlen(cfg->key_path) < 1)
    {
        pwd = getcwd(cfg->key_path, DBGMSG_KEY_PATH_LEN); ERR_NULL(pwd);
        INF("key_path=\"%s\"", cfg->key_path);
    }
    if ((cfg->key_id < DBGMSG_KEY_ID_MIN) || (cfg->key_id > DBGMSG_KEY_ID_MAX))
    {
        cfg->key_id = DBGMSG_KEY_ID_DFT;
        INF("key_id=0x%02X", cfg->key_id);
    }
    if (cfg->key < 1)
    {
        cfg->key = ftok(cfg->key_path, cfg->key_id); ERR_NEG(cfg->key);
        INF("key=0x%X", cfg->key);
    }

    dbgmsg_config_show(ctl->cfg);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static void
dbgmsg_help (void)
{   ENTR();

    printf("-%c/--%s\t%s\r\n", DBGMSG_OPTC_KEY, DBGMSG_OPTL_KEY, DBGMSG_OPTS_KEY);
    printf("--%s\t%s\r\n", DBGMSG_OPTL_KEY_PATH, DBGMSG_OPTS_KEY_PATH);
    printf("--%s\t\t%s\r\n", DBGMSG_OPTL_KEY_ID, DBGMSG_OPTS_KEY_ID);

LEXIT;
    return;
}
/*························································*/
static void
dbgmsg_release (
    DBGMSG_CTL * const                  ctl)
{   ENTR();
    ERR_NULL(ctl);

    if (ctl->qid >= 0)
    {
        msgctl(ctl->qid, IPC_RMID, NULL);
    }
    MEMZ(ctl, sizeof(DBGMSG_CTL));
    ctl->qid = -1;

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbgmsg_init (
    DBGMSG_CTL * const                  ctl,
    const int                           argc,
    char * const                        argv[])
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl); ERR_NPOS(argc); ERR_NULL(argv);

    if (ctl->cfg != NULL)
    {
        dbgmsg_release(ctl);
    }
    ret = dbgmsg_config(ctl, argc, argv); ERR_NZERO(ret);

    ctl->qid = msgget(ctl->cfg->key, IPC_CREAT); ERR_NEG(ctl->qid);
    LOG("qid=0x%08X", ctl->qid);

    ctl->ready = true;
    LOG("ready=%s", STRBOOL(ctl->ready));

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    dbgmsg_release(ctl);
    GOEXIT;
}

/*____________________________________________________________________________*/
/* DBGMSG_SVR */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
int
dbgmsg_svr_fprintf (
    DBGMSG_SVR_CTL * const              ctl,
    FILE * const                        fp)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl); ERR_NULL(fp);
    ERR_FALSE(ctl->ready);

    for (uint32_t i = 0; i < ctl->msg_count; i++)
    {
        ctl->msg = &(ctl->msg_buf[i]);
        ctl->time_local = localtime(&(ctl->msg->src_time));
        ret = fprintf(fp, DBGMSG_MSG_FMT,
                      ctl->LOCAL_YEAR, ctl->LOCAL_MON, ctl->LOCAL_DAY,
                      ctl->LOCAL_HOUR, ctl->LOCAL_MIN, ctl->LOCAL_SEC,
                      ctl->msg->src_pid, ctl->msg->src_name, ctl->msg->text); WRN_NPOS(ret);
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
int
dbgmsg_svr_recv (
    DBGMSG_SVR_CTL * const              ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready);

    ctl->msg_count = 0;
    for (uint32_t i = 0; i < DBGMSG_SVR_MSG_BUF_SIZE; i++)
    {
        ret = msgrcv(ctl->dbgmsg->qid, &(ctl->msg_buf[i]),
                     (sizeof(DBGMSG_MSG) - sizeof(long)), 0,
                     (IPC_NOWAIT | MSG_NOERROR));
        if (ret < 1)
        {
            break;
        }
        ctl->msg_count++;
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
void
dbgmsg_svr_help (void)
{   ENTR();

    printf("dbgmsg_svr:\r\n");

    dbgmsg_help();

LEXIT;
    return;
}
/*························································*/
void
dbgmsg_svr_release (
    DBGMSG_SVR_CTL * const              ctl)
{   ENTR();
    ERR_NULL(ctl);

    if (ctl->dbgmsg != NULL)
    {
        dbgmsg_release(ctl->dbgmsg);
    }
    MEMZ(ctl, sizeof(DBGMSG_SVR_CTL));

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
int
dbgmsg_svr_init (
    DBGMSG_SVR_CTL * const              ctl,
    const int                           argc,
    char * const                        argv[])
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl); ERR_NPOS(argc); ERR_NULL(argv);

    if (ctl->dbgmsg != NULL)
    {
        dbgmsg_svr_release(ctl);
    }
    ctl->dbgmsg = &(ctl->_dbgmsg);
    ret = dbgmsg_init(ctl->dbgmsg, argc, argv); ERR_NZERO(ret);

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
/* DBGMSG_CLNT */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
void
dbgmsg_clnt_printf (
    DBGMSG_CLNT_CTL * const             ctl,
    const char *                        fmt,
    ...)
{
    int                                 ret = -1;
    static DBGMSG_MSG                   _msg, * const msg = &_msg;

    if ((ctl == NULL) || (ctl->ready != true)) { return; }

    MEMZ(msg, sizeof(DBGMSG_MSG));
    msg->type     = DBGMSG_TYPE_DEBUG;
    msg->src_time = time(NULL);
    msg->src_pid  = ctl->src_pid;

    snprintf(msg->src_name, DBGMSG_SRC_NAME_LEN, "%s", ctl->src_name);
    ctl->src_name[DBGMSG_SRC_NAME_LEN - 1] = '\0';

    va_start(ctl->vargs, fmt);
    vsnprintf(msg->text, DBGMSG_TEXT_LEN, fmt, ctl->vargs);
    va_end(ctl->vargs);
    msg->text[DBGMSG_TEXT_LEN - 1] = '\0';

    ret = msgsnd(ctl->dbgmsg->qid, msg, (sizeof(DBGMSG_MSG) - sizeof(long)), IPC_NOWAIT);
    if (ret != 0)
    {
        DBGSTD("ERR!msgsnd ret=%d!=0", ret);
    }
}
/*························································*/
int
dbgmsg_clnt_set_src_name (
    DBGMSG_CLNT_CTL * const             ctl,
    char * const                        name)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl); ERR_NULL(name);

    snprintf(ctl->src_name, DBGMSG_SRC_NAME_LEN, "%s", name);
    ctl->src_name[DBGMSG_SRC_NAME_LEN - 1] = '\0';
    LOG("src_name=\"%s\"", ctl->src_name);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
void
dbgmsg_clnt_help (void)
{   ENTR();

    printf("dbgmsg_clnt:\r\n");

    dbgmsg_help();

LEXIT;
    return;
}
/*························································*/
void
dbgmsg_clnt_release (
    DBGMSG_CLNT_CTL * const             ctl)
{   ENTR();
    ERR_NULL(ctl);

    if (ctl->dbgmsg != NULL)
    {
        dbgmsg_release(ctl->dbgmsg);
    }
    MEMZ(ctl, sizeof(DBGMSG_CLNT_CTL));

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
int
dbgmsg_clnt_init (
    DBGMSG_CLNT_CTL * const             ctl,
    const int                           argc,
    char * const                        argv[])
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl); ERR_NPOS(argc); ERR_NULL(argv);

    if (ctl->dbgmsg != NULL)
    {
        dbgmsg_clnt_release(ctl);
    }
    ctl->dbgmsg = &(ctl->_dbgmsg);
    ret = dbgmsg_init(ctl->dbgmsg, argc, argv); ERR_NZERO(ret);

    ctl->src_pid = getpid(); ERR_NPOS(ctl->src_pid);
    LOG("src_pid=%d", ctl->src_pid);

    ctl->ready = true;
    LOG("ready=%s", STRBOOL(ctl->ready));

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    dbgmsg_clnt_release(ctl);
    GOEXIT;
}

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
