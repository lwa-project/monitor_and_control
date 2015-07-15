// meei.c: S.W. Ellingson, Virginia Tech, 2014 Feb 10
// ---
// REQUIRES: 
//   me.h
// ---
// This implements the function meei() and is intended to be 
//   #include'd with another file that is calling it.  
// Used to inject commands into MCS/Executive's me process.  
//   (This is the client-side of a client-server connection)
// See end of this file for history.

#include "me.h"

/* error codes returned by meei() */
#define MEEI_ERR_OK          0 /* OK */
#define MEEI_ERR_INVALID_CMD 1 /* command argument didn't make sense */
#define MEEI_ERR_CONNECT     2 /* couldn't do TCP/IP connection */
#define MEEI_ERR_REFUSED     3 /* me_exec refused the command */

int meei( char *cmd, char *args ) { 
  // Returns an error code; see MEEI_ERR_* #defines, above
  // commands:  "SHT" (orderly shutdown of executive) 
  // commands:  "STP" (terminates an observing session) 

  /*=================*/
  /*=== Variables ===*/
  /*=================*/

  int eResult = MEEI_ERR_OK;  /* error code to be returned. */

  int sockfd;                 /* socket file discriptor */
  struct sockaddr_in address; /* for network sockets */

  struct me_cmd_struct c;

  ///* sometimes first call to gettimeofday() returns something bogus, so let's get that out of the way */
  //gettimeofday( &tv, NULL ); 
  
  /* initialize c.args[] to be an empty string */
  sprintf(c.args,'\0');

  c.cmd = ME_CMD_NUL;
  if (strlen(cmd)<3) {
    printf("[%d/%d] FATAL: command '%s' not recognized\n",ME_MEEI,getpid(),cmd);
    eResult += MEEI_ERR_INVALID_CMD;
    return eResult;    
    }
  if (strncmp(cmd,"SHT",3)==0) { 
    c.cmd = ME_CMD_SHT; 
    }
  if (strncmp(cmd,"STP",3)==0) { 
    c.cmd = ME_CMD_STP; 
    sprintf(c.args,"%s",args);
    }

  if (c.cmd<=ME_CMD_NUL) {
    printf("[%d/%d] FATAL: command '%s' not recognized\n",ME_MEEI,getpid(),cmd);
    eResult += MEEI_ERR_INVALID_CMD;
    return eResult;  
    }

  /* create socket */
  sockfd = socket(
                  AF_INET,     /* domain; network sockets */
                  SOCK_STREAM, /* type (TCP-like) */
                  0);          /* protocol (normally 0) */
    
  /* network sockets */
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(LWA_IP_MEE);
  address.sin_port = htons(LWA_PORT_MEE);

  /* connect socket to server's socket */
  if (connect( sockfd, (struct sockaddr *) &address, sizeof(address))==-1) {
    eResult += MEEI_ERR_CONNECT;
    return eResult;
    }

  write(sockfd, &c, sizeof(struct me_cmd_struct) );
  read( sockfd, &c, sizeof(struct me_cmd_struct) );

  close(sockfd); 

  if (c.cmd<ME_CMD_NUL) eResult += MEEI_ERR_REFUSED;

  return eResult;

  } /* mesi() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// meei.c: S.W. Ellingson, Virginia Tech, 2014 Feb 10
//   .1: adding STP command
// meei.c: S.W. Ellingson, Virginia Tech, 2011 March 09
//   .1: initial version, using "mesi.c" as a starting point
// mesi.c: S.W. Ellingson, Virginia Tech, 2011 Feb 10
//   .1: add "+" option to <stime> parameter
// mesi.c: S.W. Ellingson, Virginia Tech, 2011 Feb 08
//   .1: args sdate, stime now determine outputs bScheduled, tv
// mesi.c: S.W. Ellingson, Virginia Tech, 2010 Nov 13
//   .1: Adding support for DP commands "FST", "BAM", and "DRX"
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

/* Code snipped converts c.tv to MJD/MPM and shows values 
      {
      long int tmjd=0;
      long int tmpm=0;
      LWA_timeval( &c.tv, &tmjd, &tmpm );
      printf("tmjd=%ld, tmpm=%ld\n",tmjd,tmpm);
      }
*/
