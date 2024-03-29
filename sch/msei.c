// msei.c: S.W. Ellingson, Virginia Tech, 2010 Jun 10
// ---
// COMPILE: gcc -o msei msei.c
// ---
// COMMAND LINE: msei <dest> <cmd> <data>
//   <dest> = DESTINATION.  (three-letter subsystem designator)
//            see LWA_MCS.h for valid entries
//   <cmd>  = TYPE. (three-letter command TYPE)
//            see LWA_MCS.h for valid entries
//   <data> = DATA. (DATA field for given TYPE) -- OPTIONAL (depends on command)
//            For NU#, SHL, ASP, and DR#, this is ASCII; use single quotes if spaces are important.
//              (The string provided will be exactly the string used for the DATA field)
//            For DP, this will be a list of parameters that will get translated into a raw binary DATA field
// ---
// REQUIRES: 
// ---
// Used to inject commands into MCS/Scheduler's ms_exec process.  
//   (This is the client-side of a client-server connection)
// See end of this file for history.

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h> /* for network sockets */
#include <arpa/inet.h>  /* for network sockets */

//#include "LWA_MCS.h"
#include "mcs.h"

#define MY_NAME "msei (v.20220502.1)"
#define ME "7" 

#define B 256

int main ( int narg, char *argv[] ) {

  /*=================*/
  /*=== Variables ===*/
  /*=================*/

  int sockfd;                 /* socket file discriptor */
  struct sockaddr_in address; /* for network sockets */

  //int len;
  int result;

  char dest[4];
  char cmd[4];
  char data[B];
  //int sid;
  //int cid;

  struct LWA_cmd_struct c; 

  struct timeval tv;  /* from sys/time.h; included via LWA_MCS.h */
  struct timezone tz;

  int bError;
  short int i2u1, i2u2, i2u3;
  int i4u1, i4u2, i4s1, i4s2;
  float f41; /* assuming this is 32 bits */
  long int i8u1;

  union {
    unsigned short int i;
    unsigned char b[2];
    } i2u;
  union {
    unsigned int i;
    unsigned char b[4];
    } i4u;
  union {
    int i;
    char b[4];
    } i4s;
  union {
    unsigned long int i;
    char b[8];
    } i8u;
  union {
    float f;
    unsigned char b[4];
    } f4;

  /*==================*/
  /*=== Initialize ===*/
  /*==================*/
    
  /* First, announce thyself */
  //printf("I am %s [%s]\n",MY_NAME,ME);

  if (narg>1) { 
    sscanf(argv[1],"%3s",dest);
    //printf("dest = <%s>\n",dest);
    } else {
    printf("[%s] FATAL: dest not provided\n",ME);
    exit(EXIT_FAILURE);
    } 
  if (narg>2) { 
    sscanf(argv[2],"%3s",cmd);
    //printf("dest = <%s>\n",dest);
    } else {
    printf("[%s] FATAL: cmd not provided\n",ME);
    exit(EXIT_FAILURE);
    } 
  strcpy(data,"");
  if (narg>3) { 
    strncpy(data,argv[3],strlen(argv[3])+1);
    } 


  /*=== Fill in structure to send to ms_exec... ===*/

  /* figure out what subsystem */
  if ( !( c.sid = LWA_getsid( dest ) ) ) {
    printf("[%s] FATAL: subsystem <%s> not recognized\n",ME,dest);
    exit(EXIT_FAILURE);    
    }

  /* Outbound value of ref doesn't matter. (Gets assigned by ms_exec.) */
  c.ref = 0;     

  /* figure out what command */
  if ( !( c.cid = LWA_getcmd( cmd ) ) ) {
    printf("[%s] FATAL: cmd <%s> not recognized\n",ME,cmd);
    exit(EXIT_FAILURE); 
    }  
  //printf("[%s] c.cid=%d\n",ME,c.cid);

  ///* not implemented yet */
  //c.subslot = 0; 

  /* Not to be scheduled; just do as soon as time permits */
  c.bScheduled = 0;

  /* Outbound value sent to current time; won't matter because it's not */
  /* scheduled (see above) */
  gettimeofday( &c.tv, &tz );

  /* Outbound value doesn't matter, but gets changed by ms_exec */
  c.bAccept = 0; 

  /* Outbound value doesn't matter, but gets changed by ms_exec */
  c.eSummary = LWA_SIDSUM_NULL; 

  /* Outbound value doesn't matter, but gets changed by ms_mcic */
  c.eMIBerror = LWA_MIBERR_OK;

  /* Gets changed by ms_exec */
  strcpy(c.data,data); /* changed in reply */
  c.datalen = -1; /* assumed to be a string */
  
#if defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP
  /* For NDP, c.data is raw binary, assembled from command line parameters. */
  /* the parameters are command-dependent */
  if (c.sid==LWA_SID_NDP) {
    
    switch (c.cid) {

       case LWA_CMD_PNG:
       case LWA_CMD_RPT:
       case LWA_CMD_SHT:
         break;
        
       case LWA_CMD_TBF:
         // DATA field structure:
         // uint8 TBF_BITS;
         // sint32 TBF_TRIG_TIME; 
         // sint32 TBF_SAMPLES;
         // uint64 DRX_TUNING_MASK
         bError=0;
         if (narg>3) { sscanf(argv[3],"%hu",&i2u1); } else {bError=1;}
         if (narg>4) { sscanf(argv[4],"%i", &i4s1); } else {bError=1;}
         if (narg>5) { sscanf(argv[5],"%i", &i4s2); } else {bError=1;}
         if (narg>6) { sscanf(argv[6],"%lu",&i8u1); } else {bError=1;}
         if (bError) {
           printf("[%s] FATAL: %s/%s args are:\n TBF_BITS (number, uint8)\nTBF_TRIG_TIME (samples, int32)\n TBF_SAMPLES (samples, int32)\n DRX_TUNING_MASK (mask, uint64)\n",ME,dest,cmd);
           return;
           }
         //printf("[%s] %hu %u %u\n",ME,i2u1,i4u1,i4u2); return;
         i2u.i = i2u1;                      c.data[ 0]=i2u.b[0]; 
         i4s.i = i4s1; c.data[ 1]=i4s.b[3]; c.data[ 2]=i4s.b[2]; c.data[ 3]=i4s.b[1]; c.data[ 4]=i4s.b[0];
         i4s.i = i4s2; c.data[ 5]=i4s.b[3]; c.data[ 6]=i4s.b[2]; c.data[ 7]=i4s.b[1]; c.data[ 8]=i4s.b[0];
         i8u.i = i8u1; c.data[ 9]=i8u.b[7]; c.data[10]=i8u.b[6]; c.data[11]=i8u.b[5]; c.data[12]=i8u.b[4];
                       c.data[13]=i8u.b[3]; c.data[14]=i8u.b[2]; c.data[15]=i8u.b[1]; c.data[16]=i8u.b[0];
         c.datalen=17;
         break;

       case LWA_CMD_COR:
         // DATA field structure:
         // int32 COR_NAVG;
         // uint64  DRX_TUNING_MASK
         // int16 COR_GAIN
         // uint8 sub_slot
         bError=0;
         if (narg>3) { sscanf(argv[3],"%i", &i4s1); } else {bError=1;}
         if (narg>4) { sscanf(argv[4],"%lu",&i8u1); } else {bError=1;}
         if (narg>5) { sscanf(argv[5],"%hu",&i2u2); } else {bError=1;}
         if (narg>6) { sscanf(argv[6],"%hu",&i2u3); } else {bError=1;}
         if (bError) {
           printf("[%s] FATAL: %s/%s args are:\n COR_NAVG (Number, int32)\n DRX_TUNING_MASK (mask, uint64)\n COR_GAIN {0..15}\n sub_slot {0..99}",ME,dest,cmd);
           return;
           }
         //printf("[%s] %hu %u %u\n",ME,i2u1,i4u1,i4u2); return;
         i4s.i = i4s1; c.data[ 0]=i4s.b[3]; c.data[ 1]=i4s.b[2]; c.data[ 2]=i4s.b[1]; c.data[ 3]=i4s.b[0];
         i8u.i = i8u1; c.data[ 4]=i8u.b[7]; c.data[ 5]=i8u.b[6]; c.data[ 6]=i8u.b[5]; c.data[ 7]=i8u.b[4];
                       c.data[ 8]=i8u.b[3]; c.data[ 9]=i8u.b[2]; c.data[10]=i8u.b[1]; c.data[11]=i8u.b[0]; 
         i2u.i = i2u2; c.data[12]=i2u.b[1]; c.data[13]=i2u.b[0]; 
         i2u.i = i2u3;                      c.data[14]=i2u.b[0]; 
         c.datalen=15;
         break;
         
       case LWA_CMD_CLK:
         //float32 CLK_SET_TIME;
         bError=0;
         if (narg>3) { sscanf(argv[3],"%u",&i4u1); } else {bError=1;}
         if (bError) {
           printf("[%s] FATAL: %s/%s arg is CLK_SET_TIME (uint32)\n",ME,dest,cmd);
           return;
           }
         i4u.i = i4u1; c.data[0]=i4u.b[3]; c.data[1]=i4u.b[2]; c.data[2]=i4u.b[1]; c.data[3]=i4u.b[0];
         c.datalen=4;
         break;

       case LWA_CMD_INI:
         break;
         
       case LWA_CMD_STP:
         //string STP;
         bError=0;
         if (narg>3) {
            bError=1;
            bError &= strncmp(argv[3], "TBF",  3);
            bError &= strncmp(argv[3], "BEAM", 4);
            bError &= strncmp(argv[3], "COR",  3);
         } else {bError=1;}
         if (bError) {
           printf("[%s] FATAL: %s/%s arg is one of TBN|TBF|BEAM#|COR (string)\n",ME,dest,cmd);
           return;
           }
         c.datalen=-1;
         break;
 
       default:
         printf("[%s] FATAL: cmd <%s> not recognized as valid for ADP\n",ME,cmd);
         return;
         break;

       } /* switch (c.cid) */

    if (c.datalen > -1) { 
     char hex[256];
     LWA_raw2hex( c.data, hex, c.datalen );      
     printf("[%s] Outbound DATA field is: 0x%s (raw binary)\n",ME,hex);  
    }

  } /* if (c.sid==LWA_SID_NDP) */
#elif defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP
  /* For ADP, c.data is raw binary, assembled from command line parameters. */
  /* the parameters are command-dependent */
  if (c.sid==LWA_SID_ADP) {
    
    switch (c.cid) {

       case LWA_CMD_PNG:
       case LWA_CMD_RPT:
       case LWA_CMD_SHT:
         break;
        
       case LWA_CMD_TBF:
         // DATA field structure:
         // uint8 TBF_BITS;
         // sint32 TBF_TRIG_TIME; 
         // sint32 TBF_SAMPLES;
         // uint64 DRX_TUNING_MASK
         bError=0;
         if (narg>3) { sscanf(argv[3],"%hu",&i2u1); } else {bError=1;}
         if (narg>4) { sscanf(argv[4],"%i", &i4s1); } else {bError=1;}
         if (narg>5) { sscanf(argv[5],"%i", &i4s2); } else {bError=1;}
         if (narg>6) { sscanf(argv[6],"%lu",&i8u1); } else {bError=1;}
         if (bError) {
           printf("[%s] FATAL: %s/%s args are:\n TBF_BITS (number, uint8)\nTBF_TRIG_TIME (samples, int32)\n TBF_SAMPLES (samples, int32)\n DRX_TUNING_MASK (mask, uint64)\n",ME,dest,cmd);
           exit(EXIT_FAILURE);
           }
         //printf("[%s] %hu %u %u\n",ME,i2u1,i4u1,i4u2); exit(EXIT_SUCCESS);
         i2u.i = i2u1;                      c.data[ 0]=i2u.b[0]; 
         i4s.i = i4s1; c.data[ 1]=i4s.b[3]; c.data[ 2]=i4s.b[2]; c.data[ 3]=i4s.b[1]; c.data[ 4]=i4s.b[0];
         i4s.i = i4s2; c.data[ 5]=i4s.b[3]; c.data[ 6]=i4s.b[2]; c.data[ 7]=i4s.b[1]; c.data[ 8]=i4s.b[0];
         i8u.i = i8u1; c.data[ 9]=i8u.b[7]; c.data[10]=i8u.b[6]; c.data[11]=i8u.b[5]; c.data[12]=i8u.b[4];
                       c.data[13]=i8u.b[3]; c.data[14]=i8u.b[2]; c.data[15]=i8u.b[1]; c.data[16]=i8u.b[0];
         c.datalen=17;
         break;

       case LWA_CMD_TBN:
         // DATA field structure:
         // float32 TBN_FREQ;
         // uint16 TBN_BW;
         // uint16 TBN_GAIN;
         // uint8 sub_slot;
         bError=0;
         if (narg>3) { sscanf(argv[3],"%f", &f41 ); } else {bError=1;}
         if (narg>4) { sscanf(argv[4],"%hu",&i2u1); } else {bError=1;}
         if (narg>5) { sscanf(argv[5],"%hu",&i2u2); } else {bError=1;}
         if (bError) {
           printf("[%s] FATAL: %s/%s args are:\n TBN_FREQ (Hz, float32)\n TBN_BW {5..11}\n TBN_GAIN {0..15}",ME,dest,cmd);
           exit(EXIT_FAILURE);
           }
         //printf("[%s] %hu %u %u\n",ME,i2u1,i4u1,i4u2); exit(EXIT_SUCCESS);
         f4.f  = f41;  c.data[0]= f4.b[3]; c.data[1]= f4.b[2]; c.data[2]= f4.b[1]; c.data[3]= f4.b[0];
         i2u.i = i2u1; c.data[4]=i2u.b[1]; c.data[5]=i2u.b[0]; 
         i2u.i = i2u2; c.data[6]=i2u.b[1]; c.data[7]=i2u.b[0]; 
         c.datalen=8;
         break;

       case LWA_CMD_COR:
         // DATA field structure:
         // int32 COR_NAVG;
         // uint64  DRX_TUNING_MASK
         // int16 COR_GAIN
         // uint8 sub_slot
         bError=0;
         if (narg>3) { sscanf(argv[3],"%i", &i4s1); } else {bError=1;}
         if (narg>4) { sscanf(argv[4],"%lu",&i8u1); } else {bError=1;}
         if (narg>5) { sscanf(argv[5],"%hu",&i2u2); } else {bError=1;}
         if (narg>6) { sscanf(argv[6],"%hu",&i2u3); } else {bError=1;}
         if (bError) {
           printf("[%s] FATAL: %s/%s args are:\n COR_NAVG (Number, int32)\n DRX_TUNING_MASK (mask, uint64)\n COR_GAIN {0..15}\n sub_slot {0..99}",ME,dest,cmd);
           exit(EXIT_FAILURE);
           }
         //printf("[%s] %hu %u %u\n",ME,i2u1,i4u1,i4u2); exit(EXIT_SUCCESS);
         i4s.i = i4s1; c.data[ 0]=i4s.b[3]; c.data[ 1]=i4s.b[2]; c.data[ 2]=i4s.b[1]; c.data[ 3]=i4s.b[0];
         i8u.i = i8u1; c.data[ 4]=i8u.b[7]; c.data[ 5]=i8u.b[6]; c.data[ 6]=i8u.b[5]; c.data[ 7]=i8u.b[4];
                       c.data[ 8]=i8u.b[3]; c.data[ 9]=i8u.b[2]; c.data[10]=i8u.b[1]; c.data[11]=i8u.b[0]; 
         i2u.i = i2u2; c.data[12]=i2u.b[1]; c.data[13]=i2u.b[0]; 
         i2u.i = i2u3;                      c.data[14]=i2u.b[0]; 
         c.datalen=15;
         break;
         
       case LWA_CMD_CLK:
         //float32 CLK_SET_TIME;
         bError=0;
         if (narg>3) { sscanf(argv[3],"%u",&i4u1); } else {bError=1;}
         if (bError) {
           printf("[%s] FATAL: %s/%s arg is CLK_SET_TIME (uint32)\n",ME,dest,cmd);
           exit(EXIT_FAILURE);
           }
         i4u.i = i4u1; c.data[0]=i4u.b[3]; c.data[1]=i4u.b[2]; c.data[2]=i4u.b[1]; c.data[3]=i4u.b[0];
         c.datalen=4;
         break;

       case LWA_CMD_INI:
         break;
         
       case LWA_CMD_STP:
         //string STP;
         bError=0;
         if (narg>3) {
            bError=1;
            bError &= strncmp(argv[3], "TBN",  3);
            bError &= strncmp(argv[3], "TBF",  3);
            bError &= strncmp(argv[3], "BEAM", 4);
            bError &= strncmp(argv[3], "COR",  3);
         } else {bError=1;}
         if (bError) {
           printf("[%s] FATAL: %s/%s arg is one of TBN|TBF|BEAM#|COR (string)\n",ME,dest,cmd);
           exit(EXIT_FAILURE);
           }
         c.datalen=-1;
         break;
 
       default:
         printf("[%s] FATAL: cmd <%s> not recognized as valid for ADP\n",ME,cmd);
         exit(EXIT_FAILURE);
         break;

       } /* switch (c.cid) */

    if (c.datalen > -1) { 
     char hex[256];
     LWA_raw2hex( c.data, hex, c.datalen );      
     printf("[%s] Outbound DATA field is: 0x%s (raw binary)\n",ME,hex);  
    }

    } /* if (c.sid==LWA_SID_ADP) */
#else
  /* For DP, c.data is raw binary, assembled from command line parameters. */
  /* the parameters are command-dependent */
  if (c.sid==LWA_SID_DP_) {
    
    switch (c.cid) {

       case LWA_CMD_PNG:
       case LWA_CMD_RPT:
       case LWA_CMD_SHT:
         break;
        
       case LWA_CMD_TBW:
         // DATA field structure:
         // uint8 TBW_BITS;
         // uint32 TBW_TRIG_TIME; 
         // uint32 TBW_SAMPLES;
         bError=0;
         if (narg>3) { sscanf(argv[3],"%hu",&i2u1); } else {bError=1;}
         if (narg>4) { sscanf(argv[4],"%u", &i4u1); } else {bError=1;}
         if (narg>5) { sscanf(argv[5],"%u", &i4u2); } else {bError=1;}
         if (bError) {
           printf("[%s] FATAL: %s/%s args are:\n TBW_BITS {0|1}\n TBW_TRIG_TIME (samples, uint32)\n TBW_SAMPLES (samples, uint32)\n",ME,dest,cmd);
           exit(EXIT_FAILURE);
           }
         //printf("[%s] %hu %u %u\n",ME,i2u1,i4u1,i4u2); exit(EXIT_SUCCESS);
         i2u.i = i2u1;                     c.data[0]=i2u.b[0]; 
         i4u.i = i4u1; c.data[1]=i4u.b[3]; c.data[2]=i4u.b[2]; c.data[3]=i4u.b[1]; c.data[4]=i4u.b[0];
         i4u.i = i4u2; c.data[5]=i4u.b[3]; c.data[6]=i4u.b[2]; c.data[7]=i4u.b[1]; c.data[8]=i4u.b[0];
         c.datalen=9;
         break;

       case LWA_CMD_TBN:
         // DATA field structure:
         // float32 TBN_FREQ;
         // uint16 TBN_BW;
         // uint16 TBN_GAIN;
         // uint8 sub_slot;
         bError=0;
         if (narg>3) { sscanf(argv[3],"%f", &f41 ); } else {bError=1;}
         if (narg>4) { sscanf(argv[4],"%hu",&i2u1); } else {bError=1;}
         if (narg>4) { sscanf(argv[5],"%hu",&i2u2); } else {bError=1;}
         if (bError) {
           printf("[%s] FATAL: %s/%s args are:\n TBN_FREQ (Hz, float32)\n TBN_BW {1..7}\n TBN_GAIN {0..15}\n sub_slot {0..99}",ME,dest,cmd);
           exit(EXIT_FAILURE);
           }
         //printf("[%s] %hu %u %u\n",ME,i2u1,i4u1,i4u2); exit(EXIT_SUCCESS);
         f4.f  = f41;  c.data[0]= f4.b[3]; c.data[1]= f4.b[2]; c.data[2]= f4.b[1]; c.data[3]= f4.b[0];
         i2u.i = i2u1; c.data[4]=i2u.b[1]; c.data[5]=i2u.b[0]; 
         i2u.i = i2u2; c.data[6]=i2u.b[1]; c.data[7]=i2u.b[0]; 
         i2u.i = i2u3;                     c.data[8]=i2u.b[0]; 
         c.datalen=9;
         break;

       case LWA_CMD_CLK:
         //float32 CLK_SET_TIME;
         bError=0;
         if (narg>3) { sscanf(argv[3],"%u",&i4u1); } else {bError=1;}
         if (bError) {
           printf("[%s] FATAL: %s/%s arg is CLK_SET_TIME (uint32)\n",ME,dest,cmd);
           exit(EXIT_FAILURE);
           }
         i4u.i = i4u1; c.data[0]=i4u.b[3]; c.data[1]=i4u.b[2]; c.data[2]=i4u.b[1]; c.data[3]=i4u.b[0];
         c.datalen=4;
         break;

       case LWA_CMD_INI:
         break;
  
      case LWA_CMD_STP:
         //string STP;
         bError=0;
         if (narg>3) {
            bError=1;
            bError &= strncmp(argv[3], "TBN",  3);
            bError &= strncmp(argv[3], "TBW",  3);
            bError &= strncmp(argv[3], "BEAM", 4);
         } else {bError=1;}
         if (bError) {
           printf("[%s] FATAL: %s/%s arg is one of TBN|TBW|BEAM# (string)\n",ME,dest,cmd);
           exit(EXIT_FAILURE);
           }
         c.datalen=-1;
         break;
           
       default:
         printf("[%s] FATAL: cmd <%s> not recognized as valid for DP\n",ME,cmd);
         exit(EXIT_FAILURE);
         break;

       } /* switch (c.cid) */

    if (c.datalen > -1) { 
     char hex[256];
     LWA_raw2hex( c.data, hex, c.datalen );      
     printf("[%s] Outbound DATA field is: 0x%s (raw binary)\n",ME,hex);  
    }

    } /* if (c.sid==LWA_SID_DP_) */
#endif

  /* create socket */
  sockfd = socket(
                  AF_INET,     /* domain; network sockets */
                  SOCK_STREAM, /* type (TCP-like) */
                  0);          /* protocol (normally 0) */
    
  /* network sockets */
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(LWA_IP_MSE);
  address.sin_port = htons(LWA_PORT_MSE);

  /* connect socket to server's socket */
  result = connect( sockfd, (struct sockaddr *) &address, sizeof(address));
  if (result==-1) {
    perror("msei");
    exit(EXIT_FAILURE);
    }

  write(sockfd, &c, sizeof(struct LWA_cmd_struct));
  read(sockfd,&c,sizeof(struct LWA_cmd_struct));
  //printf("saw %d %d %d\n",c.sid,c.cid,c.ref);
  printf("[%s] ref=%ld, bAccept=%d, eSummary=%d, data=<%s>\n",ME,c.ref,c.bAccept,c.eSummary,c.data);

  close(sockfd); 
  exit(EXIT_SUCCESS);

  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// msei.c: J. Dowell, UNM, 2022 May 2
//   .1: Added support for MCS-NDP.  This is similar to ADP.
// msei.c: J. Dowell, UNM, 2015 Sep 1
//   .1: Added support for sending STP to DP/ADP
// msei.c: J. Dowell, UNM, 2015 Aug 10
//   .1: Added support for MCS-ADP.  This is similar to DP.
// msei.c: S.W. Ellingson, Virginia Tech, 2010 Jun 08
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
