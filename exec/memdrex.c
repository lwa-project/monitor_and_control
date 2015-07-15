// memdrex.c: S.W. Ellingson, Virginia Tech, 2010 Nov 16
// ---
// COMPILE: gcc -o memdrex memdrex.c
// ---
// COMMAND LINE: memdrex           
// ---
// REQUIRES: 
//   me.h
// ---
// Used to query ms_mdre_ip (the MCS/Scheduler subsystem MIB interface) using msmdre() 
// See end of this file for history.

#include "me.h"

int main ( int narg, char *argv[] ) {

  char dbm_filename[256];
  char label[MIB_LABEL_FIELD_LENGTH];     

  char val[MIB_VAL_FIELD_LENGTH];
  struct timeval last_change;  /* from sys/time.h; included via mcs.h */
  int eResult=-1;

  struct tm *tm;               /* from sys/time.h; included via mcs.h */

  /* Process command line arguments */
  if (narg>1) { 
      //printf("[%s/%d] %s specified\n",ME,getpid(),argv[1]);
      sprintf(dbm_filename,"%s",argv[1]);
    } else {
      printf("[%d/%d] FATAL: subsystem not specified\n",ME_MEMDREX,getpid());
      return;
    } 
  if (narg>2) { 
      //printf("[%s/%d] label <%s> specified\n",ME,getpid(),argv[2]);
      sprintf(label,"%s",argv[2]);
    } else {
      printf("[%d/%d] FATAL: MIB label not specified\n",ME_MEMDREX,getpid());
      return;
    } 

  eResult = memdre(dbm_filename,label,val,&last_change);
  printf("[%d/%d] memdre() returned code %d, val='%s'\n",ME_MEMDREX,getpid(),eResult,val);
 
  /* convert, show time of last change */
  tm = gmtime(&last_change.tv_sec);
  printf("%02d%02d%02d %02d:%02d:%02d\n", (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);


  return 0;
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// memdrex.c: S.W. Ellingson, Virginia Tech, 2010 Nov 16
//   .1: Added command line interface
// memdrex.c: S.W. Ellingson, Virginia Tech, 2010 Oct 16
//   .1: Initial version
// mesix.c: S.W. Ellingson, Virginia Tech, 2010 Oct 16
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================


