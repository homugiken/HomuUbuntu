/*____________________________________________________________________________*/
/* INCLUDE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#include "debug.h"

/*____________________________________________________________________________*/
/* DBG_CLNT */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
void
dbg_clnt_printf (
    DBG_CLNT_CTL * const                ctl,
    const char *                        fmt,
    ...)
{
    static char                         str[DBG_CLNT_PRINTF_STR_LEN];

    if ((ctl == NULL) || (ctl->ready != true)) { return; }

    va_start(ctl->vargs, fmt);
    vsnprintf(str, DBG_CLNT_PRINTF_STR_LEN, fmt, ctl->vargs);
    va_end(ctl->vargs);
    str[DBG_CLNT_PRINTF_STR_LEN - 1] = '\0';

    if (ctl->cfg->dbgmsg_clnt_enable == true)
    {
        dbgmsg_clnt_printf(ctl->dbgmsg_clnt, "%s", str);
    }
}
/*························································*/
int
dbg_clnt_set_src_name (
    DBG_CLNT_CTL * const                ctl,
    char * const                        name)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl); ERR_NULL(name);
    ERR_FALSE(ctl->ready);

    if (ctl->cfg->dbgmsg_clnt_enable == true)
    {
        ret = dbgmsg_clnt_set_src_name(ctl->dbgmsg_clnt, name); ERR_NZERO(ret);
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static void
dbg_clnt_config_show (
    DBG_CLNT_CFG * const                cfg)
{   ENTR();
    ERR_NULL(cfg);

    LOG("dbgmsg_clnt_enable=%s", STRBOOL(cfg->dbgmsg_clnt_enable));

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_clnt_config (
    DBG_CLNT_CTL * const                ctl,
    const int                           argc,
    char * const                        argv[])
{   ENTR();
    int                                 ret = -1;
    DBG_CLNT_CFG *                      cfg = NULL;
    int                                 optchar;
    int                                 optindex;
    struct option                       optlist[] =
    {
        {DBG_CLNT_OPTL_DBGMSG_CLNT,     no_argument,        0, DBG_CLNT_OPTC_DBGMSG_CLNT},
        {0, 0, 0, 0}
    };
    ERR_NULL(ctl); ERR_NPOS(argc); ERR_NULL(argv);

    if (ctl->cfg == NULL)
    {
        MALLOCZ(ctl->cfg, DBG_CLNT_CFG); ERR_NULL(ctl->cfg);
    }
    else
    {
        MEMZ(ctl->cfg, sizeof(DBG_CLNT_CFG));

    }
    cfg = ctl->cfg;

    optind = 0;
    while (1)
    {
        optchar = getopt_long_only(argc, argv, "", optlist, &optindex);
        if (optchar == -1) { break; }

        switch (optchar)
        {
        case DBG_CLNT_OPTC_DBGMSG_CLNT:
            cfg->dbgmsg_clnt_enable = true;
            INF("dbgmsg_clnt_enable=%s", STRBOOL(cfg->dbgmsg_clnt_enable));
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
/*························································*/
void
dbg_clnt_help (void)
{   ENTR();

    printf("dbg_clnt:\r\n");
    dbgmsg_clnt_help();

LEXIT;
    return;
}
/*························································*/
void
dbg_clnt_release (
    DBG_CLNT_CTL * const                ctl)
{   ENTR();
    ERR_NULL(ctl);

    if (ctl->dbgmsg_clnt != NULL)
    {
        dbgmsg_clnt_release(ctl->dbgmsg_clnt);
    }

    MEMZ(ctl, sizeof(DBG_CLNT_CTL));

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
int
dbg_clnt_init (
    DBG_CLNT_CTL * const                ctl,
    const int                           argc,
    char * const                        argv[])
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl); ERR_NPOS(argc); ERR_NULL(argv);

    if (ctl->cfg != NULL)
    {
        dbg_clnt_release(ctl);
    }
    ret = dbg_clnt_config(ctl, argc, argv); ERR_NZERO(ret);
    dbg_clnt_config_show(ctl->cfg);

    if (ctl->cfg->dbgmsg_clnt_enable == true)
    {
        ctl->dbgmsg_clnt = &(ctl->_dbgmsg_clnt);
        ret = dbgmsg_clnt_init(ctl->dbgmsg_clnt, argc, argv); ERR_NZERO(ret);
    }

    ctl->ready = true;
    LOG("ready=%s", STRBOOL(ctl->ready));

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    dbg_clnt_release(ctl);
    GOEXIT;
}

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
