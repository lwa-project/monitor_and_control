// me_point_corr.c: S.W. Ellingson, Virginia Tech, 2012 Jul 03
// ---
// COMPILE: intended to be called from another program which it is compiled into
// ---
// COMMAND LINE: N/A
// ---
// REQUIRES: 
//   
// See end of this file for history.
 
#define MPC_DTR  0.017453292520
#define MPC_TINY (1e-3);  

void me_point_corr( 
                   float fPCAxisTh, /* (input) [deg] axis pointing direction, theta coordinate */
                   float fPCAxisPh, /* (input) [deg] axis pointing direction, phi coordinate */
                   float fPCRot,    /* (input) [deg] rotation around axis (right hand rule) */
                   double *alt,     /* (input/output) [deg] altitude */ 
                   double *az       /* (input/output) [deg] azimuth */ 
                  ) {

  double cx,cy,cz;
  double ux,uy,uz;
  double ct,st;
  double bx,by,bz;

  /* calculate unit vector "c" corresponding to pointing direction */
  cx = cos((90.0-(*az))*MPC_DTR)*sin((90.0-(*alt))*MPC_DTR);
  cy = sin((90.0-(*az))*MPC_DTR)*sin((90.0-(*alt))*MPC_DTR);
  cz =                           cos((90.0-(*alt))*MPC_DTR);

  /* calculate unit vector "u" corresponding to axis pointing direction */
  ux = cos(fPCAxisPh*MPC_DTR)*sin(fPCAxisTh*MPC_DTR);
  uy = sin(fPCAxisPh*MPC_DTR)*sin(fPCAxisTh*MPC_DTR);
  uz =                        cos(fPCAxisTh*MPC_DTR);

  /* precalculating to simplify expressions later */
  ct = cos(fPCRot*MPC_DTR);
  st = sin(fPCRot*MPC_DTR);

  /* rotate "c" around axis "u", yielding unit vector "b" */
  /* [1] http://en.wikipedia.org/wiki/Rotation_matrix (see "Rotation matrix from axis to angle") */
  /* [2] http://inside.mines.edu/~gmurray/ArbitraryAxisRotation/ (see Section 5.2) */
  bx = (ux*ux+(1+ux*ux)*ct)*cx + (ux*uy*(1-ct)-uz*st)*cy + (ux*uz*(1-ct)+uy*st)*cz;
  by = (ux*uy*(1-ct)+uz*st)*cx + (uy*uy+(1-uy*uy)*ct)*cy + (ux*uz*(1-ct)-ux*st)*cz;
  bz = (ux*uz*(1-ct)-uy*st)*cx + (uy*uz*(1-ct)+ux*st)*cy + (uz*uz+(1-uz*uz)*ct)*cz;

  /* check b */
  printf("me_point_corr(): (bx,by,bz) = (%lf,%lf,%lf)\n",bx,by,bz);
  printf("me_point_corr(): (Magnitude of b)^2 is %lf\n",bx*bx+by*by+bz*bz);

  /* convert b back to alt, az */
  *az  = 90.0 - (atan2(by,bx)/MPC_DTR);
  *alt = 90.0 - (acos(bz)    /MPC_DTR); 

  return;
  } /* me_point_corr */

// me_point_corr.c: S.W. Ellingson, Virginia Tech, 2012 Jul 03
// -- initial version
