// me_findsol_x.c: S.W. Ellingson, Virginia Tech, 2012 Oct 07
// ---
// COMPILE: gcc -o me_findsol_x me_findsol_x.c -lm
// ---
// COMMAND LINE: 
// ---
// EXAMPLE:
// $ ./me_findsol_x ... 
// ---
// REQUIRES: 
//   me_findsol.c
// ---
// Wrapper used to test me_findsol() 
// See end of this file for history.

#include <stdio.h>
#include <math.h>

#include "me_findsol.c"

int main ( int narg, char *argv[] ) {

  float ra;    /* (input) [h] RA */
  float dec;   /* (input) [deg] dec */
  long int mjd; /* (input) mean julian date */
  long int mpm; /* (input) milliseconds past UTC midnight */ 

  /* Parse command line */
  if (narg>1) sscanf(argv[1],"%ld",&mjd);
  if (narg>2) sscanf(argv[2],"%ld",&mpm);   
  if (narg<3) {
    printf("Useage:\n");
    printf("me_findsol_x <mjd> <mpm>\n");
    printf("  <mjd>: integer MJD\n");
    printf("  <mpm>: integer milliseconds past UTC midnight\n");
    return;
    }
 
  me_findsol(
              mjd,  /* (input) mean julian date */
              mpm,  /* (input) milliseconds past UTC midnight */ 
              &ra,  /* (output) [h] calculated RA */
              &dec  /* (output) [deg] calculated dec */ 
             );

  printf("RA  = %8.3f h   =  %02.0f h %02.0f m\n",ra, floor(ra), (ra-floor(ra))*60.0);
  printf("dec = %8.3f deg = %+03.0f d %02.0f m\n",dec,(dec/fabs(dec))*floor(fabs(dec)),(fabs(dec)-floor(fabs(dec)))*60.0);

  return 0;
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// me_findsol_x.c: S.W. Ellingson, Virginia Tech, 2012 Oct 07
//   .1: Refinements for integration into MCS codebase
// me_findsol_x.c: S.W. Ellingson, Virginia Tech, 2012 Oct 04
//   .1: Initial version
// me_getaltazx.c: S.W. Ellingson, Virginia Tech, 2011 May 01
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================


