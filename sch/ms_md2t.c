// ms_md2t.c: S.W. Ellingson, Virginia Tech, 2011 Mar 19
// ---
// COMPILE: gcc -o ms_md2t -I/usr/include/gdbm ms_md2t.c -lgdbm
// In Ubuntu, needed to install package libgdbm-dev
// ---
// COMMAND LINE: ms_md2t <subsystem>
//   <subsystem> is the 3-character subsystem designator 
// ---
// REQUIRES: 
//   LWA_MCS.h
//   dbm database representing MIB for indicated subsystem must exist
//     perhaps generated using dat2dbm
// ---
// MCS/Scheduler MIB dbm-file reader
// Note about values: what is shown depends on type_dbm:
//   NUL is shown as "NUL"
//   a#### is shown as is
//   r#### is shown as "@...."
//   i1u, i2u, and i4u are shown as "@", "@@" and "@@@@" respectively
//   f4 is shown as a human-readable float
// See end of this file for history.

#include <stdlib.h> /* needed for exit(); possibly other things */
#include <stdio.h>

#include <string.h>
#include <fcntl.h> /* needed for O_READONLY; perhaps other things */
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

//#include "LWA_MCS.h" 
#include "mcs.h"

#define MAX_LABELS 9999 /* max number of MIB entries supported */

#define MY_NAME "ms_md2t (v.20191030.1)"
#define ME "12" 

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

  FILE* fid_dat; 
  char dat_filename[256];
  int nlabels=0;
  char label0[MAX_LABELS][MIB_LABEL_FIELD_LENGTH]; 

  char line_type[ 2];   
  char index[MIB_INDEX_FIELD_LENGTH];     
  char label[MIB_LABEL_FIELD_LENGTH];     
  char val[256];
  char type_dbm[6];
  char type_icd[6];

  int k;
  char key[MIB_LABEL_FIELD_LENGTH];

  char display[33];

  int ret;
  char cmd_line[768];

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

  /*======================================*/
  /*=== Initialize: Command line stuff ===*/
  /*======================================*/
    
  /* First, announce thyself */
  //printf("[%s/%d] I am %s \n",ME,getpid(),MY_NAME);

  /* Process command line arguments */
  if (narg>1) { 
      //printf("[%s/%d] %s specified\n",ME,getpid(),argv[1]);
      ret = snprintf(dbm_filename,sizeof(dbm_filename),"%s.gdb",argv[1]);
      if (ret >= sizeof(dbm_filename)) {
        printf("[%s/%d] FATAL: subsystem is too long\n",ME,getpid());
        exit(EXIT_FAILURE);
        }
    } else {
      printf("[%s/%d] FATAL: subsystem not specified\n",ME,getpid());
      exit(EXIT_FAILURE);
    } 

  /*================================================*/
  /*=== Get correctly-ordered list of MIB labels ===*/
  /*================================================*/

  /* The first few MIB entries are added by dat2dbm.c. */
  /* We take care of these first: */
  nlabels=0;
  nlabels++; sprintf(label0[nlabels-1],"MCH_IP_ADDRESS");
  nlabels++; sprintf(label0[nlabels-1],"MCH_TX_PORT");
  nlabels++; sprintf(label0[nlabels-1],"MCH_RX_PORT");

  /* Open dat file that was used to initialize the MIB */
  ret = snprintf(dat_filename,sizeof(dat_filename),"%s_MIB_init.dat",argv[1]);
  if (ret >= sizeof(dat_filename)) {
    printf("[%s/%d] FATAL: Filename is too long\n",ME,getpid());
    exit(EXIT_FAILURE);
    } 
  fid_dat = fopen(dat_filename,"r");
  if (!fid_dat) { 
    printf("[%s/%d] FATAL: Can't read file <%s>\n",ME,getpid(),dat_filename);
    exit(EXIT_FAILURE);
    }  

  /* Extract labels */
  while (!feof(fid_dat)) {
    nlabels++;

    if (nlabels>MAX_LABELS) {
      printf("[%s/%d] FATAL: nlabels > MAX_LABELS\n",ME,getpid()); 
      exit(EXIT_FAILURE); 
      }

    /* read next line */
    fscanf(fid_dat,"%1s %s %s %s %s %s\n",
      line_type,
      index,
      label0[nlabels-1],
      val,
      type_dbm,
      type_icd);

    //printf("[%s/%d] %d <%s>\n",ME,getpid(),nlabels,label0[nlabels-1]);

    } /* while (!feof(fid_dat)) */

  /* Close dat file */
  fclose(fid_dat);

  /* At this point, label0[0..nlabels-1] is an array of labels */
  /* in the correct order that can be used as keys to recover */
  /* the MIB entries in the desired order */

  /*=========================================*/
  /*=== Make a working copy of dbm files ====*/
  /*=========================================*/

  /* This is to reduce the possibility for file contention */
  /* while MCS/Scheduler is running */

  /* make copy of the MIB; same root filename but with "_temp" added */
  sprintf(cmd_line,"cp '%s.gdb' '%s_temp.gdb'",dbm_filename,dbm_filename);
  system(cmd_line);

  /* now use *this* copy of MIB */
  ret = snprintf(dbm_filename,sizeof(dbm_filename),"%s_temp.gdb",argv[1]);
  if (ret >= sizeof(dbm_filename)) {
    printf("[%s/%d] FATAL: Failed to open dbm - filename is too long",ME,getpid());
    exit(EXIT_FAILURE);
    }

  /*======================================*/
  /*=== Initialize: dbm file =============*/
  /*======================================*/

  /* Open dbm file */
  dbm_ptr = gdbm_open(dbm_filename, 0, GDBM_READER, 0, NULL);
  if (!dbm_ptr) {
    printf("[%s/%d] FATAL: Failed to open dbm <%s> - %s\n",ME,getpid(),dbm_filename,gdbm_strerror(gdbm_errno));
    /* delete _temp copy of the MIB */
    unlink(dbm_filename);
    exit(EXIT_FAILURE);
    }

  /* === Read dbm record-by-record, using key list obtained above === */
  for (k=0; k<nlabels; k++) {
    
    memset(key, '\0', sizeof(key));
    strncpy(key,label0[k],MIB_LABEL_FIELD_LENGTH);
    datum_key.dptr   = (void *) key;
    datum_key.dsize  = strlen(key);
    datum_data = gdbm_fetch(dbm_ptr,datum_key);
    if (datum_data.dptr) {
      memcpy( &record, datum_data.dptr, datum_data.dsize );
      free(datum_data.dptr);    // Need to cleanup after a gdbm_fetch
      } else { 
      printf("[%s/%d] LWA_dbm_fetch() failed; label=<%s>\n",ME,getpid(),label0[k]);
      }

    memset( label, '\0', sizeof(label));
    strncpy(label,datum_key.dptr,datum_key.dsize);
    //printf(">%s|",label);
    printf("%-32s ",label);

    /* Decide how to show record.val: This depends on record.type_dbm: */
    if (!strncmp(record.type_dbm,"NUL",3)) { /* if the format is "NUL" (e.g., branch entries)... */
      strcpy(record.val,"NUL");               /* print "NUL" */
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
    if (!strncmp(record.type_dbm,"i1s",3)) {  /* if the format is "i1s" */    
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
        if (!strncmp(record.type_dbm,"i4sr",4)) {  /* if the format is "i4sr" (same as i4s, but big-endian) */
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

    //printf("%d|%s|%s|%s|%s|",
    //  record.eType,
    //  record.index,
    //  record.val,
    //  record.type_dbm,
    //  record.type_icd);
    //printf(" %-1d %-12s %-32s %-6s %-6s ",
    //  record.eType,
    //  record.index,
    //  record.val,
    //  record.type_dbm,
    //  record.type_icd);
    printf("%-1d ", record.eType);
    strncpy(display,record.index,    11); printf("%-12s ",display);
    strncpy(display,record.val,      31); printf("%-32s ",display);
    strncpy(display,record.type_dbm,  6); printf("%-6s ", display); 
    strncpy(display,record.type_icd,  6); printf("%-6s ", display); 

    /* convert, show time of last change */
    tv = record.last_change;
    tm = gmtime(&tv.tv_sec);
    printf("%02d%02d%02d %02d:%02d:%02d\n", (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

    } /* for () */

  /* Close dbm file */
  gdbm_close(dbm_ptr);

  /*======================================*/
  /*=== Clean up =========================*/
  /*======================================*/

  /* delete _temp copy of the MIB */
  unlink(dbm_filename);
  
  //printf("[%s/%d] exit(EXIT_SUCCESS)\n",ME,getpid());
  exit(EXIT_SUCCESS);
  } /* main() */


//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// ms_md2t.c: J. Dowell, UNM, 2019 Oct 30
//   .1 Convert to using normal GDBM for the database
// ms_md2t.c: J. Dowell, UNM, 2019 Oct 29
//   .1 Made the code "type complete"
// ms_md2t.c: J. Dowell, UNM, 2018 Jan 29
//   .1: Cleaned up a few complier warnings
// ms_md2t.c: S.W. Ellingson, Virginia Tech, 2011 Mar 19
//   .1: Discovered/fixed "close()" used in place of "fclose()"; didn't matter previously; 
//       evidently recent changes to mcs.h caused this to be important
// ms_md2t.c: S.W. Ellingson, Virginia Tech, 2010 May 25
//   .1: Branched from ms_mdr.c
// ms_mdr.c: S.W. Ellingson, Virginia Tech, 2009 Aug 16
//   .1: Dealing with unprintable fields -- now integers get printed
// ms_mdr.c: S.W. Ellingson, Virginia Tech, 2009 Aug 15
//   .1: Dealing with unprintable fields 
// ms_mdr.c: S.W. Ellingson, Virginia Tech, 2009 Aug 02
//   .1: Working on formatting (svn rev 23)
// ms_mdr.c: S.W. Ellingson, Virginia Tech, 2009 Jul 26 
//   .1: Very first version, adapted from segments of ms_mcic.c
//   .2: Implementing index/label swap in dbm database (svn rev 10)

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
