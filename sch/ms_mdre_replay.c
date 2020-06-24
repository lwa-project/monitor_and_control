// ms_mdre_replay.c: J. Dowell, UNM, 2020 Jun 24
// ---
// COMPILE:  gcc -o ms_mdre_replay -I/usr/include/gdbm ms_mdre_replay.c -lgdbm 
// In Ubuntu, needed to install package libgdbm-dev
// ---
// COMMAND LINE: ./ms_mdre_replay replaylog
// ---
// REQUIRES: 
//   LWA_MCS.h
// ---
// MCS/Executive's interface into MCS/Scheduler MIB dbm files
// Interface is via sockets, in the same manner as ms_exec
// This is intended to be launched by ms_init, and killed by ms_exec
// See end of this file for history.

#include <stdlib.h> /* needed for exit(); possibly other things */
#include <stdio.h>

#include <string.h>
#include <fcntl.h> /* needed for O_READONLY, F_GETFL; perhaps other things */
#include <gdbm.h>
#include <byteswap.h>

/* additional stuff related to sockets */
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h> /* for network sockets */
#include <arpa/inet.h>  /* for network sockets */

//#include "LWA_MCS.h" 
#include "mcs.h"

#define MY_NAME "ms_mdre_replay (v.20200624.1)"
#define ME "16" 

main ( int narg, char *argv[] ) {

  /*=================*/
  /*=== Variables ===*/
  /*=================*/
  
  char replaylog[255];  /* log to replay from */
  FILE* fpr[LWA_MAX_SID+1];
  
  struct timeval tv;  /* from sys/time.h; included via LWA_MCS.h */
  struct tm *tm;      /* from sys/time.h; included via LWA_MCS.h */
  
  int sid, status, lsid, lcid, found;
  long int ref, mjd, mpm;
  char line[255], temp[10];
  char label[MIB_LABEL_FIELD_LENGTH];     /* this is the key for dbm */
  char key[MIB_LABEL_FIELD_LENGTH];
  
  /* Imported from ms_exec.c */
  int server_len;
  int client_len;  
  int server_sockfd;                
  int client_sockfd;   
  struct sockaddr_in server_address; /* for network sockets */
  struct sockaddr_in client_address; /* for network sockets */
  int flags; /* used as part of scheme for changing accept()'s blocking behavior */

  //struct LWA_cmd_struct c;
  struct LWA_mib_entry c;

  int i;

  int eError=0;

  /*======================================*/
  /*=== Initialize: Command line stuff ===*/
  /*======================================*/
    
  /* First, announce thyself */
  printf("[%s/%d] I am %s \n",ME,getpid(),MY_NAME);

  /* Process command line arguments */
  if (narg>1) { 
    strcpy(&replaylog[0], argv[1]);
    } else {

    printf("FATAL: replaylog not provided");
    exit(EXIT_FAILURE);

    } 

  /* Set up transmit message queues - which are just the logs to replay */
  for ( i=11; i<LWA_MAX_SID; i++ ) { /* start at n=11 since n=10 is MCS (me) */ 
    fpr[i] = fopen(&replaylog[0], "r");
    if (fpr[i]==NULL) {
       //perror(" ");
       printf("FATAL: Could not fopen() replay log\n");
       exit(EXIT_FAILURE);
       }
    }
    
  /*======================================*/
  /*=== Initialize: IP connection ========*/
  /*======================================*/

  /* set up sockets interface for communicating with MCS/Sch (ms_exec) and others... */ 
  server_sockfd = socket(             /* create socket */
                         AF_INET,     /* domain; network sockets */
                         SOCK_STREAM, /* type (TCP-like) */
                         0);          /* protocol (normally 0) */
  if (server_sockfd == -1) {
    printf("[%s] FATAL: socket() failed\n",ME);
    perror("ms_mdre_replay");
    exit(EXIT_FAILURE); 
    }
  /* name socket */
  server_address.sin_family      = AF_INET;                 /* network sockets */  
  server_address.sin_addr.s_addr = inet_addr(LWA_IP_MSE);   /* network sockets */
  server_address.sin_port        = htons(LWA_PORT_MSE2);    /* network sockets */
  server_len = sizeof(server_address);

  i = bind( server_sockfd, 
           (struct sockaddr *) &server_address, 
            server_len );
  if (i == -1) {
    printf("[%s] FATAL: bind() failed (see error message below)\n",ME);
    perror("ms_exec");
    printf("[%s] If message above is ``Address already in use'':\n",ME);
    printf("[%s]   (1) Kill any ms_mcic processes (e.g., $ sh ./ms_shutdown).\n",ME);
    printf("[%s]   (2) Wait a few seconds before trying this again.\n",ME);
    exit(EXIT_FAILURE); 
    }

  /* create a connection queue */
  i = listen(server_sockfd,
             32 );           /* backlog */
  if (i == -1) {
    printf("[%s] FATAL: listen() failed\n",ME);
    perror("ms_exec");
    exit(EXIT_FAILURE); 
    }
  /* change accept() from blocking to non-blocking */
  flags = fcntl( server_sockfd, F_GETFL, 0 );
  fcntl( server_sockfd, F_SETFL, O_NONBLOCK|flags );

  /*==================*/
  /*==================*/
  /*=== Main Loop ====*/
  /*==================*/
  /*==================*/

  while (1) {

    /*=========================================================================*/
    /*=== Check inbound socket for messages from MCS/Exec or other entities ===*/
    /*=========================================================================*/ 

    /* Check socket interface for MCS/Exec or other inbound connections */
    client_len = sizeof(client_address);
    client_sockfd = accept( server_sockfd,
                            (struct sockaddr *) &client_address, 
                            &client_len );  

    if (!(client_sockfd==-1)) { /* we have a connection... */                          

      eError=0;
      //printf("Ow! Quit it...\n");

      /* read it into a LWA_mib_entry structure */
      read(client_sockfd,&c,sizeof(struct LWA_mib_entry));

      /* figure out what we need from this */
      sid = LWA_getsid(c.ss);
      sprintf(label,"%s",c.label);     /* and the label is the label... */
      //printf("Looking for '%s'\n", label);
      
      /* Look for the entry */
      /* Read through the file until we find what we are looking for */
      found = 0;
      while(fgets(&line, sizeof(line), fpr[sid]) != NULL) {
        /* Basic unpak to get the queuing status, subsystem ID, and command ID */
        strncpy(&temp, &(line[45]), 1);
        temp[1] = '\0';
        status = atoi(&temp);
        strncpy(&temp, &(line[47]), 3);
        temp[3] = '\0';
        lsid = LWA_getsid(&temp);
        strncpy(&temp, &(line[51]), 3);
        temp[3] = '\0';
        lcid = LWA_getcmd(&temp);
        
        /* Ignore lines that don't have the right subsystem or command */
        if( ( (lsid != sid) \
              || (lcid != LWA_CMD_RPT) ) ) {
            continue;
        }
        
        /*
           Processing of the line:
             if the status is TP_QUEUED or TP_SEND
             -> save the reference number
             else if the status is TP_SUCCESS or higher and we have the right reference
             -> save the response to the message
        */ 
        if( ( (status < LWA_MSELOG_TP_SUCCESS ) \
             && (strncmp(label, &(line[55]), strlen(label)) == 0) ) ) {
            strncpy(&temp, &(line[35]), 9);
            temp[9] = '\0';
            ref = atol(&temp);
            //printf("Found '%s' at '%li'\n", label, ref);
        } else {
            strncpy(&temp, &(line[35]), 9);
            temp[9] = '\0';
            
            if( ref == atol(&temp) ) {
                found = 1;
                LWA_time(&mjd, &mpm);
                LWA_time2tv(&c.last_change, mjd, mpm);
                for(i=0; i<MIB_VAL_FIELD_LENGTH; i++) {
                    if( line[55+i] != '|' && line[55+i] != '\0') {
                        c.val[i] = line[55+i];
                        c.val[i+1] = '\0';
                    } else {
                        break;
                    }
                }
                /* catch for binary temperatures (the most common "@4" in the logs) */
                if( strncmp(c.val, "@4", 2) == 0 ) {
                    c.val[0] = '7';  c.val[1] = '8';
                    c.val[2] = '.';
                    c.val[3] = '3';  c.val[4] = '5';
                    c.val[5] = '\0';
                }
                break;
            }
        }
      }
    
      /* If we found something, send it back to the main thread so that it can be logged */
      if( found ) {
        /* report back to requester, and close connection */
        write(client_sockfd,&c,sizeof(struct LWA_mib_entry));
        close(client_sockfd);
        }
        
      } /* if (!(client_sockfd==-1)) {   ...we have a connection... */  


    /* avoiding busy wait */
    usleep(10); /* Go to sleep for 1 microsecond */
    /* 1 microsecond sleep (usleep(1)) is enough to reduce CPU utilization */
    /* from near 100% to a level which is comparable to quiescent activity; */
    /* i.e., 10% or less */    

    } /* while(1) */

  /* shutdown the socket connection */
  close(server_sockfd);     

  //printf("[%s/%d] exit(EXIT_SUCCESS)\n",ME,getpid());
  exit(EXIT_SUCCESS);
  } /* main() */


//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// ms_mdre_replay.c: J. Dowell, UNM, 2020 Jun 24
//   .1 Created from ms_mdre_ip.c
// ms_mdre_ip.c: J. Dowell, UNM, 2019 Oct 30
//   .1 Convert to using normal GDBM for the database
// ms_mdre_ip.c: J. Dowell, UNM, 2019 Oct 29
//   .1 Made the code "type complete"
// ms_mdre_ip.c: J. Dowell, UNM, 2018 Jan 29
//   .1: Increased the queue size for listen()
//   .2: Cleaned up a few compiler warnings
// ms_mdre_ip.c: J. Dowell, UNM, 2015 Aug 10
//   .1: Added support for signed int and unsigneg long int representations
// ms_mdre_ip.c: S.W. Ellingson, Virginia Tech, 2010 Oct 16
//   .1: Initial version, spinning off of ms_mdre.c
// ms_mdre.c: S.W. Ellingson, Virginia Tech, 2009 Aug 16
//   .1: Dealing with unprintable fields -- integers now get printed
// ms_mdre.c: S.W. Ellingson, Virginia Tech, 2009 Aug 15
//   .1: Dealing with unprintable fields
// ms_mdre.c: S.W. Ellingson, Virginia Tech, 2009 Aug 02
//   .1: Initial version, adapted from ms_mdr.c (svn rev 23)
// ms_mdr.c: S.W. Ellingson, Virginia Tech, 2009 Aug 02
//   .1: Working on formatting
// ms_mdr.c: S.W. Ellingson, Virginia Tech, 2009 Jul 26 
//   .1: Very first version, adapted from segments of ms_mcic.c
//   .2: Implementing index/label swap in dbm database (svn rev 10)

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================












