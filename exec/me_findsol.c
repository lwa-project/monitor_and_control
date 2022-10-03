// me_findsol.c: J. Dowell, UNM, 2015 Sep 1
// ---
// COMPILE: intended to be called from another program which it is compiled into
// ---
// COMMAND LINE: N/A
// ---
// REQUIRES: 
//   
// Find RA and DEC of the sun
// See end of this file for history.

#include <math.h>

/* The codes below are ported from Xephem */
#include "ephem_astro.h"  /* mostly gutted, leaving only stuff needed for others */

void me_findsol(
                 long int mjd,  /* (input) modified julian date */
                 long int mpm,  /* (input) milliseconds past UTC midnight */ 
                 double lat,    /* (input) observer latitude [deg, +N] */
                 double lng,    /* (input) observer longitude [deg, +E] */
                 double elev,   /* (input) observer height above sea level [m] */
                 float *ra,     /* (output) [h] RA */
                 float *dec,    /* (output) [deg] dec */
                 float *dist    /* (output) [AU] distance from Earth */
                ) {

  double JD, H, JD0, TJD;
  double lambda, beta, rho;
  double dRA, dDec;
  double eps, lst, deps, dpsi;
  double ha_in, rho_topo, ha_out, dec_out;
  
  /* Get JD from mjd/mpm */
  JD0 = ((double)mjd) + 2400000.5;     /* ref: http://tycho.usno.navy.mil/mjd.html */
  H   = ((double)mpm)/(3600.0*1000.0); /* mpm in hours */
  JD = JD0 + H/24.0; /* days */
  TJD = JD + deltat(JD-MJD0)/86400.0;
  
  /* Locate the Sun */
  sunpos(TJD-MJD0, &lambda, &rho, &beta);
  
  /* to equatorial coordinates */
  ecl_eq(TJD-MJD0, beta, lambda, &dRA, &dDec);
  
  /* Apply nutation */
  nut_eq(TJD-MJD0, &dRA, &dDec);
  
  /* Apply aberration */
  ab_eq(TJD-MJD0, lambda, &dRA, &dDec);
  
  /* Correct for parallax from the Earth's center to the observer */
  utc_gst (mjd_day (JD-MJD0), mjd_hr (JD-MJD0), &lst);
  lst += radhr(lng * M_PI/180);
  obliquity (JD-MJD0, &eps);
  nutation (JD-MJD0, &deps, &dpsi);
  lst += radhr(dpsi*cos(eps+deps));
  ephem_range (&lst, 24.0);
  
  ha_in = hrrad(lst) - dRA;
  rho_topo = rho * MAU/ERAD;
  ta_par (ha_in, dDec, lat * M_PI/180, elev / ERAD, &rho_topo, &ha_out, &dec_out);
  
  dRA = hrrad(lst) - ha_out;
  ephem_range (&dRA, 2*M_PI);
  dDec = dec_out;
  
  /* Back to floats */
  *ra = (float) radhr(dRA);
  *dec = (float) raddeg(dDec);
  *dist = (float) rho;
  
  return;
  } /* me_findsol() */

// me_findsol.c: J. Dowell, UNM, 2015 Sep 1
//  -- changed the call so that the distance to the Sun in AU is also returned
// me_findsol.c: J. Dowell, UNM, 2015 Aug 31
//   -- updated to do everything through XEphem
// me_findsol.c: S.W. Ellingson, Virginia Tech, 2012 Oct 04
//   -- initial version, using me_getaltaz.c as a starting point 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2012 Jan 21
//   -- corrections to algorithm 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2011 May 05
