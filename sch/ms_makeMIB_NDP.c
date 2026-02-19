// ms_makeMIB_ADP.c: J. Dowell, UNM, 2015 Jul 14
// ---
// COMPILE: gcc -o ms_makeMIB_ADP ms_makeMIB_ADP.c
// ---
// COMMAND LINE: ms_makeMIB_ADP
//
// ---
// REQUIRES: 
// ---
// Creates ADP_MIB_init.dat.  
// See end of this file for history.

#include <stdio.h>
#include "mcs.h"

//#include "LWA_MCS.h"

#define MY_NAME "ms_makeMIB_NDP (v.20220502.1)"
#define ME "11" 

// Catch for when we compile without NDP support
#if !defined(ME_MAX_NSNAP)
#define ME_MAX_NSNAP 2
#endif
#if !defined(ME_MAX_NSERVER)
#define ME_MAX_NSERVER 3
#endif

main ( int narg, char *argv[] ) {

  /*=================*/
  /*=== Variables ===*/
  /*=================*/

  FILE *fp;
  int i,j;


  /*==================*/
  /*=== Initialize ===*/
  /*==================*/
    
  /* First, announce thyself */
  printf("I am %s [%s]\n",MY_NAME,ME);

  //if (narg>1) { 
  //  sscanf(argv[1],"%d",&n_ARXSUPPLY_NO);
  //  } else {
  //  printf("[%s] FATAL: ARXSUPPLY-NO not provided\n",ME);
  //  return;
  //  } 

  /* create file */
  fp = fopen("NDP_MIB_init.dat","w");

  /* lay in ADP-specific entries used by MCS (not part of ADP ICD) */

  fprintf(fp,"B 0.4           MCS_TBT_TRIGGER 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 0.4.1         MCS_TBT_TRIG_TIME           0       a20     NUL\n");
  fprintf(fp,"V 0.4.2         MCS_TBT_SAMPLES             0       a8      NUL\n");
  fprintf(fp,"V 0.4.3         MCS_TBT_TUNING_MASK         0       a20     NUL\n");
  fprintf(fp,"V 0.4.4         MCS_TBT_REFERENCE           0       a9      NUL\n");
  fprintf(fp,"V 0.4.5         MCS_TBT_CMD_SENT_MPM        0       a9      NUL\n");  

  fprintf(fp,"B 0.5           MCS_TBS_CONFIG 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 0.5.1         MCS_TBS_FREQ                0.000\ta12     NUL\n");
  fprintf(fp,"V 0.5.2         MCS_TBS_BW 		          0 	  a1 	NUL\n",i+1,i+1); 
  fprintf(fp,"V 0.5.3         MCS_TBS_REFERENCE           0       a9      NUL\n");
  fprintf(fp,"V 0.5.4         MCS_TBS_CMD_SENT_MPM        0       a9      NUL\n");
  
  fprintf(fp,"B 0.6           MCS_COR_CONFIG		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 0.6.1         MCS_COR_NAVG                0      a10      NUL\n");
  fprintf(fp,"V 0.6.2         MCS_COR_TUNING_MASK         0      i8u      NUL\n");
  fprintf(fp,"V 0.6.3         MCS_COR_GAIN                0       a2      NUL\n");
  fprintf(fp,"V 0.6.4         MCS_COR_SUB_SLOT            0       a2      NUL\n");
  fprintf(fp,"V 0.6.5         MCS_COR_REFERENCE           0       a9      NUL\n");
  fprintf(fp,"V 0.6.6         MCS_COR_CMD_SENT_MPM        0       a9      NUL\n"); 

  fprintf(fp,"B 0.7           MCS_CH_FILTER_SET 		NUL 	NUL 	NUL\n");
  for ( i=1; i<=512; i++ ) {
    fprintf(fp,"V 0.7.%d\tMCS_CF%03d\t\t\tUNK 	a32 	NUL\n",i,i);
    }
  
  fprintf(fp,"B 0.7           MCS_BEAM_CONFIG 		NUL 	NUL 	NUL\n");
  for ( i=0; i<4; i++ ) {
    fprintf(fp,"B 0.7.%d         MCS_BEAM%d 			NUL 	NUL 	NUL\n",i+1,i+1); /* corresponds to DRX_BEAM in DRX commands, BEAM_ID in BAM commands */
    fprintf(fp,"V 0.7.%d.1       MCS_BEAM%d_DFILE 		UNK 	a32 	NUL\n",i+1,i+1); /* name of file of delays (BEAM_DELAY[520] in BAM command */
    fprintf(fp,"V 0.7.%d.2       MCS_BEAM%d_GFILE 		UNK 	a32 	NUL\n",i+1,i+1); /* name of file of gains (sint16 BEAM_GAIN[260][2][2] in BAM command */
    fprintf(fp,"B 0.7.%d.3       MCS_BEAM%d_T1		UNK 	NUL 	NUL\n",i+1,i+1); /* corresponds to DRX_TUNING in DRX commands */
    fprintf(fp,"V 0.7.%d.3.1     MCS_BEAM%d_T1_FREQ 		0.000 	a12 	NUL\n",i+1,i+1); /* corresponds to DRX_FREQ in DRX commands [Hz] */
    fprintf(fp,"V 0.7.%d.3.2     MCS_BEAM%d_T1_BW 		0 	a1 	NUL\n",i+1,i+1); /* 1-7 */
    fprintf(fp,"V 0.7.%d.3.3     MCS_BEAM%d_T1_GAIN 		0 	a2 	NUL\n",i+1,i+1); /* 0-15 */
    fprintf(fp,"B 0.7.%d.4       MCS_BEAM%d_T2		UNK 	NUL 	NUL\n",i+1,i+1); 
    fprintf(fp,"V 0.7.%d.4.1     MCS_BEAM%d_T2_FREQ 		0.000 	a12 	NUL\n",i+1,i+1); 
    fprintf(fp,"V 0.7.%d.4.2     MCS_BEAM%d_T2_BW 		0 	a1 	NUL\n",i+1,i+1); 
    fprintf(fp,"V 0.7.%d.4.3     MCS_BEAM%d_T2_GAIN 		0 	a2 	NUL\n",i+1,i+1); 
    }
  /* lay in MCS-RESERVED */

  fprintf(fp,"B 1 		MCS-RESERVED 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 1.1 		SUMMARY			UNK	a7	a7\n");
  fprintf(fp,"V 1.2 		INFO      		UNK  a256	a256\n");
  fprintf(fp,"V 1.3 		LASTLOG   		UNK	a256	a256\n");
  fprintf(fp,"V 1.4 		SUBSYSTEM 		UNK	a3	a3\n");
  fprintf(fp,"V 1.5 		SERIALNO  		UNK	a5	a5\n");
  fprintf(fp,"V 1.6 		VERSION   		UNK	a256	a256\n");

  /* NDP-specific */

  fprintf(fp,"V 2.1 		TBT_STATUS		0	i1ur	i1ur\n");
  fprintf(fp,"V 2.2			TBT_TUNING_MASK	0	i8ur	i8ur\n");

  fprintf(fp,"V 4.1.1		NUM_DRX_TUNINGS		0	i1ur	i1ur\n");
  fprintf(fp,"V 4.1.2		NUM_FREQ_CHANS			0	i2ur	i2ur\n");
  fprintf(fp,"V 4.2 		NUM_BEAMS		0	i1ur	i1ur\n"); 
  fprintf(fp,"V 4.3 		NUM_STANDS		0	i2ur	i2ur\n");
  fprintf(fp,"V 4.4.1 		NUM_BOARDS		0	i1ur	i1ur\n");
  fprintf(fp,"V 4.4.2 		NUM_SERVERS		0	i1ur	i1ur\n");
  fprintf(fp,"V 4.5 		BEAM_FIR_COEFFS		0	i1ur	i1ur\n");
  fprintf(fp,"B 4.6 		T_NOM			NUL	NUL	NUL\n");
  for ( i=1; i<=2; i++ ) {
    fprintf(fp,"V 4.6.%d\t\tT_NOM%d			0	i2ur	i2ur\n", i, i);
    }
  
  fprintf(fp,"B 5 		FIR			NUL	NUL	NUL\n");
  fprintf(fp,"V 5.1 		FIR1			NUL	r1024	r1024\n");
  fprintf(fp,"V 5.2 		FIR2			NUL	r1024	r1024\n");
  fprintf(fp,"V 5.3 		FIR3			NUL	r1024	r1024\n");
  fprintf(fp,"V 5.4 		FIR4			NUL	r1024	r1024\n");
  fprintf(fp,"V 5.5 		FIR_CHAN_INDEX		0	i2u	i2u\n");

  fprintf(fp,"V 6 		CLK_VAL			0	i4ur	i4ur\n");
 
  for ( i=0; i<2*LWA_MAX_NSTD; i++ ) {
    fprintf(fp,"B 7.%d		ANT%d_STAT 		NUL 	NUL 	NUL\n",i+1,i+1); 
    fprintf(fp,"V 7.%d.1		ANT%d_RMS			0 	f4r 	f4r\n",i+1,i+1);
    fprintf(fp,"V 7.%d.2		ANT%d_DCOFFSET		0 	f4r 	f4r\n",i+1,i+1);
    fprintf(fp,"V 7.%d.3		ANT%d_SAT			0 	i4ur i4ur\n",i+1,i+1);
    fprintf(fp,"V 7.%d.4		ANT%d_PEAK		0    i4ur i4ur\n",i+1,i+1);
    }
  fprintf(fp,"V 7.516 	STAT_SAMP_SIZE 		0 	i4ur 	i4ur\n"); 

  fprintf(fp,"B 8 		BOARD_STAT		NUL	NUL	NUL\n");
  for ( i=0; i<ME_MAX_NSNAP; i++ ) {
    fprintf(fp,"B 8.%d\t\tBOARD%d		NUL	NUL	NUL\n",i+1,i+1);
    fprintf(fp,"V 8.%d.1\t\tBOARD%d_STAT 		0 	i8ur 	i8ur\n",i+1,i+1); 
    fprintf(fp,"V 8.%d.2\t\tBOARD%d_TEMP_MIN 		0 	f4r 	f4r\n",i+1,i+1); 
    fprintf(fp,"V 8.%d.3\t\tBOARD%d_TEMP_MAX 		0 	f4r 	f4r\n",i+1,i+1); 
    fprintf(fp,"V 8.%d.4\t\tBOARD%d_TEMP_AVG 		0 	f4r 	f4r\n",i+1,i+1); 
    fprintf(fp,"V 8.%d.3\t\tBOARD%d_FIRMWARE 		UNK 	a256 	a256\n",i+1,i+1); 
    fprintf(fp,"V 8.%d.4\t\tBOARD%d_HOSTNAME 		UNK 	a256 	a256\n",i+1,i+1); 
    }
  
  fprintf(fp,"V 9 		CMD_STAT 		0 	r606 	r606\n"); 
  
  fprintf(fp,"B 10           TBS_CONFIG 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 10.1         TBS_CONFIG_FREQ		0	f8r	f8r\n");
  fprintf(fp,"V 10.2         TBS_CONFIG_FILTER	0	i2ur	i2ur\n");
  
  for ( i=0; i<4; i++ ) {
    fprintf(fp,"B 11.%d		DRX_CONFIG_%d		NUL	NUL	NUL\n",i+1,i+1);
    for ( j=0; j<2; j++ ) {
      fprintf(fp,"B 11.%d.%d		DRX_CONFIG_%d_%d		NUL	NUL	NUL\n",i+1,j+1,i+1,j+1);
      fprintf(fp,"V 11.%d.%d.1	DRX_CONFIG_%d_%d_FREQ		0	f8r	f8r\n",i+1,j+1,i+1,j+1);
      fprintf(fp,"V 11.%d.%d.2	DRX_CONFIG_%d_%d_FILTER		0	i2ur	i2ur\n",i+1,j+1,i+1,j+1);
      fprintf(fp,"V 11.%d.%d.3	DRX_CONFIG_%d_%d_GAIN		0	i2ur	i2ur\n",i+1,j+1,i+1,j+1);
      }
    }
  
  fprintf(fp,"B 12 		SERVER_STAT		NUL	NUL	NUL\n");
  for ( i=0; i<ME_MAX_NSERVER; i++ ) {
    fprintf(fp,"B 12.%d\t\tSERVER%d		NUL	NUL	NUL\n",i+1,i+1);
    fprintf(fp,"V 12.%d.1\t\tSERVER%d_STAT 		0 	i8ur 	i8ur\n",i+1,i+1); 
    fprintf(fp,"V 12.%d.2\t\tSERVER%d_TEMP_MIN 		0 	f4r 	f4r\n",i+1,i+1); 
    fprintf(fp,"V 12.%d.3\t\tSERVER%d_TEMP_MAX 		0 	f4r 	f4r\n",i+1,i+1); 
    fprintf(fp,"V 12.%d.4\t\tSERVER%d_TEMP_AVG 		0 	f4r 	f4r\n",i+1,i+1); 
    fprintf(fp,"V 12.%d.3\t\tSERVER%d_SOFTWARE 		UNK 	a256 	a256\n",i+1,i+1); 
    fprintf(fp,"V 12.%d.4\t\tSERVER%d_HOSTNAME 		UNK 	a256 	a256\n",i+1,i+1); 
    }
  
  fprintf(fp,"B 13		GLOBAL_INFO		NUL	NUL	NUL\n");
  fprintf(fp,"V 13.1		GLOBAL_TEMP_MIN	0	f4r	f4r\n");
  fprintf(fp,"V 13.2		GLOBAL_TEMP_MAX	0	f4r	f4r\n");
  fprintf(fp,"V 13.3		GLOBAL_TEMP_AVG	0	f4r	f4r\n");
  
  close(fp);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// ms_makeMIB_NDP.c: J. Dowell, UNM, 2022 May 2
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
