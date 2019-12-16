// me_exec.c: S.W. Ellingson, Virginia Tech, 2014 Feb 10
// ---
// COMPILE: gcc -o me_exec me_exec.c -I../common
// ---
// COMMAND LINE: me_exec <flagset> <me_tpcom_pid> <me_inproc_pid>
//   <flagset>:  long unsigned integer containing sum of flags.  Default is zero.
//     1 (ME_FLAG_NO_SCH): Don't try to contact MCS/Scheduler (DIAG1 observations should still work)
//   <me_tpcom_pid>: pid of the me_tpcom process (used to kill it)
//   <me_inproc_pid>: pid of the me_tpcom process (used to kill it)
// ---
// See end of this file for history.

#include "me.h"        /* includes mcs.h */

/* defined global static */
static int bFlg_NoSch = 0; /* ME_FLAG_NO_SCH flag */

#include "me_exec_1.c" /* functions called from main(); some #defines */

/*******************************************************************/
/*** main() ********************************************************/
/*******************************************************************/

int main ( int narg, char *argv[] ) {

  /* command line parameters */
  long unsigned int flagset;
  //char filename_ssmif[ME_FILENAME_MAX_LENGTH];
  pid_t me_tpcom_pid;  /* PID for me_tpcom process */  
  pid_t me_inproc_pid; /* PID for me_inproc process */ 

  struct ssmif_struct s; 
  struct sc_struct sc;   /* structure describing station configuration/status */
  struct sdm_struct sdm; /* station dynamic MIB (SDM) */

  struct me_session_queue_struct sq; /* session queue */

  /* variables pertaining to the TCP/IP command interface */
  int server_len;
  int client_len;  
  int server_sockfd;                
  int client_sockfd;   
  struct sockaddr_in server_address; /* for network sockets */
  struct sockaddr_in client_address; /* for network sockets */
  int flags; /* used as part of scheme for changing accept()'s blocking behavior */
  struct me_cmd_struct c;

  /* variables pertaining to the TCP/IP interface to MCS/Sch */
  int sockfd_sch;             /* socket file discriptor, to MCS/Sch */
  struct sockaddr_in address; /* for network sockets */

  FILE* fp;
  int i;
  int iErr;
  int eQuit=0;  /* 1 means shutdown commanded; 2 means shutdown due to error */
  int bWriteMESS=0;

  /* log file stuff */
  FILE* fpl; 
  struct me_session_queue_struct *sq_ptr=NULL;  /* used as dummy argument in me_log() */
  char msg[ME_LOG_MAX_MSG_LENGTH]; 

  struct timeval tv;
  struct timeval tv_last_push_to_tp;
  struct timeval tv_last_pull_from_tp;
  struct timeval tv_last_poll_of_subsystems;

  long int reference;
  int err;
  
  struct timezone tz;
  struct tm *tm;      /* from sys/time.h */
  char cmd[1024];

  /* First, announce thyself */
  printf("[%d/%d] I am me_exec\n",ME_INIT,getpid());

  /* sometimes first call to gettimeofday() returns something bogus, so let's get that out of the way */
  gettimeofday( &tv, NULL ); 

  /* Parse command line */

  if (narg<2) {
    printf("[%d/%d] FATAL: flagset not specified\n",ME_ME_C,getpid());
    return;
    }
  sscanf(argv[1],"%lu",&flagset);
  //printf("[%d/%d] Input: me_tpcom_pid=%d\n",ME_ME_C,getpid(),me_tpcom_pid);

  if (narg<3) {
    printf("[%d/%d] FATAL: me_tpcom_pid not specified\n",ME_ME_C,getpid());
    return;
    }
  sscanf(argv[2],"%d",&me_tpcom_pid);
  //printf("[%d/%d] Input: me_tpcom_pid=%d\n",ME_ME_C,getpid(),me_tpcom_pid);

  if (narg<4) {
    printf("[%d/%d] FATAL: me_inproc_pid not specified\n",ME_ME_C,getpid());
    return;
    }
  sscanf(argv[3],"%d",&me_inproc_pid);

  /******************************************/
  /*** open log file ************************/
  /******************************************/
  fpl = fopen(ME_LOG_FILENAME,"a"); /* me_init may have already started log, so use "a" */
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "I am me_exec", sq_ptr, 0 );  

  /******************************************/
  /*** look at flagset **********************/
  /******************************************/
  sprintf(msg,"flagset = %lu",flagset);
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 ); 
  bFlg_NoSch = ( flagset & ME_FLAG_NO_SCH ) != 0;

  /******************************************/
  /*** Directory setup **********************/
  /******************************************/
  /* This is done by me_init, which puts the directories in a known state */

  /***************************************/
  /*** Initialize the session queue ******/
  /***************************************/

  sq.N = -1; 
  sq.bInProcessReqd = 0;
  sq.bInProc = 0;
  sq.bReady = 0;
  sq.bOutProcessReqd = 0;
  for ( i=0; i<ME_MAX_SESSION_QUEUE_LENGTH; i++ ) {
    me_initialize_session_queue_entry( &sq, i );
    }
  me_session_queue_write(&sq);
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "Session queue initialized", sq_ptr, 0 );  


  /***************************************/
  /*** Setting up SSMIF-related things ***/
  /***************************************/

  /* Read SSMIF */
  if (!(fp = fopen("state/ssmif.dat","rb"))) {
    printf("[%d/%d] FATAL: unable to fopen 'state/ssmif.dat'\n",ME_ME_C,getpid());
    sprintf(msg,"FATAL: unable to fopen 'state/ssmif.dat'");
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 ); 
    return;
    }
  fread(&s,sizeof(s),1,fp);
  fclose(fp);
  //printf("[%d/%d] Successfully read SSMIF '%s'\n",ME_ME_C,getpid(),filename_ssmif);
  sprintf(msg,"Successfully read state/ssmif.dat");
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 ); 

  /* Initialize the station configuration structure */
  me_sc_init( &sc );              
 
  /* assemble information about analog signal path & status */
  iErr = me_sc_MakeASM( s, &sc ); 
  if (iErr>0) {
    printf("[%d/%d] FATAL: me_sc_MakeASM() failed with error %d\n",ME_ME_C,getpid(),iErr);
    sprintf(msg,"FATAL: me_sc_MakeASM() failed with error %d",iErr);
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
    return;
    }
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_sc_MakeASM() completed", sq_ptr, 0 );

  /* assemble information about digital signal path & status */
  iErr = me_sc_MakeDSM( s, &sc ); 
  if (iErr>0) {
    printf("[%d/%d] FATAL: me_sc_MakeDSM() failed with error %d\n",ME_ME_C,getpid(),iErr);
    sprintf(msg,"FATAL: me_sc_MakeDSM() failed with error %d",iErr);
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
    return;
    }
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_sc_MakeDSM() completed", sq_ptr, 0 );

  /* construct default beam "gains" file (default.gf) from state/default.gft, masking out bad dipoles */
  me_make_gf( s );

  /***************************************/
  /*** Setting up SDM-related things *****/
  /***************************************/
  me_init_sdm( s, sc, &sdm ); 
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_init_sdm() completed", sq_ptr, 0 );
  me_sdm_write( &sdm );
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_sdm_write() completed", sq_ptr, 0 );

  /***************************************/
  /*** construct & write out mess.dat ****/
  /***************************************/
  iErr = me_write_mess( s, sc, sq, fpl );
  if (iErr>0) {
    printf("[%d/%d] FATAL: me_write_mess() failed with error %d\n",ME_ME_C,getpid(),iErr);
    sprintf(msg,"FATAL: me_write_mess() failed with error %d",iErr);
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
    return;
    }  

  /***************************************************************************/
  /* set up sockets interface for user-side communication. (I am the server) */ 
  /***************************************************************************/

  server_sockfd = socket(             /* create socket */
                         AF_INET,     /* domain; network sockets */
                         SOCK_STREAM, /* type (TCP-like) */
                         0);          /* protocol (normally 0) */
  if (server_sockfd == -1) {
    printf("[%d/%d] FATAL: socket() failed\n",ME_ME_C,getpid());
    perror("me_exec");
    sprintf(msg,"FATAL: socket() failed");
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
    return; 
    }
  /* name socket */
  server_address.sin_family      = AF_INET;                /* network sockets */  
  server_address.sin_addr.s_addr = inet_addr(LWA_IP_MEE);  /* network sockets */
  server_address.sin_port        = htons(LWA_PORT_MEE);    /* network sockets */
  server_len = sizeof(server_address);

  i = bind( server_sockfd, 
           (struct sockaddr *) &server_address, 
            server_len );
  if (i == -1) {
    printf("[%d/%d] FATAL: bind() failed (see error message below)\n",ME_ME_C,getpid());
    perror("me_exec");
    printf("[%d/%d] If message above is ``Address already in use'':\n",ME_ME_C,getpid());
    printf("[%d/%d]   (1) Kill any 'me' processes (e.g., '$ sh ./me_crash.sh').\n",ME_ME_C,getpid());
    printf("[%d/%d]   (2) Wait a few seconds before trying this again.\n",ME_ME_C,getpid());
    sprintf(msg,"FATAL: bind() failed");
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
    return; 
    }

  /* create a connection queue */
  i = listen(server_sockfd,
             32 );           /* backlog */
  if (i == -1) {
    printf("[%d/%d] FATAL: me_exec: listen() failed\n",ME_ME_C,getpid());
    perror("me_exec");
    sprintf(msg,"FATAL: me_exec: listen() failed\n");
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
    return; 
    }
  /* change accept() from blocking to non-blocking */
  flags = fcntl( server_sockfd, F_GETFL, 0 );
  fcntl( server_sockfd, F_SETFL, O_NONBLOCK|flags );
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "User-side TCP socket setup complete", sq_ptr, 0 );


  /****************************************************************/
  /* set up sockets interface to MCS/Scheduler. (I am the client) */ 
  /****************************************************************/

  if (bFlg_NoSch) {

      me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, 
              "ME_FLAG_NO_SCH set; skipping setup of socket to MCS/Sch", sq_ptr, 0 );

    } else {

      /* create socket */
      sockfd_sch = socket(
                     AF_INET,     /* domain; network sockets */
                     SOCK_STREAM, /* type (TCP-like) */
                     0);          /* protocol (normally 0) */
    
      /* network sockets */
      address.sin_family = AF_INET;
      address.sin_addr.s_addr = inet_addr(LWA_IP_MSE);
      address.sin_port = htons(LWA_PORT_MSE);

     /* connect socket to server's socket */
     if (connect( sockfd_sch, (struct sockaddr *) &address, sizeof(address))==-1) {
       printf("[%d/%d] FATAL: me_exec: connect() failed\n",ME_ME_C,getpid());
       perror("me_exec");
       sprintf(msg,"FATAL: me_exec: connect() failed\n");
       me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
       return;
       }

    /* Send PNG to MCS to make sure Scheduler is responding */
    err = mesi( &sockfd_sch, "MCS", "PNG", "", "today", "asap", &reference );
    sprintf(msg,"Pinged MCS/Sch: mesi() returned %d, reference=%ld",err,reference);
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
    if (err!=MESI_ERR_OK) {
      me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "FATAL (see error code in above line)", sq_ptr, 0 );
      printf("[%d/%d] FATAL: me_exec: Initial ping of MCS/Sch failed\n",ME_ME_C,getpid());
      return;  
      }

    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "MCS/Sch TCP socket setup complete", sq_ptr, 0 );

    }

  /* FIXME */
  /*****************************************************************************/
  /* temporary kludge -- shouldn't really do this until subsystems are checked */
  /*****************************************************************************/
  gettimeofday(&tv,NULL);
  sdm.station.summary = LWA_SIDSUM_WARNING; /* as a reminder we're running blind */ 
  strcpy(sdm.station.info,""); 
  sdm.station.tv = tv; 
  me_sdm_write( &sdm );
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_sdm_write() completed", sq_ptr, 0 );

  /*************************************/
  /* initialize times for timed events */ 
  /*************************************/
  gettimeofday( &tv_last_push_to_tp, NULL );
  gettimeofday( &tv_last_pull_from_tp, NULL );

  printf("[%d/%d] me_exec: Setup complete; now running.  Consult '%s' for messages.\n",ME_ME_C,getpid(), ME_LOG_FILENAME);
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "Setup complete. Entering main loop", sq_ptr, 0 );
  /***************************************/
  /***************************************/
  /*** main program loop *****************/
  /***************************************/
  /***************************************/

  while (eQuit==0) {  /* any higher value means quit */

    /* get current time */
    gettimeofday( &tv, NULL );

    /* for each session in the _READY or _OBSERVING state, turn the crank on the command queue */
    if (sq.bReady) { /* at least one session is in one of these states */
      iErr = me_action( &sq, fpl );
      switch (iErr) {
        case ME_ACTION_ERR_OK: bWriteMESS=1; /* FIXME */ break;
        default: 
          printf("[%d/%d] FATAL: me_action() failed with error %d\n",ME_ME_C,getpid(),iErr);
          sprintf(msg,"FATAL: me_action() failed with error %d",iErr);
          me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
          eQuit=2; //return;
          break;
        }  
      }    
     
    /* check TCP/IP input channel for command */

    client_len = sizeof(client_address);
    client_sockfd = accept( server_sockfd,
                            (struct sockaddr *) &client_address, 
                            &client_len );  

    if (!(client_sockfd==-1)) { /* we have a connection... */                          

      /* read it into a LWA_cmd_struct structure */
      read(client_sockfd,&c,sizeof(struct me_cmd_struct));

      /* process command */
      switch (c.cmd) {
        case ME_CMD_NUL: 
          me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "Received ME_CMD_NUL", sq_ptr, 0 );
          break;
        case ME_CMD_SHT: 
          me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "Received ME_CMD_SHT", sq_ptr, 0 );
          c.cmd=ME_CMD_NUL; /* this value used to acknowledge command */
          eQuit=1; 
          break;
        case ME_CMD_STP:
          sprintf(msg,"Received ME_CMD_STP; c.args='%s'",c.args);
          me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
          c.cmd = me_stp( &sq, fpl, c.args ); /* process the STP command */
          break;
        default:         
          sprintf(msg,"Received unrecognized command code %d",c.cmd);
          me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 ); 
          c.cmd=ME_CMD_ERR;     
          break;
        } /* switch */

      /* report back to requester, and close connection */
      write(client_sockfd,&c,sizeof(struct me_cmd_struct));
      close(client_sockfd);

      } /* if (!(client_sockfd==-1)) {   ...we have a connection... */  


    /* if any session is in the _AWAITING_OUTPROCESSING state, knock one out */
    if (sq.bOutProcessReqd) { 
      iErr = me_session_queue_outprocess( &sq, fpl ); 
      switch (iErr) {
        case ME_SESSION_QUEUE_OUTPROCESS_ERR_OK: bWriteMESS = 1; break;
        default: 
          printf("[%d/%d] FATAL: me_session_queue_outprocess() failed with error %d\n",ME_ME_C,getpid(),iErr);
          sprintf(msg,"FATAL: me_session_queue_outprocess() failed with error %d",iErr);
          me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
          eQuit=2; //return;
          break;
        }     
      }

    /* periodically check TP for new observations */
    if ( LWA_timediff(tv,tv_last_pull_from_tp) > ME_INTERVAL_PULL_FROM_TP ) {
      gettimeofday( &tv_last_pull_from_tp, NULL );
      iErr = me_pull_from_tp( &sq, fpl );
      switch (iErr) {
        case ME_PULL_FROM_TP_ERR_SESSION_QUEUE_ADD_FAIL: 
          printf("[%d/%d] FATAL: me_pull_from_tp() failed with error %d\n",ME_ME_C,getpid(),iErr);
          sprintf(msg,"FATAL: me_session_queue_outprocess() failed with error %d",iErr);
          me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
          eQuit=2; //return;
          break;
        default: break;
        }
      }

    /* Deal with sessions in queue that are in the state MESQ_STATE_AWAITING_INPROCESSING. */
    /* These need to be processed to determine start time, #obs, etc ... */
    if (sq.bInProcessReqd) { 
      iErr = me_session_queue_inprocess( &sq, fpl ); 
      switch (iErr) {
        case ME_SESSION_QUEUE_INPROCESS_ERR_OK: bWriteMESS=1; break;
        default: 
          printf("[%d/%d] FATAL: me_session_queue_inprocess() failed with error %d\n",ME_ME_C,getpid(),iErr);
          sprintf(msg,"FATAL: me_session_queue_inprocess() failed with error %d",iErr);
          me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
          eQuit=2; //return;
          break;
        }
      }

    /* Check to see if any session in the _INPROCESSING state (i.e., out to me_inproc) is ready */
    if (sq.bInProc) { /* at least one session is in this state */
      iErr = me_inproc_readback( &sq, fpl );
      switch (iErr) {
        case ME_INPROC_READBACK_ERR_OK:                   break;
        case ME_INPROC_READBACK_ERR_OK_HIT: bWriteMESS=1; break; /* possibility mess.dat should change */
        default: 
          printf("[%d/%d] FATAL: me_inproc_readback() failed with error %d\n",ME_ME_C,getpid(),iErr);
          sprintf(msg,"FATAL: me_inproc_readback() failed with error %d",iErr);
          me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
          eQuit=2; //return;
          break;
        }  
      }

    /* construct & write out mess.dat */
    if (bWriteMESS) { 
      bWriteMESS=0;
      iErr = me_write_mess( s, sc, sq, fpl );
      if (iErr>0) {
        //printf("[%d/%d] FATAL: me_write_mess() failed with error %d\n",ME_ME_C,getpid(),iErr);
        sprintf(msg,"FATAL: me_write_mess() failed with error %d",iErr);
        me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
        eQuit=2; //return;
        }
      }

    /* periodically poll subsystems for status */
    if ( LWA_timediff(tv,tv_last_poll_of_subsystems) > ME_INTERVAL_POLL_SUBSYSTEMS ) {
      gettimeofday( &tv_last_poll_of_subsystems, NULL );

#ifdef USE_ADP
      err = mesi(NULL,"ADP","RPT","SUMMARY",    "today","asap",&reference);
      err = mesi(NULL,"ADP","RPT","INFO",       "today","+1",  &reference);
#else
      err = mesi(NULL,"DP_","RPT","SUMMARY",    "today","asap",&reference);
      err = mesi(NULL,"DP_","RPT","INFO",       "today","+1",  &reference);
#endif
      err = mesi(NULL,"ASP","RPT","SUMMARY",    "today","+2",  &reference);
      err = mesi(NULL,"SHL","RPT","SUMMARY",    "today","+3",  &reference);
      err = mesi(NULL,"SHL","RPT","CURRENT-R1", "today","+4",  &reference);
#ifdef USE_ADP
      err = mesi(NULL,"SHL","RPT","CURRENT-R2", "today","+5",  &reference);
      err = mesi(NULL,"SHL","RPT","CURRENT-R3", "today","+5",  &reference);
      err = mesi(NULL,"SHL","RPT","CURRENT-R4", "today","+5",  &reference);
#else
      err = mesi(NULL,"SHL","RPT","CURRENT-R3", "today","+5",  &reference);
#endif
      err = mesi(NULL,"SHL","RPT","TEMPERATURE","today","+6",  &reference);

      err = mesi(NULL,"DR1","RPT","SUMMARY",    "today","+10",  &reference);
      err = mesi(NULL,"DR1","RPT","OP-TYPE",    "today","+11",  &reference);
      //err = mesi(NULL,"DR1","RPT","SYN",        "today","+12",  &reference);

      err = mesi(NULL,"DR2","RPT","SUMMARY",    "today","+20",  &reference);
      err = mesi(NULL,"DR2","RPT","OP-TYPE",    "today","+21",  &reference);
      //err = mesi(NULL,"DR2","RPT","SYN",        "today","+22",  &reference);

      err = mesi(NULL,"DR3","RPT","SUMMARY",    "today","+30",  &reference);
      err = mesi(NULL,"DR3","RPT","OP-TYPE",    "today","+31",  &reference);
      //err = mesi(NULL,"DR3","RPT","SYN",        "today","+32",  &reference);

      err = mesi(NULL,"DR4","RPT","SUMMARY",    "today","+40",  &reference);
      err = mesi(NULL,"DR4","RPT","OP-TYPE",    "today","+41",  &reference);
      //err = mesi(NULL,"DR4","RPT","SYN",        "today","+42",  &reference);

#ifndef USE_ADP
      err = mesi(NULL,"DR5","RPT","SUMMARY",    "today","+50",  &reference);
      err = mesi(NULL,"DR5","RPT","OP-TYPE",    "today","+51",  &reference);
      //err = mesi(NULL,"DR5","RPT","SYN",        "today","+52",  &reference);
#endif

      //if (err!=MESI_ERR_OK) {
      //   sprintf(longmsg,"FATAL: in me_action(), mesi(NULL,'%s','%s','%s','%s','%s',*) returned code %d",
      //                 LWA_sid2str(action.sid), 
      //                  LWA_cmd2str(action.cid), 
      //                  data,
      //                  sdate, stime,
      //                  err);

      //iErr = me_pull_from_tp( &sq, fpl );
      //switch (iErr) {
      //  case ME_PULL_FROM_TP_ERR_SESSION_QUEUE_ADD_FAIL: 
      //    printf("[%d/%d] FATAL: me_pull_from_tp() failed with error %d\n",ME_ME_C,getpid(),iErr);
      //    sprintf(msg,"FATAL: me_session_queue_outprocess() failed with error %d",iErr);
      //    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
      //    eQuit=2; //return;
      //    break;
      //  default: break;
      //  }
      } /* if ( LWA_timediff(tv,tv_last_poll_of_subsystems) > ME_INTERVAL_POLL_SUBSYSTEMS ) */

    /* flushing log file buffer */
    fflush(fpl);   

    /* avoiding busy wait */
    usleep(1000); /* Go to sleep for 1000 microseconds = 1 ms */

    } /* while () -- end of main program loop */

  /* let world know we're shutting down */
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "Beginning shutdown process...", sq_ptr, 0 );

  /* update sdm */
  gettimeofday(&tv,NULL);
  sdm.station.summary = LWA_SIDSUM_SHUTDWN; 
  strcpy(sdm.station.info,""); 
  sdm.station.tv = tv; 
  me_sdm_write( &sdm );
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_sdm_write() completed", sq_ptr, 0 );

  /* shutdown the socket connection */
  close(server_sockfd);     
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "Closed user-side TCP socket", sq_ptr, 0 );

  if (!bFlg_NoSch) {
    close(sockfd_sch);
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "Closed MCS/Sch TCP socket", sq_ptr, 0 );
    }

  /* kill me_tpcom */
  kill(me_tpcom_pid,SIGKILL);
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "Sent SIGKILL to me_tpcom", sq_ptr, 0 );

  /* kill me_inproc */
  kill(me_inproc_pid,SIGKILL);
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "Sent SIGKILL to me_inproc", sq_ptr, 0 );

  /* close log file */
  me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "Shutdown complete. Bye.", sq_ptr, 0 );
  fclose(fpl);
  
  /* just in case */
  fcloseall();
  
  /* put it somewhere safe */
  /* get the current time in conventional format */
  gettimeofday(&tv,&tz); 
  tm = gmtime(&tv.tv_sec);
  sprintf(cmd,"cp %s meelog_%02d%02d%02d_%02d%02d.txt", ME_LOG_FILENAME, 
         (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min);
  system(cmd);

  /* Say goodnight Gracie */
  printf("[%d/%d] me_exec: Shutdown complete. Bye.\n",ME_ME_C,getpid());
  fflush(stdout);
  
  //sleep(3); /* give time for message to write out without borking command prompt */
  return eQuit-1;
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// me_exec.c: J. Dowell, UNM, 2019 Dec 16
//  .1 Added support for a fourth data recorder for ADP-based stations
// me_exec.c: J. Dowell, UNM, 2019 Oct 30
//  .1 Convert to using normal GDBM for the database
// me_exec.c: J. Dowell, UNM, 2018 Feb 13
//  .1 Added support for a third data recorder for ADP-based stations
// me_exec.c: J. Dowell, UNM, 2018 Jan 29
//  .1 Revisited the 121204 me_exec_1.c kludge to try and keep MCS/exec running by adjusting eErr
//  .2 Increased the queue size for listen()
// me_exec.c: J. Dowell, UNM, 2017 Aug 23
//  .1 Added support for flushing the mesq.dat file on restart
// me_exec.c: J. Dowell, UNM, 2016 Aug 25
//  .1 Added support for a second data recorder for ADP-based stations
// me_exec.c: J. Dowell, UNM, 2015 Aug 28
//  .1: Added a call to save the meelog.txt file on shutdown, added support for ADP
// me_exec.c: S.W. Ellingson, Virginia Tech, 2014 Feb 10
//   .1: Adding meeix "STP" command -- allows termination of an observing session after submission
// me_exec.c: S.W. Ellingson, Virginia Tech, 2012 Mar 01
//   .1: Adding periodic polling of subsystems
// me_exec.c: S.W. Ellingson, Virginia Tech, 2011 Dec 12
//   .1: Fixed problem that me_action() discard action if early; now it just rewinds the .cs file
// me_exec.c: S.W. Ellingson, Virginia Tech, 2011 Mar 25
//   .1: Moved functions into me_exec_1.c, which is #include'd
// me_exec.c: S.W. Ellingson, Virginia Tech, 2011 Mar 12
//   .1: Renamed "me" to "me_exec"
// me.c: S.W. Ellingson, Virginia Tech, 2011 Mar 09
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================


