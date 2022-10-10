// me_getaltaz_x.c: J. Dowell, UNM, 2015 Sep 1
// ---
// COMPILE: gcc -o me_getaltaz_x me_getaltaz_x.c -I../common -lm
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
#include "me_astro.h"

int main ( int narg, char *argv[] ) {

  double ra;    /* (input) [h] RA */
  double dec;   /* (input) [deg] dec */
  double dist;  /* (input) [AU] distance */
  long int mjd; /* (input) mean julian date */
  long int mpm; /* (input) milliseconds past UTC midnight */ 
  double lat;   /* (input) [deg, +N] observer location */
  double lon;   /* (input) [deg, +E] observer location */ 
  double elev;  /* (input) [m] obsever elevation above sea level */
  double LAST;  /* (output) [h] local apparent sidereal time */
  double alt;   /* (output) [deg] calculated altitude */
  double az;    /* (output) [deg] calculated azimuth */ 
  double m, s;

  /* Parse command line */
  if (narg>1) sscanf(argv[1],"%lf",&ra);
  if (narg>2) sscanf(argv[2],"%lf",&dec);
  if (narg>3) sscanf(argv[3],"%lf",&dist);
  if (narg>4) sscanf(argv[4],"%ld",&mjd);
  if (narg>5) sscanf(argv[5],"%ld",&mpm);
  if (narg>6) sscanf(argv[6],"%lf",&lat);
  if (narg>7) sscanf(argv[7],"%lf",&lon);     
  if (narg>8) sscanf(argv[8],"%lf",&elev);
  if (narg<9) {
    printf("Useage:\n");
    printf("me_getaltaz_x <ra> <dec> <dist> <mjd> <mpm> <lat> <lon> <elev>\n");
    printf("  <ra>  : [decimal h] RA\n");
    printf("  <dec> : [decimal dec] DEC\n");
    printf("  <dist>: [AU] distnace from Earth\n");
    printf("  <mjd> : integer MJD\n");
    printf("  <mpm> : integer milliseconds past UTC midnight\n");
    printf("  <lat> : [deg, +N] observer location\n");
    printf("  <lon> : [deg, +E] observer location\n");
    printf("  <elev>: [m] observation location\n");
    return;
    }
  //printf("RA/DEC/DIST  = %6.2lf h %+6.2lf deg %.2lf AU\n",ra,dec,dist);
  //printf("MJD/MPM = %5ld %9ld\n",mjd,mpm);
  //printf("LAT/LON/ELEV = %+6.2lf deg %+7.2lf deg %7.2 m\n",lat,lon,elev); 

  me_getaltaz(
              ra,    /* (input) [h] RA */
              dec,   /* (input) [deg] dec */
              dist,  /* (input) [AU] distance from Earth */
              mjd,   /* (input) mean julian date */
              mpm,   /* (input) milliseconds past UTC midnight */ 
              lat,   /* (input) [deg, +E] observer location */
              lon,   /* (input) [deg, +N] observer location */ 
              elev,  /* (input) [m] observation location */
              &LAST, /* (output) [h] local apparent sidereal time */
              &alt,  /* (output) [deg] calculated altitude */
              &az    /* (output) [deg] calculated azimuth */ 
             );

  m = (LAST - floor(LAST))*60.0;
  s = (m - floor(m))*60.0;
  printf("LAST = %8.3lf h    =   %02.0lf h %02.0lf m %05.2lf s\n", LAST, floor(LAST), floor(m), s);
  m = (fabs(alt)-floor(fabs(alt)))*60.0;
  s = (m - floor(m))*60.0;
  printf("ALT  = %+8.3lf deg  =  %+03.0lf d %02.0lf m %04.2lf s\n", alt, (alt/fabs(alt))*floor(fabs(alt)), floor(m), s);
  m = (az- floor(az))*60.0;
  s = (m - floor(m))*60.0;
  printf("AZ   = %8.3lf deg  =  %03.0lf d %02.0lf m %05.2lf s\n", az, floor(az), floor(m), s);

  return 0;
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// me_getaltaz_x.c: J. Dowell, UNM, 2015 Sep 1
//   .1: Updated for the new structure of me_getaltaz and changed the output 
//      formatting.
// me_getaltaz_x.c: S.W. Ellingson, Virginia Tech, 2011 May 01
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================


