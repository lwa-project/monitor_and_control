// meos.c: J. Dowell, UNM, 2015 Sep 11
// ---
// REQUIRES: 
//   me.h
//   mesi.c
//   memdre.c
// ---
// This implements the function meos() and is intended to be 
//   #include'd with another file that is calling it.  
// Used to do one-shot acquisitions.
// See end of this file for history.

#include "me.h"

/* error codes returned by meos() */
#define MEOS_ERR_OK                             0 /* OK */
#define MEOS_ERR_INVALID_MODE                   1 /* invalid mode (i.e., not TBW, TBN, or DRX) specified */
#define MEOS_ERR_MESI_MCSS                      2 /* MCS/Scheduler not responding as expected */
#define MEOS_ERR_BSA_DP                         4 /* me_bSubsystemAlive("DP_") failed */
#define MEOS_ERR_BSA_DR                         8 /* me_bSubsystemAlive("DR#") failed */
#define MEOS_ERR_DR_INI			       16 /* DR# INI failed */
#define MEOS_ERR_DR_REC                        32 /* attempt to send DR# REC command failed */
#define MEOS_ERR_DP_TBX                        64 /* attempt to send DP_ TBW, TBN, or DRX command failed */
#define MEOS_ERR_DR_OPTYPE_FAIL               128 /* memdre() returned an error when asked for OP-TYPE */
#define MEOS_ERR_DR_DIR_FAIL                  256 /* problem getting DIRECTORY-COUNT or DIRECTORY-ENTRY-# */
#define MEOS_ERR_DR_CPY_FAIL                  512 /* problem doing DR# CPY */
#define MEOS_ERR_DR_DEL_FAIL                 1024 /* problem doing DR# DEL (deleting from DRSU) */

/* other parameters which affect operation */
#define MEOS_DR_START_DELAY_MS         5000 /* [ms] how far in the future [ms] to start DR recording */
                                            /* Chris Wolfe says this must be > 2.5 s */
#define MEOS_DR_LENGTH_MS_TBW         75000 /* [ms] how long to record, for TBW (takes a long time to read out) */
                                            /* This is 60s for TBW readout, plus a comfortable margin to account */
                                            /* for the simplistic sync scheme employed here */
#define MEOS_DR_MIB_UPDATE_DELAY_US 3000000 /* [us] how long to wait for a MIB entry to update after RPT issued */

int meos (
  int nDR,        /* (input) ID# of DR (e.g., 1="DR1", 2="DR2", and so on) to use */
  char *sExtDev,  /* (input) name of DR's external device to copy acquisition to */
  char *sDestDir, /* (input) Desination directory (in user space) for file.  No trailing slash. */
  char *mode,     /* (input) "TBW", "TBN", or "DRX" */
  char *args      /* (input) For TBW: "b n" where b = "12" means 12-bit [DEFAULT], "4" means 4-bit */
                  /*                              n = number of samples (up to 12000000 [DEFAULT] for 12-bit; */
                  /*                                                     up to 36000000 [DEFAULT] for 36-bit) */
                  /*                  e.g., "12 12000000" */
                  /*         For TBF: "n m" where n = number of samples   */
                  /*                              m = DRX tuning mask     */
                  /*         For TBN: "f r g s d" where f = center freq [Hz], */
                  /*                                    r = rate "1"|"2"|...|"7" */
                  /*                                    g = gain "0"|"1"|... */
                  /*                                    s = subslot "0"|"1"|... */
                  /*                                    d = duration [ms] */
                  /*                  e.g., "38000000 7 28 0 60000" */
                  /*         For DRX: "d" where d is duration [ms].  No other arguments are expected. */
                  /*                  It is assumed that appropriate FST, BAM, and DRX */
                  /*                  commands have already been sent & that DP is ready to go */
  ) {
  /* Returns error code (one of MEOS_ERR_*) */
  /* Recording is written to file named <OP-TAG>_<nDR>.dat; e.g., 055468_000000008_1.dat */

  char dest[4];
  char cmd[4];
  char data[1024];
  char sdate[1024];
  char stime[1024];

  char sDR[4];
  int b4bits = 0;
  int temp = 0;
  long int nsamp = 12000000;
  unsigned long int tuning_mask;
  float tbn_f = 0;
  int tbn_r = 0;
  int tbn_g = 0;
  int tbn_s = 0;
  long int tbn_d = 0;
  long int drx_d = 0;
  char dr_format[33];

  int err;
  int eResult=MEOS_ERR_OK;
  long int reference = 0;

  long int mjd0,mpm0;
  long int dr_start_mjd;
  long int dr_start_mpm;
  long int dr_length_ms;

  int bDone;
  char label[MIB_LABEL_FIELD_LENGTH];
  char val[MIB_VAL_FIELD_LENGTH];
  long int mjd,mpm;
  struct timeval t0;

  char optag[17];
  int nDirCount;
  int i;

  char d_tag[17];
  char d_start_mjd[7];
  char d_start_mpm[10];
  char d_stop_mjd[7];
  char d_stop_mpm[10];
  char d_df[33];
  char d_size[16];
  char d_du[16];
  char d_c[4];

  char sFilename[128];
  char sSysCmd[1024];

  printf("[%d/%d] Running meos(%d,'%s','%s','%s','%s')\n",ME_MEOS,getpid(),nDR,sExtDev,sDestDir,mode,args);
  //printf("[%d/%d] Using DR%d, '%s', '%s'\n",ME_MEOSX,getpid(),nDR,sExtDev,sDestDir);

  /* parse mode/args */
  bDone=0;
#ifdef USE_ADP
  if (!strncmp(mode,"TBF",3)) {\
    sscanf(args,"%ld %lu",&nsamp,&tuning_mask);
    printf("[%d/%d] mode='%s', nsamp=%ld, tuning_mask=%lu\n",ME_MEOS,getpid(),mode,nsamp,tuning_mask);
    sprintf(dr_format,"DEFAULT_TBF");
    bDone=1;
  }
#else
  if (!strncmp(mode,"TBW",3)) {
    b4bits = !strncmp(args,"4",1);
    sscanf(args,"%d %ld",&temp,&nsamp);
    printf("[%d/%d] mode='%s', b4bits=%d, nsamp=%ld\n",ME_MEOS,getpid(),mode,b4bits,nsamp);
    sprintf(dr_format,"DEFAULT_TBW");
    bDone=1;
    }
#endif
  if (!strncmp(mode,"TBN",3)) {
    //printf("[%d/%d] args='%s'\n",ME_MEOS,getpid(),args);
    sscanf(args,"%f %d %d %d %ld",&tbn_f,&tbn_r,&tbn_g,&tbn_s,&tbn_d);
    printf("[%d/%d] mode='%s', f=%f, r=%d, g=%d, s=%d, d=%ld [ms]\n",ME_MEOS,getpid(),mode,tbn_f,tbn_r,tbn_g,tbn_s,tbn_d);
    sprintf(dr_format,"TBN");
    bDone=1;
    }
  if (!strncmp(mode,"DRX",3)) {
    //printf("[%d/%d] args='%s'\n",ME_MEOS,getpid(),args);
    sscanf(args,"%ld",&drx_d);
    printf("[%d/%d] mode='%s', d=%ld [ms]\n",ME_MEOS,getpid(),mode,drx_d);
    sprintf(dr_format,"DRX_FILT_7");
    bDone=1;
    }
  if (!bDone) {
    printf("[%d/%d] FATAL: Invalid mode ('%s')\n",ME_MEOS,getpid(),mode);
    eResult += MEOS_ERR_INVALID_MODE;
    return eResult;
    }

  /* construct DR subsystem ID string */
  sprintf(sDR,"DR%1d",nDR);

  /* confirm that scheduler is responding */  
  err = mesi(NULL,"MCS","PNG","","today","asap",&reference);
  //printf("[%d/%d] msei() returned code %d; reference=%ld\n",ME_MEOS,getpid(),err,reference);
  if (err!=0) {
    printf("[%d/%d] FATAL: Scheduler silent or not responding as expected (msei() returned '%d')\n",ME_MEOS,getpid(),err);
    eResult += MEOS_ERR_MESI_MCSS;
    return eResult;
    }

  /* confirm that DP_ is responding */
  //err = me_bSubsystemAlive("DP_");
  //if (err!=0) {
  //  printf("[%d/%d] FATAL: ms_bSubsystemAlive('DP_') returned code %d\n",ME_MEOS,getpid(),err);
  //  eResult += MEOS_ERR_BSA_DP;
  //  return eResult;
  //  }
  //printf("[%d/%d] DP is responding NORMAL\n",ME_MEOS,getpid(),err);

  /* confirm that DRn is responding */
  //err = me_bSubsystemAlive(sDR);
  //if (err!=0) {
  //  printf("[%d/%d] FATAL: ms_bSubsystemAlive('%s') returned code %d\n",ME_MEOS,getpid(),sDR,err);
  //  eResult += MEOS_ERR_BSA_DR;
  //  return eResult;
  //  }
  //printf("[%d/%d] %s is responding NORMAL\n",ME_MEOS,getpid(),sDR,err);

  ///* Initialize DR */
  //err = mesi( NULL, sDR, "INI", "-L-D", "today", "asap", &reference ); /* FIXME may not want to use -L-D ... */
  //if (err!=MESI_ERR_OK) {
  //  printf("[%d/%d] FATAL: mesi(NULL,'%s','INI',...) returned code %d\n",ME_MEOS,getpid(),sDR,err);  
  //  eResult += MEOS_ERR_DR_INI;
  //  return eResult;  
  //  } 
  //sleep(10); /* wait 10 seconds for initialization to complete */

  /* figure out current time */
  LWA_time( &mjd0, &mpm0 );  /* gets current time; returns MJD and MPM */
  printf("[%d/%d] LWA time is now %ld %ld\n",ME_MEOS,getpid(),mjd0,mpm0); 
  
  /* figure out when to start DR and DP, and when acquisition should be done */
  dr_start_mjd = mjd0;
  dr_start_mpm = mpm0 + MEOS_DR_START_DELAY_MS;
  if ( dr_start_mpm > (24*3600*1000) ) { /* need to roll over into next day */
    dr_start_mjd += 1;
    dr_start_mpm -= (24*3600*1000); 
    }
  dr_length_ms = MEOS_DR_LENGTH_MS_TBW;
  if (!strncmp(mode,"TBN",3)) {
    dr_length_ms = tbn_d; 
    }
  if (!strncmp(mode,"DRX",3)) {
    dr_length_ms = drx_d; 
    }

  /* if TBN, then we want to start DP first (since this mode runs continuously), and then DR */
  if (!strncmp(mode,"TBN",3)) {
#ifdef USE_ADP
    sprintf(data,"%8.0f %d %d",tbn_f,tbn_r,tbn_g);
    err = mesi( NULL, "ADP", "TBN", data, "today", "asap", &reference );
#else
    sprintf(data,"%8.0f %d %d %d",tbn_f,tbn_r,tbn_g,tbn_s);
    err = mesi( NULL, "DP_", "TBN", data, "today", "asap", &reference );
#endif
    if (err!=MESI_ERR_OK) {
      printf("[%d/%d] FATAL: mesi(NULL,'DP_','TBN',...) returned code %d\n",ME_MEOS,getpid(),err);  
      eResult += MEOS_ERR_DP_TBX;
      return eResult;  
      } 
#ifdef USE_ADP
    printf("[%d/%d] ADP accepted '%s %s' (ref=%ld).  Here we go...\n",ME_MEOS,getpid(), mode, data, reference );
#else
    printf("[%d/%d] DP accepted '%s %s' (ref=%ld).  Here we go...\n",ME_MEOS,getpid(), mode, data, reference );
#endif
    }

  /* if DRX, we assume DP is already running; so above procedure (for TBN) is not necessary */

  /* Send DR# REC command */
  sprintf(data,"%06ld %09ld %09ld %s",dr_start_mjd,dr_start_mpm,dr_length_ms,dr_format);
  err = mesi( NULL, sDR, "REC", data, "today", "asap", &reference );
  if (err!=MESI_ERR_OK) {
    printf("[%d/%d] FATAL: mesi(NULL,'%s','REC',...) returned code %d\n",ME_MEOS,getpid(),sDR,err);  
    eResult += MEOS_ERR_DR_REC;
    return eResult;  
    } 
  sprintf(optag,"%06ld_%09ld",dr_start_mjd,reference);
  printf("[%d/%d] %s starts @ %ld %ld, records for %ld ms. ref=%ld.\n",ME_MEOS,getpid(),sDR,
          dr_start_mjd, dr_start_mpm, dr_length_ms, reference );
  printf("[%d/%d] I anticipate OP-TAG will be '%s'\n",ME_MEOS,getpid(),optag);
 
  /* sleep until past estimated DR start time  */
  usleep((MEOS_DR_START_DELAY_MS+1000)*1000); /* 1 s after start-delay */

#ifdef USE_ADP
  /* if TBF, now tell ADP to start. */
  if (!strncmp(mode,"TBF",3)) {
    sprintf(data,"16 0 %d %lu",nsamp,tuning_mask);
    err = mesi( NULL, "ADP", "TBF", data, "today", "asap", &reference );
    if (err!=MESI_ERR_OK) {
      printf("[%d/%d] FATAL: mesi(NULL,'ADP','TBF',...) returned code %d\n",ME_MEOS,getpid(),err);  
      eResult += MEOS_ERR_DP_TBX;
      return eResult;  
      } 
    printf("[%d/%d] ADP accepted '%s %s' (ref=%ld).  Here we go...\n",ME_MEOS,getpid(), mode, data, reference );
    }
  
#else
  /* if TBW, now tell DP to start.  It will take a couple seconds before it gets going, so */
  /* make sure DP record time is long enough to account for this! */
  if (!strncmp(mode,"TBW",3)) {
    sprintf(data,"%d 0 %ld",b4bits,nsamp);
    err = mesi( NULL, "DP_", "TBW", data, "today", "asap", &reference );
    if (err!=MESI_ERR_OK) {
      printf("[%d/%d] FATAL: mesi(NULL,'DP_','REC',...) returned code %d\n",ME_MEOS,getpid(),err);  
      eResult += MEOS_ERR_DP_TBX;
      return eResult;  
      } 
    printf("[%d/%d] DP accepted '%s %s' (ref=%ld).  Here we go...\n",ME_MEOS,getpid(), mode, data, reference );
    }
#endif
   
  /* start looking for DR to be done */
  bDone = 0;
  while (!bDone) {

    sleep(10);

    err = me_GetMIBEntry( sDR, "OP-TYPE", MEOS_DR_MIB_UPDATE_DELAY_US, val, &t0 );
    if (err!=0) { 
      eResult += MEOS_ERR_DR_OPTYPE_FAIL; 
      return eResult;
      }

    LWA_timeval(&t0,&mjd,&mpm); /* convert t0 from timeval to MJD/MPM */
    printf("[%d/%d] %s OP-TYPE='%s'; last update @ MPM=%ld.\n",ME_MEOS,getpid(),sDR,val,mpm);
    if (strncmp(val,"Idle",4)==0) { bDone = 1; }

    }
  printf("[%d/%d] Recording complete.\n",ME_MEOS,getpid());

  /* Get directory information about recording.  This is mainly to get length, but also to confirm
  /*   file was properly completed.  First, get number of directory entries (DIRECTORY-COUNT): */
  err = me_GetMIBEntry( sDR, "DIRECTORY-COUNT", MEOS_DR_MIB_UPDATE_DELAY_US, val, &t0 );
  if (err!=0) { 
    eResult += MEOS_ERR_DR_DIR_FAIL; 
    return eResult;
    }
  sscanf(val,"%d",&nDirCount);
  printf("[%d/%d] DIRECTORY-COUNT=%d.\n",ME_MEOS,getpid(),nDirCount);

  /* Assume we are the highest-numbered directory.  Get our info: */
  sprintf( label, "DIRECTORY-ENTRY-%d", nDirCount );
  err = me_GetMIBEntry( sDR, label, MEOS_DR_MIB_UPDATE_DELAY_US, val, &t0 ); 
  if (err!=0) { 
    eResult += MEOS_ERR_DR_DIR_FAIL; 
    return eResult;
    }
  printf("[%d/%d] %s: '%s'.\n",ME_MEOS,getpid(),label,val);
  sscanf(val,"%s %s %s %s %s %s %s %s %s",d_tag,d_start_mjd,d_start_mpm,d_stop_mjd,d_stop_mpm,d_df,d_size,d_du,d_c);
  printf("[%d/%d] Found %s: d_tag='%s', d_size='%s'.\n",ME_MEOS,getpid(),label,d_tag,d_size);

  /* confirm that requested external device exists on DR */
  /* FIXME */

  /* confirm that requested external device has sufficient space */
  /* FIXME */

  /* Construct filename */
  sprintf(sFilename,"%s_%d_%s.dat",d_tag,nDR,mode);  
  printf("[%d/%d] CPYing recording to %s/%s starting now:\n",ME_MEOS,getpid(),sExtDev,sFilename);

  /* Send command to move recording to specified external device on DR */
  sprintf(data,"%s 0 %s %s %s",d_tag,d_size,sExtDev,sFilename);
  err = mesi( NULL, sDR, "CPY", data, "today", "asap", &reference ); /* send DR# CPY command */
  if (err!=0) {
    printf("[%d/%d] FATAL: mesi(NULL, '%s','CPY','%s',...) returned code %d, ref='%ld'\n",ME_MEOS,getpid(),sDR,data,err,reference);  
    eResult += MEOS_ERR_DR_CPY_FAIL; 
    return eResult;
    }
//./msei DR1 CPY "055468_000000008 0 367263744 /dev/sdg1 tbw_test1_12bit_092910"
//                tag 16
//                                 start byte 15
//                                   length 15
//                                             storage ID 64
//                                                       Filename 128

  /* Wait for CPY to complete */
  bDone = 0;
  while (!bDone) {
    sleep(1);  /* some time to make sure CPY is going, so we don't prematurely see OP-TYPE="Idle" */
    err = me_GetMIBEntry( sDR, "OP-TYPE", MEOS_DR_MIB_UPDATE_DELAY_US, val, &t0 );
    if (err!=0) { 
      eResult += MEOS_ERR_DR_OPTYPE_FAIL; 
      return eResult;
      }
    LWA_timeval(&t0,&mjd,&mpm); /* convert t0 from timeval to MJD/MPM */
    printf("[%d/%d] %s OP-TYPE='%s'; last update @ MPM=%ld.\n",ME_MEOS,getpid(),sDR,val,mpm);
    if (strncmp(val,"Idle",4)==0) { bDone = 1; }
    }
  printf("[%d/%d] Copying complete.\n",ME_MEOS,getpid());
  
  /* copy file from DR external storage to specified directory */
  sprintf(sSysCmd,"scp druser@10.1.1.2%d:/LWA_EXT%s/%s %s",nDR,sExtDev,sFilename,sDestDir);
  printf("[%d/%d] About to do system('%s')...\n",ME_MEOS,getpid(),sSysCmd);
  system(sSysCmd);
  printf("[%d/%d] ...done.\n",ME_MEOS,getpid());
  
  /* delete file from DR external storage */
  /* FIXME */

  /* delete recording from DRSU to free up space */
  err = mesi( NULL, sDR, "DEL", d_tag, "today", "asap", &reference ); /* send DR# CPY command */
  if (err!=0) {
    printf("[%d/%d] FATAL: mesi(NULL, '%s','DEL','%s',...) returned code %d, ref='%ld'\n",ME_MEOS,getpid(), sDR, d_tag, err, reference);  
    eResult += MEOS_ERR_DR_DEL_FAIL; 
    //return eResult;
    }
  printf("[%d/%d] %s has been commanded to delete recording from DRSU.\n",ME_MEOS,getpid(),sDR);

  return eResult;
  } /* meos() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// meos.c: J. Dowell, UNM, 2015 Sep 11
//   .1: Modified for the new ADP TBF command
// meos.c: S.W. Ellingson, Virginia Tech, 2011 Mar 25
//   .1: Modified to use modified mesi() function, which has new sockfd_ptr arg.
// meos.c: S.W. Ellingson, Virginia Tech, 2011 Feb 10
//   .1: Adding support for DRX mode
// meos.c: S.W. Ellingson, Virginia Tech, 2010 Nov 01
//   .1: Accomodating variable number of samples in TBW
//       User's destination directory can be selected 
// meos.c: S.W. Ellingson, Virginia Tech, 2010 Oct 16
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================


