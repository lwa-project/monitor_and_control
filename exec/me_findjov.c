// me_findjov.c: S.W. Ellingson, Virginia Tech, 2012 Oct 07
// ---
// COMPILE: intended to be called from another program which it is compiled into
// ---
// COMMAND LINE: N/A
// ---
// REQUIRES: 
//   
// Find RA and DEC of Jupiter
// See end of this file for history.

/* The codes below are ported from Xephem */
#include "ephem_astro.h"  /* mostly gutted, leaving only stuff needed for others */
#include "ephem_vsop87.h"
#include "ephem_vsop87_data.c" 
#include "ephem_vsop87.c"

#define MEFJ_DTR 0.017453292520
#define MEFJ_PI  3.141592653590

void me_findjov(
                 long int mjd, /* (input) modified julian date */
                 long int mpm, /* (input) milliseconds past UTC midnight */ 
                 float *ra,   /* (output) [h] RA */
                 float *dec  /* (output) [deg] dec */
                ) {

  double JD, H, JD0;
  double ret[6]; /* this is how vsop87() returns output */
  double L,  B,  R;
  double L0, B0, R0;
  double x,y,z;
  double lambda, beta;
  double T, epsilon0, epsilon; 

  /* Get JD from mjd/mpm */
  JD0 = ((double)mjd) + 2400000.5;     /* ref: http://tycho.usno.navy.mil/mjd.html */
  H   = ((double)mpm)/(3600.0*1000.0); /* mpm in hours */
  JD = JD0 + H/24.0; /* days */

  /* Get L, B, R for Jupiter */
  vsop87 ( JD-2415020.0, JUPITER, 0, ret);
  L = ret[0]; /* [rad] */
  B = ret[1]; /* [rad] */
  R = ret[2]; /* [AU] */
  //printf("me_findjov(): L =%lf rad, B =%lf rad, R =%lf AU\n",L,B,R);

  /* Get L0, B0, R0 for Earth */
  vsop87 ( JD-2415020.0,       8, 0, ret);
  L0 = ret[0]; /* [rad] */
  B0 = ret[1]; /* [rad] */
  R0 = ret[2]; /* [AU] */
  //printf("me_findjov(): L0=%lf rad, B0=%lf rad, R0=%lf AU\n",L0,B0,R0);

  /* following "First Method" scheme in Meeus, Ch 33 */

  x = R*cos(B)*cos(L) - R0*cos(B0)*cos(L0);
  y = R*cos(B)*sin(L) - R0*cos(B0)*sin(L0);
  z = R*sin(B)        - R0*sin(B0);

  lambda = atan2( y, x             );
  beta   = atan2( z, sqrt(x*x+y*y) );

  /* getting epsilon */
  T = (JD-2451545.0)/36525.0;
  epsilon0 = ( 23.0+26.0/60.0+21.448   /3600.0) 
            -(                46.8150  /3600.0)*T
            -(                 0.00059 /3600.0)*T*T
            +(                 0.001813/3600.0)*T*T*T;
  epsilon = ( epsilon0 + 0.0 )*MEFJ_DTR; /* neglecting delta-epsilon */

  /* neglecting light travel time, other corrections */

  *ra  = atan2( sin(lambda)*cos(epsilon) - tan(beta)*sin(epsilon) , cos(lambda) ) *24.0/(2.0*MEFJ_PI);
  while ((*ra)>=24.0) { (*ra)-=24.0; }
  while ((*ra)<  0.0) { (*ra)+=24.0; }

  *dec = asin( sin(beta)*cos(epsilon) + cos(beta)*sin(epsilon)*sin(lambda) ) / MEFJ_DTR;  

  return;
  } /* me_getaltaz */

// me_findjov.c: S.W. Ellingson, Virginia Tech, 2012 Oct 07
//   -- initial version, using me_getaltaz.c as a starting point 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2012 Jan 21
//   -- corrections to algorithm 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2011 May 05
