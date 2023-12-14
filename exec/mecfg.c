// mecfg.c: S.W. Ellingson, Virginia Tech, 2010 Nov 11
// ---
// COMMAND LINE:
// $ ./mecfg <cft_file> <cf_file>
//  <cft_file>: name of a ".cft" file.  The format of this file is expected to be text, 
//    with 1 line per delay-filter (16 lines total), with 32 coefficients per line, 
//    each coefficient being a signed integer representable in 16 bits.
//  <cf_file>: name of a ".cf" file.  The format of this file is "sint16 COEFF_DATA[16][32]",
//    with big-endian elements  
// ---
// REQUIRES: 
// ---
// Coefficient file generator: Converts .cft files to .cf, 
// suitable for DP "sint16 COEFF_DATA[16][32]" arguments
// See end of this file for history.

#include "mcs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ME "mecfg"
#define MAX_DELAYS 16
#define MAX_COEFFS 32

int main( int narg, char *argv[] ) {

  char cft_file[256];
  char cf_file[256];

  signed short int c[MAX_DELAYS][MAX_COEFFS];

  int i,j;
  //char line[1024];

  FILE *fp;

  /* Parse command line */
  if (narg<3) {
    printf("%s: FATAL: Insufficient arguments.  Usage is 'mecfg <cft_file> <cf_file>'\n",ME);
    exit(EXIT_FAILURE);
    }
  sscanf(argv[1],"%s",cft_file); 
  sscanf(argv[2],"%s",cf_file); 

  /* show it */
  printf("%s: cft_file='%s'\n",ME,cft_file);
  printf("%s: cf_file ='%s'\n",ME,cf_file);

  /* zero out input matrix */
  for (i=0; i<MAX_DELAYS; i++) {
    for (j=0; j<MAX_COEFFS; j++) {
      c[i][j] = 0;
      }
    }

  /* read cft file */
  fp = fopen(cft_file,"r");
  if (!fp) {
    printf("%s: FATAL: Unable to open '%s' for input.\n",ME,cft_file);
    exit(EXIT_FAILURE);
    }
  for (i=0; i<MAX_DELAYS; i++) {
    for (j=0; j<MAX_COEFFS; j++) {
      fscanf(fp,"%hd",&c[i][j]);
      //printf("%hd ",c[i][j]);
      }
    //printf("\n");
    }
  fclose(fp);  

  /* convert to big-endian */
  for (i=0; i<MAX_DELAYS; i++) {
    for (j=0; j<MAX_COEFFS; j++) {
      c[i][j] = LWA_i2s_swap(c[i][j]);
      printf("%hd ",c[i][j]);
      }
    printf("\n");
    }

  /* save it */
  fp = fopen(cf_file,"wb");
  if (!fp) {
    printf("%s: FATAL: Unable to open '%s' for output.\n",ME,cf_file);
    exit(EXIT_FAILURE);
    }  
  fwrite( c, sizeof(c[0][0]), sizeof(c)/sizeof(c[0][0]), fp );
  fclose(fp);

  exit(EXIT_SUCCESS);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// mecfg.c: S.W. Ellingson, Virginia Tech, 2010 Nov 11
//   .1: Initial version


//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
