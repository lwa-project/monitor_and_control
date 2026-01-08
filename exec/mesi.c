// // mesi.c: J. Dowell, UNM, 2015 Sep 14
// ---
// REQUIRES: 
//   me.h
// ---
// This implements the function mesi() and is intended to be 
//   #include'd with another file that is calling it.  
// Used to inject commands into MCS/Scheduler's ms_exec process.  
//   (This is the client-side of a client-server connection)
// See end of this file for history.

#include <stdlib.h>

#include "me.h"

/* error codes returned by mesi() */
#define MESI_ERR_OK         0 /* OK */
#define MESI_ERR_DEST       1 /* Didn't recognize "dest" argument */
#define MESI_ERR_CMD        2 /* Didn't recognize "cmd" argument */
#define MESI_ERR_CONNECT    4 /* connect() to scheduler (ms_exec) failed */
#define MESI_ERR_REJECTED   8 /* Scheduler (ms_exec) rejected the command */
#define MESI_ERR_DATA      16 /* "data" argument not valid for specified "cmd" */
#define MESI_ERR_FST_FOPEN 32 /* While processing DP_/ADP/NDP FST, couldn't open specified .cf file */

int mesi( int *sockfd_ptr, /* (input) existing/open socket to MCS/Sch. Use NULL to open/close a socket for this operation */
          char *dest,  /* (input) DESTINATION.  (three-letter subsystem designator) */
                       /* see mcs.h for valid entries */
          char *cmd,   /* (input) TYPE. (three-letter command TYPE) */
                       /* see mcs.h for valid entries */
          char *data,  /* (input) DATA. (DATA field for given TYPE) -- depends on command */
          char *sdate, /* (input) Date when command should run.  Either MJD or "today".  If not specified, "today" is assumed. */
          char *stime, /* (input) Time when command should run.  Either MPM, +n, or "asap".  */
                       /*         +n (e.g,. "+3") means set <stime> to current MPM + n [seconds] */
                       /*         If not specified, "asap" is assumed. */
          long int *reference /* (output) REFERENCE returned by Scheduler (ms_exec); 0 if command was rejected */  
          ) {
// Returns an error code; see MESI_ERR_* #defines, above
// Concerning "cmd":
//   For subsystems NU#, SHL, ASP, and DR#, this is ASCII.
//     (The string provided will be exactly the string used for the DATA field)
// Concerning "data":
//   For dest="NDP", this will be a list of parameters that will get translated into a raw binary DATA field
//     For cmd="TBT": Args are TBT_TRIG_TIME   (samples from start of slot, int32)
//                             TBT_SAMPLES     (samples, uint32)
//                             DRX_TUNING_MASK (server tunings to pull data from, uint64)
//     For cmd="TBS": Args are TBS_FREQ        (Hz, float64) 
//                             TBS_BW          {1..11}
//     For cmd="FST": Args are INDEX           ( -1, 0, or channel# (1-512) ) 
//                             cname           This is the name of a file, presumed to be located in 
//                                               MCS/Scheduler's "cfiles" 
//                                               directory, containing "sint16 COEFF_DATA[16][32]"
//     For cmd="BAM": Args are beam            1..NUM_BEAMS(32) (uint16 BEAM_ID) 
//                             dfile           This is the name of a file, presumed to be located in 
//                                               MCS/Scheduler's "dfiles" 
//                                               directory, containing "uint16 BEAM_DELAY[520]" 
//                             gfile           This is the name of a file, presumed to be located in 
//                                               MCS/Scheduler's "gfiles" 
//                                               directory, containing "sint16 BEAM_GAIN[260][2][2]"   
//                             drx_tuning      1..NUM_TUNINGS(32) (uint8 DRX_TUNING)
//                             sub_slot        {0..99}
//     For cmd="DRX": Args are tuning          1..NUM_TUNINGS(32)     (uint8 DRX_TUNING)
//                             freq            [Hz]                   (float64 DRX_FREQ)
//                             ebw             Bandwidth setting 0..8 (unit8 DRX_BW)
//                             gain            0..15                  (uint16 DRX_GAIN)
//                             high_dr         0 or 1                 (uint8 OBS_B)
//     For cmd="COR": Args are COR_NAVG        integration time in sub-slots (sint32 COR_NAVG)
//                             DRX_TUNING_MASK (drx tunings to pull data from, uint64)
//                             COR_GAIN        {0..15}
//                             sub_slot        {0..99}
//
//   Remember to use quotes, since "data" includes spaces
// Note meaning of arguments is sensitive to position; so if you want <time> other than "asap", you have to
//   explicitly include a <date> parameter.
// Note that the <date>/<time> corresponds to the moment at which MCS/Scheduler (specifically, the ms_exec process)
//   will send the command to the specified subystem (specifically, to the corresponding ms_mcic process).  

  /*=================*/
  /*=== Variables ===*/
  /*=================*/

  int eResult = MESI_ERR_OK;  /* error code to be returned. */

  int sockfd;
  struct sockaddr_in address; /* for network sockets */
  int bCloseSocketOnExit;

  struct LWA_cmd_struct c;    /* This structure defined in mcs.h */

  struct timeval tv;          /* from sys/time.h; included via me.h */
  struct timezone tz;

  unsigned char i1u1;
  signed char i1s1;
  unsigned short int i2u1, i2u2, i2u3;
  signed short int i2s1, i2s2;
  unsigned int i4u1, i4u2;
  signed int i4s1;
  unsigned long int i8u1, i8u2;
  float f41;                  /* assuming this is 32 bits */
  double f81;                 /* assuming this is 64 bits */

  union {
    unsigned short int i;
    unsigned char b[2];
    } i2u;
  union {
    signed short int i;
    signed char b[2];
    } i2s;
  union {
    unsigned int i;
    unsigned char b[4];
    } i4u;
  union {
    signed int i;
    signed char b[4];
    } i4s;
  union {
    unsigned long int i;
    unsigned char b[8];
  } i8u;
  union {
    float f;
    unsigned char b[4];
    } f4;
  union {
    double f;
    unsigned char b[8];
    } f8;

  unsigned char beam;
  unsigned char tuning;
  double freq;
  unsigned char ebw;
  unsigned short int gain;
  unsigned char subslot;
  unsigned char high_dr;

  long int mpm,mjd;
  
  //printf("[%d/%d] dest  = '%s'\n",ME_MESI,getpid(),dest);
  //printf("[%d/%d] cmd   = '%s'\n",ME_MESI,getpid(),cmd);
  //printf("[%d/%d] data  = '%s'\n",ME_MESI,getpid(),data);
  //printf("[%d/%d] sdate = '%s'\n",ME_MESI,getpid(),sdate);
  //printf("[%d/%d] stime = '%s'\n",ME_MESI,getpid(),stime);

  /* sometimes first call to gettimeofday() returns something bogus, so let's get that out of the way */
  gettimeofday( &tv, NULL ); 
  
  /*=== Fill in structure to send to ms_exec... ===*/

  /* figure out what subsystem */
  if ( !( c.sid = LWA_getsid( dest ) ) ) {
    printf("[%d/%d] FATAL: subsystem <%s> not recognized\n",ME_MESI,getpid(),dest);
    eResult += MESI_ERR_DEST;
    return eResult;    
    }

  /* Outbound value of ref doesn't matter. (Gets assigned by ms_exec.) */
  c.ref = 0;     

  /* figure out what command */
  if ( !( c.cid = LWA_getcmd( cmd ) ) ) {
    printf("[%d/%d] FATAL: cmd <%s> not recognized\n",ME_MESI,getpid(),cmd);
    eResult += MESI_ERR_CMD;
    return eResult; 
    }  
  //printf("[%s] c.cid=%d\n",ME,c.cid);

  ///* not implemented yet */
  //c.subslot = 0; 

  /* set bScheduled: if stime=="asap", then this is NOT scheduled; otherwise it is  */
  c.bScheduled = 1;
  if ( (strncmp(stime,"asap",4)==0) || (strlen(stime)<4) ) c.bScheduled = 0;
  //printf("<%s> %d: c.bScheduled=%d\n",stime,strncmp(stime,"asap",4),c.bScheduled);

  /* set c.tv */
  if (c.bScheduled) {  /* this is a scheduled command; need to convert sdata and stime into c.tv */

      mjd = strtol( sdate, NULL, 10 ); 
      if (!strncmp(sdate,"today",5)) { LWA_time(&mjd,&mpm); }
      //printf("<%s>=%ld\n",sdate,mjd);

      if (!strncmp(stime,"+",1)) { /* <stime> is "+n" option */
          LWA_time(&mjd,&mpm);                     /* get current mpm */
          //printf("*** current mjd=%ld current mpm=%ld\n",mjd,mpm);
          mpm += strtol( stime+1, NULL, 10 )*1000; /* add specified offset */
        } else {
          mpm  = strtol( stime,   NULL, 10 );
        }                   
      //*** if (mpm<0)        mpm+=86400000;
      if (mpm>86400000) mpm-=86400000;
      //printf("<stime>='%s' %ld\n",stime,mpm);
      //printf("*** schedul mjd=%ld schedul mpm=%ld\n",mjd,mpm);

      gettimeofday( &c.tv, NULL );
      //printf("*** current c.tv.tv_sec=%ld c.tv.tv_usec=%ld\n", c.tv.tv_sec, c.tv.tv_usec);
      LWA_time2tv( &c.tv, mjd, mpm ); 
      //printf("*** schedul c.tv.tv_sec=%ld c.tv.tv_usec=%ld\n", c.tv.tv_sec, c.tv.tv_usec);   


    } else {           /* not scheduled; we'll just use current time so that the value is at least set */
      gettimeofday( &c.tv, NULL );
    }

  /* Outbound value doesn't matter, but gets changed by ms_exec */
  c.bAccept = 0; 

  /* Outbound value doesn't matter, but gets changed by ms_exec */
  c.eSummary = LWA_SIDSUM_NULL; 

  /* Outbound value doesn't matter, but gets changed by ms_mcic */
  c.eMIBerror = LWA_MIBERR_OK;

  /* Gets changed by ms_exec */
  strcpy(c.data,data); /* changed in reply */
  c.datalen = -1; /* assumed to be a string */

  /* For NDP, c.data is raw binary, assembled from command line parameters. */
  /* the parameters are command-dependent */
  if (c.sid==LWA_SID_NDP) {
    
    switch (c.cid) {

       case LWA_CMD_PNG:
       case LWA_CMD_RPT:
       case LWA_CMD_SHT:
         break;
        
       case LWA_CMD_FST:
         // DATA field structure:
         // sint16 INDEX;
         // sint16 COEFF_DATA[16][32];

         // What we will send in this case is simply the string "data" as provided.
         // ms_mcic will convert this.
         strcpy(c.data,data); /* changed in reply */
         c.datalen = -1;      /* it's a string, for now */

         break;

       case LWA_CMD_BAM:
         // DATA field structure:
         // uint16 BEAM_ID;
         // uint16 BEAM_DELAY[512];
         // sint16 BEAM_GAIN[256][2][2];
         // uint8 sub_slot;

         // What we will send in this case is simply the string "data" as provided.
         // ms_mcic will convert this.
         strcpy(c.data,data); /* changed in reply */
         c.datalen = -1;      /* it's a string, for now */

         break;

       case LWA_CMD_DRX:
         // uint8 DRX_TUNING;
         // float64 DRX_FREQ;
         // unit8 DRX_BW;
         // uint16 DRX_GAIN;
         // uint8 high_dr;
         // uint8 sub_slot;

         // parse the input string into parameters
         sscanf(data,"%hhu %hhu %lf %hhu %hu %hhu %hhu", &beam, &tuning, &freq, &ebw, &gain, &high_dr, &subslot);
         //printf("%f %1hhu %1hhu %1hhu %1hhu\n",freq,c.data[2],c.data[3],c.data[4],c.data[5]);

         // assemble into c.data:
         memcpy( &(c.data[0]), &beam,     1 );
         memcpy( &(c.data[1]), &tuning,   1 );

         /* flipping endian-ness of freq: */
         f8.f  = freq;
         c.data[9]= f8.b[0]; c.data[8]= f8.b[1]; c.data[7]= f8.b[2]; c.data[6]= f8.b[3];
         c.data[5]= f8.b[4]; c.data[4]= f8.b[5]; c.data[3]= f8.b[6]; c.data[2]= f8.b[7];

         /* flipping endian-ness of gain: */
         i2u.i  = gain;  c.data[11]= i2u.b[1]; c.data[12]= i2u.b[0];

         memcpy( &(c.data[10]), &ebw,      1 );
         //memcpy( &(c.data[11]), (&gain)+1, 1 ); /* flipping endian-ness of gain */
         //memcpy( &(c.data[12]), (&gain)+0, 1 ); /* flipping endian-ness of gain */
         memcpy( &(c.data[13]), &high_dr, 1 );
         memcpy( &(c.data[14]), &subslot,  1 );

         f8.b[7] = c.data[2]; f8.b[6] = c.data[3]; f8.b[5] = c.data[4]; f8.b[4] = c.data[5];
         f8.b[3] = c.data[6]; f8.b[2] = c.data[7]; f8.b[1] = c.data[8]; f8.b[0] = c.data[9]; 
         freq = f8.f;  
         //printf("%f %1hhu %1hhu %1hhu %1hhu\n",freq,c.data[2],c.data[3],c.data[4],c.data[5]);

         c.datalen=15;

         break;

       case LWA_CMD_TBT:
         // DATA field structure:
         // uint64 TBT_TRIG_TIME; 
         // uint32 TBT_SAMPLES;
         // uint64 TBT_TUNING_MASK;

         i4s1 = 0;
         i4u1 = 0;
         i8u2 = 0;
         sscanf(data,"%i %u %lu",&i4s1,&i4u1,&i8u2);
         //printf("[%d/%d] TBF args: TBF_BITS=%hu, TBF_TRIG_TIME=%u, TBF_SAMPLES=%u, DRX_TUNING_MASK=%lu\n",ME_MESI,getpid(),i2u1,i4u1,i4u2,i8u1);        
 
         // Convert slot-relative times into absolute times.  Zero remains zero since
         // NDP knows what to do with zero.
         i8u1 = 0;
         if( i4s1 != 0 ) {
           i8u1 = c.tv->tv_sec * 196000000;
           i8u1 += c.tv->tv_usec * 196;
           
           i8u1 += i4s1;
         }
         
         i8u.i = i8u1; c.data[ 0]=i8u.b[7]; c.data[ 1]=i8u.b[6]; c.data[ 2]=i8u.b[5]; c.data[ 3]=i8u.b[4];
                       c.data[ 4]=i8u.b[3]; c.data[ 5]=i8u.b[2]; c.data[ 6]=i8u.b[1]; c.data[ 7]=i8u.b[0];
         i4u.i = i4u1; c.data[ 8]=i4u.b[3]; c.data[ 9]=i4u.b[2]; c.data[10]=i4u.b[1]; c.data[11]=i4u.b[0];
         i8u.i = i8u2; c.data[12]=i8u.b[7]; c.data[13]=i8u.b[6]; c.data[14]=i8u.b[5]; c.data[15]=i8u.b[4];
                       c.data[16]=i8u.b[3]; c.data[17]=i8u.b[2]; c.data[18]=i8u.b[1]; c.data[19]=i8u.b[0];

         c.datalen=20;

         break;

       case LWA_CMD_TBS:
         // DATA field structure:
         // float64 TBS_FREQ;
         // uint8 TBS_BW
         sscanf(data,"%lf %hhu", &freq, &ebw);
         /* flipping endian-ness of freq: */
         f8.f  = freq;
         c.data[7]= f8.b[0]; c.data[6]= f8.b[1]; c.data[5]= f8.b[2]; c.data[4]= f4.b[3];
         c.data[3]= f8.b[4]; c.data[2]= f8.b[5]; c.data[1]= f8.b[6]; c.data[0]= f4.b[7];

         memcpy( &(c.data[8]), &ebw,      1 );

         f8.b[7] = c.data[0]; f8.b[6] = c.data[1]; f8.b[5] = c.data[2]; f8.b[4] = c.data[3];
         f8.b[3] = c.data[4]; f8.b[2] = c.data[5]; f8.b[1] = c.data[6]; f8.b[0] = c.data[7]; 
         freq = f8.f;  
         //printf("%f %1hhu %1hhu %1hhu %1hhu\n",freq,c.data[0],c.data[1],c.data[2],c.data[3]);

         c.datalen=9;

         break;

       case LWA_CMD_COR:
         // DATA field structure:
         // sint32 COR_NAVG;
         // uint64 DRX_TUNING_MASK; 
         // sint16 COR_GAIN;
         // uint8 sub_slot;
         
         i4s1 = 0;
         i8u1 = 0;
         i2s1 = 0;
         i1u1 = 0;
         sscanf(data,"%i %lu %hi %hhu",&i4s1,&i8u1,&i2s1,&i1u1);
         //printf("[%d/%d] COR args COR_NAVG=%i, DRX_TUNING_MASK=%lu, COR_GAIN=%hu, sub_slot=%hhu\n",ME_MESI,getpid(),i4s1,i8u1,i2s1,i2s2);
         
         i4s.i = i4s1; c.data[ 0]=i4s.b[3]; c.data[ 1]=i4s.b[2]; c.data[ 2]=i4s.b[1]; c.data[ 3]=i4s.b[0];
         i8u.i = i8u1; c.data[ 4]=i8u.b[7]; c.data[ 5]=i8u.b[6]; c.data[ 6]=i8u.b[5]; c.data[ 7]=i8u.b[4];
                       c.data[ 8]=i8u.b[3]; c.data[ 9]=i8u.b[2]; c.data[10]=i8u.b[1]; c.data[11]=i8u.b[0];
         i2s.i = i2s1; c.data[12]=i2s.b[1]; c.data[13]=i2s.b[0];
                       c.data[14]=i1u1;
         
         c.datalen=15;
         
         break;
         
       case LWA_CMD_INI:
         break;
         
       case LWA_CMD_STP:
         break;
         
       default:
         printf("[%d/%d] FATAL: cmd <%s> not recognized as valid for NDP\n",ME_MESI,getpid(),cmd);
         eResult += MESI_ERR_CMD;
         return eResult;
         break;

       } /* switch (c.cid) */
       
    if (c.datalen > -1) { 
     char hex[256];
     LWA_raw2hex( c.data, hex, c.datalen );      
     //printf("[%d/%d] Outbound DATA field is: 0x%s (raw binary)\n",ME_MESI,getpid(),hex);  
    }

  } /* if (c.sid==LWA_SID_NDP) */

  /* create socket if necessary */
  if (sockfd_ptr!=NULL) {
    bCloseSocketOnExit = 0;
    sockfd = sockfd_ptr[0];
    } else {
    bCloseSocketOnExit=1;

    sockfd = socket(
                    AF_INET,     /* domain; network sockets */
                    SOCK_STREAM, /* type (TCP-like) */
                    0);          /* protocol (normally 0) */
    
    /* network sockets */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(LWA_IP_MSE);
    address.sin_port = htons(LWA_PORT_MSE);

    /* connect socket to server's socket */
    if (connect( sockfd, (struct sockaddr *) &address, sizeof(address))==-1) {
      eResult += MESI_ERR_CONNECT;
      return eResult;
      }

    } /* if (sockfd==NULL) */

  write(sockfd, &c, sizeof(struct LWA_cmd_struct));
  read( sockfd, &c, sizeof(struct LWA_cmd_struct));

  //printf("[%d/%d] ref=%ld, bAccept=%d, eSummary=%d, data=<%s>\n",ME_MESI,getpid(),c.ref,c.bAccept,c.eSummary,c.data);

  if (bCloseSocketOnExit) close(sockfd); 

  if (!c.bAccept) {
      eResult += MESI_ERR_REJECTED;
      *reference = 0;
    } else {
      *reference = c.ref;
    }

  return eResult;

  } /* mesi() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// msei.c: J. Dowell, UNM 2026 Jan 8
//   .1: Revised TBT command structure so that the trigger time is sent as uint64
// msei.c: J. Dowell, UNM, 2022 May 3
//   .1: Updated for the NDP commands
// msei.c: J. Dowell, UNM, 2018 Jan 29
//   .1: Cleaned up a few compiler warnings for ADP-based systems
// msei.c: J. Dowell, UNM, 2015 Sep 15
//   .1: Added STP as a valid command for DP/ADP
// msei.c: J. Dowell, UNM, 2015 Sep 14
//   .1: Updated for the ADP commands
// mesi.c: S.W. Ellingson, Virginia Tech, 2011 March 25
//   .1: added sockfd_ptr argument so that already-opened socket can be used
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
