// me_precess_x.c: J. Dowell, UNM, 2015 Aug 17
// ---
// COMPILE: gcc -o me_precess_x me_precess_x.c -lm
// ---
// COMMAND LINE: 
// ---
// EXAMPLE:
// $ ./me_precess_x ... (see message below)
// ---
// REQUIRES: 
//   me_findjov.c
// ---
// Wrapper used to test me_precess() 
// See end of this file for history.

#include <math.h>
#include <stdio.h>

#include "me_astro.h"

int main ( int narg, char *argv[] ) {

  float ra;    /* (input) [h] RA */
  float dec;   /* (input) [deg] dec */
  long int mjd; /* (input) mean julian date */
  long int mpm; /* (input) milliseconds past UTC midnight */
  float m, s;
 
  /* Parse command line */
  if (narg>1) sscanf(argv[1],"%ld",&mjd);
  if (narg>2) sscanf(argv[2],"%ld",&mpm);  
  if (narg>3) sscanf(argv[3],"%f",&ra);
  if (narg>4) sscanf(argv[4],"%f",&dec);
  if (narg<4) {
    printf("Usage:\n");
    printf("me_precess_x <mjd> <mpm> <ra> <dec>\n");
    printf("  <mjd>: integer MJD\n");
    printf("  <mpm>: integer milliseconds past UTC midnight\n");
    printf("  <ra>:  float right ascension in hours, J2000.0\n");
    printf("  <dec>: float declination in degrees, J2000.0\n");
    
    return 1;
    }
 
  me_precess(
              mjd,  /* (input) mean julian date */
              mpm,  /* (input) milliseconds past UTC midnight */ 
              &ra,  /* (input/output) [h] calculated RA */
              &dec  /* (input/output) [deg] calculated dec */ 
             );

  m = (ra - floor(ra))*60.0;
  s = (m - floor(m))*60.0;
  printf("RA  = %8.3f h   =  %02.0f h %02.0f m %05.2f s\n",ra, floor(ra), floor(m), s);
  m = (fabs(dec)-floor(fabs(dec)))*60.0;
  s = (m - floor(m))*60.0;
  printf("dec = %8.3f deg = %+03.0f d %02.0f m %04.1f s\n",dec, (dec/fabs(dec))*floor(fabs(dec)), floor(m), s);

  return 0;
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// me_precess_x.c: J. Dowell, UNM, 2015 Aug 17
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================


