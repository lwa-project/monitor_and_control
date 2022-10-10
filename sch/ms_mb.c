// ms_mb.c: S.W. Ellingson, Virginia Tech, 2010 May 31
// ---
// COMPILE: gcc -o ms_mb ms_mb.c -lncurses  
// ---
// COMMAND LINE: ms_mb <subsystem> <template> <dupdate> <mupdate> <annotations> 
//   <subsystem> is the 3-character subsystem designator
//   <template>  is the filename of the "template" file; see below 
//   <dupdate>   display update period [s]; "0" means initial update only, then stop.
//               This refers to how often the MIB dbm file is read.
//               Integer seconds.
//   <mupdate>   MIB update period [s]; "0" means don't (just observe).
//               This refers to how often MIB entry updates (RPTs) are requested.
//               Only the entries in the template indicated with b=1 (see below)
//                 are update.
//               Integer seconds.
//   <annotations> is the filename of an "annotations" file; see below
//               (optional)
// EXAMPLE: 
//   $ ./ms_mb SHL ms_mb_SHL.dat 1 0 ms_mb_SHLa.dat
// ---
// REQUIRES: 
//   Calls ms_md2t to textify dbm-formatted MIB file
//   Uses the ncurses library (Ubuntu: look for packages with names like "libncurses5") 
// ---
// This is a curses-based MIB monitor.
// Hit "q" to exit.
// The screen contents and format are controlled by the file specified as <template>
// in the command line.
// The template file is ASCII, one line per item, with space delimiting items on a line
// The line format is:
//   MIB_label b x1 y1 x2 y2 x3 y3 x4 y4
//   b = 1 means request update of this entry when MIB updates happen; otherwise don't
//   x1,y1 is the screen location to write the MIB label
//   x2,y2 is the screen location to write the corresponding value
//   x3,y3 is the screen location to write the MIB update date
//   x4,y4 is the screen location to write the MIB update time
//   Note position is line,column with 1,1 meaning upper left;
// The bottom line of the screen is reserved for program use   
// The annotations file format is as follows:
//   x y text
//   x,y are screen coordinates
//   "text" is the annotation to appear at those coordinates
// See end of this file for history.

#include <stdlib.h> /* needed for exit(); possibly other things */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

#define MY_NAME "ms_mb (v.20100531.2)"
#define ME "13" 

#define MAX_LABELS 9999 /* max number of MIB entries supported */
#define MAX_FIELD_LENGTH 256

#define MAX_ANNOTATIONS 99


int main ( int narg, char *argv[] ) {

  /*=================*/
  /*=== Variables ===*/
  /*=================*/
  char subsystem[256];
  char template_file[256];
  int dupdate_period;
  int mupdate_period;

  FILE* fid;
  int nitems = 0;
  char label[MAX_LABELS][256];
  int b[MAX_LABELS];
  int x1[MAX_LABELS], y1[MAX_LABELS];
  int x2[MAX_LABELS], y2[MAX_LABELS];
  int x3[MAX_LABELS], y3[MAX_LABELS];
  int x4[MAX_LABELS], y4[MAX_LABELS];

  int nann=0;
  int ax[MAX_ANNOTATIONS];
  int ay[MAX_ANNOTATIONS];
  char at[MAX_ANNOTATIONS][MAX_FIELD_LENGTH];

  char cmd_line[256];

  char label0[MAX_FIELD_LENGTH];
  char eType[MAX_FIELD_LENGTH];
  char index[MAX_FIELD_LENGTH];
  char val[MAX_FIELD_LENGTH];
  char type_dbm[MAX_FIELD_LENGTH];
  char type_icd[MAX_FIELD_LENGTH];
  char udate[MAX_FIELD_LENGTH];
  char utime[MAX_FIELD_LENGTH];

  char label1[MAX_FIELD_LENGTH];
  char test_string[MAX_FIELD_LENGTH];

  int n;
  int bDone = 0;
  char keyin;

  int k=0;

  char tempfile[256];

  time_t t0,t1;

  char temp_line[256];

  char ann_file[256];
  int bAnn = 0;

  /*======================================*/
  /*=== Initialize: Command line stuff ===*/
  /*======================================*/
    
  /* First, announce thyself */
  printf("[%s/%d] I am %s \n",ME,getpid(),MY_NAME);

  /* Process command line arguments */
  if (narg>1) { 
      sprintf(subsystem,"%s",argv[1]);
      //printf("[%s/%d] %s specified\n",ME,getpid(),subsystem);
    } else {
      printf("[%s/%d] FATAL: subsystem not specified\n",ME,getpid());
      exit(EXIT_FAILURE);
    } 
  if (narg>2) { 
      sprintf(template_file,"%s",argv[2]);
      //printf("[%s/%d] %s specified\n",ME,getpid(),template_file);
    } else {
      printf("[%s/%d] FATAL: template not specified\n",ME,getpid());
      exit(EXIT_FAILURE);
    } 
  if (narg>3) { 
      sscanf(argv[3],"%d",&dupdate_period);
      //printf("[%s/%d] %d specified\n",ME,getpid(),dupdate_period);
    } else {
      printf("[%s/%d] FATAL: dupdate not specified\n",ME,getpid());
      exit(EXIT_FAILURE);
    } 
  if (narg>4) { 
      sscanf(argv[4],"%d",&mupdate_period);
      //printf("[%s/%d] %d specified\n",ME,getpid(),mupdate_period);
    } else {
      printf("[%s/%d] FATAL: mupdate not specified\n",ME,getpid());
      exit(EXIT_FAILURE);
    } 
  if (narg>5) { 
      sscanf(argv[5],"%s",ann_file);
      bAnn = 1;
    } else {
      bAnn = 0;
    } 

  /*======================================*/
  /*=== Read template file ===============*/
  /*======================================*/

  /* Open template file */
  fid = fopen(template_file,"r");
  if (!fid) { 
    printf("[%s/%d] FATAL: Can't read template file <%s>\n",ME,getpid(),template_file); 
    exit(EXIT_FAILURE); 
    } 

  nitems=0;
  while (!feof(fid)) {
    nitems++;

    if (nitems>MAX_LABELS) {
      printf("[%s/%d] FATAL: nlabels > MAX_LABELS\n",ME,getpid()); 
      exit(EXIT_FAILURE); 
      }

    fscanf(fid,"%s %d %d %d %d %d %d %d %d %d\n",
      label[nitems-1],               // MIB label
      &b[nitems-1],                  // =1 means this should be updated when/if MIB is to be updated
                                     // (MIB entries < 1 cannot be RPTed; set to zero in this case)
      &x1[nitems-1], &y1[nitems-1],  // screen location to write the MIB label
      &x2[nitems-1], &y2[nitems-1],  // screen location to write the corresponding value
      &x3[nitems-1], &y3[nitems-1],  // screen location to write the MIB update date
      &x4[nitems-1], &y4[nitems-1]); // screen location to write the MIB update time

    //printf("%s %d %d %d %d %d %d %d %d\n",
    //  label[nitems-1],
    //  x1[nitems-1], y1[nitems-1], 
    //  x2[nitems-1], y2[nitems-1], 
    //  x3[nitems-1], y3[nitems-1], 
    //  x4[nitems-1], y4[nitems-1]);

    } /* while (!feof(fid)) */

  /* close template file */
  fclose(fid);

  /*======================================*/
  /*=== Read annotations file ============*/
  /*======================================*/

  if (bAnn) {

    /* Open template file */
    fid = fopen(ann_file,"r");
    if (!fid) { 
      printf("[%s/%d] FATAL: Can't read annotations file <%s>\n",ME,getpid(),ann_file); 
      exit(EXIT_FAILURE); 
      } 

    nann=0;
    while (!feof(fid)) {

      if (nitems>MAX_ANNOTATIONS) {
        printf("[%s/%d] FATAL: nann > MAX_ANNOTATIONS\n",ME,getpid()); 
        exit(EXIT_FAILURE); 
        }
  
      memset(test_string,'\0',MAX_FIELD_LENGTH);
      fgets( test_string, MAX_FIELD_LENGTH, fid);
      //printf("<%s>\n",test_string);

      if (strlen(test_string)>1) {    
        sscanf(strtok(test_string, " "),"%d",&ax[nann]);
        sscanf(strtok(NULL,        " "),"%d",&ay[nann]);
        memset(at[nann],'\0',MAX_FIELD_LENGTH);
          memcpy(at[nann],strtok(NULL,"\n"),MAX_FIELD_LENGTH);
  
        //printf("%d %d <%s>\n",ax[nann],ay[nann],at[nann]);

        nann++;  
        } /* if strlen(test_string)>0 */
      } /* while (!feof(fid)) */

    /* close template file */
    fclose(fid);
    
    } /* if (bAnn) */


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

    /*=== Get human-readable MIB snapshot using ms_md2t ===*/

    sprintf(tempfile,"ms_mb_%d.txt",getpid());
    sprintf(cmd_line,"./ms_md2t %s > %s",subsystem,tempfile);
    system(cmd_line);    

    /*=== Process snapshot file ===========================*/
    /*=== Screen is updated as we go ======================*/

    /* Open snapshot file */
    fid = fopen(tempfile,"r");
    if (!fid) { 
      printf("[%s/%d] FATAL: Can't read %s\n",ME,getpid(),tempfile); 
      exit(EXIT_FAILURE); 
      } 

    clear();

    /* Read one line at a time, looking for lines corresponding to */
    /* things in the template */
    while (!feof(fid)) {

      memset(test_string,'\0',MAX_FIELD_LENGTH);
      fgets( test_string, MAX_FIELD_LENGTH, fid);
      memset(label1,   '\0',MAX_FIELD_LENGTH); memcpy(label1,  test_string+0,32); //   0- 31,  32 is space
      memset(eType,    '\0',MAX_FIELD_LENGTH); memcpy(eType,   test_string+ 33, 1); //      33,  34 is space
      memset(index,    '\0',MAX_FIELD_LENGTH); memcpy(index,   test_string+ 35,12); //  35- 46,  47 is space
      memset(val,      '\0',MAX_FIELD_LENGTH); memcpy(val,     test_string+ 48,32); //  48- 79,  80 is space
      memset(type_dbm, '\0',MAX_FIELD_LENGTH); memcpy(type_dbm,test_string+ 81, 6); //  81- 86,  87 is space
      memset(type_icd, '\0',MAX_FIELD_LENGTH); memcpy(type_icd,test_string+ 88, 6); //  88- 93,  94 is space
      memset(udate,    '\0',MAX_FIELD_LENGTH); memcpy(udate,   test_string+ 95, 6); //  95-100, 101 is space
      memset(utime,    '\0',MAX_FIELD_LENGTH); memcpy(utime,   test_string+102, 8); // 102-109

      sscanf(label1,"%s ",label0); /* this trims off the trailing whitespace, so that the label matching */
                                   /* code below works properly */

      //printf("|%s|%s|%s|%s|%s|%s|%s|%s|\n",label0,eType,index,val,type_dbm,type_icd,udate,utime);

      for (n=0; n<nitems; n++) {
        //printf("<%s> <%s>\n",label0,label[n]);
        if (!strcmp(label0,label[n])) { /* match found */
            // printf("<%s> <%s|%s|%s|%s>\n",label[n],label0,val,udate,utime);
           move(x1[n],y1[n]); printw("%s",label0);
           move(x2[n],y2[n]); printw("%s",val);
           move(x3[n],y3[n]); printw("%s",udate);
           move(x4[n],y4[n]); printw("%s",utime);
           } 
        } /* for (n=0 */      

      } /* while (!feof(fid)) */

    /* close snapshot file */
    fclose(fid);
    
    //exit(EXIT_SUCCESS);

    /* add annotations */
    if (bAnn) {
      for (n=0;n<nann;n++) {
        move(ax[n],ay[n]); printw("%s",at[n]);
        } /* for (n */
      }

    /* Add garnish and show it */
    move(LINES-1,1);      printw("%s %s %s %s %s %s. 'q' quits.",argv[0],argv[1],argv[2],argv[3],argv[4],argv[5]);
    move(LINES-1,COLS-1); if (k==0) { k++; printw("*"); } else { k=0; printw(" "); }
    refresh();

    /* Request MIB updates, if it's time */
    t1 = time(NULL);
    // move(1,1); printw("%d %d %d ",t0,t1,t1-t0); refresh();
    if ( ((t1-t0)>=mupdate_period) && (mupdate_period>0) ) { /* it's time */
      t0 = t1;  
      //move(1,1); printw("** UPDATE **"); refresh();

      /* scroll through template file and send RPT requests */
      for (n=0; n<nitems; n++) {
        if (b[n]) {
          sprintf(cmd_line,"./msei %s RPT %s > /dev/null",subsystem,label[n]);
          system(cmd_line);
          } /* if (b[n]) */
        } /* for (n=0 */

      } /* if ((t1-t0)>=mupdate_period) */
  
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

  /* delete temp files */
  sprintf(cmd_line,"rm %s",tempfile);
  system(cmd_line);

  printf("[%s/%d] exit(EXIT_SUCCESS)\n",ME,getpid());
  //printf("%d %d\n",LINES,COLS);
  exit(EXIT_SUCCESS);
  } /* main() */


//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// ms_mb.c: S.W. Ellingson, Virginia Tech, 2010 May 31
//   .1: Changes to input code so that val field could have spaces
//   .2: Adding annotations
// ms_mb.c: S.W. Ellingson, Virginia Tech, 2010 May 27
//   .1: Adding ability to force update of MIB entries
// ms_mb.c: S.W. Ellingson, Virginia Tech, 2010 May 26
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
