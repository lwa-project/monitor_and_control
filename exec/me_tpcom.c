// me_tpcom.c: S.W. Ellingson, Virginia Tech, 2013 Jul 18
// ---
// COMPILE: gcc -o me_tpcom me_tpcom.c -I../common
// ---
// COMMAND LINE: me_tpcom 
// ---
// REQUIRES: 
//   me.h
// ---
// Handles movement of files between MCS/TP and the MCS/Exec 'me' process
// See end of this file for history.

#include <stdlib.h>

#include "me.h"
#include <dirent.h> /* this is listing files in a directory */

/*******************************************************************/
/*** tp_xfer() *****************************************************/
/*******************************************************************/
/* moves/copies files to/from designated directory on TP */

#define TP_XFER_ME_TO_TP 0 /* direction of transfer is from MCS/Exec to TP */
#define TP_XFER_TP_TO_ME 1 /* direction of transfer is from TP to MCS/Exec */

#define TP_XFER_MV 0 /* file is to be *moved* */
#define TP_XFER_CP 1 /* file is to be *copied* */

#define TP_XFER_ERR_OK   0 /* no errors */
#define TP_XFER_ERR_MVCP 1 /* invalid mv_or_cp argument */
#define TP_XFER_ERR_DIR  2 /* invalid direction argument */

int tp_xfer( char *mepath,   /* relative path for file on MCS/Exec */
             char *filename, /* name of file */
             int direction,  /* TP_XFER_ME_TO_TP or TP_XFER_TP_TO_ME */
             int mv_or_cp,   /* TP_XFER_MV or TP_XFER_CP */
             int bVerbose
           ) {

  //char sMvCp[256];
  char cmd[ME_MAX_COMMAND_LINE_LENGTH];
  char cmd2[ME_MAX_COMMAND_LINE_LENGTH];
  int eErr=TP_XFER_ERR_OK;

  switch (mv_or_cp) {
    case TP_XFER_MV: break;
    case TP_XFER_CP: break;
    default: eErr=TP_XFER_ERR_MVCP; return eErr; break;
    }

  /* setting up commands */
  switch (direction) {
    case TP_XFER_TP_TO_ME:
      #ifdef ME_SCP2CP  /*see me.h */
         sprintf(cmd, "cp %s/%s %s/.",LWA_TP_SCP_DIR,filename,mepath);
         sprintf(cmd2,"rm %s/%s",LWA_TP_SCP_DIR,filename);
      #else
         sprintf(cmd, "scp -q %s:%s/%s %s/.",LWA_TP_SCP_ADDR,LWA_TP_SCP_DIR,filename,mepath);
         sprintf(cmd2,"ssh %s rm %s/%s",LWA_TP_SCP_ADDR,LWA_TP_SCP_DIR,filename);
      #endif
      break; 
    case TP_XFER_ME_TO_TP:
      #ifdef ME_SCP2CP  /*see me.h */
         sprintf(cmd, "cp %s/%s %s/.",mepath,filename,LWA_TP_SCP_DIR);
      #else
         sprintf(cmd, "scp -q %s/%s %s:%s/.",mepath,filename,LWA_TP_SCP_ADDR,LWA_TP_SCP_DIR);
      #endif
      sprintf(cmd2,"rm %s/%s",mepath,filename); 
      break; 
    default:
      eErr=TP_XFER_ERR_DIR; return eErr; 
      break;
    }
 
  if (!bVerbose) sprintf(cmd, "%s 2> /dev/null",cmd);
  if (!bVerbose) sprintf(cmd2,"%s 2> /dev/null",cmd2);

  /* do it */
  if (bVerbose)  printf("'%s'\n",cmd);
  system(cmd);
  if (mv_or_cp==TP_XFER_MV) { 
    if (bVerbose) printf("'%s'\n",cmd2); 
    system(cmd2); 
    }

  return eErr;
  } /* tp_xfer() */


/*******************************************************************/
/*** main() ********************************************************/
/*******************************************************************/

int main ( int narg, char *argv[] ) {

  struct timeval tv;
  int bDone=0;
  char cmd[ME_MAX_COMMAND_LINE_LENGTH];
  char sManifestFilename[ME_FILENAME_MAX_LENGTH];
  struct stat statbuf;
  FILE* fp;

  char sProjectID[ME_MAX_PROJECT_ID_LENGTH]; 
  int iSessionID;
  int iObs;
  char filename[ME_FILENAME_MAX_LENGTH];

  DIR *dir;
  struct dirent *sDirEnt;

  struct me_outproc_manifest_struct om;
  char om_filename[ME_FILENAME_MAX_LENGTH];

  /* First, announce thyself */
  printf("[%d/%d] I am me_tpcom\n",ME_TPCOM,getpid());

  /* sometimes first call to gettimeofday() returns something bogus, so let's get that out of the way */
  gettimeofday( &tv, NULL ); 

  /* construct the path/filename to manifest.dat, when it's in the inbox */
  sprintf(sManifestFilename,"tpcom/manifest.dat");

  /*****************/
  /*****************/
  /*** main loop ***/
  /*****************/
  /*****************/

  while (!bDone) {

    /*************/
    /* TP pull   */
    /*************/

    /* attempt to pull "manifest.dat" */
    tp_xfer( "tpcom", "manifest.dat", TP_XFER_TP_TO_ME, TP_XFER_MV, 0 );

    /* check to see if a "manifest.dat" file was transferred */
    if (stat(sManifestFilename,&statbuf)==0) {
      /* Since we snagged one, there is work to do. */
      //printf("[%d/%d] Found '%s'\n",ME_TPCOM,getpid(),sManifestFilename);

      if (!(fp=fopen(sManifestFilename,"r"))) {
        printf("[%d/%d] FATAL: Couldn't fopen('%s')\n",ME_TPCOM,getpid(),sManifestFilename);
        exit(EXIT_FAILURE);
        }

      /* transfer files identified in manifest */
      while (!feof(fp)) {
        fscanf(fp,"%s %d %d %s",sProjectID,&iSessionID,&iObs,filename); 
        if (!feof(fp)) {  /* move this file */       
          tp_xfer( "sinbox", filename, TP_XFER_TP_TO_ME, TP_XFER_MV, 0 );  
          }
        }

      fclose(fp);

      /* append contents of manifest.dat to end of similarly-named file in sinbox */
      sprintf(cmd,"cat %s >> sinbox/manifest.dat",sManifestFilename); 
      system(cmd);

      /* finally, delete the local copy of the manifest */
      sprintf(cmd,"rm %s",sManifestFilename); 
      system(cmd);

      } /* if (stat(sManifestFilename,&statbuf)==0) */

    /*********************************************/
    /* Deal with outprocessing manifests, if any */
    /*********************************************/

    /* check the state directory for any outprocessing manifest files */
    if (!(dir = opendir("state"))) {
      printf("[%d/%d] FATAL: Couldn't opendir('state')\n",ME_TPCOM,getpid());
      exit(EXIT_FAILURE);
      } 

    while ( (sDirEnt=readdir(dir)) != NULL ) {
      //printf("'%s'\n",sDirEnt->d_name);
      if (strstr(sDirEnt->d_name,".manifest")!=NULL) { /* found one */
        sprintf(om_filename,"state/%s",sDirEnt->d_name);
        //printf("me_tpcom: om_filename='%s'\n",om_filename);
        
        /* read the manifest */
        if (!(fp = fopen(om_filename,"rb"))) {
          printf("[%d/%d] FATAL: Couldn't fopen('%s')\n",ME_TPCOM,getpid(),om_filename);
          exit(EXIT_FAILURE);
          }       
        fread(&om,sizeof(om),1,fp);
        fclose(fp);
        //printf("me_tpcom: om.SDF='%s'\n",om.SDF);

        /* clear the working directory */
        sprintf(cmd,"rm -rf tpcom/temp/* 2> /dev/null");
        system(cmd);
        sprintf(cmd,"mkdir tpcom/temp/dynamic 2> /dev/null");
        system(cmd);
        
        /* moving files into temp directory */
        sprintf(cmd,"mv sinbox/%s_%04d* tpcom/temp/.",om.ssf.PROJECT_ID,om.ssf.SESSION_ID);
        system(cmd);
        system("cp state/mindelay.txt tpcom/temp/.");
        if (om.ssf.SESSION_INC_SMIB==1) system("cp state/ssmif.dat tpcom/temp/.");
        system("cp state/sdm.dat tpcom/temp/dynamic/.");
        if (om.ssf.SESSION_LOG_EXE==1)  system("cp state/meelog.txt tpcom/temp/dynamic/.");

        /* making zip file & moving it into soutbox */
        //sprintf(cmd,"cd %s/temp; tar -zcvf temp.tgz .; mv temp.tgz ../%s_%04d.tgz; cd ../.. 2> /dev/null", 
        sprintf(cmd,"cd tpcom/temp; tar -zcf ../../soutbox/%s_%04d.tgz *; cd ../..",
                                                           om.ssf.PROJECT_ID,
                                                              om.ssf.SESSION_ID);
        //printf("me_tpcom: system(%s)\n",cmd);
        system(cmd);

        /* delete the manifest */
        sprintf(cmd,"rm %s",om_filename);
        //printf("me_tpcom: system(%s)\n",cmd);
        system(cmd);        

        }
      }
  
    /* close the directory */
    closedir(dir);

    /*************/
    /* TP push   */
    /*************/

    /* push a *copy* of mess.dat */
    tp_xfer( "state", "mess.dat", TP_XFER_ME_TO_TP, TP_XFER_CP, 0 );  

    /* push a *copy* of mesq.dat */
    tp_xfer( "state", "mesq.dat", TP_XFER_ME_TO_TP, TP_XFER_CP, 0 );  

    /* push a *copy* of sdm.dat */
    tp_xfer( "state", "sdm.dat", TP_XFER_ME_TO_TP, TP_XFER_CP, 0 ); 

    /* push entire contents of soutbox */
    tp_xfer( "soutbox", "*", TP_XFER_ME_TO_TP, TP_XFER_MV, 0 ); 

    /* avoiding busy wait */
    sleep(1); 
  
    } /* while (!bDone) */

  exit(EXIT_SUCCESS);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// me_tpcom.c: J. Dowell, UNM, 2021 Jan 25
//   .1: Updated to include the mindelay.txt file
// me_tpcom.c: S.W. Ellingson, Virginia Tech, 2013 Jul 18
//   .1: Now adding ASP MIB to output tarball at end of session
// me_tpcom.c: S.W. Ellingson, Virginia Tech, 2011 Mar 11
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
