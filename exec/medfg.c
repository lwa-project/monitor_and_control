// medfg.c: J. Dowell, UNM, 2015 Aug 28
// ---
// Delay file generator: Converts .dft files to .df, 
// suitable for DP "uint16 BEAM_DELAY[520]" arguments
// See end of this file for history.
// ---
// COMMAND LINE:
// $ ./medfg <dft_file> <df_file>
//  <dft_file>: name of a ".dft" file.  The format of this file is expected to be text, 
//    with 1 line per delay, with each line consisting of 2 numbers: 
//    the coarse (integer sample) delay (0..MAX_COARSE_DELAY), and fine (subsample delay filter index) delay 0..16.
//  <df_file>: name of a ".df" file.  The format of this file is "uint16 BEAM_DELAY[520]", 
//    with big-endian elements, where for each element the high 12 bits are the coarse delay and the low
//    4 bits are the fine delay (index to a filter).  
// ---
// REQUIRES: 
// ---


#include "mcs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ME "medfg"

#if defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP
#  define MAX_ANT 512           /* number of antennas ("channels") */  
#  define MAX_COARSE_DELAY 1023 /* maximum coarse delay, in sample periods */
#elif defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP
#  define MAX_ANT 512           /* number of antennas ("channels") */  
#  define MAX_COARSE_DELAY 1023 /* maximum coarse delay, in sample periods */
#else
#  define MAX_ANT 520           /* number of antennas ("channels") */  
#  define MAX_COARSE_DELAY 4095 /* maximum coarse delay, in sample periods */
#endif

int main( int narg, char *argv[] ) {

  char dft_file[256];
  char df_file[256];

  unsigned short int d[MAX_ANT];
  unsigned short int f[MAX_ANT];

  int i;

  FILE *fp;

  /* Parse command line */
  if (narg<3) {
    printf("%s: FATAL: Insufficient arguments.  Usage is 'medfg <dft_file> <df_file>'\n",ME);
    exit(EXIT_FAILURE);
    }
  sscanf(argv[1],"%s",dft_file); 
  sscanf(argv[2],"%s",df_file); 

  /* show it */
  printf("%s: dft_file='%s'\n",ME,dft_file);
  printf("%s: df_file ='%s'\n",ME,df_file);

  /* zero out input matrix */
  for (i=0; i<MAX_ANT; i++) {
    d[i] = 0;
    }

  /* read dft file */
  fp = fopen(dft_file,"r");
  if (!fp) {
    printf("%s: FATAL: Unable to open '%s' for input.\n",ME,dft_file);
    exit(EXIT_FAILURE);
    }
  for (i=0; i<MAX_ANT; i++) {
    fscanf(fp,"%hu %hu",&d[i],&f[i]);
    //printf("%hu %hu\n",d[i],f[i]);
    }
  fclose(fp);  

  /* merge coarse and fine delays into single parameter; and convert to big-endian */
  for (i=0; i<MAX_ANT; i++) {

      d[i] = (d[i]<<4) + f[i];
      //printf("%hu\n",d[i]);

      d[i] = LWA_i2u_swap(d[i]); 
      //printf("%hu\n",d[i]);

      }

  /* save it */
  fp = fopen(df_file,"wb");
  if (!fp) {
    printf("%s: FATAL: Unable to open '%s' for output.\n",ME,df_file);
    exit(EXIT_FAILURE);
   }  
  fwrite( d, sizeof(d[0]), sizeof(d)/sizeof(d[0]), fp );
  fclose(fp);

  exit(EXIT_SUCCESS);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// medfg.c: J. Dowell, UNM, 2022 May 3
//   .1: Updated for NDP
// medfg.c: J. Dowell, UNM, 2015 Aug 28
//   .1: Updated for ADP
// medfg.c: S.W. Ellingson, Virginia Tech, 2010 Nov 11
//   .1: Initial version


//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
