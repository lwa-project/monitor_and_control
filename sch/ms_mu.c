// ms_mu.c: S.W. Ellingson, Virginia Tech, 2010 May 30
// ---
// COMPILE: gcc -o ms_mu -I/usr/include/gdbm ms_mu.c -lgdbm
// In Ubuntu, needed to install package libgdbm-dev
// ---
// COMMAND LINE: ms_mu <subsystem> <period>
//   <subsystem> is the 3-character subsystem designator 
//   <period> is an integer: 0 means update once and exit;
//      otherwise updates happen with period of this many seconds, forever.
//      (will need to need to crash out to stop) 
//      default if not specified: 0                        
//      see below for comments
// ---
// REQUIRES: 
//   LWA_MCS.h
//   dbm database representing MIB for indicated subsystem must exist
//     perhaps generated using dat2dbm
// ---
// Uses MCS/Scheduler to update all entries in a subsytem MIB dbm-file
// Note that rate at which this program slings MIB update requests at
//   MCS/Scheduler is moderated by MAX_UPDATES_PER_SEC; this is to 
//   avoid overwhelming MCS/Scheduler and the subsystem
// Thus, actual update rate likely to be slower than specified by "period"
// Certain MIB entries are excluded:
//   MCH_IP_ADDRESS, MCH_TX_PORT, and MCH_RX_PORT
//   For DP:  FIR1, FIR2, FIR3, FIR4, and FIR_CHAN_INDEX
//     (Since these change everytime they are polled)
//
// See end of this file for history.

#include <stdlib.h> /* needed for exit(); possibly other things */
#include <stdio.h>

#include <string.h>
#include <fcntl.h> /* needed for O_READONLY; perhaps other things */
#include <gdbm.h>

//#include "LWA_MCS.h" 
#include "mcs.h"

#define MAX_LABELS 9999        /* max number of MIB entries supported */
#define MAX_UPDATES_PER_SEC 30 /* do no more than this many updates in a row */
                               /* then sleep for 1 second */

#define MY_NAME "ms_mu (v.20191030.1)"
#define ME "14" 

main ( int narg, char *argv[] ) {

  /*=================*/
  /*=== Variables ===*/
  /*=================*/

  char subsystem[256];
  int period = 0;
 
  /* dbm-related variables */
  char dbm_filename[256];
  GDBM_FILE dbm_ptr;
  struct dbm_record record;
  datum datum_key;
  datum datum_data;

  //struct timeval tv;  /* from sys/time.h; included via LWA_MCS.h */
  //struct tm *tm;      /* from sys/time.h; included via LWA_MCS.h */

  //FILE* fid_dat; 
  //char dat_filename[256];
  //int nlabels=0;
  //char label0[MAX_LABELS][MIB_LABEL_FIELD_LENGTH]; 

  //char line_type[2];   
  //char index[MIB_INDEX_FIELD_LENGTH];     
  char label[MIB_LABEL_FIELD_LENGTH];     
  //char val[256];
  //char type_dbm[6];
  //char type_icd[6];

  //int k;
  //char key[MIB_LABEL_FIELD_LENGTH];

  //char display[33];

  char cmd_line[256];
  int bUpdate = 0;
  int nUpdate = 0;
  int bDone = 0;

  /*======================================*/
  /*=== Initialize: Command line stuff ===*/
  /*======================================*/
    
  /* First, announce thyself */
  printf("[%s/%d] I am %s \n",ME,getpid(),MY_NAME);

  /* Process command line arguments */
  if (narg>1) { 
      //printf("[%s/%d] subsystem = <%s> specified\n",ME,getpid(),argv[1]);
      sprintf(subsystem,"%s",argv[1]);
      printf("[%s/%d] subsystem = <%s> specified\n",ME,getpid(),subsystem);
    } else {
      printf("[%s/%d] FATAL: subsystem not specified\n",ME,getpid());
      exit(EXIT_FAILURE);
    } 
  if (narg>2) { 
      sscanf(argv[2],"%d",&period);
      printf("[%s/%d] period = %d specified\n",ME,getpid(),period);
    } else {
      period = 0;
    } 
  
  sprintf(dbm_filename,"%s",subsystem); 

  /*=========================================*/
  /*=== Make a working copy of dbm files ====*/
  /*=========================================*/

  /* This is to reduce the possibility for file contention */
  /* while MCS/Scheduler is running */

  /* make copy of the MIB; same root filename but with "_temp" added */
  sprintf(cmd_line,"cp %s.gdb %s_%d.gdb",dbm_filename,dbm_filename,getpid());
  printf("[%s/%d] system(%s)\n",ME,getpid(),cmd_line);
  system(cmd_line);
  
  /* now use *this* copy of MIB */
  sprintf(dbm_filename,"%s_%d.gdb",argv[1],getpid()); 

  //printf("[%s/%d] <%s>\n",ME,getpid(),dbm_filename);
  //exit(EXIT_SUCCESS);

  /*======================================*/
  /*=== Main loop ========================*/
  /*======================================*/

  while (!bDone) {

    /* Open dbm file */
    dbm_ptr = gdbm_open(dbm_filename, 0, GDBM_READER, 0, NULL);
    if (!dbm_ptr) {
      printf("[%s/%d] FATAL: Failed to open dbm <%s> - %s\n",ME,getpid(),dbm_filename,gdbm_strerror(gdbm_errno));
      sprintf(cmd_line,"rm %s",dbm_filename);
      system(cmd_line);
      exit(EXIT_FAILURE);
      }

    /* === Read dbm record-by-record === */
    for (
             datum_key = gdbm_firstkey(dbm_ptr);
             datum_key.dptr;
             datum_key = gdbm_nextkey(dbm_ptr,datum_key)
          ) {

      /* read next line */
      datum_data = gdbm_fetch(dbm_ptr,datum_key);

      memcpy( &record, datum_data.dptr, datum_data.dsize );

      memset( label, '\0', sizeof(label));
      strncpy(label,datum_key.dptr,datum_key.dsize);

      ///* convert, show time of last change */
      //tv = record.last_change;
      //tm = gmtime(&tv.tv_sec);
      //printf("|%02d%02d%02d %02d:%02d:%02d\n", (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

      /* assume this is an updateable MIB entry; possibly change mind below */
      bUpdate = 1;  

      /* if this is a branch entry, it's not updatable */
      if (record.eType==0) bUpdate=0;

      /* If this was a MIB entry added by dat2dbm, it shouldn't be updated */ 
      if (!strcmp(label,"MCH_IP_ADDRESS")) bUpdate=0;
      if (!strcmp(label,"MCH_TX_PORT")) bUpdate=0;
      if (!strcmp(label,"MCH_RX_PORT")) bUpdate=0;  

      /* DP FIR-related MIB entries shouldn't be updated, because they change */
      /* in response to update requests */
      if (!strcmp(label,"FIR1")) bUpdate=0; 
      if (!strcmp(label,"FIR2")) bUpdate=0;
      if (!strcmp(label,"FIR3")) bUpdate=0;
      if (!strcmp(label,"FIR4")) bUpdate=0;
      if (!strcmp(label,"FIR_CHAN_INDEX")) bUpdate=0;

      if (bUpdate) {

          //printf("[%s/%d] <%s>\n",ME,getpid(),label);
          sprintf(cmd_line,"./msei %s RPT %s",subsystem,label);
          system(cmd_line);

          /* take a break every nUpdate requests to keep from overwhelming MCS/Scheduler */
          nUpdate++;
          if (nUpdate>MAX_UPDATES_PER_SEC) {
            nUpdate=0;
            sleep(1);
            }

        } else {

          //printf("[%s/%d] NO UPDATE <%s>\n",ME,getpid(),label);

        } /* if (bUpdate) */

      } /* for () */

    /* Close dbm file */
    gdbm_close(dbm_ptr);

    if (period==0) { 
        bDone=1; 
      } else {
        sleep(period);
      }

    } /* while (!bDone) */

  /*======================================*/
  /*=== Clean up =========================*/
  /*======================================*/

  /* delete _temp copy of the MIB */
  sprintf(cmd_line,"rm %s",dbm_filename);
  printf("[%s/%d] system(%s)\n",ME,getpid(),cmd_line);
  system(cmd_line);
  
  //printf("[%s/%d] exit(EXIT_SUCCESS)\n",ME,getpid());
  exit(EXIT_SUCCESS);
  } /* main() */


//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// ms_mu.c: J. Dowell, UNM, 2019 Oct 30
//   .1 Convert to using normal GDBM for the database
// ms_mu.c: S.W. Ellingson, Virginia Tech, 2010 May 30
//   .1: Excluding DP FIR-related MIB entries
// ms_mu.c: S.W. Ellingson, Virginia Tech, 2010 May 28
//   .1: Branched from ms_md2t.c
// ms_md2t.c: S.W. Ellingson, Virginia Tech, 2010 May 25
//   .1: Branched from ms_mdr.c
// ms_mdr.c: S.W. Ellingson, Virginia Tech, 2009 Aug 16
//   .1: Dealing with unprintable fields -- now integers get printed
// ms_mdr.c: S.W. Ellingson, Virginia Tech, 2009 Aug 15
//   .1: Dealing with unprintable fields 
// ms_mdr.c: S.W. Ellingson, Virginia Tech, 2009 Aug 02
//   .1: Working on formatting (svn rev 23)
// ms_mdr.c: S.W. Ellingson, Virginia Tech, 2009 Jul 26 
//   .1: Very first version, adapted from segments of ms_mcic.c
//   .2: Implementing index/label swap in dbm database (svn rev 10)

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================

