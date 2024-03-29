// tprcs.c: S.W. Ellingson, Virginia Tech, 2011 December 16
// ---
// COMPILE: gcc -o tprcs tprcs.c -I../common
// ---
// COMMAND LINE: ./tprcs <csfile>
//   <csfile> is the name of MCS/Exec command script (".cs") file 
// ---
// REQUIRES: 
//   mcs.h via mt.h
// ---
// Reads MCS/Exec command script (".cs") file, writes it to stdout in human-readable format
// See end of this file for history.

#include <stdlib.h>

#include "mt.h"

/*==============================================================*/
/*=== main() ===================================================*/
/*==============================================================*/

int main ( int narg, char *argv[] ) {

  char csfile[256];

  FILE *fp;
  struct me_action_struct action;
  char data[16384];
  long int mjd;
  long int mpm,mpm0;
  
  /* parse command line */
  if (narg<2) {
    printf("syntax: $ ./tprcs <csfile>\n");
    exit(EXIT_FAILURE);
    }
  sprintf(csfile,"%s",argv[1]);
  printf("input: <csfile>='%s'\n",csfile);

  /* Open .cs file */
  if ((fp=fopen(csfile,"rb"))==NULL) {
    printf("[%d/%d] FATAL: Can't open '%s'\n",MT_TPRCS,getpid(),csfile);
    exit(EXIT_FAILURE);
    }

  //printf("MJD   MPM      A SID CID LEN  Data\n");  
  //printf("----- -------- - --- --- ---- ----------------------------------\n");  
  //printf("MJD   MPM      A SID CID Data\n");  
  //printf("----- -------- - --- --- ----------------------------------\n");  
  printf("MJD   MPM      del [s]   A SID CID Data\n");  
  printf("----- -------- --------- - --- --- ----------------------------------\n");  

  mpm0=-1;  

  /* main loop */
  while( fread( &action, sizeof(struct me_action_struct), 1, fp ) > 0 ) {
 
    /* read the rest of the action */
    strcpy(data,"");
    if (action.len>0) fread( data, action.len, 1, fp );

    LWA_timeval( &(action.tv), &mjd, &mpm ); /* convert timeval to (mjd,mpm) */

    if (mpm0<0) mpm0 = mpm;

    //printf("%5ld %8ld %1d %3s %3s '%s'\n",
    //        mjd, mpm, action.bASAP,
    //                      LWA_sid2str(action.sid),
    //                          LWA_cmd2str(action.cid),
    //                                 data );  

    printf("%5ld %8ld %9.3f %1d %3s %3s '%s'\n",
            mjd, mpm, ((float)(mpm-mpm0))/1000.0, 
                            action.bASAP,
                                LWA_sid2str(action.sid),
                                    LWA_cmd2str(action.cid),
                                       data ); 

    } /* while ( fread( */

  /* Close .cs file */
  fclose(fp);

  exit(EXIT_SUCCESS);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// tprcs.c: J. Dowell, UNM, 2018 Jan 29
//   .1 Cleaned up a few compiler warnings
// tprcs.c: S.W. Ellingson, Virginia Tech, 2011 December 16
//   .1: Initial version, using tpsdm as a starting point
// tpsdm.c: S.W. Ellingson, Virginia Tech, 2011 April 05
//   .1: Added new 'settings' parameters to 'sta' report; added 'ana' report
// tpsdm.c: S.W. Ellingson, Virginia Tech, 2011 March 26
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
