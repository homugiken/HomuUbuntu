#ifndef MAIN_H_
#define MAIN_H_
/*____________________________________________________________________________*/
/* INCLUDE */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#include "debug.h"

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
#define MAIN_VERBOSE_MIN                DBG_VERBOSE_MIN
#define MAIN_VERBOSE_MAX                DBG_VERBOSE_MAX
#define MAIN_VERBOSE_DFT                DBG_VERBOSE_DFT
/*························································*/
#define MAIN_SRC_NAME                   "DBG_SVR"
/*························································*/
#define MAIN_LOOP_STEP_MS               1
#define MAIN_LOOP_STEP_US               (MAIN_LOOP_STEP_MS * 1000)
/*························································*/
static void main_signal_action (int signum);
static int main_signal_init (void);
static int main_loop (void);
static void main_config_show (void);
static int main_config (const int argc, char * const argv[]);
static void main_help (char * const name);
static void main_exit (int ret);
static int main_init (const int argc, char * const argv[]);

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/
#endif /* MAIN_H_ */
