// me_findjov.c: J. Dowell, UNM, 2015 Sep 1
// ---
// COMPILE: intended to be called from another program which it is compiled into
// ---
// COMMAND LINE: N/A
// ---
// REQUIRES: 
//   
// Find RA and DEC of Jupiter
// See end of this file for history.

/* The codes below are ported from Xephem */
#include "ephem_astro.h"  /* mostly gutted, leaving only stuff needed for others */

void me_findjov(
                 long int mjd, /* (input) modified julian date */
                 long int mpm, /* (input) milliseconds past UTC midnight */ 
                 float *ra,    /* (output) [h] RA */
                 float *dec,   /* (output) [deg] dec */
                 float *dist   /* (output) [AU] distance from Earth */
                ) {

  double JD, H, JD0;
  double ret[6]; /* this is how vsop87() returns output */
  double L,  B,  R;
  double L0, B0, R0;
  double x,y,z;
  double x0,y0,z0;
  double lambda, beta, rho;
  double lambdaHelio, betaHelio, rhoHelio;
  double lambdaSun, rhoSun, betaSun;
  double dRA, dDec;

  /* Get JD from mjd/mpm */
  JD0 = ((double)mjd) + 2400000.5;     /* ref: http://tycho.usno.navy.mil/mjd.html */
  H   = ((double)mpm)/(3600.0*1000.0); /* mpm in hours */
  JD = JD0 + H/24.0; /* days */

  /* Get L, B, R for Jupiter so we can figure out how far away it is */
  vsop87 ( JD-MJD0, JUPITER, 0, ret);
  L = ret[0]; /* [rad] */
  B = ret[1]; /* [rad] */
  R = ret[2]; /* [AU] */
  
  /* Correct L, B, and R for the light travel time by looking back in time a bit*/
  vsop87 ( JD-MJD0-R/173.144633, JUPITER, 0, ret);
  L = ret[0]; /* [rad] */
  B = ret[1]; /* [rad] */
  R = ret[2]; /* [AU] */
  //printf("me_findjov(): L =%lf rad, B =%lf rad, R =%lf AU\n",L,B,R);
  
  /* Convert the location of number to cartesian x, y, z */
  sphcart(L, B, R, &x, &y, &z);

  /* Get L0, B0, R0 for Earth and convert to cartesian x0, y0, z0 */
  vsop87 ( JD-MJD0,       8, 0, ret);
  L0 = ret[0]; /* [rad] */
  B0 = ret[1]; /* [rad] */
  R0 = ret[2]; /* [AU] */
  sphcart(L0, B0, R0, &x0, &y0, &z0);
  //printf("me_findjov(): L0=%lf rad, B0=%lf rad, R0=%lf AU\n",L0,B0,R0);
  
//   /* to ecliptical heliocentric coordinates */
//   cartsph(x, y, z, &lambdaHelio, &betaHelio, &rhoHelio);
  
  /* to ecliptical geocentric coordinates */
  cartsph(x-x0, y-y0, z-z0, &lambda, &beta, &rho);
  
  /* to equatorial coordinates */
  ecl_eq(JD-MJD0, beta, lambda, &dRA, &dDec);
  
  /* Locate the Sun */
  sunpos(JD-MJD0, &lambdaSun, &rhoSun, &betaSun);
  
//   /* Apply relativistic deflection */
//   deflect(JD-MJD0, lambdaHelio, betaHelio, lambdaSun, rhoSun, rho, &dRA, &dDec);
 
  /* Apply nutation */
  nut_eq(JD-MJD0, &dRA, &dDec);
  
  /* Apply aberration */
  ab_eq(JD-MJD0, lambdaSun, &dRA, &dDec);
  
  /* Back to floats */
  *ra = (float) radhr(dRA);
  *dec = (float) raddeg(dDec);
  *dist = (float) rho;

  return;
  } /* me_findjov */

// me_findjov.c: J. Dowell, UNM, 2015 Sep 1
//  -- changed the call so that the distance to Jupiter in AU is also returned
// me_findjov.c: J. Dowell, UNM, 2015 Aug 31
//   -- updated to do everything through XEphem
// me_findjov.c: S.W. Ellingson, Virginia Tech, 2012 Oct 07
//   -- initial version, using me_getaltaz.c as a starting point 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2012 Jan 21
//   -- corrections to algorithm 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2011 May 05
