// mesix.c: J. Dowell, UNM, 2015 Sep 11
// ---
// COMPILE: gcc -o meosx meosx.c
// ---
// COMMAND LINE: meosx <DR#> <ExtDev> <DestDir> <mode> <args>
//   <DR#>:     DR to use (e.g., "1" (DEFAULT))
//   <ExtDev>:  Location of file on DR#'s external storage (e.g., "/dev/sdg1" (DEFAULT)) 
//   <DestDir>: Desination directory (in user space) for file.  No trailing slash.  DEFAULT is "." ("here")
//   <mode>:    "TBT", "TBS", "DRX"   
//   <args>:    For TBT: "n m" where n = number of samples
//                                   m = 64-bit DRX tuning mask
//              For TBS: "f r d" where f = center freq [Hz], 
//                                         r = rate "1"|"2"|...|"7" 
//                                         d = duration [ms] 
//                e.g., "38000000 8 60000" 
//              For DRX: "d" where d is duration [ms] 
//                No other arguments are expected.  It is assumed that appropriate FST, BAM, and DRX
//                commands have already been sent & that DP is ready to go 
// EXAMPLE:
// $ ./mseox                                  # uses all defaults
// $ ./mseox 1 /dev/sdg1 . TBT "196000 255"   # same thing with default values shown explicitly
// ---
// REQUIRES: 
//   me.h
// ---
// Used to do one-shot acquisition using meos() 
// See end of this file for history.

#include <stdlib.h>

#include "me.h"

int main ( int narg, char *argv[] ) {

  int eResult=-1;

  int nDR=1;
  char sExtDev[256];
  char sDestDir[256];
  char mode[4];
  char args[1024];

  /* Parse command line */
  if (narg>1) { 
    sscanf(argv[1],"%d",&nDR);
    } 
  sprintf(sExtDev,"/dev/sdg1"); /* default */
  if (narg>2) { 
    sscanf(argv[2],"%s",sExtDev);
    } 
  sprintf(sDestDir,"."); /* default */
  if (narg>3) { 
    sscanf(argv[3],"%s",sDestDir);
    }
  sprintf(mode,"TBT");
  if (narg>4) { 
    sscanf(argv[4],"%s",mode);
    } 
  sprintf(args,"196000 255");
  if (narg>5) { 
    memcpy(args,argv[5],strlen(argv[5])+1);
    }

  eResult = meos(nDR,sExtDev,sDestDir,mode,args);
  printf("[%d/%d] meos() returned code %d\n",ME_MEOSX,getpid(),eResult);
  
  exit(EXIT_SUCCESS);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// mesix.c: J. Dowell, UNM, 2015 Sep 11
//   .1: Added support for the ADP TBF mode
// mesix.c: S.W. Ellingson, Virginia Tech, 2011 Feb 10
//   .1: Adding support for DRX mode 
// mesix.c: S.W. Ellingson, Virginia Tech, 2010 Nov 01
//   .1: Allowing variable number of samples for TBW
//       User's destination directory can be selected 
// mesix.c: S.W. Ellingson, Virginia Tech, 2010 Oct 16
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
