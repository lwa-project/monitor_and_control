// me_getlst_x.c: S.W. Ellingson, Virginia Tech, 2012 Oct 07
// ---
// COMPILE: gcc -o me_getlst_x me_getlst_x.c -lm
// ---
// COMMAND LINE: 
// ---
// EXAMPLE:
// $ ./me_getlst_x ... 
// ---
// REQUIRES: 
//   me_getlst.c
// ---
// Wrapper used to test me_getlst() 
// See end of this file for history.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "me_astro.h"

int main ( int narg, char *argv[] ) {

  long int mjd; /* (input) mean julian date */
  long int mpm; /* (input) milliseconds past UTC midnight */ 
  double lat;   /* (input) [deg, +N] observer location */
  double lon;   /* (input) [deg, +E] observer location */ 
  double LAST;  /* (output) [h] local apparent sidereal time */
  double m, s;
  
  /* Parse command line */
  if (narg>1) sscanf(argv[1],"%ld",&mjd);
  if (narg>2) sscanf(argv[2],"%ld",&mpm);
  if (narg>3) sscanf(argv[3],"%lf",&lat);
  if (narg>4) sscanf(argv[4],"%lf",&lon);     
  if (narg<5) {
    printf("Useage:\n");
    printf("me_getlst_x <mjd> <mpm> <lat> <lon>\n");
    printf("  <mjd>: integer MJD\n");
    printf("  <mpm>: integer milliseconds past UTC midnight\n");
    printf("  <lat>: [deg, +N] observer location\n");
    printf("  <lon>: [deg, +E] observer location\n");
    exit(EXIT_FAILURE);
    }
  //printf("MJD/MPM = %5ld %9ld\n",mjd,mpm);
  //printf("LAT/LON = %+6.2lf deg %+7.2lf deg\n",lat,lon); 

  me_getlst(
            mjd,   /* (input) mean julian date */
            mpm,   /* (input) milliseconds past UTC midnight */ 
            lat,   /* (input) [deg, +E] observer location */
            lon,   /* (input) [deg, +N] observer location */ 
            &LAST /* (output) [h] local apparent sidereal time */
           );

  m = (LAST - floor(LAST))*60.0;
  s = (m - floor(m))*60.0;
  printf("LAST = %8.3lf h    =   %02.0lf h %02.0lf m %05.2lf s\n", LAST, floor(LAST), floor(m), s);
  
  exit(EXIT_SUCCESS);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// me_getlst_x.c: J. Dowell, UNM, 2015 Sep 1
//   .1: Initial version
//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
