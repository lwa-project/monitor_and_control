// meeix.c: S.W. Ellingson, Virginia Tech, 2014 Feb 10
// ---
// COMPILE: gcc -o meeix meeix.c
// ---
// COMMAND LINE: meeix <cmd> <args>
//   <cmd>: command to send: one of:
//     "SHT" (orderly shutdown of executive)   
//     "STP" (stop -- terminate a observing session after submission)    
//  <args>: a string surrounded by double quotes 
//     (for STP:) <PROJECT_ID> <SESSION_ID>
// ---
// EXAMPLES:
// $ ./meeix SHT
// $ ./meeix STP "LE002001 2"
// ---
// REQUIRES: 
//   me.h
//   meei.c
// ---
// Used to inject commands into MCS/Executives's 'me' process.  
//   (This is the client-side of a client-server connection)
// Uses the meei() function (from meei.c)
// See end of this file for history.

#include "me.h"
#include "meei.c"

int main ( int narg, char *argv[] ) {

  char cmd[4];
  char args[ME_MAX_COMMAND_LINE_LENGTH];
  int eResult;

  strcpy(args,"");

  /* processing command line arguments */

  if (narg>1) { 
      sscanf(argv[1],"%3s",cmd);
    } else {
      printf("[%d/%d] FATAL: cmd not provided\n",ME_MEEIX,getpid());
      return;
    } 

  if (strncmp(cmd,"STP",3)==0) { 
    /* get subsequent arguments */
    if (narg>2) { 
        //sscanf(argv[2],"%s",args);
        memcpy(args,argv[2],strlen(argv[2])+1);
      } else {
        printf("[%d/%d] FATAL: args for 'STP' command not provided\n",ME_MEEIX,getpid());
        return;
      } 
    }

  //printf("[%d/%d] cmd = '%s' args = '%s'\n",ME_MESIX,getpid(),cmd,args);

  eResult = meei(cmd,args);
  printf("[%d/%d] meei() returned code %d\n",ME_MESIX,getpid(),eResult);
  
  return 0;
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// meeix.c: J. Dowell, UNM, 2018 Jan 29
//   .1 Cleaned up a few compiler warnings
// meeix.c: S.W. Ellingson, Virginia Tech, 2014 Feb 10
//   .1: adding "STP" command
// meeix.c: S.W. Ellingson, Virginia Tech, 2011 March 09
//   .1: initial version, using mesix.c as a starting point
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


