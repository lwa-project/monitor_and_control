// ms_init_replay.c: J. Dowell, UNM, 2020 Jun 24
// ---
// COMPILE: gcc -o ms_init_replay ms_init_replay.c
// ---
// COMMAND LINE: ms_init_replay <replay_log_file> 
//   <replay_log_file> mselog.txt file to replay (required)
// ---
// REQUIRES: 
// ---
// Initialization for MCS/Scheduler
// See end of this file for history.

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/msg.h>

//#include "LWA_MCS.h"
#include "mcs.h"

#include <string.h>

#define MY_NAME "ms_init_replay (v.20200624.1)"
#define ME "1" 

#define MAX_LINE_LENGTH 256
#define MAX_TOKENS 10

int main ( int narg, char *argv[] ) {

  /*=================*/
  /*=== Variables ===*/
  /*=================*/

  char ms_init_filename[256]; /* command line arguments */
  char run_as_loop[256];

  FILE* fid;
  int bDone;

  char line[MAX_LINE_LENGTH];       /* used in parsing init file */
  char *snippet; 

  int err;
  pid_t ms_exec_pid; /* PID for ms_exec process */
  pid_t ms_mdre_pid; /* PID for ms_mdre process */
  pid_t pid;

  int mqid;
  int mqt;
  struct LWA_cmd_struct mq_msg;

  int i, found;
  int nsid;             /* number of subsystems defined */
  int sid[LWA_MAX_SID]; /* subsystem IDs defined */

  key_t mqtkey;       /* key for transmit message queue */

  char sidlist[9];     /* used to generate cmd line arg for ms_exec */
  int n;
  long int sidsum;

  /*==================*/
  /*=== Initialize ===*/
  /*==================*/
    
  /* First, announce thyself */
  printf("[%s] I am %s\n",ME,MY_NAME);

  /* Process command line arguments */
  if (narg>1) { 
    sprintf(ms_init_filename,"%s",argv[1]);
      printf("[%s] replay_log_filename: <%s>\n",ME,ms_init_filename);
    }
    else {
    printf("[%s] FATAL: replay_log_filename not provided\n",ME);
    exit(EXIT_FAILURE);
    }

  sprintf(run_as_loop,"%i",0);
  if (narg>2) {
    atoi(argv[2]);
    sprintf(run_as_loop,"%s",argv[2]);
    } 

  /* Set number of subsystems to zero */
  nsid = 0;

  /* Set up for receiving from message queue */
  mqid = msgget(
                 (key_t) MQ_MS_KEY, /* identify message queue */
                 0666 | IPC_CREAT   /* create if it doesn't already exist */
                );
  if (mqid==-1) 
    printf("[%s] WARNING: Message queue setup failed with code %d\n",ME,mqid);   

  /* Clear out message queue */
  while ( msgrcv( mqid, (void *)&mq_msg, LWA_msz(), 0, IPC_NOWAIT ) > 0 ) ;

  /* Open ms_init_file */
  fid = fopen(ms_init_filename,"r");
  if (!fid) { 
    printf("[%s] FATAL: Can't read replay_log_file\n",ME); 
    exit(EXIT_FAILURE); 
    }  

  /*==================*/
  /*=== Main Loop ====*/
  /*==================*/
  /* Read ms_init_file line-by-line, taking action as each line is read. */

  while (!feof(fid)) {

    /* read next line */
    strcpy(line,""); /* keep last-line junk from screwing this up... */
    fgets(line, sizeof(line), fid);
    
    /* parse line - the only thing we really care about is the subsystem name */
    int ymd, status;
    long int mjd, mpm, ref;
    char hms[9], ltype[2], sname[4], cname[4], cdata[MAX_LINE_LENGTH];
    sscanf(line, "%d %8s %ld %ld %1s %ld %d %3s %3s %[^|]",
                 &ymd, hms, &mjd, &mpm, ltype, &ref, &status, sname, cname, cdata);

    found = 0;
    for(i=0; i<nsid; i++) {
        if( sid[i] == LWA_getsid(sname) ) {
            found = 1;
            break;
        }
    }
    
    if( ( (!found) \
         && (LWA_getsid(sname) != 0) ) ) {
        nsid = nsid+1; /* one more subsystem has been defined */
        if (nsid>LWA_MAX_SID) {
            printf("[%s] FATAL: nsid=%d is greater than LWA_MAX_SID\n",ME,nsid);      
            exit(EXIT_FAILURE);
        }
        
        /* get system ID */
        sid[nsid-1] = LWA_getsid(sname);
    } 
    
    } /* while (!feof(fid)) */

  fclose(fid);  
  printf("[%s] Completed paring ms_init_replay log\n",ME);

  /* encode list of subsystem IDs activated for use in ms_exec invocation*/
  /* encoded as a single base-10 number, whose bits in base-2 representation */
  /* indicate the presense/absence of that subsystem ID */
  sidsum = 0;
  for (n=0;n<nsid;n++) sidsum += (1 << (sid[n]-1));
  sprintf(sidlist,"%ld",sidsum); 
  //printf("sidlist: <%s>\n",sidlist);

  printf("[%s] Launching ms_exec_replay...\n",ME);

  /* Launch executive */
  ms_exec_pid = fork();               /* create duplicate process */
  switch (ms_exec_pid) {
    case -1: /* error */
      printf("[%s] FATAL: fork for ms_exec_replay failed\n",ME);
      exit(EXIT_FAILURE);
    case 0: /* fork() succeeded; we are now in the child process */
      err = execl("./ms_exec_replay","ms_exec_replay",sidlist,ms_init_filename,run_as_loop,NULL); /* launch ms_exec_replay */
      /* if we get to this point then we failed to launch */
      if (err==-1) {
        printf("[%s] FATAL: failed to exec() ms_exec_replay\n",ME); 
        }     
      exit(EXIT_FAILURE);
      break; 
    default: /* fork() succeeded; we are now in the parent process */
      break;
    } /* switch (ms_exec_pid) */

  printf("[%s] Launching ms_mdre_replay...\n",ME);
  
  /* Launch ms_mdre_ip */
  ms_mdre_pid = fork();               /* create duplicate process */
  switch (ms_mdre_pid) {
    case -1: /* error */
      printf("[%s] FATAL: fork for ms_mdre_replay failed\n",ME);
      exit(EXIT_FAILURE);
    case 0: /* fork() succeeded; we are now in the child process */
      err = execl("./ms_mdre_replay","ms_mdre_replay",ms_init_filename,run_as_loop,NULL); /* launch ms_exec */
      /* if we get to this point then we failed to launch */
      if (err==-1) {
        printf("[%s] FATAL: failed to exec() ms_mdre_replay\n",ME); 
        }     
      exit(EXIT_FAILURE);
      break; 
    default: /* fork() succeeded; we are now in the parent process */
      break;
    } /* switch (ms_mdre_pid) */

  /* process continues to run */
  sleep(3);

  printf("[%s] exit(EXIT_SUCCESS)\n",ME);  
  exit(EXIT_SUCCESS);
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// me_init_replay.c: J. Dowell, UNM, 2020 Jun 24
//   .1 Created from ms_init.c
// ms_init.c: S.W. Ellingson, Virginia Tech, 2010 Oct 16
//   .1 Now launching ms_mdre_ip as a standalong process concurrent with ms_exec
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Aug 02
//   .1 Cleaning up console messages (svn rev 22) (svn rev 23)
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Jul 21
//   .1 implementing expanded command line for dat2dbm (IP_ADDRESS etc.)
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Jul 20
//   .1 init file can now have >1 argument
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Jul 17
//   .1 change from mq_struct to LWA_cmd_struct for message passing
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Jul 13
//   .1 bringing into common codeset; mib.h -> LWA_MCS.h
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Jul 07 
//   .1 expanding use of message queues
//   .2 mc_mcic handshake using message queue
//   .3 implementing null subsystems
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Jul 05 
//   -- adding mibinit command
//   -- added message queue (very preliminary)
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Jul 01
//   -- ms_exec now forks; ms_init stays alive 
// ms_init.c: S.W. Ellingson, Virginia Tech, 2009 Jun 27 

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
