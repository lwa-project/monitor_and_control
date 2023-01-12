// mesix.c: S.W. Ellingson, Virginia Tech, 2010 Oct 16
// ---
// COMPILE: gcc -o mesix mesix.c
// ---
// COMMAND LINE: mesix <dest> <cmd> <data> <date> <time>
//   <dest> = DESTINATION.  (three-letter subsystem designator)
//            see mcs.h for valid entries
//   <cmd>  = TYPE. (three-letter command TYPE)
//            see mcs.h for valid entries
//   <data> = DATA. (DATA field for given TYPE) -- depends on command
//            For subsystems NU#, SHL, ASP, and DR#, this is ASCII.
//              (The string provided will be exactly the string used for the DATA field)
//            For DP, this will be a list of parameters that will get translated into a raw binary DATA field
//               See mesi.c for how this gets dones
//            Remember to use quotes if this argument includes spaces
//   <date> = Date when command should run.  Either MJD or "today".  If not specified, "today" is assumed.
//   <time> = Time when command should run.  Either MPM, +n, or "asap".  
//              +n (e.g,. "+3") means set <stime> to current MPM + n [seconds] 
//              If not specified, "asap" is assumed. 
//   Note meaning of arguments is sensitive to position; so if you want <time> other than "asap", you have to
//   explicitly include a <date> parameter.
//   Note that the <date>/<time> corresponds to the moment at which MCS/Scheduler (specifically, the ms_exec process)
//     will send the command to the specified subystem (specifically, to the corresponding ms_mcic process).          
// ---
// REQUIRES: 
//   me.h
//   mesi.c
// ---
// Used to inject commands into MCS/Scheduler's ms_exec process.  
//   (This is the client-side of a client-server connection)
// Uses the mesi() function (from mesi.c)
// This is essentially equivalent to msei.c, from the MCS/Scheduler distribution
// See end of this file for history.

#include <stdlib.h>

#include "me.h"

int main ( int narg, char *argv[] ) {

  char dest[4];
  char cmd[4];
  char data[1024];
  char sdate[1024];
  char stime[1024];

  int eResult=-1;
  long int reference = 0;

  /* processing command line arguments */

  if (narg>1) { 
    sscanf(argv[1],"%3s",dest);
    //printf("dest = <%s>\n",dest);
    } else {
    printf("[%d/%d] FATAL: dest not provided\n",ME_MESIX,getpid());
    exit(EXIT_FAILURE);
    } 
  if (narg>2) { 
    sscanf(argv[2],"%3s",cmd);
    //printf("dest = <%s>\n",dest);
    } else {
    printf("[%d/%d] FATAL: cmd not provided\n",ME_MESIX,getpid());
    exit(EXIT_FAILURE);
    } 
  strcpy(data,"");
  if (narg>3) { 
    strncpy(data,argv[3],strlen(argv[3])+1);
    } 
  strcpy(sdate,"today");
  if (narg>4) { 
    strncpy(sdate,argv[4],strlen(argv[4])+1);
    } 
  strcpy(stime,"asap");
  if (narg>5) { 
    strncpy(stime,argv[5],strlen(argv[5])+1);
    } 

  //printf("[%d/%d] dest  = '%s'\n",ME_MESIX,getpid(),dest);
  //printf("[%d/%d] cmd   = '%s'\n",ME_MESIX,getpid(),cmd);
  //printf("[%d/%d] data  = '%s'\n",ME_MESIX,getpid(),data);
  //printf("[%d/%d] sdate = '%s'\n",ME_MESIX,getpid(),sdate);
  //printf("[%d/%d] stime = '%s'\n",ME_MESIX,getpid(),stime);

  eResult = mesi(NULL,dest,cmd,data,sdate,stime,&reference);
  
  printf("[%d/%d] mesi() returned code %d; reference=%ld\n",ME_MESIX,getpid(),eResult,reference);
  
  exit(EXIT_SUCCESS);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// mesix.c: S.W. Ellingson, Virginia Tech, 2010 Oct 15
//   .1: Initial version, splitting msei.c into this code and the rest into mesi.c
// msei.c: S.W. Ellingson, Virginia Tech, 2010 Oct 12
//   .1: Adding support for scheduled actions (not completed)
// msei.c: S.W. Ellingson, Virginia Tech, 2010 Jun 10
//   .1: Adding support for MCS-DR.  Nothing to do besides update comments
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Nov 7
//   .1: Changed dimension of cmd[] and dest[] from 3 to 4
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Aug 25
//   .1: Adding support for DP's TBN, CLK, INI commands; got rid of "subslot"
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Aug 17
//   .1: Adding full support (binary argument) for DP's TBW command
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Aug 16
//   .1: Added "datalen" to LWA_cmd_struct, so this is set to "-1" to indicate "string"
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Aug 06
//   .1: Fixed bug causing DATA to be truncated to first space, even when quoted
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Aug 02
//   .1: Cleaning up console output
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Jul 26
//   .1: Added LWA_cmd_struct field eMIBerror (svn rev 10)
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Jul 23
//   .1: Adding "DATA" command line argument (svn rev 7)
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Jul 20
//   .1: Adding "time to execute" field to command struct
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Jul 17
//   .1: Cleaning up; esp. implementation of LWA_cmd_struct
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Jul 13
//   .1: 
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Jul 10
//   .1: preliminary version (not even numbered); 3-letter char as argument 
//   .2: changing command line and transfer format

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
