// me_findsol.c: S.W. Ellingson, Virginia Tech, 2012 Oct 04
// ---
// COMPILE: intended to be called from another program which it is compiled into
// ---
// COMMAND LINE: N/A
// ---
// REQUIRES: 
//   
// Find RA and DEC of the sun
// See end of this file for history.
#include "ephem_astro.h"  /* mostly gutted, leaving only stuff needed for others */
#include "ephem_vsop87.h"
#include "ephem_vsop87_data.c" 
#include "ephem_vsop87.c"
#include "ephem_sun.c"
#include "ephem_obliq.c"
#include "ephem_mjd.c"
#include "ephem_sphcart.c"

#include "ephem_eq_ecl.c"
#include "ephem_nutation.c"
#include "ephem_aberration.c"

#define MEFS_DTR 0.017453292520
#define MEFS_PI  3.141592653590

void me_findsol(
                 long int mjd, /* (input) mean julian date */
                 long int mpm, /* (input) milliseconds past UTC midnight */ 
                 float *ra,   /* (output) [h] RA */
                 float *dec   /* (output) [deg] dec */
                ) {

  double JD, H, JD0;
  double lambda, beta, rho;
  double dRA, dDec;

  /* Get JD from mjd/mpm */
  JD0 = ((double)mjd) + 2400000.5;     /* ref: http://tycho.usno.navy.mil/mjd.html */
  H   = ((double)mpm)/(3600.0*1000.0); /* mpm in hours */
  JD = JD0 + H/24.0; /* days */

  /* Locate the Sun */
  sunpos(JD-MJD0, &lambda, &rho, &beta);
  
  /* to equatorial coordinates */
  ecl_eq(JD-MJD0, beta, lambda, &dRA, &dDec);
  
  /* Apply nutation */
  nut_eq(JD-MJD0, &dRA, &dDec);
  
  /* Apply aberration */
  ab_eq(JD-MJD0, lambda, &dRA, &dDec);
  
  /* Back to floats */
  *ra = (float) radhr(dRA);
  *dec = (float) raddeg(dDec);
  
  return;
  } /* me_findsol() */

// me_findsol.c: S.W. Ellingson, Virginia Tech, 2012 Oct 04
//   -- initial version, using me_getaltaz.c as a starting point 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2012 Jan 21
//   -- corrections to algorithm 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2011 May 05
