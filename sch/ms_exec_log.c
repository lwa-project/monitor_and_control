// ms_exec_log.c: S.W. Ellingson, Virginia Tech, 2012 Apr 12
// ---
// COMPILE: #INCLUDEd in ms_exec.c; see that file.
// ---
// COMMAND LINE: no applicable
// ---
// REQUIRES: 
//	Intended to be #INCLUDEd in ms_exec.c; uses it's #INCLUDEs.
// ---
// Handles file logging.
// See end of this file for history.

// Log file line format
// ====================
// "Task Progress" messages:
// aaaaaa bbbbbbbbb T ccccccccc d eee fff g*
// a  = MJD (6 characters, right justified)
// b  = MPM (9 characters, right justified)
// The character "T" to signal that this is a task progress message
// c  = REFERENCE (9 characters, right justified)
// d  = task progress (see also LWA_MSELOG_TP_* macro defines): 
//      "1"=  queued 
//      "2"=  sent; no response yet
//      "3"=  success: received response, accepted by subsystem
//      "4"=  failure: not sent, failed at ms_exec
//      "5"=  failure: sent, but failed ms_mcic
//      "6"=  failure: sent, but rejected by subsystem
//      "10"= unsolicited response received from subsystem
// e  = DESTINATION (subsystem identified in associated command message)
// f  = TYPE (i.e., command send)
// g* = DATA. 
//      For d=0 or 1, this is the DATA field of command message
//      For d=2 or 5, this is the DATA field of the response message
//      For d>3 or 4, this is a remark returned by ms_exec or ms_mcic respectively
// ------------------------
// "Information" messages:
// aaaaaa bbbbbbbbb N c*
// a  = MJD (6 characters, right justified)
// b  = MPM (9 characters, right justified)
// The character "N" to signal that this is an info message
// c* = the message

#include <ctype.h>  /* for isgraph() */

#include "fileutils.h"

#define LWA_MSELOG_FILENAME "mselog.txt"
#define LWA_MSELOG_LENGTH 139
#define LWA_MSELOG_LINES 10000 /* number of lines before starting a new file */

#define LWA_MSELOG_MTYPE_TASK 0
#define LWA_MSELOG_MTYPE_INFO 1

/* LWA_MSELOG_TP_* enumeration in LWA_MCS.h */

#define LWA_MSELOG_COMMENT_FIELD_LENGTH 90

int LWA_mse_log(
                FILE *fp,
                int msg_type, /* LWA_MSELOG_MTYPE_* */
                long int ref, /* REFERENCE,       or 0 for LWA_MSELOG_MTYPE_INFO */
                int tp,       /* LWA_MSELOG_TP_*, or 0 for LWA_MSELOG_MTYPE_INFO */ 
                int sid,      /* DESTINATION,     or 0 for LWA_MSELOG_MTYPE_INFO */ 
                int cid,      /* TYPE,            or 0 for LWA_MSELOG_MTYPE_INFO */ 
                char *data,   /* DATA             or the info message */
                int datalen,  /* ==-1 means data is a string; otherwise, */
                              /* shows this many bytes in printable hex form */
                long int *line_ctr /* number of lines currently in mselog */
               ) {
  long int mjd;
  long int mpm;
  int bSuccess=0;
  char comment_field[LWA_MSELOG_COMMENT_FIELD_LENGTH+1] = {'\0'};
  char hex[LWA_MSELOG_COMMENT_FIELD_LENGTH];
  int i;

  struct timeval tv;
  struct timezone tz;
  struct tm *tm;
  char time_string[256];

  char filename[1024];

  /* get the current time in conventional format */
  gettimeofday(&tv,&tz); 
  tm = gmtime(&tv.tv_sec);
  sprintf(time_string,"%02d%02d%02d %02d:%02d:%02d", 
         (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
 
  /* get the current time in LWA (MJD/MPM) format */
  LWA_time( &mjd, &mpm ); 

  /* construct comment field */
  if (datalen==-1) { /* data is a string */
       strncpy( comment_field, data, LWA_MSELOG_COMMENT_FIELD_LENGTH );
       if (strlen(data) > LWA_MSELOG_COMMENT_FIELD_LENGTH ) { /* If the message is too long, add a truncation marker */
         comment_field[LWA_MSELOG_COMMENT_FIELD_LENGTH-3] = '.';
         comment_field[LWA_MSELOG_COMMENT_FIELD_LENGTH-2] = '.';
         comment_field[LWA_MSELOG_COMMENT_FIELD_LENGTH-1] = '.';
       }
     } else {        /* data is not a string; convert to printable hex form */  
       i=datalen;
       if ((2*i)>LWA_MSELOG_COMMENT_FIELD_LENGTH) { i = LWA_MSELOG_COMMENT_FIELD_LENGTH/2; }
       LWA_raw2hex( data, hex, i );         
       memcpy( comment_field, hex, (2*i)+1 );          
     }

  ///* make sure comment_field consists only of printable characters */
  //for ( i=0; i<strlen(comment_field); i++ ) {
  //  if ( !isgraph(comment_field[i]) ) {
  //    //comment_field[i] = 120; /* "x" */
  //    comment_field[i] = 32; /* " " */
  //    }
  //  }

  switch (msg_type) {
    
    case LWA_MSELOG_MTYPE_TASK:
      fprintf( fp, "%s %6ld %9ld T %9ld %2d %3s %3s %s|\n", time_string, mjd, mpm, ref, tp, 
               LWA_sid2str(sid), 
               LWA_cmd2str(cid), 
               comment_field 
             );
      bSuccess = 1;
      break;
  
    case LWA_MSELOG_MTYPE_INFO:
      fprintf( fp, "%s %6ld %9ld N %s\n", time_string, mjd, mpm, comment_field ); 
      //printf( "%6ld %9ld N %s\n", mjd, mpm, data ); 
      bSuccess = 1;
      break;
    
    } /* switch (msg_type) */

  fflush(fp);

  /* check to see if it's time to close current file and open new one */
  (*line_ctr)++;
  if ( (*line_ctr) >= LWA_MSELOG_LINES ) {
    fclose(fp);
    sprintf(filename,"mselog_%06ld_%09ld.txt",mjd,mpm);
    LWA_move_file(LWA_MSELOG_FILENAME, filename);
    fp = fopen(LWA_MSELOG_FILENAME,"w");
    (*line_ctr)=0;
    }

  return bSuccess;
  } /* LWA_mse_log() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// ms_exec_log.c: S.W. Ellingson, Virginia Tech, 2012 Apr 12
//   .1: Changed LWA_mse_log to close & open new file when #lines exceed threshold
// ms_exec_log.c: S.W. Ellingson, Virginia Tech, 2009 Aug 16
//   .1: Modified to handle "datalen" argument for strings/raw binary
// ms_exec_log.c: S.W. Ellingson, Virginia Tech, 2009 Aug 01
//   .1: Extra columns for UT day/time (svn rev 19) (svn rev 20)
// ms_exec_log.c: S.W. Ellingson, Virginia Tech, 2009 Jul 31
//   .1: Changed name, since this was just logging, and not really a header file (svn rev 17,18)
// ms_exec.h: S.W. Ellingson, Virginia Tech, 2009 Jul 30
//   .1: Limiting size of comment field & checking for non-printable characters (svn rev 16)
// ms_exec.h: S.W. Ellingson, Virginia Tech, 2009 Jul 24
//   .1: First code drafted; file logging  (svn rev 8) moved some stuff with LWA_MCS.h (svn rev 9)
