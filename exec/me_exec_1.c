
#define ME_INTERVAL_PULL_FROM_TP      1000 /* [ms] interval for me_pull_from_tp */
#define ME_INTERVAL_POLL_SUBSYSTEMS 300000 /* [ms] interval for polling subsystems (me_exec.c) */

/*******************************************************************/
/*** me_action_cmd_msg() *******************************************/
/*******************************************************************/

void me_action_cmd_msg( char *msg,
                        struct me_action_struct *action,
                        char *data ) {
  long int mjd;
  long int mpm;
  int eDataFormat = 1; /* default is to show data as string */

  LWA_timeval( &(action->tv), &mjd, &mpm ); /* convert timeval to (mjd,mpm) */

  sprintf(msg,"%5ld %8ld %1d %3s %3s %d",
               mjd, mpm, action->bASAP,
                             LWA_sid2str(action->sid),
                                 LWA_cmd2str(action->cid),
                                     action->len );

  //switch (action->sid) { /* decide format based on subsystem */
  //  /* FIXME */
  //  }
  if ((action->len)<1) { eDataFormat=0; }
  //printf("action->len=%d, eDataFormat=%d\n",action->len,eDataFormat);
  switch (eDataFormat) {
    case 1: /* show entire thing as a string in the same line */
      sprintf(msg,"%s '%s'",msg,data);
      break;
    default: /* includes 0 */
      break;
    }

  return;
  } /* me_inproc_cmd_log() */

/*******************************************************************/
/*** me_initialize_session_queue_entry() ***************************/
/*******************************************************************/
/* initialize one entry (session) in a session queue */

void me_initialize_session_queue_entry( struct me_session_queue_struct *sq, int i) {
  sq->eState[i] = MESQ_STATE_NOT_USED;
  sq->iCurrentObs[i] = 0;
  sprintf(sq->sFilename[i],"");
  sq->fpc[i] = NULL;
  sq->ncr[i] = 0;
  sq->oc[i] = ME_OC_OK;
  sprintf(sq->ocs[i],"");
  sprintf(sq->ssf[i].PROJECT_ID,"");
  sq->ssf[i].SESSION_ID = 0;
  sq->ssf[i].SESSION_NOBS = 0;
  /* not bothering to initialize the remaining sq->ssf[] members since only the above are */
  /* are used before the memcpy initialization from the SSF */
  return;
  } /* me_initialize_session_queue_entry() */


/*******************************************************************/
/*** me_session_queue_write() **************************************/
/*******************************************************************/
/* write the session queue to a file */

int me_session_queue_write( struct me_session_queue_struct *sq ) {
  int i;
  FILE *fp;
  /* save information */
  fp = fopen("state/mesq.dat","w");
  fprintf(fp,"     S PROJ     SESS B    MJD       MPM       DUR NOBS C\n");
  for (i=0;i<=(sq->N);i++) {
    fprintf(fp,"%4d %1d %8s %4u %1hd %6lu %9lu %9lu %4u %1hu %s\n",
                i,  sq->eState[i],
                        sq->ssf[i].PROJECT_ID,
                            sq->ssf[i].SESSION_ID,
                                sq->ssf[i].SESSION_DRX_BEAM,
                                     sq->ssf[i].SESSION_START_MJD,
                                          sq->ssf[i].SESSION_START_MPM,
                                              sq->ssf[i].SESSION_DUR,
                                                    sq->ssf[i].SESSION_NOBS,
                                                        sq->ssf[i].SESSION_CRA,
                                                             sq->sFilename[i]  );
    }
  fclose(fp);
  return 0;
  } /* me_session_queue_write() */

/*******************************************************************/
/*** me_session_queue_add() ****************************************/
/*******************************************************************/
/* add a session to the session queue */

#define ME_SESSION_QUEUE_ADD_ERR_OK              0
#define ME_SESSION_QUEUE_ADD_ERR_BUFFER_OVERFLOW 1

int me_session_queue_add( 
  struct me_session_queue_struct *sq, 
  char *sProjectID,
  int  iSessionID, 
  char *filename, 
  FILE* fpl                                    /* for logging */
  ) {

  int eErr=ME_SESSION_QUEUE_ADD_ERR_OK;
  int i;
  struct me_session_queue_struct *sq_ptr=NULL; /* for logging */

  /* find first open queue position */
  i=0;
  while ( (i<=(sq->N)) && ((sq->eState[i])!=MESQ_STATE_NOT_USED) ) { i++; }
  if ( (i>(sq->N)) && (i>=ME_MAX_SESSION_QUEUE_LENGTH) ) { 
    //printf("[%d/%d] me_session_queue_add(): queue overflow\n",ME_ME_C,getpid());
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_session_queue_add(): queue overflow", sq_ptr, 0 );
    return ME_SESSION_QUEUE_ADD_ERR_BUFFER_OVERFLOW;
    }

  /* if this position is the new highest-indexed used one, update sq->N */  
  if (i>(sq->N)) { sq->N = i; }
 
  sq->bInProcessReqd = 1; /* set flag indicating need for in-processing */

  /* fill in info for this queue position */
  sq->eState[i] = MESQ_STATE_AWAITING_INPROCESSING;
  sprintf(sq->ssf[i].PROJECT_ID,"%s",sProjectID);
  sq->ssf[i].SESSION_ID = iSessionID;
  sprintf(sq->sFilename[i],"%s",filename);
  sq->ssf[i].SESSION_NOBS = 0; /* don't really know what this is yet */  

  //printf("debug: sq->ssf[i].PROJECT_ID='%s'\n",sq->ssf[i].PROJECT_ID);
  //printf("debug: sq->sFilename[i]='%s'\n",sq->sFilename[i]);

  /* log it */
  me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_INFO, "me_session_queue_add(): added to queue", sq, i );

  /* save it */
  me_session_queue_write(sq);
 
  return eErr;
  }

/*******************************************************************/
/*** me_session_queue_inprocess() **********************************/
/*******************************************************************/
/* in-process a session in the session queue */

#define ME_SESSION_QUEUE_INPROCESS_ERR_OK               0
#define ME_SESSION_QUEUE_INPROCESS_ERR_NO_SESSION_FOUND 1
#define ME_SESSION_QUEUE_INPROCESS_ERR_CANT_OPEN_SES    2
#define ME_SESSION_QUEUE_INPROCESS_ERR_CANT_OPEN_INP    4
//#define ME_SESSION_QUEUE_INPROCESS_ERR_CANT_FORK        8
//#define ME_SESSION_QUEUE_INPROCESS_ERR_CANT_EXECL      16

int me_session_queue_inprocess( 
  struct me_session_queue_struct *sq,
  FILE* fpl  /* for logging */
  ) {

  struct ssf_struct ssf;
  struct me_session_queue_struct *sq_ptr=NULL; /* for logging */
  char msg[ME_LOG_MAX_MSG_LENGTH];

  int i;
  int eErr=ME_SESSION_QUEUE_INPROCESS_ERR_OK;
  FILE *fp;
  char filename[ME_FILENAME_MAX_LENGTH];

  /* find a session to be in-processed */
  i=0;
  while ( ((sq->eState[i])!=MESQ_STATE_AWAITING_INPROCESSING) && (i<(sq->N)) ) { i++; }
  if ((sq->eState[i])!=MESQ_STATE_AWAITING_INPROCESSING) {
    //printf("[%d/%d] me_session_inprocess(): couldn't find a session to process\n",ME_ME_C,getpid());
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_session_inprocess(): couldn't find a session to process", sq_ptr, 0 );
    return ME_SESSION_QUEUE_INPROCESS_ERR_NO_SESSION_FOUND;
    }
 
  /* load this SSF */
  sprintf(filename,"sinbox/%s",sq->sFilename[i]);
  if (!(fp = fopen(filename,"rb"))) {
    //printf("[%d/%d] me_session_inprocess(): can't open '%s'\n",ME_ME_C,getpid(),filename);
    sprintf(msg,"me_session_inprocess(): can't open '%s'",filename);
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
    return ME_SESSION_QUEUE_INPROCESS_ERR_CANT_OPEN_SES;
    }
  fread(&ssf,sizeof(ssf),1,fp);
  fclose(fp);

  /* copy information from ssf into session queue */
  memcpy( &(sq->ssf[i]), &ssf, sizeof(ssf) );
  //void *memcpy(void *dest, const void *src, size_t n);

  //printf("debug: sq->sFilename[i]='%s'\n",sq->sFilename[i]);
  //printf("debug: sq->ssf[i].PROJECT_ID='%s'\n",sq->ssf[i].PROJECT_ID);

  /************************/
  /* Send it to me_inproc */
  /************************/

  /* Write sq->ssf[i] to file */ 
  sprintf(filename,"sinbox/%s_%04u.inp",sq->ssf[i].PROJECT_ID,sq->ssf[i].SESSION_ID);
  if (!(fp = fopen(filename,"wb"))) {
    sprintf(msg,"me_session_inprocess(): can't open '%s'",filename);
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
    return ME_SESSION_QUEUE_INPROCESS_ERR_CANT_OPEN_INP;
    }
  fwrite(&(sq->ssf[i]),sizeof(sq->ssf[i]),1,fp);
  fclose(fp);  

  /* (Next time me_inproc checks, it should see this file and act */

  /* promote state of this observation */
  sq->eState[i] = MESQ_STATE_INPROCESSING;  
  sq->bInProc = 1; /* i.e., at least one session is out to me_inproc */

  /* log it */
  me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_INFO, "me_session_inprocess() complete", sq, i );

  /*******************************************/
  /* FIXME: Temporary kludge for development */
  /*******************************************/
  //sq->eState[i] = MESQ_STATE_AWAITING_OUTPROCESSING;
  //sq->bInProc = 0; 
  //sq->bOutProcessReqd = 1;

  /* write out updated session queue */
  me_session_queue_write(sq);

  /* check through queue to see how to set bInProcessReqd flag on exit */
  sq->bInProcessReqd = 0;
  for ( i=0; i<=(sq->N); i++ ) {
    if ((sq->eState[i])==MESQ_STATE_AWAITING_INPROCESSING) { sq->bInProcessReqd = 1; }
    }

  return eErr;
  }


/*******************************************************************/
/*** me_inproc_readback() ******************************************/
/*******************************************************************/
/* readback a session that has been processed by me_inproc */

#define ME_INPROC_READBACK_ERR_OK               0
#define ME_INPROC_READBACK_ERR_OK_HIT           1 /* actually did something; should write mess.dat */
#define ME_INPROC_READBACK_ERR_NO_SESSION_FOUND 2
#define ME_INPROC_READBACK_ERR_CS_NOT_FOUND     3 /* couldn't find cs file */
#define ME_INPROC_READBACK_ERR_CS_BAD_SID       4 /* first command in cs file is not for sid=LWA_SID_MCS */
#define ME_INPROC_READBACK_ERR_CS_BAD_CID       5 /* first command in cs file is not for sid=LWA_SID_MCS */

int me_inproc_readback( 
  struct me_session_queue_struct *sq,
  FILE* fpl /* for logging */
  ) {

  struct ssf_struct ssf;
  struct me_session_queue_struct *sq_ptr=NULL; /* for logging */
  char msg[ME_LOG_MAX_MSG_LENGTH];

  int i;
  int eErr=ME_INPROC_READBACK_ERR_OK;
  char filename[ME_FILENAME_MAX_LENGTH];
  int bDone=0;
  int bFoundOne=0;

  struct me_action_struct action;
  struct stat statbuf;

  /* search through sessions currently in the MESQ_STATE_INPROCESSING state, */
  /* checking each to see if the associated .inp has disappeared  */
  /* (this is me_inproc's way of saying it's done */
  i=0;
  bDone=0;
  bFoundOne=0;
  while (!bDone) {

    /* searching through session queue ... */
    while ( ((sq->eState[i])!=MESQ_STATE_INPROCESSING) && (i<(sq->N)) ) { i++; }

    /* if we're here, it'e either because we found a session that is in the _INPROCESSING state, */
    /* OR we've hit the end of the queue. */

    if ((sq->eState[i])==MESQ_STATE_INPROCESSING) {

        bFoundOne = 1; /* meaning we found a session in this state */

        /* check to see if this session is ready */
        sprintf(filename,"sinbox/%s_%04u.inp",sq->ssf[i].PROJECT_ID,sq->ssf[i].SESSION_ID);        
        if (stat(filename,&statbuf)!=0) { /* it's missing, signaling that this one is ready */
          bDone=1;

          /* queue up script file */
          sprintf(filename,"sinbox/%s_%04u.cs",sq->ssf[i].PROJECT_ID,sq->ssf[i].SESSION_ID);   
          if ( (sq->fpc[i]=fopen(filename,"rb")) == NULL ) { 
            me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_INFO, "me_inproc_readback(): cs file not found", sq, i );
            eErr = ME_INPROC_READBACK_ERR_CS_NOT_FOUND; /* this should be FATAL */
            return;
            }

          /* read first command from command script file */
          /* (first command is nominally LWA_SID_MCS, LWA_CMD_NUL; so we want to check that) */
          fread( &action, sizeof(struct me_action_struct), 1, sq->fpc[i] );
          sq->ncr[i] = 1; /* read our first command */
     
          if ( action.sid != LWA_SID_MCS ) {
            me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_INFO, "me_inproc_readback(): action.sid != LWA_SID_MCS", sq, i );
            fclose(sq->fpc[i]);
            eErr = ME_INPROC_READBACK_ERR_CS_BAD_SID; /* this should be FATAL */
            return;
            }

          switch(action.cid) {
            case LWA_CMD_NUL: /* what we expect to see in the first command */
              sq->eState[i] = MESQ_STATE_READY; /* promote state of this observation */
              sq->bReady = 1;                   /* signal that at least one session is in this state */ 
              me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_INFO, "me_inproc_readback(): This one's ready", sq, i );
              eErr = ME_INPROC_READBACK_ERR_OK_HIT; 
              break;
            case LWA_CMD_ESN: /* end session normally: not really what we expect to see, but not wrong. */
              sq->eState[i] = MESQ_STATE_AWAITING_OUTPROCESSING; /* promote state of this observation */
              sq->bOutProcessReqd = 1;                           /* signal that at least one session is in this state */ 
              me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_INFO, "me_inproc_readback(): LWA_CMD_ESN", sq, i );
              sq->oc[i] += ME_OC_OK; 
              sprintf(sq->ocs[i],"Commanded ESN");
              eErr = ME_INPROC_READBACK_ERR_OK_HIT;               
              break;
            case LWA_CMD_ESF: /* "end session as failed": something went wrong in me_inproc. End session but not me_exec */
              sq->eState[i] = MESQ_STATE_AWAITING_OUTPROCESSING; /* promote state of this observation */
              sq->bOutProcessReqd = 1;                           /* signal that at least one session is in this state */ 
              me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_INFO, "me_inproc_readback(): LWA_CMD_ESF", sq, i );
              sq->oc[i] += ME_OC_RCVD_ESF; 
              sprintf(sq->ocs[i],"Commanded ESF");
              eErr = ME_INPROC_READBACK_ERR_OK_HIT;               
              break;
            default: /* something goofy happened; FATAL */
              sprintf(msg,"me_inproc_readback(): action.cid=%d ('%s') invalid",action.cid,LWA_cmd2str(action.cid));
              me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_INFO, msg, sq, i );
              fclose(sq->fpc[i]);
              eErr = ME_INPROC_READBACK_ERR_CS_BAD_CID; 
              return;
              break;
            } /* switch(action.cid) */
   
          /* if we arrived here, command script checks out and this session has changed state */
          me_session_queue_write(sq); /* write out updated session queue */

          /* check through queue to see how to set bInProc flag on exit */
          sq->bInProc = 0;
          for ( i=0; i<=(sq->N); i++ ) {
            if ((sq->eState[i])==MESQ_STATE_INPROCESSING) { sq->bInProc = 1; }
            }

          } /* if (stat( ... */

        i++; /* we're not necessarily done yet */

      } else { /* we must be at the end of the session queue */

        bDone = 1;
        if (!bFoundOne) {
          me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_inproc_readback(): couldn't find a session to check", sq_ptr, 0 );
          eErr = ME_INPROC_READBACK_ERR_NO_SESSION_FOUND;
          } 

      } /* ((sq->eState[i])... */

    } /* while (!bDone) */

  return eErr;
  }


/*******************************************************************/
/*** me_action() ***************************************************/
/*******************************************************************/
/* do an action */

#define ME_ACTION_ERR_OK                 0
#define ME_ACTION_ERR_NO_SESSION_FOUND   1
#define ME_ACTION_ERR_SID_NOT_RECOGNIZED 2
#define ME_ACTION_ERR_CID_NOT_RECOGNIZED 3
#define ME_ACTION_ERR_MESI_FAILED        4

int me_action( 
  struct me_session_queue_struct *sq,
  FILE* fpl /* for logging */
  ) {
  
  struct me_session_queue_struct *sq_ptr=NULL; /* for logging */

  int i;
  int eErr = ME_ACTION_ERR_OK;

  int bDone;
  int bFoundOne = 0; 
  int bActNow = 0;
  int bGo = 0;

  struct me_action_struct action;
  struct timeval tv;

  char msg[ME_LOG_MAX_MSG_LENGTH];
  char filename[ME_FILENAME_MAX_LENGTH];
  FILE *fp;

  char longmsg[16384];
  char longmsg2[16384];  
  char data[16384];
  long int mjd,mpm;
  char sdate[32];
  char stime[32];
  int err;
  long int reference = 0;

  char cmd[ME_MAX_COMMAND_LINE_LENGTH];

  char sid_macro[12];
  char barcode[1024];

  /* search through sessions currently in the "READY" or "OBSERVING" state */
  i=0;
  bDone=0;
  bFoundOne=0;
  while (!bDone) {

    /* searching through session queue ... */
    while ( !( ((sq->eState[i])==MESQ_STATE_READY) || ((sq->eState[i])==MESQ_STATE_OBSERVING) ) && (i<(sq->N)) ) { i++; }

    /* if we're here, it'e either because we found a session that is in the READY or OBSERVING state, */
    /* OR we've hit the end of the queue. */

    if ( ((sq->eState[i])==MESQ_STATE_READY) || ((sq->eState[i])==MESQ_STATE_OBSERVING) ) {
        bFoundOne = 1; /* meaning we found a session in this state */
       
        /* read a command from the session command script file */
        fread( &action, sizeof(struct me_action_struct), 1, sq->fpc[i] );
        sprintf(data,"");
        if (action.len>0) fread( data, action.len, 1, sq->fpc[i] );
                 //printf("\n ftell()=%ld\n",ftell(sq->fpc[i]));
        //printf("\n *** action.bASAP=%d, action.sid=%d, action.cid=%d, action.len=%d, data=<%s> \n\n",action.bASAP, action.sid, action.cid, action.len, data );
        (sq->ncr[i])++; /* keeping track of how many commands we've read */
  
        /* check to see if this is something we need to know */
        bActNow = 0;
        if (action.bASAP) { 
            bActNow=1; 
          } else {
            gettimeofday( &tv, NULL ); 
            if  ( LWA_timediff(action.tv,tv) < ME_ACTION_WINDOW ) { 
                bActNow=1; 
              } else {
                //printf("ftell()=%ld\n",ftell(sq->fpc[i]));
                //printf("fseek(fp,%ld,%d)\n",-((long)sizeof(struct me_action_struct)), SEEK_CUR);
                fseek( sq->fpc[i], -((long)sizeof(struct me_action_struct))-action.len, SEEK_CUR ); /* rewind file by one "action" */
                //printf("ftell()=%ld\n",ftell(sq->fpc[i]));
              } 
          }

        /* do action */
        if (bActNow) { 

          if ((sq->eState[i])==MESQ_STATE_READY) { /* in case we haven't promoted this already */
            sq->eState[i]=MESQ_STATE_OBSERVING; 
            me_session_queue_write(sq);
            } 
          
          me_action_cmd_msg( longmsg, &action, data );
          sprintf(longmsg2,"me_action(): cmd: %s",longmsg);
          me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_SCH_CMD, longmsg2, sq, i ); 

          bGo = 0; /* this gets set to 1 if we need to send a command to scheduler */

          switch (action.sid) {

            case LWA_SID_MCS:
              switch (action.cid) {
                case LWA_CMD_NUL:
                  break;
                case LWA_CMD_ESN:
                case LWA_CMD_ESF:
                  sq->eState[i] = MESQ_STATE_AWAITING_OUTPROCESSING; /* promote state of this observation */
                  sq->bOutProcessReqd = 1;                           /* signal that at least one session is in this state */ 
                  me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_INFO, "me_action(): session ended", sq, i );
                  if (action.cid==LWA_CMD_ESF) { sq->oc[i] += ME_OC_RCVD_ESF; }
                  sprintf(sq->ocs[i],"Commanded %s",LWA_cmd2str(action.cid));
                  /* SMF is updated in the outproc function */
                  break;
                case LWA_CMD_OBS:
                  (sq->iCurrentObs[i])++;
                  sq->oc[i] = ME_OC_OK;
                  sprintf(sq->ocs[i],"UNK");
                  //sprintf(sq->sTag[i],"UNK"); /* Overwrites actual recording of this value! */

                  /* ask the DR about the barcode of the DRSU it is currently using. (result goes to MIB, checked on OBE) */
                  sprintf(sid_macro,"DR%d", sq->ssf[i].SESSION_DRX_BEAM );
                  err = mesi( NULL, sid_macro, "RPT", "DRSU-BARCODE", "today", "asap", &reference );

                  /* save ASP MIB as it exists at start of observation */
                  sprintf(cmd, "scp %s:%s/ASP.pag sinbox/%s_%04u_ASP_begin.pag",
                          LWA_SCH_SCP_ADDR,LWA_SCH_SCP_DIR,sq->ssf[i].PROJECT_ID,sq->ssf[i].SESSION_ID);
                  system(cmd);
                  sprintf(cmd, "scp %s:%s/ASP.dir sinbox/%s_%04u_ASP_begin.dir",
                          LWA_SCH_SCP_ADDR,LWA_SCH_SCP_DIR,sq->ssf[i].PROJECT_ID,sq->ssf[i].SESSION_ID);
                  system(cmd);

                  break;
                case LWA_CMD_OBE:

                  /* ask the DR about the barcode of the DRSU it is currently using. (should be current in MIB; was requested on OBS) */
                  sprintf(sid_macro,"DR%d", sq->ssf[i].SESSION_DRX_BEAM );
                  err = memdre( sid_macro, "DRSU-BARCODE", barcode, &tv );

                  ///* ask DR about the size of the recorded file (and associated metadata) */
                  //err = mesi( NULL, sid /* FIXME */, LWA_CID_RPT, "DIRECTORY-COUNT", "today", "asap", &reference );
                  ///* wait 3 seconds for response to come in */

                  /* save ASP MIB as it exists at end of observation */
                  sprintf(cmd, "scp %s:%s/ASP.pag sinbox/%s_%04u_ASP_end.pag",
                          LWA_SCH_SCP_ADDR,LWA_SCH_SCP_DIR,sq->ssf[i].PROJECT_ID,sq->ssf[i].SESSION_ID);
                  system(cmd);
                  sprintf(cmd, "scp %s:%s/ASP.dir sinbox/%s_%04u_ASP_end.dir",
                          LWA_SCH_SCP_ADDR,LWA_SCH_SCP_DIR,sq->ssf[i].PROJECT_ID,sq->ssf[i].SESSION_ID);
                  system(cmd);

                  /* write the metadata file */
                  sprintf(filename,"sinbox/%s_%04u_metadata.txt",sq->ssf[i].PROJECT_ID,sq->ssf[i].SESSION_ID);
                  fp = fopen(filename,"a"); /* note we are appending */
                  fprintf(fp,"%4d [%s] [%s] %2d [%s]\n",sq->iCurrentObs[i],sq->sTag[i],barcode,sq->oc[i],sq->ocs[i]);
                  fclose(fp);

                  break;
                default:
                  sprintf(msg,"me_action(): I don't recognize cid=%d ('%s')",action.cid,LWA_cmd2str(action.cid));
                  me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_SCH_CMD, msg, sq, i );
                  bDone = 1;
                  eErr = ME_ACTION_ERR_CID_NOT_RECOGNIZED;
                  break; 
                } /* switch (action.cid) */        
              break;

            case LWA_SID_NU1: break;
            case LWA_SID_NU2: break;
            case LWA_SID_NU3: break;
            case LWA_SID_NU4: break;
            case LWA_SID_NU5: break;
            case LWA_SID_NU6: break;
            case LWA_SID_NU7: break;
            case LWA_SID_NU8: break;
            case LWA_SID_NU9: break;
            //case LWA_SID_MCS: // this taken care of above 
            case LWA_SID_SHL: break;
            case LWA_SID_ASP: break;
              
            case LWA_SID_DP_: 
              bGo=1; 
              break;

            case LWA_SID_DR1:
            case LWA_SID_DR2:
            case LWA_SID_DR3:
            case LWA_SID_DR4:
            case LWA_SID_DR5: 
              bGo=1; 
              break;

            default:
              bDone = 1;
              sprintf(msg,"me_action(): I don't recognize sid=%d ('%s')",action.sid,LWA_sid2str(action.sid));
              me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_SCH_CMD, msg, sq, i );
              eErr = ME_ACTION_ERR_SID_NOT_RECOGNIZED; 
              break;

            } /* switch (action.sid) */

          if ( (bGo) && (!bFlg_NoSch) )  {  

            //sprintf(sdate,"today"); 
            //sprintf(stime,"asap"); 
            if (!action.bASAP) { 
              LWA_timeval( &(action.tv), &mjd, &mpm ); 
              sprintf(sdate,"%ld",mjd);
              sprintf(stime,"%ld",mpm);
              }
            err = mesi( NULL, 
                        LWA_sid2str(action.sid), 
                        LWA_cmd2str(action.cid), 
                        data,
                        sdate, stime,
                        &reference );
            if (err!=MESI_ERR_OK) {
                sprintf(longmsg,"FATAL: in me_action(), mesi(NULL,'%s','%s','%s','%s','%s',*) returned code %d",
                        LWA_sid2str(action.sid), 
                        LWA_cmd2str(action.cid), 
                        data,
                        sdate, stime,
                        err);
                bDone = 1;
                me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_SCH_CMD, longmsg, sq, i );
                printf("[%d/%d] %s\n",ME_ME_C,getpid(), longmsg );
                eErr = ME_ACTION_ERR_MESI_FAILED;  

                /* 121204 Kludge to prevent MCS/Exec shutdown when MCS/Sch says "Insufficient advance notice" */
                if (err==8) {
                  err=MESI_ERR_OK;
                  sprintf(longmsg,"121204 kludge: downgrading above from FATAL to WARNING");
                  bDone = 0;
                  me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_SCH_CMD, longmsg, sq, i );
                  printf("[%d/%d] %s\n",ME_ME_C,getpid(), longmsg );
                  }

              } else {
                sprintf(longmsg,"me_action(): above cmd accepted; ref=%ld",reference);
                me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_INFO, longmsg, sq, i );
                if (action.cid==LWA_CMD_REC) { sprintf(sq->sTag[i],"%06ld_%09ld",mjd,reference); } 
                if (action.cid==LWA_CMD_SPC) { sprintf(sq->sTag[i],"%06ld_%09ld",mjd,reference); }
              }

            } /* if (bGo && (!nFlg_NoSch)) */

          } /* if (bActNow) */

        /* other things to consider: */
        /* -- check for EOF */

        /* we're not necessarily done yet */
        i++; 

      } else { /* we must be at the end of the session queue */

        bDone = 1;
        if (!bFoundOne) {
          me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_action(): couldn't find a session to act on", sq_ptr, 0 );
          eErr = ME_ACTION_ERR_NO_SESSION_FOUND;
          } 

      } /* ((sq->eState[i])... */

    } /* while (!bDone) */

  /* check through queue to see how to set bReady flag on exit */
  sq->bReady = 0;
  for ( i=0; i<=(sq->N); i++ ) {
    if ((sq->eState[i])==MESQ_STATE_READY    ) { sq->bReady = 1; }
    if ((sq->eState[i])==MESQ_STATE_OBSERVING) { sq->bReady = 1; }
    }

  return eErr;
  }


/*******************************************************************/
/*** me_session_queue_outprocess() *********************************/
/*******************************************************************/
/* out-process a session in the session queue */

#define ME_SESSION_QUEUE_OUTPROCESS_ERR_OK               0
#define ME_SESSION_QUEUE_OUTPROCESS_ERR_NO_SESSION_FOUND 1

int me_session_queue_outprocess( 
  struct me_session_queue_struct *sq,
  FILE* fpl
  ) {

  int i,k;
  int eErr=ME_SESSION_QUEUE_OUTPROCESS_ERR_OK;
  struct me_session_queue_struct *sq_ptr=NULL;
  FILE *fp;
  char filename[ME_FILENAME_MAX_LENGTH];
  struct me_outproc_manifest_struct om;
  struct stat statbuf;

  /* find a session to be out-processed */
  i=0;
  while ( ((sq->eState[i])!=MESQ_STATE_AWAITING_OUTPROCESSING) && (i<(sq->N)) ) { i++; }
  if ((sq->eState[i])!=MESQ_STATE_AWAITING_OUTPROCESSING) {
    //printf("[%d/%d] me_session_outprocess(): couldn't find a session to process\n",ME_ME_C,getpid());
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_session_outprocess(): couldn't find a session to process", sq_ptr, 0 );
    return ME_SESSION_QUEUE_OUTPROCESS_ERR_NO_SESSION_FOUND;
    }

  /**************************/
  /*** outprocessing here ***/
  /**************************/
  
  /* close the command script file */
  fclose(sq->fpc[i]);
  sq->fpc[i] = NULL;  /* reset file pointer */

  /* if session failed, then last obs did not update the SMF, so need to finish here. */
  /* also need to do this if we got a "ESN" before the end of the first observation, in which case there is no file yet */
  sprintf(filename,"sinbox/%s_%04u_metadata.txt",sq->ssf[i].PROJECT_ID,sq->ssf[i].SESSION_ID);
  if ( ( sq->oc[i] & ME_OC_RCVD_ESF ) || (stat(filename,&statbuf)!=0) )  {
    fp = fopen(filename,"a"); /* note we are appending */
    //fprintf(fp,"%4d %2d %s\n",sq->iCurrentObs[i],sq->oc[i],sq->ocs[i]);
    fprintf(fp,"%4d %16s %2d %s\n",sq->iCurrentObs[i],sq->sTag[i],sq->oc[i],sq->ocs[i]);
    fclose(fp);
    }

  /* prepare a manifest for me_tpcom */
  memcpy( &(om.ssf), &(sq->ssf[i]), sizeof(om.ssf) );
  /* me_tpcom knows to include .txt, .ses, .obs, .ipl, and .cs files for this session */
  /* me_tpcom knows to include state/ssmif.dat */

  /* FIXME me_tpcom knows to include current dynamic MIB SDM (= "STAT" parameters from SSMIF + Station-level info) */
  /* FIXME me_tpcom knows to include the various MIBs + mselog + meelog, according ssf flags */
  /* FIXME me_tpcom knows to include design info, according to ssf flags */

  /* write it */
  sprintf(filename,"state/%s_%04u.manifest",sq->ssf[i].PROJECT_ID,sq->ssf[i].SESSION_ID);
  fp = fopen(filename,"wb");
  fwrite(&om,sizeof(om),1,fp);
  fclose(fp);

  /* log successful conclusion */
  me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_INFO, "me_session_outprocess() complete", sq, i );

  /* clear this slot in the session queue */
  me_initialize_session_queue_entry( sq, i );

  /* check through queue to see how to set bOutProcessReqd flag on exit */
  /* also to reset sq->N */
  sq->bOutProcessReqd = 0;
  k = -1;
  for ( i=0; i<=(sq->N); i++ ) {
    if ((sq->eState[i])==MESQ_STATE_AWAITING_OUTPROCESSING) { sq->bOutProcessReqd = 1; }
    if ((sq->eState[i])> MESQ_STATE_NOT_USED) { k=i; }
    }
  sq->N = k;

  /* write out updated session queue */
  me_session_queue_write(sq);

  return eErr;
  }


/*******************************************************************/
/*** me_pull_from_tp() *********************************************/
/*******************************************************************/
/* Check for a "manifest.dat" file in sinbox and inventory indicated files */
/* adds session information to session queue (sq) */ 

#define ME_PULL_FROM_TP_ERR_OK                     0
#define ME_PULL_FROM_TP_ERR_SESSION_QUEUE_ADD_FAIL 1

int me_pull_from_tp( struct me_session_queue_struct *sq,
                     FILE* fpl 
                   ) {
  //char cmd[ME_MAX_COMMAND_LINE_LENGTH];
  int iErr=ME_PULL_FROM_TP_ERR_OK;
  struct stat statbuf;
  FILE *fp;
  struct me_session_queue_struct *sq_ptr=NULL;
  char msg[ME_LOG_MAX_MSG_LENGTH];

  char sProjectID[ME_MAX_PROJECT_ID_LENGTH]; 
  int iSessionID;
  int iObs;
  char filename[ME_FILENAME_MAX_LENGTH];

  /* check to see if a "manifest.dat" exists */
  if (stat("sinbox/manifest.dat",&statbuf)==0) {
    /* Since we snagged one, there is work to do. */
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_pull_from_tp() found a manifest", sq_ptr, 0 );

    fp=fopen("sinbox/manifest.dat","r");

    /* read manifest */
    while (!feof(fp)) {
      fscanf(fp,"%s %d %d %s",sProjectID,&iSessionID,&iObs,filename); 
      if (!feof(fp)) {
        //printf("<%s>\n",filename);  

        /* remember this information so that we can easily process these files later */
        if (iObs==0) { /* this is a .ses file */
          /* add to session queue */
          if ( me_session_queue_add( sq, sProjectID, iSessionID, filename, fpl ) > 0 ) {
            //printf("[%d/%d] me_pull_from_tp(): session queue overflow\n",ME_ME_C,getpid());
            me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_pull_from_tp(): session queue overflow", sq_ptr, 0 );
            return ME_PULL_FROM_TP_ERR_SESSION_QUEUE_ADD_FAIL;
            } 
          } /* if (iObs==0) */

        }
      }

    /* close & delete manifest.dat */
    fclose(fp);
    system("rm sinbox/manifest.dat");

    } /* if (stat( */

  return iErr;
  } /* me_pull_from_tp() */

/*******************************************************************/
/*** me_write_mess() ***********************************************/
/*******************************************************************/
/* writes out a MCS/Exec Session Schedule (mess.dat) file */

#define ME_WRITE_MESS_ERR_CANT_FOPEN 1
int me_write_mess( struct ssmif_struct s, 
                   struct sc_struct sc,
                   struct me_session_queue_struct sq,
                   FILE* fpl  /* point to log file */
                  ) {
  int iErr=0;
  FILE *fp;
  int i;
  char msg[ME_LOG_MAX_MSG_LENGTH];
  struct me_session_queue_struct *sq_ptr=NULL;

  if (!(fp = fopen("state/mess.dat","w"))) {
    //printf("[%d/%d] me_write_mess() can't fopen('state/mess.dat')\n",ME_ME_C,getpid());
    sprintf(msg,"me_write_mess() can't fopen 'state/mess.dat'");
    me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, msg, sq_ptr, 0 );
    return ME_WRITE_MESS_ERR_CANT_FOPEN; 
    }

  fprintf(fp,"%d\n",s.eCRA);

  for (i=0;i<ME_MAX_NDPOUT;i++) {
    fprintf(fp,"%d ",sc.DPO[i].iStat);
    }
  fprintf(fp,"\n");

  for ( i=0; i<=sq.N; i++ ) {
    if ( sq.eState[i] >= MESQ_STATE_READY ) {
      fprintf(fp,"%6lu %9lu %1hd %9lu %hu %s %u\n",
                  sq.ssf[i].SESSION_START_MJD,
                       sq.ssf[i].SESSION_START_MPM,
                            sq.ssf[i].SESSION_DRX_BEAM,
                                 sq.ssf[i].SESSION_DUR,
                                      sq.ssf[i].SESSION_CRA,
                                          sq.ssf[i].PROJECT_ID,
                                             sq.ssf[i].SESSION_ID);
      }
    }

  fclose(fp);

  //me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_write_mess() completed", sq_ptr, 0 );

  return iErr;
  }  

//Format of schedule file: (mess.dat)
//line 0 is MCS_CRA
//line 1 is n n n n n (state of functioning beams 1-5, based on SSMIF)
//Each line is a session, sorted by Start MJD, Start MPM, then DP_output
//StartMJD/MPM, DP-output# (1-5), DURATION, CRA, PROJECTID/SESSION_ID

/*******************************************************************/
/*** me_init_sdm() *************************************************/
/*******************************************************************/
/* Sets SDM to reasonable initial values, but does NOT try to get */
/* actual values; e.g., Scheduler is not consulted */ 

int me_init_sdm( struct ssmif_struct s,
                 struct sc_struct sc, 
                 struct sdm_struct *sdm
               ) {

  int i,j;
  struct timeval tv;
  char filename[256];
  FILE *fp;

  sprintf(filename,"state/sdm.dat");
  if ((fp=fopen(filename,"rb"))==NULL) {
    printf("[%d/%d] Can't open '%s'\n",ME_ME_C,getpid(),filename);
    return;
    }
  fread(sdm,sizeof(struct sdm_struct),1,fp);
  fclose(fp); 
  //printf("sdm_init: sdm->settings.tbn_gain=%hd\n",sdm->settings.tbn_gain);

  gettimeofday(&tv,NULL);

  sdm->station.summary = LWA_SIDSUM_BOOTING; sprintf(sdm->station.info,""); sdm->station.tv = tv; /* station */
  sdm->shl.summary     = LWA_SIDSUM_UNK;     sprintf(sdm->shl.info,    ""); sdm->shl.tv     = tv; /* SHL */
  sdm->asp.summary     = LWA_SIDSUM_UNK;     sprintf(sdm->asp.info,    ""); sdm->asp.tv     = tv; /* ASP */
  sdm->dp.summary      = LWA_SIDSUM_UNK;     sprintf(sdm->dp.info,     ""); sdm->dp.tv      = tv; /* DP */
  for (i=0;i<ME_MAX_NDR;i++) {
    sdm->dr[i].summary = LWA_SIDSUM_UNK; sprintf(sdm->dr[i].info,  ""); sdm->dr[i].tv   = tv; /* DR# */
    }

  for (i=0;i<ME_MAX_NFEE;i++) { sdm->ssss.eFEEStat[i] = s.iFEEStat[i]; } /* FEE_STAT[] */
  for (i=0;i<ME_MAX_NRPD;i++) { sdm->ssss.eRPDStat[i] = s.iRPDStat[i]; } /* RPD_STAT[] */
  for (i=0;i<ME_MAX_NSEP;i++) { sdm->ssss.eSEPStat[i] = s.iSEPStat[i]; } /* SEP_STAT[] */
  for (i=0;i<ME_MAX_NARB;i++) { 
    for (j=0;j<ME_MAX_NARBCH;j++) { 
      sdm->ssss.eARBStat[i][j] = s.eARBStat[i][j]; } }                   /* ARB_STAT[][] */
  for (i=0;i<ME_MAX_NDP1;i++) { 
    for (j=0;j<ME_MAX_NDP1CH;j++) { 
      sdm->ssss.eDP1Stat[i][j] = s.eDP1Stat[i][j]; } }                   /* DP1_STAT[][] */
  for (i=0;i<ME_MAX_NDP2;i++) { sdm->ssss.eDP2Stat[i] = s.eDP2Stat[i]; } /* DP2_STAT[] */
  for (i=0;i<ME_MAX_NDR;i++)  { sdm->ssss.eDRStat[i]  = s.eDRStat[i];  } /* DR_STAT[] */

  for (i=0;i<s.nStd;i++) { 
    for (j=0;j<2;j++) {    
      sdm->ant_stat[i][j] = sc.Stand[i].Ant[j].iSS; } }
  for (i=0;i<ME_MAX_NDR;i++)  { 
      sdm->dpo_stat[i]    = sc.DPO[i].iStat;  } 

  /* .settings are left alone */ 

  return;
  } /* me_init_sdm() */


/*******************************************************************/
/*** me_make_gf() *************************************************/
/*******************************************************************/
/* construct default beam "gains" file (default.gf) from state/default.gft, masking out bad dipoles */

int me_make_gf( struct ssmif_struct s
              ) {

  int stdmask[ME_MAX_NSTD];
  int stdmask_count=0;
  int i;

  FILE *fpi;
  float gi[2][2];

  FILE *fpo;

  char cmd[256];

  /* Make a list of stands to "mask" (mark out of service) */
  for ( i=0; i<ME_MAX_NSTD; i++ ) {
    printf("me_make_gf(): i=%d ",i);
    stdmask[i]=0;
    if (i>=s.nStd) {
        /* there are not this many stands, so leave marked out */
        printf("i>=s.nStd\n");
      } else {
        /* should be within range of stands specified by s.nStd */
        /* note: iAntStat[1..520(nominally)]; i.e., is not zero-based */
        //printf("iAntStat[%3d]=%d, iAntStat[%3d]=%d, ",2*i+1,s.iAntStat[2*i+0], 
        //                                              2*i+2,s.iAntStat[2*i+1]);
        if ( (s.iAntStat[2*i+0]>2) && (s.iAntStat[2*i+1]>2) ) {
          stdmask[i]=1;
          }
        printf("stdmask[%d]=%d\n",i,stdmask[i]);
      } /* if (i>=s.nStd) */
    } /* for ( i=0 */
  
  /* Report findings */
  stdmask_count = 0;
  printf("me_make_gf(): Stands marked out:");
  for ( i=0; i<s.nStd; i++ ) {  
    if (!stdmask[i]) {
      printf(" %d",i+1);
      stdmask_count++;
      }
    }
  printf("\n");
  printf("me_make_gf(): %d of %d stands marked out\n",stdmask_count,s.nStd);

  /* Read in lines from state/default.gft, write out masked lines to state/default_m.gft */
  fpi = fopen("state/default.gft","r");
  if (!fpi) {
    printf("me_exec_1 / me_make_gf(): FATAL: Unable to open 'state/default.gft' for input.\n");
    return;
    }
  fpo = fopen("state/default_m.gft","w");
  i=0;
  while ( fscanf(fpi,"%f %f %f %f",&gi[0][0],&gi[0][1],&gi[1][0],&gi[1][1]) >0) {
    //printf("me_make_gf(): i=%d %4.2f %4.2f %4.2f %4.2f\n",i,gi[0][0],gi[0][1],gi[1][0],gi[1][1]);
    fprintf(fpo,"%7.3f %7.3f %7.3f %7.3f\n",gi[0][0]*stdmask[i],gi[0][1]*stdmask[i],gi[1][0]*stdmask[i],gi[1][1]*stdmask[i]);
    i++;
    }
  fclose(fpi); 
  fclose(fpo);

  /* Call megfg to convert state/default_m.gft to state/default.gf */
  sprintf(cmd,"./megfg state/default_m.gft state/default.gf");
  system(cmd);

  /* cp state/default.gf to ../sch/gfiles/. */
  sprintf(cmd, "scp state/default.gf %s:%s/gfiles/.",LWA_SCH_SCP_ADDR,LWA_SCH_SCP_DIR);
  system(cmd);

  return;
  } /* me_gf() */


/*******************************************************/
/*** me_stp() ******************************************/
/*******************************************************/
/* responding to a "STP" command sent via meei(x): Terminate an observing session */

int me_stp( 
  struct me_session_queue_struct *sq,
  FILE* fpl, /* for logging */
  char *args
  ) {

  char sProjectID[ME_MAX_PROJECT_ID_LENGTH];
  int iSessionID;

  int i,ii;
  int b1,b2;
 
  struct me_session_queue_struct *sq_ptr=NULL; /* for logging */
  int err = ME_CMD_NUL; /* this value used to acknowledge command */

  char sid_macro[12];
  char barcode[1024];
  struct timeval tv;
  char filename[ME_FILENAME_MAX_LENGTH];
  FILE *fp;

  /* get project ID and session ID */
  sscanf(args,"%s %d",sProjectID,&iSessionID);

  /* search for it in the session queue */
  ii = -1; /* this will become the index into the session queue if we find the session */
  for ( i=0; i<=sq->N; i++ ) {
    b1 = (strncmp(sq->ssf[i].PROJECT_ID,sProjectID,strlen(sProjectID))==0);
    b2 = (sq->ssf[i].SESSION_ID)==iSessionID; 
    if ( b1 && b2 ) { ii = i; } /* found it! remember index */
    }

  if (ii>=0) { /* found it */

    /* what to do about it depends on it's current state: */
    switch(sq->eState[ii]) {
      case MESQ_STATE_NOT_USED:
        me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_stp(): MESQ_STATE_NOT_USED; nothing to do (shouldn't be here)", sq_ptr, 0 );
        break;
      case MESQ_STATE_AWAITING_INPROCESSING:
        me_initialize_session_queue_entry(sq,ii); 
        me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_stp(): Killed session while MESQ_STATE_AWAITING_INPROCESSING", sq_ptr, 0 );
        break;
      case MESQ_STATE_INPROCESSING:
        me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_stp(): Failed because state is MESQ_STATE_INPROCESSING; try again later", sq_ptr, 0 );
        err = ME_CMD_ERR;
        break;
      case MESQ_STATE_READY:
      case MESQ_STATE_OBSERVING:

        sq->eState[ii] = MESQ_STATE_AWAITING_OUTPROCESSING;
        sq->bOutProcessReqd = 1;                            /* signal that at least one session is in this state */ 
        me_log( fpl, ME_LOG_SCOPE_SESSION, ME_LOG_TYPE_INFO, "me_sdp(): session ended", sq, ii );

        sq->oc[ii] += ME_OC_MEEI_STP; 
        sprintf(sq->ocs[ii],"Rcvd STP from meei");

        /* ask the DR about the barcode of the DRSU it is currently using. (should be current in MIB; was requested on OBS) */
        sprintf(sid_macro,"DR%d", sq->ssf[ii].SESSION_DRX_BEAM );
        err = memdre( sid_macro, "DRSU-BARCODE", barcode, &tv );

        /* write the metadata file */
        sprintf(filename,"sinbox/%s_%04u_metadata.txt",sq->ssf[ii].PROJECT_ID,sq->ssf[ii].SESSION_ID);
        fp = fopen(filename,"a"); /* note we are appending */
        fprintf(fp,"%4d [%s] [%s] %2d [%s]\n",sq->iCurrentObs[ii],sq->sTag[ii],barcode,sq->oc[ii],sq->ocs[ii]);
        fclose(fp);

        /* check through queue to see how to set bReady flag on exit */
        sq->bReady = 0;
        for ( i=0; i<=(sq->N); i++ ) {
          if ((sq->eState[i])==MESQ_STATE_READY    ) { sq->bReady = 1; }
          if ((sq->eState[i])==MESQ_STATE_OBSERVING) { sq->bReady = 1; }
          }

        break;
      case MESQ_STATE_AWAITING_OUTPROCESSING:
        me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_stp(): Failed because state is MESQ_STATE_AWAITING_OUTPROCESSING", sq_ptr, 0 );
        err = ME_CMD_ERR;
        break;       
      default:
        me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_stp() doesn't understand this eState (shouldn't be here)", sq_ptr, 0 );
        err = ME_CMD_ERR;
        break;
      } /* switch() */ 

    } else {
      me_log( fpl, ME_LOG_SCOPE_NONSPECIFIC, ME_LOG_TYPE_INFO, "me_stp(): Did not find indicated session", sq_ptr, 0 );
      err = ME_CMD_ERR;
    }

  return err;
  } /* me_stp() */
