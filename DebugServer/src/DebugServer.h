#ifndef DEBUGSERVER_H_
#define DEBUGSERVER_H_
/*____________________________________________________________________________*/
/* INCLUDE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#include "debug.h"

/*____________________________________________________________________________*/
/* DBG_SVR */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define DBG_SVR_VERBOSE_MIN             DBG_VERBOSE_ERR
#define DBG_SVR_VERBOSE_MAX             DBG_VERBOSE_TAG
#define DBG_SVR_VERBOSE_DFT             DBG_VERBOSE_MAX
/*························································*/
#define DBG_SVR_OPTL_LOG_PATH           "path"
#define DBG_SVR_OPTC_LOG_PATH           'p'
#define DBG_SVR_OPTS_LOG_PATH           "log file storage path"
#define DBG_SVR_OPTL_LOG_SIZE           "size"
#define DBG_SVR_OPTC_LOG_SIZE           's'
#define DBG_SVR_OPTS_LOG_SIZE           "log file size limit (MB)"
#define DBG_SVR_OPTL_LOG_COUNT          "count"
#define DBG_SVR_OPTC_LOG_COUNT          'c'
#define DBG_SVR_OPTS_LOG_COUNT          "log file quantity limit "
#define DBG_SVR_OPTL_DBGMSG_SVR         "dbgmsg"
#define DBG_SVR_OPTC_DBGMSG_SVR         'm'
#define DBG_SVR_OPTS_DBGMSG_SVR         "dbgmsg_svr enable"
/*························································*/
#define DBG_SVR_DIR_PATH_LEN            1024
#define DBG_SVR_DIR_PATH_DFT            "./dbg"
/*························································*/
#define DBG_SVR_LOG_HEADER_TIME_FMT     "[TIME|START=%04d/%02d/%02d-%02d:%02d:%02d]\r\n"
#define DBG_SVR_LOG_TRAILER_TIME_FMT    "[TIME|END=%04d/%02d/%02d-%02d:%02d:%02d]\r\n"
/*························································*/
#define DBG_SVR_LOG_NAME_LEN            1024
#define DBG_SVR_LOG_NAME_FMT            "dbg-%04d-%04d%02d%02d-%02d%02d%02d.txt"
#define DBG_SVR_LOG_PATH_NAME_LEN       (DBG_SVR_DIR_PATH_LEN + DBG_SVR_LOG_NAME_LEN)
/*························································*/
#define DBG_SVR_LOG_SIZE_MIN            1
#define DBG_SVR_LOG_SIZE_MAX            10
#define DBG_SVR_LOG_SIZE_DFT            DBG_SVR_LOG_SIZE_MIN
/*························································*/
#define DBG_SVR_LOG_COUNT_MIN           10
#define DBG_SVR_LOG_COUNT_MAX           100
#define DBG_SVR_LOG_COUNT_DFT           DBG_SVR_LOG_COUNT_MIN
/*························································*/
#define DBG_SVR_IDX_NAME_LEN            32
#define DBG_SVR_IDX_NAME_FMT            "dbg-idx.txt"
#define DBG_SVR_IDX_PATH_NAME_LEN       (DBG_SVR_DIR_PATH_LEN + DBG_SVR_IDX_NAME_LEN)
/*························································*/
#define DBG_SVR_IDX_FMT                 "[IDX|CREATE=%04u|DELETE=%04u]\r\n"
#define DBG_SVR_IDX_MIN                 0
#define DBG_SVR_IDX_MAX                 9999
/*························································*/
typedef struct DBG_SVR_CFG {
    char                                dir_path[DBG_SVR_DIR_PATH_LEN];
    uint32_t                            log_size;           /* MB */
    uint32_t                            log_count;
    bool                                dbgmsg_svr_enable;
} DBG_SVR_CFG;
/*························································*/
typedef struct DBG_SVR_CTL {
    bool                                ready;
    DBG_SVR_CFG *                       cfg;
    DBGMSG_SVR_CTL                      _dbgmsg_svr, * dbgmsg_svr;
    time_t                              time_now;
    struct tm *                         time_local;
    char                                log_name[DBG_SVR_LOG_NAME_LEN];
    char                                log_path_name[DBG_SVR_LOG_PATH_NAME_LEN];
    FILE *                              log_fp;
    uint32_t                            log_size_current;   /* MB */
    char                                idx_path_name[DBG_SVR_IDX_PATH_NAME_LEN];
    FILE *                              idx_fp;
    uint32_t                            idx_create;
    uint32_t                            idx_delete;
    DIR *                               dir;
} DBG_SVR_CTL;
/*························································*/
static int dbg_svr_loop_job (DBG_SVR_CTL * const ctl);
static int dbg_svr_recv (DBG_SVR_CTL * const ctl);
/*························································*/
static int dbg_svr_log_delete (DBG_SVR_CTL * const ctl);
static int dbg_svr_log_check_size (DBG_SVR_CTL * const ctl);
static int dbg_svr_log_shift (DBG_SVR_CTL * const ctl);
static void dbg_svr_log_flush (DBG_SVR_CTL * const ctl);
static int dbg_svr_log_fprintf (DBG_SVR_CTL * const ctl);
static int dbg_svr_log_write_trailer (DBG_SVR_CTL * const ctl);
static int dbg_svr_log_write_header (DBG_SVR_CTL * const ctl);
static int dbg_svr_log_open_new (DBG_SVR_CTL * const ctl);
static int dbg_svr_log_name_new (DBG_SVR_CTL * const ctl);
static void dbg_svr_log_close (DBG_SVR_CTL * const ctl);
static int dbg_svr_log_open (DBG_SVR_CTL * const ctl);
/*························································*/
static int dbg_svr_idx_delete_increase (DBG_SVR_CTL * const ctl);
static int dbg_svr_idx_create_increase (DBG_SVR_CTL * const ctl);
static void dbg_svr_idx_flush (DBG_SVR_CTL * const ctl);
static int dbg_svr_idx_write (DBG_SVR_CTL * const ctl);
static int dbg_svr_idx_read (DBG_SVR_CTL * const ctl);
static void dbg_svr_idx_close (DBG_SVR_CTL * const ctl);
static int dbg_svr_idx_open_write (DBG_SVR_CTL * const ctl);
static int dbg_svr_idx_open_read (DBG_SVR_CTL * const ctl);
/*························································*/
static int dbg_svr_mkdir (DBG_SVR_CTL * const ctl);
static void dbg_svr_dir_close (DBG_SVR_CTL * const ctl);
static int dbg_svr_dir_open (DBG_SVR_CTL * const ctl);
/*························································*/
static void dbg_svr_config_show (DBG_SVR_CFG * const cfg);
static int dbg_svr_config (DBG_SVR_CTL * const ctl, const int argc, char * const argv[]);
void dbg_svr_help (void);
void dbg_svr_release (DBG_SVR_CTL * const ctl);
int dbg_svr_init (DBG_SVR_CTL * const ctl, const int argc, char * const argvp[]);

/*____________________________________________________________________________*/
/* MAIN */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#define MAIN_OPTL_HELP                  "help"
#define MAIN_OPTC_HELP                  'h'
#define MAIN_OPTS_HELP                  "show help info"
#define MAIN_OPTL_VERBOSE               "verbose"
#define MAIN_OPTC_VERBOSE               'v'
#define MAIN_OPTS_VERBOSE               "set verbosity level"
/*························································*/
static int main_loop (void);
static void main_config_show (void);
static int main_config (const int argc, char * const argv[]);
static void main_help (char * const name);
static void main_exit (int ret);
static int main_init (const int argc, char * const argv[]);

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#endif /* DEBUGSERVER_H_ */
