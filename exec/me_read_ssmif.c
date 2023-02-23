// me_read_ssmif.c: S.W. Ellingson, Virginia Tech, 2011 Mar 03
// ---
// COMPILE: gcc -o me_read_ssmif.c me_read_ssmif.c -I../common
// ---
// COMMAND LINE: me_read_ssmif <ssmif>
//   <ssmif>  path/name for SSMIF
// ---
// REQUIRES: 
//   me.h
// ---
// See end of this file for history.

#include "me.h"

#define ME_FILENAME_MAX_LENGTH 256

int main ( int narg, char *argv[] ) {

  char filename_ssmif[ME_FILENAME_MAX_LENGTH];

  FILE* fp;

  /* Parse command line */

  if (narg<2) {
    printf("[%d/%d] FATAL: filename_ssmif not specified\n",ME_ME_C,getpid());
    return;
    }
  sprintf(filename_ssmif,"%s",argv[1]);
  printf("[%d/%d] Input: filename_ssmif='%s'\n",ME_ME_C,getpid(),filename_ssmif);
  

  printf("[%d/%d] *********************************************************\n",ME_ME_C,getpid()); 
  printf("[%d/%d] *** Reading SSMIF ***************************************\n",ME_ME_C,getpid());
  printf("[%d/%d] *********************************************************\n",ME_ME_C,getpid());

  /* Open SSMIF */
  if (!(fp = fopen(filename_ssmif,"r"))) {
    printf("[%d/%d] FATAL: unable to fopen SSMIF '%s'\n",ME_ME_C,getpid(),filename_ssmif);
    exit(EXIT_FAILURE);
    }
 
  //strcpy(data,"");
  //#include "me_read_ssmif.c" /* code to read/parse keywords pertaining to PI and project */

  fclose(fp);

  exit(EXIT_SUCCESS);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// me_read_ssmif.c: S.W. Ellingson, Virginia Tech, 2011 Mar 03
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
