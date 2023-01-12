// me_point_corr_x.c: S.W. Ellingson, Virginia Tech, 2012 Jul 03
// ---
// COMPILE: gcc -o me_point_corr_x me_point_corr_x.c -I../common -lm
// ---
// COMMAND LINE: see help message in source code, below
// ---
// EXAMPLE:
//   see command line help
// ---
// REQUIRES: 
//   me_point_corr.c
// ---
// Wrapper used to test me_point_corr() 
// See end of this file for history.

#include <stdlib.h>
#include <math.h>

#include "me.h"
#include "me_astro.h"

int main ( int narg, char *argv[] ) {

  float fPCAxisTh;
  float fPCAxisPh;
  float fPCRot;
  double alt;   /* [deg] calculated altitude */
  double az;    /* [deg] calculated azimuth */ 

  /* Parse command line */
  if (narg>1) sscanf(argv[1],"%f",&fPCAxisTh);
  if (narg>2) sscanf(argv[2],"%f",&fPCAxisPh);
  if (narg>3) sscanf(argv[3],"%f",&fPCRot);
  if (narg>4) sscanf(argv[4],"%lf",&alt);
  if (narg>5) sscanf(argv[5],"%lf",&az);    
  if (narg<6) {
    printf("Useage:\n");
    printf("me_point_corr_x <fPCAxisTh> <fPCAxisPh> <fPCAxisRot> <alt> <az>\n");
    exit(EXIT_FAILURE);
    }

  me_point_corr( 
                 fPCAxisTh, /* (input) [deg] axis pointing direction, theta coordinate */
                 fPCAxisPh, /* (input) [deg] axis pointing direction, phi coordinate */
                 fPCRot,    /* (input) [deg] rotation around axis (right hand rule) */
                &alt,       /* (input/output) [deg] altitude */ 
                &az         /* (input/output) [deg] azimuth */ 
                  );

  printf("alt [deg] = %lf\n",alt);
  printf("az  [deg] = %lf\n",az);

  exit(EXIT_SUCCESS);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// me_point_corr_x.c: S.W. Ellingson, Virginia Tech, 2012 Jul 03
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
