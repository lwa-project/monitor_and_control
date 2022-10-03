// me_findlun.c: J. Dowell, UNM, 2022 Sep 30
// ---
// COMPILE: intended to be called from another program which it is compiled into
// ---
// COMMAND LINE: N/A
// ---
// REQUIRES: 
//   
// Find RA and DEC of the Moon
// See end of this file for history.

/* The codes below are ported from Xephem */
#include "ephem_astro.h"  /* mostly gutted, leaving only stuff needed for others */

void me_findlun(
                 long int mjd,  /* (input) modified julian date */
                 long int mpm,  /* (input) milliseconds past UTC midnight */ 
                 float *ra,     /* (output) [h] RA */
                 float *dec,    /* (output) [deg] dec */
                 float *dist    /* (output) [AU] distance from Earth */
                ) {
  double JD, H, JD0, TJD;
  double ret[6]; /* this is how vsop87() returns output */
  double L,  B,  R, msp, mdp;
  double dRA, dDec;
  
  /* Get JD from mjd/mpm */
  JD0 = ((double)mjd) + 2400000.5;     /* ref: http://tycho.usno.navy.mil/mjd.html */
  H   = ((double)mpm)/(3600.0*1000.0); /* mpm in hours */
  JD = JD0 + H/24.0; /* days */
  TJD = JD + deltat(JD-MJD0)/86400.0;

  /* Get L, B, R for the Moon so we can figure out how far away it is */
  moon (TJD-MJD0, &L, &B, &R, &msp, &mdp);
  
  /* Correct L, B, and R for the light travel time by looking back in time a bit*/
  moon (TJD-MJD0-R/173.144633, &L, &B, &R, &msp, &mdp);
  //printf("me_findlun(): L =%lf rad, B =%lf rad, R =%lf AU\n",L,B,R);
  
  /* to equatorial coordinates */
  ecl_eq (TJD-MJD0, B, L, &dRA, &dDec);
  
  /* Apply nutation */
  nut_eq (TJD-MJD0, &dRA, &dDec);
  
  /* Back to floats */
  *ra = (float) radhr(dRA);
  *dec = (float) raddeg(dDec);
  *dist = (float) R;

  return;
  } /* me_findlun */

// me_findlun.c: J. Dowell, UNM, 2022 Sep 30
//  -- initial version, using me_getjov.c as a starting point 
// me_findjov.c: J. Dowell, UNM, 2015 Sep 1
//  -- changed the call so that the distance to Jupiter in AU is also returned
// me_findjov.c: J. Dowell, UNM, 2015 Aug 31
//   -- updated to do everything through XEphem
// me_findjov.c: S.W. Ellingson, Virginia Tech, 2012 Oct 07
//   -- initial version, using me_getaltaz.c as a starting point 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2012 Jan 21
//   -- corrections to algorithm 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2011 May 05