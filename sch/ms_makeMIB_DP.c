// ms_makeMIB_DP.c: S.W. Ellingson, Virginia Tech, 2010 May 31
// ---
// COMPILE: gcc -o ms_makeMIB_DP ms_makeMIB_DP.c
// ---
// COMMAND LINE: ms_makeMIB_DP
//
// ---
// REQUIRES: 
// ---
// Creates DP_MIB_init.dat.  
// See end of this file for history.

#include <stdio.h>

//#include "LWA_MCS.h"

#define MY_NAME "ms_makeMIB_DP (v.20100531.1)"
#define ME "11" 

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
  fp = fopen("DP__MIB_init.dat","w");

  /* lay in DP-specific entries used by MCS (not part of DP ICD) */

  fprintf(fp,"B 0.4           MCS_TBW_TRIGGER 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 0.4.1         MCS_TBW_BITS                0       a2      NUL\n");
  fprintf(fp,"V 0.4.2         MCS_TBW_TRIG_TIME           0       a9      NUL\n");
  fprintf(fp,"V 0.4.3         MCS_TBW_SAMPLES             0       a8      NUL\n");
  fprintf(fp,"V 0.4.4         MCS_TBW_REFERENCE           0       a9      NUL\n");
  fprintf(fp,"V 0.4.5         MCS_TBW_CMD_SENT_MPM        0       a9      NUL\n");  
 
  fprintf(fp,"B 0.5           MCS_TBN_CONFIG 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 0.5.1         MCS_TBN_FREQ                0.000\ta12     NUL\n");
  fprintf(fp,"V 0.5.2         MCS_TBN_BW                  0       a1      NUL\n");
  fprintf(fp,"V 0.5.3         MCS_TBN_GAIN                0       a2      NUL\n");
  fprintf(fp,"V 0.5.4         MCS_TBN_SUB_SLOT            0       a2      NUL\n");
  fprintf(fp,"V 0.5.5         MCS_TBN_REFERENCE           0       a9      NUL\n");
  fprintf(fp,"V 0.5.6         MCS_TBN_CMD_SENT_MPM        0       a9      NUL\n"); 

  fprintf(fp,"B 0.6           MCS_CH_FILTER_SET 		NUL 	NUL 	NUL\n");
  for ( i=1; i<=520; i++ ) {                                             /* name of '.cf' file of coefficient sets */
    fprintf(fp,"V 0.6.%d\t\tMCS_CF%03d\t\t\tUNK 	a32 	NUL\n",i,i);
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
  fprintf(fp,"V 1.2 		INFO      		UNK	a256	a256\n");
  fprintf(fp,"V 1.3 		LASTLOG   		UNK	a256	a256\n");
  fprintf(fp,"V 1.4 		SUBSYSTEM 		UNK	a3	a3\n");
  fprintf(fp,"V 1.5 		SERIALNO  		UNK	a5	a5\n");
  fprintf(fp,"V 1.6 		VERSION   		UNK	a256	a256\n");

  /* DP-specific */

  fprintf(fp,"V 2 		TBW_STATUS		0	i1ur	i1ur\n");

  fprintf(fp,"V 3 		NUM_TBN_BITS		0	i1ur	i1ur\n");

  fprintf(fp,"V 4.1 		NUM_DRX_TUNINGS	0	i1ur	i1ur\n");
  fprintf(fp,"V 4.2 		NUM_BEAMS			0	i1ur	i1ur\n"); 
  fprintf(fp,"V 4.3 		NUM_STANDS		0	i2ur	i2ur\n");
  fprintf(fp,"V 4.4 		NUM_BOARDS		0	i1ur	i1ur\n");
  fprintf(fp,"V 4.5 		BEAM_FIR_COEFFS	0	i1ur	i1ur\n");
  fprintf(fp,"B 4.6 		T_NOM			NUL	NUL	NUL\n");
  for ( i=0; i<4; i++ ) {
    fprintf(fp,"V 4.6.%d		T_NOM%d			0	i2ur	i2ur\n",i+1,i+1);
    }

  fprintf(fp,"B 5 			FIR				NUL	NUL		NUL\n");
  for ( i=0; i<4; i++ ) {
    fprintf(fp,"V 5.%d		FIR%d			NUL	r1024	r1024\n",i+1,i+1);
    }
  fprintf(fp,"V 5.5 		FIR_CHAN_INDEX		0	i2u		i2u\n");

  fprintf(fp,"V 6 		CLK_VAL			0	i4ur	i4ur\n");
 
  for ( i=0; i<520; i++ ) {
    fprintf(fp,"B 7.%d		ANT%d_STAT 		NUL 	NUL 	NUL\n",i+1,i+1); 
    fprintf(fp,"V 7.%d.1		ANT%d_RMS			0 	f4r 	f4r\n",i+1,i+1);
    fprintf(fp,"V 7.%d.2		ANT%d_DCOFFSET		0 	f4r 	f4r\n",i+1,i+1);
    fprintf(fp,"V 7.%d.3		ANT%d_SAT			0 	i4ur i4ur\n",i+1,i+1);
    fprintf(fp,"V 7.%d.4		ANT%d_PEAK		0    i4ur i4ur\n",i+1,i+1);
    }
  fprintf(fp,"V 7.521 	STAT_SAMP_SIZE 		0 	i4ur 	i4ur\n"); 

  fprintf(fp,"B 8 		BOARD_STAT		NUL	NUL	NUL\n");
  for ( i=0; i<28; i++ ) {
    fprintf(fp,"B 8.%d		BOARD%d_INFO 		NUL	NUL	NUL\n",i+1,i+1); 
    fprintf(fp,"V 8.%d.1		BOARD%d_STAT		0	i4ur	i4ur	\n",i+1,i+1);
    fprintf(fp,"V 8.%d.2		BOARD%d_TEMP_MIN		0	f4r	f4r	\n",i+1,i+1);
    fprintf(fp,"V 8.%d.3		BOARD%d_TEMP_MAX		0	f4r	f4r	\n",i+1,i+1);
    fprintf(fp,"V 8.%d.4		BOARD%d_TEMP_AVG		0	f4r	f4r	\n",i+1,i+1);
    fprintf(fp,"V 8.%d.5		BOARD%d_FIRMWARE		0	a256	a256	\n",i+1,i+1);
    fprintf(fp,"V 8.%d.6		BOARD%d_HOSTNAME		0	a256	a256	\n",i+1,i+1);
    }

  fprintf(fp,"V 9 		CMD_STAT 		0 	r606 	r606\n"); 
  
  fprintf(fp,"B 10           TBN_CONFIG 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 10.1         TBN_CONFIG_FREQ		0	f4r	f4r\n");
  fprintf(fp,"V 10.2         TBN_CONFIG_FILTER	0	i2ur	i2ur\n");
  fprintf(fp,"V 10.3         TBN_CONFIG_GAIN		0	i2ur	i2ur\n");
  
  fprintf(fp,"B 11		DRX_CONFIG		NUL	NUL	NUL\n");
  for ( i=0; i<4; i++ ) {
    fprintf(fp,"B 11.%d		DRX_CONFIG_%d		NUL	NUL	NUL\n",i+1,i+1);
    for ( j=0; j<2; j++ ) {
      fprintf(fp,"B 11.%d.%d		DRX_CONFIG_%d_%d		NUL	NUL	NUL\n",i+1,j+1,i+1,j+1);
      fprintf(fp,"V 11.%d.%d.1	DRX_CONFIG_%d_%d_FREQ		0	f4r	f4r\n",i+1,j+1,i+1,j+1);
      fprintf(fp,"V 11.%d.%d.2	DRX_CONFIG_%d_%d_FILTER		0	i2ur	i2ur\n",i+1,j+1,i+1,j+1);
      fprintf(fp,"V 11.%d.%d.3	DRX_CONFIG_%d_%d_GAIN		0	i2ur	i2ur\n",i+1,j+1,i+1,j+1);
      }
    }

  close(fp);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// ms_makeMIB_DP.c: S.W. Ellingson, Virginia Tech, 2010 May 31
//   .1: Added MIB branch 0.5, TBN_CONFIG
// ms_makeMIB_DP.c: S.W. Ellingson, Virginia Tech, 2010 May 30
//   .1: Added MIB branch 0.4, TBW_TRIGGER
// ms_makeMIB_DP.c: S.W. Ellingson, Virginia Tech, 2010 May 27
//   .1: Fix: Changed CLK_VAL from branch label to value label
// ms_makeMIB_DP.c: S.W. Ellingson, Virginia Tech, 2010 May 25
//   .1: Minor mods to MIB initial values
// ms_makeMIB_DP.c: S.W. Ellingson, Virginia Tech, 2009 Aug 16
//   .1: Modifying choice of type_dbm format specifiers
// ms_makeMIB_DP.c: S.W. Ellingson, Virginia Tech, 2009 Aug 14
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================

