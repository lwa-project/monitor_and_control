// me_mon.c: J. Dowell, UNM, 2015 Aug 17
// ---
// COMPILE: gcc -o me_mon me_mon.c -lncurses  
// ---
// COMMAND LINE: ./me_mon <dupdate>
//   <dupdate>   display update period [s]; "0" means one initial update only.
// EXAMPLE: 
//   $ ./me_mon
// ---
// REQUIRES: 
//   Uses the ncurses library (Ubuntu: look for packages with names like "libncurses5") 
// ---
// This is a curses-based MCS/Executive monitor that is based on ms_mon.
//   Shows the last few lines of mselog.txt
//   Also shows the *current* time in various formats, in the last line
// See end of this file for history.

#include <stdlib.h> /* needed for exit(); possibly other things */
#include <stdio.h>
#include <string.h>
#include <ncurses.h>

//#include "LWA_MCS.h"
#include "mcs.h"

#define MY_NAME "me_mon (v.201508170.1)"
#define ME "17" 

#define MEELOG_STR_LEN 4096

int main ( int narg, char *argv[] ) {

  /*=================*/
  /*=== Variables ===*/
  /*=================*/
  int dupdate_period=0;

//  int n;
  int bDone = 0;
  char keyin;

  int k=0;

  char tempfile[256];
  char cmd_line[256];

  FILE* fid;
  char line[MEELOG_STR_LEN];

  time_t t0,t1;

  long int mjd;
  long int mpm;
  struct timeval tv;
  struct tm *tm;
  char time_string[256];
  char status_line[256];
  
  /*======================================*/
  /*=== Initialize: Command line stuff ===*/
  /*======================================*/
    
  /* First, announce thyself */
  printf("[%s/%d] I am %s \n",ME,getpid(),MY_NAME);

  /* Process command line arguments */
  if (narg>1) { 
      sscanf(argv[1],"%d",&dupdate_period);
      //printf("[%s/%d] %d specified\n",ME,getpid(),dupdate_period);
    } else {
      printf("[%s/%d] FATAL: dupdate not specified\n",ME,getpid());
      exit(EXIT_FAILURE);
    } 

  /*======================================*/
  /*=== Setup Screen =====================*/
  /*======================================*/
  initscr();
  cbreak();  /* make characters available as soon as they are typed */
  noecho();
  if (dupdate_period>0) {
    halfdelay(1); /* limit blocking to 0.1 s when checking for characters */
    }
 
  //endwin();

  t0 = time(NULL);

  /*======================================*/
  /*=== Main Loop ========================*/
  /*======================================*/

  while (!bDone) {

    /* Get end of mselog.txt as a separate file */
    sprintf(tempfile,"me_mon_%d.txt",getpid());
    sprintf(cmd_line,"tail -23 state/meelog.txt > %s",tempfile);
    system(cmd_line);    

    /* get the current time in LWA (MJD/MPM) format */
    LWA_time( &mjd, &mpm ); 

    /* get the current time in conventional format */
    gettimeofday(&tv,NULL); 
    tm = gmtime(&tv.tv_sec);
    sprintf(time_string,"%02d%02d%02d %02d:%02d:%02d", 
         (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
 
    /* read this file */
    fid = fopen(tempfile,"r");
    if (!fid) { 
      endwin();
      printf("[%s/%d] FATAL: Can't read %s\n",ME,getpid(),tempfile); 
      exit(EXIT_FAILURE); 
      } 
    memset(line,'\0',strlen(line));  /* fill "line" with terminator characters */
    fscanf(fid,"%4096c",line);      
    fclose(fid);

    /* prepare current time/status string, which will go at bottom-of-screen */ 
    sprintf(status_line, "%s %6ld %9ld. %s %s. 'q' quits.", time_string, mjd, mpm, argv[0],argv[1] );

    /* clear screen and show fragment from mselog.txt */
    clear();
    move(0,0); printw("%s",line);

    /* Add garnish and show it */
    move(LINES-1,0);      printw("%s",status_line);
    move(LINES-1,COLS-1); if (k==0) { k++; printw("*"); } else { k=0; printw(" "); }
    refresh();
  
    /* done; sleep until next update */
    if (dupdate_period>0) { sleep(dupdate_period); } 

    /* process key entry */
    keyin = getch();
    if (keyin=='q' || keyin=='Q') { bDone=1; } 
   
    } /* while (!bDone) */

  /*======================================*/
  /*=== Exit =============================*/
  /*======================================*/

  /* close down ncurses screen */
  endwin();

  /* delete temp file */
  sprintf(cmd_line,"rm %s",tempfile);
  system(cmd_line);

  printf("[%s/%d] exit(EXIT_SUCCESS)\n",ME,getpid());
  //printf("%d %d\n",LINES,COLS);
  exit(EXIT_SUCCESS);
  } /* main() */


//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// me_mon.c: J. Dowell, UNM, 2015 Aug 17
//   .1: Initial version built from ms_mon.c

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
