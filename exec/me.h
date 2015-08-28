
#ifndef ME_H  /* this keeps this header from getting rolled in more than once */
#define ME_H

/*************************************/
/*** WARNING: COMPILE-TIME OPTION: ***/
/*************************************/
/* uncomment the line below if you want to use cp to transfer files */
/* uncommented by J. Dowell 2014/2/25 */
#define ME_SCP2CP /* WARNING: assumes tp, exec, and sch are on same machine! */

#include "mcs.h"

#define ME_ACTION_WINDOW 1000 /* number of milliseconds prior to scheduled time */
                              /* that me_exec will consider sending a command to scheduler */

/* flags within the command line parameter "flagset" */
/* see header of me_exec.c for details */
#define ME_FLAG_NO_SCH 1 

#define ME_FILENAME_MAX_LENGTH 256
#define ME_MAX_COMMAND_LINE_LENGTH 256
#define ME_MAX_PROJECT_ID_LENGTH 9 /* set by MCS0030 */
#define ME_OUTCOME_STRING_LENGTH 40

/* ME_MAX_* moved to mcs.h */

/* Software module self-identification codes */
#define ME_ME_H    100 /* me.h (this file) */
#define ME_MESI    101 /* mesi() from mesi.c */
#define ME_MESIX   102 /* main() from mesix.c */
#define ME_MEOS    103 /* meos() from meos.c */
#define ME_MEOSX   104 /* main() from meosx.c */
#define ME_MEMDRE  105 /* main() from meosx.c */
#define ME_MEMDREX 106 /* main() from memdrex.c */
#define ME_ME_C    107 /* main() from me.c */
#define ME_MERS    108 /* main() from mers.c */
#define ME_MEEI    109 /* main() from meeix.c */
#define ME_MEEIX   110 /* main() from meeix.c */
#define ME_INIT    111 /* main() from me_init.c */
#define ME_TPCOM   112 /* main() from me_tpcom.c */
#define ME_INPROC  113 /* main() from me_inproc.c */
#define ME_MEFSDFG 114 /* main() from mefsdfg.c */

#include "mesi.c"
#include "memdre.c"
#include "meos.c"

/**************************************/
/*** me_sc_init( ) ********************/
/**************************************/

void me_sc_init( struct sc_struct *sc ) {
  /*  initializes the station structure (sc) to reasonable values */

  int i,k;

  for ( i=0; i<ME_MAX_NSTD; i++) {
    sc->Stand[i].fLx=0.0;
    sc->Stand[i].fLy=0.0;
    sc->Stand[i].fLz=0.0;
    for ( k=0; k<2; k++ ) {
      sc->Stand[i].Ant[k].iSS=0;
      sc->Stand[i].Ant[k].iID = -1;
      sc->Stand[i].Ant[k].iStat = 0;
      sc->Stand[i].Ant[k].fTheta = 0.0;
      sc->Stand[i].Ant[k].fPhi = 0.0;
      sc->Stand[i].Ant[k].eDesi = -1;
      sc->Stand[i].Ant[k].FEE.i = -1;
      sc->Stand[i].Ant[k].FEE.p = -1;
      sc->Stand[i].Ant[k].FEE.iStat = -1;
      sc->Stand[i].Ant[k].RPD.i = -1;
      sc->Stand[i].Ant[k].RPD.iStat = 0;
      sc->Stand[i].Ant[k].SEP.i = -1;
      sc->Stand[i].Ant[k].SEP.iStat = 0;
      sprintf(sc->Stand[i].Ant[k].SEP.sID,"");
      sc->Stand[i].Ant[k].ARX.i = -1;
      sc->Stand[i].Ant[k].ARX.c = -1;
      sc->Stand[i].Ant[k].ARX.iStat = 0;
      sc->Stand[i].Ant[k].DP.i = -1;
      sc->Stand[i].Ant[k].DP.c = -1;
      sc->Stand[i].Ant[k].DP.iStat = 0;
      }
    }
  for (i=0; i<ME_MAX_NDPOUT; i++) {
    sc->DPO[i].iStat = 0;
    sc->DPO[i].iDR = 0;
    }

  return;
  } /* me_sc_init() */


/************************************************/
/*** MCS/Exec ("me") TCP/IP command interface ***/
/************************************************/

#define ME_CMD_ERR -1 /* this returned by me_exec if command not understood */
#define ME_CMD_NUL 0  /* null (invalid command) */
#define ME_CMD_SHT 1  /* shutdown command */
#define ME_CMD_STP 2  /* stop command -- terminates a session after it's been submitted */
struct me_cmd_struct {
  int cmd;                               /* one of ME_CMD_* */
  char args[ME_MAX_COMMAND_LINE_LENGTH]; /* depends on cmd */
  };

/************************************************/
/*** MCS/Exec ("me") to me_tpcom interface ******/
/************************************************/

struct me_outproc_manifest_struct {
//  char SDF[ME_FILENAME_MAX_LENGTH]; /* name of SDF (.txt) file */
//  char SSF[ME_FILENAME_MAX_LENGTH]; /* name of SSF (.ses) file */
//  char SMF[ME_FILENAME_MAX_LENGTH]; /* name of SMF (_metadata.txt) file */
//  char IPL[ME_FILENAME_MAX_LENGTH]; /* name of (.ipl) file */
//  char CS[ME_FILENAME_MAX_LENGTH]; /* name of (.cs) file */
  struct ssf_struct ssf;
  };

/*******************************************************************/
/*** session queue definition **************************************/
/*******************************************************************/

#define ME_MAX_SESSION_QUEUE_LENGTH 100
/* these are defined values for .eState: */
#define MESQ_STATE_NOT_USED               0 /* this position in queue not currently used */ 
#define MESQ_STATE_AWAITING_INPROCESSING  1 /* this session needs to be inprocessed (not yet scheduled) */
#define MESQ_STATE_INPROCESSING           2 /* inprocessing underway (waiting for external process) (not yet scheduled) */
#define MESQ_STATE_READY                  3 /* this session is ready; under command script control */
#define MESQ_STATE_OBSERVING              4 /* this session is underway; under command script control */
#define MESQ_STATE_AWAITING_OUTPROCESSING 5 /* done, no longer under command script control, awaiting end-of-session processing */
/* these are defined values for .oc: */
#define ME_OC_OK       0 /* concluded successfully with no issues to report */
#define ME_OC_RCVD_ESF 1 /* received a *commanded* (from me_inproc) "end session -- failed" */
#define ME_OC_MEEI_STP 2 /* session terminated as a result of a "STP" command received from meei() */

struct me_session_queue_struct {
  int N;               /* highest index containing an valid observation */
  int bInProcessReqd;  /* = 1 if at least one session is MESQ_STATE_AWAITING_INPROCESSING; otherwise 0 */
  int bInProc;         /* = 1 if at least one session is MESQ_STATE_INPROCESSING; otherwise 0 */
  int bReady;          /* = 1 if at least one session is MESQ_STATE_READY or OBSERVING; otherwise 0 */
  int bOutProcessReqd; /* = 1 if at least one session is MESQ_STATE_AWAITING_OUTPROCESSING; otherwise 0 */  
  /* rest of this structure is a list of sessions */
  int  eState[ME_MAX_SESSION_QUEUE_LENGTH];                               /* see MESQ_STATE_* */  
  int  iCurrentObs[ME_MAX_SESSION_QUEUE_LENGTH];                          /* 0, or # of currently-running observation */      
  char sFilename[ME_MAX_SESSION_QUEUE_LENGTH][ME_FILENAME_MAX_LENGTH];    /* name of .ses file */ 
  FILE* fpc[ME_MAX_SESSION_QUEUE_LENGTH];                                 /* pointer to command script file */
  long int ncr[ME_MAX_SESSION_QUEUE_LENGTH];                              /* number of commands read from command script file */  
  int oc[ME_MAX_SESSION_QUEUE_LENGTH];                                    /* last session outcome code; see ME_OC_* */
  char ocs[ME_MAX_SESSION_QUEUE_LENGTH][ME_OUTCOME_STRING_LENGTH];        /* last session outcome code; elaboration */
  char sTag[ME_MAX_SESSION_QUEUE_LENGTH][18];                             /* used to save OP_TAG of recording */
  struct ssf_struct ssf[ME_MAX_SESSION_QUEUE_LENGTH];                     /* SSF for this session */
  };

/*******************************************************************/
/*** me_log() ******************************************************/
/*******************************************************************/
/* add an entry to the meelog */

#define ME_LOG_FILENAME "state/meelog.txt"

#define ME_LOG_SCOPE_NONSPECIFIC 0 /* doesn't pertain to a session, so don't show session-specific info */
#define ME_LOG_SCOPE_SESSION     1 /* pertains to a session, so show project/session/obs/state info */

#define ME_LOG_TYPE_INFO         0 /* info only */
#define ME_LOG_TYPE_SCH_CMD      1 /* notification of a command sent to MCS/Scheduler */ 

#define ME_LOG_MAX_MSG_LENGTH   16384 /* max length of the message field of a log line */

int me_log( FILE *fp,                          /* handle to an open log file */
            int eScope,                        /* see ME_LOG_SCOPE_* */
            int eType,                         /* see ME_LOG_TYPE_*  */
            char *msg,                         /* message field contents */
            struct me_session_queue_struct *sq, 
            int i                              /* session queue entry to reference */
            ) {

  char line[ 40+ME_LOG_MAX_MSG_LENGTH ]; 
  long int mjd,mpm;
  
  LWA_time(&mjd,&mpm);
  sprintf(line,"%5ld %8ld ",mjd,mpm);
  switch (eScope) {
    case    ME_LOG_SCOPE_SESSION: sprintf(line,"%sS",line); break;
    default:                      sprintf(line,"%sG",line); break;
    }
  switch (eType) {
    case    ME_LOG_TYPE_SCH_CMD:  sprintf(line,"%sC ",line); break;
    default:                      sprintf(line,"%sI ",line); break;
    }
  if (eScope==ME_LOG_SCOPE_SESSION) {
    sprintf(line,"%s %8s %4u %1d %4d",line,
                     sq->ssf[i].PROJECT_ID,
                         sq->ssf[i].SESSION_ID,
                             sq->eState[i],
                                 sq->iCurrentObs[i]);
    } else {
    sprintf(line,"%s                     ",line);
    }
  sprintf(line,"%s %s",line,msg);
  fprintf(fp,"%s\n",line);

  fflush(fp);

  return 0;
  }

/**************************************/
/*** me_bSubsystemAlive( ) ************/
/**************************************/

/* Return error codes */
#define ME_BSA_OK                 0
#define ME_BSA_INVALID_SS         1
#define ME_BSA_MEMDRE_FAIL        2
#define ME_BSA_MESI_FAIL          4
#define ME_BSA_MEMDRE_FAIL_2      8
#define ME_BSA_SUMMARY_NOT_NORMAL 16
#define ME_BSA_DIDNT_UPDATE       32

int me_bSubsystemAlive( char *ss ) {
  /* ss (input) subsystem three-letter designator */
  /* returns an error code ME_SUBSYSTEM_* indicating if subsystem is alive and responding */ 

  char val[MIB_VAL_FIELD_LENGTH];
  struct timeval t0,t1;
  long int mjd0,mjd1;
  long int mpm0,mpm1;
  int err;

  int eResult = ME_BSA_OK;
  long int reference = 0;

  /* check if 'ss' is a valid subsystem */
  /* FIXME -- ME_BSA_INVALID_SS */

  /* determine when SUMMARY field was last updated -> t0 */
  err = memdre( ss, "SUMMARY", val, &t0 );                    /* ask scheduler */ 
  if (err!=0) {
    eResult += ME_BSA_MEMDRE_FAIL; 
    return eResult;
    }
  LWA_timeval(&t0,&mjd0,&mpm0);                               /* convert t0 from timeval to MJD/MPM */
  //printf("[%d/%d] val='%s', mjd0='%ld', mpm0='%ld'\n",ME_ME_H,getpid(),val,mjd0,mpm0); 

  /* send a PNG command; this should update SUMMARY */
  err = mesi( NULL, ss, "PNG", "", "today", "asap", &reference); 
  if (err!=0) { 
    eResult += ME_BSA_MESI_FAIL;
    return eResult;
    }
  sleep(3); /* wait for subsystem & scheduler to process */

  /* determine value of SUMMARY field now, and time of last update */
  err = memdre( ss, "SUMMARY", val, &t1 );                    /* ask scheduler */
  if (err!=0) {
    eResult += ME_BSA_MEMDRE_FAIL_2; 
    return eResult;
    }
  LWA_timeval(&t1,&mjd1,&mpm1);                               /* convert t0 from timeval to MJD/MPM */
  //printf("[%d/%d] val='%s', mjd1='%ld', mpm1='%ld'\n",ME_ME_H,getpid(),val,mjd1,mpm1);

  /* SUMMARY should be "NORMAL"... */
  if (strncmp(val,"NORMAL",6)!=0) {
    //printf("[%d/%d] SUMMARY is now '%s'\n",val),ME_ME_H,getpid(),val);
    eResult += ME_BSA_SUMMARY_NOT_NORMAL;
    }

  /* mpm1 should be different from mpm0; that is, SUMMARY should have been updated */
  if (mpm1==mpm0) {
    //printf("[%d/%d] SUMMARY did not update\n",ME_ME_H,getpid());
    eResult += ME_BSA_DIDNT_UPDATE; 
    }

  return eResult;
  } /* function bSubsystemAlive( ) */


/* me_GetMIBEntry error codes */
#define ME_GETMIBENTRY_OK          0
#define ME_GETMIBENTRY_MESI_FAIL   1
#define ME_GETMIBENTRY_MEMDRE_FAIL 2

int me_GetMIBEntry( char *ss,                        /* (input) subsystem TLA */ 
                    char *mib_label,                 /* (input) MIB label */
                    long int mib_delay_us,           /* (input) [ms] how long to wait for MIB to update */
                                                     /*         0 = do not request update; just read it */
                    char *val,                       /* (output) MIB entry */ 
                    struct timeval *t0               /* (output) Last update of MIB entry */
                  ) {

  int err;
  long int reference;
  int eResult = ME_GETMIBENTRY_OK;

  if (mib_delay_us!=0) {

    err = mesi( NULL, ss, "RPT", mib_label, "today", "asap", &reference ); /* command update of MIB entry */ 
    if (err!=0) {
      printf("[%d/%d] FATAL: In me_GetMIBEntry, mesi(NULL,'%s','RPT','%s',...) returned code %d, ref='%ld'\n",
             ME_ME_H,getpid(),ss,mib_label,err,reference);  
      eResult += ME_GETMIBENTRY_MESI_FAIL; 
      return eResult;
      }

    usleep(mib_delay_us); /* Avoid 'Failed to open dbm' problem; allow time to update MIB */
  
    }

  err = memdre( ss, mib_label, val, t0 ); /* check MIB */ 
  if (err!=0) {
    printf("[%d/%d] FATAL: In me_GetMIBEntry, memdre('%s','%s',...) returned code %d, val='%s'\n",
           ME_MEOS,getpid(),ss,mib_label,err,val);  
    eResult += ME_GETMIBENTRY_MEMDRE_FAIL; 
    return eResult;
    }

  return eResult;
  }  


#endif // #ifndef MCS_H 
