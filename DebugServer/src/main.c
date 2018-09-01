/*____________________________________________________________________________*/
/* INCLUDE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#include "main.h"

/*____________________________________________________________________________*/
/* GLOBAL */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
uint32_t                                gverbose = MAIN_VERBOSE_DFT;
DBG_CLNT_CTL                            _gdbg_clnt = {0}, * const gdbg_clnt = &(_gdbg_clnt);
DBG_SVR_CTL                             _gdbg_svr = {0}, * const gdbg_svr = &(_gdbg_svr);

/*____________________________________________________________________________*/
/* MAIN */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
static void
main_signal_action (
    int                                 signum
    )
{   ENTR();

    INF("signum=%d", signum);
    switch (signum)
    {
    case SIGINT:
        WRN("SIGINT");
        main_exit(0);
        break;
    case SIGTERM:
        WRN("SIGTERM");
        main_exit(0);
        break;
    default:
        break;
    }

LEXIT;
    return;
}
/*························································*/
static int
main_signal_init (void)
{   ENTR();
    int                                 ret = -1;
    struct sigaction                    _sigact = {0}, * const sigact = &_sigact;

    MEMZ(sigact, sizeof(struct sigaction));
    sigact->sa_handler = main_signal_action;
    ret = sigemptyset(&(sigact->sa_mask)); ERR_NZERO(ret);

    ret = sigaction(SIGINT, sigact, NULL); ERR_NZERO(ret);
    INF("ctrl+c to quit");

    ret = sigaction(SIGTERM, sigact, NULL); ERR_NZERO(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
main_loop (void)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(gdbg_svr);

    while (1)
    {
        usleep(MAIN_LOOP_STEP_US);

        if (gdbg_svr->ready == true)
        {
            ret = dbg_svr_loop_job(gdbg_svr); WRN_NZERO(ret);
        }
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static void
main_config_show (void)
{   ENTR();

    LOG("gverbose=%u", gverbose);

LEXIT;
    return;
}
/*························································*/
static int
main_config (
    const int                           argc,
    char * const                        argv[])
{   ENTR();
    int                                 ret = -1;
    int                                 optchar, optindex;
    struct option                       optlist[] =
    {
        {MAIN_OPTL_HELP,                no_argument,        0, MAIN_OPTC_HELP},
        {MAIN_OPTL_VERBOSE,             required_argument,  0, MAIN_OPTC_VERBOSE},
        {0, 0, 0, 0}
    };
    ERR_NPOS(argc); ERR_NULL(argv);

    opterr = 0;
    optind = 0;
    while (1)
    {
        optchar = getopt_long_only(argc, argv, "", optlist, &optindex);
        if (optchar == -1) { break; }
        switch (optchar)
        {
        case MAIN_OPTC_HELP:
            main_help(argv[0]);
            break;
        case MAIN_OPTC_VERBOSE:
            ERR_OPTARG_INVALID();
            gverbose = strtoul(optarg, NULL, 0);
            INF("gverbose=%u", gverbose);
            break;
        default:
            break;
        }
    }

    if ((gverbose < MAIN_VERBOSE_MIN) || (gverbose > MAIN_VERBOSE_MAX))
    {
        gverbose = MAIN_VERBOSE_DFT;
        INF("gverbose=%u", gverbose);
    }

    main_config_show();

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static void
main_help (
    char * const                        name)
{   ENTR();
    ERR_NULL(name);

    printf("%s options:\r\n", name);
    printf("-%c/--%s\t%s\r\n",
           MAIN_OPTC_HELP, MAIN_OPTL_HELP, MAIN_OPTS_HELP);
    printf("-%c/--%s\t%s <%d,%d>\r\n",
           MAIN_OPTC_VERBOSE, MAIN_OPTL_VERBOSE, MAIN_OPTS_VERBOSE,
           DBG_VERBOSE_MIN, DBG_VERBOSE_MAX);

    dbg_clnt_help();
    dbg_svr_help();

LEXIT;
    main_exit(0);
    return;
LERROR;
    GOEXIT;
}
/*························································*/
static void
main_exit (
    int                                 ret)
{   ENTR();

    WRN_NZERO(ret);

    if (gdbg_clnt != NULL)
    {
        dbg_clnt_release(gdbg_clnt);
    }
    if (gdbg_svr != NULL)
    {
        dbg_svr_release(gdbg_svr);
    }

LEXIT;
    exit(ret);
}
/*························································*/
static int
main_init (
    const int                           argc,
    char * const                        argv[])
{   ENTR();
    int                                 ret = -1;
    ERR_NPOS(argc); ERR_NULL(argv);

    ret = main_signal_init(); ERR_NZERO(ret);
    ret = main_config(argc, argv); ERR_NZERO(ret);

    ERR_NULL(gdbg_clnt);
    MEMZ(gdbg_clnt, sizeof(DBG_CLNT_CTL));
    ret = dbg_clnt_init(gdbg_clnt, argc, argv); ERR_NZERO(ret);
    ret = dbg_clnt_set_src_name(gdbg_clnt, MAIN_SRC_NAME);

    LOG("argc=%d", argc);
    for (int i = 1; i < argc; i++)
    {
        LOG("argv[%d]=\"%s\"", i, argv[i]);
    }

    ERR_NULL(gdbg_svr);
    MEMZ(gdbg_svr, sizeof(DBG_SVR_CTL));
    ret = dbg_svr_init(gdbg_svr, argc, argv); ERR_NZERO(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
int
main (
    const int                           argc,
    char * const                        argv[])
{   ENTR();
    int                                 ret = -1;
    ERR_NPOS(argc); ERR_NULL(argv);

    if (argc < 2)
    {
        main_help(argv[0]);
        GOEXIT;
    }

    ret = main_init(argc, argv); ERR_NZERO(ret);
    ret = main_loop(); ERR_NZERO(ret);

    ret = 0;
LEXIT;
    main_exit(ret);
    return(ret);
LERROR;
    GOEXIT;
}

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
