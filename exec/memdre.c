// memdre.c: S.W. Ellingson, Virginia Tech, 2010 Oct 16
// ---
// REQUIRES: 
//   me.h
// ---
// This implements the function memdre() and is intended to be 
//   #include'd with another file that is calling it.  
// Used to query MCS/Scheduler's ms_mdre_ip process.  
//   (This is the client-side of a client-server connection)
// See end of this file for history.

#include "me.h"

/* error codes returned by memdre() */
#define MEMDRE_ERR_OK           0 /* OK */
#define MEMDRE_ERR_CONNECT      1 /* Can't connect with ms_mdre_ip */
#define MEMDRE_ERR_MS_MDRE_FAIL 2 /* ms_mdre_ip returned "Err:" indicating problem */

int memdre( char *ss,    /* (input) Three-character subsystem designator */
                         /* see mcs.h for valid entries */
            char *label, /* (input) MIB label to query.*/
            char *val,   /* (output) value of that MIB entry (if error, begins 'Err: ') */
            struct timeval *last_change
          ) {
  // Returns an error code; see MEMDRE_ERR_* #defines, above

  /*=================*/
  /*=== Variables ===*/
  /*=================*/

  int eResult = MEMDRE_ERR_OK;  /* error code to be returned. */

  int sockfd;                 /* socket file discriptor */
  struct sockaddr_in address; /* for network sockets */

  //struct LWA_cmd_struct c;    /* This structure defined in me.h */
  struct LWA_mib_entry c;

  struct timeval tv;          /* from sys/time.h; included via me.h */
  struct timezone tz;

  //printf("[%d/%d] dest  = '%s'\n",ME_MEDRE,getpid(),dest);

  /* clear fields before we get started; also null-pad for safe string handling */
  memset(&c,'\0',sizeof(struct LWA_mib_entry));

  /*=== Fill in structure to send to ms_mdre_ip... ===*/
  memcpy(c.ss,ss,3);
  memcpy(c.label,label,strlen(label));

  /* create socket */
  sockfd = socket(
                  AF_INET,     /* domain; network sockets */
                  SOCK_STREAM, /* type (TCP-like) */
                  0);          /* protocol (normally 0) */
    
  /* network sockets */
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(LWA_IP_MSE);  /* This should point at ms_mdre_ip */
  address.sin_port = htons(LWA_PORT_MSE2);          /* This should point at ms_mdre_ip */

  /* connect socket to server's socket */
  if (connect( sockfd, (struct sockaddr *) &address, sizeof(address))==-1) {
    eResult += MEMDRE_ERR_CONNECT;
    return eResult;
    }

  write(sockfd, &c, sizeof(struct LWA_mib_entry));
  read(sockfd,&c,sizeof(struct LWA_mib_entry));

  close(sockfd); 

  //printf("[%d/%d] c.val='%s'\n",ME_MEMDRE,getpid(),c.val);
  sprintf(val,"%s",c.val);
  //*last_change = c.last_change;
  memcpy( last_change, &(c.last_change), sizeof(struct timeval) );

  if (!strncmp(val,"Err:",4)) {
    eResult += MEMDRE_ERR_MS_MDRE_FAIL;
    }

  return eResult;

  } /* memdre() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// memdre.c: S.W. Ellingson, Virginia Tech, 2010 Oct 16
//   .1: Initial version, using mesi.c as a starting point
// mesi.c: S.W. Ellingson, Virginia Tech, 2010 Oct 15
//   .1: Initial version, splitting msei.c into this code and the rest into mesix.c
// --- NOTE NAME CHANGE ----
// msei.c: S.W. Ellingson, Virginia Tech, 2010 Oct 12
//   .1: Adding support for scheduled actions
// msei.c: S.W. Ellingson, Virginia Tech, 2010 Jun 10
//   .1: Adding support for MCS-DR.  Nothing to do besides update comments
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Nov 7
//   .1: Changed dimension of cmd[] and dest[] from 3 to 4
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Aug 25
//   .1: Adding support for DP's TBN, CLK, INI commands; got rid of "subslot"
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Aug 17
//   .1: Adding full support (binary argument) for DP's TBW command
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Aug 16
//   .1: Added "datalen" to LWA_cmd_struct, so this is set to "-1" to indicate "string"
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Aug 06
//   .1: Fixed bug causing DATA to be truncated to first space, even when quoted
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Aug 02
//   .1: Cleaning up console output
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Jul 26
//   .1: Added LWA_cmd_struct field eMIBerror (svn rev 10)
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Jul 23
//   .1: Adding "DATA" command line argument (svn rev 7)
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Jul 20
//   .1: Adding "time to execute" field to command struct
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Jul 17
//   .1: Cleaning up; esp. implementation of LWA_cmd_struct
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Jul 13
//   .1: 
// msei.c: S.W. Ellingson, Virginia Tech, 2009 Jul 10
//   .1: preliminary version (not even numbered); 3-letter char as argument 
//   .2: changing command line and transfer format

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================

