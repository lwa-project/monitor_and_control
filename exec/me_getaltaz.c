// me_getaltaz.c: J. Dowell, UNM, 2015 Sep 1
// ---
// COMPILE: intended to be called from another program which it is compiled into
// ---
// COMMAND LINE: N/A
// ---
// REQUIRES: 
//   
// See end of this file for history.

#include <math.h>
#include <stdio.h>
#include "sofa.h"
#include "sofam.h"
#include "me_astro.h"

void me_getaltaz(
                 double ra,    /* (input) [h] RA */
                 double dec,   /* (input) [deg] dec */
                 double dist,  /* (input) [AU] distance to body */
                 long int mjd, /* (input) mean julian date */
                 long int mpm, /* (input) milliseconds past UTC midnight */ 
                 double lat,   /* (input) [deg, +N] observer location */
                 double lon,   /* (input) [deg, +E] observer location */ 
                 double elev,  /* (input) [m] observer elevation above sea level */
                 double *LAST, /* (output) [h] local apparent sidereal time */
                 double *alt,  /* (output) [deg] calculated altitude */
                 double *az    /* (output) [deg] calculated azimuth */ 
                ) {

  double tai1, tai2, tt1, tt2;
  double GAST, xp, yp, dut, fRA, fDec, eo;
  double r[3][3], x, y, s;
  double HA, tHA, tDec;
  
  /* Get TAI from mjd/mpm */
  int status = iauUtctai(mjd+DJM0+mpm/1000.0/86400, 0.0, &tai1, &tai2);
  if( status == 1 ) {
    printf("WARNING: iauUtctai returned 1 - dubious year\n");
  }
  
  /* Get TT from TAI */
  iauTaitt(tai1, tai2, &tt1, &tt2);
  
  /* Get GAST */
  GAST = iauGst06a(mjd+DJM0, mpm/1000.0/86400, tt1, tt2);
  
  /* Convert to radians */
  lat *= DD2R;
  lon *= DD2R;
  ra *= 15 * DD2R;
  dec *= DD2R;
  
  /* Switch to CIO based positions */
  // Equaninox-based BPN matrix...
  iauPnm06a(tt1, tt2, &r[0]);
  // Celestial intermediate pole (CIP) location...
  iauBpn2xy(r, &x, &y);
  // Celestrial intermediate origin locator...
  s = iauS06(tt1, tt2, x, y);
  // ... and add the equation of origins to the RA
  ra = iauAnp(ra + iauEors(r, s));
  
  /* Get the Earth orientation parameters for the day */
  status = me_geteop(mjd, mpm, &xp, &yp, &dut);
  if( status == 1 ) {
    printf("WARNING: me_geteop returned 1 - MJD/MPM out of range of table\n");
  }
  
  /* Parallax from the geocenter to the observer */
  double sp, era, obs[2][3], obj[3];
  if( dist < 1e3 ) {
    sp = iauSp00(tt1, tt2);
    era = iauEra00(mjd+DJM0, mpm/1000.0/86400);
    iauPvtob(lon, lat, elev, \
             xp, yp, sp, era, \
             &obs[0]);
    iauS2p(ra, dec, dist, &obj[0]);
    iauSxpv(1/DAU, obs, &obs[0]);
    iauPmp(obj, obs[0], &obj[0]);
    iauP2s(obj, &ra, &dec, &dist);
  }
  
  /* Get the LST */
  *LAST = iauAnp(GAST + lon);
  *LAST *= DR2D / 15;
  
  /* Get the topocentric coordinates */
  iauAtio13(ra, dec, \
            DJM0, mjd+mpm/1000.0/86400, dut, \
            lon, lat, elev, xp * (DD2R/3600), yp * (DD2R/3600), \
            0.0, 0.0, 0.0, 0.0,
            az, alt, &HA, \
            &fDec, &fRA);
            
  /* Zenith angle to altitude (elevation) */
  *alt = DPI/2 - *alt;
  
  int idmsf[4];
  char sg='+';
  iauA2af(2, *alt, &sg, idmsf);
  printf("%c%i d %02i d %02i.%02i\n", sg, idmsf[0], idmsf[1], idmsf[2], idmsf[3]);
  
  /* Back to degrees */
  *alt *= DR2D;
  *az *= DR2D;
  
  } /* me_getaltaz */

// me_getaltaz.c: J. Dowell, UNM, 2015 Sep 1
// -- Changed to providing topocentric apparent positions by including parallax
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2012 Oct 07
// -- Making sure azimuth is in range [0,360]
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2012 Jan 21
//   -- corrections to algorithm 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2011 May 05
