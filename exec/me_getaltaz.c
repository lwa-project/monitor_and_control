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

#include "ephem_astro.h"

#define DTR 0.017453292520

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

  double JD, H, JD0;
  double lst, eps, deps, dpsi;
  double HA, tHA, tDec;

  /* Get JD from mjd/mpm */
  JD0 = ((double)mjd) + 2400000.5;     /* ref: http://tycho.usno.navy.mil/mjd.html */
  H   = ((double)mpm)/(3600.0*1000.0); /* mpm in hours */
  JD = JD0 + H/24.0; /* days */

  /* Get the Greenwich Mean Sidereal Time */
  utc_gst(mjd_day(JD-MJD0), mjd_hr(JD-MJD0),  LAST);
  
  /* Convert to Local Mean Sidereal Time */
  *LAST += radhr(degrad(lon));
  
  /* Convert from mean to apparent */
  obliquity(JD-MJD0, &eps);
  nutation(JD-MJD0, &deps, &dpsi);
  *LAST += radhr(dpsi*cos(eps+deps));
  ephem_range (LAST, 24.0);
  
  /* Hour angle */
  /* http://en.wikipedia.org/wiki/Hour_angle */
  HA = hrrad(*LAST-ra); /* [rad] */ 
  ephem_range (&HA, 2.0*PI);
  
  /* More unit conversions */
  dec = degrad(dec);
  lat = degrad(lat);
  lon = degrad(lon);
  
  /* Deal with parallax if this object is "close" */
  if ( dist < 1000 ) {
     dist = dist * MAU/ERAD;		// AU -> m -> Earth radii
     elev = elev / ERAD;			// m -> Earth radii
     ta_par(HA, dec, lat, elev, &dist, &tHA, &tDec);
     
     HA = tHA;
     dec = tDec;
  }
  
  /* Convert to Alt/Az */
  /* http://en.wikipedia.org/wiki/Celestial_coordinate_system */
  *alt = asin(   sin(lat)*sin(dec) + cos(lat)*cos(dec)*cos(HA)              );
  *az  = acos( ( cos(lat)*sin(dec) - sin(lat)*cos(dec)*cos(HA)) / cos(*alt) );
  if (HA<PI) { *az = 2*PI - *az; }
  
  /* Back to degrees */
  *alt = raddeg(*alt);
  *az = raddeg(*az);

  /* make sure az is in range 0--> 360 */
  ephem_range (az, 360.0);

  //AZ/ALT from RA/DEC
  //-- sin ALT = sin LAT sin DEC + cos LAT cos DEC cos H 
  //-- cos AZ = ( cos LAT sin DEC - sin LAT cos DEC cos H ) / cos ALT
  //  if H < pi then AZ = 360 - AZ

  //printf("JD=%lf [days]\n",JD); 
  //printf("GMST=%lf [h]\n",GMST);
  //printf("GAST=%lf [h]\n",GAST);

  return;
  } /* me_getaltaz */

// me_getaltaz.c: J. Dowell, UNM, 2015 Sep 1
// -- Changed to providing topocentric apparent positions by including parallax
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2012 Oct 07
// -- Making sure azimuth is in range [0,360]
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2012 Jan 21
//   -- corrections to algorithm 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2011 May 05
