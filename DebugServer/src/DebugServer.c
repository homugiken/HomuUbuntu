/*____________________________________________________________________________*/
/* INCLUDE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#include "debug.h"

/*____________________________________________________________________________*/
/* GLOBAL */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
uint32_t                                _gverbose = DBG_VERBOSE_DFT, * const gverbose = &_gverbose;
DBG_CLNT_CTL                            _gdbg_clnt = {0}, * const gdbg_clnt = &(_gdbg_clnt);
static DBG_SVR_CTL                      _gdbg_svr = {0}, * const gdbg_svr = &(_gdbg_svr);

/*____________________________________________________________________________*/
/* DBG_SVR_DIR */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
static void
dbg_svr_dir_close (
    DBG_SVR_DIR_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    if (ctl->dp != NULL)
    {
        ret = closedir(ctl->dp); WRN_NZERO(ret);
        ctl->dp = NULL;
    }

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_dir_open (
    DBG_SVR_DIR_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl); ERR_NPOS(strlen(ctl->path_name));

    if (ctl->dp != NULL)
    {
        dbg_svr_dir_close(ctl);
    }
    ctl->dp = opendir(ctl->path_name); ERR_NULL(ctl->dp);
    LOG("opendir=\"%s\"", ctl->path_name);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_dir_make (
    DBG_SVR_DIR_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    char                                str[DBG_SVR_DIR_PATH_NAME_LEN];
    uint32_t                            len;
    ERR_NULL(ctl);

    len = strlen(ctl->path_name);
    for (uint32_t i = 0; i < len; i++)
    {
        if (((ctl->path_name[i] == '/') && (i > 0)) || (i == (len - 1)))
        {
            MEMZ(str, DBG_SVR_DIR_PATH_NAME_LEN);
            snprintf(str, (i + 2), "%s", ctl->path_name);
            ret = mkdir(str, DEFFILEMODE);
            if ((ret < 0) && (errno != EEXIST))
            {
                ERR_NZERO(ret);
            }
            LOG("mkdir=\"%s\"", str);
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
dbg_svr_dir_release (
    DBG_SVR_DIR_CTL * const             ctl)
{   ENTR();
    ERR_NULL(ctl);

    if (ctl->dp != NULL)
    {
        dbg_svr_dir_close(ctl);
    }
    MEMZ(ctl, sizeof(DBG_SVR_DIR_CTL));

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_dir_init (
    DBG_SVR_DIR_CTL * const             ctl,
    char * const                        path)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl); ERR_NULL(path);

    if (strlen(path) > 1)
    {
        snprintf(ctl->path_name, DBG_SVR_DIR_PATH_NAME_LEN, "%s/%s",
                 path, DBG_SVR_DIR_NAME_DFT);
    }
    else
    {
        char *                          pwd;
        pwd = getcwd(ctl->path_name, GENERAL_PATH_LEN); ERR_NULL(pwd);
        snprintf(&(ctl->path_name[strlen(ctl->path_name)]), DBG_SVR_DIR_PATH_NAME_LEN,
                 "%s", DBG_SVR_DIR_NAME_DFT);
    }
    LOG("path_name=\"%s\"", ctl->path_name);

    ret = dbg_svr_dir_open(ctl); WRN_NZERO(ret);
    if (ret != 0)
    {
        ret = dbg_svr_dir_make(ctl); ERR_NZERO(ret);
        ret = dbg_svr_dir_open(ctl); ERR_NZERO(ret);
        dbg_svr_dir_close(ctl);
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

/*____________________________________________________________________________*/
/* DBG_SVR_IDX */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
static int
dbg_svr_idx_increase_remove (
    DBG_SVR_IDX_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    if (++(ctl->remove) > DBG_SVR_IDX_MAX)
    {
        ctl->remove = 0;
    }
    ret = dbg_svr_idx_write(ctl); ERR_NZERO(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_idx_increase_create (
    DBG_SVR_IDX_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    if (++(ctl->create) > DBG_SVR_IDX_MAX)
    {
        ctl->create = 0;
    }
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
    DBG_SVR_IDX_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    ret = dbg_svr_idx_open_write(ctl); ERR_NZERO(ret);
    ret = fprintf(ctl->fp, DBG_SVR_IDX_FMT, ctl->create, ctl->remove); ERR_NPOS(ret);
    dbg_svr_idx_close(ctl);
    LOG("create=%04u remove=%04u", ctl->create, ctl->remove);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_idx_read (
    DBG_SVR_IDX_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    ret = dbg_svr_idx_open_read(ctl); ERR_NZERO(ret);
    ret = fscanf(ctl->fp, DBG_SVR_IDX_FMT, &(ctl->create), &(ctl->remove)); ERR_NPOS(ret);
    dbg_svr_idx_close(ctl);
    LOG("create=%04u remove=%04u", ctl->create, ctl->remove);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static void
dbg_svr_idx_close (
    DBG_SVR_IDX_CTL * const             ctl)
{   ENTR();
    ERR_NULL(ctl);

    if(ctl->fp != NULL)
    {
        fflush(ctl->fp);
        fclose(ctl->fp);
        ctl->fp = NULL;
    }

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_idx_open_write (
    DBG_SVR_IDX_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_NPOS(strlen(ctl->path_name));

    if (ctl->fp != NULL)
    {
        dbg_svr_idx_close(ctl);
    }
    ctl->fp = fopen(ctl->path_name, "w+"); ERR_NULL(ctl->fp);
    rewind(ctl->fp);
    INF("fopen=\"%s\"", ctl->path_name);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_idx_open_read (
    DBG_SVR_IDX_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_NPOS(strlen(ctl->path_name));

    if (ctl->fp != NULL)
    {
        dbg_svr_idx_close(ctl);
    }
    ctl->fp = fopen(ctl->path_name, "r"); ERR_NULL(ctl->fp);
    rewind(ctl->fp);
    INF("fopen=\"%s\"", ctl->path_name);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static void
dbg_svr_idx_release (
    DBG_SVR_IDX_CTL * const             ctl)
{   ENTR();
    ERR_NULL(ctl);

    if (ctl->fp != NULL)
    {
        dbg_svr_idx_close(ctl);
    }
    MEMZ(ctl, sizeof(DBG_SVR_IDX_CTL));

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_idx_init (
    DBG_SVR_IDX_CTL * const             ctl,
    char * const                        path)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl); ERR_NULL(path);

    if (strlen(path) > 1)
    {
        snprintf(ctl->path_name, DBG_SVR_DIR_PATH_NAME_LEN, "%s/%s",
                 path, DBG_SVR_IDX_NAME_DFT);
    }
    else
    {
        char *                          pwd;
        pwd = getcwd(ctl->path_name, GENERAL_PATH_LEN); ERR_NULL(pwd);
        snprintf(&(ctl->path_name[strlen(ctl->path_name)]), DBG_SVR_IDX_PATH_NAME_LEN,
                 "%s/%s", DBG_SVR_DIR_NAME_DFT, DBG_SVR_IDX_NAME_DFT );
    }
    LOG("path_name=\"%s\"", ctl->path_name);

    ret = dbg_svr_idx_read(ctl);
    if (ret != 0)
    {
        ctl->create = DBG_SVR_IDX_MIN;
        ctl->remove = DBG_SVR_IDX_MIN;
        ret = dbg_svr_idx_write(ctl); ERR_NZERO(ret);
        ret = dbg_svr_idx_read(ctl); ERR_NZERO(ret);
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

/*____________________________________________________________________________*/
/* DBG_SVR_LOG */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
static int
dbg_svr_log_remove_oldest (
    DBG_SVR_LOG_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    char                                cmd[DBG_SVR_LOG_PATH_NAME_LEN + 32] = {0};
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready);

    snprintf(cmd, (DBG_SVR_LOG_PATH_NAME_LEN + 32), DBG_SVR_LOG_REMOVE_NAME_FMT,
             ctl->dir->path_name, ctl->idx->remove);
    ret = system(cmd); ERR_NZERO(ret);
    LOG("remove=\"%s\"", cmd);
    ret = dbg_svr_idx_increase_remove(ctl->idx); ERR_NZERO(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_log_check_count (
    DBG_SVR_LOG_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready);

    ctl->count = ((ctl->idx->create >= ctl->idx->remove) ? 0 : (DBG_SVR_IDX_MAX + 1)) + \
                 ctl->idx->create - ctl->idx->remove;
    LOG("count=%04u", ctl->count);
    while (ctl->count-- > ctl->cfg->limit_count)
    {
        ret = dbg_svr_log_remove_oldest(ctl); ERR_NZERO(ret);
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
    DBG_SVR_LOG_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready); ERR_NULL(ctl->fp);

    fseek(ctl->fp, 0 , SEEK_END);
    ctl->sizeMB = ftell(ctl->fp) / (1024 * 1024);
    LOG("sizeMB=%dMB", ctl->sizeMB);
    if (ctl->sizeMB >= ctl->cfg->limit_sizeMB)
    {
        ret = dbg_svr_log_open_next(ctl); ERR_NZERO(ret);
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_log_name_next (
    DBG_SVR_LOG_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready);

    ctl->time_now = time(NULL);
    ctl->time_local = localtime(&(ctl->time_now));
    snprintf(ctl->name, DBG_SVR_LOG_NAME_LEN, DBG_SVR_LOG_NAME_FMT,
             ctl->idx->create,
             ctl->LOCAL_YEAR, ctl->LOCAL_MON, ctl->LOCAL_DAY,
             ctl->LOCAL_HOUR, ctl->LOCAL_MIN, ctl->LOCAL_SEC);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_log_open_next (
    DBG_SVR_LOG_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready);

    ret = dbg_svr_log_name_next(ctl); ERR_NZERO(ret);
    ret = dbg_svr_log_open(ctl); ERR_NZERO(ret);
    ret = dbg_svr_log_write_header(ctl); ERR_NZERO(ret);
    ret = dbg_svr_idx_increase_create(ctl->idx); ERR_NZERO(ret);
    ret = dbg_svr_log_check_count(ctl); ERR_NZERO(ret);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static void
dbg_svr_log_flush (
    DBG_SVR_LOG_CTL * const             ctl)
{   ENTR();
    ERR_NULL(ctl);

    if (ctl->fp != NULL)
    {
        fflush(ctl->fp);
    }

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_log_write_trailer (
    DBG_SVR_LOG_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready); ERR_NULL(ctl->fp);

    ctl->time_now = time(NULL);
    ctl->time_local = localtime(&(ctl->time_now));
    fprintf(ctl->fp, DBG_SVR_LOG_TRAILER_FMT, ctl->dir->path_name, ctl->name);
    fprintf(ctl->fp, DBG_SVR_LOG_TRAILER_TIME_FMT,
                  ctl->LOCAL_YEAR, ctl->LOCAL_MON, ctl->LOCAL_DAY,
                  ctl->LOCAL_HOUR, ctl->LOCAL_MIN, ctl->LOCAL_SEC);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_log_write_header (
    DBG_SVR_LOG_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready); ERR_NULL(ctl->fp);

    ctl->time_now = time(NULL);
    ctl->time_local = localtime(&(ctl->time_now));
    ret = fprintf(ctl->fp, DBG_SVR_IDX_FMT, ctl->idx->create, ctl->idx->remove); WRN_NPOS(ret);
    ret = fprintf(ctl->fp, DBG_SVR_LOG_MAKE_FMT, __DATE__, __TIME__); WRN_NPOS(ret);
    ret = fprintf(ctl->fp, DBG_SVR_LOG_HEADER_FMT, ctl->dir->path_name, ctl->name); WRN_NPOS(ret);
    ret = fprintf(ctl->fp, DBG_SVR_LOG_HEADER_TIME_FMT,
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
static void
dbg_svr_log_close (
    DBG_SVR_LOG_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);

    if (ctl->fp != NULL)
    {
        ret = dbg_svr_log_write_trailer(ctl); WRN_NZERO(ret);

        fflush(ctl->fp);
        fclose(ctl->fp);
        ctl->fp = NULL;
    }

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_log_open (
    DBG_SVR_LOG_CTL * const             ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready);

    if (ctl->fp != NULL)
    {
        dbg_svr_log_close(ctl);
    }
    snprintf(ctl->path_name, DBG_SVR_LOG_PATH_NAME_LEN, "%s/%s",
             ctl->dir->path_name, ctl->name);
    ctl->fp = fopen(ctl->path_name, "w+"); ERR_NULL(ctl->fp);
    LOG("fopen=\"%s\"", ctl->path_name);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static void
dbg_svr_log_config_show (
    DBG_SVR_LOG_CFG * const             cfg)
{   ENTR();
    ERR_NULL(cfg);

    LOG("path=\"%s\"", cfg->path);
    LOG("limit_sizeMB=%uMB", cfg->limit_sizeMB);
    LOG("limit_count=%u", cfg->limit_count);

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_log_config (
    DBG_SVR_LOG_CTL * const             ctl,
    const int                           argc,
    char * const                        argv[])
{   ENTR();
    int                                 ret = -1;
    DBG_SVR_LOG_CFG *                   cfg = NULL;
    int                                 optchar;
    int                                 optindex;
    struct option                       optlist[] =
    {
        {DBG_SVR_LOG_OPTL_PATH,         required_argument,  0, DBG_SVR_LOG_OPTC_PATH},
        {DBG_SVR_LOG_OPTL_SIZEMB,       required_argument,  0, DBG_SVR_LOG_OPTC_SIZEMB},
        {DBG_SVR_LOG_OPTL_COUNT,        required_argument,  0, DBG_SVR_LOG_OPTC_COUNT},
        {0, 0, 0, 0}
    };
    ERR_NULL(ctl); ERR_NPOS(argc); ERR_NULL(argv);

    if (ctl->cfg == NULL)
    {
        MALLOCZ(ctl->cfg, DBG_SVR_LOG_CFG); ERR_NULL(ctl->cfg);
    }
    else
    {
        MEMZ(ctl->cfg, sizeof(DBG_SVR_LOG_CFG));
    }
    cfg = ctl->cfg;

    opterr = 0;
    optind = 0;
    while (1)
    {
        optchar = getopt_long_only(argc, argv, "", optlist, &optindex);
        if (optchar == -1) { break; }

        switch (optchar)
        {
        case DBG_SVR_LOG_OPTC_PATH:
            ERR_OPTARG_INVALID();
            snprintf(cfg->path, GENERAL_PATH_LEN, "%s", optarg);
            cfg->path[GENERAL_PATH_LEN - 1] = '\0';
            INF("path=\"%s\"", cfg->path);
            break;
        case DBG_SVR_LOG_OPTC_SIZEMB:
            ERR_OPTARG_INVALID();
            cfg->limit_sizeMB = strtoul(optarg, NULL, 0);
            INF("limit_sizeMB=%uMB", cfg->limit_sizeMB);
            break;
        case DBG_SVR_LOG_OPTC_COUNT:
            ERR_OPTARG_INVALID();
            cfg->limit_count = strtoul(optarg, NULL, 0);
            INF("limit_count=%u", cfg->limit_count);
            break;
        default:
                break;
        }
    }

    if (strnlen(cfg->path, GENERAL_PATH_LEN) < 1)
    {
        char *                          pwd;
        pwd = getcwd(cfg->path, GENERAL_PATH_LEN); ERR_NULL(pwd);
        cfg->path[GENERAL_PATH_LEN - 1] = '\0';
        INF("path=\"%s\"", cfg->path);
    }

    if ((cfg->limit_sizeMB < DBG_SVR_LOG_SIZEMB_MIN) || (cfg->limit_sizeMB > DBG_SVR_LOG_SIZEMB_MAX))
    {
        cfg->limit_sizeMB = DBG_SVR_LOG_SIZEMB_DFT;
        INF("limit_sizeMB=%uMB", cfg->limit_sizeMB);
    }
    REGULATE(cfg->limit_sizeMB, DBG_SVR_LOG_SIZEMB_MIN, DBG_SVR_LOG_SIZEMB_MAX);

    if ((cfg->limit_count < DBG_SVR_LOG_COUNT_MIN) || (cfg->limit_count > DBG_SVR_LOG_COUNT_MAX))
    {
        cfg->limit_count = DBG_SVR_LOG_COUNT_DFT;
        INF("limit_count=%u", cfg->limit_count);
    }
    REGULATE(cfg->limit_count, DBG_SVR_LOG_COUNT_MIN, DBG_SVR_LOG_COUNT_MAX);

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
static void
dbg_svr_log_help (void)
{   ENTR();

    printf("dbg_svr_log:\r\n");
    printf("-%c/--%s\t%s\r\n",
           DBG_SVR_LOG_OPTC_PATH, DBG_SVR_LOG_OPTL_PATH, DBG_SVR_LOG_OPTS_PATH);
    printf("-%c/--%s\t%s <%d,%d>\r\n",
           DBG_SVR_LOG_OPTC_SIZEMB, DBG_SVR_LOG_OPTL_SIZEMB, DBG_SVR_LOG_OPTS_SIZEMB,
           DBG_SVR_LOG_SIZEMB_MIN, DBG_SVR_LOG_SIZEMB_MAX);
    printf("-%c/--%s\t%s <%d,%d>\r\n",
           DBG_SVR_LOG_OPTC_COUNT, DBG_SVR_LOG_OPTL_COUNT, DBG_SVR_LOG_OPTS_COUNT,
           DBG_SVR_LOG_COUNT_MIN, DBG_SVR_LOG_COUNT_MAX);

LEXIT;
    return;
}
/*························································*/
static void
dbg_svr_log_release (
    DBG_SVR_LOG_CTL * const             ctl)
{   ENTR();
    ERR_NULL(ctl);

    if (ctl->fp != NULL)
    {
        dbg_svr_log_close(ctl);
    }
    if (ctl->cfg != NULL)
    {
        free(ctl->cfg);
    }

    if (ctl->dir != NULL)
    {
        dbg_svr_dir_release(ctl->dir);
    }

    if (ctl->idx != NULL)
    {
        dbg_svr_idx_release(ctl->idx);
    }
    MEMZ(ctl, sizeof(DBG_SVR_LOG_CTL));

LEXIT;
    return;
LERROR;
    GOEXIT;
}
/*························································*/
static int
dbg_svr_log_init (
    DBG_SVR_LOG_CTL * const             ctl,
    const int                           argc,
    char * const                        argv[])
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl); ERR_NPOS(argc); ERR_NULL(argv);

    if (ctl->cfg != NULL)
    {
        dbg_svr_log_release(ctl);
    }
    ret = dbg_svr_log_config(ctl, argc, argv); ERR_NZERO(ret);
    dbg_svr_log_config_show(ctl->cfg);

    ctl->dir = &(ctl->_dir);
    ret = dbg_svr_dir_init(ctl->dir, ctl->cfg->path); ERR_NZERO(ret);

    ctl->idx = &(ctl->_idx);
    ret = dbg_svr_idx_init(ctl->idx, ctl->dir->path_name); ERR_NZERO(ret);

    ctl->ready = true;
    LOG("ready=%s", STRBOOL(ctl->ready));

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}

/*____________________________________________________________________________*/
/* DBG_SVR */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
static int
dbg_svr_test (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    static time_t                       time_now = 0;
    static time_t                       time_last = 0;
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready);

    time_now = time(NULL);
    if (time_now != time_last)
    {
        time_last = time_now;
        LOG("time_now=%u", time_now);
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
int
dbg_svr_loop_job (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready);

    if (ctl->cfg->test_enable)
    {
        ret = dbg_svr_test(ctl); WRN_NZERO(ret);
    }
#if 0
    PAUSE();
#endif

    ret = dbg_svr_recv(ctl); WRN_NZERO(ret);
    if (ret == 0)
    {
        ret = dbg_svr_fprintf(ctl); WRN_NZERO(ret);
    }
#if 0
    PAUSE();
#endif

    ctl->time_now = time(NULL);
    if ((ctl->time_now < ctl->time_last) || ((ctl->time_now - ctl->time_last) >= 60))
    {
        ctl->time_last = ctl->time_now;
        dbg_svr_log_flush(ctl->log);
        ret = dbg_svr_log_check_size(ctl->log); WRN_NZERO(ret);
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
int
dbg_svr_fprintf (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready); ERR_FALSE(ctl->log->ready);

    if ((ctl->cfg->dbgmsg_svr_enable == true) && (ctl->dbgmsg_svr->ready == true))
    {
        ret = dbgmsg_svr_fprintf(ctl->dbgmsg_svr, stdout); WRN_NZERO(ret);
        if (ctl->log->fp != NULL)
        {
            ret = dbgmsg_svr_fprintf(ctl->dbgmsg_svr, ctl->log->fp); WRN_NZERO(ret);
        }
    }

    ret = 0;
LEXIT;
    return(ret);
LERROR;
    GOEXIT;
}
/*························································*/
int
dbg_svr_recv (
    DBG_SVR_CTL * const                 ctl)
{   ENTR();
    int                                 ret = -1;
    ERR_NULL(ctl);
    ERR_FALSE(ctl->ready);

    if ((ctl->cfg->dbgmsg_svr_enable == true) && (ctl->dbgmsg_svr->ready == true))
    {
        printf("here\n");
        ret = dbgmsg_svr_recv(ctl->dbgmsg_svr); WRN_NZERO(ret);
    }

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

    LOG("test_enable=%s", STRBOOL(cfg->test_enable));
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
        {DBG_SVR_OPTL_TEST,             no_argument,        0, DBG_SVR_OPTC_TEST},
        {DBG_SVR_OPTL_DBGMSG_SVR,       no_argument,        0, DBG_SVR_OPTC_DBGMSG_SVR},
        {0, 0, 0, 0}
    };
    ERR_NPOS(argc); ERR_NULL(argv);

    if (ctl->cfg == NULL)
    {
        MALLOCZ(ctl->cfg, DBG_SVR_CFG); ERR_NULL(ctl->cfg);
    }
    else
    {
        MEMZ(ctl->cfg, sizeof(DBG_SVR_CFG));
    }
    cfg = ctl->cfg;

    opterr = 0;
    optind = 0;
    while (1)
    {
        optchar = getopt_long_only(argc, argv, "", optlist, &optindex);
        if (optchar == -1) { break; }

        switch (optchar)
        {
        case DBG_SVR_OPTC_TEST:
            cfg->test_enable = true;
            INF("test_enable=%s", STRBOOL(cfg->test_enable));
            break;
        case DBG_SVR_OPTC_DBGMSG_SVR:
            cfg->dbgmsg_svr_enable = true;
            INF("dbgmsg_svr_enable=%s", STRBOOL(cfg->dbgmsg_svr_enable));
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
dbg_svr_help (void)
{   ENTR();

    printf("dbg_svr:\r\n");
    printf("-%c/--%s\t%s\r\n", DBG_SVR_OPTC_TEST, DBG_SVR_OPTL_TEST, DBG_SVR_OPTS_TEST);
    printf("-%c/--%s\t%s\r\n", DBG_SVR_OPTC_DBGMSG_SVR, DBG_SVR_OPTL_DBGMSG_SVR, DBG_SVR_OPTS_DBGMSG_SVR);

    dbg_svr_log_help();
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

    if (ctl->log != NULL)
    {
        dbg_svr_log_release(ctl->log);
    }
    if (ctl->dbgmsg_svr != NULL)
    {
        dbgmsg_svr_release(ctl->dbgmsg_svr);
    }
    MEMZ(ctl, sizeof(DBG_SVR_CTL));

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

    ctl->log = &(ctl->_log);
    ret = dbg_svr_log_init(ctl->log, argc, argv); ERR_NZERO(ret);
    ret = dbg_svr_log_open_next(ctl->log); ERR_NZERO(ret);
#if 0
    PAUSE();
#endif

    if (ctl->cfg->dbgmsg_svr_enable == true)
    {
        ctl->dbgmsg_svr = &(ctl->_dbgmsg_svr);
        ERR_NULL(ctl->dbgmsg_svr);
        ret = dbgmsg_svr_init(ctl->dbgmsg_svr, argc, argv); ERR_NZERO(ret);
    }

    ctl->time_now = time(NULL);
    ctl->time_last = ctl->time_now;

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
static void
main_signal_action (
    int                                 signum
    )
{   ENTR();
    switch (signum)
    {
    case SIGINT:
        WRN("SIGINT");
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
    struct sigaction                    * sigact = NULL;

    MALLOCZ(sigact, struct sigaction);
    sigact->sa_handler = main_signal_action;
    ret = sigemptyset(&(sigact->sa_mask)); ERR_NZERO(ret);

    ret = sigaction(SIGINT, sigact, NULL); ERR_NZERO(ret);
    LOG("ctrl+c to quit");

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
        usleep(1000);

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

    LOG("gverbose=%u", *gverbose);

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


    LOG("argc=%d", argc);
    for (int i = 1; i < argc; i++)
    {
        LOG("argv[%d]=\"%s\"", i, argv[i]);
    }

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
            main_exit(0);
            break;
        case MAIN_OPTC_VERBOSE:
            ERR_OPTARG_INVALID();
            *gverbose = strtoul(optarg, NULL, 0);
            INF("gverbose=%u", *gverbose);
            break;
        default:
            break;
        }
    }

    REGULATE(*gverbose, MAIN_VERBOSE_MIN, MAIN_VERBOSE_MAX);

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

    ret = main_config(argc, argv); ERR_NZERO(ret);
    main_config_show();
#if 0
    PAUSE();
#endif

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

    *gverbose = MAIN_VERBOSE_DFT;
    LOG("gverbose=%p", gverbose);

    ret = main_signal_init(); ERR_NZERO(ret);

    ERR_NULL(gdbg_clnt);
    MEMZ(gdbg_clnt, sizeof(DBG_CLNT_CTL));
    ret = dbg_clnt_init(gdbg_clnt, argc, argv); ERR_NZERO(ret);
    ret = dbg_clnt_set_src_name(gdbg_clnt, MAIN_SRC_NAME); WRN_NZERO(ret);
#if 0
    PAUSE();
#endif

    ret = main_init(argc, argv); ERR_NZERO(ret);
#if 0
    PAUSE();
#endif

    ret = main_loop(); ERR_NZERO(ret);

    ret = 0;
LEXIT;
    main_exit(ret);
    return(ret);
LERROR;
    GOEXIT;
}

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
