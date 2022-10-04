// tpss.c: S.W. Ellingson, Virginia Tech, 2014 Mar 10
// ---
// COMPILE: gcc -o tpss tpss.c -I../common
// ---
// COMMAND LINE: tpss <sdf> <max_phase> <bIgnoreActualTime> <mbox>
//   <sdf>:   filename corresponding to a session defintion file (SDF)
//   <max_phase>: highest-numbered phase of processing to execute.
//      1: SDF file parsing
//      2: Consistency & additional error checking
//      3: Check resource availability (i.e., scheduling)
//      4: Generate output SDF, SSF, OSFs (binary files); dumped in local directory
//      5: Push SDF/SSF/OSFs to <mbox> and notify MCS/Executive  
//   <bIgnoreActualTime>: Normally 0. If 1, tpss will not complain about start dates being in the past
//      or too far in the future.  This is to facilitate testing & demonstration.
//      bIgnoreActualTime=1 not allowed if max_phase>=5.
//      bIgnoreActualTime=-1 means the same thing as 1, except max_phase=5 is allowed.  
//        This is for development/testing only and is NOT recommended when MCS/Executive is running
//   <mbox>: The path to the directory on the local machine used to exchange files with MCS/Exec
//      No trailing slash.  Default (if not specified) is "."
// ---
// EXAMPLE:
//   ./tp_session_sch sdf.txt 5
// ---
// REQUIRES: 
//   uses many subroutines from mcs.h
// ---
// See MCS0030 for details about what this code does, and for context within MCS.
// See end of this file for history.

#include "mt.h"

#ifdef USE_ADP
#define TPSS_FORMAT_VERSION 6            /* version of MCS0030 used here - ADP */
#else
#define TPSS_FORMAT_VERSION 5            /* version of MCS0030 used here - DP */
#endif
#define MAX_SDF_LINE_LENGTH 4096
#define MAX_SDF_NOTE_LENGTH 32
#define MAX_NUMBER_OF_OBSERVATIONS 150
#define MAX_MJD_SCHEDULE_AHEAD 30   /* max number of days in the future allowed to schedule */
#define MAX_STP_N 1024              /* max number of steps in a stepped-mode observation */
#define MAX_BEAM_DELAY 65535        /* max value that OBS_BEAM_DELAY[][] can have */
#define TBF_INVERSE_DUTY_CYCLE  150 /* assumed ratio of TBF read-out time to TBF dump time */
#define TBW_INVERSE_DUTY_CYCLE 2500 /* assumed ratio of TBW read-out time to TBW acquisition time */
#define MIN_OBS_STEP_LENGTH 5       /* minimum number of milliseconds in an observation step */


#define TPSS_PL_KEYWORD_MATCH    0
#define TPSS_PL_BLANK_LINE       1
#define TPSS_PL_EOF              2
#define TPSS_PL_KEYWORD_MISMATCH 3
#define TPSS_PL_OVERLONG_LINE    4

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
/*=== tpss_parse_line() ========================================*/
/*==============================================================*/

int tpss_parse_line( 
  FILE *fp,             /* pointer to SDF (used only if file input specified; see below) */
  char *target_keyword, /* keyword expected in line to be read */
  char *data            /* on input: */
                        /*   if this is a null string, input comes from the file */
                        /*   otherwise, this string is taken to be the input, and the file is ignored */
                        /* on output: */  
                        /*   if keyword found, this is the associated data field */
                        /*   if keyword mismatch, this will be the entire line that was read */
                        /*   otherwise, a null string is returned */
  ) {

  char line[MAX_SDF_LINE_LENGTH];
  int i;
  int b1,b2;

  if (strlen(data)==0) { /* file input is specified */
      if (feof(fp)) {
        //printf("tpss_parse_line = %d\n",TPSS_PL_EOF);
        return TPSS_PL_EOF;
        }
      fgets(line,MAX_SDF_LINE_LENGTH,fp);
      //printf("[%d/%d] line='%s'\n",MT_TPSS,getpid(),line);
    } else {             /* use "data" as the input */
      strcpy(line,data);
    }

  strcpy(data,""); /* initialize "data" */

  if (strlen(line)>MAX_SDF_LINE_LENGTH) {
    //printf("tpss_parse_line = %d\n",TPSS_PL_OVERLONG_LINE);
    return TPSS_PL_OVERLONG_LINE;
    }

  if (iswhitespace(line[0])) {
    //printf("tpss_parse_line = %d\n",TPSS_PL_BLANK_LINE);
    return TPSS_PL_BLANK_LINE;
    }

  if (strncmp(line,target_keyword,strlen(target_keyword))!=0) { /* keyword mismatch */
    //printf("tpss_parse_line = %d\n",TPSS_PL_KEYWORD_MISMATCH);
    //printf("<%s> <%s>\n",line,target_keyword);
    strcpy(data,line); /* the line is returned, so it can be used later */
    return TPSS_PL_KEYWORD_MISMATCH;
    }

  /* Above trap fails in cases where subsequent keyword is a better match, but longer; */
  /* e.g., line starting "OBS_DUR+" will trap on "OBS_DUR", which comes earlier. */
  /* To mitigate, require that next character be either " " or "[" */
  b1 = ( line[ strlen(target_keyword) ] == ' ' );
  b2 = ( line[ strlen(target_keyword) ] == '[' );
  if ( (!b1) && (!b2) ) {
    strcpy(data,line); /* the line is returned, so it can be used later */
    //printf("<%s> <%s> %d %d\n",line,target_keyword,b1,b2);
    return TPSS_PL_KEYWORD_MISMATCH;
    }

  //printf("tpss_parse_line = %d\n",TPSS_PL_KEYWORD_MATCH);

  /* find first non-whitespace character in data field */
  i = strlen(target_keyword);
  while ( iswhitespace( line[i] ) && (i<(strlen(line))-1) ) i++;
 
  strcpy(data,&(line[i]));
  data[strlen(data)-1]='\0'; /* overwrite '\n' with string terminator */

  return TPSS_PL_KEYWORD_MATCH;
  }
/*=== tpss_parse_line() ===*/


/*==============================================================*/
/*=== main() ===================================================*/
/*==============================================================*/

int main ( int narg, char *argv[] ) {

  /* command line parameters */
  char sdfname[256]; /* filename of SDF */
  int max_phase=0;
  int bIgnoreActualTime=0;
  char mbox[256];

  FILE *fpsdf;

  int i,k;
  char keyword[MAX_SDF_LINE_LENGTH];
  char data[491520];

  int nobs=0; /* number of observations defined */

  long int mjd, mpm;
  char mode_string[256];

  int n,m,p,q,r;

  struct timeval t0,t1;
  long int d;
  long int ds;

  int b_TB_requested=0;
  int b_DRX_requested=0;
  int b_NOOP_requested=0;

  int err;
  char msg[1024];
  int err_max, i_max;

  long int t_tbw;

  FILE *fp;
  char ssfname[256];
  char osfname[256];

  unsigned char u1;
  unsigned short int u2;
  unsigned int u4;
  unsigned long int u8;
  signed short int i2;
  float f4;

  struct ssf_struct ssf;
  struct osf_struct osf;
  struct osfs_struct osfs;
  struct beam_struct beam;
  struct osf2_struct osf2;

  char cmd[491520];
  char line[1024];

  //int nASPcmds = 0;

  /* these are the actual parameters after they've been read */
  #include "tpss0.c" /* declarations of variables representing keywords */

  /* Parse command line */
  if (narg>1) { 
      sscanf(argv[1],"%s",sdfname);
    } else {
      printf("[%d/%d] FATAL: sdf not specified\n",MT_TPSS,getpid());
      return;
    }
  printf("[%d/%d] Input: sdf='%s'\n",MT_TPSS,getpid(),sdfname);

  if (narg>2) { 
      sscanf(argv[2],"%d",&max_phase);
    } else {
      printf("[%d/%d] FATAL: max_phase not specified\n",MT_TPSS,getpid());
      return;
    }
  printf("[%d/%d] Input: max_phase=%d\n",MT_TPSS,getpid(),max_phase);

  if (narg>3) { 
      sscanf(argv[3],"%d",&bIgnoreActualTime);
    } else {
      printf("[%d/%d] FATAL: bIgnoreActualTime not specified\n",MT_TPSS,getpid());
      return;
    }
  printf("[%d/%d] Input: bIgnoreActualTime=%d\n",MT_TPSS,getpid(),bIgnoreActualTime);
  if ((bIgnoreActualTime==1) && (max_phase>=5)) {
    printf("[%d/%d] bIgnoreActualTime=1 not allowed for max_phase>=5\n",MT_TPSS,getpid());
    return;
    }
  if (bIgnoreActualTime==-1) bIgnoreActualTime=1; 

  sprintf(mbox,".");
  if (narg>4) sscanf(argv[4],"%s",mbox);
  printf("[%d/%d] Input: mbox='%s'\n",MT_TPSS,getpid(),mbox);

  if (max_phase<1) return;
  printf("[%d/%d] *********************************************************\n",MT_TPSS,getpid()); 
  printf("[%d/%d] *** Phase 1: Reading/Parsing Session Defintion File *****\n",MT_TPSS,getpid());
  printf("[%d/%d] *********************************************************\n",MT_TPSS,getpid());

  /* Open input SDF */
  if (!(fpsdf = fopen(sdfname,"r"))) {
    printf("[%d/%d] FATAL: unable to fopen('%s')\n",MT_TPSS,getpid(),sdfname);
    return;
    }
 
  strcpy(data,"");

  #include "tpss1.c" /* code to read/parse keywords pertaining to PI and project */
  #include "tpss2.c" /* code read/parse keywords pertaining to session */

  /* the next keyword must be OBS_ID */
  strcpy(keyword,"OBS_ID");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:
      nobs++;
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%u",&(obs[nobs].OBS_ID)); 
      printf("...converts to %u\n",obs[nobs].OBS_ID);
      if ( obs[nobs].OBS_ID != nobs ) {
        printf("[%d/%d] FATAL: OBS_ID not incrementing correctly\n",MT_TPSS,getpid());
        return;
        }
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   return; break;
    case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); return; break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
    }

  while (i==TPSS_PL_KEYWORD_MATCH) { /* as long as we see "OBS_ID" */

    printf("[%d/%d] === Parsing Defined Observation #%d ===\n",MT_TPSS,getpid(),nobs);
    LWA_time( &mjd, &mpm ); /* current time, used for error checking */
    #include "tpss3.c" /* code to read/parse keywords pertaining to observation */
    obs[nobs].OBS_STP_N = 0;     
    obs[nobs].OBS_STP_RADEC = 0;
    if (obs[nobs].OBS_MODE==LWA_OM_STEPPED || obs[nobs].OBS_MODE==LWA_OM_STEPPED) {
      #include "tpss4.c" /* code to read/parse keywords pertaining to observation in STEPPED mode */
      }
    #include "tpss5.c" /* code to read/parse optional keywords */
    
    /* the next keyword must be OBS_ID */
    strcpy(keyword,"OBS_ID");
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:
        nobs++;
        printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
        sscanf(data,"%u",&(obs[nobs].OBS_ID)); 
        printf("...converts to %u\n",obs[nobs].OBS_ID);
        if ( obs[nobs].OBS_ID != nobs ) {
          printf("[%d/%d] FATAL: OBS_ID not incrementing correctly\n",MT_TPSS,getpid());
          return;
          }
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:                                                                                            break;
      case TPSS_PL_KEYWORD_MISMATCH:                                                                               break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
      }

    } /* while (i==TPSS_PL_KEYWORD_MATCH) */

  /* Close input SDF */
  fclose(fpsdf);

  /* For STEPPED-mode observations, we need to compute obs[].OBS_DUR */
  for (n=1;n<=nobs;n++) {
    if (obs[n].OBS_MODE==LWA_OM_STEPPED || obs[n].OBS_MODE==LWA_OM_STEPPED_NOOP) { 
      obs[n].OBS_DUR = 0;
      for ( i=1; i<=obs[n].OBS_STP_N; i++ ) { obs[n].OBS_DUR += obs[n].OBS_STP_T[i]; }          
      }  
    }

  printf("[%d/%d] Phase 1: OK\n",MT_TPSS,getpid());

  if (max_phase<2) return;
  printf("[%d/%d] *********************************************************\n",MT_TPSS,getpid()); 
  printf("[%d/%d] *** Phase 2: Second Pass Consistency/Error Checking *****\n",MT_TPSS,getpid());
  printf("[%d/%d] *********************************************************\n",MT_TPSS,getpid());

  if (nobs<1) {
    printf("[%d/%d] FATAL: no observations defined\n",MT_TPSS,getpid());
    return;
    }

  for (n=1;n<=nobs;n++) {
#ifdef USE_ADP
    if ( (obs[n].OBS_DUR==0) && ( ! ( (obs[n].OBS_MODE==LWA_OM_TBF)     || 
                                      (obs[n].OBS_MODE==LWA_OM_DIAG1)     ) ) )  {
      printf("[%d/%d] FATAL: obs[%d].OBS_DUR==0 for a mode other than TBF or DIAG\n",MT_TPSS,getpid(),n);
      return;
      }
#else
    if ( (obs[n].OBS_DUR==0) && ( ! ( (obs[n].OBS_MODE==LWA_OM_TBW)     || 
                                      (obs[n].OBS_MODE==LWA_OM_DIAG1)     ) ) )  {
      printf("[%d/%d] FATAL: obs[%d].OBS_DUR==0 for a mode other than TBW or DIAG\n",MT_TPSS,getpid(),n);
      return;
      }
#endif
    if ( ( (obs[n].OBS_MODE==LWA_OM_TRK_RADEC) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_NOOP )   ) && (obs[n].OBS_RA<0) ) {
      printf("[%d/%d] FATAL: obs[%d].OBS_RA<0 when mode is TRK_RADEC\n",MT_TPSS,getpid(),n);
      return;
      }
#ifdef USE_ADP
    if ( ( obs[n].OBS_MODE==LWA_OM_TBF ) && ( obs[n].OBS_FREQ1<222417950 ) ) {
      printf("[%d/%d] FATAL: obs[%d].OBS_FREQ1 invalid while mode is TBF\n",MT_TPSS,getpid(),n);
      return;
      }
#endif
#ifdef USE_ADP
    if ( ( obs[n].OBS_MODE==LWA_OM_TBN ) && ( obs[n].OBS_FREQ1<65739295 ) ) {
      printf("[%d/%d] FATAL: obs[%d].OBS_FREQ1 invalid while mode is TBN\n",MT_TPSS,getpid(),n);
      return;
      }
#else
    if ( ( obs[n].OBS_MODE==LWA_OM_TBN ) && ( obs[n].OBS_FREQ1<109565492 ) ) {
      printf("[%d/%d] FATAL: obs[%d].OBS_FREQ1 invalid while mode is TBN\n",MT_TPSS,getpid(),n);
      return;
      }
#endif
#ifdef USE_ADP
    if ( ( (obs[n].OBS_MODE==LWA_OM_TRK_RADEC) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_NOOP ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_SOL  ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_JOV  ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_LUN  )   ) && (obs[n].OBS_FREQ1<222417950) ) {
      printf("[%d/%d] FATAL: obs[%d].OBS_FREQ1 invalid while mode is TRK_RADEC, TRK_SOL, TRK_JOV, TRK_LUN, or TBN\n",MT_TPSS,getpid(),n);
      return;
      }
    if ( ( (obs[n].OBS_MODE==LWA_OM_TRK_RADEC) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_NOOP ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_SOL  ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_JOV  ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_LUN  )    ) && (obs[n].OBS_FREQ2 != 0 && obs[n].OBS_FREQ2<222417950) ) {
      printf("[%d/%d] FATAL: obs[%d].OBS_FREQ2 invalid while mode is TRK_RADEC, TRK_SOL, TRK_JOV, or TRK_LUN\n",MT_TPSS,getpid(),n);
      return;
      }
#else
    if ( ( (obs[n].OBS_MODE==LWA_OM_TRK_RADEC) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_NOOP ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_SOL  ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_JOV  ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_LUN  )    ) && (obs[n].OBS_FREQ1<219130984) ) {
      printf("[%d/%d] FATAL: obs[%d].OBS_FREQ1 invalid while mode is TRK_RADEC, TRK_SOL, TRK_JOV, TRK_LUN, or TBN\n",MT_TPSS,getpid(),n);
      return;
      }
    if ( ( (obs[n].OBS_MODE==LWA_OM_TRK_RADEC) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_NOOP ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_SOL  ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_JOV  ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_LUN  )    ) && (obs[n].OBS_FREQ2 != 0 && obs[n].OBS_FREQ2<219130984) ) {
      printf("[%d/%d] FATAL: obs[%d].OBS_FREQ2 invalid while mode is TRK_RADEC, TRK_SOL, TRK_JOV, or TRK_LUN\n",MT_TPSS,getpid(),n);
      return;
      }
#endif
#ifdef USE_ADP
    if ( ( (obs[n].OBS_MODE==LWA_OM_TRK_RADEC) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_NOOP ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_SOL  ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_JOV  ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_LUN  ) ||
           (obs[n].OBS_MODE==LWA_OM_TBF      ) ||
           (obs[n].OBS_MODE==LWA_OM_TBN      )    ) && (obs[n].OBS_BW<=0) ) {
      printf("[%d/%d] FATAL: obs[%d].OBS_BW invalid while mode is TRK_RADEC, TRK_SOL, TRK_JOV, TRK_LUN, or TBN\n",MT_TPSS,getpid(),n);
      return;
      }
    if ( ( (obs[n].OBS_MODE==LWA_OM_TRK_RADEC) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_NOOP ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_SOL  ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_JOV  ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_LUN  )    ) && (obs[n].OBS_BW>7) ) {
      printf("[%d/%d] FATAL: obs[%d].OBS_BW invalid while mode is TRK_RADEC, TRK_SOL, or TRK_JOV\n",MT_TPSS,getpid(),n);
      return;
      }
#else
    if ( ( (obs[n].OBS_MODE==LWA_OM_TRK_RADEC) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_NOOP ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_SOL  ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_JOV  ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_LUN  ) ||
           (obs[n].OBS_MODE==LWA_OM_TBN      )    ) && (obs[n].OBS_BW<=0) ) {
      printf("[%d/%d] FATAL: obs[%d].OBS_BW invalid while mode is TRK_RADEC, TRK_SOL, TRK_JOV, TRK_LUN, or TBN\n",MT_TPSS,getpid(),n);
      return;
      }
    if ( ( (obs[n].OBS_MODE==LWA_OM_TRK_RADEC) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_NOOP ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_SOL  ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_JOV  ) ||
           (obs[n].OBS_MODE==LWA_OM_TRK_LUN  )    ) && (obs[n].OBS_BW>7) ) {
      printf("[%d/%d] FATAL: obs[%d].OBS_BW invalid while mode is TRK_RADEC, TRK_SOL, TRK_JOV, or TRK_LUN\n",MT_TPSS,getpid(),n);
      return;
      }
#endif
#ifdef USE_ADP
    if ( (obs[n].OBS_MODE==LWA_OM_TBF      ) && (SESSION_DRX_BEAM!=1) ) {
      printf("[%d/%d] FATAL: SESSION_DRX_BEAM!=1 when obs[%d].OBS_MODE is TBF\n",MT_TPSS,getpid(),n);
      return;
      }
    if ( (obs[n].OBS_MODE==LWA_OM_TBN      ) && (SESSION_DRX_BEAM>-1) ) {
      printf("[%d/%d] FATAL: SESSION_DRX_BEAM>-1 when obs[%d].OBS_MODE is TBN\n",MT_TPSS,getpid(),n);
      return;
      }
#else
    if ( ( (obs[n].OBS_MODE==LWA_OM_TBW      ) || 
           (obs[n].OBS_MODE==LWA_OM_TBN      )   ) && (SESSION_DRX_BEAM>-1) ) {
      printf("[%d/%d] FATAL: SESSION_DRX_BEAM>-1 when obs[%d].OBS_MODE is TBW or TBN\n",MT_TPSS,getpid(),n);
      return;
      }
#endif
    } /* for n */

#ifdef USE_ADP
  /* check to make sure that session doesn't mix TBF/TBN with other observing modes */
  b_TB_requested = 0;
  b_DRX_requested = 0;
  for (n=1;n<=nobs;n++) {
    if ( (obs[n].OBS_MODE==LWA_OM_TBN         )   ) b_TB_requested = 1;
    if ( (obs[n].OBS_MODE==LWA_OM_TBF         ) || 
         (obs[n].OBS_MODE==LWA_OM_TRK_RADEC   ) ||
         (obs[n].OBS_MODE==LWA_OM_TRK_NOOP    ) || 
         (obs[n].OBS_MODE==LWA_OM_TRK_SOL     ) ||
         (obs[n].OBS_MODE==LWA_OM_TRK_JOV     ) ||
         (obs[n].OBS_MODE==LWA_OM_TRK_LUN     ) ||
         (obs[n].OBS_MODE==LWA_OM_STEPPED     ) ||
         (obs[n].OBS_MODE==LWA_OM_STEPPED_NOOP)   ) b_DRX_requested = 1;
    }
  if ( b_TB_requested && b_DRX_requested ) {
    printf("[%d/%d] FATAL: Sessions cannot mix TBN with other observing modes\n",MT_TPSS,getpid());
    return;
    }
    
  if (b_TB_requested) SESSION_DRX_BEAM=ME_MAX_NDPOUT;
#else
  /* check to make sure that session doesn't mix TBW/TBN with other observing modes */
  b_TB_requested = 0;
  b_DRX_requested = 0;
  for (n=1;n<=nobs;n++) {
    if ( (obs[n].OBS_MODE==LWA_OM_TBW         ) || 
         (obs[n].OBS_MODE==LWA_OM_TBN         )   ) b_TB_requested = 1;
    if ( (obs[n].OBS_MODE==LWA_OM_TRK_RADEC   ) || 
         (obs[n].OBS_MODE==LWA_OM_TRK_NOOP    ) ||
         (obs[n].OBS_MODE==LWA_OM_TRK_SOL     ) ||
         (obs[n].OBS_MODE==LWA_OM_TRK_JOV     ) ||
         (obs[n].OBS_MODE==LWA_OM_TRK_LUN     ) ||
         (obs[n].OBS_MODE==LWA_OM_STEPPED     ) ||
         (obs[n].OBS_MODE==LWA_OM_STEPPED_NOOP)   ) b_DRX_requested = 1;
    }
  if ( b_TB_requested && b_DRX_requested ) {
    printf("[%d/%d] FATAL: Sessions cannot mix TBW/TBN with other observing modes\n",MT_TPSS,getpid());
    return;
    }
    
  if (b_TB_requested) SESSION_DRX_BEAM=ME_MAX_NDPOUT;
#endif
  
  /* check to make sure that session doesn't mix TRK_NOOP with other beamforming modes */
  b_DRX_requested = 0;
  b_NOOP_requested = 0;
  for (n=1;n<=nobs;n++) {
    if ( (obs[n].OBS_MODE==LWA_OM_TRK_NOOP    ) ||
         (obs[n].OBS_MODE==LWA_OM_STEPPED_NOOP)   ) b_NOOP_requested = 1;
    if ( (obs[n].OBS_MODE==LWA_OM_TBF         ) || 
         (obs[n].OBS_MODE==LWA_OM_TRK_RADEC   ) || 
         (obs[n].OBS_MODE==LWA_OM_TRK_SOL     ) ||
         (obs[n].OBS_MODE==LWA_OM_TRK_JOV     ) ||
         (obs[n].OBS_MODE==LWA_OM_STEPPED     )   ) b_DRX_requested = 1;
    }
  if ( b_NOOP_requested && b_DRX_requested ) {
    printf("[%d/%d] FATAL: Sessions cannot mix NOOP with other beamforming modes\n",MT_TPSS,getpid());
    return;
    }
    
#ifdef USE_ADP
   /* if mode = TBF, OBS_DUR needs to be computed */
  for (n=1;n<=nobs;n++) {
    if (obs[n].OBS_MODE==LWA_OM_TBF) {
      t_tbw = ( obs[n].OBS_TBF_SAMPLES / 196000 ) +1; /* convert samples to ms */
      t_tbw *= TBF_INVERSE_DUTY_CYCLE;                /* account for read-out time after triggering (~100:1) */
      if( obs[n].OBS_FREQ2 != 0 ) {
        t_tbw *= 2;                                   /* account for the second tuning, if used */
        }
      t_tbw += 5000;                                  /* account for the buffer fill lag */
      obs[n].OBS_DUR = t_tbw;
      printf("[%d/%d] Computed obs[%d].OBS_DUR = %ld [ms] for this TBF observation\n",MT_TPSS,getpid(),n,obs[n].OBS_DUR);
      }
    }
#else
  /* if mode = TBW, OBS_DUR needs to be computed */
  for (n=1;n<=nobs;n++) {
    if (obs[n].OBS_MODE==LWA_OM_TBW) {
      t_tbw = ( obs[n].OBS_TBW_SAMPLES / 196000 ) +1; /* convert samples to ms */
      t_tbw *= TBW_INVERSE_DUTY_CYCLE;         /* account for read-out time after triggering (~500:1) */
      obs[n].OBS_DUR = t_tbw; /* convert samples to ms */
      printf("[%d/%d] Computed obs[%d].OBS_DUR = %ld [ms] for this TBW observation\n",MT_TPSS,getpid(),n,obs[n].OBS_DUR);
      }
    }
#endif

  /* check to make sure no observation end times overlap with subsequent observation start times */
  for (n=1;n<nobs;n++) {
    LWA_time2tv( &t0, obs[n  ].OBS_START_MJD, obs[n  ].OBS_START_MPM ); /* t0 is now start time as a timeval */
    LWA_timeadd( &t0, obs[n  ].OBS_DUR );                              /* t0 is now end time as a timeval */
    LWA_time2tv( &t1, obs[n+1].OBS_START_MJD, obs[n+1].OBS_START_MPM ); /* t1 is now start time as a timeval */
    if (LWA_timediff(t1,t0)<0) {
      printf("[%d/%d] FATAL: Observation %d overlaps Observation %d\n",MT_TPSS,getpid(),n,n+1);
      return;
      } 
    }

  //onthefly checking to add
  //make sure strcpy(parm,data)'s don't overflow parm string because data is too long

  /* Calculating ASP setup times for each observation */
  //printf("%d %d %d\n",obs[1].OBS_FEE[0][0],obs[1].OBS_FEE[1][0],obs[1].OBS_FEE[2][0]);
  for (i=1;i<=nobs;i++) {
    obs[i].ASP_setup_time = 0;
    for (n=1;n<=LWA_MAX_NSTD;n++) { 
      for (p=0;p<=1;p++) { 
        if (obs[i].OBS_FEE[n][p]!=-1) { obs[i].ASP_setup_time += LWA_ASP_OP_TIME_MS; }
        }
      }
    for (n=1;n<=LWA_MAX_NSTD;n++) { if (obs[i].OBS_ASP_FLT[n]!=-1) obs[i].ASP_setup_time += LWA_ASP_OP_TIME_MS; }
    for (n=1;n<=LWA_MAX_NSTD;n++) { if (obs[i].OBS_ASP_AT1[n]!=-1) obs[i].ASP_setup_time += LWA_ASP_OP_TIME_MS; }
    for (n=1;n<=LWA_MAX_NSTD;n++) { if (obs[i].OBS_ASP_AT2[n]!=-1) obs[i].ASP_setup_time += LWA_ASP_OP_TIME_MS; }
    for (n=1;n<=LWA_MAX_NSTD;n++) { if (obs[i].OBS_ASP_ATS[n]!=-1) obs[i].ASP_setup_time += LWA_ASP_OP_TIME_MS; }
    printf("[%d/%d] Obs#%d ASP setup time is %ld [ms]\n",MT_TPSS,getpid(),i,obs[i].ASP_setup_time);
    } /* for i */

  /* t0 = (start time of first observation) - (obs#1 ASP setup time) - (DP+DR setup time) */
  LWA_time2tv( &t0, obs[1].OBS_START_MJD, obs[1].OBS_START_MPM ); /* t0 is now start time as a timeval */
  LWA_timeadd( &t0,-obs[1].ASP_setup_time-LWA_SESS_DRDP_INIT_TIME_MS );
  printf("[%d/%d] Time allocated for session DR+DP setup is %d [ms]\n",MT_TPSS,getpid(),LWA_SESS_DRDP_INIT_TIME_MS);
  printf("[%d/%d] Time allocated for session ASP   setup is %ld [ms]\n",MT_TPSS,getpid(),obs[1].ASP_setup_time);
  LWA_timeval( &t0, &SESSION_START_MJD, &SESSION_START_MPM ); /* remember this (session start time */
  printf("[%d/%d] Session start will be MJD=%ld MPM=%ld\n",MT_TPSS,getpid(),SESSION_START_MJD,SESSION_START_MPM);

  /* t1 = (start time of last observation) + (duration of last observation) + (guard time) */  
  LWA_time2tv( &t1, obs[nobs].OBS_START_MJD, obs[nobs].OBS_START_MPM );  
  LWA_timeadd( &t1, obs[nobs].OBS_DUR );   
  LWA_timeadd( &t1, LWA_SESS_GUARD_TIME_MS );   
  printf("[%d/%d] Time allocated for session closeout is %d [ms]\n",MT_TPSS,getpid(),LWA_SESS_GUARD_TIME_MS);  

  /* calculating overall duration of session = t1 - t0 */
  d = LWA_timediff( t1, t0 );                                           /* d is now t1-t0 [ms] */
  printf("[%d/%d] Total duration of session is %ld ms = %f min = %f h\n",MT_TPSS,getpid(),
         d,(((float)d/1000))/60.0,(((float)d/1000))/3600.0);

  printf("[%d/%d] Phase 2: OK\n",MT_TPSS,getpid());

  if (max_phase<3) return;
  printf("[%d/%d] *********************************************************\n",MT_TPSS,getpid()); 
  printf("[%d/%d] *** Phase 3: Checking Resource Availability *************\n",MT_TPSS,getpid());
  printf("[%d/%d] *********************************************************\n",MT_TPSS,getpid());

  /* check defined session against mess.dat */
  if (SESSION_DRX_BEAM>0) {  /* a particular DRX output has been requested */
     
     err = LWA_dpoavail( SESSION_DRX_BEAM, t0, d, mbox, msg );
     printf("[%d/%d] LWA_dpoavail says: %s\n",MT_TPSS,getpid(),msg); 
     if (err<=0) {
       printf("[%d/%d] FATAL: SESSION_DRX_BEAM==%hd cannot be accommodated\n",MT_TPSS,getpid(),SESSION_DRX_BEAM);
       return; 
       }

    } else {                 /* No particular DRX output has been requested  */
      printf("[%d/%d] FATAL: SESSION_DRX_BEAM not specified\n",MT_TPSS,getpid());
      return;

      //if (b_DRX_requested) { /* and DRX-dependent mode(s) have been requested */
      //  /* check each beam output, select best */
      //  err_max = -9999;
      //  i_max = 0;
      //  for (i=1;i<=4;i++) {
      //    err = LWA_dpoavail( i, t0, d, mbox, msg );
      //    printf("[%d/%d] For DP output %d of 4, LWA_dpoavail says: %s\n",MT_TPSS,getpid(),i,msg);
      //    if (err>err_max) { err_max = err; i_max = i; }
      //    }
      //  if (err_max<1) {
      //    printf("[%d/%d] FATAL: Unable to schedule\n",MT_TPSS,getpid(),i,msg);
      //    return;
      //    }
      //  SESSION_DRX_BEAM = i_max;
      //  printf("[%d/%d] Selected DP output %hd of 4\n",MT_TPSS,getpid(),SESSION_DRX_BEAM);
      //  } /* if (b_DRX_requested) */

    }

  printf("[%d/%d] Phase 3: OK\n",MT_TPSS,getpid());

  if (max_phase<4) return;
  printf("[%d/%d] *************************************************\n",MT_TPSS,getpid()); 
  printf("[%d/%d] *** Phase 4: Generate Full-SDF, SSF, and OSFs ***\n",MT_TPSS,getpid());
  printf("[%d/%d] *************************************************\n",MT_TPSS,getpid());

  /*** OSF file ********************************************/

  sprintf(sdfname,"%s_%04u.txt",PROJECT_ID,SESSION_ID);
  if (!(fp = fopen(sdfname,"w"))) {
    printf("[%d/%d] Unable to create SDF '%s'\n",MT_TPSS,getpid(),sdfname);
    return;
    }

  fprintf(fp,"PI_ID %s\n",PI_ID);
  fprintf(fp,"PI_NAME %s\n",PI_NAME);
  fprintf(fp,"\n");

  fprintf(fp,"PROJECT_ID %s\n",PROJECT_ID);
  fprintf(fp,"PROJECT_TITLE %s\n",PROJECT_TITLE);
  fprintf(fp,"PROJECT_REMPI %s\n",PROJECT_REMPI);
  fprintf(fp,"PROJECT_REMPO %s\n",PROJECT_REMPO);
  fprintf(fp,"\n");

  fprintf(fp,"SESSION_ID %u\n",SESSION_ID);
  fprintf(fp,"SESSION_TITLE %s\n",SESSION_TITLE);
  fprintf(fp,"SESSION_REMPI %s\n",SESSION_REMPI);
  fprintf(fp,"SESSION_REMPO %s\n",SESSION_REMPO);
  fprintf(fp,"\n");

  fprintf(fp,"SESSION_CRA %d\n",SESSION_CRA);
  fprintf(fp,"SESSION_DRX_BEAM %hd\n",SESSION_DRX_BEAM);
  fprintf(fp,"SESSION_SPC '%s'\n",SESSION_SPC);
  fprintf(fp,"\n");

  fprintf(fp,"SESSION_MRP_ASP %d\n",SESSION_MRP_ASP);
  fprintf(fp,"SESSION_MRP_DP_ %d\n",SESSION_MRP_DP_);
  fprintf(fp,"SESSION_MRP_DR1 %d\n",SESSION_MRP_DR1);
  fprintf(fp,"SESSION_MRP_DR2 %d\n",SESSION_MRP_DR2);
  fprintf(fp,"SESSION_MRP_DR3 %d\n",SESSION_MRP_DR3);
  fprintf(fp,"SESSION_MRP_DR4 %d\n",SESSION_MRP_DR4);
  fprintf(fp,"SESSION_MRP_DR5 %d\n",SESSION_MRP_DR5);
  fprintf(fp,"SESSION_MRP_SHL %d\n",SESSION_MRP_SHL);
  fprintf(fp,"SESSION_MRP_MCS %d\n",SESSION_MRP_MCS);
  
  fprintf(fp,"SESSION_MUP_ASP %d\n",SESSION_MUP_ASP);
  fprintf(fp,"SESSION_MUP_DP_ %d\n",SESSION_MUP_DP_);
  fprintf(fp,"SESSION_MUP_DR1 %d\n",SESSION_MUP_DR1);
  fprintf(fp,"SESSION_MUP_DR2 %d\n",SESSION_MUP_DR2);
  fprintf(fp,"SESSION_MUP_DR3 %d\n",SESSION_MUP_DR3);
  fprintf(fp,"SESSION_MUP_DR4 %d\n",SESSION_MUP_DR4);
  fprintf(fp,"SESSION_MUP_DR5 %d\n",SESSION_MUP_DR5);
  fprintf(fp,"SESSION_MUP_SHL %d\n",SESSION_MUP_SHL);
  fprintf(fp,"SESSION_MUP_MCS %d\n",SESSION_MUP_MCS);

  fprintf(fp,"SESSION_LOG_SCH %d\n",SESSION_LOG_SCH);
  fprintf(fp,"SESSION_LOG_EXE %d\n",SESSION_LOG_EXE);
  fprintf(fp,"SESSION_INC_SMIB %d\n",SESSION_INC_SMIB);
  fprintf(fp,"SESSION_INC_DES  %d\n",SESSION_INC_DES);
  fprintf(fp,"\n");

  for (n=1;n<=nobs;n++) {

    fprintf(fp,"OBS_ID %u\n",    obs[n].OBS_ID);
    fprintf(fp,"OBS_TITLE %s\n", obs[n].OBS_TITLE);
    fprintf(fp,"OBS_TARGET %s\n",obs[n].OBS_TARGET);
    fprintf(fp,"OBS_REMPI %s\n", obs[n].OBS_REMPI);
    fprintf(fp,"OBS_REMPO %s\n", obs[n].OBS_REMPO);
   
    fprintf(fp,"OBS_START_MJD %ld\n",obs[n].OBS_START_MJD);
    fprintf(fp,"OBS_START_MPM %ld\n",obs[n].OBS_START_MPM);
    fprintf(fp,"OBS_START %s\n", obs[n].OBS_START);
    fprintf(fp,"OBS_DUR %ld\n",obs[n].OBS_DUR);
    fprintf(fp,"OBS_DUR+ %s\n", obs[n].OBS_DURp);
    LWA_saymode(obs[n].OBS_MODE,msg); fprintf(fp,"OBS_MODE %s\n",msg);
    fprintf(fp,"OBS_BDM '%s'\n",obs[n].OBS_BDM);
    fprintf(fp,"OBS_RA %f\n", obs[n].OBS_RA);
    fprintf(fp,"OBS_DEC %f\n", obs[n].OBS_DEC);
    fprintf(fp,"OBS_B %d\n", obs[n].OBS_B);

    fprintf(fp,"OBS_FREQ1 %ld\n",obs[n].OBS_FREQ1);
    fprintf(fp,"OBS_FREQ1+ %s\n",obs[n].OBS_FREQ1p);
    fprintf(fp,"OBS_FREQ2 %ld\n",obs[n].OBS_FREQ2);
    fprintf(fp,"OBS_FREQ2+ %s\n",obs[n].OBS_FREQ2p);
    fprintf(fp,"OBS_BW %d\n",    obs[n].OBS_BW);
    fprintf(fp,"OBS_BW+ %s\n",   obs[n].OBS_BWp);

    if (obs[n].OBS_MODE==LWA_OM_STEPPED || obs[n].OBS_MODE==LWA_OM_STEPPED_NOOP) {

      fprintf(fp,"OBS_STP_N %ld\n",obs[n].OBS_STP_N);
      fprintf(fp,"OBS_STP_RADEC %d\n",obs[n].OBS_STP_RADEC);

      for (m=1;m<=obs[n].OBS_STP_N;m++) {

        fprintf(fp,"\n");
        fprintf(fp,"OBS_STP_C1[%d] %f\n",m,obs[n].OBS_STP_C1[m]);
        fprintf(fp,"OBS_STP_C2[%d] %f\n",m,obs[n].OBS_STP_C2[m]);
        fprintf(fp,"OBS_STP_T[%d] %ld\n",m,obs[n].OBS_STP_T[m]);
        fprintf(fp,"OBS_STP_FREQ1[%d] %ld\n",m,obs[n].OBS_STP_FREQ1[m]);
        fprintf(fp,"OBS_STP_FREQ1+[%d] %s\n",m,obs[n].OBS_STP_FREQ1p[m]);
        fprintf(fp,"OBS_STP_FREQ2[%d] %ld\n",m,obs[n].OBS_STP_FREQ2[m]);
        fprintf(fp,"OBS_STP_FREQ2+[%d] %s\n",m,obs[n].OBS_STP_FREQ2p[m]);
        fprintf(fp,"OBS_STP_B[%d] %d\n",m,obs[n].OBS_STP_B[m]);

        if (obs[n].OBS_STP_B[m]==LWA_BT_SPEC_DELAYS_GAINS) {

          fprintf(fp,"\n");   
          for (p=1;p<=2*LWA_MAX_NSTD;p++) {
            fprintf(fp,"OBS_BEAM_DELAY[%d][%d] %hu\n",m,p,obs[n].OBS_BEAM_DELAY[m][p]);
            }
  
          fprintf(fp,"\n");   
          for (p=1;p<=2*LWA_MAX_NSTD;p++) {
            for (q=1;q<=2;q++) {
              for (r=1;r<=2;r++) {
                fprintf(fp,"OBS_BEAM_GAIN[%d][%d][%d][%d] %hd\n",m,p,q,r,obs[n].OBS_BEAM_GAIN[m][p][q][r]);
                }
              }
            }

          } /* if (obs[n].OBS_STP_B[m]==LWA_BT_SPEC_DELAYS_GAINS) */

        } /* for m */

      } /* if (obs[n].OBS_MODE==LWA_OM_STEPPED || obs[n].OBS_MODE==LWA_OM_STEPPED_NOOP) */

    fprintf(fp,"\n");
    for (m=1;m<=LWA_MAX_NSTD;m++) {
      for (p=1;p<=2;p++) {
        fprintf(fp,"OBS_FEE[%d][%d] %d\n",m,p,obs[n].OBS_FEE[m][p]);
        }
      }

    fprintf(fp,"\n");
    for (m=1;m<=LWA_MAX_NSTD;m++) {
      fprintf(fp,"OBS_ASP_FLT[%d] %d\n",m,obs[n].OBS_ASP_FLT[m]);
      }

    fprintf(fp,"\n");
    for (m=1;m<=LWA_MAX_NSTD;m++) {
      fprintf(fp,"OBS_ASP_AT1[%d] %d\n",m,obs[n].OBS_ASP_AT1[m]);
      }

    fprintf(fp,"\n");
    for (m=1;m<=LWA_MAX_NSTD;m++) {
      fprintf(fp,"OBS_ASP_AT2[%d] %d\n",m,obs[n].OBS_ASP_AT2[m]);
      }

    fprintf(fp,"\n");
    for (m=1;m<=LWA_MAX_NSTD;m++) {
      fprintf(fp,"OBS_ASP_ATS[%d] %d\n",m,obs[n].OBS_ASP_ATS[m]);
      }

    fprintf(fp,"\n");
#ifdef USE_ADP
    fprintf(fp,"OBS_TBF_SAMPLES %ld\n",obs[n].OBS_TBF_SAMPLES);
    fprintf(fp,"OBS_TBF_GAIN %d\n",obs[n].OBS_TBF_GAIN);
#else
    fprintf(fp,"OBS_TBW_BITS %d\n",obs[n].OBS_TBW_BITS);
    fprintf(fp,"OBS_TBW_SAMPLES %ld\n",obs[n].OBS_TBW_SAMPLES);
#endif
    fprintf(fp,"OBS_TBN_GAIN %d\n",obs[n].OBS_TBN_GAIN);
    fprintf(fp,"OBS_DRX_GAIN %d\n",obs[n].OBS_DRX_GAIN);

    } /* for n */

  fclose(fp);

  printf("[%d/%d] SDF '%s' successfully written\n",MT_TPSS,getpid(),sdfname);

  /*** SSF file ********************************************/

  ssf.FORMAT_VERSION = TPSS_FORMAT_VERSION;
  sprintf(ssf.PROJECT_ID,"%s",PROJECT_ID);
  ssf.SESSION_ID = SESSION_ID;
  ssf.SESSION_CRA = SESSION_CRA;
  ssf.SESSION_DRX_BEAM = SESSION_DRX_BEAM;
  sprintf(ssf.SESSION_SPC,"%s",SESSION_SPC);
  ssf.SESSION_START_MJD = SESSION_START_MJD;
  ssf.SESSION_START_MPM = SESSION_START_MPM;
  ssf.SESSION_DUR = d;
  ssf.SESSION_NOBS = nobs;
  ssf.SESSION_MRP_ASP = SESSION_MRP_ASP;
  ssf.SESSION_MRP_DP_ = SESSION_MRP_DP_;
  ssf.SESSION_MRP_DR1 = SESSION_MRP_DR1;
  ssf.SESSION_MRP_DR2 = SESSION_MRP_DR2;
  ssf.SESSION_MRP_DR3 = SESSION_MRP_DR3;
  ssf.SESSION_MRP_DR4 = SESSION_MRP_DR4;
  ssf.SESSION_MRP_DR5 = SESSION_MRP_DR5;
  ssf.SESSION_MRP_SHL = SESSION_MRP_SHL;
  ssf.SESSION_MRP_MCS = SESSION_MRP_MCS;
  ssf.SESSION_MUP_ASP = SESSION_MUP_ASP;
  ssf.SESSION_MUP_DP_ = SESSION_MUP_DP_;
  ssf.SESSION_MUP_DR1 = SESSION_MUP_DR1;
  ssf.SESSION_MUP_DR2 = SESSION_MUP_DR2;
  ssf.SESSION_MUP_DR3 = SESSION_MUP_DR3;
  ssf.SESSION_MUP_DR4 = SESSION_MUP_DR4;
  ssf.SESSION_MUP_DR5 = SESSION_MUP_DR5;
  ssf.SESSION_MUP_SHL = SESSION_MUP_SHL;
  ssf.SESSION_MUP_MCS = SESSION_MUP_MCS;
  ssf.SESSION_LOG_SCH = SESSION_LOG_SCH;
  ssf.SESSION_LOG_EXE = SESSION_LOG_EXE;
  ssf.SESSION_INC_SMIB =SESSION_INC_SMIB;
  ssf.SESSION_INC_DES  =SESSION_INC_DES;

  sprintf(ssfname,"%s_%04u.ses",PROJECT_ID,SESSION_ID);
  if (!(fp = fopen(ssfname,"wb"))) {
    printf("[%d/%d] Unable to create SSF '%s'\n",MT_TPSS,getpid(),ssfname);
    return;
    }
  fwrite(&ssf,sizeof(struct ssf_struct),1,fp);
  fclose(fp);

  printf("[%d/%d] SSF '%s' successfully written\n",MT_TPSS,getpid(),ssfname);

  /*** OSF file ********************************************/

  for (n=1;n<=nobs;n++) {
 
    sprintf(osfname,"%s_%04d_%04u.obs",PROJECT_ID,SESSION_ID,n);
    if ((fp = fopen(osfname,"wb"))==NULL) {
      printf("[%d/%d] Unable to create OSF '%s'\n",MT_TPSS,getpid(),osfname);
      return;
      }

    osf.FORMAT_VERSION = TPSS_FORMAT_VERSION;   //printf("osf.FORMAT_VERSION  = %hu\n",osf.FORMAT_VERSION);
    sprintf(osf.PROJECT_ID,"%8s",PROJECT_ID);   //printf("osf.PROJECT_ID = '%s'\n",osf.PROJECT_ID);
    osf.SESSION_ID = SESSION_ID;                //printf("osf.SESSION_ID = %u\n",osf.SESSION_ID);
    osf.SESSION_DRX_BEAM = SESSION_DRX_BEAM;    //printf("osf.SESSION_DRX_BEAM = %hd\n",osf.SESSION_DRX_BEAM);
    sprintf(osf.SESSION_SPC,"%s",SESSION_SPC);    
    osf.OBS_ID = obs[n].OBS_ID;                 //printf("osf.OBS_ID = %u\n",osf.OBS_ID);
    osf.OBS_START_MJD = obs[n].OBS_START_MJD;
    osf.OBS_START_MPM = obs[n].OBS_START_MPM;
    osf.OBS_DUR       = obs[n].OBS_DUR;
    osf.OBS_MODE      = obs[n].OBS_MODE;
    sprintf(osf.OBS_BDM,"%s",obs[n].OBS_BDM);
    osf.OBS_RA        = obs[n].OBS_RA;
    osf.OBS_DEC       = obs[n].OBS_DEC;
    osf.OBS_B         = obs[n].OBS_B;
    osf.OBS_FREQ1     = obs[n].OBS_FREQ1;
    osf.OBS_FREQ2     = obs[n].OBS_FREQ2;
    osf.OBS_BW        = obs[n].OBS_BW;
    osf.OBS_STP_N     = obs[n].OBS_STP_N;
    osf.OBS_STP_RADEC = obs[n].OBS_STP_RADEC;

    fwrite(&osf,sizeof(struct osf_struct),1,fp);    

    for (m=1;m<=obs[n].OBS_STP_N;m++) {

      osfs.OBS_STP_C1    = obs[n].OBS_STP_C1[m];
      osfs.OBS_STP_C2    = obs[n].OBS_STP_C2[m];
      osfs.OBS_STP_T     = obs[n].OBS_STP_T[m];
      osfs.OBS_STP_FREQ1 = obs[n].OBS_STP_FREQ1[m];
      osfs.OBS_STP_FREQ2 = obs[n].OBS_STP_FREQ2[m];
      osfs.OBS_STP_B     = obs[n].OBS_STP_B[m];

      fwrite(&osfs,sizeof(struct osfs_struct),1,fp);

      if (obs[n].OBS_STP_B[m]==LWA_BT_SPEC_DELAYS_GAINS) {

        for (p=1;p<=2*LWA_MAX_NSTD;p++) {
          beam.OBS_BEAM_DELAY[p-1] = obs[n].OBS_BEAM_DELAY[m][p]; 
          }

        for (p=1;p<=LWA_MAX_NSTD;p++) {
          for (q=1;q<=2;q++) {
            for (r=1;r<=2;r++) {
              beam.OBS_BEAM_GAIN[p-1][q-1][r-1] = obs[n].OBS_BEAM_GAIN[m][p][q][r]; 
              }
            }
          }

        fwrite(&beam,sizeof(struct beam_struct),1,fp);

        } /* if (obs[n].OBS_STP_B[m]==LWA_BT_SPEC_DELAYS_GAINS) */

      u4 = 4294967294; fwrite(&u4, sizeof(u4),1,fp); /* = $2^{32}-2$. */

      } /* for m */

    for (m=1;m<=LWA_MAX_NSTD;m++) {
      for (p=1;p<=2;p++) {
        osf2.OBS_FEE[m-1][p-1] = obs[n].OBS_FEE[m][p]; 
        }
      }
    for (m=1;m<=LWA_MAX_NSTD;m++) { osf2.OBS_ASP_FLT[m-1] = obs[n].OBS_ASP_FLT[m]; }
    for (m=1;m<=LWA_MAX_NSTD;m++) { osf2.OBS_ASP_AT1[m-1] = obs[n].OBS_ASP_AT1[m]; }
    for (m=1;m<=LWA_MAX_NSTD;m++) { osf2.OBS_ASP_AT2[m-1] = obs[n].OBS_ASP_AT2[m]; }
    for (m=1;m<=LWA_MAX_NSTD;m++) { osf2.OBS_ASP_ATS[m-1] = obs[n].OBS_ASP_ATS[m]; }
#ifdef USE_ADP
    osf2.OBS_TBF_SAMPLES     = obs[n].OBS_TBF_SAMPLES;
    osf2.OBS_TBF_GAIN        = obs[n].OBS_TBF_GAIN;
#else
    osf2.OBS_TBW_BITS        = obs[n].OBS_TBW_BITS; 
    osf2.OBS_TBW_SAMPLES     = obs[n].OBS_TBW_SAMPLES;
#endif
    osf2.OBS_TBN_GAIN        = obs[n].OBS_TBN_GAIN;
    osf2.OBS_DRX_GAIN        = obs[n].OBS_DRX_GAIN;

    fwrite(&osf2,sizeof(struct osf2_struct),1,fp);

    u4 = 4294967295; fwrite(&u4, sizeof(u4),1,fp); /* = $2^{32}-1$. */

    fclose(fp);

    printf("[%d/%d] OSF '%s' successfully written\n",MT_TPSS,getpid(),osfname);

    } /* for n */

  printf("[%d/%d] Phase 4: OK\n",MT_TPSS,getpid());

  if (max_phase<5) return;
  printf("[%d/%d] ******************************************************\n",MT_TPSS,getpid()); 
  printf("[%d/%d] *** Phase 5: Push to tp outbox and notify MCS/Exec ***\n",MT_TPSS,getpid());
  printf("[%d/%d] ******************************************************\n",MT_TPSS,getpid());

  /* Checking to see if this session already exists in manifest.txt */
  /* check if manifest.dat exists as expected */
  sprintf(data,"%s/manifest.dat",mbox);
  if (!(fp=fopen(data,"r"))) {
    
      /* If it can't be opened, then no need to check for redundant observations */

    } else {

      /* check if this session already appears in manifest */
      sprintf(data,"%s_%04u",PROJECT_ID,SESSION_ID);
      while (!feof(fp)) {
        fscanf(fp,"%s",line); //printf("<%s> <%s>\n",data,line);
        if (!strncmp(line,data,strlen(data))) {
          printf("[%d/%d] FATAL: Session '%s' already appears in manifest\n",MT_TPSS,getpid(),data);
          return;
          }
        } 

      fclose(fp);

    }

  /* Checking to see if this session already exists in mess.dat */
  /* check if mess.dat exists as expected */
  sprintf(data,"%s/mess.dat",mbox);
  if (!(fp=fopen(data,"r"))) {
    
      /* If it can't be opened, then no need to check for redundant observations */

    } else {

      /* check if this session already appears */
      while (!feof(fp)) {
        fgets(line,256,fp); 
        sprintf(data,"%04u",SESSION_ID);
        //printf("<%s> <%s> <%s>\n",line,PROJECT_ID,data);
        if ( (strstr(line,PROJECT_ID)!=NULL) && (strstr(line,data)!=NULL) ) {
          printf("[%d/%d] FATAL: Session already appears in mess.dat\n",MT_TPSS,getpid());
          return;
          }
        } 

      fclose(fp);

    }

  if (strncmp(mbox,".",1)!=0) { /* don't move files if dest dir = source dir */
    sprintf(cmd,"mv %s %s/.",sdfname,mbox); system(cmd);
    sprintf(cmd,"mv %s %s/.",ssfname,mbox); system(cmd);
    for (n=1;n<=nobs;n++) {
      sprintf(cmd,"mv %s_%04u_%04d.obs %s/.",PROJECT_ID,SESSION_ID,n,mbox); system(cmd);
      }
    }
  printf("[%d/%d] Moved SDF, SSF, and OSFs to directory '%s'\n",MT_TPSS,getpid(),mbox);   

  strcpy(data,"");

  //sprintf(data,"%s%s",data,sdfname);
  //sprintf(data,"%s\n%s",data,ssfname);

  sprintf(data,"%s%s %4u %4d %s",data,PROJECT_ID,SESSION_ID,-1,sdfname);
  sprintf(data,"%s\n%s %4u %4d %s",data,PROJECT_ID,SESSION_ID,0,ssfname);
  for (n=1;n<=nobs;n++) sprintf(data,"%s\n%s %4u %4d %s_%04d_%04d.obs",data,PROJECT_ID,SESSION_ID,n,PROJECT_ID,SESSION_ID,n);

  //printf("<%s>\n",data);
  sprintf(cmd,"echo \"%s\" > manifest_add.dat",data); system(cmd);
  sprintf(cmd,"cat manifest_add.dat >> %s/manifest.dat",mbox); system(cmd);
  printf("[%d/%d] Updated %s/manifest.dat\n",MT_TPSS,getpid(),mbox);

  printf("[%d/%d] Phase 5: OK\n",MT_TPSS,getpid());

  return 0;
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// tpss.c: J. Dowell, UNM 2022 Sep 30
//   .1 Added support for TRK_LUN
// tpcc.s: J. Dowell, UNM, 2020 Apr 7
//   .1 Changed TRK_NULL to TRK_NOOP and added a STEPPED_NOOP mode
// tpss.c: J. Dowell, UNM, 2020 Apr 6
//   .1 Added support for TRK_NULL which is like TRK_RADEC but doesn't record any
//      data
// tpss.c: J. Dowell, UNM, 2019 Dec 6
//   .1 Fixed a bug where the frequency limits in tpss4.c did not match those 
//      in tpss3.c
// tpss.c: J. Dowell, UNM, 2019 Dec 4
//   .1 Fixed a typo in the ADP TBN upper tuning limit and updated the ADP DRX 
//      tuning limits for the new 19.8 MHz mode
// tpss.c: J. Dowell, UNM, 2019 Feb 12
//   .1 Made the frequency validation consistent across all of tpss
// tpss.c: J. Dowell, UNM, 2019 Jan 28
//   .1 Allow OBS_ASP_FLT to go up to 5 for ADP-based systems
// tpss.c: J. Dowell, UNM, 2019 Jan 25
//   .1 Updated the TBF time calculation to take into account whether or not two
//      tunings are being used
// tpss.c: J. Dowell, UNM, 2018 Dec 22
//   .1 Tweaked the TBW duration calculation since DP is now faster
// tpss.c: J. Dowell, UNM, 2018 Feb 13
//   .1 Added a check to force TBF to run on beam one at ADP-based stations
//   .2 Increased the DRX/TBF bandwidth code limit to 7
// tpss.c: J. Dowell, UNM, 2018 Jan 29
//   .1 Cleaned up a few compiler warnings
// tpss.c J. Dowell, UNM, 2017 Sep 8
//   .1 Changed the tuning and bandwidth limits to match what ADP can currently do
// tpss.c J. Dowell, UNM, 2017 Apr 20
//   .1 Changed the duration of TBF recordings to reflect what ADP can do
// tpss.c J. Dowell, UNM, 2016 Aug 25
//   .1 Added support for single tuning "half beams" where tuning 2 is not set
// tpss.c: S.W. Ellingson, Virginia Tech, 2014 Mar 10
//   .1 Added BDM command; added OBS_BDM keyword
// tpss.c: S.W. Ellingson, Virginia Tech, 2013 Jan 28
//   .1 Redimensioned mode_string[] to 256 (from 10) to fix overrun problem
// tpss.c: S.W. Ellingson, Virginia Tech, 2013 Jan 23
//   .1 Changed MAX_BEAM_DELAY 
// tpss.c: S.W. Ellingson, Virginia Tech, 2012 Oct 07
//   .1: Upgrading to accomodate TRK_SOL and TRK_JOV (no actual changes were required)
// tpss.c: S.W. Ellingson, Virginia Tech, 2012 Sep 29
//   .1: Upgrading to accomodate STEPPED mode
// tpss.c: S.W. Ellingson, Virginia Tech, 2012 Apr 12
//   .1: Rejects DRX-mode sessions when SESSION_DRX_BEAM is not specified in SDF
//   .2: Rejects sessions which already appear in manifest or which conflict in time
// tpss.c: S.W. Ellingson, Virginia Tech, 2012 Mar 02
//   .1: Increased size of "data[]" and "cmd[]" strings -- now able to handle 120+ observations/session
// tpss.c: S.W. Ellingson, Virginia Tech, 2012 Feb 16
//   .1: Added SESSION_SPC keyword
// tpss.c: S.W. Ellingson, Virginia Tech, 2011 December 16
//   .1: Fixed problem that observation-specific ASP settings were not set up for every observation
//       Computing advance time needed to deal with session-specific ASP commands
//       Session start time in SSF now reflects session DP+DR start times and obs#1 ASP start time 
// tpss.c: S.W. Ellingson, Virginia Tech, 2011 March 10
//   .1: Changed manifest.dat format
//   .2: Changing SSF to structure variable 
// tpss.c: S.W. Ellingson, Virginia Tech, 2011 March 08
//   .1: Various bug fixes
// tpss.c: S.W. Ellingson, Virginia Tech, 2011 March 01
//   .1: Updated FORMAT_VERSION to 3
// tpss.c: S.W. Ellingson, Virginia Tech, 2011 Feb 27
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
