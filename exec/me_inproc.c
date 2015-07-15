// me_inproc.c: S.W. Ellingson, Virginia Tech, 2012 Jan 24
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

#include <math.h>        /* needed for me_getaltaz.c (at least) */
#include <dirent.h>      /* this is listing files in a directory */

#include "me.h"
#include "me_getaltaz.c" /* astro coordinate routine */

#define ME_INPROC_DATA_LEN 256   /* size of the "data" string associated with each */
                                 /* command in the command script */
#define ME_INPROC_MAX_CMDS 16384 /* max number of commands */

struct cs_command {        /* one command in the command script */
  struct me_action_struct action;
  char data[ME_INPROC_DATA_LEN];
  };

/*************************************************************/
/*** me_beamspec() *******************************************/
/*************************************************************/
/* Reads the command script (.cs) file. */
/* Makes a list of beam delay & gain files needed by BAM commands */
/* The list is fed to a separate utility which generates the spec files */
/* The spec files are then transferred to Scheduler */

int me_beamspec( char *cs_filename, 
                 FILE *fpl ) {
  int err = 0;
  FILE *fp;
  FILE *fpo;
  char bm_filename[1024];
  struct me_action_struct action;
  char data[16384];
  int arg1, arg4;
  float fmhz, alt, az;
  char gfname[1024];  
  char cmd[256]; 

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
    return 1;
    }

  fprintf(fpl,"me_beamspec() file reads as follows:\n");

  /* main loop */
  while( fread( &action, sizeof(struct me_action_struct), 1, fp ) > 0 ) {
 
    /* read the rest of the action */
    sprintf(data,"");
    if (action.len>0) fread( data, action.len, 1, fp );

    if ((action.sid==LWA_SID_DP_) && (action.cid==LWA_CMD_BAM)) {
      //fprintf(fpo,"'%s'\n",data);
      //'1 740_766_2071.df 111226_XY.gf 0'
      //'1 740_766_2072.df 111226_XY.gf 0'
      sscanf(data,"%d %3f_%3f_%4f.df %s %d",&arg1,&fmhz,&alt,&az,gfname,&arg4);
      fmhz /= 10;
      alt /= 10;
      az /= 10;
      fprintf(fpo,"%4.1f %5.1f\n",alt,az);
      fprintf(fpl,"%4.1f %5.1f\n",alt,az);
      }

    } /* while ( fread( */

  /* Close files */
  fclose(fp);  /* .cs */
  fclose(fpo); /* .bm */

  /* clean out the me_inproc_bm directory */
  sprintf(cmd,"rm -rf me_inproc_bm/*");
  system(cmd);

  /* Call mefsdfg to generate the files */ 
  fprintf(fpl,"me_beamspec(): system('./mefsdfg state me_inproc_bm %4.1f 2 sinbox/bm.dat')\n",fmhz);
  sprintf(cmd,"./mefsdfg state me_inproc_bm %4.1f 2 sinbox/bm.dat",fmhz);
  system(cmd);

  /* Transfer the files */
  sprintf(cmd, "scp me_inproc_bm/* %s:%s/dfiles/.",LWA_SCH_SCP_ADDR,LWA_SCH_SCP_DIR);
  fprintf(fpl,"me_beamspec(): system('%s')\n",cmd);
  //printf("'%s'\n",cmd);
  system(cmd);

  return err;
  } /* me_beamspec() */

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

  return;
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
  return;
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


/*******************************************************************/
/*** main() ********************************************************/
/*******************************************************************/

int main ( int narg, char *argv[] ) {

  /* command line parameters */
  char inp_filename[ME_FILENAME_MAX_LENGTH];

  /* other variables */

  FILE *fp;
  struct ssf_struct ssf;

  FILE *fpl; /* log file */
  char log_filename[ME_FILENAME_MAX_LENGTH];

  FILE *fpc; /* command script file */
  char cs_filename[ME_FILENAME_MAX_LENGTH];

  FILE *fpo; /* obs file */
  char osf_filename[ME_FILENAME_MAX_LENGTH];

  struct timeval tv;
  struct timeval tv2; /* used to remember observation end time while computing BAM updates */

  struct cs_command cs[ME_INPROC_MAX_CMDS];  /* the command script */
  long int ncs = 0; /* number of commands in command script */

  char cmd[ME_MAX_COMMAND_LINE_LENGTH];
  
  int bDone=0;

  DIR *dir;
  struct dirent *sDirEnt;

  int i,m,p;

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

  int dr_sid;
  long int dr_length_ms;
  char dr_format[33];

  char dfile[1024];
  char gfile[1024];

  struct ssmif_struct s; 

  long int mjd, mpm;
 
  double alt, az, last;
  double last_alt, last_az;
  int bFirst;

  int b[ME_INPROC_MAX_CMDS];
  int k,k0,k1;

  int bOBS=0;

  int err=0;

  /* First, announce thyself */
  printf("[%d/%d] I am me_inproc\n",ME_INPROC,getpid());

  /* sometimes first call to gettimeofday() returns something bogus, so let's get that out of the way */
  gettimeofday( &tv, NULL ); 

  /* get SSMIF */  
  fp=fopen("state/ssmif.dat","rb");
  fread(&s,sizeof(struct ssmif_struct),1,fp);
  fclose(fp); 

  /*****************/
  /*****************/
  /*** main loop ***/
  /*****************/
  /*****************/

  while (!bDone) {
    
    /* check for a .inp file */
    if (!(dir = opendir("sinbox"))) {
      printf("[%d/%d] FATAL: Couldn't opendir('sinbox')\n",ME_INPROC,getpid());
      return;
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
          return;
          }
        fread(&ssf,sizeof(struct ssf_struct),1,fp);
        fclose(fp);  
        //printf("...ssf.PROJECT_ID='%s'\n",ssf.PROJECT_ID);
        //return;

        /* open a log file for this session */
        sprintf(log_filename,"sinbox/%s_%04u.ipl",ssf.PROJECT_ID,ssf.SESSION_ID);
        //printf("...log_filename='%s'\n",log_filename); 
        if ((fpl = fopen(log_filename,"w"))==NULL) {
          printf("[%d/%d] FATAL: me_inproc can't open '%s'\n",ME_INPROC,getpid(),log_filename);
          closedir(dir);
          fcloseall();
          return;
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
          sprintf(cs[ncs].data,"");
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
            fcloseall();          
            return;   
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
              eD=0;
              break;
            case LWA_OM_TRK_SOL:   /* not implemented */
            case LWA_OM_TRK_JOV:   /* not implemented */
            case LWA_OM_STEPPED:   /* not implemented */
              fprintf(fpl,"Encountered an unimplemented OBS_MODE; zapping command script:\n");
              eD=-1;  
              break;
            case LWA_OM_TBW:      
              eD=0;
              break;
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
            fprintf(fpl,"cs file closed\n");
            if ((fpc=fopen(cs_filename,"wb"))==NULL) {
              fprintf(fpl,"[%d/%d] FATAL: me_inproc can't open '%s'\n",ME_INPROC,getpid(),cs_filename);
              printf(     "[%d/%d] FATAL: me_inproc can't open '%s'\n",ME_INPROC,getpid(),cs_filename);
              closedir(dir);
              fcloseall();          
              return;    
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
              fcloseall();          
              return; 
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
            fcloseall();         
            return; 
            }

          /* close the .obs file */
          fclose(fpo);
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
          sprintf(cs[ncs].data,""); 
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
            sprintf(cs[ncs].data,""); 
            me_inproc_cmd_log( fpl, &(cs[ncs]), 0 ); /* write log msg explaining command */
            ncs++;

            /* FIXME just before observation start: set OBS_FEE, OBS_ASP_* */

            /* ====================== */
            /* === DR REC command === */
            /* ====================== */

            /* for DP outputs 1-4 (beams), we do this once; i.e., one recording per session */
            /* for DP output 5 (TBN/TBW), we do a new recording for each observation */
            if ( ( (osf.SESSION_DRX_BEAM<5) && (i==1) ) || 
                 (  osf.SESSION_DRX_BEAM==5           )   ) {

              dr_sid=-1;
              if ( osf.SESSION_DRX_BEAM == s.iDRDP[0] ) dr_sid = LWA_SID_DR1;
              if ( osf.SESSION_DRX_BEAM == s.iDRDP[1] ) dr_sid = LWA_SID_DR2;
              if ( osf.SESSION_DRX_BEAM == s.iDRDP[2] ) dr_sid = LWA_SID_DR3;
              if ( osf.SESSION_DRX_BEAM == s.iDRDP[3] ) dr_sid = LWA_SID_DR4;
              if ( osf.SESSION_DRX_BEAM == s.iDRDP[4] ) dr_sid = LWA_SID_DR5;
              if (dr_sid==-1) {
                printf(     "[%d/%d] FATAL: osf.SESSION_DRX_BEAM=%d is not in s.iDRDP[0..4]\n",ME_INPROC,getpid(),osf.SESSION_DRX_BEAM); 
                fprintf(fpl,"[%d/%d] FATAL: osf.SESSION_DRX_BEAM=%d is not in s.iDRDP[0..4]\n",ME_INPROC,getpid(),osf.SESSION_DRX_BEAM);
                closedir(dir);
                fcloseall();
                return;
                }

              if (osf.SESSION_DRX_BEAM<5) {
                  dr_length_ms = ssf.SESSION_DUR; /* beam obs are recorded contiguously in one session */
                } else {
                  dr_length_ms = osf.OBS_DUR; /* each TBN/TBW observation is a separate recording */ 
                }
              sprintf(dr_format,""); 
              switch (osf.OBS_MODE) {
                case LWA_OM_TRK_RADEC: 
                case LWA_OM_TRK_SOL:   
                case LWA_OM_TRK_JOV:   
                case LWA_OM_STEPPED:   
                  sprintf(dr_format,"DRX_FILT_%1hu",osf.OBS_BW); 
                  break;
                case LWA_OM_TBW:       
                  sprintf(dr_format,"DEFAULT_TBW"); 
                  break;
                case LWA_OM_TBN:       
                  sprintf(dr_format,"DEFAULT_TBN"); 
                  break;
                case LWA_OM_DIAG1:
                  printf(     "[%d/%d] DR setup: osf.OBS_MODE=%d: How'd I get here?\n",ME_INPROC,getpid(),osf.OBS_MODE);
                  fprintf(fpl,"[%d/%d] DR setup: osf.OBS_MODE=%d: How'd I get here?\n",ME_INPROC,getpid(),osf.OBS_MODE);
                default:
                  printf(     "[%d/%d] FATAL: During DR setup, osf.OBS_MODE=%d not recognized\n",ME_INPROC,getpid(),osf.OBS_MODE);
                  fprintf(fpl,"[%d/%d] FATAL: During DR setup, osf.OBS_MODE=%d not recognized\n",ME_INPROC,getpid(),osf.OBS_MODE);
                  closedir(dir);
                  fcloseall();
                  return;  
                  break;
                }

              /* Figure out when to send command to DR */
              me_timecalc( osf.OBS_START_MJD, osf.OBS_START_MPM, /* calc time to send command to DR */
                          -LWA_SESS_DRDP_INIT_TIME_MS,
                           &mjd, &mpm );  
              LWA_time2tv( &tv, mjd, mpm );

              /* construct the command */
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

            /* ================== */
            /* === DP command === */
            /* ================== */

            /* DP commands neet to be sent in first 80% of slot N-2, given start time in slot N */
            me_timecalc( osf.OBS_START_MJD, osf.OBS_START_MPM, /* calc time to send command to DP */
                        -2000,
                         &dp_cmd_mjd, &dp_cmd_mpm );  

            switch (osf.OBS_MODE) {

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

              case LWA_OM_TBN:

                /* TBN trigger time is in units of "subslots" (1/100ths of a second) */
                t0 = osf.OBS_START_MPM % 1000; /* number of ms beyond a second boundary */
                t0 /= 10; if (t0>99) t0=99; /* now in subslots */
                
                ///* deal with user requests to use SSMIF-specified defaults */ 
                //if (osf2.OBS_TBN_GAIN==-1) { osf2.OBS_TBN_GAIN = s.settings.tbn_gain; }
                ///* if SSMIF also leaves it up MCS, set this to 20 */ 
                //if (osf2.OBS_TBN_GAIN==-1) { osf2.OBS_TBN_GAIN = 20; }
                osf2.OBS_TBN_GAIN = 20; /* FIXME */

                /* construct the command */ 
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

                break; /* LWA_OM_TBN */ 

              case LWA_OM_TRK_RADEC:

                /* DRX trigger time is in units of "subslots" (1/100ths of a second) */
                t0 = dp_cmd_mpm % 1000; /* number of ms beyond a second boundary */
                t0 /= 10; if (t0>99) t0=99; /* now in subslots */
                

                //printf("debug: s.settings.drx_gain=%hd\n",s.settings.drx_gain);
                //printf("debug: osf2.OBS_DRX_GAIN=%hd\n",osf2.OBS_DRX_GAIN);

                ///* deal with user requests to use SSMIF defaults */ 
                //if (osf2.OBS_DRX_GAIN==-1) { osf2.OBS_DRX_GAIN = s.settings.drx_gain; }
                ///* if SSMIF also leaves this up to MCS, then choose DRX GAIN = 7 */ 
                //if (osf2.OBS_DRX_GAIN==-1) { osf2.OBS_DRX_GAIN = 7; }
                
                //osf2.OBS_DRX_GAIN = 4; /* FIXME */
                if (osf2.OBS_DRX_GAIN<1) { osf2.OBS_DRX_GAIN = 6; }

                //printf("debug: osf2.OBS_DRX_GAIN=%hd\n",osf2.OBS_DRX_GAIN);

                /* DRX commands */
//     For cmd="DRX": Args are beam          1..NUM_BEAMS(4)        (uint8 DRX_BEAM)
//                             tuning        1..NUM_TUNINGS(2)      (uint8 DRX_TUNING)
//                             freq          [Hz]                   (float32 DRX_FREQ)
//                             ebw  	     Bandwidth setting 1..7 (unit8 DRX_BW)
//                             gain          0..15                  (uint16 DRX_GAIN)
//                             subslot       0..99                  (uint8 sub_slot)

                LWA_time2tv( &(cs[ncs].action.tv), dp_cmd_mjd, dp_cmd_mpm );
                cs[ncs].action.bASAP = 0;                   
                cs[ncs].action.sid = LWA_SID_DP_;  
                cs[ncs].action.cid = LWA_CMD_DRX; 
                sprintf( cs[ncs].data, "%hd 1 %8.0f %hu %hd %ld",
                                osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                                    //tuning 1..NUM_TUNINGS(2) (uint8 DRX_TUNING)
                                      (4.563480616e-02)*(osf.OBS_FREQ1), /* center freq in Hz */
                                            osf.OBS_BW,                  /* 1-7 */
                                                osf2.OBS_DRX_GAIN,       /* 0-12 */
                                                    t0);                 // subslot 0..99 (uint8 sub_slot)
                cs[ncs].action.len = strlen(cs[ncs].data)+1; 
                me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                ncs++;

                LWA_time2tv( &(cs[ncs].action.tv), dp_cmd_mjd, dp_cmd_mpm );
                cs[ncs].action.bASAP = 0;                   
                cs[ncs].action.sid = LWA_SID_DP_;  
                cs[ncs].action.cid = LWA_CMD_DRX; 
                sprintf( cs[ncs].data, "%hd 2 %8.0f %hu %hd %ld",
                                osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                                    //tuning 1..NUM_TUNINGS(2) (uint8 DRX_TUNING)
                                      (4.563480616e-02)*(osf.OBS_FREQ2), /* center freq in Hz */
                                            osf.OBS_BW,                  /* 1-7 */
                                                osf2.OBS_DRX_GAIN,       /* 0-12 */
                                                    t0);                 // subslot 0..99 (uint8 sub_slot)
                cs[ncs].action.len = strlen(cs[ncs].data)+1;
                me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                ncs++;

                /*--- BAM commands ---*/

                /* figure out when to stop doing BAM updates */
                LWA_time2tv( &tv2, osf.OBS_START_MJD, osf.OBS_START_MPM ); 
                LWA_timeadd( &tv2, osf.OBS_DUR );

                /* initializing */
                LWA_time2tv( &tv, osf.OBS_START_MJD, osf.OBS_START_MPM );
                last_alt = 0.0;
                last_az = 0.0;
                bFirst = 1;  
      
                /* looping over time */
                while ( LWA_timediff( tv2, tv ) > 0 ) {

                  LWA_timeval(&tv,&mjd,&mpm); /* get current MJD/MPM */
                  me_getaltaz( osf.OBS_RA, osf.OBS_DEC, mjd, mpm, s.fGeoN, s.fGeoE, &last, &alt, &az ); /* get updated alt/az */

                  //printf("alt=%f last_alt=%f %f | az=%f last_az=%f %f\n",alt,last_alt,angle_sep(alt,last_alt,360.0),az,last_az,angle_sep(alt,last_alt,360.0));
                  if ( (angle_sep(alt,last_alt,360.0)>=LWA_RES_DEG) || (angle_sep(az,last_az,360.0)>=LWA_RES_DEG) || bFirst ) {

                    bFirst = 0;

                    /* Trigger time is in units of "subslots" (1/100ths of a second) */
                    t0 = mpm % 1000; /* number of ms beyond a second boundary */
                    t0 /= 10; if (t0>99) t0=99; /* now in subslots */

                    /* Figure out what dfile to use (construct filename) */
                    sprintf(dfile,"740_%03.0lf_%04.0lf.df",alt*10,az*10); /* FIXME */
                    //sprintf(dfile,"dfile.df"); 

                    /* Figure out what gfile to use (construct filename) */
                    sprintf(gfile,"111226_XY.gf"); /* FIXME */
                    //sprintf(gfile,"gfile.gf"); 

                    sprintf( cs[ncs].data, "%hd %s %s %ld",
                                    osf.SESSION_DRX_BEAM, //beam 1..NUM_BEAMS(4) (uint8 DRX_BEAM)
                                        dfile,
                                           gfile,
                                              t0);

                    /* Must be sent in first 80% of slot N-2 */
                    cs[ncs].action.tv.tv_sec  = tv.tv_sec - 2;
                    cs[ncs].action.tv.tv_usec = 0; 
                    cs[ncs].action.bASAP = 0;                   
                    cs[ncs].action.sid = LWA_SID_DP_;  
                    cs[ncs].action.cid = LWA_CMD_BAM; 
                    cs[ncs].action.len = strlen(cs[ncs].data)+1; 
                    me_inproc_cmd_log( fpl, &(cs[ncs]), 1 ); /* write log msg explaining command */
                    ncs++; 

                    last_alt = alt;
                    last_az  = az;

                    } /* if ( (angle_sep */

                  LWA_timeadd(&tv,LWA_REPOINT_CHECK_INTERVAL_SEC*1000);

                  } /* while ( LWA_timediff( tv2, tv ) > 0 )  */

                break; /* LWA_OM_TRK_RADEC */ 

              default:
                printf(     "[%d/%d] FATAL: During DP setup, osf.OBS_MODE=%d not recognized\n",ME_INPROC,getpid(),osf.OBS_MODE);
                fprintf(fpl,"[%d/%d] FATAL: During DP setup, osf.OBS_MODE=%d not recognized\n",ME_INPROC,getpid(),osf.OBS_MODE);
                closedir(dir);
                fcloseall();
                return; 
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
            fcloseall();          
            return;   
            }
          fprintf(fpl,"opened '%s' (second pass)\n",osf_filename);

          /*-------------------------------------------------*/
          /*--- fast forward to stepped observations part ---*/
          /*-------------------------------------------------*/
          m = fread( &osf, sizeof(struct osf_struct), 1, fpo );

          /* If a STEPPED observation, we have a set of { osfs_struct, beam }'s for each step */
          for ( m=1; m<=osf.OBS_STP_N; m++ ) {

            fread(&osfs,sizeof(struct osfs_struct),1,fpo);
            //osfs.OBS_STP_C1    = obs[n].OBS_STP_C1[m];
            //osfs.OBS_STP_C2    = obs[n].OBS_STP_C2[m];
            //osfs.OBS_STP_T     = obs[n].OBS_STP_T[m];
            //osfs.OBS_STP_FREQ1 = obs[n].OBS_STP_FREQ1[m];
            //osfs.OBS_STP_FREQ2 = obs[n].OBS_STP_FREQ2[m];
            //osfs.OBS_STP_B     = obs[n].OBS_STP_B[m];

            if (eD==0) {
              /* process */
              } /* if (eD==0) */

            if ( osfs.OBS_STP_B==LWA_BT_SPEC_DELAYS_GAINS) {

              fread(&beam,sizeof(struct beam_struct),1,fpo);
              //for (p=1;p<=2*LWA_MAX_NSTD;p++) {
              //  beam.OBS_BEAM_DELAY[p-1] = obs[n].OBS_BEAM_DELAY[m][p]; 
              //  }
              //for (p=1;p<=260;p++) {
              //  for (q=1;q<=2;q++) {
              //    for (r=1;r<=2;r++) {
              //      beam.OBS_BEAM_GAIN[p-1][q-1][r-1] = obs[n].OBS_BEAM_GAIN[m][p][q][r]; 
              //      }
              //    }
              //  }

              if (eD==0) {
                /* process */
                } /* if (eD==0) */

              } /* if ( osfs.OBS_STP_B==LWA_BT_SPEC_DELAYS_GAINS) */

            fread(&u4,sizeof(u4),1,fpo);
            //u4 = 4294967294; fwrite(&u4, sizeof(u4),1,fp); /* = $2^{32}-2$. */
            if ( u4 != 4294967294 ) {
              fprintf(fpl,"[%d/%d] FATAL: me_inproc doesn't see '2^32-2' marker\n",ME_INPROC,getpid()); 
              printf(     "[%d/%d] FATAL: me_inproc doesn't see '2^32-2' marker\n",ME_INPROC,getpid());
              closedir(dir); 
              fcloseall();          
              return; 
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
            sprintf(cs[ncs].data,""); 
            me_inproc_cmd_log( fpl, &(cs[ncs]), 0 ); /* write log msg explaining command */
            ncs++;
            }

          /* close the .obs file; nothing else we need */
          fclose(fpo);
          fprintf(fpl,"closed '%s' (second pass)\n",osf_filename);

          /* writing out contents of osf2 structure */
          fprintf(fpl,"Writing out osf2:\n");
          for (m=0;m<260;m++) {
            for (p=0;p<2;p++) {
              fprintf(fpl,"osf2.OBS_FEE[%d][%d]=%hd\n",m,p,osf2.OBS_FEE[m][p]); 
              }
            }
          for (m=0;m<260;m++) { fprintf(fpl,"osf2.OBS_ASP_FLT[%d]=%hd\n",m,osf2.OBS_ASP_FLT[m]); }
          for (m=0;m<260;m++) { fprintf(fpl,"osf2.OBS_ASP_AT1[%d]=%hd\n",m,osf2.OBS_ASP_AT1[m]); }
          for (m=0;m<260;m++) { fprintf(fpl,"osf2.OBS_ASP_AT2[%d]=%hd\n",m,osf2.OBS_ASP_AT2[m]); }
          for (m=0;m<260;m++) { fprintf(fpl,"osf2.OBS_ASP_ATS[%d]=%hd\n",m,osf2.OBS_ASP_ATS[m]); }
          fprintf(fpl,"osf2.OBS_TBW_BITS=%hu\n",osf2.OBS_TBW_BITS); 
          fprintf(fpl,"osf2.OBS_TBW_SAMPLES=%u\n",osf2.OBS_TBW_SAMPLES);   
          fprintf(fpl,"osf2.OBS_TBN_GAIN=%hd\n",osf2.OBS_TBN_GAIN);  
          fprintf(fpl,"osf2.OBS_DRX_GAIN=%hd\n",osf2.OBS_DRX_GAIN); 

          } /* for ( i=1, i<=ssf.SESSION_NOBS; i++ ) */

        /*******************************************************/
        /***  Winding down *************************************/
        /*******************************************************/

        /* final command to tell me_exec that we're done */
        if (eD>=0) {
          cs[ncs].action.tv.tv_sec   = cs[ncs-1].action.tv.tv_sec;
          cs[ncs].action.tv.tv_usec  = cs[ncs-1].action.tv.tv_usec;
          cs[ncs].action.bASAP = 0;                   
          cs[ncs].action.sid = LWA_SID_MCS;           /* first command is always directed to MCS */
          cs[ncs].action.cid = LWA_CMD_ESN;           /* end session normally */
          cs[ncs].action.len = 0;                     /* no additional bytes */
          sprintf(cs[ncs].data,"");
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
          fcloseall();          
         return;    
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
       
        } /* if (strstr(sDirEnt->d_name,".inp")!=NULL) */
      } /* while ( (sDirEnt=readdir(dir)) != NULL ) */

    /* close the directory */
    closedir(dir);

    /* avoiding busy wait */
    sleep(1); 
    } /* while (!bDone) */


  return 0;
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
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

