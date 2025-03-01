// me_inproc.c: J. Dowell, UNM, 2015 Aug 31
// ---
// COMPILE: gcc -o me_inproc me_inproc.c -I../common -lm
// ---
// COMMAND LINE: me_inproc 
// ---
// REQUIRES: 
//   me.h
// ---
// Handles inprocessing of sessions for me_exec
//   Lurks in wait for a "sinbox/*.inp" file to work on
//   Output is a script "sinbox/*.cs" that me_exec uses to conduct observation
//   "sinbox/*.ipl" is a log file documenting what happens during inprocessing 
//   When done, "sinbox/*.inp" is deleted, signaling me_exec that we're done
// Notes:
//   If the mode of the first observation is "DIAG1", the only commands given will be
//     the initial "null" command and "ESN", directing me_exec to conclude the session
//     as "successful"
//   If the first "DIAG1" mode observation occurs in observation 2 or later, then 
//     the session setup and prior observations will happen, and then the "ESN" will
//     be commanded 
// See end of this file for history.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>        /* needed for me_getaltaz.c + others */
#include <dirent.h>      /* this is listing files in a directory */

#include "mcs.h"
#include "me.h"
#include "me_astro.h"
/* NOTE: the above also pulls in several additional files "ephem_*" */

#define ME_INPROC_DATA_LEN 256   /* size of the "data" string associated with each */
                                 /* command in the command script */
#define ME_INPROC_MAX_CMDS 16384 /* max number of commands */

struct cs_command {        /* one command in the command script */
  struct me_action_struct action;
  char data[ME_INPROC_DATA_LEN];
  };

/*************************************************************/
/*** me_medfg() **********************************************/
/*************************************************************/
/* parses "beam" (custom beam delays), makes dfile, sends to scheduler */
/* NOTE: This is used for custom beams in STEPPED mode -- otherwise, see me_beamspec() */
int me_medfg( struct beam_struct beam,
              char *dfile, /* needs to end in ".df" */
              FILE *fpl ) {

  unsigned short int d[2*LWA_MAX_NSTD];
  int i;
  FILE *fp = NULL;
  char cmd[256]; 
  char df_file[256];

  /* convert to big-endian while packing into d[] */
  for (i=0; i<2*LWA_MAX_NSTD; i++) {
      d[i] = LWA_i2u_swap(beam.OBS_BEAM_DELAY[i]);
      //printf("%hu\n",d[i]);
      } /* for i */

  /* save the delay file */
  sprintf(df_file,"me_inproc_bm/%s",dfile);
  fp = fopen(df_file,"wb");
  if (!fp) {
    printf("FATAL: In me_medfg() of me_inproc(), unable to open '%s' for output.\n",df_file);
    fprintf(fp,"FATAL: In me_medfg() of me_inproc(), unable to open '%s' for output.\n",df_file);
    exit(EXIT_FAILURE);
   }  
  fwrite( d, sizeof(d[0]), sizeof(d)/sizeof(d[0]), fp );
  fclose(fp);
  fp = NULL;

  /* Transfer the file */
  #ifdef ME_SCP2CP  /*see me.h */
    sprintf(cmd, "cp %s %s/dfiles/.",df_file,LWA_SCH_SCP_DIR);
  #else
     sprintf(cmd, "scp %s %s:%s/dfiles/.",df_file,LWA_SCH_SCP_ADDR,LWA_SCH_SCP_DIR);
  #endif
  fprintf(fpl,"me_medfg(): system('%s')\n",cmd);
  //printf("'%s'\n",cmd);
  system(cmd);

  return 0;
  }

/*************************************************************/
/*** me_megfg() **********************************************/
/*************************************************************/
/* parses "beam" (custom beam gains), makes gfile, sends to scheduler */
/* NOTE: This is used for custom beams in STEPPED mode */
int me_megfg( struct beam_struct beam,
              char *gfile, /* needs to end in ".gf" */
              FILE *fpl ) {

  signed short int g[LWA_MAX_NSTD][2][2]; /* matches up with signed short int beam.OBS_BEAM_GAIN[LWA_MAX_NSTD][2][2]; */
  int i,j,k;
  FILE *fp = NULL;
  char cmd[256]; 
  char gf_file[256];

  /* zero out input matrix */
  for (i=0; i<LWA_MAX_NSTD; i++) {
    g[i][0][0] = 0;
    g[i][0][1] = 0;
    g[i][1][0] = 0;
    g[i][1][1] = 0;
    }

  /* pack g[][][] while converting to big-endian */
  for (i=0; i<LWA_MAX_NSTD; i++) {
    for (j=0; j<2; j++) {
      for (k=0; k<2; k++) {
        g[i][j][k] = LWA_i2s_swap(beam.OBS_BEAM_GAIN[i][j][k]);  
        }
      }
    //printf("%hd %hd %hd %hd\n",g[i][0][0],g[i][0][1],g[i][1][0],g[i][1][1]);
    }

  /* save the gain file */
  sprintf(gf_file,"me_inproc_bm/%s",gfile);
  fp = fopen(gf_file,"wb");
  if (!fp) {
    printf("FATAL: In me_megfg() of me_inproc(), unable to open '%s' for output.\n",gf_file);
    fprintf(fp,"FATAL: In me_megfg() of me_inproc(), unable to open '%s' for output.\n",gf_file);
    exit(EXIT_FAILURE);
   }  
  fwrite( g, sizeof(g[0][0][0]), sizeof(g)/sizeof(g[0][0][0]), fp );
  fclose(fp);
  fp = NULL;

  /* Transfer the file */
  #ifdef ME_SCP2CP  /*see me.h */
     sprintf(cmd, "cp %s %s/gfiles/.",gf_file,LWA_SCH_SCP_DIR);
  #else
     sprintf(cmd, "scp %s %s:%s/gfiles/.",gf_file,LWA_SCH_SCP_ADDR,LWA_SCH_SCP_DIR);
  #endif
  fprintf(fpl,"me_megfg(): system('%s')\n",cmd);
  //printf("'%s'\n",cmd);
  system(cmd);

  return 0;
  }

/*************************************************************/
/*** me_beamspec() *******************************************/
/*************************************************************/
/* Reads the command script (.cs) file. */
/* Makes a list of beam delay & gain files needed by BAM commands */
/* The list is fed to a separate utility which generates the spec files */
/* The spec files are then transferred to Scheduler */
/* NOTE: This is NOT used for custom beams in STEPPED mode -- see me_me?fg() */

int me_beamspec( char *cs_filename, 
                 FILE *fpl ) {
  int err = 0;
  FILE *fp = NULL;
  FILE *fpo = NULL;
  char bm_filename[1024];
  struct me_action_struct action;
  char data[16384];
  int arg1, arg4;
  float fmhz, alt, az;
  char gfname[1024];  
  char cmd[256]; 
  int m=0; /* using to count number of beams that will be processed */

  sprintf(bm_filename,"sinbox/bm.dat");

  /* Open .cs file */
  if ((fp=fopen(cs_filename,"rb"))==NULL) {
    fprintf(fpl,"FATAL: me_beamspec() can't open cs_filename='%s'\n",cs_filename);
    return 1;
    }

  /* Open .bm file */
  if ((fpo=fopen(bm_filename,"w"))==NULL) {
    fprintf(fpl,"FATAL: me_beamspec() can't open bm_filename='%s'\n",bm_filename);
    fclose(fp);
    fp = NULL;
    return 1;
    }

  fprintf(fpl,"me_beamspec() file reads as follows:\n");

  /* main loop */
  while( fread( &action, sizeof(struct me_action_struct), 1, fp ) > 0 ) {
 
    /* read the rest of the action */
    strcpy(data,"");
    if (action.len>0) fread( data, action.len, 1, fp );

    if ((action.sid==LWA_SID_DP_ || action.sid==LWA_SID_ADP || action.sid==LWA_SID_NDP) && (action.cid==LWA_CMD_BAM)) {

      if (data[2]!=99) { /* ASCII 99 ("c") denotes custom beams -- those are already dealt with */
        m++;
         
        //fprintf(fpo,"'%s'\n",data);
        //'1 740_766_2071.df 111226_XY.gf 0'
        //'1 740_766_2072.df 111226_XY.gf 0'
        sscanf(data,"%d %3f_%3f_%4f.df %s %d",&arg1,&fmhz,&alt,&az,gfname,&arg4);
        fmhz /= 10;
        alt /= 10;
        az /= 10;
        fprintf(fpo,"%4.1f %5.1f\n",alt,az);
        fprintf(fpl,"%4.1f %5.1f\n",alt,az);

        } /* if (data[3]!="c") */

      } /* if ((action.sid==LWA_SID_DP_) && (action.cid==LWA_CMD_BAM)) */

    } /* while ( fread( */

  /* Close files */
  fclose(fp);
  fp = NULL;  /* .cs */
  fclose(fpo);
  fpo = NULL; /* .bm */

  if (m>0) { /* could be zero if all beams were "custom" beams */

    /* clean out the me_inproc_bm directory */
    sprintf(cmd,"rm -rf me_inproc_bm/*");
    system(cmd);

    /* Call mefsdfg to generate the files */ 
    fprintf(fpl,"me_beamspec(): system('./mefsdfg state me_inproc_bm %4.1f 2 sinbox/bm.dat')\n",fmhz);
    sprintf(cmd,"./mefsdfg state me_inproc_bm %4.1f 2 sinbox/bm.dat",fmhz);
    system(cmd);

    /* Transfer the files */
    #ifdef ME_SCP2CP  /*see me.h */
       sprintf(cmd, "cp me_inproc_bm/* %s/dfiles/.",LWA_SCH_SCP_DIR);
    #else
       sprintf(cmd, "scp me_inproc_bm/* %s:%s/dfiles/.",LWA_SCH_SCP_ADDR,LWA_SCH_SCP_DIR);
    #endif
    fprintf(fpl,"me_beamspec(): system('%s')\n",cmd);
    //printf("'%s'\n",cmd);
    system(cmd);

    } /* if (m>0) */

  return err;
  } /* me_beamspec() */


/*************************************************************/
/*** me_bdm_setup() ******************************************/
/*************************************************************/
/* Set up observation in "OBS_BDM" mode.  This means: */
/* Parsing the arguments, constructing the appropriate gain file for BAM commands, and saving it */

int me_bdm_setup( char *OBS_BDM, 
                  FILE *fpl,
                  char *gfile    /* input: unique filename including ".gf" extension */
                ) {

  int err = 0;

  int   std;
  float gb;
  float gd;
  char  pol[2];

  FILE *fpi = NULL;
  FILE *fpo = NULL;

  int i;
  float g[2][2];
  float norm;

  char cmd[256];

  fprintf(fpl,"me_bdm_setup(): OBS_BDM keyword detected:\n");

  pol[0] = '\0'; pol[1] = '\0';
  sscanf(OBS_BDM,"%d %f %f %1c",&std,&gb,&gd,pol);
  fprintf(fpl,"  parameters: std=%d, gb=%f, gd=%f, pol='%s'\n",std,gb,gd,pol);

  /* error checking */
  if ( (std<1) || (std>ME_MAX_NSTD) ) { 
    fprintf(fpl,"  FATAL: std out of range\n");
    err = 1; 
    }
  if (!( (pol[0]=='X') || (pol[0]=='Y') )) {
    fprintf(fpl,"  FATAL: pol is not 'X' or 'Y'\n");
    err = 2; 
    }
  if (err>0) { 
    fprintf(fpl,"  Aborting OBS_BDM setup.  Obs will run normally (as beam-beam)\n");
    return err; 
    }

  /* Read in lines from state/default_m.gft (was created by me_make_gf() during exec startup), *\
  /* Then write out modified lines to me_inproc_bm/temp.gft */
  fpi = fopen("state/default_m.gft","r");
  if (!fpi) {
    printf("me_inproc.c / me_bdm_setup(): FATAL: Unable to open 'state/default_m.gft' for input.\n");
    exit(EXIT_FAILURE);
    }
  fpo = fopen("me_inproc_bm/temp.gft","w");

  i=0; /* counting stands */
  while ( fscanf(fpi,"%f %f %f %f",&g[0][0],&g[0][1],&g[1][0],&g[1][1]) >0) {
    i++; /* so, first stand read is considered stand 1, next is 2, etc. */
    //printf("me_bdm_setup(): std %d in: %4.2f %4.2f %4.2f %4.2f\n",i,g[0][0],g[0][1],g[1][0],g[1][1]);

    //norm = g[0][0]*g[0][0] + g[0][1]*g[0][1] + g[1][0]*g[1][0] + g[1][1]*g[1][1];
    //if (norm>(1.0e-3)) { /* otherwise, assume the stand has been marked out and should remain zeroed. */
    //  g[0][0]=0.0; g[0][1]=0.0; g[1][0]=0.0; g[1][1]=0.0;
    //  //if (pol[0]=='X') { g[0][0] = gb; } else { g[0][1] = gb; } // replaced 140324
    //  if (pol[0]=='X') { g[0][0] = gb; } else { g[1][0] = gb; }
    //  if (i==std) { 
    //    //if (pol[0]=='X') { g[1][0] = gd; } else { g[1][1] = gd; } // replaced 140324
    //    if (pol[0]=='X') { g[0][1] = gd; } else { g[1][1] = gd; }
    //    }
    //  } /* if norm */

    // 140325: replaced above block with this:
    norm = g[0][0]*g[0][0] + g[0][1]*g[0][1] + g[1][0]*g[1][0] + g[1][1]*g[1][1];
    g[0][0]=0.0; g[0][1]=0.0; g[1][0]=0.0; g[1][1]=0.0;
    if (i==std) { 
        if (pol[0]=='X') { g[0][1] = gd; } else { g[1][1] = gd; }
      } else {
        if (norm>(1.0e-4)) { /* otherwise, assume the stand has been marked out and should remain zeroed. */
          if (pol[0]=='X') { g[0][0] = gb; } else { g[1][0] = gb; }
          }
      }

    fprintf(fpo,"%7.3f %7.3f %7.3f %7.3f\n",g[0][0],g[0][1],g[1][0],g[1][1]);
    } /* while (fscanf */

  fclose(fpi);
  fpi = NULL; 
  fclose(fpo);
  fpo = NULL;

  /* Call megfg to convert ASCII to packed binary form */
  fprintf(fpl,"  running './megfg me_inproc_bm/temp.gft me_inproc_bm/%s'\n",gfile);
  sprintf(cmd,"./megfg me_inproc_bm/temp.gft me_inproc_bm/%s",gfile);
  system(cmd);

  /* copy this file to ../sch/gfiles/. */
  #ifdef ME_SCP2CP  /*see me.h */
     sprintf(cmd, "cp me_inproc_bm/%s %s/gfiles/.",gfile,LWA_SCH_SCP_DIR);
  #else
     sprintf(cmd, "scp me_inproc_bm/%s %s:%s/gfiles/.",gfile,LWA_SCH_SCP_ADDR,LWA_SCH_SCP_DIR);
  #endif
  system(cmd);

  return err;
  } /* me_bdm_setup() */


/*******************************************************************/
/*** me_inproc_cmd_log() *******************************************/
/*******************************************************************/

void me_inproc_cmd_log( FILE *fpl,
                        struct cs_command *cmd,
                        int eDataFormat ) {
  long int mjd;
  long int mpm;

  LWA_timeval( &(cmd->action.tv), &mjd, &mpm ); /* convert timeval to (mjd,mpm) */

  fprintf(fpl,"cmd: %5ld  %8ld  %1d %3s %3s %d",
                    mjd,  mpm,  cmd->action.bASAP,
                                    LWA_sid2str(cmd->action.sid),
                                        LWA_cmd2str(cmd->action.cid),
                                            cmd->action.len );
  switch (eDataFormat) {
    case 1: /* show entire thing as a string in the same line */
      fprintf(fpl," '%s'\n",cmd->data);
      break;
    default: /* includes 0 */
      fprintf(fpl,"\n");
      break;
    }
  } /* me_inproc_cmd_log() */

/*************************************************************/
/*** me_timecalc() *******************************************/
/*************************************************************/

void me_timecalc( long int mjd0, 
                  long int mpm0, 
                  long int delms, /* milliseconds to add to (mjd0,mpm0) */
                  long int *mjd,  /* result goes here */
                  long int *mpm) {
  (*mjd) = mjd0;
  (*mpm) = mpm0 + delms;
  while ((*mpm)>=86400000) { 
    (*mpm) -= 86400000;
    (*mjd)++;
    }
  while (*mpm<0) { 
    (*mpm) += 86400000;
    (*mjd)--;
    }
  } /* me_timecalc() */

/*************************************************************/
/*** angle_sep() *********************************************/
/*************************************************************/

double angle_sep( double a, double b, double modulo ) { 
  /* finds angular distance between angles a and b */
  /* "modulo" is 360.0 for degrees and 6.2832 for radians */
  /* deals correctly with wrapping */
  double c,c1,c2;
  while (a< 0.0   ) { a+=modulo; } /* guarantees a is in [0,modulo] */
  while (a>=modulo) { a-=modulo; } 
  while (b< 0.0   ) { b+=modulo; } /* guarantees b is in [0,modulo] */
  while (b>=modulo) { b-=modulo; }
  c  = fabs(  a         -  b         );
  c1 = fabs( (a+modulo) -  b         );
  c2 = fabs(  a         - (b+modulo) );  
  if (c1<c) c=c1;
  if (c2<c) c=c2;
  return c;
  }


/*************************************************************/
/*** angle_sep_2d() ******************************************/
/*************************************************************/

double angle_sep_2d( double az0, double alt0, double az1, double alt1 ) {
  /* finds angular distance between points (az0,alt0) and (az1,alt1) */
  /* in degrees */
  double tE, tA, h;
  az0 *= M_PI/180;  /* to radians */
  alt0 *= M_PI/180;  /* to radians */
  az1 *= M_PI/180;  /* to radians */
  alt1 *= M_PI/180;  /* to radians */

  /* the Haversine formula */
  tE = sin((alt1-alt0)/2);
  tA = sin((az1-az0)/2);
  h = tE*tE + cos(alt0)*cos(alt1)*tA*tA;
  if( h > 1) { h = 1; }  /* make sure we are in range */
  h = 2*asin(sqrt(h));

  h *= 180/M_PI;  /* to degrees */
  return h;
  }


/*************************************************************/
/*** me_trim() *********************************************/
/*************************************************************/

void me_trim( char *src, char *dest ) {

  int i;
  int k=0;
  for (i=0;i<strlen(src);i++) {
    if (src[i]!=' ') { 
      dest[k] = src[i];
      k++;
      }
    }
  dest[k] = '\0';
  } /* me_trim() */

/*******************************************************************/
/*** main() ********************************************************/
/*******************************************************************/

int main ( int narg, char *argv[] ) {

  /* command line parameters */
  char inp_filename[ME_FILENAME_MAX_LENGTH];

  /* other variables */

  FILE *fp = NULL;
  struct ssf_struct ssf;

  FILE *fpl = NULL; /* log file */
  char log_filename[ME_FILENAME_MAX_LENGTH];

  FILE *fpc = NULL; /* command script file */
  char cs_filename[ME_FILENAME_MAX_LENGTH];

  FILE *fpo = NULL; /* obs file */
  char osf_filename[ME_FILENAME_MAX_LENGTH];

  struct timeval tv;
  struct timeval tv2; /* used to remember observation end time while computing BAM updates */

  struct cs_command cs[ME_INPROC_MAX_CMDS];  /* the command script */
  long int ncs = 0; /* number of commands in command script */

  char cmd[ME_MAX_COMMAND_LINE_LENGTH];
  
  int bDone=0;

  DIR *dir;
  struct dirent *sDirEnt;

  int i,j,m,p;

  struct osf_struct osf;
  struct osfs_struct osfs;
  struct beam_struct beam;
  struct osf2_struct osf2;
  unsigned int u4;

  char ssc[256];

  int eD = 0; /* =0 is normal operation */
              /* =-1 means we saw an illegal (probably unimplemented) observation mode request */
              /* =1 means we saw a DIAG1 observation request */

  long int dp_cmd_mjd;
  long int dp_cmd_mpm;
  long int t0;
  int b4bits;
  unsigned long int tuning_mask;
  int gain1, gain2;

  int dr_sid;
  long int dr_length_ms;
  char dr_format[33];

  char dfile[1024];
  char gfile[1024];

  struct ssmif_struct s; 

  long int mjd, mpm;
  double ra, dec;
  double dist;
 
  double alt, az, last;
  double last_alt, last_az;
  int bFirst;

  int b[ME_INPROC_MAX_CMDS];
  int k,k0,k1;

  int bOBS=0;

  int err=0;
  int esnTimeAdjust;

  char sProjectIDtrimmed[1024];

  /* First, announce thyself */
  printf("[%d/%d] I am me_inproc\n",ME_INPROC,getpid());

  /* sometimes first call to gettimeofday() returns something bogus, so let's get that out of the way */
  gettimeofday( &tv, NULL ); 

  /* get SSMIF */  
  fp=fopen("state/ssmif.dat","rb");
  fread(&s,sizeof(struct ssmif_struct),1,fp);
  fclose(fp);
  fp = NULL; 
  
  /* set the DRX state variables so that we only DRX when necessary */
  unsigned int last_drx_freq1 = 0;
  unsigned short int last_drx_bw1 = 0; 
  signed short int last_drx_gain1 = -1;
  unsigned char last_drx_high_dr1 = 0;
  unsigned int last_drx_freq2 = 0;
  unsigned short int last_drx_bw2 = 0;
  signed short int last_drx_gain2 = -1;
  unsigned char last_drx_high_dr2 = 0;

  /*****************/
  /*****************/
  /*** main loop ***/
  /*****************/
  /*****************/  

  while (!bDone) {
    
    /* check for a .inp file */
    if (!(dir = opendir("sinbox"))) {
      printf("[%d/%d] FATAL: Couldn't opendir('sinbox')\n",ME_INPROC,getpid());
      exit(EXIT_FAILURE);
      } 

    while ( (sDirEnt=readdir(dir)) != NULL ) {
      //printf("Looking at '%s'\n",sDirEnt->d_name);
      if (strstr(sDirEnt->d_name,".inp")!=NULL) { /* found one */
        //printf("...this is a .inp; working on it\n",sDirEnt->d_name);      

        /*******************************************************/
        /*** setting up this file (session) for inprocessing ***/
        /*******************************************************/

        /* initializing the command script command count */
        ncs=0;
     
        /* initializing other things */
        bOBS=0;

        /* read the .inp file */
        sprintf(inp_filename,"sinbox/%s",sDirEnt->d_name);
        //printf("...inp_filename='%s':\n",inp_filename); 
        if ((fp=fopen(inp_filename,"rb"))==NULL) {
          printf("[%d/%d] FATAL: me_inproc can't open '%s'\n",ME_INPROC,getpid(),inp_filename);
          closedir(dir);
          exit(EXIT_FAILURE);
          }
        fread(&ssf,sizeof(struct ssf_struct),1,fp);
        fclose(fp);
        fp = NULL;  
        //printf("...ssf.PROJECT_ID='%s'\n",ssf.PROJECT_ID);
        //return;

        /* open a log file for this session */
        sprintf(log_filename,"sinbox/%s_%04u.ipl",ssf.PROJECT_ID,ssf.SESSION_ID);
        //printf("...log_filename='%s'\n",log_filename); 
        if ((fpl = fopen(log_filename,"w"))==NULL) {
          printf("[%d/%d] FATAL: me_inproc can't open '%s'\n",ME_INPROC,getpid(),log_filename);
          closedir(dir);
          if( fp != NULL ) {
            fclose(fp);
            fp = NULL;
          }
          if( fpl != NULL ) {
            fclose(fpl);
            fpl = NULL;
          }
          if( fpc != NULL ) {
            fclose(fpc);
            fpc = NULL;
          }
          if( fpo != NULL ) {
            fclose(fpo);
            fpo = NULL;
          } 
          exit(EXIT_FAILURE);
          }
        fprintf(fpl,"[%d/%d] starting\n",ME_INPROC,getpid());

        /* there has to be at least one observaton */
        if (ssf.SESSION_NOBS<1) {

          fprintf(fpl,"Concluding with ESF status because ssf.SESSION_NOBS<1.\n");

          gettimeofday( &(cs[ncs].action.tv), NULL ); /* not used in this case */
          cs[ncs].action.bASAP = 1;                   /* execute ASAP */
          cs[ncs].action.sid = LWA_SID_MCS;           /* first command is always directed to MCS */
          cs[ncs].action.cid = LWA_CMD_ESF;           /* end session as failure */
          cs[ncs].action.len = 0;                     /* no additional bytes */
          strcpy(cs[ncs].data,"");
          me_inproc_cmd_log( fpl, &(cs[ncs]), 0 ); /* write log msg explaining command */
          ncs++;

          }


        /*******************************************************/
        /*** Iterate through observations **********************/
        /*******************************************************/
        for ( i=1; i<=ssf.SESSION_NOBS; i++ ) {

          /*******************************************************/
          /*******************************************************/
          /*** FIRST PASS THROUGH FILE ***************************/
          /*******************************************************/
          /*******************************************************/
          /* have to make two passes because some information needed to do */
          /* observation may appear at end of .obs file */

          /* open the .obs file */
          //sprintf(osf_filename,"../../tp/mbox/%s_%04d_%04d.obs",ssf.PROJECT_ID,ssf.SESSION_ID,i);
          sprintf(osf_filename,"sinbox/%s_%04d_%04d.obs",ssf.PROJECT_ID,ssf.SESSION_ID,i);
          if ((fpo = fopen(osf_filename,"rb"))==NULL) {
            fprintf(fpl,"[%d/%d] FATAL: me_inproc can't open '%s'\n",ME_INPROC,getpid(),osf_filename);
            printf(     "[%d/%d] FATAL: me_inproc can't open '%s'\n",ME_INPROC,getpid(),osf_filename);
            closedir(dir);
            fflush(fpl);
            if( fp != NULL ) {
              fclose(fp);
              fp = NULL;
            }
            if( fpl != NULL ) {
              fclose(fpl);
              fpl = NULL;
            }
            if( fpc != NULL ) {
              fclose(fpc);
              fpc = NULL;
            }
            if( fpo != NULL ) {
              fclose(fpo);
              fpo = NULL;
            }           
            exit(EXIT_FAILURE);   
            }
          fprintf(fpl,"opened '%s' (first pass)\n",osf_filename);

          /*----------------------------------------------*/
          /*--- Reading .obs file ------------------------*/
          /*----------------------------------------------*/

          /* The first part of the .obs file is a osf_struct */
          m = fread( &osf, sizeof(struct osf_struct), 1, fpo );

          fprintf(fpl,"  osf.FORMAT_VERSION  = %hu\n",osf.FORMAT_VERSION);
          fprintf(fpl,"  osf.PROJECT_ID = '%s'\n",osf.PROJECT_ID);
          fprintf(fpl,"  osf.SESSION_ID = %u\n",osf.SESSION_ID);        
          fprintf(fpl,"  osf.SESSION_DRX_BEAM = %hd\n",osf.SESSION_DRX_BEAM);
          fprintf(fpl,"  osf.OBS_ID = %u\n",osf.OBS_ID);
          fprintf(fpl,"  osf.OBS_START_MJD = %lu\n",osf.OBS_START_MJD);
          fprintf(fpl,"  osf.OBS_START_MPM = %lu\n",osf.OBS_START_MPM);
          fprintf(fpl,"  osf.OBS_DUR = %lu\n",osf.OBS_DUR);
          LWA_saymode( osf.OBS_MODE, ssc ); fprintf(fpl,"  osf.OBS_MODE  = %hu ('%s')\n",osf.OBS_MODE,ssc);
          fprintf(fpl,"  osf.OBS_BDM = '%s'\n",osf.OBS_BDM);
          fprintf(fpl,"  osf.OBS_RA = %f\n",osf.OBS_RA);
          fprintf(fpl,"  osf.OBS_DEC = %f\n",osf.OBS_DEC);
          fprintf(fpl,"  osf.OBS_B = %hd\n",osf.OBS_B);
		fprintf(fpl,"  osf.OBS_FREQ1 = %u\n",osf.OBS_FREQ1);
          fprintf(fpl,"  osf.OBS_FREQ2 = %u\n",osf.OBS_FREQ2);
          fprintf(fpl,"  osf.OBS_BW = %hu\n",osf.OBS_BW);
          fprintf(fpl,"  osf.OBS_STP_N = %u\n",osf.OBS_STP_N);
          fprintf(fpl,"  osf.OBS_STP_RADEC = %hu\n",osf.OBS_STP_RADEC);
          //fcloseall(); closedir(dir); return;

          /* parse observing modes */
          LWA_saymode( osf.OBS_MODE, ssc );
          fprintf(fpl,"  osf.OBS_MODE = %hu ('%s')\n",osf.OBS_MODE,ssc);
          switch (osf.OBS_MODE) {
            case LWA_OM_TRK_RADEC:
            case LWA_OM_TRK_SOL:   
            case LWA_OM_TRK_JOV:   
            case LWA_OM_TRK_LUN:   
              eD=0;
              break;
            case LWA_OM_STEPPED:
#if (defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP) || (defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP)
            case LWA_OM_TBF:
#else
            case LWA_OM_TBW:      
#endif
            case LWA_OM_TBN:
            
              eD=0;  
              break;
            case LWA_OM_DIAG1:
              eD=1;
              break;
            default:               /* illegal mode */
              fprintf(fpl,"Encountered an illegal OBS_MODE; zapping command script:\n");
              eD=-1;
              break;
            }

          if (eD==-1) { /* if this observation mode is bogus, zap command script */
            fclose(fpc);
            fpc = NULL;
            fprintf(fpl,"cs file closed\n");
            if ((fpc=fopen(cs_filename,"wb"))==NULL) {
              fprintf(fpl,"[%d/%d] FATAL: me_inproc can't open '%s'\n",ME_INPROC,getpid(),cs_filename);
              printf(     "[%d/%d] FATAL: me_inproc can't open '%s'\n",ME_INPROC,getpid(),cs_filename);
              closedir(dir);
              if( fp != NULL ) {
                fclose(fp);
                fp = NULL;
              }
              if( fpl != NULL ) {
                fclose(fpl);
                fpl = NULL;
              }
              if( fpc != NULL ) {
                fclose(fpc);
                fpc = NULL;
              }
              if( fpo != NULL ) {
                fclose(fpo);
                fpo = NULL;
              }           
              exit(EXIT_FAILURE);    
              }  
            fprintf(fpl,"cs file is open\n"); 
            }

          /*********************************/
          /* for the rest of this pass ... */
          /*********************************/  

          /* If a STEPPED observation, we have a set of { osfs_struct, beam }'s for each step */
          for ( m=1; m<=osf.OBS_STP_N; m++ ) {
            fread(&osfs,sizeof(struct osfs_struct),1,fpo);
            if ( osfs.OBS_STP_B==LWA_BT_SPEC_DELAYS_GAINS) {
              fread(&beam,sizeof(struct beam_struct),1,fpo);
              } /* if ( osfs.OBS_STP_B==LWA_BT_SPEC_DELAYS_GAINS) */
            fread(&u4,sizeof(u4),1,fpo);
            if ( u4 != 4294967294 ) {
              fprintf(fpl,"[%d/%d] FATAL: me_inproc doesn't see '2^32-2' marker\n",ME_INPROC,getpid()); 
              printf(     "[%d/%d] FATAL: me_inproc doesn't see '2^32-2' marker\n",ME_INPROC,getpid());
              closedir(dir); 
              if( fp != NULL ) {
                fclose(fp);
                fp = NULL;
              }
              if( fpl != NULL ) {
                fclose(fpl);
                fpl = NULL;
              }
              if( fpc != NULL ) {
                fclose(fpc);
                fpc = NULL;
              }
              if( fpo != NULL ) {
                fclose(fpo);
                fpo = NULL;
              }      
              exit(EXIT_FAILURE); 
              }
            } /* for m */

          fread(&osf2,sizeof(struct osf2_struct),1,fpo);
          /* these get written to log just before log is closed */  

          fread(&u4,sizeof(u4),1,fpo);
          //u4 = 4294967295; fwrite(&u4, sizeof(u4),1,fp); /* = $2^{32}-1$. */
          if ( u4 != 4294967295 ) {
            fprintf(fpl,"[%d/%d] FATAL: me_inproc doesn't see '2^32-1' marker\n",ME_INPROC,getpid());
            printf(     "[%d/%d] FATAL: me_inproc doesn't see '2^32-1' marker\n",ME_INPROC,getpid()); 
            closedir(dir);
            if( fp != NULL ) {
              fclose(fp);
              fp = NULL;
            }
            if( fpl != NULL ) {
              fclose(fpl);
              fpl = NULL;
            }
            if( fpc != NULL ) {
              fclose(fpc);
              fpc = NULL;
            }
            if( fpo != NULL ) {
              fclose(fpo);
              fpo = NULL;
            }          
            exit(EXIT_FAILURE); 
            }

          /* close the .obs file */
          fclose(fpo);
          fpo = NULL;
          fprintf(fpl,"closed '%s' (first pass)\n",osf_filename);

          /*************************************************/
          /*** End of first pass ***************************/
          /*************************************************/
  
          /* Writing first command of the session: */
          LWA_time2tv( &(cs[ncs].action.tv), ssf.SESSION_START_MJD, ssf.SESSION_START_MPM );
          cs[ncs].action.bASAP = 0;                   
          cs[ncs].action.sid = LWA_SID_MCS;           /* directed to MCS */
          switch (eD) {
            case  0: /* normal processing */
            case  1: /* DIAG1 mode */
              cs[ncs].action.cid = LWA_CMD_NUL; /* normal first command */
              break;
            //case  1: /* DIAG1 mode */
            //  action.cid = LWA_CMD_ESN; /* For DIAG1 mode, we do nothing */
            //  break;
            default: /* error; includes eD==-1 */
              cs[ncs].action.cid = LWA_CMD_ESF; /* If the mode is unimplemented or illegal, fail */
              break;
            }
          cs[ncs].action.len = 0;                     /* no additional bytes */
          strcpy(cs[ncs].data,""); 
          if ( (i==1) || (eD!=0) ) {
            me_inproc_cmd_log( fpl, &(cs[ncs]), 0 ); /* write log msg explaining command */
            ncs++;
            }

          /*****************************/
          /* session setup happens now */
          /*****************************/ 
          if ( (i==1) && (eD==0) ) {
            fprintf(fpl,"Beginning session setup\n");
            /* FIXME */
            /* prep data recorder */
            } /* session setup */
            // SESSION_CRA;
            // SESSION_START_MJD;
            // SESSION_START_MPM;
            // SESSION_MRP_ASP;
            // SESSION_MRP_DP_;
            // SESSION_MRP_DR1;
            // SESSION_MRP_DR2;
            // SESSION_MRP_DR3;
            // SESSION_MRP_DR4;
            // SESSION_MRP_DR5;
            // SESSION_MRP_SHL;
            // SESSION_MRP_MCS;
            // SESSION_MUP_ASP;
            // SESSION_MUP_DP_;
            // SESSION_MUP_DR1;
            // SESSION_MUP_DR2;
            // SESSION_MUP_DR3;
            // SESSION_MUP_DR4;
            // SESSION_MUP_DR5;
            // SESSION_MUP_SHL;
            // SESSION_MUP_MCS;
            // SESSION_LOG_SCH;
            // SESSION_LOG_EXE;
            // SESSION_INC_SMIB;
            // SESSION_INC_DES;

          /*********************************/
          /* observation setup happens now */
          /*********************************/           
          if (eD==0) {

            /* write "observation start" command */
            bOBS=1; /* remember that we did this */
            LWA_time2tv( &(cs[ncs].action.tv), osf.OBS_START_MJD, osf.OBS_START_MPM ); 
            cs[ncs].action.bASAP = 0;                   
            cs[ncs].action.sid = LWA_SID_MCS;  
            cs[ncs].action.cid = LWA_CMD_OBS; 
            cs[ncs].action.len = 0;
            strcpy(cs[ncs].data,""); 
            me_inproc_cmd_log( fpl, &(cs[ncs]), 0 ); /* write log msg explaining command */
            ncs++;

            /* FIXME just before observation start: set OBS_FEE, OBS_ASP_* */

            /* ====================== */
            /* === DR REC command === */
            /* ====================== */
            
#if defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP
            /* for NDP output 1, 2, 3, 4 (beams) we do this once; i.e., one recording per session */
            /* for NDP output 1 (TBF) output we do a new recording for each observation */
            if ( ( (osf.SESSION_DRX_BEAM<=ME_MAX_NDPOUT) && (osf.OBS_MODE != LWA_OM_TBF) && (i==1) ) || 
                 ( (osf.SESSION_DRX_BEAM<ME_MAX_NDPOUT) && (osf.OBS_MODE == LWA_OM_TBF) ) ) {
              dr_sid=-1;
              for( j=0; j<ME_MAX_NDR; j++ ) {
                 if( osf.SESSION_DRX_BEAM == s.iDRDP[j] ) {
                    dr_sid = LWA_SID_DR1 + j;
                    break;
                    }
                 }
#elif defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP
            /* for ADP output 1, 2, 3 (beams) we do this once; i.e., one recording per session */
            /* for ADP output 1 (TBF) output we do a new recording for each observation */
            /* for ADP output 4 (TBN) output we do a new recording for each observation */
            if ( ( (osf.SESSION_DRX_BEAM<ME_MAX_NDPOUT) && (osf.OBS_MODE != LWA_OM_TBF) && (i==1) ) || 
                 ( (osf.SESSION_DRX_BEAM<ME_MAX_NDPOUT) && (osf.OBS_MODE == LWA_OM_TBF) ) ||
                 (  osf.SESSION_DRX_BEAM==ME_MAX_NDPOUT           )   ) {
              dr_sid=-1;
              for( j=0; j<ME_MAX_NDR; j++ ) {
                 if( osf.SESSION_DRX_BEAM == s.iDRDP[j] ) {
                    dr_sid = LWA_SID_DR1 + j;
                    break;
                    }
                 }
#else
            /* for DP outputs 1-4 (beams), we do this once; i.e., one recording per session */
            /* for DP output 5 (TBN/TBW), we do a new recording for each observation */
            if ( ( (osf.SESSION_DRX_BEAM<ME_MAX_NDPOUT) && (i==1) ) || 
                 (  osf.SESSION_DRX_BEAM==ME_MAX_NDPOUT           )   ) {
              dr_sid=-1;
              for( j=0; j<ME_MAX_NDR; j++ ) {
                 if( osf.SESSION_DRX_BEAM == s.iDRDP[j] ) {
                    dr_sid = LWA_SID_DR1 + j;
                    break;
                    }
                 }
#endif
              if (dr_sid==-1) {
                printf(     "[%d/%d] FATAL: osf.SESSION_DRX_BEAM=%d is not in s.iDRDP[0..%d]\n",ME_INPROC,getpid(),osf.SESSION_DRX_BEAM,ME_MAX_NDR-1); 
                fprintf(fpl,"[%d/%d] FATAL: osf.SESSION_DRX_BEAM=%d is not in s.iDRDP[0..%d]\n",ME_INPROC,getpid(),osf.SESSION_DRX_BEAM,ME_MAX_NDR-1);
                closedir(dir);
                if( fp != NULL ) {
                  fclose(fp);
                  fp = NULL;
                }
                if( fpl != NULL ) {
                  fclose(fpl);
                  fpl = NULL;
                }
                if( fpc != NULL ) {
                  fclose(fpc);
                  fpc = NULL;
                }
                if( fpo != NULL ) {
                  fclose(fpo);
                  fpo = NULL;
                } 
                exit(EXIT_FAILURE);
                }

#if defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP                
              if ((osf.SESSION_DRX_BEAM<=ME_MAX_NDPOUT) && (osf.OBS_MODE != LWA_OM_TBF)) {
                  dr_length_ms = ssf.SESSION_DUR; /* beam obs are recorded contiguously in one session */
                } else {
                  dr_length_ms = osf.OBS_DUR; /* each TBF observation is a separate recording */ 
                }
#elif defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP                
              if ((osf.SESSION_DRX_BEAM<ME_MAX_NDPOUT) && (osf.OBS_MODE != LWA_OM_TBF)) {
                  dr_length_ms = ssf.SESSION_DUR; /* beam obs are recorded contiguously in one session */
                } else {
                  dr_length_ms = osf.OBS_DUR; /* each TBN/TBF observation is a separate recording */ 
                }
#else
              if (osf.SESSION_DRX_BEAM<ME_MAX_NDPOUT) {
                  dr_length_ms = ssf.SESSION_DUR; /* beam obs are recorded contiguously in one session */
                } else {
                  dr_length_ms = osf.OBS_DUR; /* each TBN/TBW observation is a separate recording */ 
                }
#endif
              strcpy(dr_format,""); 
              switch (osf.OBS_MODE) {
                case LWA_OM_TRK_RADEC: 
                case LWA_OM_TRK_SOL:   
                case LWA_OM_TRK_JOV:   
                case LWA_OM_TRK_LUN:   
                case LWA_OM_STEPPED:
              #if defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP
                  if (osf.OBS_B == LWA_BT_HIGH_DR) {
                    sprintf(dr_format,"DRX8_FILT_%1hu",osf.OBS_BW);
                  } else {
                    sprintf(dr_format,"DRX_FILT_%1hu",osf.OBS_BW);
                  }
              #else
                  sprintf(dr_format,"DRX_FILT_%1hu",osf.OBS_BW); 
              #endif
                  break;
#if (defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP) || (defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP)
                case LWA_OM_TBF:
                  sprintf(dr_format,"DEFAULT_TBF"); 
                  break;
#else
                case LWA_OM_TBW:       
                  sprintf(dr_format,"DEFAULT_TBW"); 
                  break;
#endif
#if !defined(LWA_BACKEND_IS_NDP) || !LWA_BACKEND_IS_NDP
                case LWA_OM_TBN:       
                  sprintf(dr_format,"DEFAULT_TBN"); 
                  break;
#endif
                case LWA_OM_DIAG1:
                  printf(     "[%d/%d] DR setup: osf.OBS_MODE=%d: How'd I get here?\n",ME_INPROC,getpid(),osf.OBS_MODE);
                  fprintf(fpl,"[%d/%d] DR setup: osf.OBS_MODE=%d: How'd I get here?\n",ME_INPROC,getpid(),osf.OBS_MODE);
                default:
                  printf(     "[%d/%d] FATAL: During DR setup, osf.OBS_MODE=%d not recognized\n",ME_INPROC,getpid(),osf.OBS_MODE);
                  fprintf(fpl,"[%d/%d] FATAL: During DR setup, osf.OBS_MODE=%d not recognized\n",ME_INPROC,getpid(),osf.OBS_MODE);
                  closedir(dir);
                  if( fp != NULL ) {
                    fclose(fp);
                    fp = NULL;
                  }
                  if( fpl != NULL ) {
                    fclose(fpl);
                    fpl = NULL;
                  }
                  if( fpc != NULL ) {
                    fclose(fpc);
                    fpc = NULL;
                  }
                  if( fpo != NULL ) {
                    fclose(fpo);
                    fpo = NULL;
                  } 
                  exit(EXIT_FAILURE);  
                  break;
                }

              /* Figure out when to send command to DR */
              me_timecalc( osf.OBS_START_MJD, osf.OBS_START_MPM, /* calc time to send command to DR */
                          -LWA_SESS_DRDP_INIT_TIME_MS,
                           &mjd, &mpm );  
              LWA_time2tv( &tv, mjd, mpm );

              /* construct the command and poll the barcode (if there is a DR to send it to) */
              if( dr_sid != -1 ) {
                 /* data capture command */
                 cs[ncs].action.tv.tv_sec  = tv.tv_sec;
                 cs[ncs].action.tv.tv_usec = tv.tv_usec; 
                 cs[ncs].action.bASAP = 0;                   
                 cs[ncs].action.sid = dr_sid;       /* DR that this is directed to */
                 
                 if (strlen(osf.SESSION_SPC)==0) {
                     /* this is a default ("voltage") mode recording */
                     cs[ncs].action.cid = LWA_CMD_REC;  /* record */ 
                     //sprintf(cs[ncs].data,"%06ld %09ld %09ld %s",osf.OBS_START_MJD,osf.OBS_START_MPM,dr_length_ms,dr_format); // leading zeros
                     sprintf(cs[ncs].data,"%6ld %9ld %9ld %s",osf.OBS_START_MJD,osf.OBS_START_MPM,dr_length_ms,dr_format); // no leading zeros
                   } else {
                     /* this is an SPC ("spectrometer") mode recording */
                     cs[ncs].action.cid = LWA_CMD_SPC;  /* record */ 
                     sprintf(cs[ncs].data,"%6ld %9ld %9ld %s",osf.OBS_START_MJD,osf.OBS_START_MPM,dr_length_ms,osf.SESSION_SPC); // no leading zeros
                   }
                   
                 cs[ncs].action.len = strlen(cs[ncs].data)+1;
                 me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                 ncs++;
                 
                 /* barcode query (only for the first observation) */
                 if ( i==1 ) {
                    cs[ncs].action.tv.tv_sec  = tv.tv_sec + 1;
                    cs[ncs].action.tv.tv_usec = tv.tv_usec; 
                    cs[ncs].action.bASAP = 0;                   
                    cs[ncs].action.sid = dr_sid;       /* DR that this is directed to */
                    cs[ncs].action.cid = LWA_CMD_RPT;
                    sprintf(cs[ncs].data,"DRSU-BARCODE");
                    cs[ncs].action.len = strlen(cs[ncs].data)+1;
                    me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                    ncs++;
                    }
                 }

//  /* Send DR# REC command */
//  sprintf(data,"%06ld %09ld %09ld %s",dr_start_mjd,dr_start_mpm,dr_length_ms,dr_format);
//  err = mesi( NULL, sDR, "REC", data, "today", "asap", &reference );
//  if (err!=MESI_ERR_OK) {
//    printf("[%d/%d] FATAL: mesi(NULL,'%s','REC',...) returned code %d\n",ME_MEOS,getpid(),sDR,err);  
//    eResult += MEOS_ERR_DR_REC;
//    return eResult;  
//    } 
//  sprintf(optag,"%06ld_%09ld",dr_start_mjd,reference);
//  printf("[%d/%d] %s starts @ %ld %ld, records for %ld ms. ref=%ld.\n",ME_MEOS,getpid(),sDR,
//          dr_start_mjd, dr_start_mpm, dr_length_ms, reference );
//  printf("[%d/%d] I anticipate OP-TAG will be '%s'\n",ME_MEOS,getpid(),optag);

              } /* if ( ( (osf.SESSION_DRX_BEAM<5) && (i==1) ... */

            /* ====================== */
            /* === DP/ADP command === */
            /* ====================== */

            /* DP/ADP commands neet to be sent in first 80% of slot N-2, given start time in slot N */
            me_timecalc( osf.OBS_START_MJD, osf.OBS_START_MPM, /* calc time to send command to DP */
                        -2000,
                         &dp_cmd_mjd, &dp_cmd_mpm );  

            switch (osf.OBS_MODE) {

#if (defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP) || (defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP)
              case LWA_OM_TBF:
                /* TBF trigger time is in units of samples from beginning of slot */
                t0 = osf.OBS_START_MPM % 1000; /* number of ms beyond a second boundary */
                t0 = 196000 * t0; /* [samples/ms] * [ms] */
                
                ///* deal with user requests to use SSMIF-specified defaults */ 
                //if (osf2.OBS_TBF_GAIN==-1) { osf2.OBS_TBF_GAIN = s.settings.tbf_gain; }
                ///* if SSMIF also leaves it up MCS, set this to 6 */ 
                if (osf2.OBS_TBF_GAIN==-1) { osf2.OBS_TBF_GAIN = 6; }
                
                /* Unpack the osf2.OBS_TBF_GAIN value to allow two */
                /* different gains to be used with DP.             */
                /* Updated: 2015 Aug 31                            */
                if (osf2.OBS_TBF_GAIN < 16 ) {
                   gain1 = osf2.OBS_TBF_GAIN;
                   gain2 = osf2.OBS_TBF_GAIN;
                } else {
                   gain1 = (osf2.OBS_TBF_GAIN >> 4) & 0xF;
                   gain2 = osf2.OBS_TBF_GAIN & 0xF;
                }
                
                /* TBF needs a DRX command to set things up */
                /** Tuning 1 **/
                if ( (osf.OBS_FREQ1 != last_drx_freq1) || \
                     (osf.OBS_BW != last_drx_bw1) || \
                     (gain1 != last_drx_gain1) ) {
                  LWA_time2tv( &(cs[ncs].action.tv), dp_cmd_mjd, dp_cmd_mpm );
                  cs[ncs].action.tv.tv_sec -= 2; /* Must be sent in slot N-4 instead of N-2 */
                  cs[ncs].action.bASAP = 0;
                  cs[ncs].action.sid = LWA_SID_ADP;  
                  cs[ncs].action.cid = LWA_CMD_DRX; 
                  sprintf( cs[ncs].data, "%hd %12.3f %hu %hd",
                                  1, //tuning 1..NUM_TUNINGS(2) (uint8 DRX_TUNING)
                                        (4.563480616e-02)*(osf.OBS_FREQ1), /* center freq in Hz */
                                              osf.OBS_BW,                  /* 0-8 */
                                                  gain1);                  /* 0-15 */
                  cs[ncs].action.len = strlen(cs[ncs].data)+1; 
                  me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                  ncs++;
                  last_drx_freq1 = osf.OBS_FREQ1;
                  last_drx_bw1 = osf.OBS_BW;
                  last_drx_gain1 = gain1;
                  }
                
                /** Tuning 2 - if needed **/
                if ( (osf.OBS_FREQ2 != 0) && \
                     ( (osf.OBS_FREQ2 != last_drx_freq2) || \
                       (osf.OBS_BW != last_drx_bw2) || \
                       (gain2 != last_drx_gain2) ) && \
                     (osf.SESSION_DRX_BEAM == 1) ) {
                  LWA_time2tv( &(cs[ncs].action.tv), dp_cmd_mjd, dp_cmd_mpm+10 ); /* staggering send times for DP commands by 10 ms */
                  cs[ncs].action.tv.tv_sec -= 2; /* Must be sent in slot N-4 instead of N-2 */
                  cs[ncs].action.bASAP = 0;                   
                  cs[ncs].action.sid = LWA_SID_ADP;  
                  cs[ncs].action.cid = LWA_CMD_DRX; 
                  sprintf( cs[ncs].data, "%hd %12.3f %hu %hd",
                                  2, //tuning 2..NUM_TUNINGS(2) (uint8 DRX_TUNING)
                                        (4.563480616e-02)*(osf.OBS_FREQ2), /* center freq in Hz */
                                              osf.OBS_BW,                  /* 0-8 */
                                                  gain2);                  /* 0-15 */
                  cs[ncs].action.len = strlen(cs[ncs].data)+1;
                  me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                  ncs++;
                  last_drx_freq2 = osf.OBS_FREQ2;
                  last_drx_bw2 = osf.OBS_BW;
                  last_drx_gain2 = gain2;
                  }
                
                /* Define the tuning mask to use */
                tuning_mask = (unsigned long int) 1;
                if( osf.OBS_FREQ2 != 0) {
                  tuning_mask = (unsigned long int) 3;   // (1<<0) | (1<<1)
                }
                
                /* Build up the TBF command */
                LWA_time2tv( &(cs[ncs].action.tv), dp_cmd_mjd, dp_cmd_mpm+5000 );	// TBF needs a bit for the ADP buffers to flush
                cs[ncs].action.bASAP = 0;                   
                cs[ncs].action.sid = LWA_SID_ADP;  
                cs[ncs].action.cid = LWA_CMD_TBF;  
                sprintf( cs[ncs].data, "8 %ld %u %lu", t0, osf2.OBS_TBF_SAMPLES, tuning_mask );
                cs[ncs].action.len = strlen(cs[ncs].data)+1; 
                me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                ncs++;

                break; /* LWA_OM_TBF */
#else
              case LWA_OM_TBW:

                /* TBW trigger time is in units of samples from beginning of slot */
                t0 = osf.OBS_START_MPM % 1000; /* number of ms beyond a second boundary */
                t0 = 196000 * t0; /* [samples/ms] * [ms] */

                b4bits = 0; if (osf2.OBS_TBW_BITS==4) { b4bits = 1; }

                LWA_time2tv( &(cs[ncs].action.tv), dp_cmd_mjd, dp_cmd_mpm );
                cs[ncs].action.bASAP = 0;                   
                cs[ncs].action.sid = LWA_SID_DP_;  
                cs[ncs].action.cid = LWA_CMD_TBW;  
                sprintf( cs[ncs].data, "%d %ld %u", b4bits, t0, osf2.OBS_TBW_SAMPLES );
                cs[ncs].action.len = strlen(cs[ncs].data)+1; 
                me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                ncs++;

                break; /* LWA_OM_TBW */
#endif

//  /* if TBW, now tell DP to start.  It will take a couple seconds before it gets going, so */
//  /* make sure DP record time is long enough to account for this! */
//  if (!strncmp(mode,"TBW",3)) {
//    sprintf(data,"%d 0 %ld",b4bits,nsamp);
//    err = mesi( NULL, "DP_", "TBW", data, "today", "asap", &reference );
//    if (err!=MESI_ERR_OK) {
//      printf("[%d/%d] FATAL: mesi(NULL,'DP_','REC',...) returned code %d\n",ME_MEOS,getpid(),err);  
//      eResult += MEOS_ERR_DP_TBX;
//      return eResult;  
//      } 
//    printf("[%d/%d] DP accepted '%s %s' (ref=%ld).  Here we go...\n",ME_MEOS,getpid(), mode, data, reference );
//    }

#if !defined(LWA_BACKEND_IS_NDP) || !LWA_BACKEND_IS_NDP
              case LWA_OM_TBN:

                /* TBN trigger time is in units of "subslots" (1/100ths of a second) */
                t0 = osf.OBS_START_MPM % 1000; /* number of ms beyond a second boundary */
                t0 /= 10; if (t0>99) t0=99; /* now in subslots */
                
                ///* deal with user requests to use SSMIF-specified defaults */ 
                //if (osf2.OBS_TBN_GAIN==-1) { osf2.OBS_TBN_GAIN = s.settings.tbn_gain; }
                ///* if SSMIF also leaves it up MCS, set this to 20 */ 
                if (osf2.OBS_TBN_GAIN==-1) { osf2.OBS_TBN_GAIN = 20; }
                //osf2.OBS_TBN_GAIN = 20; /* FIXME */

                /* construct the command */ 
#if defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP
                LWA_time2tv( &(cs[ncs].action.tv), dp_cmd_mjd, dp_cmd_mpm );
                cs[ncs].action.tv.tv_sec -= 2; /* Must be sent in slot N-4 instead of N-2 */
                cs[ncs].action.bASAP = 0; 
                cs[ncs].action.sid = LWA_SID_ADP;
                cs[ncs].action.cid = LWA_CMD_TBN;  
                sprintf( cs[ncs].data, "%8.0f %hu %hd",
                                (4.563480616e-02)*(osf.OBS_FREQ1), /* center freq in Hz */
                                      osf.OBS_BW,                  /* 1-11 */
                                          osf2.OBS_TBN_GAIN);      /* 0-30 */
                cs[ncs].action.len = strlen(cs[ncs].data)+1;
                me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                ncs++;
#else
                LWA_time2tv( &(cs[ncs].action.tv), dp_cmd_mjd, dp_cmd_mpm );
                cs[ncs].action.bASAP = 0; 
                cs[ncs].action.sid = LWA_SID_DP_;
                cs[ncs].action.cid = LWA_CMD_TBN;  
                sprintf( cs[ncs].data, "%8.0f %hu %hd %ld",
                                (4.563480616e-02)*(osf.OBS_FREQ1), /* center freq in Hz */
                                      osf.OBS_BW,                  /* 1-7 */
                                          osf2.OBS_TBN_GAIN,       /* 0-30 */
                                              t0);                 /* subslot */ 
                cs[ncs].action.len = strlen(cs[ncs].data)+1;
                me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                ncs++;
#endif
                break; /* LWA_OM_TBN */ 
#endif

              case LWA_OM_TRK_RADEC:
              case LWA_OM_TRK_SOL:
              case LWA_OM_TRK_JOV:
              case LWA_OM_TRK_LUN:

                /* DRX trigger time is in units of "subslots" (1/100ths of a second) */
                t0 = dp_cmd_mpm % 1000; /* number of ms beyond a second boundary */
                t0 /= 10; if (t0>99) t0=99; /* now in subslots */
                
                //printf("debug: s.settings.drx_gain=%hd\n",s.settings.drx_gain);
                //printf("debug: osf2.OBS_DRX_GAIN=%hd\n",osf2.OBS_DRX_GAIN);

                ///* deal with user requests to use SSMIF defaults */ 
                //if (osf2.OBS_DRX_GAIN==-1) { osf2.OBS_DRX_GAIN = s.settings.drx_gain; }
                ///* if SSMIF also leaves this up to MCS, then choose DRX GAIN = 7 */ 
                //if (osf2.OBS_DRX_GAIN==-1) { osf2.OBS_DRX_GAIN = 7; }
                //osf2.OBS_DRX_GAIN = 6; /* FIXME */
                 
                if (osf2.OBS_DRX_GAIN<0) { osf2.OBS_DRX_GAIN = 6; }
                
                /* Unpack the osf2.OBS_DRX_GAIN value to allow two */
                /* different gains to be used with DP.             */
                /* Updated: 2015 Aug 31                            */
                if (osf2.OBS_DRX_GAIN < 16 ) {
                   gain1 = osf2.OBS_DRX_GAIN;
                   gain2 = osf2.OBS_DRX_GAIN;
                } else {
                   gain1 = (osf2.OBS_DRX_GAIN >> 4) & 0xF;
                   gain2 = osf2.OBS_DRX_GAIN & 0xF;
                }

                //printf("debug: osf2.OBS_DRX_GAIN=%hd\n",osf2.OBS_DRX_GAIN);

#if defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP
                /* ADP - DRX commands */
//     For cmd="DRX": Args are beam          1..NUM_BEAMS(16)        (uint8 DRX_BEAM)
//                             freq          [Hz]                   (float32 DRX_FREQ)
//                             ebw  	     Bandwidth setting 1..8 (unit8 DRX_BW)
//                             gain          0..15                  (uint16 DRX_GAIN)
//     NOTE: BEAM 1 is the master beam and the only beam that sets the tuning frequencies
                if ( ( (osf.OBS_FREQ1 != last_drx_freq1) || \
                       (osf.OBS_BW != last_drx_bw1) || \
                       (gain1 != last_drx_gain1) ) && \
                     (osf.SESSION_DRX_BEAM == 1) ) {
                  LWA_time2tv( &(cs[ncs].action.tv), dp_cmd_mjd, dp_cmd_mpm );
                  cs[ncs].action.tv.tv_sec -= 2; /* Must be sent in slot N-4 instead of N-2 */
                  cs[ncs].action.bASAP = 0;
                  cs[ncs].action.sid = LWA_SID_ADP;  
                  cs[ncs].action.cid = LWA_CMD_DRX; 
                  sprintf( cs[ncs].data, "%hd %12.3f %hu %hd",
                                  1, //tuning 1..NUM_TUNINGS(2) (uint8 DRX_TUNING)
                                        (4.563480616e-02)*(osf.OBS_FREQ1), /* center freq in Hz */
                                              osf.OBS_BW,                  /* 0-8 */
                                                  gain1);                  /* 0-15 */
                  cs[ncs].action.len = strlen(cs[ncs].data)+1; 
                  me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                  ncs++;
                  last_drx_freq1 = osf.OBS_FREQ1;
                  last_drx_bw1 = osf.OBS_BW;
                  last_drx_gain1 = gain1;
                  }
                
                // Disable the second DRX tuning for half beams
                if ( (osf.OBS_FREQ2 != 0) && \
                     ( (osf.OBS_FREQ2 != last_drx_freq2) || \
                       (osf.OBS_BW != last_drx_bw2) || \
                       (gain2 != last_drx_gain2) ) && \
                     (osf.SESSION_DRX_BEAM == 1) ) {
                  LWA_time2tv( &(cs[ncs].action.tv), dp_cmd_mjd, dp_cmd_mpm+10 ); /* staggering send times for DP commands by 10 ms */
                  cs[ncs].action.tv.tv_sec -= 2; /* Must be sent in slot N-4 instead of N-2 */
                  cs[ncs].action.bASAP = 0;                   
                  cs[ncs].action.sid = LWA_SID_ADP;  
                  cs[ncs].action.cid = LWA_CMD_DRX; 
                  sprintf( cs[ncs].data, "%hd %12.3f %hu %hd",
                                  2, //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                                        (4.563480616e-02)*(osf.OBS_FREQ2), /* center freq in Hz */
                                              osf.OBS_BW,                  /* 0-8 */
                                                  gain2);                  /* 0-15 */
                  cs[ncs].action.len = strlen(cs[ncs].data)+1;
                  me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                  ncs++;
                  last_drx_freq2 = osf.OBS_FREQ2;
                  last_drx_bw2 = osf.OBS_BW;
                  last_drx_gain2 = gain2;
                  }
#else
                /* DP - DRX commands */
//     For cmd="DRX": Args are beam          1..NUM_BEAMS(4)        (uint8 DRX_BEAM)
//                             tuning        1..NUM_TUNINGS(2)      (uint8 DRX_TUNING)
//                             freq          [Hz]                   (float32 DRX_FREQ)
//                             ebw  	     Bandwidth setting 1..7 (unit8 DRX_BW)
//                             gain          0..15                  (uint16 DRX_GAIN)
//                             subslot       0..99                  (uint8 sub_slot)
                if ( (osf.OBS_FREQ1 != last_drx_freq1) || \
                     (osf.OBS_BW != last_drx_bw1) || \
                     (gain1 != last_drx_gain1) || \
                     (osf.OBS_B != last_drx_high_dr1) ) {
                  LWA_time2tv( &(cs[ncs].action.tv), dp_cmd_mjd, dp_cmd_mpm );
                  cs[ncs].action.bASAP = 0;
              #if defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP
                  cs[ncs].action.tv.tv_sec -= 2; /* Must be sent in slot N-4 instead of N-2 */
                  cs[ncs].action.sid = LWA_SID_NDP;
                  cs[ncs].action.cid = LWA_CMD_DRX; 
                  sprintf( cs[ncs].data, "%hd 1 %12.3f %hu %hd %ld %ld %ld",
                                  osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                                      //tuning 1..NUM_TUNINGS(2) (uint8 DRX_TUNING)
                                        (4.563480616e-02)*(osf.OBS_FREQ1),                /* center freq in Hz */
                                              osf.OBS_BW,                                 /* 1-7 */
                                                  gain1,                                  /* 0-15 */
                                                      osf.OBS_B == LWA_BT_HIGH_DR ? 1 : 0,/* High DR mode*/
                                                          t0);                            /* subslot 0..99 (uint8 sub_slot) */
              #else
                  cs[ncs].action.sid = LWA_SID_DP_;
                  cs[ncs].action.cid = LWA_CMD_DRX; 
                  sprintf( cs[ncs].data, "%hd 1 %12.3f %hu %hd %ld",
                                  osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                                      //tuning 1..NUM_TUNINGS(2) (uint8 DRX_TUNING)
                                        (4.563480616e-02)*(osf.OBS_FREQ1), /* center freq in Hz */
                                              osf.OBS_BW,                  /* 1-7 */
                                                  gain1,                   /* 0-15 */
                                                      t0);                 /* subslot 0..99 (uint8 sub_slot) */
              #endif
                  cs[ncs].action.len = strlen(cs[ncs].data)+1; 
                  me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                  ncs++;
                  last_drx_freq1 = osf.OBS_FREQ1;
                  last_drx_bw1 = osf.OBS_BW;
                  last_drx_gain1 = gain1;
                  last_drx_high_dr1 = osf.OBS_B;
                  }

                // Disable the second DRX tuning for half beams
                if ( (osf.OBS_FREQ2 != 0) && \
                     ( (osf.OBS_FREQ2 != last_drx_freq2) || \
                       (osf.OBS_BW != last_drx_bw2) || \
                       (gain2 != last_drx_gain2) || \
                       (osf.OBS_B != last_drx_high_dr2) ) ) {
                  LWA_time2tv( &(cs[ncs].action.tv), dp_cmd_mjd, dp_cmd_mpm+10 ); /* staggering send times for DP commands by 10 ms */
                  cs[ncs].action.bASAP = 0;                   
              #if defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP
                  cs[ncs].action.tv.tv_sec -= 2; /* Must be sent in slot N-4 instead of N-2 */
                  cs[ncs].action.sid = LWA_SID_NDP;
                  cs[ncs].action.cid = LWA_CMD_DRX; 
                  sprintf( cs[ncs].data, "%hd 2 %12.3f %hu %hd %ld %ld",
                                  osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                                      //tuning 1..NUM_TUNINGS(2) (uint8 DRX_TUNING)
                                        (4.563480616e-02)*(osf.OBS_FREQ2),                /* center freq in Hz */
                                              osf.OBS_BW,                                 /* 1-7 */
                                                  gain2,                                  /* 0-15 */
                                                      osf.OBS_B == LWA_BT_HIGH_DR ? 1 : 0,/* High DR mode*/
                                                          t0);                            /* subslot 0..99 (uint8 sub_slot) */
              #else
                  cs[ncs].action.sid = LWA_SID_DP_;  
                  cs[ncs].action.cid = LWA_CMD_DRX; 
                  sprintf( cs[ncs].data, "%hd 2 %12.3f %hu %hd %ld",
                                  osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                                      //tuning 1..NUM_TUNINGS(2) (uint8 DRX_TUNING)
                                        (4.563480616e-02)*(osf.OBS_FREQ2), /* center freq in Hz */
                                              osf.OBS_BW,                  /* 1-7 */
                                                  gain2,                   /* 0-15 */
                                                      t0);                 /* subslot 0..99 (uint8 sub_slot) */
              #endif
                  cs[ncs].action.len = strlen(cs[ncs].data)+1;
                  me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                  ncs++;
                  last_drx_freq2 = osf.OBS_FREQ2;
                  last_drx_bw2 = osf.OBS_BW;
                  last_drx_gain2 = gain2;
                  last_drx_high_dr2 = osf.OBS_B;
                  }
#endif
                
                /*--- BAM commands ---*/

                /* figure out when to stop doing BAM updates */
                LWA_time2tv( &tv2, osf.OBS_START_MJD, osf.OBS_START_MPM ); 
                LWA_timeadd( &tv2, osf.OBS_DUR );

                /* initializing */
                LWA_time2tv( &tv, osf.OBS_START_MJD, osf.OBS_START_MPM );
                last_alt = 0.0;
                last_az = 0.0;
                bFirst = 1;  

                /* Figure out what gfile to use (construct filename) */
                //sprintf(gfile,"gfile.gf"); 
                //sprintf(gfile,"111226_XY.gf"); 
                sprintf(gfile,"default.gf"); /* this will be overwritten if OBS_BDM keyword is invoked (see below) */
                /* 140310: Adding support for OBS_BDM keyword */
                if (strlen(osf.OBS_BDM)>0) {
                  me_trim(osf.PROJECT_ID,sProjectIDtrimmed);
                  //sprintf(gfile,"c%s_%04u_%04u.gf",osf.PROJECT_ID,osf.SESSION_ID,osf.OBS_ID);
                  sprintf(gfile,"c%s_%04u_%04u.gf",sProjectIDtrimmed,osf.SESSION_ID,osf.OBS_ID);
                  me_bdm_setup( osf.OBS_BDM, fpl, gfile ); 
                  } 
      
                /* looping over time */
                while ( LWA_timediff( tv2, tv ) > 0 ) {

                  LWA_timeval(&tv,&mjd,&mpm); /* get current MJD/MPM */

                  /* If this is TRK_SOL or TRK_JOV, we need to get RA and DEC first  */
                  /* Otherwise we need to take the J2000.0 coordiantes and move them */
                  /* into geocentric apparent coordiantes at the epoch of date.      */
                  /* Updated: 2015 Aug 31                                            */
                  switch (osf.OBS_MODE) {
                    case LWA_OM_TRK_SOL:
                      me_findsol( mjd, mpm, &ra, &dec, &dist );
                      osf.OBS_RA = (float) ra;
                      osf.OBS_DEC = (float) dec;
	                    break;  
                    case LWA_OM_TRK_JOV:
                      me_findjov( mjd, mpm, &ra, &dec, &dist );
                      osf.OBS_RA = (float) ra;
                      osf.OBS_DEC = (float) dec;
                      break;
                    case LWA_OM_TRK_LUN:
                      me_findlun( mjd, mpm, &ra, &dec, &dist );
                      osf.OBS_RA = (float) ra;
                      osf.OBS_DEC = (float) dec;
                      break;
                    case LWA_OM_TRK_RADEC:
                      ra = osf.OBS_RA;
                      dec = osf.OBS_DEC;
                      me_precess( mjd, mpm, &ra, &dec );
                      dist = 1e10;
                      break;
                    default: break;
                    }

                  /* get updated alt/az */
                  me_getaltaz( ra, 
                               dec, 
                               dist, 
                               mjd, mpm, 
                               s.fGeoN, s.fGeoE, s.fGeoEl, 
                               &last, &alt, &az ); /* alt and az are in degrees */                    
                  /* pointing correction */
                  me_point_corr( s.fPCAxisTh, s.fPCAxisPh, s.fPCRot, &alt, &az );                    

                  //printf("alt=%f last_alt=%f %f | az=%f last_az=%f %f\n",alt,last_alt,angle_sep(alt,last_alt,360.0),az,last_az,angle_sep(alt,last_alt,360.0));
                  if ( (angle_sep_2d(az,alt,last_az,last_alt)>=LWA_RES_DEG) || bFirst ) {

                    bFirst = 0;

                    /* Trigger time is in units of "subslots" (1/100ths of a second) */
                    t0 = mpm % 1000; /* number of ms beyond a second boundary */
                    t0 /= 10; if (t0>99) t0=99; /* now in subslots */

                    /* Figure out what dfile to use (construct filename) */
                    sprintf(dfile,"740_%03.0lf_%04.0lf.df",alt*10,az*10); /* FIXME */
                    //sprintf(dfile,"dfile.df"); 
                    
#if defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP
                    /* Must be sent in first 80% of slot N-2 */
                    cs[ncs].action.tv.tv_sec  = tv.tv_sec - 2;
                    cs[ncs].action.tv.tv_usec = 20000; /* staggering send times for DP commands by 10 ms */
                    cs[ncs].action.bASAP = 0;
                    cs[ncs].action.sid = LWA_SID_ADP;  
                    cs[ncs].action.cid = LWA_CMD_BAM; 
                    sprintf( cs[ncs].data, "%hd %s %s %hd %ld",
                                    osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(1) (uint8 DRX_BEAM)
                                        dfile,
                                           gfile,
                                              1, 
                                                 t0);
                    cs[ncs].action.len = strlen(cs[ncs].data)+1; 
                    me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                    ncs++;
                    
                    cs[ncs].action.tv.tv_sec  = tv.tv_sec - 2;
                    cs[ncs].action.tv.tv_usec = 30000; /* staggering send times for DP commands by 10 ms */
                    cs[ncs].action.bASAP = 0;
                    cs[ncs].action.sid = LWA_SID_ADP;  
                    cs[ncs].action.cid = LWA_CMD_BAM; 
                    sprintf( cs[ncs].data, "%hd %s %s %hd %ld",
                                    osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(1) (uint8 DRX_BEAM)
                                        dfile,
                                           gfile,
                                              2, 
                                                 t0);
                    cs[ncs].action.len = strlen(cs[ncs].data)+1; 
                    me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                    ncs++; 
#else
                    /* Must be sent in first 80% of slot N-2 */
                    cs[ncs].action.tv.tv_sec  = tv.tv_sec - 2;
                    cs[ncs].action.tv.tv_usec = 20000; /* staggering send times for DP commands by 10 ms */
                    cs[ncs].action.bASAP = 0;
                    #if defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP
                    cs[ncs].action.sid = LWA_SID_NDP;
                    #else
                    cs[ncs].action.sid = LWA_SID_DP_; 
                    #endif 
                    cs[ncs].action.cid = LWA_CMD_BAM; 
                    sprintf( cs[ncs].data, "%hd %s %s %ld",
                                    osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                                        dfile,
                                           gfile,
                                              t0);
                    cs[ncs].action.len = strlen(cs[ncs].data)+1; 
                    me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                    ncs++; 
#endif
                    
                    last_alt = alt;
                    last_az  = az;

                    } /* if ( (angle_sep_2d */

                  LWA_timeadd(&tv,LWA_REPOINT_CHECK_INTERVAL_SEC*1000);

                  } /* while ( LWA_timediff( tv2, tv ) > 0 )  */

                break; /* LWA_OM_TRK_RADEC */ 

              case LWA_OM_STEPPED:
                /* this is done in a separate pass */
                break;

              default:
#if defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP
                printf(     "[%d/%d] FATAL: During NDP setup, osf.OBS_MODE=%d not recognized\n",ME_INPROC,getpid(),osf.OBS_MODE);
                fprintf(fpl,"[%d/%d] FATAL: During NDP setup, osf.OBS_MODE=%d not recognized\n",ME_INPROC,getpid(),osf.OBS_MODE);
#elif defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP
                printf(     "[%d/%d] FATAL: During ADP setup, osf.OBS_MODE=%d not recognized\n",ME_INPROC,getpid(),osf.OBS_MODE);
                fprintf(fpl,"[%d/%d] FATAL: During ADP setup, osf.OBS_MODE=%d not recognized\n",ME_INPROC,getpid(),osf.OBS_MODE);
#else
                printf(     "[%d/%d] FATAL: During DP setup, osf.OBS_MODE=%d not recognized\n",ME_INPROC,getpid(),osf.OBS_MODE);
                fprintf(fpl,"[%d/%d] FATAL: During DP setup, osf.OBS_MODE=%d not recognized\n",ME_INPROC,getpid(),osf.OBS_MODE);
#endif
                closedir(dir);
                if( fp != NULL ) {
                  fclose(fp);
                  fp = NULL;
                }
                if( fpl != NULL ) {
                  fclose(fpl);
                  fpl = NULL;
                }
                if( fpc != NULL ) {
                  fclose(fpc);
                  fpc = NULL;
                }
                if( fpo != NULL ) {
                  fclose(fpo);
                  fpo = NULL;
                } 
                exit(EXIT_FAILURE); 
                break;
              } /* switch (osf.OBS_MODE) */

            } /* if (eD==0) */

//  unsigned long int  OBS_START_MJD;
//  unsigned long int  OBS_START_MPM;
//  unsigned long int  OBS_DUR;
//  unsigned short int OBS_MODE;
//  float              OBS_RA;
//  float              OBS_DEC;
//  unsigned short int OBS_B;
//  unsigned int       OBS_FREQ1;
//  unsigned int       OBS_FREQ2;
//  unsigned short int OBS_BW;
//  unsigned int       OBS_STP_N;
//  unsigned short int OBS_STP_RADEC;
//...
//  signed short int   OBS_FEE[LWA_MAX_NSTD][2];
//  signed short int   OBS_ASP_FLT[LWA_MAX_NSTD];
//  signed short int   OBS_ASP_AT1[LWA_MAX_NSTD];
//  signed short int   OBS_ASP_AT2[LWA_MAX_NSTD];
//  signed short int   OBS_ASP_ATS[LWA_MAX_NSTD];
//  unsigned short int OBS_TBW_BITS;
//  unsigned int       OBS_TBW_SAMPLES;
//  signed short int   OBS_TBN_GAIN;
//  signed short int   OBS_DRX_GAIN;

          /*******************************************************/
          /*******************************************************/
          /*** SECOND PASS THROUGH FILE **************************/
          /*******************************************************/
          /*******************************************************/

          /* open the .obs file */
          sprintf(osf_filename,"sinbox/%s_%04d_%04d.obs",ssf.PROJECT_ID,ssf.SESSION_ID,i);
          if ((fpo = fopen(osf_filename,"rb"))==NULL) {
            fprintf(fpl,"[%d/%d] FATAL: me_inproc can't open '%s'\n",ME_INPROC,getpid(),osf_filename);
            printf(     "[%d/%d] FATAL: me_inproc can't open '%s'\n",ME_INPROC,getpid(),osf_filename);
            closedir(dir);
            if( fp != NULL ) {
              fclose(fp);
              fp = NULL;
            }
            if( fpl != NULL ) {
              fclose(fpl);
              fpl = NULL;
            }
            if( fpc != NULL ) {
              fclose(fpc);
              fpc = NULL;
            }
            if( fpo != NULL ) {
              fclose(fpo);
              fpo = NULL;
            }           
            exit(EXIT_FAILURE);   
            }
          fprintf(fpl,"opened '%s' (second pass)\n",osf_filename);

          /*-------------------------------------------------*/
          /*--- fast forward to stepped observations part ---*/
          /*-------------------------------------------------*/
          m = fread( &osf, sizeof(struct osf_struct), 1, fpo ); /* this also reloads osf.* */

          /* If this is a STEPPED-mode observation, deal with DRX gain setting */  
          if ( (osf.OBS_MODE==LWA_OM_STEPPED) && (eD==0) ) {               
            if (osf2.OBS_DRX_GAIN<0) { osf2.OBS_DRX_GAIN = 6; }
            
            /* Unpack the osf2.OBS_DRX_GAIN value to allow two */
            /* different gains to be used with DP.             */
            /* Updated: 2015 Aug 31                            */
            if (osf2.OBS_DRX_GAIN < 16 ) {
               gain1 = osf2.OBS_DRX_GAIN;
               gain2 = osf2.OBS_DRX_GAIN;
            } else {
               gain1 = (osf2.OBS_DRX_GAIN >> 4) & 0xF;
               gain2 = osf2.OBS_DRX_GAIN & 0xF;
            }
                
            } /* if (eD==0) */

          /* initialize tv to observation start time */
          LWA_time2tv( &tv, osf.OBS_START_MJD, osf.OBS_START_MPM ); 

          /* If this is a STEPPED-mode observation, we have a set of { osfs_struct, beam }'s for each step */
          for ( m=1; m<=osf.OBS_STP_N; m++ ) {

            fread(&osfs,sizeof(struct osfs_struct),1,fpo);
            //osfs.OBS_STP_C1    = obs[n].OBS_STP_C1[m];
            //osfs.OBS_STP_C2    = obs[n].OBS_STP_C2[m];
            //osfs.OBS_STP_T     = obs[n].OBS_STP_T[m];
            //osfs.OBS_STP_FREQ1 = obs[n].OBS_STP_FREQ1[m];
            //osfs.OBS_STP_FREQ2 = obs[n].OBS_STP_FREQ2[m];
            //osfs.OBS_STP_B     = obs[n].OBS_STP_B[m];
            LWA_timeval( &tv, &mjd, &mpm ); /* get MJD and MPM for start of this step */
            
            if (eD==0) {
              /*=== BEGIN: STEPPED-mode processing added 120929 ==============================================*/

#if defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP
              /* Need to figure out what ADP subslot this corresponds to */
              
              /* NOTE: BEAM 1 is the master beam and the only beam that sets 
               * the tuning frequencies */
              
              if ( ( (osfs.OBS_STP_FREQ1 != last_drx_freq1) || \
                     (osf.OBS_BW != last_drx_bw1) || \
                     (gain1 != last_drx_gain1) ) && \
                   (osf.SESSION_DRX_BEAM == 1) ) {
                t0 = mpm % 1000;                /* number of ms beyond a second boundary */
                t0 /= 10; if (t0>99) t0=99;     /* now in subslots */                  
              
                /* here's the DRX command setting FREQ1: */
                cs[ncs].action.tv.tv_sec  = tv.tv_sec - 4; /* Must be sent in first 80% of slot N-4 */
                cs[ncs].action.tv.tv_usec = 0; 
                cs[ncs].action.bASAP = 0; 
                cs[ncs].action.sid = LWA_SID_ADP;  
                cs[ncs].action.cid = LWA_CMD_DRX; 
                sprintf( cs[ncs].data, "%hd %12.3f %hu %hd",
                                1, //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                                      (4.563480616e-02)*(osfs.OBS_STP_FREQ1), /* center freq in Hz */
                                            osf.OBS_BW,                       /* 0-8 */
                                                gain1);                       /* 0-15 */
                cs[ncs].action.len = strlen(cs[ncs].data)+1; 
                me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                ncs++;
                last_drx_freq1 = osfs.OBS_STP_FREQ1;
                last_drx_bw1 = osf.OBS_BW;
                last_drx_gain1 = gain1;
                }
              
              /* here's the DRX command setting FREQ2 (if it hasn't been disabled): */
              if ( (osfs.OBS_STP_FREQ2 != 0) && \
                   ( (osfs.OBS_STP_FREQ2 != last_drx_freq2) || \
                     (osf.OBS_BW != last_drx_bw2) || \
                     (gain2 != last_drx_gain2) ) && \
                   (osf.SESSION_DRX_BEAM == 1) ) {
                cs[ncs].action.tv.tv_sec  = tv.tv_sec - 4; /* Must be sent in first 80% of slot N-4 */
                cs[ncs].action.tv.tv_usec = 10000;         /* staggering send times for DP commands by 10 ms */
                cs[ncs].action.bASAP = 0;    
                cs[ncs].action.sid = LWA_SID_ADP;  
                cs[ncs].action.cid = LWA_CMD_DRX; 
                sprintf( cs[ncs].data, "%hd %12.3f %hu %hd",
                                2, //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                                      (4.563480616e-02)*(osfs.OBS_STP_FREQ2), /* center freq in Hz */
                                            osf.OBS_BW,                       /* 0-8 */
                                                gain2);                       /* 0-15 */
                cs[ncs].action.len = strlen(cs[ncs].data)+1;
                me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                ncs++;
                last_drx_freq2 = osfs.OBS_STP_FREQ2;
                last_drx_bw2 = osf.OBS_BW;
                last_drx_gain2 = gain2;
                }
#else
              /* Need to figure out what DP subslot this corresponds to */
              if ( (osfs.OBS_STP_FREQ1 != last_drx_freq1) || \
                   (osf.OBS_BW != last_drx_bw1) || \
                   (gain1 != last_drx_gain1) || \
                   (osfs.OBS_STP_B != last_drx_high_dr1) ) {
                t0 = mpm % 1000;                /* number of ms beyond a second boundary */
                t0 /= 10; if (t0>99) t0=99;     /* now in subslots */                  
                
                /* here's the DRX command setting FREQ1: */
                cs[ncs].action.tv.tv_sec  = tv.tv_sec - 2; /* Must be sent in first 80% of slot N-2 */
                cs[ncs].action.tv.tv_usec = 0; 
                cs[ncs].action.bASAP = 0;
            #if defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP
                cs[ncs].action.tv.tv_sec  = tv.tv_sec - 4; /* Must be sent in first 80% of slot N-4 */
                cs[ncs].action.sid = LWA_SID_NDP;
                cs[ncs].action.cid = LWA_CMD_DRX; 
                sprintf( cs[ncs].data, "%hd 1 %12.3f %hu %hd %ld %ld",
                                osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                                    //tuning 1..NUM_TUNINGS(2) (uint8 DRX_TUNING)
                                      (4.563480616e-02)*(osfs.OBS_STP_FREQ1),                /* center freq in Hz */
                                            osf.OBS_BW,                                      /* 1-7 */
                                                gain1,                                       /* 0-15 */
                                                    osfs.OBS_STP_B == LWA_BT_HIGH_DR ? 1 : 0,/* High DR mode*/
                                                    t0);                                     // subslot 0..99 (uint8 sub_slot)
            #else
                cs[ncs].action.sid = LWA_SID_DP_;  
                cs[ncs].action.cid = LWA_CMD_DRX; 
                sprintf( cs[ncs].data, "%hd 1 %12.3f %hu %hd %ld",
                                osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                                    //tuning 1..NUM_TUNINGS(2) (uint8 DRX_TUNING)
                                      (4.563480616e-02)*(osfs.OBS_STP_FREQ1), /* center freq in Hz */
                                            osf.OBS_BW,                  /* 1-7 */
                                                gain1,                   /* 0-15 */
                                                    t0);                 // subslot 0..99 (uint8 sub_slot)
            #endif
                cs[ncs].action.len = strlen(cs[ncs].data)+1; 
                me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                ncs++;
                last_drx_freq1 = osfs.OBS_STP_FREQ1;
                last_drx_bw1 = osf.OBS_BW;
                last_drx_gain1 = gain1;
                last_drx_high_dr1 = osfs.OBS_STP_B;
                }
              
              /* here's the DRX command setting FREQ2 (if it hasn't been disabled): */
              if ( (osfs.OBS_STP_FREQ2 != 0) && \
                   ( (osfs.OBS_STP_FREQ2 != last_drx_freq2) || \
                     (osf.OBS_BW != last_drx_bw2) || \
                     (gain2 != last_drx_gain2) || \
                     (osfs.OBS_STP_B != last_drx_high_dr2) ) ) {
                cs[ncs].action.tv.tv_sec  = tv.tv_sec - 2; /* Must be sent in first 80% of slot N-2 */
                cs[ncs].action.tv.tv_usec = 10000;         /* staggering send times for DP commands by 10 ms */
                cs[ncs].action.bASAP = 0;
            #if defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP
                cs[ncs].action.tv.tv_sec  = tv.tv_sec - 4; /* Must be sent in first 80% of slot N-4 */
                cs[ncs].action.sid = LWA_SID_NDP;
                cs[ncs].action.cid = LWA_CMD_DRX; 
                sprintf( cs[ncs].data, "%hd 2 %12.3f %hu %hd %ld %ld",
                                osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                                    //tuning 1..NUM_TUNINGS(2) (uint8 DRX_TUNING)
                                      (4.563480616e-02)*(osfs.OBS_STP_FREQ2),                /* center freq in Hz */
                                            osf.OBS_BW,                                      /* 1-7 */
                                                gain2,                                       /* 0-15 */
                                                    osfs.OBS_STP_B == LWA_BT_HIGH_DR ? 1 : 0,/* High DR mode*/
                                                    t0);                                     // subslot 0..99 (uint8 sub_slot)
            #else
                cs[ncs].action.sid = LWA_SID_DP_;  
                cs[ncs].action.cid = LWA_CMD_DRX; 
                sprintf( cs[ncs].data, "%hd 2 %12.3f %hu %hd %ld",
                                osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                                    //tuning 1..NUM_TUNINGS(2) (uint8 DRX_TUNING)
                                      (4.563480616e-02)*(osfs.OBS_STP_FREQ2), /* center freq in Hz */
                                            osf.OBS_BW,                  /* 1-7 */
                                                gain2,                   /* 0-15 */
                                                    t0);                 // subslot 0..99 (uint8 sub_slot)
            #endif
                cs[ncs].action.len = strlen(cs[ncs].data)+1;
                me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                ncs++;
                last_drx_freq2 = osfs.OBS_STP_FREQ2;
                last_drx_bw2 = osf.OBS_BW;
                last_drx_gain2 = gain2;
                last_drx_high_dr2 = osfs.OBS_STP_B;
                }
#endif
              
              /* working out dfile and gfile for BAM command.  Two possibilities: */
              if ( osfs.OBS_STP_B!=LWA_BT_SPEC_DELAYS_GAINS) { 

                /* --- SIMPLE beamforming: -----------------------------------------------------*/

                /* Getting alt, az for beam pointing: */
                if (osf.OBS_STP_RADEC) { /* osfs.OBS_STP_C1,C2 represent RA/DEC */
                    /* If we've been given RA and dec we need to take the J2000.0 coordinates */
                    /* and move them into geocentric apparent coordiantes at the epoch of     */
                    /* date.                                                                  */
                    /* Updated: 2015 Aug 31                                                   */
                    ra = osfs.OBS_STP_C1;
                    dec = osfs.OBS_STP_C2;
                    me_precess( mjd, mpm, &ra, &dec);
                    dist = 1e10; 
                    me_getaltaz( ra, 
                                 dec, 
                                 dist, 
                                 mjd, mpm, 
                                 s.fGeoN, s.fGeoE, s.fGeoEl, 
                                 &last, &alt, &az ); /* alt and az are in degrees */  
                  } else {
                    az  = osfs.OBS_STP_C1;
                    alt = osfs.OBS_STP_C2;
                  } /* if (osf.OBS_STP_RADEC) */
                me_point_corr( s.fPCAxisTh, s.fPCAxisPh, s.fPCRot, &alt, &az ); /* pointing correction */

                /* Figure out what dfile to use (construct filename) */
                sprintf(dfile,"740_%03.0lf_%04.0lf.df",alt*10,az*10); 
 
                /* Figure out what gfile to use (construct filename) */
                sprintf(gfile,"default.gf"); 
                if (strlen(osf.OBS_BDM)>0) {
                  me_trim(osf.PROJECT_ID,sProjectIDtrimmed);
                  //sprintf(gfile,"c%s_%04u_%04u_%04d.gf",osf.PROJECT_ID,osf.SESSION_ID,osf.OBS_ID,m);
                  sprintf(gfile,"c%s_%04u_%04u_%04d.gf",sProjectIDtrimmed,osf.SESSION_ID,osf.OBS_ID,m);
                  me_bdm_setup( osf.OBS_BDM, fpl, gfile ); 
                  } 

                } else { 

                /* --- SPEC_DELAYS_GAINS beamforming: ---------------------------------------*/

                //printf("FATAL: Shouldn't be here!"); exit(EXIT_FAILURE);

                /* come up with unique root filename for delay and gain files */ 
                /* These start with "c" to denote "custom" -- this keeps me_beamspec() from getting confused */
                me_trim(osf.PROJECT_ID,sProjectIDtrimmed);
                //sprintf(dfile,"c%s_%04u_%04u_%04d.df",osf.PROJECT_ID,osf.SESSION_ID,osf.OBS_ID,m);
                //sprintf(gfile,"c%s_%04u_%04u_%04d.gf",osf.PROJECT_ID,osf.SESSION_ID,osf.OBS_ID,m);
                sprintf(dfile,"c%s_%04u_%04u_%04d.df",sProjectIDtrimmed,osf.SESSION_ID,osf.OBS_ID,m);
                sprintf(gfile,"c%s_%04u_%04u_%04d.gf",sProjectIDtrimmed,osf.SESSION_ID,osf.OBS_ID,m);

                /* read custom delays and gains */
                fread(&beam,sizeof(struct beam_struct),1,fpo);

                me_medfg(beam,dfile,fpl); /* make delay file and deliver to sch */
                me_megfg(beam,gfile,fpl); /* make gain file and deliver to sch */

                } /* if ( osfs.OBS_STP_B!=LWA_BT_SPEC_DELAYS_GAINS) {} else {} */


#if defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP
              /* Trigger time is in units of "subslots" (1/100ths of a second) */
              t0 = mpm % 1000;                /* number of ms beyond a second boundary */
              t0 /= 10; if (t0>99) t0=99;     /* now in subslots */
              /* so here's the BAM command: */
              cs[ncs].action.tv.tv_sec  = tv.tv_sec - 2; /* Must be sent in first 80% of slot N-2 */
              cs[ncs].action.tv.tv_usec = 20000;         /* staggering send times for DP commands by 10 ms */
              cs[ncs].action.bASAP = 0;
              cs[ncs].action.sid = LWA_SID_ADP;  
              cs[ncs].action.cid = LWA_CMD_BAM; 
              sprintf( cs[ncs].data, "%hd %s %s %d %ld",
                                      osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(1) (uint8 DRX_BEAM)
                                          dfile,
                                             gfile,
                                                1, 
                                                   t0);
              cs[ncs].action.len = strlen(cs[ncs].data)+1;
              me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
              ncs++;
              
              /* so here's the other BAM command: */
              cs[ncs].action.tv.tv_sec  = tv.tv_sec - 2; /* Must be sent in first 80% of slot N-2 */
              cs[ncs].action.tv.tv_usec = 30000;         /* staggering send times for DP commands by 10 ms */
              cs[ncs].action.bASAP = 0;
              cs[ncs].action.sid = LWA_SID_ADP;  
              cs[ncs].action.cid = LWA_CMD_BAM; 
              sprintf( cs[ncs].data, "%hd %s %s %d %ld",
                                      osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(1) (uint8 DRX_BEAM)
                                          dfile,
                                             gfile,
                                                2, 
                                                   t0);
              cs[ncs].action.len = strlen(cs[ncs].data)+1;
              me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
              ncs++;
#else
              /* Trigger time is in units of "subslots" (1/100ths of a second) */
              t0 = mpm % 1000;                /* number of ms beyond a second boundary */
              t0 /= 10; if (t0>99) t0=99;     /* now in subslots */
              /* so here's the BAM command: */
              cs[ncs].action.tv.tv_sec  = tv.tv_sec - 2; /* Must be sent in first 80% of slot N-2 */
              cs[ncs].action.tv.tv_usec = 20000;         /* staggering send times for DP commands by 10 ms */
              cs[ncs].action.bASAP = 0;
              #if defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP
              cs[ncs].action.sid = LWA_SID_NDP;
              #else
              cs[ncs].action.sid = LWA_SID_DP_;  
              #endif
              cs[ncs].action.cid = LWA_CMD_BAM; 
              sprintf( cs[ncs].data, "%hd %s %s %ld",
                                      osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                                          dfile,
                                             gfile,
                                                t0);
              cs[ncs].action.len = strlen(cs[ncs].data)+1;
              me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
              ncs++;
#endif
              
              /* determine the absolute start time for NEXT step */
              LWA_timeadd( &tv, osfs.OBS_STP_T );   

              /*=== END: STEPPED-mode processing  added 120929 ==============================================*/
              } /* if (eD==0) */

            /* reading marker at end of data that should have been read to this point */
            fread(&u4,sizeof(u4),1,fpo);
            //u4 = 4294967294; fwrite(&u4, sizeof(u4),1,fp); /* = $2^{32}-2$. */
            if ( u4 != 4294967294 ) {
              fprintf(fpl,"[%d/%d] FATAL: me_inproc doesn't see '2^32-2' marker\n",ME_INPROC,getpid()); 
              printf(     "[%d/%d] FATAL: me_inproc doesn't see '2^32-2' marker\n",ME_INPROC,getpid());
              closedir(dir); 
              if( fp != NULL ) {
                fclose(fp);
                fp = NULL;
              }
              if( fpl != NULL ) {
                fclose(fpl);
                fpl = NULL;
              }
              if( fpc != NULL ) {
                fclose(fpc);
                fpc = NULL;
              }
              if( fpo != NULL ) {
                fclose(fpo);
                fpo = NULL;
              }         
              exit(EXIT_FAILURE);
              }

            } /* for m */

          /* write OBE ("observation end") command */
          if (bOBS) { /* only do this if we wrote an "OBS" */
            bOBS=0;
            LWA_time2tv( &(cs[ncs].action.tv), osf.OBS_START_MJD, osf.OBS_START_MPM ); 
            LWA_timeadd( &(cs[ncs].action.tv), osf.OBS_DUR );
            cs[ncs].action.bASAP = 0;                   
            cs[ncs].action.sid = LWA_SID_MCS;  
            cs[ncs].action.cid = LWA_CMD_OBE; 
            cs[ncs].action.len = 0;
            strcpy(cs[ncs].data,""); 
            me_inproc_cmd_log( fpl, &(cs[ncs]), 0 ); /* write log msg explaining command */
            ncs++;
            }

          /* close the .obs file; nothing else we need */
          fclose(fpo);
          fpo = NULL;
          fprintf(fpl,"closed '%s' (second pass)\n",osf_filename);

          /* writing out contents of osf2 structure */
          fprintf(fpl,"Writing out osf2:\n");
          for (m=0;m<LWA_MAX_NSTD;m++) {
            for (p=0;p<2;p++) {
              fprintf(fpl,"osf2.OBS_FEE[%d][%d]=%hd\n",m,p,osf2.OBS_FEE[m][p]); 
              }
            }
          for (m=0;m<LWA_MAX_NSTD;m++) { fprintf(fpl,"osf2.OBS_ASP_FLT[%d]=%hd\n",m,osf2.OBS_ASP_FLT[m]); }
          for (m=0;m<LWA_MAX_NSTD;m++) { fprintf(fpl,"osf2.OBS_ASP_AT1[%d]=%hd\n",m,osf2.OBS_ASP_AT1[m]); }
          for (m=0;m<LWA_MAX_NSTD;m++) { fprintf(fpl,"osf2.OBS_ASP_AT2[%d]=%hd\n",m,osf2.OBS_ASP_AT2[m]); }
          for (m=0;m<LWA_MAX_NSTD;m++) { fprintf(fpl,"osf2.OBS_ASP_ATS[%d]=%hd\n",m,osf2.OBS_ASP_ATS[m]); }
#if (defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP) || (defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP)
          fprintf(fpl,"osf2.OBS_TBF_SAMPLES=%u\n",osf2.OBS_TBF_SAMPLES);
          fprintf(fpl,"osf2.OBS_TBF_GAIN=%hd\n",osf2.OBS_TBF_GAIN);
#else
          fprintf(fpl,"osf2.OBS_TBW_BITS=%hu\n",osf2.OBS_TBW_BITS); 
          fprintf(fpl,"osf2.OBS_TBW_SAMPLES=%u\n",osf2.OBS_TBW_SAMPLES);   
#endif
#if !defined(LWA_BACKEND_IS_NDP) || !LWA_BACKEND_IS_NDP
          fprintf(fpl,"osf2.OBS_TBN_GAIN=%hd\n",osf2.OBS_TBN_GAIN);  
          fprintf(fpl,"osf2.OBS_DRX_GAIN=%hd\n",osf2.OBS_DRX_GAIN);
#endif
          } /* for ( i=1, i<=ssf.SESSION_NOBS; i++ ) */

        /*******************************************************/
        /***  Winding down *************************************/
        /*******************************************************/

        /* final command to tell me_exec that we're done */
        if (eD>=0) {
          /* Shut down the beams if we are done with them */
          /* Updated: 2015 Aug 31                         */
          esnTimeAdjust = 0;
          switch( osf.OBS_MODE ) {
#if (defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP) || (defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP)
              case LWA_OM_TBF:
                 last_drx_freq1 = 0;
                 last_drx_bw1 = 0;
                 last_drx_gain1 = -1;
#endif
              case LWA_OM_TRK_RADEC:
              case LWA_OM_TRK_SOL:
              case LWA_OM_TRK_JOV:
              case LWA_OM_TRK_LUN:
              case LWA_OM_STEPPED:
#if defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP
                 cs[ncs].action.tv.tv_sec  = cs[ncs-1].action.tv.tv_sec;
                 cs[ncs].action.tv.tv_usec  = cs[ncs-1].action.tv.tv_usec + 20000;
                 cs[ncs].action.sid = LWA_SID_ADP;  
                 cs[ncs].action.cid = LWA_CMD_STP; 
                 sprintf( cs[ncs].data, "BEAM%d",
                                      osf.SESSION_DRX_BEAM); //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                 cs[ncs].action.len = strlen(cs[ncs].data)+1;
                 me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                 ncs++;
                 esnTimeAdjust += 20000;
                 if( osf.SESSION_DRX_BEAM == 1 ) {
                   last_drx_freq1 = 0;
                   last_drx_bw1 = 0;
                   last_drx_gain1 = -1;
                   last_drx_freq2 = 0;
                   last_drx_bw2 = 0;
                   last_drx_gain2 = -1;
                   }
#else
                 cs[ncs].action.tv.tv_sec  = cs[ncs-1].action.tv.tv_sec;
                 cs[ncs].action.tv.tv_usec  = cs[ncs-1].action.tv.tv_usec + 20000;
                 #if defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP
                 cs[ncs].action.sid = LWA_SID_NDP;
                 #else
                 cs[ncs].action.sid = LWA_SID_DP_;  
                 #endif
                 cs[ncs].action.cid = LWA_CMD_STP; 
                 sprintf( cs[ncs].data, "BEAM%d",
                                      osf.SESSION_DRX_BEAM); //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                 cs[ncs].action.len = strlen(cs[ncs].data)+1;
                 me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                 ncs++;
                 esnTimeAdjust += 20000;
                 last_drx_freq1 = 0;
                 last_drx_bw1 = 0;
                 last_drx_gain1 = -1;
                 last_drx_freq2 = 0;
                 last_drx_bw2 = 0;
                 last_drx_gain2 = -1;
#endif
                 break;
              default: break;
              }
              
          cs[ncs].action.tv.tv_sec   = cs[ncs-1].action.tv.tv_sec + 1;
          cs[ncs].action.tv.tv_usec  = cs[ncs-1].action.tv.tv_usec - esnTimeAdjust;
          cs[ncs].action.bASAP = 0;                   
          cs[ncs].action.sid = LWA_SID_MCS;           /* first command is always directed to MCS */
          cs[ncs].action.cid = LWA_CMD_ESN;           /* end session normally */
          cs[ncs].action.len = 0;                     /* no additional bytes */
          strcpy(cs[ncs].data,"");
          me_inproc_cmd_log( fpl, &(cs[ncs]), 0 ); /* write log msg explaining command */
          ncs++;
          
          } /* if (eD==0) */

        /* open a command script file */
        sprintf(cs_filename,"sinbox/%s_%04u.cs",ssf.PROJECT_ID,ssf.SESSION_ID);
        fprintf(fpl,"cs_filename = '%s'\n",cs_filename); 
        if ((fpc=fopen(cs_filename,"wb"))==NULL) {
          fprintf(fpl,"[%d/%d] FATAL: me_inproc can't open '%s'\n",ME_INPROC,getpid(),cs_filename);
          printf(     "[%d/%d] FATAL: me_inproc can't open '%s'\n",ME_INPROC,getpid(),cs_filename);
          closedir(dir);
          if( fp != NULL ) {
            fclose(fp);
            fp = NULL;
          }
          if( fpl != NULL ) {
            fclose(fpl);
            fpl = NULL;
          }
          if( fpc != NULL ) {
            fclose(fpc);
            fpc = NULL;
          }
          if( fpo != NULL ) {
            fclose(fpo);
            fpo = NULL;
          }           
         exit(EXIT_FAILURE);    
          }  
        fprintf(fpl,"cs file is open\n"); 

        /*--- write command script, sorting on time order ---*/

        /* initialize key for sort */
        for (k=0;k<ncs;k++) { b[k] = 0; }

        for (i=0;i<ncs;i++) { /* we know we have to do this i times */
          
          /* find first unprocessed command and remember it's time */
          k0=0; 
          while (b[k0]) { k0++; }
          tv.tv_sec  = cs[k0].action.tv.tv_sec;
          tv.tv_usec = cs[k0].action.tv.tv_usec;

          /* sort through rest, finding soonest unprocessed command */ 
          k1=k0;
          for (k=k0+1;k<ncs;k++) {
            if (!b[k]) {
              if ( LWA_timediff( cs[k].action.tv, tv ) < 0 ) {
                k1=k;
                tv.tv_sec  = cs[k].action.tv.tv_sec;
                tv.tv_usec = cs[k].action.tv.tv_usec;
                } 
              } /* if (!b[k]) */
            } /* for k */
          b[k1]=1;
          //printf("%d ",k1);

          fwrite(&(cs[k1].action),sizeof(struct me_action_struct),1,fpc);
          if ( cs[k1].action.len > 0 ) {
            fwrite( cs[k1].data, cs[k1].action.len, 1, fpc );
            }

          } /* for i */

        /* close command script file */
        fclose(fpc);
        fpc = NULL;
        fprintf(fpl,"cs file closed\n");

        /* read through command script file and generate all beam definitions */
        /* needed by MCS/Scheduler */
        fprintf(fpl,"calling me_beamspec('%s',fpl)\n",cs_filename);
        err = me_beamspec(cs_filename,fpl);
        fprintf(fpl,"me_beamspec() returned with err=%d\n",err);

        /* delete .inp file.  This signals me_exec that we're done */
        sprintf(cmd,"rm %s",inp_filename);
        //printf("...system(%s)\n",cmd);
        fprintf(fpl,"inp file deleted\n");
        system(cmd);

        /* close log file */
        fprintf(fpl,"Closing log file. Bye.\n");
        fflush(fpl);
        fclose(fpl);
        fpl = NULL;
       
        } /* if (strstr(sDirEnt->d_name,".inp")!=NULL) */
      } /* while ( (sDirEnt=readdir(dir)) != NULL ) */

    /* close the directory */
    closedir(dir);

    /* avoiding busy wait */
    sleep(1); 
    } /* while (!bDone) */


  exit(EXIT_SUCCESS);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// me_inproc.c: J. Dowell, UNM, 2022 Sep 30
//   .1 Added support for TRK_LUN
// me_inproc.c: J. Dowell, UNM, 2020 Sep 29
//   .1 Fixed a bug in RA/Dec STEPPED mode observations that caused the start time 
//      of the steps to not be updated correctly
// me_inproc.c: J. Dowell, UNM, 2019 Dec 16
//   .1 Documentation update for ADP-based stations
// me_inproc.c: J. Dowell, UNM, 2019 Apr 3
//   .1 Fixed a bug in STEPPED observations where a OBS_DRX_GAIN of 0 was ignored
// me_inproc.c: J. Dowell, UNM, 2019 Mar 4
//   .1 Applied the 2019 Feb 25 change to the DRX command timing for ADP to STEPPED
//      mode observations as well
// me_inproc.c: J. Dowell, UNM, 2019 Feb 25
//   .1 Moved the ADP DRX and TBN commands forward by two seconds to deal with an 
//      internal ADP delay
// me_inproc.c: J. Dowell, UNM, 2019 Jan 25
//   .1 Fixed the TBF command so that the correct tuning mask is set.  Also, added
//      sending the second DRX command to set the second tuning if needed.
// me_inproc.c: J. Dowell, UNM, 2018 Mar 5
//   .1 Cleaned up a few more dual beam ADP related problems that caused BAM commands
//      to be malformed
// me_inproc.c: J. Dowell, UNM, 2018 Feb 21
//   .1 Fixed an ADP-based station problem where too many 'STP BEAM#' commands are sent
// me_inproc.c: J. Dowell, UNM, 2018 Feb 13
//   .1 Added support for a second beam for ADP-based stations
// me_inproc.c: J. Dowell, UNM, 2018 Feb 5
//   .1 Fixed a BAM command problem for ADP-based stations
// me_inproc.c: J. Dowell, UNM, 2018 Jan 29
//   .1 Added state for DRX commands so that unnecesary commands are not sent
//   .2 Cleaned up a few compiler warnings
// me_inproc.c: J. Dowell, UNM 2017 Apr 20
//   .1 Added a five second lag between a DRX command at the associated TBF command
// me_inproc.c: J. Dowell, UNM 2016 Aug 25
//   .1 Added support for single tuning "half beams" where tuning 2 is not set
// me_inproc.c: J. Dowell, UNM, 2015 Aug 31
//   .1 Added support for ADP, precession of RA/dec coordinates from J200 to EoD, 
//      support for different gain values for the two tunings in a beam.  There are
//      probably a lot of problems in this version that need to be found and fixed.
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2014 Mar 25
//   .1 In me_bdm_setup(), requested dipole overrides bad stand mask.
//   .2 In me_bdm_setup(), requested dipole is not included in beam. 
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2014 Mar 24
//   .1 In me_bdm_setup(), transposed gain matrix; looks like I had it wrong before.
//   .2 Fix for "less than 8-char project ID" issue (trimming spaces prior to forming filenames)
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2014 Mar 10
//   .1 Added support for OBS_BDM keyword
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2013 Jan 28
//   .1 Implementing custom beam delays and gains
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2012 Oct 07
//   .1 Implementing TRK_SOL and TRK_JOV
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2012 Sep 29
//   .1 Partial implementation of STEPPED mode
//      Scrambled times over 20 ms spread for DRX and BAM at start of obs or step
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2012 Jul 05
//   .1 Changed gain file included in BAM commands from "111226_XY.gf" to "default.gf"
//      (This is the "masked" gain file in which SSMIF-indicated bad stands are zeroed)
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2012 Jul 02
//   .1 Implemented DRX beam pointing correction
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2012 Jan 24
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2012 Feb 16
//   .1 Added support for SESSION_SPC keyword
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2012 Jan 24
//   .1 Now working generating a list of pointings and generating the associated
//      delay files all at once at the end of processing 
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2012 Jan 21
//   .1 For TRK_RADEC, now sending actual dfile and gfile names. 
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2011 Dec 21
//   .1 Rewriting command script sequencing.  Now constructing commands out as they are
//      worked out, and then fixing sequencing as a separate step
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2011 Dec 19
//   .1 Continuing to improve sequencing and timing of commands.  Added ChkSndOBE().
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2011 Dec 18
//   .1  Improving sequencing and timing of commands
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2011 Apr 16
//   .1  Now selects correct DR based on SSMIF
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2011 Apr 12
//   .1: Adding TBN mode
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2011 Apr 07
//   .1: Adding TBW mode
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2011 Mar 31
//   .1: Rearranged flow
// me_inproc.c: S.W. Ellingson, Virginia Tech, 2011 Mar 17
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
