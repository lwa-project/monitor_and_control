// ms_mdre.c: S.W. Ellingson, Virginia Tech, 2009 Aug 16
// ---
// COMPILE: gcc -o ms_mdre -I/usr/include/gdbm ms_mdr.c -lgdbm
// In Ubuntu, needed to install package libgdbm-dev
// ---
// COMMAND LINE: ms_mdre <subsystem> <MIB_label>
//   <subsystem> is the 3-character subsystem designator 
//   <MIB_label> is the alphanumeric label identifying the MIB entry 
// ---
// REQUIRES: 
//   LWA_MCS.h
//   dbm database representing MIB for indicated subsystem must exist
//     perhaps generated using dat2dbm
// ---
// MCS/Scheduler MIB dbm-file reader for entries
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
#include <byteswap.h>

//#include "LWA_MCS.h" 
#include "mcs.h"

#define MY_NAME "ms_mdre (v.20191030.1)"
#define ME "9" 

main ( int narg, char *argv[] ) {

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

  /*======================================*/
  /*=== Initialize: Command line stuff ===*/
  /*======================================*/
    
  /* First, announce thyself */
  //printf("[%s/%d] I am %s \n",ME,getpid(),MY_NAME);

  /* Process command line arguments */
  if (narg>1) { 
      //printf("[%s/%d] %s specified\n",ME,getpid(),argv[1]);
      sprintf(dbm_filename,"%s.gdb",argv[1]);
    } else {
      printf("[%s/%d] FATAL: subsystem not specified\n",ME,getpid());
      exit(EXIT_FAILURE);
    } 
  if (narg>2) { 
      //printf("[%s/%d] label <%s> specified\n",ME,getpid(),argv[2]);
      sprintf(label,"%s",argv[2]);
    } else {
      printf("[%s/%d] FATAL: MIB label not specified\n",ME,getpid());
      exit(EXIT_FAILURE);
    } 

  /*======================================*/
  /*=== Initialize: dbm file =============*/
  /*======================================*/

  /* Open dbm file */
  dbm_ptr = gdbm_open(dbm_filename, 0, GDBM_READER, 0, NULL);
  if (!dbm_ptr) {
    printf("[%s/%d] FATAL: Failed to open dbm <%s> - %s\n",ME,getpid(),dbm_filename,gdbm_strerror(gdbm_errno));
    exit(EXIT_FAILURE);
    }

  sprintf(key,"%s",label);
  datum_key.dptr = key;
  datum_key.dsize = strlen(key);
  datum_data = gdbm_fetch(dbm_ptr,datum_key);
  if (datum_data.dptr) {
      memcpy( &record, datum_data.dptr, datum_data.dsize );
      free(datum_data.dptr);    // Need to cleanup after a gdbm_fetch
      //strncpy(ip_address,record.val,15);
    } else {
      printf("[%s/%d] Failed to find label=<%s> in dbm.\n", ME, getpid(),label);
      exit(EXIT_FAILURE);
    }

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
    if (!strncmp(record.type_dbm,"i4ur",4)) {  /* if the format is "i4ur" (same as i4u, but big-endian) */
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

  //printf( "%-s %-s %-s |", record.index, label, record.val );
  printf( "%-s\n", record.val );

  /* convert, show time of last change */
  tv = record.last_change;
  tm = gmtime(&tv.tv_sec);
  printf("%02d%02d%02d %02d:%02d:%02d\n", (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

  /* Close dbm file */
  gdbm_close(dbm_ptr);

  //printf("[%s/%d] exit(EXIT_SUCCESS)\n",ME,getpid());
  exit(EXIT_SUCCESS);
  } /* main() */


//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// ms_mdre.c: J. Dowell, UNM, 2019 Oct 30
//   .1 Convert to using normal GDBM for the database
// ms_mdre.c: J. Dowell, UNM, 2019 Oct 29
//   .1 Made the code "type complete"
// ms_mdre.c: J. Dowell, UNM, 2018 Jan 29
//   .1: Cleaned up a few complier warnings
// ms_mdre.c: J. Dowell, UNM, 2015 Aug 10
//   .1: Added support for signed int and unsigned long int representations
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

