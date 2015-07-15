// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2012 Oct 07
// ---
// COMPILE: intended to be called from another program which it is compiled into
// ---
// COMMAND LINE: N/A
// ---
// REQUIRES: 
//   
// See end of this file for history.
 
#define DTR 0.017453292520
#define PI  3.141592653590

void me_getaltaz(
                 double ra,    /* (input) [h] RA */
                 double dec,   /* (input) [deg] dec */
                 long int mjd, /* (input) mean julian date */
                 long int mpm, /* (input) milliseconds past UTC midnight */ 
                 double lat,   /* (input) [deg, +N] observer location */
                 double lon,   /* (input) [deg, +E] observer location */ 
                 double *LAST, /* (output) [h] local apparent sidereal time */
                 double *alt,  /* (output) [deg] calculated altitude */
                 double *az    /* (output) [deg] calculated azimuth */ 
                ) {

  double JD,JD0;
  double D,D0,H,T,GMST;
  double omega,L,delta_psi,epsilon,eqeq,GAST;
  double HA;

  /* Get JD from mjd/mpm */
  JD0 = ((double)mjd) + 2400000.5;     /* ref: http://tycho.usno.navy.mil/mjd.html */
  H   = ((double)mpm)/(3600.0*1000.0); /* mpm in hours */
  JD = JD0 + H/24.0; 

  /********************/
  /* Get LAST from JD */
  /********************/
  /* ref: http://www.usno.navy.mil/USNO/astronomical-applications/astronomical-information-center/approx-sider-time */

  /* (1) Get GMST */
  D  = JD  - 2451545.0;         /* days from 2000 January 1, 12h UT (Julian date 2451545.0) */
  D0 = JD0 - 2451545.0;
  T = D/36525.0;                /* number of centuries since the year 2000 */
  GMST = 6.697374558 + 0.06570982441908*D0 + 1.00273790935*H + 0.000026*T*T; /* in hours */
  GMST -= 24.0*floor(GMST/24.0); /* Renormalize GMST to [0..24] */
  
  /* (2) Get GAST from GMST */
  omega = 125.04 - 0.052954*D;  /* [deg] the Longitude of the ascending node of the Moon */
  L = 280.47 + 0.98565*D;       /* [deg] the Mean Longitude of the Sun */
  delta_psi = -0.000319*sin(omega*DTR) - 0.000024*sin(2*L*DTR); /* [h] nutation in longitude */
  epsilon = 23.4393 - 0.0000004*D; /* [deg] obliquity */
  eqeq = delta_psi * cos(epsilon*DTR);
  GAST = GMST + eqeq;
  GAST -= 24.0*floor(GAST/24.0); /* Renormalize GAST to [0..24] */

  /* (3) Get LAST from GAST */
  *LAST = GAST + (lon/15.0);
  *LAST -= 24.0*floor(*LAST/24.0); /* Renormalize GAST to [0..24] */
  
  /* Hour angle */
  /* http://en.wikipedia.org/wiki/Hour_angle */
  HA = (*LAST-ra)*2.0*PI/24.0; /* [rad] */ 
  while (HA<0.0) { HA=HA+2.0*PI; }

  /* Convert to Alt/Az */
  /* http://en.wikipedia.org/wiki/Celestial_coordinate_system */
  *alt = asin(   sin(lat*DTR)*sin(dec*DTR) + cos(lat*DTR)*cos(dec*DTR)*cos(HA)                  )/DTR;
  *az  = acos( ( cos(lat*DTR)*sin(dec*DTR) - sin(lat*DTR)*cos(dec*DTR)*cos(HA)) / cos(*alt*DTR) )/DTR;
  if (HA<PI) { *az = 360.0 - *az; }

  /* make sure az is in range 0--> 360 */
  while ((*az)>=360.0) { (*az)-=360.0; } 
  while ((*az)<   0.0) { (*az)+=360.0; } 

  //AZ/ALT from RA/DEC
  //-- sin ALT = sin LAT sin DEC + cos LAT cos DEC cos H 
  //-- cos AZ = ( cos LAT sin DEC - sin LAT cos DEC cos H ) / cos ALT
  //  if H < pi then AZ = 360 - AZ

  //printf("JD=%lf [days]\n",JD); 
  //printf("GMST=%lf [h]\n",GMST);
  //printf("GAST=%lf [h]\n",GAST);

  return;
  } /* me_getaltaz */

// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2012 Oct 07
// -- Making sure azimuth is in range [0,360]
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2012 Jan 21
//   -- corrections to algorithm 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2011 May 05
