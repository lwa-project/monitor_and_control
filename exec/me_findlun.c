// me_findlun.c: J. Dowell, UNM, 2022 Sep 30
// ---
// COMPILE: intended to be called from another program which it is compiled into
// ---
// COMMAND LINE: N/A
// ---
// REQUIRES: 
//   
// Find the geocentric apparent RA and DEC of the Moon
// See end of this file for history.

#include "sofa.h"
#include "sofam.h"

void me_findlun(
                 long int mjd,  /* (input) modified julian date */
                 long int mpm,  /* (input) milliseconds past UTC midnight */ 
                 double *ra,    /* (output) [h] RA */
                 double *dec,   /* (output) [deg] dec */
                 double *dist   /* (output) [AU] distance from Earth */
                ) {
  double tai1, tai2, tt1, tt2;
  double pv[2][3], tpv[2][3];
  double L,  B,  R, dL, dB, dR;
  double pmat[3][3], nmat[3][3];
  
  /* Get TAI from mjd/mpm */
  iauUtctai(mjd+DJM0, mpm/1000.0/86400, &tai1, &tai2);
  
  /* Get TT from TAI */
  iauTaitt(tai1, tai2, &tt1, &tt2);
  
  /* Get the geocentric position of the Moon in spherical coordinates */
  iauMoon98(tt1, tt2, &pv[0]);
  iauPv2s(pv, &L, &B, &R, &dL, &dB, &dR);
  
  /* Get the geocentric position of the Moon corrected for light travel time */
  iauMoon98(tt1, tt2 - R/DC, &pv[0]);
  
  /* Apply precession and nutation */
  iauPmat06(tt1, tt2, &pmat[0]);
  iauNum06a(tt1, tt2, &nmat[0]);
  iauRxpv(pmat, pv, &tpv[0]);
  iauRxpv(nmat, tpv, &pv[0]);
  iauPv2s(pv, &L, &B, &R, &dL, &dB, &dR);
  
  /* Cleanup */
  L = iauAnp(L);
  
  *ra = L * (DR2D / 15);
  *dec = B * DR2D;
  *dist = R;
  
  } /* me_findlun */

// me_findlun.c: J. Dowell, UNM, 2022 Oct 7
//  -- updated for the SOFA library
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
