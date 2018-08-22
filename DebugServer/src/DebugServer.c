/*____________________________________________________________________________*/
/* INCLUDE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#include "debug.h"

/*____________________________________________________________________________*/
/* GLOBAL */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
static DBG_SVR_CTL                      _gdbg_svr, * const gdbg_svr = &(_gdbg_svr);

/*____________________________________________________________________________*/
/* DBG_SVR */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
static int
dbg_svr_loop_job (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready);

    ret = dbg_svr_recv(ctl); WRN_NZERO(ret);

    ret = dbg_svr_log_fprintf(ctl); WRN_NZERO(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_recv (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready);

    if ((ctl->cfg->dbgmsg_svr_enable == true) && (ctl->dbgmsg_svr->ready == true))
    {
        ret = dbgmsg_svr_recv(ctl->dbgmsg_svr); WRN_NZERO(ret);
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_log_check_size (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready);

    if (ctl->log_fp == NULL)
    {
            ret = 0;
            GOEXIT;
    }

    fseek(ctl->log_fp, 0 , SEEK_END);
    ctl->log_size_current = ftell(ctl->log_fp) / (1024 * 1024);
    INF("log_size_current=%dMB", ctl->log_size_current);

    if (ctl->log_size_current >= ctl->cfg->log_size)
    {
        ret = dbg_svr_log_shift(ctl); ERR_NZERO(ret);
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
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

    ret = dbg_svr_log_open_new(ctl); ERR_NZERO(ret);

    ret = dbg_svr_idx_create_increase(ctl); ERR_NZERO(ret);

    /*
    TODO    delete
    */

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_log_fprintf (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready); ERR_NULL(ctl->log_fp);

    if ((ctl->cfg->dbgmsg_svr_enable == true) && (ctl->dbgmsg_svr->ready == true))
    {
        ret = dbgmsg_svr_fprintf(ctl->dbgmsg_svr, ctl->log_fp); WRN_NZERO(ret);
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_log_write_trailer (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_NULL(ctl->log_fp);

    ctl->time_now = time(NULL);
    ctl->time_local = localtime(&(ctl->time_now));
    ret = fprintf(ctl->log_fp, DBG_SVR_LOG_TRAILER_TIME_FMT,
                  ctl->LOCAL_YEAR, ctl->LOCAL_MON, ctl->LOCAL_DAY,
                  ctl->LOCAL_HOUR, ctl->LOCAL_MIN, ctl->LOCAL_SEC); WRN_NPOS(ret);

    dbg_svr_log_flush(ctl);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
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

    ret = fprintf(ctl->log_fp, "[DBG_SVR|COMPILE_TIME=%s-%s]\r\n", __DATE__, __TIME__); WRN_NPOS(ret);
    ret = fprintf(ctl->log_fp, "[LOG|PATH=%s|NAME=%s]\r\n", ctl->cfg->dir_path, ctl->log_name); WRN_NPOS(ret);
    ret = fprintf(ctl->log_fp, DBG_SVR_IDX_FMT, ctl->idx_create, ctl->idx_delete); WRN_NPOS(ret);

    ctl->time_now = time(NULL);
    ctl->time_local = localtime(&(ctl->time_now));
    ret = fprintf(ctl->log_fp, DBG_SVR_LOG_HEADER_TIME_FMT,
                  ctl->LOCAL_YEAR, ctl->LOCAL_MON, ctl->LOCAL_DAY,
                  ctl->LOCAL_HOUR, ctl->LOCAL_MIN, ctl->LOCAL_SEC); WRN_NPOS(ret);

    dbg_svr_log_flush(ctl);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_log_open_new (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    ret = dbg_svr_log_name_new(ctl); ERR_NZERO(ret);

    ret = dbg_svr_log_open(ctl); ERR_NZERO(ret);

    ret = dbg_svr_log_write_header(ctl); ERR_NZERO(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
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
/*························································*/
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
/*························································*/
static void
dbg_svr_log_close (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    if (ctl->log_fp != NULL)
    {
        dbg_svr_log_flush(ctl);

        ret = dbg_svr_log_write_trailer(ctl); WRN_NZERO(ret);

        fclose(ctl->log_fp);
        ctl->log_fp = NULL;
        LOG("close=\"%s\"", ctl->log_path_name);
    }

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_log_open (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_NULL(ctl->dir);

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
/*························································*/
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
/*························································*/
static int
dbg_svr_idx_write (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_NULL(ctl->cfg);

    ret = dbg_svr_idx_open_write(ctl); ERR_NZERO(ret);

    ret = fprintf(ctl->idx_fp, DBG_SVR_IDX_FMT, ctl->idx_create, ctl->idx_delete); ERR_NPOS(ret);
    INF("idx_create=%04u idx_delete=%04u", ctl->idx_create, ctl->idx_delete);

    dbg_svr_idx_close(ctl);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
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
        ret = 0;
        GOEXIT;
    }

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
/*························································*/
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
/*························································*/
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
/*························································*/
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

    ret = dbg_svr_dir_open(ctl); WRN_NZERO(ret);
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

    ret = dbg_svr_dir_open(ctl); ERR_NZERO(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static void
dbg_svr_dir_close (
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
/*························································*/
static int
dbg_svr_dir_open (
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
            dbg_svr_dir_close(ctl);
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
dbg_svr_config_show (
    DBG_SVR_CFG * const                 cfg)
{   ENTR();
    ERR_NULL(cfg);

    LOG("dir_path=\"%s\"", cfg->dir_path);
    LOG("log_size=%u", cfg->log_size);
    LOG("log_count=%u", cfg->log_count);
    LOG("dbgmsg_svr_enable=%s", STRBOOL(cfg->dbgmsg_svr_enable));

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_config (
    DBG_SVR_CTL * const                 ctl,
    const int                           argc,
    char * const                        argv[])
{   ENTR();
    int                                 ret = -1;
    DBG_SVR_CFG *                       cfg = NULL;
    int                                 optchar;
    int                                 optindex;
    struct option                       optlist[] =
    {
        {DBG_SVR_OPTL_LOG_PATH,         required_argument,  0, DBG_SVR_OPTC_LOG_PATH},
        {DBG_SVR_OPTL_LOG_SIZE,         required_argument,  0, DBG_SVR_OPTC_LOG_SIZE},
        {DBG_SVR_OPTL_LOG_COUNT,        required_argument,  0, DBG_SVR_OPTC_LOG_COUNT},
        {DBG_SVR_OPTL_DBGMSG_SVR,       no_argument,        0, DBG_SVR_OPTC_DBGMSG_SVR},
        {0, 0, 0, 0}
    };
    ERR_NULL(cfg); ERR_NPOS(argc); ERR_NULL(argv);

    if (ctl->cfg == NULL)
    {
        MALLOCZ(ctl->cfg, DBG_SVR_CFG); ERR_NULL(ctl->cfg);
    }
    else
    {
        MEMZ(ctl->cfg, sizeof(DBG_SVR_CFG));
    }
    cfg = ctl->cfg;

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

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
void
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
/*························································*/
void
dbg_svr_release (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();

    ERR_NULL(ctl);

    dbg_svr_log_close(ctl);
    dbg_svr_idx_close(ctl);
    dbg_svr_dir_close(ctl);

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
/*························································*/
int
dbg_svr_init (
    DBG_SVR_CTL * const                 ctl,
    const int                           argc,
    char * const                        argv[])
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl); ERR_NPOS(argc); ERR_NULL(argv);

    if (ctl->cfg != NULL)
    {
        dbg_svr_release(ctl);
    }
    ret = dbg_svr_config(ctl, argc, argv); ERR_NZERO(ret);
    dbg_svr_config_show(ctl->cfg);

    ret = dbg_svr_mkdir(ctl); ERR_NZERO(ret);
    ret = dbg_svr_idx_read(ctl); ERR_NZERO(ret);
    ret = dbg_svr_log_shift(ctl); ERR_NZERO(ret);

    if (ctl->cfg->dbgmsg_svr_enable == true)
    {
        ctl->dbgmsg_svr = &(ctl->_dbgmsg_svr);
        ret = dbgmsg_svr_init(ctl->dbgmsg_svr, argc, argv); ERR_NZERO(ret);
    }

    ctl->ready = true;
    LOG("ready=%s", STRBOOL(ctl->ready));

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
    ERR_NZERO(gdbg_svr);

    while (1)
    {
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

    LOG("gverbose=%d", gverbose);

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
    int                                 optchar;
    int                                 optindex;
    struct option                       optlist[] =
    {
        {MAIN_OPTL_HELP,                no_argument,        0, MAIN_OPTC_HELP},
        {MAIN_OPTL_VERBOSE,             required_argument,  0, MAIN_OPTC_VERBOSE},
        {0, 0, 0, 0}
    };
    ERR_NPOS(argc); ERR_NULL(argv);

    if (argc < 2) { GOERROR; }
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
        case MAIN_OPTC_HELP:
            main_help(argv[0]);
            main_exit(0);
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

    if ((gverbose < DBG_SVR_VERBOSE_MIN) || (gverbose > DBG_SVR_VERBOSE_MAX))
    {
        gverbose = DBG_SVR_VERBOSE_DFT;
        INF("gverbose=%u", gverbose);
    }

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
    printf("-%c/--%s\t%s\r\n", MAIN_OPTC_HELP, MAIN_OPTL_HELP, MAIN_OPTS_HELP);
    printf("-%c/--%s\t%s <%d,%d>\r\n",
           MAIN_OPTC_VERBOSE, MAIN_OPTL_VERBOSE, MAIN_OPTS_VERBOSE,
           DBG_VERBOSE_MIN, DBG_VERBOSE_MAX);

    dbg_clnt_help();
    dbg_svr_help();

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
static void
main_exit (
    int                                 ret)
{   ENTR();

    LOG("ret=%d", ret);

    if (gdbg_svr != NULL)
    {
        dbg_svr_release(gdbg_svr);
    }
    if (gdbg_clnt != NULL)
    {
        dbg_clnt_release(gdbg_clnt);
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

    ret = main_config(argc, argv); ERR_NZERO(ret);
    main_config_show();

    ERR_NULL(gdbg_svr);
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
        main_exit(0);
    }

    ERR_NULL(gdbg_clnt);
    MEMZ(gdbg_clnt, sizeof(DBG_CLNT_CTL));
    ret = dbg_clnt_init(gdbg_clnt, argc, argv); ERR_NZERO(ret);
    ret = dbg_clnt_set_src_name(gdbg_clnt, "DBG_SVR"); WRN_NZERO(ret);

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
