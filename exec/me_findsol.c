// me_findsol.c: S.W. Ellingson, Virginia Tech, 2012 Oct 04
// ---
// COMPILE: intended to be called from another program which it is compiled into
// ---
// COMMAND LINE: N/A
// ---
// REQUIRES: 
//   
// Find RA and DEC of the sun
// See end of this file for history.
 
#define MEFS_DTR 0.017453292520
#define MEFS_PI  3.141592653590

void me_findsol(
                 long int mjd, /* (input) mean julian date */
                 long int mpm, /* (input) milliseconds past UTC midnight */ 
                 float *ra,   /* (output) [h] RA */
                 float *dec   /* (output) [deg] dec */
                ) {

  double JD, H, JD0;
  double T, L0, Lp, M, e, C, cd, v, R, Omega, lambda, epsilon0, delta_epsilon, epsilon; 

  /* Get JD from mjd/mpm */
  JD0 = ((double)mjd) + 2400000.5;     /* ref: http://tycho.usno.navy.mil/mjd.html */
  H   = ((double)mpm)/(3600.0*1000.0); /* mpm in hours */
  JD = JD0 + H/24.0; /* days */

  T = (JD-2451545.0)/36525.0;
  //printf("T = %lf\n",T);

  L0 = 280.46646 + 36000.76983*T + 0.0003032*T*T;
  //printf("L0 = %lf [deg]\n",L0);

  M  = 357.52911 + 35999.05029*T + 0.0001537*T*T; 
  //printf("M = %lf [deg]\n",M);

  e  = 0.016708634 - 0.000042037*T + 0.0000001267*T*T;
  //printf("e = %lf\n",e);  

  C = ( 1.914602 - 0.004817*T - 0.000014*T*T ) * sin(M*MEFS_DTR)
     +( 0.019993 - 0.000101*T                ) * sin(2.0*M*MEFS_DTR)
     +  0.000289                               * sin(3.0*M*MEFS_DTR); 
  //printf("C = %lf [deg]\n",C);    

  cd = L0 + C;
  //printf("cd = %lf [deg]\n",cd); 

  v  = M + C; /* deg */

  R = 1.000001018*(1.0-e*e)/(1.0+e*cos(v*MEFS_DTR));
  //printf("R = %lf\n",R); 

  Omega = 125.04 - 1934.136*T; 
  //printf("Omega = %lf [deg]\n",Omega); 

  lambda = cd - 0.00569 - 0.00478*sin(Omega*MEFS_DTR); 
  //printf("lambda = %lf [deg]\n",lambda); 

  epsilon0 = ( 23.0+26.0/60.0+21.448   /3600.0) 
            -(                46.8150  /3600.0)*T
            -(                 0.00059 /3600.0)*T*T
            +(                 0.001813/3600.0)*T*T*T;
  //printf("epsilon0 = %lf [deg]\n",epsilon0);

  Lp = 218.3165 + 481267.8813*T; /* deg */
  delta_epsilon = (9.20/3600.0)*cos(Omega*MEFS_DTR)
                 +(0.57/3600.0)*cos(2.0*L0*MEFS_DTR)
                 +(0.10/3600.0)*cos(2.0*Lp*MEFS_DTR)
                 -(0.09/3600.0)*cos(2.0*Omega*MEFS_DTR); /* deg */
  epsilon = epsilon0 + delta_epsilon;
  //printf("epsilon = %lf [deg]\n",epsilon);

  *ra = atan2( cos(epsilon*MEFS_DTR) * sin(cd*MEFS_DTR), cos(cd*MEFS_DTR) ) * 24.0/(2.0*MEFS_PI) ; /* [h] */
  while ((*ra)>=24.0) { (*ra)-=24.0; }
  while ((*ra)<  0.0) { (*ra)+=24.0; }

  *dec = asin( sin(epsilon*MEFS_DTR) * sin(cd*MEFS_DTR) ) * 180.0/MEFS_PI; /* [deg] */  

  return;
  } /* me_findsol() */

// me_findsol.c: S.W. Ellingson, Virginia Tech, 2012 Oct 04
//   -- initial version, using me_getaltaz.c as a starting point 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2012 Jan 21
//   -- corrections to algorithm 
// me_getaltaz.c: S.W. Ellingson, Virginia Tech, 2011 May 05
