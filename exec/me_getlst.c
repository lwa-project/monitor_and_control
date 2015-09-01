// COMPILE: intended to be called from another program which it is compiled into
// ---
// COMMAND LINE: N/A
// ---
// REQUIRES: 
//   
// Find the local apparent sidereal time for a location

#include <math.h>

#include "ephem_astro.h"  /* mostly gutted, leaving only stuff needed for others */

void me_getlst(
               long int mjd, /* (input) mean julian date */
               long int mpm, /* (input) milliseconds past UTC midnight */ 
               double lat,   /* (input) [deg, +N] observer location */
               double lon,   /* (input) [deg, +E] observer location */ 
               double *LAST /* (output) [h] local apparent sidereal time */
              ) {

  double JD, H, JD0;
  double lst, eps, deps, dpsi;

  /* Get JD from mjd/mpm */
  JD0 = ((double)mjd) + 2400000.5;     /* ref: http://tycho.usno.navy.mil/mjd.html */
  H   = ((double)mpm)/(3600.0*1000.0); /* mpm in hours */
  JD = JD0 + H/24.0; /* days */

  /* Get the Greenwich Mean Sidereal Time */
  utc_gst(mjd_day(JD-MJD0), mjd_hr(JD-MJD0),  &lst);
  
  /* Convert to Local Mean Sidereal Time */
  lst += radhr(degrad(lon));
  
  /* Convert from mean to apparent */
  obliquity(JD-MJD0, &eps);
  nutation(JD-MJD0, &deps, &dpsi);
  lst += radhr(dpsi*cos(eps+deps));
  ephem_range (&lst, 24.0);
  
  *LAST = lst;
  
  return;
  } /* me_getlst() */

// me_getlst.c: J. Dowell, UNM, 2015 Sep 1
//   -- initial version
