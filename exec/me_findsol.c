// me_findsol.c: J. Dowell, UNM, 2015 Sep 1
// ---
// COMPILE: intended to be called from another program which it is compiled into
// ---
// COMMAND LINE: N/A
// ---
// REQUIRES: 
//   
// Find the geocentric apparent RA and DEC of the Sun
// See end of this file for history.

#include "sofa.h"
#include "sofam.h"

void me_findsol(
                 long int mjd,  /* (input) modified julian date */
                 long int mpm,  /* (input) milliseconds past UTC midnight */ 
                 double *ra,    /* (output) [h] RA */
                 double *dec,   /* (output) [deg] dec */
                 double *dist   /* (output) [AU] distance from Earth */
                ) {

  double tai1, tai2, tt1, tt2;
  double pv[2][3], pvb[2][3], tpv[2][3];
  double L,  B,  R, dL, dB, dR;
  double pnmat[3][3];
  
  /* Get TAI from mjd/mpm */
  iauUtctai(mjd+DJM0, mpm/1000.0/86400, &tai1, &tai2);
  
  /* Get TT from TAI */
  iauTaitt(tai1, tai2, &tt1, &tt2);
  
  /* Get the position of the Earth in spherical coordinates */
  iauEpv00(tt1, tt2, &pv[0], &pvb[0]);
  iauPv2s(pv, &L, &B, &R, &dL, &dB, &dR);

  /* Get the position of the Earth in spherical coordinates corrected for */
  /* light travel time from the Sun.*/
  iauEpv00(tt1, tt2 - R/DC, &pv[0], &pvb[0]);
  
  /* Flip it around to be for the Sun */
  iauSxpv(-1.0, &pv[0], &pv[0]);
  iauPv2s(pv, &L, &B, &R, &dL, &dB, &dR);
  
  /* Apply precession and nutation */
  iauPnm06a(tt1, tt2, &pnmat[0]);
  iauRxpv(pnmat, pv, &tpv[0]);
  iauPv2s(tpv, &L, &B, &R, &dL, &dB, &dR);
  
  /* Apply aberration */
  // Already done for us apparently
  
  /* Cleanup */
  L = iauAnp(L);
  
  *ra = L * (DR2D / 15);
  *dec = B * DR2D;
  *dist = R;
  
  } /* me_findsol() */

// me_findsol.c: J. Dowell, UNM, 2022 Oct 7
//  -- updated to use the SOFA library
// me_findsol.c: J. Dowell, UNM, 2015 Sep 1
//  -- changed the call so that the distance to the Sun in AU is also returned
// me_findsol.c: J. Dowell, UNM, 2015 Aug 31
//   -- updated to do everything through XEphem
// me_findsol.c: S.W. Ellingson, Virginia Tech, 2012 Oct 04
//   -- initial version, using me_getaltaz.c as a starting point 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2012 Jan 21
//   -- corrections to algorithm 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2011 May 05
