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
#include "sofa.h"
#include "sofam.h"

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
  double GAST, fRA, fDec, eo;
  double r[3][3], x, y, s;
  double HA, tHA, tDec;
  
  /* Get TAI from mjd/mpm */
  iauUtctai(mjd+DJM0, mpm/1000.0/86400, &tai1, &tai2);
  
  /* Get TT from TAI */
  iauTaitt(tai1, tai2, &tt1, &tt2);
  
  /* Get GAST */
  GAST = iauGst06a(mjd+DJM0, mpm/1000.0/86400, tt1, tt2);
  
  /* Convert to radians */
  lat *= DD2R;
  lon *= DD2R;
  ra *= 15 * DD2R;
  dec *= DD2R;
  
  /* Get the LST */
  *LAST = iauAnp(GAST + lon);
  *LAST *= DR2D / 15;
  
  /* Switch to CIO based positions */
  // Equaninox-based BPN matrix...
  iauPnm06a(tt1, tt2, &r[0]);
  // Celestial intermediate pole (CIP) location...
  iauBpn2xy(r, &x, &y);
  // Celestrial intermediate origin locator...
  s = iauS06(tt1, tt2, x, y);
  // ... and add the equation of origins to the RA
  ra += iauEors(r, s);
  
  /* Parallax from the geocenter to the observer */
  double obs[2][3], obj[3];
  if( dist < 1e3 ) {
    iauPvtob(lon, lat, elev, \
             0.0, 0.0, s, GAST, \
             &obs[0]);
    iauS2p(ra, dec, dist, &obj[0]);
    obj[0] -= obs[0][0]/DAU;
    obj[1] -= obs[0][1]/DAU;
    obj[2] -= obs[0][2]/DAU;
    iauP2s(obj, &ra, &dec, &dist);
  }
  
  /* Get the topocentric coordinates */
  iauAtio13(ra, dec, \
            mjd+DJM0, mpm/1000.0/86400, 0.0, \
            lon, lat, elev, 0.0, 0.0, \
            0.0, 0.0, 0.0, 0.0,
            az, alt, &HA, \
            &fDec, &fRA);
            
  /* Zenith angle to altitude (elevation) */
  *alt = DPI/2 - *alt;
  
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
