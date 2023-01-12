// tprs.c: S.W. Ellingson, Virginia Tech, 2011 Apr 05
// ---
// COMPILE: gcc -o tprs.c tprs.c -I../common
// ---
// COMMAND LINE: ./tprs <ssmif>
//   <ssmif>  path/name for SSMIF
// ---
// Reads specified SSMIF and reduces it to a packed binary structure which can be 
// read by MCS/Executive and utility programs
// See end of this file for history.

#include <stdlib.h>

#include "mt.h"

#define ME_FILENAME_MAX_LENGTH 256
#define MAX_SSMIF_LINE_LENGTH 4096
#define MERS_VERBOSE 0 

/* These used by mers_parse_line() */
#define MERS_PL_KEYWORD_MATCH    0
#define MERS_PL_BC_LINE          1 /* "BC" meaning "blank" or "comment" */
#define MERS_PL_EOF              2
#define MERS_PL_KEYWORD_MISMATCH 3
#define MERS_PL_OVERLONG_LINE    4

static struct ssmif_struct s; 

/*==============================================================*/
/*=== iswhitespace() ===========================================*/
/*==============================================================*/

int iswhitespace( char c ) {
  int bResult=0;
  if (c==0x09) bResult=0x09; /* horizontal tab */
  if (c==0x0a) bResult=0x0a; /* linefeed */
  if (c==0x0b) bResult=0x0b; /* vertical tab */
  if (c==0x0c) bResult=0x0c; /* form feed */
  if (c==0x0d) bResult=0x0d; /* carriage return */
  if (c==0x20) bResult=0x20; /* space */
  return bResult;
  }

/*==============================================================*/
/*=== mers_parse_line() ========================================*/
/*==============================================================*/

int mers_parse_line( 
  FILE *fp,             /* pointer to SSMIF (used only if file input specified; see below) */
  char *target_keyword, /* keyword expected in line to be read */
  char *data,           /* on input: */
                        /*   if this is a null string, input comes from the file */
                        /*   otherwise, this string is taken to be the input, and the file is ignored */
                        /* on output: */  
                        /*   if keyword found, this is the associated data field */
                        /*   if keyword mismatch, this will be the entire line that was read */
                        /*   otherwise, a null string is returned */
  int bVerbose          /* =1 means be verbose; =0 normally */ 
  ) {

  char line[MAX_SSMIF_LINE_LENGTH];
  int i;

  if (strlen(data)==0) { /* file input is specified */
      if (feof(fp)) {
        if (bVerbose) printf("[%d/%d] mers_parse_line == MERS_PL_EOF\n",MT_TPRS,getpid());
        return MERS_PL_EOF;
        }
      fgets(line,MAX_SSMIF_LINE_LENGTH,fp);
    } else {             /* use "data" as the input */
      strcpy(line,data);
    }

  strcpy(data,""); /* initialize "data" */

  //if (bVerbose) printf("[%d/%d] mers_parse_line: line='%s'\n",MT_TPRS,getpid(),line);

  if (strlen(line)>MAX_SSMIF_LINE_LENGTH) {
    if (bVerbose) printf("[%d/%d] mers_parse_line == MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());
    return MERS_PL_OVERLONG_LINE;
    }

  if ( (iswhitespace(line[0])) || (line[0]=='#') ) {
    if (bVerbose) printf("[%d/%d] mers_parse_line == MERS_PL_BC_LINE\n",MT_TPRS,getpid());
    return MERS_PL_BC_LINE;
    }

  if (strncmp(line,target_keyword,strlen(target_keyword))!=0) { /* keyword mismatch */
    strcpy(data,line); /* the line is returned, so it can be used later */
    if (bVerbose) printf("[%d/%d] mers_parse_line == MERS_PL_KEYWORD_MISMATCH:\n line='%s' target='%s'\n",MT_TPRS,getpid(),line,target_keyword);
    return MERS_PL_KEYWORD_MISMATCH;
    }

  /* find first non-whitespace character in data field */
  i = strlen(target_keyword);
  while ( iswhitespace( line[i] ) && (i<(strlen(line))-1) ) i++;
 
  strcpy(data,&(line[i]));
  data[strlen(data)-1]='\0'; /* overwrite '\n' with string terminator */

  /* check for embedded comment in this line */
  i=0; 
  while (i<strlen(data)) {
    if (data[i]=='#') { data[i]='\0'; }
    i++;
    }

  /* remove trailing whitespace */
  i=strlen(data)-1;
  while (iswhitespace(data[i])) {
    data[i]='\0';
    i--;
    }

  if (bVerbose) printf("[%d/%d] mers_parse_line == MERS_PL_KEYWORD_MATCH\n",MT_TPRS,getpid());
  return MERS_PL_KEYWORD_MATCH;
  }
/*=== tpss_parse_line() ===*/

/*==============================================================*/
/*=== main() ===================================================*/
/*==============================================================*/

int main ( int narg, char *argv[] ) {

  char filename_ssmif[ME_FILENAME_MAX_LENGTH];

  FILE* fp;

  int i;
  char keyword[MAX_SSMIF_LINE_LENGTH];
  char data[MAX_SSMIF_LINE_LENGTH];

  int b;
  char sTemp[ME_MAX_SSNAME_LENGTH+1];

  int k,m;
  int iStd;
  int iAnt;
  int iFEE;
  int iRPD;
  int iSEP;
  int iARB;
  int iDP1;
  int iDP2;
  int iDR;
  int iRack;
  int iPort;

  int eAntDesi_default;
  int eFEEDesi_default;
  float fFEEGai1_default;
  float fFEEGai2_default;
  int eRPDDesi_default;
  float fRPDVF_default;
  float fRPDDD_default;
  float fRPDA0_default;
  float fRPDA1_default;
  float fRPDFref_default;
  float fRPDStr_default;
  float fSEPLeng_default;
  int   eSEPDesi_default;
  float fSEPGain_default;
  int   eARBDesi_default;
  float fARBGain_default;
  signed short int fee_default;
  signed short int asp_flt_default;
  signed short int asp_at1_default;
  signed short int asp_at2_default;
  signed short int asp_ats_default;

  int c[2*ME_MAX_NSTD];

  /* Parse command line */
  if (narg<2) {
    printf("[%d/%d] FATAL: filename_ssmif not specified\n",MT_TPRS,getpid());
    exit(EXIT_FAILURE);
    }
  sprintf(filename_ssmif,"%s",argv[1]);
  printf("[%d/%d] Input: filename_ssmif='%s'\n",MT_TPRS,getpid(),filename_ssmif);
  if (!strcmp(filename_ssmif,"ssmif.dat")) {
    printf("[%d/%d] FATAL: Input SSMIF cannot be named 'ssmif.dat' (this filename reserved)\n",MT_TPRS,getpid());
    exit(EXIT_FAILURE);  
    }

  printf("[%d/%d] *********************************************************\n",MT_TPRS,getpid()); 
  printf("[%d/%d] *** Reading SSMIF ***************************************\n",MT_TPRS,getpid());
  printf("[%d/%d] *********************************************************\n",MT_TPRS,getpid());

  /* Open SSMIF */
  if (!(fp = fopen(filename_ssmif,"r"))) {
    printf("[%d/%d] FATAL: unable to fopen SSMIF '%s'\n",MT_TPRS,getpid(),filename_ssmif);
    exit(EXIT_FAILURE);
    }

  strcpy(data,"");
  #include "tprs1.c" /* a lot of code for reading SSMIF keywords */
  fclose(fp);

  /* check for multiple ARB_ANT[][] assignments to the same antenna */
  for ( iAnt=0; iAnt<(2*s.nStd); iAnt++ ) { c[iAnt]=0; } 
  for ( iARB=0; iARB<s.nARB; iARB++ ) {
    for (k=0;k<s.nARBCH;k++) {
      if (s.iARBAnt[iARB][k]!=0) {
        if ( c[ s.iARBAnt[iARB][k] -1 ] ) {
          printf("[%d/%d] FATAL: ARB_ANT[%d][%d] = %d has multiple assignments\n",MT_TPRS,getpid(),iARB+1,k+1,s.iARBAnt[iARB][k]);
          //return 1; 
          }
        } 
      c[ s.iARBAnt[iARB][k] -1 ] = 1;
      //printf("[%d/%d] Saw: ARB_ANT[%d][%d] = %d\n",MT_TPRS,getpid(),iARB+1,k+1,s.iARBAnt[iARB][k]);
      } /* for (k */
    } /* for ( iARB */     
  //return 1;

  printf("[%d/%d] *********************************************************\n",MT_TPRS,getpid()); 
  printf("[%d/%d] *** Writing ssmif.dat ***********************************\n",MT_TPRS,getpid());
  printf("[%d/%d] *********************************************************\n",MT_TPRS,getpid());

  if (!(fp = fopen("ssmif.dat","wb"))) {
    printf("[%d/%d] FATAL: unable to fopen 'ssmif.dat'\n",MT_TPRS,getpid());
    exit(EXIT_FAILURE);
    }  

  fwrite(&s,sizeof(s),1,fp);
  //size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream); 

  fclose(fp);

  exit(EXIT_SUCCESS);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// tprs.c: J. Dowell, UNM, 2018 Jan 29
//   .1 Cleaned up a few compiler warnings
// tprs.c: S.W. Ellingson, Virginia Tech, 2011 Apr 05
//   .1: Added additional "settings" keywords
// tprs.c: S.W. Ellingson, Virginia Tech, 2011 Mar 29
//   .1: Moved to MCS/TP from MCS/Exec & renamed
// mers.c: S.W. Ellingson, Virginia Tech, 2011 Mar 03
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
