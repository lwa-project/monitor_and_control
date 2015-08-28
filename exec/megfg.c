// megfg.c: J. Dowell, UNM, 2015 Aug 28
// ---
// Gains file generator: Converts .gft files to .gf, 
// suitable for DP "sint16 BEAM_GAIN[260][2][2]" argument
// See end of this file for history.
// ---
// COMMAND LINE:
// $ ./megfg <gft_file> <gf_file>
//  <gft_file>: name of a ".gft" file.  The format of this file is expected to be text, 
//    with 1 line per stand, with each line consisting of 4 numbers: xx, xy, yx, yy; 
//    each being a float between 0 and 1.
//  <gf_file>: name of a ".gf" file.  The format of this file is "sint16 BEAM_GAIN[260][2][2]", 
//    with big-endian elements, where for each element the representation fixed point "16.1".  
// ---
// REQUIRES: 
// ---


#include <stdio.h>
#include <string.h>

#define ME "megfg"

#ifdef USE_ADP
#define MAX_STD 256           /* max number of stands */  
#else
#define MAX_STD 260           /* max number of stands */  
#endif

int main( int narg, char *argv[] ) {

  char gft_file[256];
  char gf_file[256];

  float gi[MAX_STD][2][2];
  signed short int g[MAX_STD][2][2];

  int i,j,k;

  FILE *fp;

  union {
    signed short int i;
    char b[2];
    } i2s;

  char bb;

  /* Parse command line */
  if (narg<3) {
    printf("%s: FATAL: Insufficient arguments.  Usage is 'megfg <gft_file> <gf_file>'\n",ME);
    return;
    }
  sscanf(argv[1],"%s",gft_file); 
  sscanf(argv[2],"%s",gf_file); 

  /* show it */
  //printf("%s: gft_file='%s'\n",ME,gft_file);
  //printf("%s: gf_file ='%s'\n",ME,gf_file);

  /* zero out input matrix */
  for (i=0; i<MAX_STD; i++) {
    g[i][0][0] = 0;
    g[i][0][1] = 0;
    g[i][1][0] = 0;
    g[i][1][1] = 0;
    }

  /* read dft file */
  fp = fopen(gft_file,"r");
  if (!fp) {
    printf("%s: FATAL: Unable to open '%s' for input.\n",ME,gft_file);
    return;
    }
  for (i=0; i<MAX_STD; i++) {
    fscanf(fp,"%f %f %f %f",&gi[i][0][0],&gi[i][0][1],&gi[i][1][0],&gi[i][1][1]);
    //printf("%f %f %f %f\n",gi[i][0][0],gi[i][0][1],gi[i][1][0],gi[i][1][1]);
    }
  fclose(fp);  

  /* convert to fixed point */
  for (i=0; i<MAX_STD; i++) {
    for (j=0; j<2; j++) {
      for (k=0; k<2; k++) {
        g[i][j][k] = (unsigned short int) 32767*gi[i][j][k]; 
        }
      }
    //printf("%hd %hd %hd %hd\n",g[i][0][0],g[i][0][1],g[i][1][0],g[i][1][1]);
    }

  /* convert to big-endian */
  for (i=0; i<MAX_STD; i++) {
    for (j=0; j<2; j++) {
      for (k=0; k<2; k++) {
        i2s.i = g[i][j][k]; 
        bb=i2s.b[0]; i2s.b[0]=i2s.b[1]; i2s.b[1]=bb;
        g[i][j][k] = i2s.i;  
        }
      }
    //printf("%hd %hd %hd %hd\n",g[i][0][0],g[i][0][1],g[i][1][0],g[i][1][1]);
    }

  /* save it */
  fp = fopen(gf_file,"wb");
  if (!fp) {
    printf("%s: FATAL: Unable to open '%s' for output.\n",ME,gf_file);
    return;
   }  
  fwrite( g, sizeof(g[0][0][0]), sizeof(g)/sizeof(g[0][0][0]), fp );
  fclose(fp);

  return;
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// megfg.c: J. Dowell, UNM, 2015 Aug 28
//   .1: Added support for ADP
// megfg.c: S.W. Ellingson, Virginia Tech, 2014 Mar 11
//   .1: Removed some annoying printf's
// megfg.c: S.W. Ellingson, Virginia Tech, 2010 Nov 11
//   .1: Initial version


//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================


