// tptc.c: S.W. Ellingson, Virginia Tech, 2011 March 07
// ---
// COMPILE: gcc -o tptc tptc.c -I../common
// ---
// COMMAND LINE: tptc
// ---
// REQUIRES: 
//   mcs.h via mt.h
// ---
// This code used to check time-keeping on local PC and OS, as well as 
//   time calculation & conversion routines in mcs.h
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
  long int h,m,s;
  int i;

  /* an initial call to avoid the intermittent-bad-result-on-first-call error */
  gettimeofday(&tv,NULL);

  printf("[%d/%d] --- Checking gettimeofday(&tv,NULL) --------\n",MT_TPTC,getpid());
  printf("[%d/%d] This checks that PC clock & OS timevals are correct.\n",MT_TPTC,getpid());
  gettimeofday(&tv,NULL); 
  tm = gmtime(&tv.tv_sec);
  printf("[%d/%d] Current time is YYMMDD=%02d%02d%02d %02d:%02d:%02d UTC\n",MT_TPTC,getpid(),
         (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
  printf("[%d/%d] Confirm using http://tycho.usno.navy.mil/cgi-bin/timer.pl\n",MT_TPTC,getpid());

  printf("[%d/%d] --- Checking LWA_timeval(&tv,&mjd,&mpm) ---\n",MT_TPTC,getpid());
  printf("[%d/%d] This checks timeval -> MPM/MJD conversion.\n",MT_TPTC,getpid());
  LWA_timeval( &tv, &mjd, &mpm );
  printf("[%d/%d] Current MJD=%ld MPM=%ld\n",MT_TPTC,getpid(),mjd,mpm);
  printf("[%d/%d] Confirm MJD using http://www.csgnetwork.com/julianmodifdateconv.html\n",MT_TPTC,getpid());
  h = mpm/(3600*1000);
  m = (mpm - (h*3600*1000))/(60*1000);
  s = (mpm - (h*3600*1000) - (m*60*1000))/1000;
  printf("[%d/%d] Converting MPM to hh:mm:ss gives %02ld:%02ld:%02ld UTC (Check against timeval)\n",MT_TPTC,getpid(),h,m,s);

  printf("[%d/%d] --- Checking LWA_time2tv(&tv,mjd,mpm) ---\n",MT_TPTC,getpid());
  printf("[%d/%d] This checks MPM/MJD -> timeval conversion.\n",MT_TPTC,getpid());
  LWA_time2tv( &tv, mjd, mpm ); 
  tm = gmtime(&tv.tv_sec);
  printf("[%d/%d] Current time is YYMMDD=%02d%02d%02d %02d:%02d:%02d UTC\n",MT_TPTC,getpid(),
         (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

  printf("[%d/%d] --- Repeating daily for one year ---\n",MT_TPTC,getpid());
  printf("[%d/%d] Adding 1 day to timeval, converting to MPM/MJD, and then back to timeval.\n",MT_TPTC,getpid());
  printf("[%d/%d] This should catch day- or month-specific anomolies.\n",MT_TPTC,getpid());
  for ( i=1; i<=366; i++ ) {
    tv.tv_sec += (24*3600);         /* advance timeval one day */
    LWA_timeval( &tv, &mjd, &mpm ); /* get new MPM, MJD */
    LWA_time2tv( &tv, mjd, mpm );   /* convert back to timeval */
    tm = gmtime(&tv.tv_sec);
    printf("[%d/%d] +%3d days: time is YYMMDD=%02d%02d%02d %02d:%02d:%02d UTC\n",MT_TPTC,getpid(),i,
         (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    }
  printf("[%d/%d] If the above spans Feb 29 of a leap year (e.g., 2012,2016,...),\n",MT_TPTC,getpid());
  printf("[%d/%d]   then the '+366' date should be exactly one year from now.\n",MT_TPTC,getpid());
  printf("[%d/%d] Otherwise, the '+365' date should be exactly one year from now.\n",MT_TPTC,getpid());
  printf("[%d/%d] Scan daily results if anomolies are suspected.\n",MT_TPTC,getpid());

  return 0;
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// tptc.c: S.W. Ellingson, Virginia Tech, 2011 March 07
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================


