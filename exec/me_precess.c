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

/* The codes below are ported from Xephem */
#include "ephem_astro.h"  /* mostly gutted, leaving only stuff needed for others */
#include "ephem_vsop87.h"
#include "ephem_vsop87_data.c" 
#include "ephem_vsop87.c"
#include "ephem_sun.c"
#include "ephem_obliq.c"
#include "ephem_mjd.c"
#include "ephem_sphcart.c"

#include "ephem_precess.c"
#include "ephem_nutation.c"
#include "ephem_aberration.c"

void me_precess(
                 long int mjd, /* (input) modified julian date */
                 long int mpm, /* (input) milliseconds past UTC midnight */ 
                 float *ra,   /* (input/output) [h] RA */
                 float *dec  /* (input/output) [deg] dec */
                ) {

   double JD, H, JD0;
  double lambda, rho, beta;
  double dRA, dDec;
  
  /* Get JD from mjd/mpm */
  JD0 = ((double)mjd) + 2400000.5;     /* ref: http://tycho.usno.navy.mil/mjd.html */
  H   = ((double)mpm)/(3600.0*1000.0); /* mpm in hours */
  JD = JD0 + H/24.0; /* days */

  /* Convert to radians */
  dRA = hrrad((double) *ra);
  dDec = degrad((double) *dec);
  
  /* Precess to the current epoch from J2000.0 */
  precess(J2000, JD-MJD0, &dRA, &dDec);
  
  /* Apply nutation */
  nut_eq(JD-MJD0, &dRA, &dDec);
  
  /* Locate the Sun */
  sunpos(JD-MJD0, &lambda, &rho, &beta);
  
  /* Apply aberration */
  ab_eq(JD-MJD0, lambda, &dRA, &dDec);
  
  /* Back to floats */
  *ra = (float) radhr(dRA);
  *dec = (float) raddeg(dDec);

  return;
  } /* me_precess */

// me_precess.c: J. Dowell, UNM, 2015 Aug 17
//   -- initial version, using me_findjov.c as a starting point 
