// ms_makeMIB_DR.c: J. Dowell, UNM, 2015 Sep 10
// ---
// COMPILE: gcc -o ms_makeMIB_DR ms_makeMIB_DR.c
// ---
// COMMAND LINE: ms_makeMIB_DR <ID> <MaxScheduleCount> <MaxDirectoryCount> <MaxDeviceCount> <MaxCPUCount> <MaxHDDCount> <MaxFormatsCount> <MaxLogCount> <MaxDRSUCount>
//   <ID> is an integer (1-5) identifying which MCS-DR we're talking about 
//   <MaxScheduleCount> maximum expected value of SCHEDULE-COUNT
//   <MaxDirectoryCount> maximum expected value of DIRECTORY-COUNT
//   <MaxDeviceCount> maximum expected value of DEVICE-COUNT
//   <MaxCPUCount> maximum expected value of CPU-COUNT
//   <MaxHDDCount> maximum expected value of HDD-COUNT
//   <MaxFormatsCount> maximum expected value of FORMATS-COUNT
//   <MaxLogCount> maximum expected value of LOG-COUNT
//   <MaxDRSUCount> maximum expected value of DRSU-COUNT
// ---
// EXAMPLE: $ ./ms_makeMIB_DR 1 5 5 2 8 5 3 10 2
//
// ---
// REQUIRES: 
// ---
// Creates DR#_MIB_init.dat, where "#"=<ID>.  
// See end of this file for history.

#include <stdio.h>

//#include "LWA_MCS.h"

#define MY_NAME "ms_makeMIB_DR (v.20150910.1)"
#define ME "15" 

main ( int narg, char *argv[] ) {

  /*=================*/
  /*=== Variables ===*/
  /*=================*/

  int id = 0;
  int nMaxScheduleCount = 0;
  int nMaxDirectoryCount = 0;
  int nMaxDeviceCount = 0;
  int nMaxCPUCount = 0;
  int nMaxHDDCount = 0;
  int nMaxFormatsCount = 0;
  int nMaxLogCount = 0;
  int nMaxDRSUCount = 0;

  char filename[256];
  FILE *fp;
  int i;


  /*==================*/
  /*=== Initialize ===*/
  /*==================*/
    
  /* First, announce thyself */
  printf("I am %s [%s]\n",MY_NAME,ME);

  if (narg>1) { 
    sscanf(argv[1],"%d",&id);
    } else {
    printf("[%s] FATAL: ID not provided\n",ME);
    return;
    } 
  if (narg>2) { 
    sscanf(argv[2],"%d",&nMaxScheduleCount);
    } else {
    printf("[%s] FATAL: nMaxScheduleCount not provided\n",ME);
    return;
    } 
  if (narg>3) { 
    sscanf(argv[3],"%d",&nMaxDirectoryCount);
    } else {
    printf("[%s] FATAL: nMaxDirectoryCount not provided\n",ME);
    return;
    } 
  if (narg>4) { 
    sscanf(argv[4],"%d",&nMaxDeviceCount);
    } else {
    printf("[%s] FATAL: nMaxDeviceCount not provided\n",ME);
    return;
    } 
  if (narg>5) { 
    sscanf(argv[5],"%d",&nMaxCPUCount);
    } else {
    printf("[%s] FATAL: nMaxCPUCount not provided\n",ME);
    return;
    } 
  if (narg>6) { 
    sscanf(argv[6],"%d",&nMaxHDDCount);
    } else {
    printf("[%s] FATAL: nMaxHDDCount not provided\n",ME);
    return;
    } 
  if (narg>7) { 
    sscanf(argv[7],"%d",&nMaxFormatsCount);
    } else {
    printf("[%s] FATAL: nMaxFormatsCount not provided\n",ME);
    return;
    } 
  if (narg>8) { 
    sscanf(argv[8],"%d",&nMaxLogCount);
    } else {
    printf("[%s] FATAL: nMaxLogCount not provided\n",ME);
    return;
    } 
  if (narg>9) { 
    sscanf(argv[9],"%d",&nMaxDRSUCount);
    } else {
    printf("[%s] FATAL: nMaxDRSUCount not provided\n",ME);
    return;
    } 

  /* create file */
  sprintf(filename,"DR%1d_MIB_init.dat",id);
  fp = fopen(filename,"w");

  ///* lay in added MIB Entries */
  //fprintf(fp,"B 0.9  \t\tMCS-SUPPLEMENTAL 	NUL 	NUL 	NUL\n");
  //fprintf(fp,"V 0.9.1\t\tN-BOARDS	 	UNK 	a3 	a3\n");  // Number of boards (see INI command)

  /* Additional MIB entries */
  fprintf(fp,"B 0.8 		BUF 			NUL 	NUL 	NUL\n");   /* Parameters sent in accepted "BUF" command */
  fprintf(fp,"V 0.8.1 	BUF-CONFIG		UNK	a5	a5\n");    /*   First 5 single-character parameters */
  fprintf(fp,"V 0.8.2 	BUF-OFFSET		UNK	a16	a16\n");   /*   Offset */
  fprintf(fp,"V 0.8.3 	BUF-WIDTH		UNK	a16	a16\n");   /*   Width */
  fprintf(fp,"V 0.8.4 	BUF-INTERVAL		UNK	a16	a16\n");   /*   Interval */ 
  fprintf(fp,"B 0.9 		GET 			NUL 	NUL 	NUL\n");   /* Response to last "GET" command */
  fprintf(fp,"V 0.9.1 	GET-TAG			UNK	a16	a16\n");   /*   "Tag" identifying recording this came from */ 
  fprintf(fp,"V 0.9.2 	GET-START-BYTE		UNK	a15	a15\n");   /*   Start byte within recording */
  fprintf(fp,"V 0.9.3 	GET-LENGTH		UNK	a15	a15\n");   /*   Length of data in bytes*/
  fprintf(fp,"V 0.9.4 	GET-DATA		UNK	r8192	r8192\n"); /*   Data returned */  

  /* lay in MCS-RESERVED */
  fprintf(fp,"B 1 		MCS-RESERVED 		NUL 	NUL 	NUL\n");
  fprintf(fp,"V 1.1 		SUMMARY			UNK	a7	a7\n");
  fprintf(fp,"V 1.2 		INFO      		UNK	a256	a256\n");
  fprintf(fp,"V 1.3 		LASTLOG   		UNK	a256	a256\n");
  fprintf(fp,"V 1.4 		SUBSYSTEM 		UNK	a3	a3\n");
  fprintf(fp,"V 1.5 		SERIALNO  		UNK	a5	a5\n");
  fprintf(fp,"V 1.6 		VERSION   		UNK	a256	a256\n");
 
  fprintf(fp,"B 2 		CURRENT-OPERATION	NUL	NUL	NUL\n");
  fprintf(fp,"V 2.1 		OP-TYPE			UNK	a11	a11\n");

  fprintf(fp,"B 2.2 		OP-SCHEDULE		NUL	NUL	NUL\n");
  fprintf(fp,"V 2.2.1 	OP-START		UNK	a16	a16\n");
  fprintf(fp,"V 2.2.2 	OP-STOP			UNK	a16	a16\n");
  fprintf(fp,"V 2.3 		OP-REFERENCE		UNK	a9	a9\n");

  fprintf(fp,"B 2.4 		OP-FILEINFO-INTERNAL	NUL	NUL	NUL\n");
  fprintf(fp,"V 2.4.1 	OP-TAG			UNK	a16	a16\n");
  fprintf(fp,"V 2.4.2 	OP-FORMAT		UNK	a32	a32\n");
  fprintf(fp,"V 2.4.3 	OP-POSITION		UNK	a47	a47\n");

  fprintf(fp,"B 2.5 		OP-FILEINFO-EXTERNAL	NUL	NUL	NUL\n");
  fprintf(fp,"V 2.5.1 	OP-FILENAME		UNK	a193	a193\n");
  fprintf(fp,"V 2.5.2 	OP-FILEINDEX		UNK	a9	a9\n");

  fprintf(fp,"B 3 		SCHEDULE		NUL	NUL	NUL\n");
  fprintf(fp,"V 3.1 		SCHEDULE-COUNT		UNK	a6	a6\n");
  fprintf(fp,"B 3.2 		SCHEDULE-ENTRIES	NUL	NUL	NUL\n");
  for ( i=0; i<nMaxScheduleCount; i++) {
    fprintf(fp,"V 3.2.%d		SCHEDULE-ENTRY-%d 	UNK 	a76 	a76\n",i+1,i+1);
    }

  fprintf(fp,"B 4 		DIRECTORY		NUL	NUL	NUL\n");
  fprintf(fp,"V 4.1 		DIRECTORY-COUNT		UNK	a6	a6\n");
  fprintf(fp,"B 4.2 		DIRECTORY-ENTRIES	NUL	NUL	NUL\n");
  for ( i=0; i<nMaxDirectoryCount; i++) {
    fprintf(fp,"V 4.2.%d		DIRECTORY-ENTRY-%d 	UNK 	a112 	a112\n",i+1,i+1);
    }

  fprintf(fp,"B 5 		STORAGE-INFO		NUL	NUL	NUL\n");
  fprintf(fp,"V 5.1 		TOTAL-STORAGE		UNK	a15	a15\n");
  fprintf(fp,"V 5.2 		REMAINING-STORAGE	UNK	a15	a15\n");
  fprintf(fp,"V 5.3 		CONTIGUOUS-STORAGE	UNK	a15	a15\n");

  fprintf(fp,"B 6 		REMOVABLE-DEVICES	NUL	NUL	NUL\n");
  fprintf(fp,"V 6.1 		DEVICE-COUNT		UNK	a6	a6\n");
  fprintf(fp,"B 6.2 		DEVICE-IDS		NUL	NUL	NUL\n");
  for ( i=0; i<nMaxDeviceCount; i++) {
    fprintf(fp,"V 6.2.%d		DEVICE-ID-%d 		UNK 	a64 	a64\n",i+1,i+1);
    }
  fprintf(fp,"B 6.3 		DEVICE-BARCODES		NUL	NUL	NUL\n");
  for ( i=0; i<nMaxDeviceCount; i++) {
    fprintf(fp,"V 6.3.%d		DEVICE-BARCODE-%d 		UNK 	a64 	a64\n",i+1,i+1);
    }
  fprintf(fp,"B 6.4 		DEVICE-STORAGES		NUL	NUL	NUL\n");
  for ( i=0; i<nMaxDeviceCount; i++) {
    fprintf(fp,"V 6.4.%d		DEVICE-STORAGE-%d 	UNK 	a15 	a15\n",i+1,i+1);
    }

  fprintf(fp,"B 7 		CPU-INFO		NUL	NUL	NUL\n");
  fprintf(fp,"V 7.1 		CPU-COUNT		UNK	a3	a3\n");
  fprintf(fp,"B 7.2 		CPU-TEMPS		NUL	NUL	NUL\n");
  for ( i=0; i<nMaxCPUCount; i++) {
    fprintf(fp,"V 7.2.%d		CPU-TEMP-%d 		UNK 	a3 	a3\n",i+1,i+1);
    }

  fprintf(fp,"B 8 		HDD-INFO		NUL	NUL	NUL\n");
  fprintf(fp,"V 8.1 		HDD-COUNT		UNK	a3	a3\n");
  fprintf(fp,"B 8.2 		HDD-TEMPS		NUL	NUL	NUL\n");
  for ( i=0; i<nMaxHDDCount; i++) {
    fprintf(fp,"V 8.2.%d		HDD-TEMP-%d 		UNK 	a3 	a3\n",i+1,i+1);
    }

  fprintf(fp,"B 9 		DATA-FORMATS		NUL	NUL	NUL\n");
  fprintf(fp,"V 9.1 		FORMAT-COUNT		UNK	a3	a3\n");
  fprintf(fp,"B 9.2 		FORMAT-NAMES		NUL	NUL	NUL\n");
  for ( i=0; i<nMaxFormatsCount; i++) {
    fprintf(fp,"V 9.2.%d		FORMAT-NAME-%d 		UNK 	a256 	a256\n",i+1,i+1);
    }
  fprintf(fp,"B 9.3 		FORMAT-PAYLOADS		NUL	NUL	NUL\n");
  for ( i=0; i<nMaxFormatsCount; i++) {
    fprintf(fp,"V 9.3.%d		FORMAT-PAYLOAD-%d 	UNK 	a4 	a4\n",i+1,i+1);
    }
  fprintf(fp,"B 9.4 		FORMAT-RATES		NUL	NUL	NUL\n");
  for ( i=0; i<nMaxFormatsCount; i++) {
    fprintf(fp,"V 9.4.%d		FORMAT-RATE-%d 		UNK 	a9 	a9\n",i+1,i+1);
    }
  fprintf(fp,"B 9.5 		FORMAT-SPECS		NUL	NUL	NUL\n");
  for ( i=0; i<nMaxFormatsCount; i++) {
    fprintf(fp,"V 9.5.%d		FORMAT-SPEC-%d 		UNK 	a256 	a256\n",i+1,i+1);
    }

  fprintf(fp,"B 10 		LOG			NUL	NUL	NUL\n");
  fprintf(fp,"V 10.1 		LOG-COUNT		UNK	a6	a6\n");
  fprintf(fp,"B 10.2 		LOG-ENTRIES		NUL	NUL	NUL\n");
  for ( i=0; i<nMaxLogCount; i++) {
    fprintf(fp,"V 10.2.%d	LOG-ENTRY-%d 		UNK 	a259 	a259\n",i+1,i+1);
    }

  fprintf(fp,"B 11 		LIVE-BUFFER		NUL	NUL	NUL\n");
  fprintf(fp,"V 11.2 		BUFFER			UNK	r8192	r8192\n");
  fprintf(fp,"V 11.2 		BUFFER-RESTRICT		UNK	r8192	r8192\n");

  fprintf(fp,"B 12 		DRSU-STATUS		NUL	NUL	NUL\n");
  fprintf(fp,"V 12.1 		DRSU-COUNT		UNK	a2	a2\n");
  fprintf(fp,"V 12.2 		DRSU-SELECTED		UNK	a2	a2\n");
  fprintf(fp,"B 12.3 		DRSU-INFO		NUL	NUL	NUL\n");
  for ( i=0; i<nMaxDRSUCount; i++) {
    fprintf(fp,"V 12.3.%d	DRSU-INFO-%d 		UNK 	a88 	a88\n",i+1,i+1);
    }
  fprintf(fp,"V 12.4 		DRSU-BARCODE		UNK	a1024	a1024\n");
  
  fprintf(fp,"B 13 		TIMETAG-LAG		NUL	NUL	NUL\n");
  fprintf(fp,"V 13.1		TT-LAG-INITIAL		UNK	a64	a64\n");
  fprintf(fp,"V 13.2		TT-LAG		UNK	a64	a64\n");

  close(fp);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// ms_makeMIB_DR.c: J. Dowell, UNM, 2015 Sep 10
//   .1: Updated for DROS v2
// ms_makeMIB_DR.c: S.W. Ellingson, Virginia Tech, 2013 Jul 18
//   .1: Added 12.4 (DRSU-BARCODE)
// ms_makeMIB_DR.c: S.W. Ellingson, Virginia Tech, 2010 Jun 07
//   .1: Initial version, using ms_makeMIB_ASP.c as a starting point

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================

