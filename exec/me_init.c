// me_init.c: S.W. Ellingson, Virginia Tech, 2012 Jul 05
// ---
// COMPILE: gcc -o me_init me_init.c -I../common
// ---
// COMMAND LINE: me_init <flagset> 
//   <flagset>:  integer passed to me_exec, containing flags.  Default is zero.  
//               see me_exec for details.
// ---
// REQUIRES: 
// ---
// Initialization for MCS/Scheduler
// Assumes ssmif.dat is on TP and tries to copy/read it
// See end of this file for history.

#include <stdlib.h>

#include "me.h"

#define MY_NAME "me_init (v.20180222.1)"

int main ( int narg, char *argv[] ) {

  /*=================*/
  /*=== Variables ===*/
  /*=================*/

  /* command line parameters */
  char filename_ssmif[ME_FILENAME_MAX_LENGTH];
  long unsigned int flagset;

  int err;
  pid_t me_exec_pid;  /* PID for me_exec process */
  pid_t me_tpcom_pid; /* PID for me_tpcom process */
  pid_t me_inproc_pid; /* PID for me_inproc process */

  char sFlagset[256];
  char sMeTpcomPid[256];
  char sMeInprocPid[256];

  FILE* fpl; /* log file */
  struct me_session_queue_struct *sq_ptr=NULL;  /* used as dummy argument in me_log() */
  char msg[ME_LOG_MAX_MSG_LENGTH]; 

  char cmd[256]; 

  struct timeval tv;
  struct sdm_struct sdm;

  int i,j;

  struct stat statbuf;
  int bFlg_NoSch = 0; /* ME_FLAG_NO_SCH flag */

  /*=================================*/
  /*=== Process Command Line Args ===*/
  /*=================================*/
    
  /* First, announce thyself */
  printf("[%d/%d] I am %s\n",ME_INIT,getpid(),MY_NAME);

  /* process command line arguments */
  if (narg<2) {
    printf("[%d/%d] me_init: FATAL: flagset not specified\n",ME_INIT,getpid());
    return 1;
    }
  sscanf(argv[1],"%lu",&flagset);
  //printf("[%d/%d] Input: flagset='%lu'\n",ME_INIT,getpid(),flagset);

  /*=======================*/
  /*=== Killing zombies ===*/
  /*=======================*/
  printf("[%d/%d] Killing zombies...\n",ME_INIT,getpid());
  system("killall -v me_tpcom 2> /dev/null");
  system("killall -v me_inproc 2> /dev/null");
  system("killall -v me_exec 2> /dev/null");

  /*=======================*/
  /*=== Directory Setup ===*/
  /*=======================*/
  printf("[%d/%d] me_init: Setting up directories...\n",ME_INIT,getpid());

  /* me_exec directories */
  system("mkdir sinbox 2> /dev/null");  /* create "sinbox" directory; if it already exists, no problem */
  system("rm sinbox/* 2> /dev/null");   /* clean out sinbox directory */
  system("mkdir state 2> /dev/null");   /* create "state" directory; if it already exists, no problem */
  system("rm state/* 2> /dev/null");    /* clean out state directory */
  system("mkdir soutbox 2> /dev/null"); /* create "soutbox" directory; if it already exists, no problem. */
                                        /* if it exists and has files in it, files are preserved and */
                                        /* remain available for me_tpcom to push */

  /* me_tpcom directories */
  system("mkdir tpcom 2> /dev/null");      /* me_tpcom working directory */
  system("rm tpcom/* 2> /dev/null");       /* clean it out */
  system("mkdir tpcom/temp 2> /dev/null");         /* temp directory for assembling files into tgz. */

  /*=====================*/
  /*=== open log file ===*/
  /*=====================*/
  fpl = fopen(ME_LOG_FILENAME,"w");
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: Directory setup complete...", sq_ptr, 0 );

  /******************************************/
  /*** look at flagset **********************/
  /******************************************/
  sprintf(msg,"me_init: flagset = %lu",flagset);
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 ); 
  bFlg_NoSch = ( flagset & ME_FLAG_NO_SCH ) != 0;

  /*================================*/
  /*=== Copying ssmif.dat from TP ===*/
  /*================================*/
  printf("[%d/%d] Copying ssmif.dat from TP...\n",ME_INIT,getpid());
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: Copying ssmif.dat from TP...", sq_ptr, 0 );
  sprintf(cmd, "scp %s:%s/ssmif.dat state/.",LWA_TP_SCP_ADDR,LWA_TP_SCP_DIR);
  //printf("'%s'\n",cmd);
  system(cmd);

  // Code in this block was replaced with code from next block on 120705
  ///*==================================================*/
  ///*=== Copying default.gf from TP to Exec and Sch ===*/
  ///*==================================================*/
  //printf("[%d/%d] Copying default.gf from TP...\n",ME_INIT,getpid());
  //me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: Copying default.gf from TP...", sq_ptr, 0 );
  //sprintf(cmd, "scp %s:%s/default.gf state/.",LWA_TP_SCP_ADDR,LWA_TP_SCP_DIR);
  ////printf("'%s'\n",cmd);
  //system(cmd);
  //if (stat("state/default.gf",&statbuf)!=0) { /* it's missing, transfer failed (perhaps file was missing on TP) */
  //  printf("[%d/%d] me_init: FATAL: transfer of 'default.gf' from TP failed.\n",ME_INIT,getpid());
  //  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: FATAL: transfer of 'default.gf' from TP failed.", sq_ptr, 0 );
  //  fclose(fpl);
  //  return 1;
  //  }
  ///* now move this on to scheduler */
  //if (!bFlg_NoSch) {
  //  printf("[%d/%d] Copying default.gf to Scheduler...\n",ME_INIT,getpid());
  //  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: Copying default.gf to Scheduler...", sq_ptr, 0 );
  //  sprintf(cmd, "scp state/default.gf %s:%s/gfiles/.",LWA_SCH_SCP_ADDR,LWA_SCH_SCP_DIR);
  //  //printf("'%s'\n",cmd);
  //  system(cmd);
  //  }

  /*==================================================*/
  /*=== Copying default.gft from TP to Exec ==========*/
  /*==================================================*/
  printf("[%d/%d] Copying default.gft from TP...\n",ME_INIT,getpid());
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: Copying default.gft from TP...", sq_ptr, 0 );
  sprintf(cmd, "scp %s:%s/default.gft state/.",LWA_TP_SCP_ADDR,LWA_TP_SCP_DIR);
  //printf("'%s'\n",cmd);
  system(cmd);
  if (stat("state/default.gft",&statbuf)!=0) { /* it's missing, transfer failed (perhaps file was missing on TP) */
    printf("[%d/%d] me_init: FATAL: transfer of 'default.gft' from TP failed.\n",ME_INIT,getpid());
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: FATAL: transfer of 'default.gft' from TP failed.", sq_ptr, 0 );
    fclose(fpl);
    return 1;
    }

  /*======================================*/
  /*== find the mimimum beamformer delay =*/
  /*======================================*/
  printf("[%d/%d] me_init: Setting station minimum beamformer delay with ./medrange...\n",ME_INIT,getpid());
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: Setting station minimum beamformer delay with ./medrange...", sq_ptr, 0 );
  i = system("./medrange state 2> /dev/null");
  if( i != 0 ) {
    printf("[%d/%d] me_init: WARNING: failed to set station minimum beamformer delay\n",ME_INIT,getpid());
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: WARNING: failed to set station minimum beamformer delay", sq_ptr, 0 );
    }

  /*==========================*/
  /*=== Launching me_tpcom ===*/
  /*==========================*/
  printf("[%d/%d] me_init: Launching ./me_tpcom...\n",ME_INIT,getpid());
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: Launching ./me_tpcom...", sq_ptr, 0 );

  /* Launch me_tpcom */
  me_tpcom_pid = fork();  /* create duplicate process */
  switch (me_tpcom_pid) {
    case -1: /* error */
      printf("[%d/%d] me_init: FATAL: fork for ./me_tpcom failed\n",ME_INIT,getpid());
      me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: FATAL: fork for ./me_tpcom failed", sq_ptr, 0 );
      return 1;
      break;
    case 0: /* fork() succeeded; we are now in the child process */
      err = execl("./me_tpcom","me_tpcom",NULL); /* launch me_tpcom */
      /* if we get to this point then we failed to launch */
      if (err==-1) {
        printf("[%d/%d] me_init: FATAL: failed to execl() ./me_tpcom\n",ME_INIT,getpid()); 
        me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: FATAL: failed to execl() ./me_tpcom", sq_ptr, 0 );
        }     
      return 2;
      break; 
    default: /* fork() succeeded; we are now in the parent process */
      break;
    } /* switch (me_tpcom_pid) */

  /*==========================*/
  /*=== Launching me_inproc ===*/
  /*==========================*/
  printf("[%d/%d] me_init: Launching ./me_inproc...\n",ME_INIT,getpid());
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: Launching ./me_inproc...", sq_ptr, 0 );

  /* Launch me_inproc */
  me_inproc_pid = fork();  /* create duplicate process */
  switch (me_inproc_pid) {
    case -1: /* error */
      printf("[%d/%d] me_init: FATAL: fork for ./me_inproc failed\n",ME_INIT,getpid());
      me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: FATAL: fork for ./me_inproc failed", sq_ptr, 0 );
      return 1;
      break;
    case 0: /* fork() succeeded; we are now in the child process */
      err = execl("./me_inproc","me_inproc",NULL); /* launch me_inproc */
      /* if we get to this point then we failed to launch */
      if (err==-1) {
        printf("[%d/%d] me_init: FATAL: failed to execl() ./me_inproc\n",ME_INIT,getpid()); 
        me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: FATAL: failed to execl() ./me_inproc", sq_ptr, 0 );
        }     
      return 2;
      break; 
    default: /* fork() succeeded; we are now in the parent process */
      break;
    } /* switch (me_inproc_pid) */

  /*======================================*/
  /*=== initialize station dynamic MIB ===*/
  /*======================================*/

  gettimeofday(&tv,NULL); /* sometimes this goofs first time it is called in a process */
  gettimeofday(&tv,NULL);

  /* SUMMARY variables */
  sdm.station.summary = LWA_SIDSUM_BOOTING; strcpy(sdm.station.info,""); sdm.station.tv = tv; /* station */
  sdm.shl.summary     = LWA_SIDSUM_UNK;     strcpy(sdm.shl.info,    ""); sdm.shl.tv     = tv; /* SHL */
  sdm.asp.summary     = LWA_SIDSUM_UNK;     strcpy(sdm.asp.info,    ""); sdm.asp.tv     = tv; /* ASP */
  sdm.ndp.summary     = LWA_SIDSUM_UNK;     strcpy(sdm.ndp.info,    ""); sdm.ndp.tv     = tv; /* NDP */
  for (i=0;i<ME_MAX_NDR;i++) {
    sdm.dr[i].summary = LWA_SIDSUM_UNK; strcpy(sdm.dr[i].info,""); sdm.dr[i].tv   = tv; /* DR# */
    }

  /* setting all STAT variables to "not installed" */
  for (i=0;i<ME_MAX_NFEE;i++) { sdm.ssss.eFEEStat[i] = 0; } /* FEE_STAT[] */
  for (i=0;i<ME_MAX_NRPD;i++) { sdm.ssss.eRPDStat[i] = 0; } /* RPD_STAT[] */
  for (i=0;i<ME_MAX_NSEP;i++) { sdm.ssss.eSEPStat[i] = 0; } /* SEP_STAT[] */
  for (i=0;i<ME_MAX_NARB;i++) { 
    for (j=0;j<ME_MAX_NARBCH;j++) { 
      sdm.ssss.eARBStat[i][j] = 0; } }                   /* ARB_STAT[][] */
  for (i=0;i<ME_MAX_NSNAP;i++) { 
    for (j=0;j<ME_MAX_NSNAPCH;j++) { 
      sdm.ssss.eSnapStat[i][j] = 0; } }                   /* SNAP_STAT[][] */
  for (i=0;i<ME_MAX_NDR;i++)  { sdm.ssss.eDRStat[i]  = 0;  } /* DR_STAT[] */

  /* setting all status to "not installed" */
  for (i=0;i<ME_MAX_NSTD;i++) { 
    for (j=0;j<2;j++) {    
      sdm.ant_stat[i][j] = 0; } }
  for (i=0;i<ME_MAX_NDR;i++)  { 
      sdm.ndpo_stat[i]    = 0;  } 

  sdm.settings.mrp_asp = 0;
  sdm.settings.mrp_ndp = 0;
  sdm.settings.mrp_dr1 = 0;
  sdm.settings.mrp_dr2 = 0;
  sdm.settings.mrp_dr3 = 0;
  sdm.settings.mrp_dr4 = 0;
  sdm.settings.mrp_dr5 = 0;
  sdm.settings.mrp_shl = 0;
  sdm.settings.mrp_mcs = 0;

  sdm.settings.mup_asp = 0;
  sdm.settings.mup_ndp = 0;
  sdm.settings.mup_dr1 = 0;
  sdm.settings.mup_dr2 = 0;
  sdm.settings.mup_dr3 = 0;
  sdm.settings.mup_dr4 = 0;
  sdm.settings.mup_dr5 = 0;
  sdm.settings.mup_shl = 0;
  sdm.settings.mup_mcs = 0;

  for (i=0;i<ME_MAX_NSTD;i++) { 
    sdm.settings.fee[i] = -1;
    sdm.settings.asp_flt[i] = -1;
    sdm.settings.asp_at1[i] = -1;
    sdm.settings.asp_at2[i] = -1;
    sdm.settings.asp_ats[i] = -1;
    }
  sdm.settings.drx_gain = -1;

  /* write it */
  me_sdm_write( &sdm );

  /* let me_tpcom push a *copy* to TP */
  system("cp state/sdm.dat soutbox/. 2> /dev/null");

  printf("[%d/%d] me_init: station dynamic MIB initialized\n",ME_INIT,getpid());
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: station dynamic MIB initialized", sq_ptr, 0 );

  /* close down log so that we have no problems when me_exec tries to take it over */
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: Ready to launch me_exec", sq_ptr, 0 );
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init: Closing log in preparation for handoff", sq_ptr, 0 );
  fclose(fpl);
  sleep(1); /* make sure log file is stable before me_exec tries to use it */

  /*==========================*/
  /*=== Launching me_exec ====*/
  /*==========================*/
  printf("[%d/%d] me_init: Launching ./me_exec...\n",ME_INIT,getpid());

  /* flagset passed as a string so that we can use execl() */
  sprintf(sFlagset,"%lu",flagset);

  /* Remember me_tpcom's pid so that me_exec can kill it */
  sprintf(sMeTpcomPid,"%d",me_tpcom_pid);

  /* Remember me_inproc's pid so that me_exec can kill it */
  sprintf(sMeInprocPid,"%d",me_inproc_pid);

  /* Launch executive (the me_exec process) */
  me_exec_pid = fork();  /* create duplicate process */
  switch (me_exec_pid) {
    case -1: /* error */
      printf("[%d/%d] FATAL: fork for ./me_exec failed\n",ME_INIT,getpid());
      return 1;
    case 0: /* fork() succeeded; we are now in the child process */
      err = execl("./me_exec","me_exec",sFlagset,sMeTpcomPid,sMeInprocPid,NULL); /* launch me_exec */
      /* if we get to this point then we failed to launch */
      if (err==-1) {
        printf("[%d/%d] me_init: FATAL: failed to execl('./me_exec',...)\n",ME_INIT,getpid()); 
        }     
      return 2;
      break; 
    default: /* fork() succeeded; we are now in the parent process */
      break;
    } /* switch (me_exec_pid) */

  /* This allows messages from me_exec and me_tpcom to complete without borking the command prompt: */
  sleep(3);

  /* say goodbye */
  printf("[%d/%d] me_init: I'm done. Exiting normally.\n",ME_INIT,getpid());  

  exit(EXIT_SUCCESS);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// me_init.c: J. Dowell, UNM, 2021 Jan 25
//   .1 Updated to call medrange as part of the setup
// me_init.c: J. Dowell, UNM, 2018 Feb 22
//   .1 Updated the exit code so that we know if me_init was successful or not
// me_init.c: J. Dowell, UNM, 2018 Jan 29
//   .1 Cleaned up a few compiler warnings
// me_init.c: J. Dowell, UNM, 2015 Aug 28
//   .1 Updated for ADP.
// me_init.c: S.W. Ellingson, Virginia Tech, 2012 Jul 05
//   .1 Previously, default.gf  was copied from tp/mbox to exec/state and sch/gfiles
//      Now,        default.gft is  copied from tp/mbox to exec/state (only)
//      (me_exec modifies this file, compiles it to a .gf, and sends it to sch/gfiles)
// me_init.c: S.W. Ellingson, Virginia Tech, 2011 Mar 25
//   .1 Added command line parameter directing "Scheduler-less" operation
// me_init.c: S.W. Ellingson, Virginia Tech, 2011 Mar 17
//   .1 Now launching me_inproc 
// me_init.c: S.W. Ellingson, Virginia Tech, 2011 Mar 11
//   .1 Initial version; using ms_init as a starting point
// ms_init.c: S.W. Ellingson, Virginia Tech, 2010 Oct 16
//   .1 Now launching ms_mdre_ip as a standalong process concurrent with ms_exec
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Aug 02
//   .1 Cleaning up console messages (svn rev 22) (svn rev 23)
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Jul 21
//   .1 implementing expanded command line for dat2dbm (IP_ADDRESS etc.)
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Jul 20
//   .1 init file can now have >1 argument
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Jul 17
//   .1 change from mq_struct to LWA_cmd_struct for message passing
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Jul 13
//   .1 bringing into common codeset; mib.h -> LWA_MCS.h
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Jul 07 
//   .1 expanding use of message queues
//   .2 mc_mcic handshake using message queue
//   .3 implementing null subsystems
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Jul 05 
//   -- adding mibinit command
//   -- added message queue (very preliminary)
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Jul 01
//   -- ms_exec now forks; ms_init stays alive 
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Jun 27 

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
