// medfg.c: S.W. Ellingson, Virginia Tech, 2010 Nov 11
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


#include <stdio.h>
#include <string.h>

#define ME "medfg"

#define MAX_ANT 520           /* number of antennas ("channels") */  
#define MAX_COARSE_DELAY 4095 /* maximum coarse delay, in sample periods */

int main( int narg, char *argv[] ) {

  char dft_file[256];
  char df_file[256];

  unsigned short int d[MAX_ANT];
  unsigned short int f[MAX_ANT];

  int i;

  FILE *fp;

  union {
    unsigned short int i;
    char b[2];
    } i2u;

  char bb;

  /* Parse command line */
  if (narg<3) {
    printf("%s: FATAL: Insufficient arguments.  Usage is 'medfg <dft_file> <df_file>'\n",ME);
    return;
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
    return;
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

      i2u.i = d[i]; 
      bb=i2u.b[0]; i2u.b[0]=i2u.b[1]; i2u.b[1]=bb;
      d[i] = i2u.i;  
      //printf("%hu\n",d[i]);

      }

  /* save it */
  fp = fopen(df_file,"wb");
  if (!fp) {
    printf("%s: FATAL: Unable to open '%s' for output.\n",ME,df_file);
    return;
   }  
  fwrite( d, sizeof(d[0]), sizeof(d)/sizeof(d[0]), fp );
  fclose(fp);

  return;
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// mecfg.c: S.W. Ellingson, Virginia Tech, 2010 Nov 11
//   .1: Initial version


//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================


