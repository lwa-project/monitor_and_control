
#ifndef MCS_COMMON_H  /* this keeps this header from getting rolled in more than once */
#define MCS_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>     /* added 110309 to facilitate use of "stat()" in me_exec */
#include <sys/socket.h>
#include <sys/stat.h>   /* added 110309 to facilitate use of "stat()" in me_exec */
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h> /* added 110309 for network sockets in me_exec */
#include <sys/un.h>
#include <netinet/in.h> /* for network sockets */
#include <arpa/inet.h>  /* for network sockets */
#include <fcntl.h>      /* added 110309 for network sockets in me_exec; for F_GETFL, other possibly other stuff */
#include <signal.h>     /* added 110312 to allow me_exec to send kill(SIGKILL) to me_tpcom */ 

#if defined(__linux__)

/* Linux */
#include <byteswap.h>

#elif defined(__APPLE__)

/* OSX */
#include <libkern/OSByteOrder.h>
#define __bswap_16 OSSwapInt16
#define __bswap_32 OSSwapInt32
#define __bswap_64 OSSwapInt64

#endif

#define LWA_MAX_REFERENCE 999999999 /* largest reference number before roll-over */
#define LWA_MS_CMD_ADV_NOTICE_MS 5 /* [ms] required advance notice for time-scheduled commands */ 

/* these are for ms_mcic's (pending) task queue */
#define LWA_PTQ_SIZE 500  /* because NDP limits us to 120 commands/slot (times 3 seconds for timeout) */
#define LWA_PTQ_TIMEOUT 15 /*FIXME*/ /* timeout in seconds (increased from 4 to 5 to accommodate NDP emulator's INI) */

/* these are for ms_exec's task queue */
#define LWA_MS_TASK_QUEUE_LENGTH 740
#define LWA_MS_TASK_QUEUE_TIMEOUT 20 /*FIXME*/ /* timeout in seconds. */
                                     /* slightly longer so ms_mcic always times out before ms_exec */ 

/* ================================================= */
/* === BEGIN: IP addresses, sockets, directories === *
/* ================================================= */
/* (check /etc/services for ports already assigned) */
#include "config.h"

/* common to all configurations */
#define LWA_PORT_MSE  9734 /* port for MCS Scheduler "ms_exec" process */
#define LWA_PORT_MSE2 9735 /* port for MCS Scheduler "ms_mdre_ip" process */
#define LWA_PORT_MEE  9736 /* port for MCS Scheduler "ms_exec" process */

/* =============================================== */
/* === END: IP addresses, sockets, directories === */
/* =============================================== */

#define LWA_ASP_OP_TIME_MS 1000            /* assumed time [ms] for ASP to do one operation safely */
#define LWA_SESS_DRNDP_INIT_TIME_MS 5000    /* time [ms] to allow for NDP+DR initialization at start of session */
#define LWA_SESS_GUARD_TIME_MS 5000        /* time [ms] to allow at end of session to avoid stomping on next session */

#define MIB_REC_TYPE_BRANCH 0 
#define MIB_REC_TYPE_VALUE  1 
#define MIB_INDEX_FIELD_LENGTH 12
#define MIB_LABEL_FIELD_LENGTH 32
#define MIB_VAL_FIELD_LENGTH 8192 /* changed from 256 on 100701 for v.0.6.1 */

/* record structure for dbm */
struct dbm_record {
                    /* Note MIB index is used to key the dbm database;
                       thus this appears in a separare structure */
  int  eType;       /* branch or value; use MIB_REC_TYPE_* */
  char index[MIB_INDEX_FIELD_LENGTH];   /* MIB index. Stored as char */
  char val[MIB_VAL_FIELD_LENGTH];    /* MIB value.  Always stored as char. */
  char type_dbm[6]; /* Indicates data type used in dbm database. */  
                    /*   See elsewhere for defs */
  char type_icd[6]; /* Indicates data type/format for MCS ICD "RPT" responses. */ 
                    /*   See elsewhere for defs */
  struct timeval last_change; /* Indicates time this entry was last changed */
                       /* .tv_sec is seconds into current epoch */
                       /* .tv_usec is fractional remainder in microseconds */
  };

// Data type/format codes:
// ------------------------
// NUL:   No data stored (e.g., branch head entry)
// a####: printable (i.e., ASCII minus escape codes), #### = number of characters
//        e.g., "a3" means 3 printable ASCII-encoded characters
// r####: raw data (not printable), #### = number of bytes
//        e.g., "r1024" means 1024 bytes of raw data
// i1u:   integer, 1 byte,  unsigned, big-endian (=uint8)
// i2u:   integer, 2 bytes, unsigned, big-endian (=uint16)
// i4u:   integer, 4 bytes, unsigned, big-endian (=uint32)
// f4:    float, 4 bytes, big-endian (=float32)

//#define MQ_MAX_TEXT 512
#define MQ_MS_KEY   1000 /* key for scheduler receive queue */
                         /* (the one that the mcic's send to */
/* note: queue keys for subsystems = MQ_MS_KEY + LWA_SID_* */

/* for IP exchanges between MCS/Exec and ms_mdre_ip */
struct LWA_mib_entry {
  char ss[4];                          /* three-character subsystem designator */
  char label[MIB_LABEL_FIELD_LENGTH];  /* MIB label */
  char val[MIB_VAL_FIELD_LENGTH];      /* MIB value, as char. */
  struct timeval last_change;          /* Indicates time this entry was last changed */
                       /* .tv_sec is seconds into current epoch */
                       /* .tv_usec is fractional remainder in microseconds */
  };

#define LWA_MSELOG_TP_AVAIL     0 /* no task; used to make task queue slots as available */
#define LWA_MSELOG_TP_QUEUED    1  
#define LWA_MSELOG_TP_SENT      2 
#define LWA_MSELOG_TP_SUCCESS   3 /* subsystem accepted ("A") */
#define LWA_MSELOG_TP_FAIL_EXEC 4 
#define LWA_MSELOG_TP_FAIL_MCIC 5 
#define LWA_MSELOG_TP_FAIL_REJD 6 /* subsystem rejected ("R") */
#define LWA_MSELOG_TP_DONE_UNK  7 /* ms_mcic happy, but subsystem response not clear */ 
                                  /* (task considered done) */
#define LWA_MSELOG_TP_DONE_PTQT 8 /* ms_mcic reporting PTQ timeout (i.e., subsystem response timed out) */ 
                                  /* (task considered done) */
#define LWA_MSELOG_TP_FAIL_FILE 9 /* ms_mcic reporting unable to read file needed for FST or BAM command */ 

#define LWA_MSELOG_TP_UNSOLICITED 10 /* Response received without a corresponding request */

/* Using approach of bit-flagging (note power-of-two values) so these can be added: */
#define LWA_MIBERR_OK         0 /* no error to report */
#define LWA_MIBERR_CANTOPEN   1 /* couldn't open MIB dbm */
#define LWA_MIBERR_CANTSTORE  2 /* couldn't store to MIB dbm */
#define LWA_MIBERR_REF_UNK    4 /* REFERENCE was unrecognized, so not sure what this message is */
                                /* in response to.  Other than SUMMARY, MIB may not have been */
                                /* properly updated. */
#define LWA_MIBERR_CANTFETCH  8 /* couldn't fetch from MIB dbm */
#define LWA_MIBERR_SID_UNK   16 /* Subsystem ID (3-char) was unrecognized. Other than SUMMARY, */
                                /* MIB may not have been properly updated. */
#define LWA_MIBERR_SID_CID   32 /* Command is something this subsystem shouldn't have supported */
                                /* e.g., NU# doing something other than PNG, RPT, or SHT */
                                /* e.g., Receiving MCS as a subsystem ID */  
                                /* e.g., Subsystem MIB handler (ms_mcic_XXX.c) didn't recognize command */ 
#define LWA_MIBERR_OTHER     64 /* MIB may be out of sync for other reasons.  For example: */
                                /* - PTQ timeout, so not sure if command was acted up or not */ 

/* Subsystem SUMMARY (MIB 1.1) values */
#define LWA_SIDSUM_NULL    0 
#define LWA_SIDSUM_NORMAL  1
#define LWA_SIDSUM_WARNING 2
#define LWA_SIDSUM_ERROR   3
#define LWA_SIDSUM_BOOTING 4
#define LWA_SIDSUM_SHUTDWN 5
#define LWA_SIDSUM_UNK     6

int LWA_getsum( char *summary ) {
  /* "summary" is the 7 character (max) R-SUMMARY */
  /* returns the LWA_SIDSUM_* code, or 0 if there is an error or "NULL" */
  char summary2[8];
  int eSummary = LWA_SIDSUM_NULL;

  sscanf(summary,"%7s",summary2); /* strips off any leading or trailing whitespace */

  if (!strcmp( summary2 ,"NULL"   )) eSummary = LWA_SIDSUM_NULL;
  if (!strcmp( summary2 ,"NORMAL" )) eSummary = LWA_SIDSUM_NORMAL;
  if (!strcmp( summary2 ,"WARNING")) eSummary = LWA_SIDSUM_WARNING;
  if (!strcmp( summary2 ,"ERROR"  )) eSummary = LWA_SIDSUM_ERROR;
  if (!strcmp( summary2 ,"BOOTING")) eSummary = LWA_SIDSUM_BOOTING;
  if (!strcmp( summary2 ,"SHUTDWN")) eSummary = LWA_SIDSUM_SHUTDWN;

  return eSummary;
  } /* LWA_getsum() */

char *LWA_saysum( int sidsum ) {
  /* sidsum is one of LWA_SIDSUM_* */
  /* returns the associated status string */
  /* returns "NULL" if there is an error */
  if (sidsum == LWA_SIDSUM_NULL   ) return "NULL";
  if (sidsum == LWA_SIDSUM_NORMAL ) return "NORMAL";
  if (sidsum == LWA_SIDSUM_WARNING) return "WARNING";
  if (sidsum == LWA_SIDSUM_BOOTING) return "BOOTING";
  if (sidsum == LWA_SIDSUM_SHUTDWN) return "SHUTDWN";
  if (sidsum == LWA_SIDSUM_UNK    ) return "UNK";
  return "NULL";
  } /* LWA_saysum() */


/* === LWA Subsystem IDs === */
#define LWA_MAX_SID 20 /* maximum subsystem code; also maximum number of subsystems */
#define LWA_SID_NU1  1 /* null subsystem #1 (used for testing) */
#define LWA_SID_NU2  2 /* null subsystem #2 (used for testing) */
#define LWA_SID_NU3  3 /* null subsystem #3 (used for testing) */
#define LWA_SID_NU4  4 /* null subsystem #4 (used for testing) */
#define LWA_SID_NU5  5 /* null subsystem #5 (used for testing) */
#define LWA_SID_NU6  6 /* null subsystem #6 (used for testing) */
#define LWA_SID_NU7  7 /* null subsystem #7 (used for testing) */
#define LWA_SID_NU8  8 /* null subsystem #8 (used for testing) */
#define LWA_SID_NU9  9 /* null subsystem #9 (used for testing) */
#define LWA_SID_MCS 10 /* MCS */
#define LWA_SID_SHL 11 /* SHL (shelter) */
#define LWA_SID_ASP 12 /* ASP */
#define LWA_SID_DP_ 13 /* DP - Obsolete */
#define LWA_SID_DR1 14 /* MCS-DR #1 */
#define LWA_SID_DR2 15 /* MCS-DR #2 */
#define LWA_SID_DR3 16 /* MCS-DR #3 */
#define LWA_SID_DR4 17 /* MCS-DR #4 */
#define LWA_SID_DR5 18 /* MCS-DR #5 */
#define LWA_SID_ADP 19 /* ADP - Obsolete */
#define LWA_SID_NDP 20 /* NDP */
/* When adding subsystems, remember to change LWA_MAX_SID ! */

int LWA_getsid( char *ssc ) {
  /* ssc is the three-character subsystem code */
  /* returns the LWA subsystem ID, or 0 if there is an error */
  int sid = 0;
  if (!strcmp(ssc,"NU1")) sid = LWA_SID_NU1;
  if (!strcmp(ssc,"NU2")) sid = LWA_SID_NU2;
  if (!strcmp(ssc,"NU3")) sid = LWA_SID_NU3;
  if (!strcmp(ssc,"NU4")) sid = LWA_SID_NU4;
  if (!strcmp(ssc,"NU5")) sid = LWA_SID_NU5;
  if (!strcmp(ssc,"NU6")) sid = LWA_SID_NU6;
  if (!strcmp(ssc,"NU7")) sid = LWA_SID_NU7;
  if (!strcmp(ssc,"NU8")) sid = LWA_SID_NU8;
  if (!strcmp(ssc,"NU9")) sid = LWA_SID_NU9;
  if (!strcmp(ssc,"MCS")) sid = LWA_SID_MCS;
  if (!strcmp(ssc,"SHL")) sid = LWA_SID_SHL;
  if (!strcmp(ssc,"ASP")) sid = LWA_SID_ASP;
  if (!strcmp(ssc,"DP_")) sid = LWA_SID_DP_;
  if (!strcmp(ssc,"DR1")) sid = LWA_SID_DR1;
  if (!strcmp(ssc,"DR2")) sid = LWA_SID_DR2;
  if (!strcmp(ssc,"DR3")) sid = LWA_SID_DR3;
  if (!strcmp(ssc,"DR4")) sid = LWA_SID_DR4;
  if (!strcmp(ssc,"DR5")) sid = LWA_SID_DR5;
  if (!strcmp(ssc,"ADP")) sid = LWA_SID_ADP;
  if (!strcmp(ssc,"NDP")) sid = LWA_SID_NDP;
  return sid;
  } /* LWA_getsid() */

char *LWA_sid2str( int sid ) {
  /* sid is the LWA subsystem ID */
  /* returns the associated three-character subsystem code */
  /* returns "XXX" if there is an error */
  if (sid == LWA_SID_NU1) return "NU1";
  if (sid == LWA_SID_NU2) return "NU2";
  if (sid == LWA_SID_NU3) return "NU3";
  if (sid == LWA_SID_NU4) return "NU4";
  if (sid == LWA_SID_NU5) return "NU5";
  if (sid == LWA_SID_NU6) return "NU6";
  if (sid == LWA_SID_NU7) return "NU7";
  if (sid == LWA_SID_NU8) return "NU8";
  if (sid == LWA_SID_NU9) return "NU9";
  if (sid == LWA_SID_MCS) return "MCS";
  if (sid == LWA_SID_SHL) return "SHL";
  if (sid == LWA_SID_ASP) return "ASP";
  if (sid == LWA_SID_DP_) return "DP_";
  if (sid == LWA_SID_DR1) return "DR1";
  if (sid == LWA_SID_DR2) return "DR2";
  if (sid == LWA_SID_DR3) return "DR3";
  if (sid == LWA_SID_DR4) return "DR4";
  if (sid == LWA_SID_DR5) return "DR5";
  if (sid == LWA_SID_ADP) return "ADP";
  if (sid == LWA_SID_NDP) return "NDP";
  return "XXX";
  } /* LWA_sid2str() */
 
/* === LWA Command (TYPE) IDs === */
#define LWA_MAX_CMD  41 /* maximum code; also maximum number of commands */
#define LWA_CMD_MCSSHT 0 /* Not a subsystem command.  Directs ms_mcic to shutdown */
#define LWA_CMD_PNG      1 /* PNG */
#define LWA_CMD_RPT      2 /* RPT */
#define LWA_CMD_SHT      3 /* SHT */
#define LWA_CMD_INI      4 /* INI (SHL,ASP,NDP,DR#) */
#define LWA_CMD_TMP      5 /* TMP (SHL) */
#define LWA_CMD_DIF      6 /* DIF (SHL) */
#define LWA_CMD_PWR      7 /* PWR (SHL) */
#define LWA_CMD_FIL      8 /* FIL (ASP) */
#define LWA_CMD_AT1      9 /* AT1 (ASP) */
#define LWA_CMD_AT2     10 /* AT2 (ASP) */
#define LWA_CMD_ATS     11 /* ATS (Obsolete) */
#define LWA_CMD_FPW     12 /* FPW (ASP) */
#define LWA_CMD_RXP     13 /* RXP (ASP) */
#define LWA_CMD_FEP     14 /* FEP (ASP) */
#define LWA_CMD_TBW     15 /* TBW (Obsolete) */
#define LWA_CMD_TBN     16 /* TBN (Obsolete) */
#define LWA_CMD_DRX     17 /* DRX (NDP) */
#define LWA_CMD_BAM     18 /* BAM (NDP) */
#define LWA_CMD_FST     19 /* FST (Obsolete) */
#define LWA_CMD_CLK     20 /* CLK (Obsolete) */
#define LWA_CMD_REC     21 /* REC (DR#) */
#define LWA_CMD_DEL     22 /* DEL (DR#) */
#define LWA_CMD_STP     23 /* STP (DR#,NDP) */
#define LWA_CMD_GET     24 /* GET (DR#) */
#define LWA_CMD_CPY     25 /* CPY (DR#) */
#define LWA_CMD_DMP     26 /* DMP (DR#) */
#define LWA_CMD_FMT     27 /* FMT (DR#) */
#define LWA_CMD_DWN     28 /* DWN (DR#) */
#define LWA_CMD_UP_     29 /* UP_ (DR#).  Note ICD calls this "UP" (no underscore) */
#define LWA_CMD_SEL     30 /* SEL (DR#) */
#define LWA_CMD_SYN     31 /* SYN (DR#) */
#define LWA_CMD_TST     32 /* TST (DR#) */
#define LWA_CMD_BUF     33 /* BUF (DR#) */
#define LWA_CMD_NUL     34 /* (MCS) null command; does nothing. */
#define LWA_CMD_ESN     35 /* (MCS) end session normally */
#define LWA_CMD_ESF     36 /* (MCS) end session as failed */
#define LWA_CMD_OBS     37 /* (MCS) observation start  */
#define LWA_CMD_OBE     38 /* (MCS) observation end */
#define LWA_CMD_SPC     39 /* SPC (DR#) */
#define LWA_CMD_TBF     40 /* TBF (Obsolete) */
#define LWA_CMD_COR     41 /* COR (NDP) */
#define LWA_CMD_TBT     42 /* TBT (NDP) */
#define LWA_CMD_TBS     43 /* TBS (NDP) */
#define LWA_CMD_AT3     44 /* AT3 (ASP) */

/* When adding commands, remember to change LWA_MAX_CMD ! */

int LWA_getcmd( char *ssc ) {
  /* ssc is the three-character command (TYP) */
  /* returns the LWA command ("TYPE"), or 0 if there is an error */
  int cmd = 0;
  if (!strcmp(ssc,"PNG")) cmd = LWA_CMD_PNG;
  if (!strcmp(ssc,"RPT")) cmd = LWA_CMD_RPT;
  if (!strcmp(ssc,"SHT")) cmd = LWA_CMD_SHT;
  if (!strcmp(ssc,"INI")) cmd = LWA_CMD_INI;
  if (!strcmp(ssc,"TMP")) cmd = LWA_CMD_TMP;
  if (!strcmp(ssc,"DIF")) cmd = LWA_CMD_DIF;
  if (!strcmp(ssc,"PWR")) cmd = LWA_CMD_PWR;
  if (!strcmp(ssc,"FIL")) cmd = LWA_CMD_FIL;
  if (!strcmp(ssc,"AT1")) cmd = LWA_CMD_AT1;
  if (!strcmp(ssc,"AT2")) cmd = LWA_CMD_AT2;
  if (!strcmp(ssc,"ATS")) cmd = LWA_CMD_ATS;
  if (!strcmp(ssc,"FPW")) cmd = LWA_CMD_FPW;
  if (!strcmp(ssc,"RXP")) cmd = LWA_CMD_RXP;
  if (!strcmp(ssc,"FEP")) cmd = LWA_CMD_FEP;
  if (!strcmp(ssc,"TBW")) cmd = LWA_CMD_TBW;
  if (!strcmp(ssc,"TBN")) cmd = LWA_CMD_TBN;
  if (!strcmp(ssc,"DRX")) cmd = LWA_CMD_DRX;
  if (!strcmp(ssc,"BAM")) cmd = LWA_CMD_BAM;
  if (!strcmp(ssc,"FST")) cmd = LWA_CMD_FST;
  if (!strcmp(ssc,"CLK")) cmd = LWA_CMD_CLK;
  if (!strcmp(ssc,"REC")) cmd = LWA_CMD_REC;
  if (!strcmp(ssc,"DEL")) cmd = LWA_CMD_DEL;
  if (!strcmp(ssc,"STP")) cmd = LWA_CMD_STP;
  if (!strcmp(ssc,"GET")) cmd = LWA_CMD_GET;
  if (!strcmp(ssc,"CPY")) cmd = LWA_CMD_CPY;
  if (!strcmp(ssc,"DMP")) cmd = LWA_CMD_DMP;
  if (!strcmp(ssc,"FMT")) cmd = LWA_CMD_FMT;
  if (!strcmp(ssc,"DWN")) cmd = LWA_CMD_DWN;
  if (!strcmp(ssc,"UP_")) cmd = LWA_CMD_UP_;
  if (!strcmp(ssc,"SEL")) cmd = LWA_CMD_SEL;
  if (!strcmp(ssc,"SYN")) cmd = LWA_CMD_SYN;
  if (!strcmp(ssc,"TST")) cmd = LWA_CMD_TST;
  if (!strcmp(ssc,"BUF")) cmd = LWA_CMD_BUF;
  if (!strcmp(ssc,"NUL")) cmd = LWA_CMD_NUL;
  if (!strcmp(ssc,"ESN")) cmd = LWA_CMD_ESN;
  if (!strcmp(ssc,"ESF")) cmd = LWA_CMD_ESF;
  if (!strcmp(ssc,"OBS")) cmd = LWA_CMD_OBS;
  if (!strcmp(ssc,"OBE")) cmd = LWA_CMD_OBE;
  if (!strcmp(ssc,"SPC")) cmd = LWA_CMD_SPC;
  if (!strcmp(ssc,"TBF")) cmd = LWA_CMD_TBF;
  if (!strcmp(ssc,"COR")) cmd = LWA_CMD_COR;
  if (!strcmp(ssc,"TBT")) cmd = LWA_CMD_TBT;
  if (!strcmp(ssc,"TBS")) cmd = LWA_CMD_TBS;
  if (!strcmp(ssc,"AT3")) cmd = LWA_CMD_AT3;
  return cmd;
  } /* LWA_getcmd() */

char *LWA_cmd2str( int cmd ) {
  /* ssc is the three-character command (TYP) */
  /* returns the LWA command ("TYPE"), or "   " if there is an error */
  if (cmd == LWA_CMD_MCSSHT)  return "SHT";
  if (cmd == LWA_CMD_PNG)     return "PNG";
  if (cmd == LWA_CMD_RPT)     return "RPT";
  if (cmd == LWA_CMD_SHT)     return "SHT";
  if (cmd == LWA_CMD_INI)     return "INI";
  if (cmd == LWA_CMD_TMP)     return "TMP";
  if (cmd == LWA_CMD_DIF)     return "DIF";
  if (cmd == LWA_CMD_PWR)     return "PWR";
  if (cmd == LWA_CMD_FIL)     return "FIL";
  if (cmd == LWA_CMD_AT1)     return "AT1";
  if (cmd == LWA_CMD_AT2)     return "AT2";
  if (cmd == LWA_CMD_ATS)     return "ATS";
  if (cmd == LWA_CMD_FPW)     return "FPW";
  if (cmd == LWA_CMD_RXP)     return "RXP";
  if (cmd == LWA_CMD_FEP)     return "FEP";
  if (cmd == LWA_CMD_TBW)     return "TBW";
  if (cmd == LWA_CMD_TBN)     return "TBN";
  if (cmd == LWA_CMD_DRX)     return "DRX";
  if (cmd == LWA_CMD_BAM)     return "BAM";
  if (cmd == LWA_CMD_FST)     return "FST";
  if (cmd == LWA_CMD_CLK)     return "CLK";
  if (cmd == LWA_CMD_REC)     return "REC";
  if (cmd == LWA_CMD_DEL)     return "DEL";
  if (cmd == LWA_CMD_STP)     return "STP";
  if (cmd == LWA_CMD_GET)     return "GET";
  if (cmd == LWA_CMD_CPY)     return "CPY";
  if (cmd == LWA_CMD_DMP)     return "DMP";
  if (cmd == LWA_CMD_FMT)     return "FMT";
  if (cmd == LWA_CMD_DWN)     return "DWN";
  if (cmd == LWA_CMD_UP_)     return "UP_";
  if (cmd == LWA_CMD_SEL)     return "SEL";
  if (cmd == LWA_CMD_SYN)     return "SYN";
  if (cmd == LWA_CMD_TST)     return "TST";
  if (cmd == LWA_CMD_BUF)     return "BUF";
  if (cmd == LWA_CMD_NUL)     return "NUL";
  if (cmd == LWA_CMD_ESN)     return "ESN";
  if (cmd == LWA_CMD_ESF)     return "ESF";
  if (cmd == LWA_CMD_OBS)     return "OBS";
  if (cmd == LWA_CMD_OBE)     return "OBE";
  if (cmd == LWA_CMD_SPC)     return "SPC";
  if (cmd == LWA_CMD_TBF)     return "TBF";
  if (cmd == LWA_CMD_COR)     return "COR";
  if (cmd == LWA_CMD_TBT)     return "TBT";
  if (cmd == LWA_CMD_TBS)     return "TBS";
  if (cmd == LWA_CMD_AT3)     return "AT3";
  return "   ";
  } /* LWA_getsid() */

#define LWA_CMD_STRUCT_DATA_FIELD_LENGTH 256
struct LWA_cmd_struct {
  long int sid;      /* subsystem ID.  Must be "long int" to accomodate message queue use */
  long int ref;      /* REFERENCE number */
  int  cid;          /* command ID */
  //int  subslot;      /* subslot for  */
  int  bScheduled;   /* = 0 means "not scheduled (do as time permits)"; */
                     /* = 1 means "do as close as posible to time indicated by tv field" */  
  struct timeval tv; /* Indicates time this command is to take effect */
                     /* .tv_sec is seconds into current epoch */
                     /* .tv_usec is fractional remainder in microseconds */
  int  bAccept;      /* response: see LWA_MSELOG_TP_* */
  int  eSummary;     /* summary; see LWA_SIDSUM_* macrodefines */
  int  eMIBerror;    /* >0 if ms_mcic had a problem with the MIB.  see LWA_MIBERR_* */
  char data[LWA_CMD_STRUCT_DATA_FIELD_LENGTH]; /* DATA on way out, R-COMMENT on way back */   
  int  datalen;      /* -1 for (printable) string; 0 for zero-length; otherwise number of significant bytes */
  };

void LWA_raw2hex( char *raw, char *hex, int n ) {
  /* creates a string hex[ 0 .. 2*n ] (including \0) which is the printable */
  /* hex represenation of the bytes raw[ 0 .. n-1 ] */

  int i;
  for (i=0; i<n; i++) {
    sprintf( &(hex[2*i]), "%02x", (unsigned char) raw[i] );  
    }
  hex[2*n] = '\0';   

  return;
  } /* LWA_raw2hex() */

/* === message queue === */
size_t LWA_msz() {
  /* returns size of mq_struc structure, minus sid field, */
  /* for use in message queue commands */
  //struct mq_struct mqs;
  struct LWA_cmd_struct mqs;
  return sizeof(mqs) - sizeof(mqs.sid);
  }

void LWA_timeval( 
                 struct timeval *tv, /* time as a timeval struct */
                 long int *mjd,      /* MJD */
                 long int *mpm       /* MPM */  
                 ) {
  /* converts a timeval to MJD and MPM */
  struct tm *tm;      /* from sys/time.h */
  long int a,y,m,p,q;
  
  tm = gmtime(&(tv->tv_sec));
  //printf("LWA_timeval: %02d:%02d:%02d %ld\n", tm->tm_hour, tm->tm_min, tm->tm_sec,           
  //                                            tv->tv_usec);

  /* construct the MJD field */
  /* adapted from http://paste.lisp.org/display/73536 */
  /* can check result using http://www.csgnetwork.com/julianmodifdateconv.html */
  a = (14 - ( tm->tm_mon +1) ) / 12; 
  y = ( tm->tm_year + 1900) + 4800 - a; // tm->tm_year is the number of years since 1900
  m = ( tm->tm_mon + 1) + (12 * a) - 3; // tm->tm_mon is number of months since Jan (0..11)
  p = tm->tm_mday + (((153 * m) + 2) / 5) + (365 * y); 
  q = (y/4) - (y/100) + (y/400) - 32045; 
  *mjd = (p+q) - 2400000.5;
  *mpm = ( (tm->tm_hour)*3600 + (tm->tm_min)*60 + (tm->tm_sec) ) * 1000
                    + (tv->tv_usec)/1000 ; 

  return;
  } /* LWA_timeval() */


void LWA_time2tv( 
               struct timeval *tv,
               long int mjd, /* MJD */
               long int mpm  /* MPM */  
               ) {
  /* given MJD and MPM, returns the associated timeval */
  /* essentially, this is LWA_timeval() in reverse */

  struct timeval tv_ref;  /* from sys/time.h */
  long int mpm_ms;
  long int mpm_s;
  long int mjd_s;

  //printf("mjd = %ld, mpm = %ld\n",mjd,mpm);

  // See http://en.wikipedia.org/wiki/Julian_day for a good explanation of the relationship between
  // MJD and Unix system time.  In a nutshell:
  // MJD [days] =  JD - 2400000.5,  where JD is the Julian Day
  // UST [days] = (JD - 2440587.5), where UST is "Unix system time"; thus:

  mpm_ms = mpm % 1000;          // number of milliseconds remainder after removing integer seconds from mpm
  mpm_s  = (mpm - mpm_ms)/1000; // number of integer seconds in mpm
  mjd_s  = mjd*86400 + mpm_s;   // MJD expressed as integer seconds
  tv->tv_sec  = mjd_s - 3506716800; // integer seconds into Unix epoc; i.e., UST-MJD converted to seconds
  tv->tv_usec = mpm_ms*1000;        // microseconds remainder 

  return;
  } /* LWA_time2tv() */


void LWA_time( 
               long int *mjd,      /* MJD */
               long int *mpm       /* MPM */  
               ) {
  /* gets current time; returns MJD and MPM */

  struct timeval tv;  /* from sys/time.h */
  struct timezone tz; /* set but useless; see notes below */

  gettimeofday( &tv, &tz );     /* tz is set, but useless; see notes below */
  LWA_timeval( &tv, mjd, mpm ); /* determine MPM, MJD */
  //printf("%ld %ld\n",mjd,mpm);
  
  return;
  } /* LWA_time() */


void LWA_timeadd( struct timeval *t, long int milliseconds ) {
  long int seconds;
  seconds = milliseconds/1000;     /* break milliseconds up into integer seconds and remaining milliseconds */
  milliseconds -= (seconds*1000);
  t->tv_sec += seconds;            /* do the add and carry */
  t->tv_usec += milliseconds*1000;
  while ( (t->tv_usec) >= 1000000 ) { (t->tv_usec)-=1000000; (t->tv_sec)+=1; } 
  while ( (t->tv_usec) <        0 ) { (t->tv_usec)+=1000000; (t->tv_sec)-=1; } 
  }

long int LWA_timediff( struct timeval t1, struct timeval t0 ) {
  long int t1ms, t0ms;

  /* remove integer number of seconds that t1 and t0 have in common */
  if ( t1.tv_sec >= t0.tv_sec ) {
      t1.tv_sec -= t0.tv_sec; t0.tv_sec = 0;
    } else {
      t0.tv_sec -= t1.tv_sec; t1.tv_sec = 0;
    }
  
  /* convert into absolute milliseconds */
  t1ms = t1.tv_sec * 1000 + t1.tv_usec/1000;   
  t0ms = t0.tv_sec * 1000 + t0.tv_usec/1000; 

  return (t1ms-t0ms);
  }

int LWA_isMCSRSVD(char *label) {
  /* returns 1 if first characters of label correspond to an MCS-RESERVED MIB entry; */
  /* otherwise, 0 */
  int b = 0;
  if (!strcmp(label,"SUMMARY"))   { b=1; }
  if (!strcmp(label,"INFO"))      { b=1; }
  if (!strcmp(label,"LASTLOG"))   { b=1; }
  if (!strcmp(label,"SUBSYSTEM")) { b=1; }
  if (!strcmp(label,"SERIALNO"))  { b=1; }
  if (!strcmp(label,"VERSION"))   { b=1; }
  return b;
  } /* LWA_isMCSRSVD() */

unsigned short int LWA_i2u_swap( unsigned short int x) {
  /* changes endianness of an unsigned short int */
  return __bswap_16(x);
  }

signed short int LWA_i2s_swap( signed short int x) {
  /* changes endianness of an signed short int */
  return __bswap_16(x);
  }

unsigned int LWA_i4u_swap( unsigned int x ) {
  /* changes endianness of an unsigned int */
  return __bswap_32(x);
  }
  
signed int LWA_i4s_swap( signed int x ) {
  /* changes endianness of an signed int */
  return __bswap_32(x);
  }

unsigned long int LWA_i8u_swap( unsigned long int x ) {
  /* changes endianness of an unsigned long int */
  return __bswap_64(x);
  }

signed long int LWA_i8s_swap( signed long int x ) {
  /* changes endianness of an signed long int */
  return __bswap_64(x);
  }
  
float LWA_f4_swap( float x ) {
  /* changes endianness of a float (assumes 4 bytes) */
  union {
    float f;
    unsigned int i;
    } f4;
  f4.f = x;
  f4.i = __bswap_32(f4.i);
  return f4.f;
  }

double LWA_f8_swap( double x ) {
  /* changes endianness of a double (assumes 8 bytes) */
  union {
    double f;
    unsigned long int i;
    } f8;
  f8.f = x;
  f8.i = __bswap_64(f8.i);
  return f8.f;
  }

/*====================================================*/
/*=== MCS/Exec stuff =================================*/
/*====================================================*/

/**************************************/
/*** moved here from me.h *************/
/**************************************/

#define ME_SSMIF_FORMAT_VERSION 10
#define ME_MAX_NSTD 256
#define ME_MAX_NFEE 256
#define ME_MAX_NRPD 512
#define ME_MAX_NSEP 512
#define ME_MAX_NARB 32
#define ME_MAX_NSNAP 16
#define ME_MAX_NSERVER 5
#define ME_MAX_FEEID_LENGTH 10
#define ME_MAX_RACK 8
#define ME_MAX_PORT 50
#define ME_MAX_RPDID_LENGTH 25
#define ME_MAX_SEPID_LENGTH 25
#define ME_MAX_SEPCABL_LENGTH 25
#define ME_MAX_NARBCH 16
#define ME_MAX_ARBID_LENGTH 10
#define ME_MAX_NSNAPCH 64
#define ME_MAX_SNAPID_LENGTH 10
#define ME_MAX_SERVERID_LENGTH 10
#define ME_MAX_NDR 5
#define ME_MAX_DRID_LENGTH 10
#define ME_MAX_NPWRPORT 50
#define ME_MAX_SSNAME_LENGTH 3 /* for codes used for PWR_NAME */
#define ME_MAX_NDPOUT 5 /* NDP outputs; 1,2,3,4,5 */

/* defined observing modes (MCS0030) */
#define LWA_OM_TRK_RADEC 1
#define LWA_OM_TRK_SOL   2
#define LWA_OM_TRK_JOV   3
#define LWA_OM_STEPPED   4
#define LWA_OM_TBW       5
#define LWA_OM_TBN       6
#define LWA_OM_DIAG1     7
#define LWA_OM_TBF       8
#define LWA_OM_TRK_LUN   9
#define LWA_OM_TBT      10
#define LWA_OM_TBS      11

int LWA_getmode( char *ssc ) {
  /* ssc is a string from which to determine mode */
  /* returns the LWA observing mode ("LWA_OM_"), or 0 if there is an error */
  unsigned short int mode = 0;
  if (!strncmp(ssc,"TRK_RADEC",9)) mode = LWA_OM_TRK_RADEC;
  if (!strncmp(ssc,"TRK_SOL"  ,7)) mode = LWA_OM_TRK_SOL;
  if (!strncmp(ssc,"TRK_JOV"  ,7)) mode = LWA_OM_TRK_JOV;
  if (!strncmp(ssc,"TRK_LUN"  ,7)) mode = LWA_OM_TRK_LUN;
  if (!strncmp(ssc,"STEPPED"  ,7)) mode = LWA_OM_STEPPED;
  if (!strncmp(ssc,"DIAG1"    ,5)) mode = LWA_OM_DIAG1;
  if (!strncmp(ssc,"TBT"      ,3)) mode = LWA_OM_TBT;
  if (!strncmp(ssc,"TBS"      ,3)) mode = LWA_OM_TBS;
  return mode;
  } /* LWA_getmode() */

void LWA_saymode( unsigned short int mode, char *ssc ) {
  /* mode is one of LWA_OM_* */
  /* ssc is the associated text description */
  strcpy(ssc,"");
  if (mode==LWA_OM_TRK_RADEC) strcpy(ssc,"TRK_RADEC");
  if (mode==LWA_OM_TRK_SOL  ) strcpy(ssc,"TRK_SOL");
  if (mode==LWA_OM_TRK_JOV  ) strcpy(ssc,"TRK_JOV");
  if (mode==LWA_OM_TRK_LUN  ) strcpy(ssc,"TRK_LUN");
  if (mode==LWA_OM_STEPPED  ) strcpy(ssc,"STEPPED");
  if (mode==LWA_OM_DIAG1    ) strcpy(ssc,"DIAG1");
  if (mode==LWA_OM_TBT      ) strcpy(ssc,"TBT");
  if (mode==LWA_OM_TBS      ) strcpy(ssc,"TBS");
  return;
  } /* LWA_saymode() */

/* defined beam types (MCS0030) */
#define LWA_BT_SIMPLE            1
#define LWA_BT_HIGH_DR           2
#define LWA_BT_SPEC_DELAYS_GAINS 3

int LWA_getbeamtype( char *ssc ) {
  /* ssc is a string from which to determine beamtype */
  /* returns the LWA beam type ("LWA_BT_"), or 0 if there is an error */
  int mode = 0;
  if (!strncmp(ssc,"SIMPLE", 6)) mode = LWA_BT_SIMPLE;
  if (!strncmp(ssc,"HIGH_DR",7)) mode = LWA_BT_HIGH_DR;
  if (!strncmp(ssc,"SPEC_DELAYS_GAINS",16)) mode = LWA_BT_SPEC_DELAYS_GAINS;
  return mode;
  } /* LWA_getbeamtype() */

void LWA_saybeamtype( int mode, char *ssc ) {
  /* mode is one of LWA_BT_* */
  /* ssc is the associated text description */
  strcpy(ssc,"");
  if (mode==LWA_BT_SIMPLE ) strcpy(ssc,"SIMPLE");
  if (mode==LWA_BT_HIGH_DR) strcpy(ssc,"HIGH_DR");
  if (mode==LWA_BT_SPEC_DELAYS_GAINS) strcpy(ssc,"SPEC_DELAYS_GAINS");
  return;
  } /* LWA_saybeamtype() */

int LWA_ndpoavail( 
  signed short int ndpo,            /* NDP output: numbered 1 through 5 (5 is TBT/TBS) */
  struct timeval t0,  /* start time */
  long int d,         /* duration (ms) */
  char *path,         /* path to mess.dat, no trailing slash */
  char *msg )         /* text message summarizing result */
  /* reads mess.dat, determines if requested NDP output is available */
  /* returns <0 if error, >=0 corresponds to Status of that DP output */ 
  /* -1: invalid dpo (not in range 1..5) */ 
  /* -2: mess.dat not found */
  /* -3: time conflict */ 
  {
  FILE *fp;
  char *filename;
  char line[256];
  int ndpos[ME_MAX_NDPOUT];
  int i;
  long int mjd,mpm;
  int ndpox;
  long int durx;
  int cra;
  char project_id[256];
  char session_id[256];
  struct timeval t1,t2,t3;

   if ((ndpo<1) || (ndpo>ME_MAX_NDPOUT)) {
    sprintf(msg,"ndpo=%d is out of range (1-%i)",ndpo,ME_MAX_NDPOUT);
    return -1;
    }
    
  filename = (char*) malloc(strlen(path)+strlen("/mess.dat")+1);
  if (!filename) {
    sprintf(msg,"can't open file '%s'",filename);
    return -2;
    }
  strcpy(filename, path);
  strcat(filename, "/mess.dat");
  if (!(fp=fopen(filename,"r"))) {
    sprintf(msg,"can't open file '%s'",filename);
    free(filename);
    return -2;
    }
    
  for (i=0;i<ME_MAX_NDPOUT;i++) ndpos[i]=-1; /* initialize these */
  
  fgets(line,256,fp); /* this should be MCS_CRA */
  fgets(line,256,fp); /* this should five numbers representing status of each beam output */
  //printf("LWA_dpoavail: <%s>\n",line);
  sscanf(line,"%d %d %d %d %d",&(ndpos[0]),&(ndpos[1]),&(ndpos[2]),&(ndpos[3]),&(ndpos[4])); 

  t1.tv_sec  = t0.tv_sec;
  t1.tv_usec = t0.tv_usec;
  LWA_timeadd( &t1, d );    /* t1 is now the end time for the requested session */

  fgets(line,256,fp);
  while (!feof(fp)) {

    sscanf(line,"%ld %ld %d %ld %d %255s %255s",&mjd,&mpm,&ndpox,&durx,&cra,project_id,session_id);
    //printf("LWA_ndpoavail: %ld %ld %d %ld %d <%s> <%s>\n",mjd,mpm,dpox,durx,cra,project_id,session_id);

    if (ndpox==ndpo) {
      LWA_time2tv( &t2, mjd, mpm ); /* get start time as a timeval */
      t3.tv_sec  = t2.tv_sec;       /* construct end time as a timeval */
      t3.tv_usec = t2.tv_usec;
      LWA_timeadd( &t3, durx );     /* end time as a timeval */     
      if ( ( LWA_timediff(t3,t0)>0 ) && ( LWA_timediff(t1,t2)>0 ) ) {
        fclose(fp);
        sprintf(msg,"conflict with project '%s' session '%s'",project_id,session_id);
        free(filename);
        return -3;
        }
      }

    fgets(line,256,fp);
    if (strlen(line)<32) fgets(line,256,fp);

    }

  fclose(fp); 
  free(filename);

  if (ndpos[ndpo-1]<1) {
    sprintf(msg,"UNAVAILABLE (status = %d)",ndpos[ndpo-1]);
    return 0; /* not available because this output has status <1 */
    }

  sprintf(msg,"available with status = %d",ndpos[ndpo-1]);
  return ndpos[ndpo-1];
  }

/* defines a session */
struct ssf_struct {
  unsigned short int FORMAT_VERSION;
  char PROJECT_ID[9];
  unsigned int SESSION_ID;
  unsigned short int SESSION_CRA;
  signed short int   SESSION_DRX_BEAM;
  char               SESSION_SPC[32];
  unsigned long int  SESSION_START_MJD;
  unsigned long int  SESSION_START_MPM;
  unsigned long int  SESSION_DUR;
  unsigned int       SESSION_NOBS;
  signed short int SESSION_MRP_ASP;
  signed short int SESSION_MRP_NDP;
  signed short int SESSION_MRP_DR1;
  signed short int SESSION_MRP_DR2;
  signed short int SESSION_MRP_DR3;
  signed short int SESSION_MRP_DR4;
  signed short int SESSION_MRP_DR5;
  signed short int SESSION_MRP_SHL;
  signed short int SESSION_MRP_MCS;
  signed short int SESSION_MUP_ASP;
  signed short int SESSION_MUP_NDP;
  signed short int SESSION_MUP_DR1;
  signed short int SESSION_MUP_DR2;
  signed short int SESSION_MUP_DR3;
  signed short int SESSION_MUP_DR4;
  signed short int SESSION_MUP_DR5;
  signed short int SESSION_MUP_SHL;
  signed short int SESSION_MUP_MCS;
  signed char SESSION_LOG_SCH;
  signed char SESSION_LOG_EXE;
  signed char SESSION_INC_SMIB;
  signed char SESSION_INC_DES;
  };

struct osf_struct {
  unsigned short int FORMAT_VERSION;
  char               PROJECT_ID[9];
  unsigned int       SESSION_ID;
  signed short int   SESSION_DRX_BEAM;
  char               SESSION_SPC[32];
  unsigned int       OBS_ID; 
  unsigned long int  OBS_START_MJD;
  unsigned long int  OBS_START_MPM;
  unsigned long int  OBS_DUR;
  unsigned short int OBS_MODE;
  char               OBS_BDM[32];  /* added 140310 */
  float              OBS_RA;
  float              OBS_DEC;
  unsigned short int OBS_B;
  unsigned int       OBS_FREQ1;
  unsigned int       OBS_FREQ2;
  unsigned short int OBS_BW;
  unsigned int       OBS_STP_N;
  unsigned short int OBS_STP_RADEC;
  };

struct osfs_struct { /* one step within an observation */
  float              OBS_STP_C1;
  float              OBS_STP_C2;
  unsigned int       OBS_STP_T;
  unsigned int       OBS_STP_FREQ1;
  unsigned int       OBS_STP_FREQ2;
  unsigned short int OBS_STP_B;
  };

#define LWA_MAX_NSTD 256 /* FIXME should be reconciled with ME_MAX_NSTD */
struct beam_struct {
  unsigned short int OBS_BEAM_DELAY[2*LWA_MAX_NSTD];
  signed short int   OBS_BEAM_GAIN[LWA_MAX_NSTD][2][2];
  };

struct osf2_struct { /* really just a continuation of osf_struct */
  signed short int   OBS_FEE[LWA_MAX_NSTD][2];
  signed short int   OBS_ASP_FLT[LWA_MAX_NSTD];
  signed short int   OBS_ASP_AT1[LWA_MAX_NSTD];
  signed short int   OBS_ASP_AT2[LWA_MAX_NSTD];
  signed short int   OBS_ASP_AT3[LWA_MAX_NSTD];
  unsigned int       OBS_TBT_SAMPLES;
  signed short int   OBS_DRX_GAIN;
  };

struct me_action_struct {  /* atomic unit of action as me_exec executes session */
  struct timeval tv;  /* time at which to execute action */
  int bASAP;          /* =1 means ignore tv; instead do this ASAP. =0 otherwise */
  int sid;            /* subsystem ID; see LWA_SID_* */
  int cid;            /* command ID; see LWA_CMD_* */
  int len;            /* number of bytes in remainder of command */
  };

/* this sub-structure is used in both the ssmif and sdm */
struct station_settings_struct {
  signed short int mrp_asp; // SESSION_MRP_ASP // MRP_ASP
  signed short int mrp_ndp; // SESSION_MRP_NDP // MRP_NDP
  signed short int mrp_dr1; // SESSION_MRP_DR1 // MRP_DR1
  signed short int mrp_dr2; // SESSION_MRP_DR2 // MRP_DR2
  signed short int mrp_dr3; // SESSION_MRP_DR3 // MRP_DR3
  signed short int mrp_dr4; // SESSION_MRP_DR4 // MRP_DR4
  signed short int mrp_dr5; // SESSION_MRP_DR5 // MRP_DR5
  signed short int mrp_shl; // SESSION_MRP_SHL // MRP_SHL
  signed short int mrp_mcs; // SESSION_MRP_MCS // MRP_MCS
  signed short int mup_asp; // SESSION_MUP_ASP // MUP_ASP
  signed short int mup_ndp; // SESSION_MUP_NDP // MUP_NDP
  signed short int mup_dr1; // SESSION_MUP_DR1 // MUP_DR1
  signed short int mup_dr2; // SESSION_MUP_DR2 // MUP_DR2
  signed short int mup_dr3; // SESSION_MUP_DR3 // MUP_DR3
  signed short int mup_dr4; // SESSION_MUP_DR4 // MUP_DR4
  signed short int mup_dr5; // SESSION_MUP_DR5 // MUP_DR5
  signed short int mup_shl; // SESSION_MUP_SHL // MUP_SHL
  signed short int mup_mcs; // SESSION_MUP_MCS // MUP_MCS
  signed short int fee[LWA_MAX_NSTD];     // OBS_FEE[LWA_MAX_NSTD][2]  // FEE[LWA_MAX_NSTD]
  signed short int asp_flt[LWA_MAX_NSTD]; // OBS_ASP_FLT[LWA_MAX_NSTD] // ASP_FLT[LWA_MAX_NSTD]
  signed short int asp_at1[LWA_MAX_NSTD]; // OBS_ASP_AT1[LWA_MAX_NSTD] // ASP_AT1[LWA_MAX_NSTD]
  signed short int asp_at2[LWA_MAX_NSTD]; // OBS_ASP_AT2[LWA_MAX_NSTD] // ASP_AT2[LWA_MAX_NSTD]
  signed short int asp_at3[LWA_MAX_NSTD]; // OBS_ASP_AT3[LWA_MAX_NSTD] // ASP_AT3[LWA_MAX_NSTD]
  signed short int drx_gain; // OBS_DRX_GAIN // DRX_GAIN
  };

/* structure corresponding to SSMIF */
struct ssmif_struct {
  int    iFormatVersion;           /* FORMAT_VERSION */
  char   sStationID[3];            /* STATION_ID */
  double fGeoN;                    /* GEO_N */
  double fGeoE;                    /* GEO_E */
  double fGeoEl;                   /* GEO_EL */
  int    nStd;                     /* N_STD */
  double fStdLx[ME_MAX_NSTD];      /* STD_LX[] */
  double fStdLy[ME_MAX_NSTD];      /* STD_LY[] */
  double fStdLz[ME_MAX_NSTD];      /* STD_LZ[] */
  int    iAntStd[2*ME_MAX_NSTD];   /* ANT_STD[] */
  int    iAntOrie[2*ME_MAX_NSTD];  /* ANT_ORIE[] */
  int    iAntStat[2*ME_MAX_NSTD];  /* ANT_STAT[] */
  float  fAntTheta[2*ME_MAX_NSTD]; /* ANT_THETA[] */
  float  fAntPhi[2*ME_MAX_NSTD];   /* ANT_PHI[] */
  int    eAntDesi[2*ME_MAX_NSTD];  /* ANT_DESI[] */
  int    nFEE;                     /* N_FEE */
  char   sFEEID[ME_MAX_NFEE][ME_MAX_FEEID_LENGTH+1]; /* FEE_ID[] */
  int    iFEEStat[ME_MAX_NFEE];    /* FEE_STAT[] */
  int    eFEEDesi[ME_MAX_NFEE];    /* FEE_DESI[] */
  float  fFEEGai1[ME_MAX_NFEE];    /* FEE_GAI1[] */
  float  fFEEGai2[ME_MAX_NFEE];    /* FEE_GAI2[] */
  int    iFEEAnt1[ME_MAX_NFEE];    /* FEE_ANT1[] */
  int    iFEEAnt2[ME_MAX_NFEE];    /* FEE_ANT2[] */
  int    iFEERack[ME_MAX_NFEE];    /* FEE_RACK[] */
  int    iFEEPort[ME_MAX_NFEE];    /* FEE_PORT[] */
  int    nRPD;                     /* N_RPD */
  char   sRPDID[ME_MAX_NRPD][ME_MAX_RPDID_LENGTH+1]; /* RPD_ID[] */
  int    iRPDStat[ME_MAX_NRPD];    /* RPD_STAT[] */
  int    eRPDDesi[ME_MAX_NRPD];    /* RPD_DESI[] */
  float  fRPDLeng[ME_MAX_NRPD];    /* RPD_LENG[] */
  float  fRPDVF[ME_MAX_NRPD];      /* RPD_VF[] */
  float  fRPDDD[ME_MAX_NRPD];      /* RPD_DD[] */
  float  fRPDA0[ME_MAX_NRPD];      /* RPD_A0[] */
  float  fRPDA1[ME_MAX_NRPD];      /* RPD_A1[] */
  float  fRPDFref[ME_MAX_NRPD];    /* RPD_FREF[] */
  float  fRPDStr[ME_MAX_NRPD];     /* RPD_STR[] */
  int    iRPDAnt[ME_MAX_NRPD];     /* RPD_ANT[] */
  int    nSEP;                     /* N_SEP */
  char   sSEPID[ME_MAX_NSEP][ME_MAX_SEPID_LENGTH+1]; /* SEP_ID[] */
  int    iSEPStat[ME_MAX_NSEP];    /* SEP_STAT[] */
  char   sSEPCabl[ME_MAX_NSEP][ME_MAX_SEPCABL_LENGTH+1]; /* SEP_Cabl[] */
  float  fSEPLeng[ME_MAX_NSEP];    /* SEP_LENG[] */
  int    eSEPDesi[ME_MAX_NSEP];    /* SEP_DESI[] */
  float  fSEPGain[ME_MAX_NSEP];    /* SEP_GAIN[] */
  int    iSEPAnt[ME_MAX_NSEP];     /* SEP_ANT[] */
  int    nARB;                     /* N_ARB */
  int    nARBCH;                   /* N_ARBCH */
  char   sARBID[ME_MAX_NARB][ME_MAX_ARBID_LENGTH+1]; /* ARB_ID[] */
  int    iARBSlot[ME_MAX_NARB];    /* ARB_SLOT[] */
  int    eARBDesi[ME_MAX_NARB];    /* ARB_DESI[] */
  int    iARBRack[ME_MAX_NARB];    /* ARB_RACK[] */
  int    iARBPort[ME_MAX_NARB];    /* ARB_PORT[] */
  int    eARBStat[ME_MAX_NARB][ME_MAX_NARBCH];       /* ARB_STAT[][] */
  float  fARBGain[ME_MAX_NARB][ME_MAX_NARBCH];        /* ARB_GAIN[][] */
  int    iARBAnt[ME_MAX_NARB][ME_MAX_NARBCH];        /* ARB_ANT[][] */
  char   sARBIN[ME_MAX_NARB][ME_MAX_NARBCH][ME_MAX_ARBID_LENGTH+1]; /* ARB_IN[][] */
  char   sARBOUT[ME_MAX_NARB][ME_MAX_NARBCH][ME_MAX_ARBID_LENGTH+1]; /* ARB_OUT[][] */
  int    nSnap;                     /* N_SNAP */
  int    nSnapCh;                   /* N_SNAPCH */
  char   sSnapID[ME_MAX_NSNAP][ME_MAX_SNAPID_LENGTH+1]; /* SNAP_ID[] */
  char   sSnapSlot[ME_MAX_NSNAP][ME_MAX_SNAPID_LENGTH+1]; /* SNAP_SLOT[] */
  int    eSnapDesi[ME_MAX_NSNAP]; /* SNAP_DESI[] */
  int    eSnapStat[ME_MAX_NSNAP][ME_MAX_NSNAPCH];       /* SNAP_STAT[][] */
  char   sSnapINR[ME_MAX_NSNAP][ME_MAX_NSNAPCH][ME_MAX_SNAPID_LENGTH+1]; /* SNAP_INR[][] */
  char   sSnapINC[ME_MAX_NSNAP][ME_MAX_NSNAPCH][ME_MAX_SNAPID_LENGTH+1]; /* SNAP_INC[][] */
  int    iSnapAnt[ME_MAX_NSNAP][ME_MAX_NSNAPCH];        /* SNAP_ANT[][] */
  int    nServer;                     /* N_SERVER */
  char   sServerID[ME_MAX_NSERVER][ME_MAX_SERVERID_LENGTH+1]; /* SERVER_ID[] */
  char   sServerSlot[ME_MAX_NSERVER][ME_MAX_SERVERID_LENGTH+1]; /* SERVER_SLOT[] */
  int    eServerStat[ME_MAX_NSERVER];       /* SERVER_STAT[] */
  int    eServerDesi[ME_MAX_NSERVER];       /* SERVER_DESI[] */
  int    nDR;                     /* N_DR */
  int    eDRStat[ME_MAX_NDR];       /* DR_STAT[] */
  char   sDRID[ME_MAX_NDR][ME_MAX_DRID_LENGTH+1]; /* DR_ID[] */
  char   sDRPC[ME_MAX_NDR][ME_MAX_DRID_LENGTH+1]; /* DR_PC[] */
  int    iDRNDP[ME_MAX_NDR];       /* DR_NDP[] */
  int    nPwrRack;                /* N_PWR_RACK */
  int    nPwrPort[ME_MAX_RACK];   /* N_PWR_PORT[] */
  int    ePwrSS[ME_MAX_RACK][ME_MAX_NPWRPORT]; /* PWR_SS[][], converted to a LWA_SID_ value */
  char   sPwrName[ME_MAX_RACK][ME_MAX_NPWRPORT][ME_MAX_SSNAME_LENGTH+1]; /* PWR_NAME[][] */
  int    eCRA;                /* MCS_CRA */
  float  fPCAxisTh; /* PC_AXIS_TH */
  float  fPCAxisPh; /* PC_AXIS_PH */
  float  fPCRot;    /* PC_ROT */
  struct station_settings_struct settings; /* these are the statically-defined versions of these things */
  };

/**************************************/
/*** station dynamic MIB (SDM) ********/
/**************************************/

/* subsystem status */
struct subsystem_status_struct {
  int summary;        /* SUMMARY; one of LWA_SIDSUM_* */
  char info[256];     /* INFO */
  struct timeval tv;  /* time SUMMARY and INFO were last updated */
  };

/* sub-sub-system status */
/* note: this is the subset of the SSMIF with things that can change */
struct subsubsystem_status_struct {
  int    eFEEStat[ME_MAX_NFEE];                /* FEE_STAT[] */
  int    eRPDStat[ME_MAX_NRPD];                /* RPD_STAT[] */
  int    eSEPStat[ME_MAX_NSEP];                /* SEP_STAT[] */
  int    eARBStat[ME_MAX_NARB][ME_MAX_NARBCH]; /* ARB_STAT[][] */
  int    eSnapStat[ME_MAX_NSNAP][ME_MAX_NSNAPCH]; /* SNAP_STAT[][] */
  int    eServerStat[ME_MAX_NSERVER];             /* SERVER_STAT[] */
  int    eDRStat[ME_MAX_NDR];                  /* DR_STAT[] */
  };

/* station dynamic MIB (SDM) */
struct sdm_struct  {
  struct    subsystem_status_struct station;        /* Station overall status */
  struct    subsystem_status_struct shl;            /* SHL status */
  struct    subsystem_status_struct asp;            /* ASP status */
  struct    subsystem_status_struct ndp;            /* NDP status */
  struct    subsystem_status_struct dr[ME_MAX_NDR]; /* DR# status (0=DR1,1=DR2,...) */
  struct subsubsystem_status_struct ssss;  /* correspond to ssmif "stat" items */
  int ant_stat[ME_MAX_NSTD][2]; /* corresponds to sc.Stand[i].Ant[k].iSS, but dynamically updated */
  int ndpo_stat[ME_MAX_NDR];    /* corresponds to sc.NDPO[i].iStat, but dynamically updated */ 
  struct station_settings_struct settings; /* these are the current, dynamically-varying versions of these things */
  };

/*******************************************************************/
/*** me_sdm_write() ************************************************/
/*******************************************************************/

void me_sdm_write( struct sdm_struct *sdm ) {

  FILE* fp;

  fp=fopen("state/sdm.dat","wb");
  fwrite(sdm,sizeof(struct sdm_struct),1,fp);
  fclose(fp);
  } /* me_sdm_write() */


/**************************************/
/* station configuration structure    */
/**************************************/

/* FEE configuration */
struct fee_port_struct {
  int i;     /* index (in 1..N_FEE) of FEE */
  int p;     /* =0 for FEE port 1, =1 for FEE port 2 */
  int iStat; /* FEE_STAT */
  };

/* RPD configuration */
struct rpd_struct {
  int i;     /* index (in 1..N_RPD) of RPD */
  int iStat; /* RPD_STAT */
  };

/* SEP configuration */
struct sep_struct {
  int i;     /* index (in 1..N_SEP) of SEP */
  int iStat; /* SEP_STAT */
  char sID[ME_MAX_SEPID_LENGTH+1]; /* SEP_ID */
  };

/* ARX channel configuration */
struct arx_ch_struct {
  int i;     /* index of ARB (in 1..N_ARB) */
  int c;     /* index of ARB channel (in 1..N_ARB) */
  int iStat; /* ARB_STAT[][] */
  };

/* NDP channel configuration */
struct ndp_ch_struct {
  int i;     /* index of DP1 (in 1..N_DP1) */
  int c;     /* index of DP1 input channel (in 1..N_DP1CH) */
  int iStat; /* NDP_STAT[][] */
  };

/* Antenna analog configuration. Describes antenna and connections to it. */
struct ant_struct {
  int    iSS;  /* this is summary status computed from iStat variables for connected things */
  int    iID;
  int    iStat;  /* ANT_STAT[] */
  float  fTheta; /* ANT_THETA[] */
  float  fPhi;   /* ANT_PHI[] */
  int    eDesi;  /* ANT_DESI[] */
  struct fee_port_struct FEE;
  struct rpd_struct RPD;
  struct sep_struct SEP;
  struct arx_ch_struct ARX;
  struct ndp_ch_struct NDP;
  };

/* Stand analog configuration. Describes 1 stand (both antennas). */
struct stand_struct {
  double fLx;    /* STD_LX[] */
  double fLy;    /* STD_LX[] */
  double fLz;    /* STD_LX[] */
  struct ant_struct Ant[2]; /* '0' is antenna orientation '0' (N-S) (ANT_ORIE[]) */
                            /* '1' is antenna orientation '1' (E-W)              */
  };

/* Describes 1 DP output (i.e., 4 x DRX + TBT/TBS) output, through DR */
struct ndpo_struct {
  int iStat;   /* summary status for this output */
  int iDR;     /* the DR index (1-5) attached to this NDP output */
  };

/* Station configuration */
struct sc_struct {
  struct stand_struct Stand[ME_MAX_NSTD];
  struct ndpo_struct NDPO[ME_MAX_NDPOUT]; /* NDP output; 1..4 are beams; 5 is TBT/TBS */
  };

/**************************************/
/*** me_sc_MakeASM( ) *****************/
/**************************************/

#define ME_SC_MAKEASM_ERR_STD_OUT_OF_RANGE 1
#define ME_SC_MAKEASM_ERR_ORI_OUT_OF_RANGE 2
int me_sc_MakeASM( struct ssmif_struct s, struct sc_struct *sc ) {
  /* make analog signal map (basically, fill in sc) using s */
  int i,k,m;
  int c;
  int iErr = 0;

  /* load stand locations into station config data structure */  
  for ( i=0; i<s.nStd; i++ ) {
    sc->Stand[i].fLx = s.fStdLx[i];
    sc->Stand[i].fLy = s.fStdLy[i];
    sc->Stand[i].fLz = s.fStdLz[i];
    }

  /* load antenna information into station config data structure */
  for ( i=0; i<(2*s.nStd); i++ ) {

    k = s.iAntStd[i]-1;  /* this is the stand that this antenna is associated with */ 
    if ( (k<0) || (k>=s.nStd) ) {
      printf("[%d] me_sc_MakeASM(): i=%d. k=%d is invalid\n",getpid(),i,k);
      return ME_SC_MAKEASM_ERR_STD_OUT_OF_RANGE;
      }
    m = s.iAntOrie[i]; /* this is the orientation of this antenna */
    if ( (m<0) || (m>1) ) {
      printf("[%d] me_sc_MakeASM(): i=%d. m=%d is invalid\n",getpid(),i,m);
      return ME_SC_MAKEASM_ERR_ORI_OUT_OF_RANGE;
      }

    sc->Stand[k].Ant[m].iID    = i;
    sc->Stand[k].Ant[m].iStat  = s.iAntStat[i];
    sc->Stand[k].Ant[m].fTheta = s.fAntTheta[i];
    sc->Stand[k].Ant[m].fPhi   = s.fAntPhi[i];
    sc->Stand[k].Ant[m].eDesi  = s.eAntDesi[i];

    }

  /* load FEE information into station config data structure */
  for ( i=0; i<s.nFEE; i++ ) {

    m = s.iFEEAnt1[i]-1; /* this is the ANT that FEE port 1 is associated with */ 
    k = s.iAntStd[m]-1;  /* this is the STD that FEE port 1 is associated with */
    sc->Stand[k].Ant[s.iAntOrie[m]].FEE.i = i;    
    sc->Stand[k].Ant[s.iAntOrie[m]].FEE.p = 0; /* FEE port 1 */
    sc->Stand[k].Ant[s.iAntOrie[m]].FEE.iStat = s.iFEEStat[i]; 
  
    //printf("%d %d %d | %d %d %d\n",i,m,k,sc.Stand[k].Ant[s.iAntOrie[m]].FEE.i,sc.Stand[k].Ant[s.iAntOrie[m]].FEE.p,sc.Stand[k].Ant[s.iAntOrie[m]].FEE.iStat);

    m = s.iFEEAnt2[i]-1; /* this is the ANT that FEE port 2 is associated with */ 
    k = s.iAntStd[m]-1;  /* this is the STD that FEE port 2 is associated with */
    sc->Stand[k].Ant[s.iAntOrie[m]].FEE.i = i;    
    sc->Stand[k].Ant[s.iAntOrie[m]].FEE.p = 1; /* FEE port 2 */
    sc->Stand[k].Ant[s.iAntOrie[m]].FEE.iStat = s.iFEEStat[i]; 

    //printf("%d %d %d | %d %d %d\n",i,m,k,sc.Stand[k].Ant[s.iAntOrie[m]].FEE.i,sc.Stand[k].Ant[s.iAntOrie[m]].FEE.p,sc.Stand[k].Ant[s.iAntOrie[m]].FEE.iStat);

    }

  /* load RPD information into station config data structure */
  for ( i=0; i<s.nRPD; i++ ) {
    m = s.iRPDAnt[i]-1; /* this is the ANT that this RPD is associated with */ 
    k = s.iAntStd[m]-1; /* this is the STD that this RPD is associated with */
    sc->Stand[k].Ant[s.iAntOrie[m]].RPD.i = i;    
    sc->Stand[k].Ant[s.iAntOrie[m]].RPD.iStat = s.iRPDStat[i]; 
    }

  /* load SEP information into station config data structure */
  for ( i=0; i<s.nSEP; i++ ) {
    m = s.iSEPAnt[i]-1; /* this is the ANT that this SEP is associated with */ 
    k = s.iAntStd[m]-1; /* this is the STD that this SEP is associated with */
    sc->Stand[k].Ant[s.iAntOrie[m]].SEP.i = i;    
    sc->Stand[k].Ant[s.iAntOrie[m]].SEP.iStat = s.iSEPStat[i];
    sprintf(sc->Stand[k].Ant[s.iAntOrie[m]].SEP.sID,"%s",s.sSEPID[i]); 
    }

  /* load ARX channel information into station config data structure */
  for ( i=0; i<s.nARB; i++ ) {
    for ( c=0; c<s.nARBCH; c++ ) {
      m = s.iARBAnt[i][c]-1; /* this is the ANT that this ARX channel is associated with */ 

      if (m>=0) { /* will not be true if this ARX channel has not been associated with an antenna */

        k = s.iAntStd[m]-1;    /* this is the STD that this ARX channel is associated with */

        //printf("%d %d %d %d m=%d k=%d\n",i,c,s.nARB,s.nARBCH,m,k);
        // --- below (new SSMIF) did crash:
        // 32 7 33 16 m=515 k=257
        // 32 8 33 16 m=-1 k=-1072249702
        // --- below (old SSMIF) did not crash:
        // 32 7 33 16 m=515 k=257
        // 32 8 33 16 m=-1 k=-1

        sc->Stand[k].Ant[s.iAntOrie[m]].ARX.i = i;  
        sc->Stand[k].Ant[s.iAntOrie[m]].ARX.c = c;     
        sc->Stand[k].Ant[s.iAntOrie[m]].ARX.iStat = s.eARBStat[i][c];

        } /* if (m>=0) */

      } /* for c */ 
    } /* for i */

  /* load SNAP channel information into station config data structure */
  for ( i=0; i<s.nSnap; i++ ) {
    for ( c=0; c<s.nSnapCh; c++ ) {
      m = s.iSnapAnt[i][c]-1; /* this is the ANT that this SNAP channel is associated with */
      k = s.iAntStd[m]-1;    /* this is the STD that this SNAP channel is associated with */
      sc->Stand[k].Ant[s.iAntOrie[m]].NDP.i = i;
      sc->Stand[k].Ant[s.iAntOrie[m]].NDP.c = c;
      sc->Stand[k].Ant[s.iAntOrie[m]].NDP.iStat = s.eSnapStat[i][c];
      } /* for c */
    } /* for i */

  /* compute the stand's iStat */
  for ( i=0; i<s.nStd; i++ ) {
    for ( k=0; k<2; k++ ) {
      sc->Stand[i].Ant[k].iSS = 3;
      if ( sc->Stand[i].Ant[k].iStat     < sc->Stand[i].Ant[k].iSS ) { sc->Stand[i].Ant[k].iSS = sc->Stand[i].Ant[k].iStat;     }
      if ( sc->Stand[i].Ant[k].FEE.iStat < sc->Stand[i].Ant[k].iSS ) { sc->Stand[i].Ant[k].iSS = sc->Stand[i].Ant[k].FEE.iStat; }
      if ( sc->Stand[i].Ant[k].RPD.iStat < sc->Stand[i].Ant[k].iSS ) { sc->Stand[i].Ant[k].iSS = sc->Stand[i].Ant[k].RPD.iStat; }
      if ( sc->Stand[i].Ant[k].SEP.iStat < sc->Stand[i].Ant[k].iSS ) { sc->Stand[i].Ant[k].iSS = sc->Stand[i].Ant[k].SEP.iStat; }
      if ( sc->Stand[i].Ant[k].ARX.iStat < sc->Stand[i].Ant[k].iSS ) { sc->Stand[i].Ant[k].iSS = sc->Stand[i].Ant[k].ARX.iStat; }
      if ( sc->Stand[i].Ant[k].NDP.iStat < sc->Stand[i].Ant[k].iSS ) { sc->Stand[i].Ant[k].iSS = sc->Stand[i].Ant[k].NDP.iStat;  }
      }
    }

  return iErr;
  } /* me_sc_MakeASM() */

/**************************************/
/*** me_sc_MakeDSM( ) *****************/
/**************************************/

int me_sc_MakeDSM( struct ssmif_struct s, struct sc_struct *sc ) {
  /* make digital signal map (basically, fill in sc) using s */
  int i;
  int iErr = 0;

  /* figure out which DR is assigned to each DP output */
  for ( i=0; i<s.nDR; i++ ) { sc->NDPO[ s.iDRNDP[i]-1 ].iDR = i+1; }

  /* figure out status of each DP/ADP output */
  /* Note that only the status of the associated DP board and DR are considered. */ 
  for ( i=0; i<ME_MAX_NDPOUT; i++ ) {

    /* assume this until we find otherwise */
    sc->NDPO[i].iStat = 3;

    /* consider status of attached data recorder */
    if ( s.eDRStat[ sc->NDPO[i].iDR -1 ] < sc->NDPO[i].iStat ) { 
      sc->NDPO[i].iStat = s.eDRStat[ sc->NDPO[i].iDR -1 ]; 
      }

    /* consider the status of the ROACH/SNAP boards */
    if (i<ME_MAX_NDPOUT) { /* i=0 to 1: These are standard beams */
       /* no basis for marking this anything other than "3" at the moment*/
      sc->NDPO[i].iStat = 3;
    }
    
    /* consider the status of the servers */
    if (i<ME_MAX_NDPOUT) { /* i=0 to 1: These are standard beams */
       /* no basis for marking this anything other than "3" at the moment*/
      sc->NDPO[i].iStat = 3;
    }

    }

  return iErr;
  } /* me_sc_MakeDSM() */



/*============================================*/

#ifdef __cplusplus
} // extern "C"
#endif

#endif // #ifndef MCS_COMMON_H 

//==================================================================================
//=== NOTES ========================================================================
//==================================================================================
//
// MCS/Sch "ME" Codes
// 1 ms_init.c
// 2 ms_exec.c
// 3 ms_mcic1.c (MCS Common ICD Client, frontend) -- deprecated
// 4 ms_mcic2.c (MCS Common ICD Client, backend) -- deprecated
// 5 dat2dbm.c
// 6 ms_mcic.c  (MCS Common ICD Client, replaces 3 and 4)
// 7 msei.c  
// 8 ms_mdr.c  (MCS/Scheduler MIB dbm-file reader)
// 9 ms_mdre.c (MCS/Scheduler MIB dbm-file reader for entries)
// 10 ms_makeMIB_ASP.c 
// 11 ms_makeMIB_DP.c 
// 12 ms_md2t.c
// 13 ms_mb.c
// 14 ms_mu.c
// 15 ms_makeMIB_DR.c
// 16 ms_mdre_ip.c
// 17 ms_mon.c

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// Jan 21, 2025: Added LWA_MSELOG_TP_UNSOLICITED
// Feb 21, 2023: Moved some configuration parameters into config.h
// Sep 30, 2022: Added support for the moon tracking mode TRK_LUN
// May 2, 2022:  Added new SIDs and CMDs for the Next Generation Digital Processor (NDP)
// Dec 16, 2019: Updated the ADP-based stations to support a third beam and
//               four data recorders
// Feb 18, 2018: Updated the ADP-based stations to support a second beam and
//               three data recorders
// Aug 25, 2016: Updated the ADP-based stations to support two data recorders
// Sep 11, 2015: Updated various structures and limits for ADP-based stations
// Jul 14, 2015: Added new SID and CMDs for the Advanced Digital Processor (ADP)
// Mar 10, 2014: Added BDM command; added OBS_BDM keyword to osf_ struct
// Jan 23, 2013: Fixed index overrun in me_sc_MakeASM( ) (see lwa1 staff exploder emails)
// Apr 12, 2012: Modified dpoavail() error codes to facilitate change in tpss
// Feb 16, 2012: Added SPC command
//               Added SESSION_SPC keyword to ssf_ and osf_ structs
// Mar 26, 2011: Moved ME_MAX_* #define's here (from me.h)
//               Moved station dynamic MIB structure here  
// Mar 24, 2011: Significant modifications and additions while implementing me_exec
//               and me_inproc
// Mar 17, 2011: Significant modifications and additions while implementing me_exec
//               and me_inproc
// Mar 07, 2011: Merged LWA_MCS.h and LWA_MCS_Subsystems.h into mcs.h;
//               deprecated LWA_MCS.h and LWA_MCS_Subsystems.h.
//               So now Sch, Exec, and TP all use common (this) header file
// Feb 08, 2011: Replaced LWA_time2tv() implementation with the one from MCS/exec
// Jul 01, 2010: Changed MIB_VAL_FIELD_LENGTH from 256 to 8192
// May 31, 2010: Added utility function LWA_f4_swap()
// May 30, 2010: Added utility functions LWA_i2u_swap() and LWA_i4u_swap()
// May 25, 2010: Increased LWA_PTQ_TIMEOUT from 4 to 5 to accommodate NDP emulator
// Aug 25, 2009: Got rid of "subslot" field
// Aug 17, 2009: Added LWA_isMCSRSVD()
// Aug 16, 2009: Added LWA_raw2hex(); added datalen to LWA_cmd_struct
// Aug 15, 2009: Revising codes used in type_dbm and type_icd
// Jul 31, 2009: Added LWA_MS_TASK_QUEUE_TIMEOUT
//               Separated out LWA_MCS_subsystems.h (svn rev 17)
// Jul 30, 2009: Minor changes/fixes (svn rev 16)
// Jul 30, 2009: (svn rev 15)
// Jul 28, 2009: Attempting to straighten out timekeeping:
//               time_t replacing explicit typing (long ints)
//               gmtime() replacing localtime()               (svn rev 12)
//               added LWA_time2tv(); fixed time-related bugs (svn rev 13)
// Jul 28, 2009: (svn rev 11)
// Jul 26, 2009: Fleshing out LWA_getsum(), added LWA_MIBERR_*
//               Swapping index and label for use in dbm files (svn rev 10)
// Jul 24, 2009: Added LWA_getsum() (svn rev 9)
// Jul 24, 2009: Added LWA_time() (svn rev 8)
// Jul 23, 2009: Added LWA_sid2str(), LWA_cmd2str(), LWA_timeval() functions (svn rev 7)
// Jul 20, 2009: Modified LWA_cmd_struct to include a timeval struct field
// Jul 17, 2009: Removed "mq_struct", replaced with LWA_cmd_struct
// Jul 13, 2009: Renamed "LWA_MCS.h" (was "mib.h")
