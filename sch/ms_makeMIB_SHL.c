// ms_makeMIB_SHL.c: J. Dowell, UNM, 2015 Sep 9
// ---
// COMPILE: gcc -o ms_makeMIB_SHL ms_makeMIB_SHL.c
// ---
// COMMAND LINE: ms_makeMIB_SHL <RACK-COUNT> <RACK-PORT-COUNT>
//
// ---
// REQUIRES: 
// ---
// Creates ASP_MIB_init.dat.  
// See end of this file for history.

#include <stdio.h>
#include <unistd.h>

//#include "LWA_MCS.h"

#define MY_NAME "ms_makeMIB_SHL (v.20150909.1)"
#define ME "10" 

int main ( int narg, char *argv[] ) {

  /*=================*/
  /*=== Variables ===*/
  /*=================*/

  int n_RACK = 0;
  int n_RACK_PORT = 0;
  
  FILE *fp;
  int i,j;


  /*==================*/
  /*=== Initialize ===*/
  /*==================*/
    
  /* First, announce thyself */
  printf("I am %s [%s]\n",MY_NAME,ME);

  if (narg>1) { 
    sscanf(argv[1],"%d",&n_RACK);
    } else {
    printf("[%s] FATAL: RACK-COUNT not provided\n",ME);
    return 1;
    } 
  if (narg>2) { 
    sscanf(argv[2],"%d",&n_RACK_PORT);
    } else {
    printf("[%s] FATAL: RACK-PORT-COUNT not provided\n",ME);
    return 1;
    } 
    
    
  /* create file */
  fp = fopen("SHL_MIB_init.dat","w");

  /* lay in MCS-RESERVED */
  fprintf(fp,"B 1 		MCS-RESERVED 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 1.1 		SUMMARY			UNK	a7	a7\n");
  fprintf(fp,"V 1.2 		INFO      		UNK	a256	a256\n");
  fprintf(fp,"V 1.3 		LASTLOG   		UNK	a256	a256\n");
  fprintf(fp,"V 1.4 		SUBSYSTEM 		UNK	a3	a3\n");
  fprintf(fp,"V 1.5 		SERIALNO  		UNK	a5	a5\n");
  fprintf(fp,"V 1.6 		VERSION   		UNK	a256	a256\n");

  /* SHL-POWER */
  fprintf(fp,"B 2 		SHL-POWER 		NUL 	NUL 	NUL\n");
  for (i=0; i<n_RACK; i++ ) {
    fprintf(fp,"B 2.%d 		PWR-RACK-%d 		NUL 	NUL 	NUL\n",i+1,i+1);
    fprintf(fp,"V 2.%d.1		PORTS-AVALIABLE-R%d		0	a2	a2\n",i+1,i+1);
    fprintf(fp,"B 2.%d.2		PORT-STATUS-R%d		NUL	NUL	NUL\n",i+1,i+1);
    for (j=0; j<n_RACK_PORT; j++ ) {
      fprintf(fp,"V 2.%d.2.%d		PWR-R%d-%d		OFF	a3	a3\n",i+1,j+1,i+1,j+1);
      }
    fprintf(fp,"V 2.%d.3 	CURRENT-R%d	 	UNK	a10	a10\n",i+1,i+1);
    fprintf(fp,"V 2.%d.4 	VOLTAGE-R%d		UNK	a10	a10\n",i+1,i+1);
    fprintf(fp,"V 2.%d.5 	FREQUENCY-R%d		UNK	a10	a10\n",i+1,i+1);
    fprintf(fp,"V 2.%d.6 	BATCHARGE-R%d		UNK	a10	a10\n",i+1,i+1);
    fprintf(fp,"V 2.%d.7 	BATSTATUS-R%d		UNK	a10	a10\n",i+1,i+1);
    fprintf(fp,"V 2.%d.8 	OUTSOURCE-R%d		UNK	a10	a10\n",i+1,i+1);
    }

  /* SHL-ECS */
  fprintf(fp,"B 3 		SHL-ECS 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 3.1		SET-POINT		UNK	a10	a10\n");
  fprintf(fp,"V 3.2		DIFFERENTIAL	UNK	a10	a10\n");
  fprintf(fp,"V 3.3		TEMPERATURE	UNK	a10	a10\n");
  
  /* SHL-WX */
  fprintf(fp,"B 4 		SHL-WX 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 4.1		WX-UPDATE		UNK	a20	a20\n");
  fprintf(fp,"V 4.2		WX-TEMPERATURE	UNK	a10	a10\n");
  fprintf(fp,"V 4.3		WX-HUMIDITY	UNK	a10	a10\n");
  fprintf(fp,"V 4.4		WX-PRESSURE	UNK	a10	a10\n");
  fprintf(fp,"V 4.5		WX-WIND		UNK	a25	a25\n");
  fprintf(fp,"V 4.6		WX-GUST		UNK	a25	a25\n");
  fprintf(fp,"B 4.7		WX-RAINFALL	NUL	NUL	NUL\n");
  fprintf(fp,"B 4.7.1		WX-RAINFALL-RATE	UNK	a10	a10\n");
  fprintf(fp,"B 4.7.2		WX-RAINFALL-TOTAL	UNK	a10	a10\n");
  
  /* SHL-LIGHTNING */
  fprintf(fp,"B 5 		SHL-LIGHTNING 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 5.1		LIGHTNING-RADIUS		UNK	a10	a10\n");
  fprintf(fp,"V 5.2		LIGHTNING-10MIN	UNK	a10	a10\n");
  fprintf(fp,"V 5.3		LIGHTNING-30MIN	UNK	a10	a10\n");
  
  close(fp);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// ms_makeMIB_SHL.c: J. Dowell, UNM, 2015 Sep 9
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================

