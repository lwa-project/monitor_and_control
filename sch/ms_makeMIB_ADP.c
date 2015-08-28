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

//#include "LWA_MCS.h"

#define MY_NAME "ms_makeMIB_ADP (v.20150817.1)"
#define ME "11" 

main ( int narg, char *argv[] ) {

  /*=================*/
  /*=== Variables ===*/
  /*=================*/

  FILE *fp;
  int i;


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
  fp = fopen("ADP_MIB_init.dat","w");

  /* lay in ADP-specific entries used by MCS (not part of ADP ICD) */

  fprintf(fp,"B 0.4           TBF_TRIGGER 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 0.4.1         TBF_BITS                0       a2      NUL\n");
  fprintf(fp,"V 0.4.2         TBF_TRIG_TIME           0       a9      NUL\n");
  fprintf(fp,"V 0.4.3         TBF_SAMPLES             0       a8      NUL\n");
  fprintf(fp,"V 0.4.4         TBF_TUNING_MASK         0       i8ur    NUL\n");
  fprintf(fp,"V 0.4.5         TBF_REFERENCE           0       a9      NUL\n");
  fprintf(fp,"V 0.4.6         TBF_CMD_SENT_MPM        0       a9      NUL\n");  
 
  fprintf(fp,"B 0.5           TBN_CONFIG 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 0.5.1         TBN_FREQ                0.000\ta12     NUL\n");
  fprintf(fp,"V 0.5.2         TBN_BW                  0       a1      NUL\n");
  fprintf(fp,"V 0.5.3         TBN_GAIN                0       a2      NUL\n");
  fprintf(fp,"V 0.5.4         TBN_REFERENCE           0       a9      NUL\n");
  fprintf(fp,"V 0.5.5         TBN_CMD_SENT_MPM        0       a9      NUL\n"); 
  
  fprintf(fp,"B 0.6           COR_CONFIG		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 0.6.1         COR_NAVG                0      a10      NUL\n");
  fprintf(fp,"V 0.6.2         COR_TUNING_MASK         0      i8u      NUL\n");
  fprintf(fp,"V 0.6.3         COR_GAIN                0       a2      NUL\n");
  fprintf(fp,"V 0.6.4         COR_SUB_SLOT            0       a2      NUL\n");
  fprintf(fp,"V 0.6.5         COR_REFERENCE           0       a9      NUL\n");
  fprintf(fp,"V 0.6.6         COR_CMD_SENT_MPM        0       a9      NUL\n"); 

  fprintf(fp,"B 0.7           CH_FILTER_SET 		NUL 	NUL 	NUL\n");
  for ( i=1; i<=512; i++ ) {
    fprintf(fp,"V 0.7.%d\tCF%03d\t\t\tUNK 	a32 	NUL\n",i,i);
    }
  
  fprintf(fp,"B 0.8           TUNING_CONFIG 		NUL 	NUL 	NUL\n");
  for ( i=1; i<=32 , i++ ) {
    fprintf(fp,"B 0.8.%d         TUNINGM%d 			NUL 	NUL 	NUL\n", i, i); /* corresponds to DRX_BEAM in DRX commands, BEAM_ID in BAM commands */
    fprintf(fp,"V 0.8.%d.1     TUNING%d_FREQ 		0.000 	a12 	NUL\n", i, i); /* corresponds to DRX_FREQ in DRX commands [Hz] */
    fprintf(fp,"V 0.8.%d.2     TUNING%d_BW 		0 	a1 	NUL\n", i, i); /* 1-8 */
    fprintf(fp,"V 0.8.%d.3     TUNING%d_GAIN 		0 	a2 	NUL\n", i, i); /* 0-15 */
    }
    
  fprintf(fp,"B 0.9           BEAM_CONFIG 		NUL 	NUL 	NUL\n");
  for ( i=1; i<=32 , i++ ) {
    fprintf(fp,"B 0.9.%d         BEAM%d 			NUL 	NUL 	NUL\n", i, i); /* corresponds to DRX_BEAM in DRX commands, BEAM_ID in BAM commands */
    fprintf(fp,"V 0.9.%d.1       BEAM%d_DFILE 		UNK 	a32 	NUL\n", i, i); /* name of file of delays (BEAM_DELAY[520] in BAM command */
    fprintf(fp,"V 0.9.%d.2       BEAM%d_GFILE 		UNK 	a32 	NUL\n", i, i); /* name of file of gains (sint16 BEAM_GAIN[260][2][2] in BAM command */
    fprintf(fp,"B 0.9.%d.3       BEAM%d_TUNING		UNK 	NUL 	NUL\n", i, i); /* corresponds to DRX_TUNING in DRX commands */
    }

  /* lay in MCS-RESERVED */

  fprintf(fp,"B 1 		MCS-RESERVED 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 1.1 		SUMMARY			UNK	a7	a7\n");
  fprintf(fp,"V 1.2 		INFO      		UNK  a256	a256\n");
  fprintf(fp,"V 1.3 		LASTLOG   		UNK	a256	a256\n");
  fprintf(fp,"V 1.4 		SUBSYSTEM 		UNK	a3	a3\n");
  fprintf(fp,"V 1.5 		SERIALNO  		UNK	a5	a5\n");
  fprintf(fp,"V 1.6 		VERSION   		UNK	a256	a256\n");

  /* ADP-specific */

  fprintf(fp,"V 2 		TBF_STATUS		0	i1ur	i1ur\n");

  fprintf(fp,"V 3 		NUM_TBN_BITS		0	i1ur	i1ur\n");

  fprintf(fp,"V 4.1 		NUM_DRX_TUNINGS		0	i1ur	i1ur\n");
  fprintf(fp,"V 4.2 		NUM_BEAMS		0	i1ur	i1ur\n"); 
  fprintf(fp,"V 4.3 		NUM_STANDS		0	i2ur	i2ur\n");
  fprintf(fp,"V 4.4.1 		NUM_BOARDS		0	i1ur	i1ur\n");
  fprintf(fp,"V 4.4.2 		NUM_SERVERS		0	i1ur	i1ur\n");
  fprintf(fp,"V 4.5 		BEAM_FIR_COEFFS		0	i1ur	i1ur\n");
  fprintf(fp,"B 4.6 		T_NOM			NUL	NUL	NUL\n");
  for ( i=1; i<=32 , i++ ) {
    fprintf(fp,"V 4.6.%d\t\tT_NOM%d			0	i2ur	i2ur\n", i, i);
    }
  
  fprintf(fp,"B 5 		FIR			NUL	NUL	NUL\n");
  fprintf(fp,"V 5.1 		FIR1			NUL	r1024	r1024\n");
  fprintf(fp,"V 5.2 		FIR2			NUL	r1024	r1024\n");
  fprintf(fp,"V 5.3 		FIR3			NUL	r1024	r1024\n");
  fprintf(fp,"V 5.4 		FIR4			NUL	r1024	r1024\n");
  fprintf(fp,"V 5.5 		FIR_CHAN_INDEX		0	i2u	i2u\n");

  fprintf(fp,"V 6 		CLK_VAL			0	i4ur	i4ur\n");
 
  for ( i=0; i<512; i++ ) {
    fprintf(fp,"B 7.%d\t\tANT%d_STAT 		NUL 	NUL 	NUL\n",i+1,i+1); 
    fprintf(fp,"V 7.%d.1\tANT%d_RMS\t\t0 	f4r 	f4r\n",i+1,i+1);
    fprintf(fp,"V 7.%d.2\tANT%d_DCOFFSET\t\t0 	f4r 	f4r\n",i+1,i+1);
    fprintf(fp,"V 7.%d.3\tANT%d_SAT\t\t0 	i4ur 	i4ur\n",i+1,i+1);
    fprintf(fp,"V 7.%d.4\t\tANT%d_PEAK\t\t0	fr 	f4r\n",i+1,i+1);
    }
  fprintf(fp,"V 7.521 	STAT_SAMP_SIZE 		0 	i4ur 	i4ur\n"); 

  fprintf(fp,"B 8 		BOARD_STAT		NUL	NUL	NUL\n");
  for ( i=0; i<16; i++ ) {
    fprintf(fp,"V 8.%d\t\tBOARD%d_STAT 		0 	i4ur 	i4ur\n",i+1,i+1); 
    }

  //fprintf(fp,"V 8.29\t\tBOARD29_STAT 		1 	i4u 	i4u\n"); 
  //fprintf(fp,"V 8.29\t\tBOARD30_STAT 		255 	i4u 	i4u\n");
  //fprintf(fp,"V 8.29\t\tBOARD31_STAT 		65536 	i4u 	i4u\n");
  //fprintf(fp,"V 8.29\t\tBOARD32_STAT 		16777215 	i4u 	i4u\n");
  //fprintf(fp,"V 8.29\t\tBOARD33_STAT 		3999999999 	i4u 	i4u\n"); 

  fprintf(fp,"V 9 		CMD_STAT 		0 	r606 	r606\n"); 

  close(fp);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// ms_makeMIB_ADP.c: J. Dowell, UNM, 2015 Aug 17
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================

