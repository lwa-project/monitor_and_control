// me_findjov.c: J. Dowell, UNM, 2015 Sep 1
// ---
// COMPILE: intended to be called from another program which it is compiled into
// ---
// COMMAND LINE: N/A
// ---
// REQUIRES: 
//   
// Find the geocentric apparent RA and DEC of Jupiter
// See end of this file for history.

#include "sofa.h"
#include "sofam.h"

void me_findjov(
                 long int mjd, /* (input) modified julian date */
                 long int mpm, /* (input) milliseconds past UTC midnight */ 
                 float *ra,    /* (output) [h] RA */
                 float *dec,   /* (output) [deg] dec */
                 float *dist   /* (output) [AU] distance from Earth */
                ) {

  double tai1, tai2, tt1, tt2;
  double pv[2][3], pve[2][3], pvb[2][3], tpv[2][3];
  double L,  B,  R, dL, dB, dR;
  int i,j;
  double pmat[3][3], nmat[3][3];
  double R2;
  
  /* Get TAI from mjd/mpm */
  iauUtctai(mjd+DJM0, mpm/1000.0/86400, &tai1, &tai2);
  
  /* Get TT from TAI */
  iauTaitt(tai1, tai2, &tt1, &tt2);
  
  /* Get the heliocentric position of Jupiter and the Earth */
  iauPlan94(tt1, tt2, 5, pv);
  iauEpv00(tt1, tt2, pve, pvb);
  
  /* Un-correct for aberration */
  iauASTROM astrom;
  iauApcg13(tt1, tt2, &astrom);
  
  /* Get the geocentric position of Jupiter */
  for(i=0; i<3; i++) {
    pv[0][i] -= pve[0][i];
    pv[1][i] -= pve[1][i];
  }
  iauPv2s(pv, &L, &B, &R, &dL, &dB, &dR);
  
  /* Get the geocentric position of Jupiter corrected for light travel time */
  iauPlan94(tt1, tt2 - R/DC, 5, pv);
  
  /* Get the barycentric position of Jupiter corrected for light travel time */
  for(i=0; i<3; i++) {
    pv[0][i] -= pve[0][i];
    pv[1][i] -= pve[1][i];
  }
  iauPv2s(pv, &L, &B, &R, &dL, &dB, &dR);
  
  /* Apply relativistic deflection */
  iauLdsun(pv[0], astrom.eh, astrom.em, &tpv[0][0]);
 
  /* Apply aberration */
  iauAb(tpv[0], astrom.v, astrom.em, astrom.bm1, &pv[0][0]);
  
  /* Apply precession and nutation */
  iauPmat06(tt1, tt2, &pmat[0]);
  iauNum06a(tt1, tt2, &nmat[0]);
  iauRxpv(pmat, pv, &tpv[0]);
  iauRxpv(nmat, tpv, &pv[0]);
  iauPv2s(pv, &L, &B, &R2, &dL, &dB, &dR);
  
  /* Cleanup */
  L = iauAnp(L);
  
  /* Back to floats */
  *ra = (float) L * DR2D / 15;
  *dec = (float) B * DR2D;
  *dist = (float) R;
  
  } /* me_findjov */

// me_findjov.c: J. Dowell, UNM, 2022 Oct 7
//  -- updated for the SOFA library
// me_findjov.c: J. Dowell, UNM, 2022 Oct 3
//  -- add in gravitational deflection near the Sun
// me_findjov.c: J. Dowell, UNM, 2015 Sep 1
//  -- changed the call so that the distance to Jupiter in AU is also returned
// me_findjov.c: J. Dowell, UNM, 2015 Aug 31
//   -- updated to do everything through XEphem
// me_findjov.c: S.W. Ellingson, Virginia Tech, 2012 Oct 07
//   -- initial version, using me_getaltaz.c as a starting point 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2012 Jan 21
//   -- corrections to algorithm 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2011 May 05
