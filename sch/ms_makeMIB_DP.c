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
  fp = fopen("DP__MIB_init.dat","w");

  /* lay in DP-specific entries used by MCS (not part of DP ICD) */

  fprintf(fp,"B 0.4           TBW_TRIGGER 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 0.4.1         TBW_BITS                0       a2      NUL\n");
  fprintf(fp,"V 0.4.2         TBW_TRIG_TIME           0       a9      NUL\n");
  fprintf(fp,"V 0.4.3         TBW_SAMPLES             0       a8      NUL\n");
  fprintf(fp,"V 0.4.4         TBW_REFERENCE           0       a9      NUL\n");
  fprintf(fp,"V 0.4.5         TBW_CMD_SENT_MPM        0       a9      NUL\n");  
 
  fprintf(fp,"B 0.5           TBN_CONFIG 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 0.5.1         TBN_FREQ                0.000\ta12     NUL\n");
  fprintf(fp,"V 0.5.2         TBN_BW                  0       a1      NUL\n");
  fprintf(fp,"V 0.5.3         TBN_GAIN                0       a2      NUL\n");
  fprintf(fp,"V 0.5.4         TBN_SUB_SLOT            0       a2      NUL\n");
  fprintf(fp,"V 0.5.5         TBN_REFERENCE           0       a9      NUL\n");
  fprintf(fp,"V 0.5.6         TBN_CMD_SENT_MPM        0       a9      NUL\n"); 

  fprintf(fp,"B 0.6           CH_FILTER_SET 		NUL 	NUL 	NUL\n");
  for ( i=1; i<=9; i++ ) {                                             /* name of '.cf' file of coefficient sets */
    fprintf(fp,"V 0.6.%d\t\tCF%03d\t\t\tUNK 	a32 	NUL\n",i,i);
    }
  for ( i=10; i<=520; i++ ) {
    fprintf(fp,"V 0.6.%d\tCF%03d\t\t\tUNK 	a32 	NUL\n",i,i);
    }

  fprintf(fp,"B 0.7           BEAM_CONFIG 		NUL 	NUL 	NUL\n");
  fprintf(fp,"B 0.7.1         BEAM1 			NUL 	NUL 	NUL\n"); /* corresponds to DRX_BEAM in DRX commands, BEAM_ID in BAM commands */
  fprintf(fp,"V 0.7.1.1       BEAM1_DFILE 		UNK 	a32 	NUL\n"); /* name of file of delays (BEAM_DELAY[520] in BAM command */
  fprintf(fp,"V 0.7.1.2       BEAM1_GFILE 		UNK 	a32 	NUL\n"); /* name of file of gains (sint16 BEAM_GAIN[260][2][2] in BAM command */
  fprintf(fp,"B 0.7.1.3       BEAM1_T1		UNK 	NUL 	NUL\n"); /* corresponds to DRX_TUNING in DRX commands */
  fprintf(fp,"V 0.7.1.3.1     BEAM1_T1_FREQ 		0.000 	a12 	NUL\n"); /* corresponds to DRX_FREQ in DRX commands [Hz] */
  fprintf(fp,"V 0.7.1.3.2     BEAM1_T1_BW 		0 	a1 	NUL\n"); /* 1-7 */
  fprintf(fp,"V 0.7.1.3.3     BEAM1_T1_GAIN 		0 	a2 	NUL\n"); /* 0-15 */
  fprintf(fp,"B 0.7.1.4       BEAM1_T2		UNK 	NUL 	NUL\n"); 
  fprintf(fp,"V 0.7.1.4.1     BEAM1_T2_FREQ 		0.000 	a12 	NUL\n"); 
  fprintf(fp,"V 0.7.1.4.2     BEAM1_T2_BW 		0 	a1 	NUL\n"); 
  fprintf(fp,"V 0.7.1.4.3     BEAM1_T2_GAIN 		0 	a2 	NUL\n"); 

  fprintf(fp,"B 0.7.2         BEAM2 			NUL 	NUL 	NUL\n"); 
  fprintf(fp,"V 0.7.2.1       BEAM2_DFILE 		UNK 	a32 	NUL\n"); 
  fprintf(fp,"V 0.7.2.2       BEAM2_GFILE 		UNK 	a32 	NUL\n"); 
  fprintf(fp,"B 0.7.2.3       BEAM2_T1		UNK 	NUL 	NUL\n"); 
  fprintf(fp,"V 0.7.2.3.1     BEAM2_T1_FREQ 		0.000 	a12 	NUL\n"); 
  fprintf(fp,"V 0.7.2.3.2     BEAM2_T1_BW 		0 	a1 	NUL\n"); 
  fprintf(fp,"V 0.7.2.3.3     BEAM2_T1_GAIN 		0 	a2 	NUL\n"); 
  fprintf(fp,"B 0.7.2.4       BEAM2_T2		UNK 	NUL 	NUL\n"); 
  fprintf(fp,"V 0.7.2.4.1     BEAM2_T2_FREQ 		0.000 	a12 	NUL\n"); 
  fprintf(fp,"V 0.7.2.4.2     BEAM2_T2_BW 		0 	a1 	NUL\n"); 
  fprintf(fp,"V 0.7.2.4.3     BEAM2_T2_GAIN 		0 	a2 	NUL\n"); 

  fprintf(fp,"B 0.7.3         BEAM3 			NUL 	NUL 	NUL\n"); 
  fprintf(fp,"V 0.7.3.1       BEAM3_DFILE 		UNK 	a32 	NUL\n"); 
  fprintf(fp,"V 0.7.3.2       BEAM3_GFILE 		UNK 	a32 	NUL\n"); 
  fprintf(fp,"B 0.7.3.3       BEAM3_T1		UNK 	NUL 	NUL\n"); 
  fprintf(fp,"V 0.7.3.3.1     BEAM3_T1_FREQ 		0.000 	a12 	NUL\n"); 
  fprintf(fp,"V 0.7.3.3.2     BEAM3_T1_BW 		0 	a1 	NUL\n"); 
  fprintf(fp,"V 0.7.3.3.3     BEAM3_T1_GAIN 		0 	a2 	NUL\n"); 
  fprintf(fp,"B 0.7.3.4       BEAM3_T2		UNK 	NUL 	NUL\n"); 
  fprintf(fp,"V 0.7.3.4.1     BEAM3_T2_FREQ 		0.000 	a12 	NUL\n"); 
  fprintf(fp,"V 0.7.3.4.2     BEAM3_T2_BW 		0 	a1 	NUL\n"); 
  fprintf(fp,"V 0.7.3.4.3     BEAM3_T2_GAIN 		0 	a2 	NUL\n"); 

  fprintf(fp,"B 0.7.4         BEAM4 			NUL 	NUL 	NUL\n"); 
  fprintf(fp,"V 0.7.4.1       BEAM4_DFILE 		UNK 	a32 	NUL\n"); 
  fprintf(fp,"V 0.7.4.2       BEAM4_GFILE 		UNK 	a32 	NUL\n"); 
  fprintf(fp,"B 0.7.4.3       BEAM4_T1		UNK 	NUL 	NUL\n"); 
  fprintf(fp,"V 0.7.4.3.1     BEAM4_T1_FREQ 		0.000 	a12 	NUL\n"); 
  fprintf(fp,"V 0.7.4.3.2     BEAM4_T1_BW 		0 	a1 	NUL\n"); 
  fprintf(fp,"V 0.7.4.3.3     BEAM4_T1_GAIN 		0 	a2 	NUL\n"); 
  fprintf(fp,"B 0.7.4.4       BEAM4_T2		UNK 	NUL 	NUL\n"); 
  fprintf(fp,"V 0.7.4.4.1     BEAM4_T2_FREQ 		0.000 	a12 	NUL\n"); 
  fprintf(fp,"V 0.7.4.4.2     BEAM4_T2_BW 		0 	a1 	NUL\n"); 
  fprintf(fp,"V 0.7.4.4.3     BEAM4_T2_GAIN 		0 	a2 	NUL\n"); 

  /* lay in MCS-RESERVED */

  fprintf(fp,"B 1 		MCS-RESERVED 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 1.1 		SUMMARY			UNK	a7	a7\n");
  fprintf(fp,"V 1.2 		INFO      		UNK  	a256	a256\n");
  fprintf(fp,"V 1.3 		LASTLOG   		UNK	a256	a256\n");
  fprintf(fp,"V 1.4 		SUBSYSTEM 		UNK	a3	a3\n");
  fprintf(fp,"V 1.5 		SERIALNO  		UNK	a5	a5\n");
  fprintf(fp,"V 1.6 		VERSION   		UNK	a256	a256\n");

  /* DP-specific */

  fprintf(fp,"V 2 		TBW_STATUS		0	i1ur	i1ur\n");

  fprintf(fp,"V 3 		NUM_TBN_BITS		0	i1ur	i1ur\n");

  fprintf(fp,"V 4.1 		NUM_DRX_TUNINGS		0	i1ur	i1ur\n");
  fprintf(fp,"V 4.2 		NUM_BEAMS		0	i1ur	i1ur\n"); 
  fprintf(fp,"V 4.3 		NUM_STANDS		0	i2ur	i2ur\n");
  fprintf(fp,"V 4.4 		NUM_BOARDS		0	i1ur	i1ur\n");
  fprintf(fp,"V 4.5 		BEAM_FIR_COEFFS		0	i1ur	i1ur\n");
  fprintf(fp,"B 4.6 		T_NOM			NUL	NUL	NUL\n");
  fprintf(fp,"V 4.6.1\t\tT_NOM1			0	i2ur	i2ur\n");
  fprintf(fp,"V 4.6.2\t\tT_NOM2			0	i2ur	i2ur\n");
  fprintf(fp,"V 4.6.3\t\tT_NOM3			0	i2ur	i2ur\n");
  fprintf(fp,"V 4.6.4\t\tT_NOM4			0	i2ur	i2ur\n");
  fprintf(fp,"V 4.6.5\t\tT_NOM5			0	i2ur	i2ur\n");
  fprintf(fp,"V 4.6.6\t\tT_NOM6			0	i2ur	i2ur\n");
  fprintf(fp,"V 4.6.7\t\tT_NOM7			0	i2ur	i2ur\n");
  fprintf(fp,"V 4.6.8\t\tT_NOM8			0	i2ur	i2ur\n");
  fprintf(fp,"V 4.6.9\t\tT_NOM9			0	i2ur	i2ur\n");
  fprintf(fp,"V 4.6.10\tT_NOM10			0	i2ur	i2ur\n");
  fprintf(fp,"V 4.6.11\tT_NOM11			0	i2ur	i2ur\n");
  fprintf(fp,"V 4.6.12\tT_NOM12			0	i2ur	i2ur\n");
  fprintf(fp,"V 4.6.13\tT_NOM13			0	i2ur	i2ur\n");
  fprintf(fp,"V 4.6.14\tT_NOM14			0	i2ur	i2ur\n");
  fprintf(fp,"V 4.6.15\tT_NOM15			0	i2ur	i2ur\n");
  fprintf(fp,"V 4.6.16\tT_NOM16			0	i2ur	i2ur\n");

  fprintf(fp,"B 5 		FIR			NUL	NUL	NUL\n");
  fprintf(fp,"V 5.1 		FIR1			NUL	r1024	r1024\n");
  fprintf(fp,"V 5.2 		FIR2			NUL	r1024	r1024\n");
  fprintf(fp,"V 5.3 		FIR3			NUL	r1024	r1024\n");
  fprintf(fp,"V 5.4 		FIR4			NUL	r1024	r1024\n");
  fprintf(fp,"V 5.5 		FIR_CHAN_INDEX		0	i2u	i2u\n");

  fprintf(fp,"V 6 		CLK_VAL			0	i4ur	i4ur\n");
 
  for ( i=0; i<9; i++ ) {
    fprintf(fp,"B 7.%d\t\tANT%d_STAT 		NUL 	NUL 	NUL\n",i+1,i+1); 
    fprintf(fp,"V 7.%d.1\t\tANT%d_RMS\t\t0 	f4r 	f4r\n",i+1,i+1);
    fprintf(fp,"V 7.%d.2\t\tANT%d_DCOFFSET\t\t0 	f4r 	f4r\n",i+1,i+1);
    fprintf(fp,"V 7.%d.3\t\tANT%d_SAT\t\t0 	i4ur 	i4ur\n",i+1,i+1);
    }
  for ( i=9; i<520; i++ ) {
    fprintf(fp,"B 7.%d\t\tANT%d_STAT 		NUL 	NUL 	NUL\n",i+1,i+1); 
    fprintf(fp,"V 7.%d.1\tANT%d_RMS\t\t0 	f4r 	f4r\n",i+1,i+1);
    fprintf(fp,"V 7.%d.2\tANT%d_DCOFFSET\t\t0 	f4r 	f4r\n",i+1,i+1);
    fprintf(fp,"V 7.%d.3\tANT%d_SAT\t\t0 	i4ur 	i4ur\n",i+1,i+1);
    }
  fprintf(fp,"V 7.521 	STAT_SAMP_SIZE 		0 	i4ur 	i4ur\n"); 

  fprintf(fp,"B 8 		BOARD_STAT		NUL	NUL	NUL\n");
  for ( i=0; i<28; i++ ) {
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

