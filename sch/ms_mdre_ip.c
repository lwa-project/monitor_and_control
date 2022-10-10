// ms_mdre_ip.c: S.W. Ellingson, Virginia Tech, 2010 Oct 16
// ---
// COMPILE:  gcc -o ms_mdre_ip -I/usr/include/gdbm ms_mdre_ip.c -lgdbm 
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
#include <gdbm.h>

#if defined(__linux__)
/* Linux */
#include <byteswap.h>
#elif defined(__APPLE__) && defined(__MACH__)
/* OSX */
#include <libkern/OSByteOrder.h>
#define bswap_16 OSSwapInt16
#define bswap_32 OSSwapInt32
#define bswap_64 OSSwapInt64
#endif

/* additional stuff related to sockets */
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h> /* for network sockets */
#include <arpa/inet.h>  /* for network sockets */

//#include "LWA_MCS.h" 
#include "mcs.h"

#define MY_NAME "ms_mdre_ip (v.20191030.1)"
#define ME "16" 

int main ( int narg, char *argv[] ) {

  /*=================*/
  /*=== Variables ===*/
  /*=================*/

  /* dbm-related variables */
  char dbm_filename[256];
  GDBM_FILE dbm_ptr;
  struct dbm_record record;
  datum datum_key;
  datum datum_data;

  struct timeval tv;  /* from sys/time.h; included via LWA_MCS.h */
  struct tm *tm;      /* from sys/time.h; included via LWA_MCS.h */

  char label[MIB_LABEL_FIELD_LENGTH];     /* this is the key for dbm */
  char key[MIB_LABEL_FIELD_LENGTH];

  union {
    unsigned char i;
    unsigned char b[1];
    } i1u;
  union {
    signed char i;
    unsigned char b[1];
    } i1s;
  union {
    unsigned short int i;
    unsigned char b[2];
    } i2u;
  union {
    signed short int i;
    unsigned char b[2];
    } i2s;
  union {
    unsigned int i;
    unsigned char b[4];
    } i4u;
  union {
    signed int i;
    unsigned char b[4];
    } i4s;
  union {
    unsigned long int i;
    unsigned char b[8];
    } i8u;
  union {
    signed long int i;
    unsigned char b[8];
    } i8s;
  union {
    float f;
    unsigned int i;
    unsigned char b[4];
    } f4;
  union {
    double f;
    unsigned long int i;
    unsigned char b[8];
    } f8;
 
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
      sprintf(dbm_filename,"%s.gdb",c.ss); /* dbm_filename is the TLA-subsystem identifier */
      sprintf(label,"%s",c.label);     /* and the label is the label... */

      //printf("> dbm_filename='%s'\n",dbm_filename);
      //printf("> label='%s'\n",label);   

       /* Open dbm file and look for entry */
      dbm_ptr = gdbm_open(dbm_filename, 0, GDBM_READER, 0, NULL);

      if (!dbm_ptr) { /* not able to open dbm file */

          //printf("[%s/%d] FATAL: Failed to open dbm <%s>. eError=%d\n",ME,getpid(),dbm_filename,eError);
          eError = 1;
          sprintf(c.val,"Err: Failed to open dbm - %s",gdbm_strerror(gdbm_errno));
          //c.last_change = 0;

        } else { /* able to open dbm file.  Look for label... */

          sprintf(key,"%s",label);
          datum_key.dptr = key;
          datum_key.dsize = strlen(key);
          datum_data = gdbm_fetch(dbm_ptr,datum_key);
          if (datum_data.dptr) {
              //printf("[%s/%d] Found it. eError=%d.\n", ME, getpid(),eError);
              memcpy( &record, datum_data.dptr, datum_data.dsize );
              free(datum_data.dptr);    // Need to cleanup after a gdbm_fetch
              //strncpy(ip_address,record.val,15);
            } else {
              //printf("[%s/%d] Failed to find label=<%s> in dbm. eError=%d\n", ME, getpid(),label,eError);
              eError = 1;
              sprintf(c.val,"Err: Failed to find label");
              //c.last_change = 0;
            }

          /* Close dbm file */
          gdbm_close(dbm_ptr);

        } /* if (!dbm_ptr) */

      if (!eError) {

        /* Decide how to show record.val: This depends on record.type_dbm: */
        if (!strncmp(record.type_dbm,"NUL",3)) { /* if the format is "NUL" (e.g., branch entries)... */
          strcpy(record.val,"NUL");            /* print "NUL" for value */
          }
        if (!strncmp(record.type_dbm,"a",1)) { 
                                                 /* do nothing; fine the way it is */
          }    
        if (!strncmp(record.type_dbm,"r",1)) {   /* if the field is not printable... */
          strcpy(record.val,"@...");           /* just print "@" instead */
          }
        if (!strncmp(record.type_dbm,"i1u",3)) {  /* if the format is "i1u" */
          i1u.b[0]=record.val[0];           /* unpack the bytes into a union structure */
          sprintf(record.val,"%hhu",i1u.i); /* overwrite in human-readable representation */ 
          }
        if (!strncmp(record.type_dbm,"i1s",3)) { /* if the format is "i1s" */
          i1s.b[0]=record.val[0];           /* unpack the bytes into a union structure */
          sprintf(record.val,"%hhi",i1s.i); /* overwrite in human-readable representation */ 
          }
        if (!strncmp(record.type_dbm,"i2u",3)) {  /* if the format is "i2u" */   
          i2u.b[0]=record.val[0];           /* unpack the bytes into a union structure */
          i2u.b[1]=record.val[1];
          if (!strncmp(record.type_dbm,"i2ur",4)) {  /* if the format is "i2ur" (same as i2u, but big-endian) */
            i2u.i = bswap_16(i2u.i);
            }
          sprintf(record.val,"%hu",i2u.i); /* overwrite in human-readable representation */ 
          }
        if (!strncmp(record.type_dbm,"i2s",3)) {  /* if the format is "i2s" */   
          i2s.b[0]=record.val[0];           /* unpack the bytes into a union structure */
          i2s.b[1]=record.val[1];
          if (!strncmp(record.type_dbm,"i2sr",4)) {  /* if the format is "i2sr" (same as i2s, but big-endian) */
            i2s.i = bswap_16(i2s.i);
            }
          sprintf(record.val,"%hi",i2s.i); /* overwrite in human-readable representation */ 
          }
        if (!strncmp(record.type_dbm,"i4u",3)) {  /* if the format is "i4u" */   
          i4u.b[0]=record.val[0];           /* unpack the bytes into a union structure */
          i4u.b[1]=record.val[1];
          i4u.b[2]=record.val[2];
          i4u.b[3]=record.val[3];
          if (!strncmp(record.type_dbm,"i4ur",4)) {  /* if the format is "i4ur" (same as i4u, but big-endian) */
            i4u.i = bswap_32(i4u.i);
            }
          sprintf(record.val,"%u",i4u.i); /* overwrite in human-readable representation */  
          }
        if (!strncmp(record.type_dbm,"i4s",3)) {  /* if the format is "i4s" */   
          i4s.b[0]=record.val[0];           /* unpack the bytes into a union structure */
          i4s.b[1]=record.val[1];
          i4s.b[2]=record.val[2];
          i4s.b[3]=record.val[3];
          if (!strncmp(record.type_dbm,"i4sr",4)) {  /* if the format is "i4ur" (same as i4s, but big-endian) */
            i4s.i = bswap_32(i4s.i);
            }
          sprintf(record.val,"%i",i4s.i); /* overwrite in human-readable representation */  
          }
        if (!strncmp(record.type_dbm,"i8u",3)) {  /* if the format is "i8u" */   
          i8u.b[0]=record.val[0];           /* unpack the bytes into a union structure */
          i8u.b[1]=record.val[1];
          i8u.b[2]=record.val[2];
          i8u.b[3]=record.val[3];
          i8u.b[4]=record.val[4];
          i8u.b[5]=record.val[5];
          i8u.b[6]=record.val[6];
          i8u.b[7]=record.val[7];
          if (!strncmp(record.type_dbm,"i8ur",4)) {  /* if the format is "i8ur" (same as i8u, but big-endian) */
            i8u.i = bswap_64(i8u.i);
            }
          sprintf(record.val,"%lu",i8u.i); /* overwrite in human-readable representation */  
          }
        if (!strncmp(record.type_dbm,"i8s",3)) {  /* if the format is "i8s" */   
          i8s.b[0]=record.val[0];           /* unpack the bytes into a union structure */
          i8s.b[1]=record.val[1];
          i8s.b[2]=record.val[2];
          i8s.b[3]=record.val[3];
          i8s.b[4]=record.val[4];
          i8s.b[5]=record.val[5];
          i8s.b[6]=record.val[6];
          i8s.b[7]=record.val[7];
          if (!strncmp(record.type_dbm,"i8sr",4)) {  /* if the format is "i8sr" (same as i8s, but big-endian) */
            i8s.i = bswap_64(i8s.i);
            }
          sprintf(record.val,"%li",i8s.i); /* overwrite in human-readable representation */  
          }
        if (!strncmp(record.type_dbm,"f4",2)) {  /* if the format is "f4" */
          f4.b[0]=record.val[0];           /* unpack the bytes into a union structure */
          f4.b[1]=record.val[1];
          f4.b[2]=record.val[2];
          f4.b[3]=record.val[3];
          if (!strncmp(record.type_dbm,"f4r",3)) {  /* if the format is "f4r" (same as f4, but big-endian) */
             f4.i = bswap_32(f4.i);    
          }
          sprintf(record.val,"%f",f4.f); /* overwrite in human-readable representation */    
          }
        if (!strncmp(record.type_dbm,"f8",2)) {  /* if the format is "f8" */
          f8.b[0]=record.val[0];           /* unpack the bytes into a union structure */
          f8.b[1]=record.val[1];
          f8.b[2]=record.val[2];
          f8.b[3]=record.val[3];
          f8.b[4]=record.val[4];
          f8.b[5]=record.val[5];
          f8.b[6]=record.val[6];
          f8.b[7]=record.val[7];
          if (!strncmp(record.type_dbm,"f8r",3)) {  /* if the format is "f8r" (same as f8, but big-endian) */
            f8.i = bswap_64(f8.i);    
            }
          sprintf(record.val,"%f",f8.f); /* overwrite in human-readable representation */    
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












