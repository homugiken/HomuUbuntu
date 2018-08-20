/*____________________________________________________________________________*/
/* INCLUDE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#include "debug.h"

/*____________________________________________________________________________*/
/* OPTION */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
extern int                              optind;
extern char *                           optarg;
static struct option                    dbgmsg_optlist[] =
{
    {DBGMSG_OPTL_KEY,                   required_argument,  0,                  DBGMSG_OPTC_KEY},
    {DBGMSG_OPTL_KEY_PATH,              required_argument,  0,                  0},
    {DBGMSG_OPTL_KEY_ID,                required_argument,  0,                  0},
    {DBGMSG_OPTL_SRC_NAME,              required_argument,  0,                  0},
    {0, 0, 0, 0}
};

/*____________________________________________________________________________*/
/* DBGMSG */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
void
dbgmsg_printf (
    DBGMSG_CTL * const                  ctl,
    const char *                        fmt,
    ...)
{
    static DBGMSG_MSG                   _msg, * const msg = &_msg;

    if ((ctl == NULL) || (ctl->ready != true)) { return; }

    MEMZ(msg, sizeof(DBGMSG_MSG));
    msg->type = DBGMSG_TYPE_DEBUG;
    msg->src_time = time(NULL);
    msg->src_pid = ctl->src_pid;
    snprintf(msg->src_name, DBGMSG_SRC_NAME_LEN, "%s", ctl->cfg->src_name);

    va_start(ctl->vargs, fmt);
    vsnprintf(msg->text, DBGSTD_TEXT_LEN, fmt, ctl->vargs);
    va_end(ctl->vargs);

    msgsnd(ctl->qid, msg, (sizeof(DBGMSG_MSG) - sizeof(long)), IPC_NOWAIT);
}
/*························································*/
int
dbgmsg_fprintf(
    FILE * const                        fp,
    const DBGMSG_MSG *                  const msg)
{   ENTR();
    int                                 ret = -1;
    static struct tm *                  time_local;
    static char                         str[2048];
    ERR_NULL(fp); ERR_NULL(msg);

    MEMZ(str, 2048);
    time_local = localtime(&(msg->src_time));
    snprintf(str, 2048, DBGMSG_MSG_FMT,
             LOCAL_YEAR, LOCAL_MON, LOCAL_DAY,
             LOCAL_HOUR, LOCAL_MIN, LOCAL_SEC,
             msg->src_pid, msg->src_name, msg->text);
    fprintf(stdout, "%s", str);
    ret = fprintf(fp, "%s", str); ERR_NPOS(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
int
dbgmsg_msgrcv(
    const DBGMSG_CTL * const            ctl,
    DBGMSG_MSG * const                  msg)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl); ERR_NULL(msg);
    ERR_FALSE(ctl->ready);

    MEMZ(msg, sizeof(DBGMSG_MSG));
    ret = msgrcv(ctl->qid, msg, (sizeof(DBGMSG_MSG) - sizeof(long)), (IPC_NOWAIT | MSG_NOERROR)); ERR_NPOS(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbgmsg_msgget(
    DBGMSG_CTL * const                  ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_NULL(ctl->cfg);

    ctl->qid = msgget(ctl->cfg->key, IPC_CREAT); ERR_NEG(ctl->qid);
    LOG("qid=%d", ctl->qid);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static void
dbgmsg_config_show (
    DBGMSG_CFG * const                  cfg)
{   ENTR();
    ERR_NULL(cfg);

    LOG("key=0x%08X", cfg->key);
    LOG("key_path=\"%s\"", cfg->key_path);
    LOG("key_id=0x%02X", cfg->key_id);
    LOG("src_name=\"%s\"", cfg->src_name);

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
    const char * const                  argv[])
{   ENTR();
    int                                 ret = -1;
    int                                 optchar;
    int                                 optindex;
    DBGMSG_CFG *                        cfg;
    ERR_NULL(ctl); ERR_NPOS(argc); ERR_NULL(argv);

    if (ctl->cfg == NULL)
    {
        MALLOCZ(ctl->cfg, DBGMSG_CFG); ERR_NULL(ctl->cfg);
    }
    cfg = ctl->cfg;
    MEMZ(cfg, sizeof(DBGMSG_CFG));

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
            else if (strncmp(optlist[optindex].name, DBGMSG_OPTL_SRC_NAME, strlen(DBGMSG_OPTL_SRC_NAME)) == 0)
            {
                ERR_OPTARG_INVALID();
                snprintf(cfg->src_name, DBGMSG_SRC_NAME_LEN, "%s", optarg);
                INF("src_name=\"%s\"", cfg->src_name);
            }
            break;
        default:
                break;
        }
    }

    if (strlen(cfg->key_path) < 1)
    {
        snprintf(cfg->key_path, DBGMSG_KEY_PATH_LEN, "%s", DBGMSG_KEY_PATH_DFT);
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

    dbgmsg_config_show(cfg);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
void
dbgmsg_help (void)
{   ENTR();

    printf("-%c/--%s\t%s\r\n", DBGMSG_OPTC_KEY, DBGMSG_OPTL_KEY, DBGMSG_OPTS_KEY);
    printf("--%s\t%s\r\n", DBGMSG_OPTL_KEY_PATH, DBGMSG_OPTS_KEY_PATH);
    printf("--%s\t\t%s\r\n", DBGMSG_OPTL_KEY_ID, DBGMSG_OPTS_KEY_ID);
    printf("--%s\t\t%s\r\n", DBGMSG_OPTL_SRC_NAME, DBGMSG_OPTS_SRC_NAME);

LEXIT;
    return;
}
/*························································*/
void
dbgmsg_release (
    DBGMSG_CTL * const                  ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    if (ctl->cfg != NULL)
    {
        free(ctl->cfg);
    }
    if (ctl->qid >=0)
    {
        ret = msgctl(ctl->qid, IPC_RMID, NULL); WRN_NZERO(ret);
    }

    MEMZ(ctl, sizeof(DBGMSG_CTL));
    ctl->qid = -1;

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
int
dbgmsg_init (
    DBGMSG_CTL * const                  ctl,
    const int                           argc,
    const char * const                  argv[])
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl); ERR_NPOS(argc); ERR_NULL(argv);

    if (ctl->cfg != NULL)
    {
        dbgmsg_release(ctl);
    }
    ret = dbgmsg_config(ctl, argc, argv); ERR_NZERO(ret);
    dbgmsg_config_show(ctl);

    ret = dbgmsg_msgget(ctl); ERR_NZERO(ret);

    ctl->ready = true;
    LOG("ready=%s", STRBOOL(ctl->ready));

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    dbgmsg_release(ctl);
    GOEXIT;
}

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
