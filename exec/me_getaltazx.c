// me_getaltazx.c: S.W. Ellingson, Virginia Tech, 2011 May 05
// ---
// COMPILE: gcc -o me_getaltazx me_getaltazx.c -I../common -lm
// ---
// COMMAND LINE: see help message in source code, below
// ---
// EXAMPLE:
// $ ./me_getaltazx  
// ---
// REQUIRES: 
//   me.h, me_getaltaz.c
// ---
// Wrapper used to test me_getaltaz() 
// See end of this file for history.

#include <math.h>

#include "me.h"
#include "me_getaltaz.c"

int main ( int narg, char *argv[] ) {

  double ra;    /* (input) [h] RA */
  double dec;   /* (input) [deg] dec */
  long int mjd; /* (input) mean julian date */
  long int mpm; /* (input) milliseconds past UTC midnight */ 
  double lat;   /* (input) [deg, +N] observer location */
  double lon;   /* (input) [deg, +E] observer location */ 
  double LAST;  /* (output) [h] local apparent sidereal time */
  double alt;   /* (output) [deg] calculated altitude */
  double az;    /* (output) [deg] calculated azimuth */ 

  /* Parse command line */
  if (narg>1) sscanf(argv[1],"%lf",&ra);
  if (narg>2) sscanf(argv[2],"%lf",&dec);
  if (narg>3) sscanf(argv[3],"%ld",&mjd);
  if (narg>4) sscanf(argv[4],"%ld",&mpm);
  if (narg>5) sscanf(argv[5],"%lf",&lat);
  if (narg>6) sscanf(argv[6],"%lf",&lon);     
  if (narg<7) {
    printf("Useage:\n");
    printf("me_getaltazx <ra> <dec> <mjd> <mpm> <lat> <lon>\n");
    printf("  <ra>:  [decimal h] RA\n");
    printf("  <dec>: [decimal dec] DEC\n");
    printf("  <mjd>: integer MJD\n");
    printf("  <mpm>: integer milliseconds past UTC midnight\n");
    printf("  <lat>: [deg, +E] observer location\n");
    printf("  <lon>: [deg, +E] observer location\n");
    return;
    }
  //printf("RA/DEC  = %6.2lf h %+6.2lf deg\n",ra,dec);
  //printf("MJD/MPM = %5ld %9ld\n",mjd,mpm);
  //printf("LAT/LON = %+6.2lf deg %+7.2lf deg\n",lat,lon); 

  me_getaltaz(
              ra,    /* (input) [h] RA */
              dec,   /* (input) [deg] dec */
              mjd,   /* (input) mean julian date */
              mpm,   /* (input) milliseconds past UTC midnight */ 
              lat,   /* (input) [deg, +E] observer location */
              lon,   /* (input) [deg, +N] observer location */ 
              &LAST, /* (output) [h] local apparent sidereal time */
              &alt,  /* (output) [deg] calculated altitude */
              &az    /* (output) [deg] calculated azimuth */ 
             );

  printf("LAST = %6.3lf h: ALT/AZ = %+8.3lf deg %+8.3lf deg\n",LAST,alt,az);

  return 0;
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// me_getaltazx.c: S.W. Ellingson, Virginia Tech, 2011 May 01
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================


