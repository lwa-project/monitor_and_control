// COMPILE: intended to be called from another program which it is compiled into
// ---
// COMMAND LINE: N/A
// ---
// REQUIRES: 
//   
// Find the local apparent sidereal time for a location

#include <math.h>
#include "sofa.h"
#include "sofam.h"

void me_getlst(
               long int mjd, /* (input) mean julian date */
               long int mpm, /* (input) milliseconds past UTC midnight */ 
               double lat,   /* (input) [deg, +N] observer location */
               double lon,   /* (input) [deg, +E] observer location */ 
               double *LAST /* (output) [h] local apparent sidereal time */
              ) {

  double tai1, tai2, tt1, tt2;
  
  /* Get TAI from mjd/mpm */
  iauUtctai(mjd+DJM0, mpm/1000.0/86400, &tai1, &tai2);
  
  /* Get TT from TAI */
  iauTaitt(tai1, tai2, &tt1, &tt2);
  
  /* Get GAST */
  *LAST = iauGst06a(mjd+DJM0, mpm/1000.0/86400, tt1, tt2);
  
  /* Convert to radians */
  lat *= DD2R;
  lon *= DD2R;
  
  /* Get the LST */
  *LAST += lon;
  while( *LAST < 0) *LAST += D2PI;
  while( *LAST > D2PI) *LAST -= D2PI;
  *LAST *= DR2D / 15;
  
  } /* me_getlst() */

// me_getlst.c: J. Dowell, UNM, 2022 Oct 7
//   -- updated to use the SOFA library
// me_getlst.c: J. Dowell, UNM, 2015 Sep 1
//   -- initial version
