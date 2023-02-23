// me_precess.c: J. Dowell, UNM, 2015 Aug 17
// ---
// COMPILE: intended to be called from another program which it is compiled into
// ---
// COMMAND LINE: N/A
// ---
// REQUIRES: 
//   
// Precesses a RA/Dec coordinate pair from J2000.0 to the current epoch.  Also, 
// apply corrections for nutation and aberration to the coordinates.

#include <stdio.h>
#include "sofa.h"
#include "sofam.h"

void me_precess(
                 long int mjd, /* (input) modified julian date */
                 long int mpm, /* (input) milliseconds past UTC midnight */ 
                 double *ra,   /* (input/output) [h] RA */
                 double *dec   /* (input/output) [deg] dec */
                ) {

  double tai1, tai2, tt1, tt2;
  double dRA, dDec, fRA, fDec, eo;
  
  /* Get TAI from mjd/mpm */
  int status = iauUtctai(mjd+DJM0, mpm/1000.0/86400, &tai1, &tai2);
  if( status == 1 ) {
    printf("WARNING: iauUtctai returned 1 - dubious year\n");
  }
  
  /* Get TT from TAI */
  iauTaitt(tai1, tai2, &tt1, &tt2);
  
  /* Convert to radians */
  dRA = (double) *ra * 15 * DD2R;
  dDec = (double) *dec * DD2R;
  
  /* Move into the CIRS */
  iauAtci13(dRA, dDec, \
            0.0, 0.0, 0.0, 0.0, \
            tt1, tt2, \
            &fRA, &fDec, &eo);
            
  /* Swith to equinox based positions */
  fRA = iauAnp(fRA - eo);
  
  /* Back to floats */
  *ra = fRA * (DR2D / 15);
  *dec = fDec * DR2D;
  
  } /* me_precess */

// me_precess.c: J. Dowell, UNM, 2022 Oct 6
//   -- converted to the SOFA library
// me_precess.c: J. Dowell, UNM, 2015 Aug 17
//   -- initial version, using me_findjov.c as a starting point 
