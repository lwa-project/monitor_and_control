// ms_mdre_ip.c: S.W. Ellingson, Virginia Tech, 2010 Oct 16
// ---
// COMPILE:  gcc -o ms_mdre_ip -I/usr/include/gdbm ms_mdre_ip.c -lgdbm_compat -lgdbm 
// In Ubuntu, needed to install package libgdbm-dev
// ---
// COMMAND LINE: ./ms_mdre_ip
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
#include <gdbm-ndbm.h>

/* additional stuff related to sockets */
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h> /* for network sockets */
#include <arpa/inet.h>  /* for network sockets */

//#include "LWA_MCS.h" 
#include "mcs.h"

#define MY_NAME "ms_mdre_ip (v.20101016.1)"
#define ME "16" 

main ( int narg, char *argv[] ) {

  /*=================*/
  /*=== Variables ===*/
  /*=================*/

  /* dbm-related variables */
  char dbm_filename[256];
  DBM *dbm_ptr;
  struct dbm_record record;
  datum datum_key;
  datum datum_data;

  struct timeval tv;  /* from sys/time.h; included via LWA_MCS.h */
  struct tm *tm;      /* from sys/time.h; included via LWA_MCS.h */

  char label[MIB_LABEL_FIELD_LENGTH];     /* this is the key for dbm */
  char key[MIB_LABEL_FIELD_LENGTH];

  union {
    unsigned short int i;
    unsigned char b[2];
    } i2u;
  union {
    unsigned int i;
    unsigned char b[4];
    } i4u;
  union {
    float f;
    unsigned char b[4];
    } f4;
 
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
    perror("ms_exec");
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
             5 );           /* backlog */
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
      sprintf(dbm_filename,"%s",c.ss); /* dbm_filename is the TLA-subsystem identifier */
      sprintf(label,"%s",c.label);     /* and the label is the label... */

      //printf("> dbm_filename='%s'\n",dbm_filename);
      //printf("> label='%s'\n",label);   

       /* Open dbm file and look for entry */
      dbm_ptr = dbm_open(dbm_filename, O_RDONLY);

      if (!dbm_ptr) { /* not able to open dbm file */

          //printf("[%s/%d] FATAL: Failed to open dbm <%s>. eError=%d\n",ME,getpid(),dbm_filename,eError);
          eError = 1;
          sprintf(c.val,"Err: Failed to open dbm");
          //c.last_change = 0;

        } else { /* able to open dbm file.  Look for label... */

          sprintf(key,"%s",label);
          datum_key.dptr = key;
          datum_key.dsize = strlen(key);
          datum_data = dbm_fetch(dbm_ptr,datum_key);
          if (datum_data.dptr) {
              //printf("[%s/%d] Found it. eError=%d.\n", ME, getpid(),eError);
              memcpy( &record, datum_data.dptr, datum_data.dsize );
              //strncpy(ip_address,record.val,15);
            } else {
              //printf("[%s/%d] Failed to find label=<%s> in dbm. eError=%d\n", ME, getpid(),label,eError);
              eError = 1;
              sprintf(c.val,"Err: Failed to find label");
              //c.last_change = 0;
            }

          /* Close dbm file */
          dbm_close(dbm_ptr);

        } /* if (!dbm_ptr) */

      if (!eError) {

        /* Decide how to show record.val: This depends on record.type_dbm: */
        if (!strncmp(record.type_dbm,"NUL",3)) { /* if the format is "NUL" (e.g., branch entries)... */
          strcpy(record.val,"NUL\0");            /* print "NUL" for value */
          }
        if (!strncmp(record.type_dbm,"a",1)) { 
                                                 /* do nothing; fine the way it is */
          }    
        if (!strncmp(record.type_dbm,"r",1)) {   /* if the field is not printable... */
          strcpy(record.val,"@...\0");           /* just print "@" instead */
          }
        if (!strncmp(record.type_dbm,"i1u",3)) {  /* if the format is "i1u" */
          //i1u.b = record.val[0];           /* unpack the bytes into a union structure */
          //sprintf(record.val,"%c\0",i1u.i); /* overwrite in human-readable representation */    
          //printf("[%s/%d] Not expecting to see i1u as a type_dbm.  Treating as raw.\n",ME,getpid());
          //strcpy(record.val,"@\0");              /* just print "@" instead */
          i2u.b[0]=record.val[0];           /* unpack the bytes into a union structure */
          i2u.b[1]=0;
          sprintf(record.val,"%hu\0",i2u.i); /* overwrite in human-readable representation */ 
          }
        if (!strncmp(record.type_dbm,"i2u",3)) {  /* if the format is "i2u" */   
          //printf("[%s/%d] Not expecting to see i2u as a type_dbm.  Treating as raw.\n",ME,getpid());
          //strcpy(record.val,"@@\0");              /* just print "@@" instead */
          i2u.b[0]=record.val[0];           /* unpack the bytes into a union structure */
          i2u.b[1]=record.val[1];
          sprintf(record.val,"%hu\0",i2u.i); /* overwrite in human-readable representation */ 
          }
        if (!strncmp(record.type_dbm,"i4u",3)) {  /* if the format is "i4u" */   
          //printf("[%s/%d] Not expecting to see i4u as a type_dbm.  Treating as raw.\n",ME,getpid());
          //strcpy(record.val,"@@@@\0");              /* just print "@@@@" instead */
          i4u.b[0]=record.val[0];           /* unpack the bytes into a union structure */
          i4u.b[1]=record.val[1];
          i4u.b[2]=record.val[2];
          i4u.b[3]=record.val[3];
          sprintf(record.val,"%u\0",i4u.i); /* overwrite in human-readable representation */  
          }
        if (!strncmp(record.type_dbm,"f4",2)) {  /* if the format is "f4" */
          f4.b[0]=record.val[0];           /* unpack the bytes into a union structure */
          f4.b[1]=record.val[1];
          f4.b[2]=record.val[2];
          f4.b[3]=record.val[3];
          sprintf(record.val,"%f\0",f4.f); /* overwrite in human-readable representation */    
          }

        //printf("[%s/%d] About to show record.val. eError=%d\n", ME, getpid(),eError);

        ////printf( "%-s %-s %-s |", record.index, label, record.val );
        //printf( "> '%-s'\n", record.val ); /* the dash in "%-s" causes left-justification */

        ///* convert, show time of last change */
        tv = record.last_change;
        tm = gmtime(&tv.tv_sec);
        //printf("> '%02d%02d%02d %02d:%02d:%02d'\n", (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

        /* note the things we need to send back */
        //memset( &c, '\0', sizeof(struct LWA_mib_entry) );
        //memcpy( c.val, record.val, MIB_VAL_FIELD_LENGTH );
        sprintf(c.val,"%s",record.val);
        memcpy( &(c.last_change), &(record.last_change), sizeof(struct timeval) );
        //c.last_change = record.last_change;

        } /* if (!eError) */

      /* report back to requester, and close connection */
      write(client_sockfd,&c,sizeof(struct LWA_mib_entry));
      close(client_sockfd);

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












