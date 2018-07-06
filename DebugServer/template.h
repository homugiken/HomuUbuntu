/*____________________________________________________________________________*/

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/


/*____________________________________________________________________________*/

/* EOF */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/


/*____________________________________________________________________________*/
#ifndef XXXX_H_
#define XXXX_H_

#endif /* XXXX_H_ */
/* EOF */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/


/*____________________________________________________________________________*/
/* XXXX */
/*------------------------------------*/
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/


/*____________________________________________________________________________*/
/* MAIN */
/*------------------------------------*/
static int
main_loop (void)
{       ENTR();
        int                             ret = -1;

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* main_loop */
/*------------------------------------*/
static int
main_reset (void)
{       ENTR();
        int                             ret = -1;

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* main_reset */
/*------------------------------------*/
static void
main_exit (
        int                             ret
        )
{       ENTR();

exit:   EXIT();
        exit(ret);
        return;
}       /* main_exit */
/*------------------------------------*/
static int
main_init (
        const int                       argc,
        char * const                    argv[])
{       ENTR();
        int                             ret = -1;
        int                             i;
        ERR_NPOS(argc); ERR_NULL(argv);

        ERR_NULL(gctl); MEMZERO(gctl, sizeof(GLOBAL_CTL));
        ERR_NULL(gcfg); MEMZERO(gcfg, sizeof(GLOBAL_CFG));
        ERR_NULL(gstat); MEMZERO(gstat, sizeof(GLOBAL_STAT));

        gstat->pid = getpid();
        gstat->ppid = getppid();
        LOG("pid=%d ppid=%d", gstat->pid, gstat->ppid);
        LOG("argc=%d argv=%p", argc, argv);
        for (i = 0; i < argc; i++)
        {
                LOG("argv[%d]=\"%s\"", i, argv[i]);
        }

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* main_init */
/*------------------------------------*/
int
main (
        const int                       argc,
        char * const                    argv[])
{       ENTR();
        int                             ret = -1;
        ERR_NPOS(argc); ERR_NULL(argv);

        ret = main_init(argc, argv); ERR_NZERO(ret);

        ret = main_loop(); ERR_NZERO(ret);

        ret = 0;
exit:   EXIT();
        main_exit(ret);
        return(ret);
error:  FAIL();
        goto exit;
}       /* main */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/


/*____________________________________________________________________________*/
/* XXXX */
/*------------------------------------*/
static int
XXXX_job (
        XXXX_CTL * const                ctl)
{       ENTR();
        int                             ret = -1;
        ERR_NULL(ctl);

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* XXXX_job */
/*------------------------------------*/
static int
XXXX_loop (
        XXXX_CTL * const                ctl)
{       ENTR();
        int                             ret = -1;
        ERR_NULL(ctl);

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* XXXX_loop */
/*------------------------------------*/
static int
XXXX_reset (
        XXXX_CTL * const                ctl)
{       ENTR();
        int                             ret = -1;
        ERR_NULL(ctl);

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* XXXX_reset */
/*------------------------------------*/
static void
XXXX_release (
        XXXX_CTL * const                ctl)
{       ENTR();
        ERR_NULL(ctl);

exit:   EXIT();
        return;
error:  FAIL();
        goto exit;
}       /* XXXX_release */
/*------------------------------------*/
static int
XXXX_init (
        XXXX_CTL * const                ctl,
        XXXX_CFG * const                cfg)
{       ENTR();
        int                             ret = -1;
        ERR_NULL(ctl); ERR_NULL(cfg);

        ret = 0;
exit:   EXIT();
        return(ret);
error:  FAIL();
        goto exit;
}       /* XXXX_init */
/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*/