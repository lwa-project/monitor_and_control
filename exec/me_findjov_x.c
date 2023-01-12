// me_findjov_x.c: S.W. Ellingson, Virginia Tech, 2012 Oct 07
// ---
// COMPILE: gcc -o me_findjov_x me_findjov_x.c -lm
// ---
// COMMAND LINE: 
// ---
// EXAMPLE:
// $ ./me_findjov_x ... (see message below)
// ---
// REQUIRES: 
//   me_findjov.c
// ---
// Wrapper used to test me_findjov() 
// See end of this file for history.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "me_astro.h"

int main ( int narg, char *argv[] ) {
  
  long int mjd; /* (input) mean julian date */
  long int mpm; /* (input) milliseconds past UTC midnight */ 
  float ra;     /* (output) [h] RA */
  float dec;    /* (output) [deg] dec */
  float dist;   /* (output) [AU] distance from Earth */
  float m, s;
 
  /* Parse command line */
  if (narg>1) sscanf(argv[1],"%ld",&mjd);
  if (narg>2) sscanf(argv[2],"%ld",&mpm);   
  if (narg<3) {
    printf("Useage:\n");
    printf("me_findjov_x <mjd> <mpm>\n");
    printf("  <mjd>: integer MJD\n");
    printf("  <mpm>: integer milliseconds past UTC midnight\n");
    exit(EXIT_FAILURE);
    }
 
  me_findjov(
              mjd,        /* (input) mean julian date */
              mpm,        /* (input) milliseconds past UTC midnight */ 
              &ra,        /* (output) [h] calculated RA */
              &dec,       /* (output) [deg] calculated dec */ 
              &dist       /* (output) [AU] distance from Earth */
             );

  m = (ra - floor(ra))*60.0;
  s = (m - floor(m))*60.0;
  printf("RA   = %8.3f h   =  %02.0f h %02.0f m %05.2f s\n",ra, floor(ra), floor(m), s);
  m = (fabs(dec)-floor(fabs(dec)))*60.0;
  s = (m - floor(m))*60.0;
  printf("dec  = %8.3f deg = %+03.0f d %02.0f m %04.1f s\n",dec, (dec/fabs(dec))*floor(fabs(dec)), floor(m), s);
  printf("dist = %8.6f AU\n", dist);

  exit(EXIT_SUCCESS);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// me_findjov_x.c: S.W. Ellingson, Virginia Tech, 2012 Oct 07
//   .1: Initial version
// me_getaltazx.c: S.W. Ellingson, Virginia Tech, 2011 May 01
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
