// tpms.c: S.W. Ellingson, Virginia Tech, 2011 December 22
// ---
// COMPILE: gcc -o tpms tpms.c -I../common
// ---
// COMMAND LINE: ./tpms <mode> <dt> <arg3> <arg4>
//   <mode> is one of "DIAG1", "TBN", "TBW", "TRK_RADEC", etc. from MCS0030
//   <dt> is integer number of seconds from current time at which to initiate session;
//     will round up to the nearest whole second
//   <arg3>, <arg4>, etc. depend on <mode>.  Here they are:
//     TRK_RADEC: <arg3> OBS_DUR [ms], integer.  DEFAULT is 10000.
//                <arg4> RA [deg].  DEFAULT is 5.6
//                <arg5> DEC [h].  DEFAULT is +22.0
//                <arg6> FREQ1 [tuning word].  DEFAULT is  832697741 (37.999999997 MHz)
//                <arg7> FREQ2 [tuning word].  DEFAULT is 1621569285 (73.999999990 MHz)
//                <arg8> BW [filter code].  DEFAULT is 7.
//     TBN:       <arg3> OBS_DUR [ms], integer.  DEFAULT is 10000.
//                <arg4> FREQ1 [tuning word].  DEFAULT is 832697741 (37.999999997 MHz)
//                (OBS_BW is assumed to be 7 (100 kSPS) 
//     TBW:       <arg3> OBS_DUR [ms], integer.  DEFAULT is 10000.
//     DIAG1:     (none)
// ---
// REQUIRES: 
//   mcs.h via mt.h
// ---
// Generate session definition files for simple observations; 
//   time of observation can be determined automatically to be a certain distance into
//   the future
// See end of this file for history.

#include "mt.h"

/*==============================================================*/
/*=== main() ===================================================*/
/*==============================================================*/

int main ( int narg, char *argv[] ) {

  struct timeval tv;
  struct tm *tm;
  long int mjd=0;
  long int mpm=0;
  
  /* command line parameters */
  char sMode[256];
  long int dt=0;

  int eMode=0;
  long int iDur=0;
  long int iFreq=0;
  long int iFreq2=0;
  float fRA;
  float fDEC;
  int iBW;

  FILE *fp;

  long mjd_start;
  long mpm_start;
  char sPrettyTime[256];

  /* Parse command line */
  if (narg<2) {
      printf("[%d/%d] FATAL: <mode> not specified\n",MT_TPMS,getpid());
      return;
    } else {
      sprintf(sMode,"%s",argv[1]); 
      if (!(eMode=LWA_getmode(sMode))) { 
        printf("[%d/%d] FATAL: Invalid <mode>\n",MT_TPSS,getpid());   
        return;
        }
      switch (eMode) { /* not all modes are yet implemented */
        case LWA_OM_TRK_RADEC: break;
        case LWA_OM_TBN: break;
        case LWA_OM_TBW: break;
        case LWA_OM_DIAG1: break;
        default: printf("[%d/%d] FATAL: This <mode> not yet implemented\n",MT_TPSS,getpid()); return; break;
        }
      LWA_saymode( eMode, sMode );
    }
  if (narg<3) {
      printf("[%d/%d] FATAL: <dt> not specified\n",MT_TPMS,getpid());
      return;
    } else {
      sscanf(argv[2],"%ld",&dt);    
    }
  printf("[%d/%d] INPUT: eMode=%d ('%s')\n",MT_TPMS,getpid(),eMode,sMode);
  printf("[%d/%d] INPUT: dt=%ld [s]\n",MT_TPMS,getpid(),dt);
  switch (eMode) {
    case LWA_OM_TRK_RADEC:
      iDur=10000;        if (narg>=4) sscanf(argv[3],"%ld",&iDur);
      fRA=5.6;           if (narg>=5) sscanf(argv[4],"%f",&fRA);
      fDEC=22.0;         if (narg>=6) sscanf(argv[5],"%f",&fDEC);
      iFreq =832697741;  if (narg>=7) sscanf(argv[6],"%ld",&iFreq); 
      iFreq2=1621569285; if (narg>=8) sscanf(argv[7],"%ld",&iFreq2);
      iBW=7;             if (narg>=9) sscanf(argv[8],"%d",&iBW);
      printf("[%d/%d] INPUT: iDur=%ld [ms]\n",MT_TPMS,getpid(),iDur);
      printf("[%d/%d] INPUT: fRA = %6.3f [deg]\n",MT_TPMS,getpid(),fRA);
      printf("[%d/%d] INPUT: fDEC=%+6.3f [h]\n",MT_TPMS,getpid(),fDEC);
      printf("[%d/%d] INPUT: iFreq =%ld\n",MT_TPMS,getpid(),iFreq);
      printf("[%d/%d] INPUT: iFreq2=%ld\n",MT_TPMS,getpid(),iFreq2);
      printf("[%d/%d] INPUT: iBW=%d\n",MT_TPMS,getpid(),iBW);
      break;
    case LWA_OM_TBN:
      iDur=10000;      if (narg>=4) sscanf(argv[3],"%ld",&iDur);   
      iFreq=832697741; if (narg>=5) sscanf(argv[4],"%ld",&iFreq); 
      printf("[%d/%d] INPUT: iDur=%ld\n",MT_TPMS,getpid(),iDur);
      printf("[%d/%d] INPUT: iFreq=%ld\n",MT_TPMS,getpid(),iFreq);
      break;
    case LWA_OM_TBW:
      iDur=10000;      if (narg>=4) sscanf(argv[3],"%ld",&iDur);  
      printf("[%d/%d] INPUT: iDur=%ld\n",MT_TPMS,getpid(),iDur);
      break;
    case LWA_OM_DIAG1:
      break;
    default: 
      printf("[%d/%d] FATAL: Mode supported but not implemented(?)\n",MT_TPSS,getpid()); 
      return; 
      break;
    }

  /* an initial call to avoid the intermittent-bad-result-on-first-call error */
  gettimeofday(&tv,NULL);

  /* show time in a pretty format */
  printf("[%d/%d] Check that time values are correct:\n",MT_TPMS,getpid());
  gettimeofday(&tv,NULL); 
  tm = gmtime(&tv.tv_sec);
  printf("[%d/%d] Current time is YYMMDD=%02d%02d%02d %02d:%02d:%02d UTC\n",MT_TPMS,getpid(),
         (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
  printf("[%d/%d] Confirm using http://tycho.usno.navy.mil/cgi-bin/timer.pl\n",MT_TPMS,getpid());

  /* show time in MJD/MPM format */
  LWA_timeval( &tv, &mjd, &mpm );
  printf("[%d/%d] Current MJD=%ld MPM=%ld\n",MT_TPMS,getpid(),mjd,mpm);
  printf("[%d/%d] Confirm MJD using http://www.csgnetwork.com/julianmodifdateconv.html\n",MT_TPMS,getpid());

  /* determine start time */
  tv.tv_sec += dt+1;                            /* advance in time */
  tv.tv_usec = 0;
  LWA_timeval( &tv, &mjd_start, &mpm_start ); /* get start MPM, MJD */

  /* show start time in a pretty format */
  LWA_time2tv( &tv, mjd_start, mpm_start );   /* convert back to timeval */
  tm = gmtime(&tv.tv_sec);
  sprintf(sPrettyTime,"UTC %04d/%02d/%02d %02d:%02d:%02d",
          (tm->tm_year)-100+2000, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
  printf("[%d/%d] Start time is %s\n",MT_TPMS,getpid(),sPrettyTime);

  /* open SDF */
  fp = fopen("tpms.out","w");

  /* write project/session sections */
  fprintf(fp,"PI_ID          0\n");
  fprintf(fp,"PI_NAME        Not_Specified\n");
  fprintf(fp,"\n");
  fprintf(fp,"PROJECT_ID     TPMS0001\n");
  fprintf(fp,"PROJECT_TITLE  tpms-generated SDF\n");
  fprintf(fp,"PROJECT_REMPI  mode='%s', dt=%ld\n",sMode,dt);
  fprintf(fp,"PROJECT_REMPO  (none)\n");
  fprintf(fp,"\n");
  fprintf(fp,"SESSION_ID     1\n");
  fprintf(fp,"SESSION_TITLE  tpms-generated SDF\n",sMode);
  fprintf(fp,"SESSION_REMPI  (none)\n");
  fprintf(fp,"SESSION_REMPO  (none)\n");

  /* write preface of observation section */
  fprintf(fp,"\n");
  fprintf(fp,"OBS_ID         1\n");
  fprintf(fp,"OBS_TITLE      tpms-generated %s observation\n",sMode);
  //fprintf(fp,"OBS_TARGET     Observation 1 Target\n");
  //fprintf(fp,"OBS_REMPI      Observation 1 REMPI\n");
  //fprintf(fp,"OBS_REMPO      Observation 1 REMPO\n");
  fprintf(fp,"OBS_START_MJD  %ld\n",mjd_start);
  fprintf(fp,"OBS_START_MPM  %ld\n",mpm_start);
  fprintf(fp,"OBS_START      %s\n",sPrettyTime);

  /* OBS_DUR */
  fprintf(fp,"OBS_DUR        %ld\n",iDur);
  //fprintf(fp,"OBS_DUR+       00:00:10.000\n"); 

  /* OBS_MODE */
  fprintf(fp,"OBS_MODE       %s\n",sMode);  

  switch (eMode) {
    case LWA_OM_DIAG1:
      break;
    case LWA_OM_TBN:
      fprintf(fp,"OBS_FREQ1      %ld\n",iFreq);
      //fprintf(fp,"OBS_FREQ1+     19.999999955 MHz\n");
      fprintf(fp,"OBS_BW         7\n");
      fprintf(fp,"OBS_BW+        100 kSPS\n"); 
      break;
    case LWA_OM_TBW:
      break;
    case LWA_OM_TRK_RADEC:
      fprintf(fp,"OBS_RA         %6.3f\n",fRA);
      fprintf(fp,"OBS_DEC        %+6.3f\n",fDEC);
      fprintf(fp,"OBS_B          SIMPLE\n");
      fprintf(fp,"OBS_FREQ1      %ld\n",iFreq);
      //fprintf(fp,"OBS_FREQ1+     19.999999955 MHz\n");
      fprintf(fp,"OBS_FREQ2      %ld\n",iFreq2);
      //fprintf(fp,"OBS_FREQ2+     19.999999955 MHz\n");     
      fprintf(fp,"OBS_BW         %d\n",iBW);
      //fprintf(fp,"OBS_BW+        19.6 MSPS\n");
      break; 
    default:
      printf("[%d/%d] FATAL: Mode supported but not implemented(?)\n",MT_TPSS,getpid()); 
      return; 
      break;
    }

  /* close SDF */
  fclose(fp);

  return 0;
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// tpms.c: S.W. Ellingson, Virginia Tech, 2011 December 22
//   .1: Adding TBW mode
// tpms.c: S.W. Ellingson, Virginia Tech, 2011 December 19
//   .1: Adding TRK_RADEC mode
// tpms.c: S.W. Ellingson, Virginia Tech, 2011 December 14
//   .1: Initial version
// tptc.c: S.W. Ellingson, Virginia Tech, 2011 March 07
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================


