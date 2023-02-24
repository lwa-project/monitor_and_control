// medrange.c: J. Dowell, UNM, 2021 Jan 20
// ---
// COMPILE: gcc -o medrange medrange.c -I../common -lm
// ---
// COMMAND LINE: see help message (in code below)
// ---
// REQUIRES: 
//   mcs.h via me.h
// ---
// Generates the minimum and maximum delays a station will use
// needs to see state/ssmif.dat
// See end of this file for history.

#include "me.h"
#include <math.h>
#include <stdlib.h>

#define FS (196.0e+6)      /* [samples/s] sample rate */
#define DTR 0.017453292520 /* pi/180 */
#define FLAG_VAL (1e+20)

#if defined(USE_NDP) && USE_NDP
#  if defined(NPD_IS_FULL_STATION) && NDP_IS_FULL_STATION
#    define MAX_DP_CH 512      /* number of SNAP channel inputs (per NDP ICD) */
#  else
#    define MAX_DP_CH 128      /* number of SNAP channel inputs (per NDP ICD) */
#  endif
#elif defined(USE_ADP) && USE_ADP
#  define MAX_DP_CH 512      /* number of ROACH channel inputs (per ADP ICD) */
#else
#  define MAX_DP_CH 520      /* number of DP1 channel inputs (per DP ICD) */
#endif

#define CHK_RES_DEG 1.0

/*==============================================================*/
/*=== main() ===================================================*/
/*==============================================================*/

int main ( int narg, char *argv[] ) {

  char sSDir[256];
  float fmhz=74.0;
  float alt, az;

  char filename[512];
  FILE *fp;
  char cmd[512];

  static struct ssmif_struct s; // when this code was tpsdm, had problems if this wasn't declared static
  //struct sdm_struct sdm;
  //struct sc_struct sc;

  int m,p;
  int id[2*ME_MAX_NSTD]; /* reverse lookup (DP input channel index given antenna index); simplifies work later */

  int iAlt, nAlt;
  int iAz,  nAz;
  float fAlt;
  float fAz;

  float c = (2.99792458e+8); /* [m/s] speed of light in free space */

  int i,k;
  int ia;
  float l;
  float vf;
  float dd;

  float tcmin,tcmax;

  float fTheta;
  float fPhi;

  float  px[MAX_DP_CH]; /* antenna position x [m] for indicated DP channel (numbered here as 0..519 as opposed to 1..520) */
  float  py[MAX_DP_CH]; /* antenna position y [m] for indicated DP channel (numbered here as 0..519 as opposed to 1..520) */
  float  pz[MAX_DP_CH]; /* antenna position z [m] for indicated DP channel (numbered here as 0..519 as opposed to 1..520) */
  double dg[MAX_DP_CH]; /* geometrical delay [s] for indicated DP channel (numbered here as 0..519 as opposed to 1..520) */ 
  double dc[MAX_DP_CH]; /* cable delay [s] for indicated DP channel (numbered here as 0..519 as opposed to 1..520) */
  double d[MAX_DP_CH];  /* total delay [s] for indicated DP channel (numbered here as 0..519 as opposed to 1..520) */

  double dmin, dmax, drng;
  int bFlag;

  unsigned short int ddc;
  unsigned short int ddf;
  unsigned short int ddm[MAX_DP_CH];

  int bDone=0;
  FILE *fpl;

  /* parse command line */
  if (narg<2) {
    printf("\n");
    printf("syntax: $ ./medrange <sdir> [<fmhz>]\n");
    printf("  <sdir>: relative path (no trailing slash) to ssmif.dat\n");
    printf("  <fmhz>: center frequency in MHz, used to compute dispersive component of cable delay, defaults to 74\n");
    printf("example: '$ ./medrange state 74'\n");
    printf("\n");
    exit(EXIT_FAILURE);
    }

  if (narg>1) {
      sprintf(sSDir,"%s",argv[1]);
    } else {
      printf("[%d/%d] FATAL: <sdir> not specified\n",ME_DRANGE,getpid());
      exit(EXIT_FAILURE);
    }
  //printf("input: <sdir>='%s'\n",sSDir);

  if (narg>2) {
      sscanf(argv[2],"%f",&fmhz);
    }

  printf("input <sdir>='%s'\n",sSDir);
  printf("input <fmhz>=%f [MHz]\n",fmhz);

  /* Get SSMIF */
  sprintf(filename,"%s/ssmif.dat",sSDir);
  if ((fp=fopen(filename,"rb"))==NULL) {
    printf("[%d/%d] FATAL: Can't open '%s'\n",ME_DRANGE,getpid(),filename);
    exit(EXIT_FAILURE);
    }
  fread(&s,sizeof(struct ssmif_struct),1,fp);
  fclose(fp);
  //exit(EXIT_SUCCESS);

  /* cleanup */
  sprintf(cmd,"rm -rf %s/mindelay.txt",sSDir);
  system(cmd);

  ///* assemble information about analog signal mapping */
  //eErr = me_sc_MakeASM( s, &sc );
  //if (eErr>0) {
  //  printf("[%d/%d] FATAL: me_sc_MakeASM() failed with error %d\n",MT_TPSDM,getpid(),eErr);
  //  exit(EXIT_FAILURE);
  //  }
  ///* assemble information about digital signal mapping */
  //eErr = me_sc_MakeDSM( s, &sc ); 
  //if (eErr>0) {
  //  printf("[%d/%d] FATAL: me_sc_MakeDSM() failed with error %d\n",MT_TPSDM,getpid(),eErr);
  //  exit(EXIT_FAILURE);
  //  }

  /*********************************************/
  /* Figure out px[],py[],pz[] *****************/
  /*********************************************/
  /* note: these are indexed 0..519, per DP ICD channel ordering (0..511 for ADP) */

  /* flag all antenna locations to begin, so we can detect if one has been missed */
  for ( i=0; i<MAX_DP_CH; i++ ) { /* iterating by DP1-input index (per DP ICD) */
    px[i] = FLAG_VAL; 
    py[i] = FLAG_VAL; 
    pz[i] = FLAG_VAL; 
    }   
  for ( i=0; i<2*ME_MAX_NSTD; i++) {
    id[i] = -1;
    }

#if defined(USE_NDP) && USE_NDP
  /* figure out antenna positions, indexed by NDP channel */
  for (i=0;i<s.nSnap;i++) { 
    for (k=0;k<s.nSnapCh;k++) { 

      if (s.iSnapAnt[i][k]!=0) { /* otherwise this Snap input is not connected to an antenna */      
        ia = s.iSnapAnt[i][k] - 1;
        px[i*s.nSnapCh+k] = s.fStdLx[ s.iAntStd[ia] - 1 ];
        py[i*s.nSnapCh+k] = s.fStdLy[ s.iAntStd[ia] - 1 ];
        pz[i*s.nSnapCh+k] = s.fStdLz[ s.iAntStd[ia] - 1 ];
        id[ia] = i*s.nSnapCh+k; /* reverse lookup (NDP input channel index given antenna index); simplifies work later */
        }

      //printf("%d %d | %d | %d %d | %f\n",i,k,i*s.nDP1Ch+k,ia+1,s.iAntStd[ia],px[i*s.nDP1Ch+k]);

      } /* for k */
    } /* for i */
#elif defined(USE_ADP) && USE_ADP
  /* figure out antenna positions, indexed by ADP channel */
  for (i=0;i<s.nRoach;i++) { 
    for (k=0;k<s.nRoachCh;k++) { 

      if (s.iRoachAnt[i][k]!=0) { /* otherwise this Roach input is not connected to an antenna */      
        ia = s.iRoachAnt[i][k] - 1;
        px[i*s.nRoachCh+k] = s.fStdLx[ s.iAntStd[ia] - 1 ];
        py[i*s.nRoachCh+k] = s.fStdLy[ s.iAntStd[ia] - 1 ];
        pz[i*s.nRoachCh+k] = s.fStdLz[ s.iAntStd[ia] - 1 ];
        id[ia] = i*s.nRoachCh+k; /* reverse lookup (ADP input channel index given antenna index); simplifies work later */
        }

      //printf("%d %d | %d | %d %d | %f\n",i,k,i*s.nDP1Ch+k,ia+1,s.iAntStd[ia],px[i*s.nDP1Ch+k]);

      } /* for k */
    } /* for i */
#else
  /* figure out antenna positions, indexed by DP channel */
  for (i=0;i<s.nDP1;i++) { 
    for (k=0;k<s.nDP1Ch;k++) { 

      if (s.iDP1Ant[i][k]!=0) { /* otherwise this DP1 input is not connected to an antenna */      
        ia = s.iDP1Ant[i][k] - 1;
        px[i*s.nDP1Ch+k] = s.fStdLx[ s.iAntStd[ia] - 1 ];
        py[i*s.nDP1Ch+k] = s.fStdLy[ s.iAntStd[ia] - 1 ];
        pz[i*s.nDP1Ch+k] = s.fStdLz[ s.iAntStd[ia] - 1 ];
        id[ia] = i*s.nDP1Ch+k; /* reverse lookup (DP input channel index given antenna index); simplifies work later */
        }

      //printf("%d %d | %d | %d %d | %f\n",i,k,i*s.nDP1Ch+k,ia+1,s.iAntStd[ia],px[i*s.nDP1Ch+k]);

      } /* for k */
    } /* for i */
#endif

  /* check for un-determined stand positions */
  for ( i=0; i<MAX_DP_CH; i++ ) { /* iterating by DP-input index */
    if (px[i]>(FLAG_VAL/2.0)) { printf("[%d/%d] px[%d] not computed\n",ME_DRANGE,getpid(),i+1); }
    if (py[i]>(FLAG_VAL/2.0)) { printf("[%d/%d] py[%d] not computed\n",ME_DRANGE,getpid(),i+1); }
    if (pz[i]>(FLAG_VAL/2.0)) { printf("[%d/%d] pz[%d] not computed\n",ME_DRANGE,getpid(),i+1); }
    }
  //printf("[%d/%d] Will continue using only DP input channels 1..%d\n",ME_DRANGE,getpid(),imax);

  /* diagnostic readback */  
  //for ( i=0; i<ME_MAX_NDP1*ME_MAX_NDP1CH; i++ ) { /* iterating by DP-input index */
  //  m = i/s.nDP1Ch;
  //  p = i%s.nDP1Ch;
  //  printf("i=%3d | p: %7.2f %7.2f %7.2f | s.iDP1Ant[%2d][%2d]=%3d, s.iAntStd[]=%3d\n",i,px[i],py[i],pz[i],m,p,
  //    s.iDP1Ant[m][p],
  //    s.iAntStd[s.iDP1Ant[m][p]-1]);
  //  }   
  //exit(EXIT_SUCCESS);

  /***********************************/
  /* Figure out dc[] *****************/
  /***********************************/
  /* note: these are indexed 0..519, per DP ICD channel ordering */

  /* flag all delay numbers to begin, so we can detect if one has been missed */
  for ( i=0; i<MAX_DP_CH; i++ ) { /* iterating by DP1-input index (per DP ICD) */
    dc[i] = FLAG_VAL; 
    } 

  /* figure out cable delays at this frequency */

  for ( i=0; i<s.nRPD; i++ ) { /* iterating by RPD index */

    /* which antenna is this? --> ia */
    if (s.iRPDAnt[i]<0) {
        ia = -s.iRPDAnt[i]; /* negative values used to indicate input-only-connected cables */
      } else {
        ia = s.iRPDAnt[i]; /* the associated antenna index, minus 1 to make it zero-based */
                           /* trapping for special case of s.iRPDAnt[i]==0 below */
      }
    ia--; /* converting from 1,2,.. to 0,1,... */

    if (s.iRPDAnt[i]!=0) { /* otherwise this cable is unknown */
      if (id[ia]>-1) { /* otherwise we don't know what DP channel input this cable is connected to */
        l  = s.fRPDLeng[i] * s.fRPDStr[i]; /* length [m] is (purchased length [m]) * (coefficient of stretching) */
        vf = s.fRPDVF[i]/100.0;            /* velocity factor [unitless] */
        dd = s.fRPDDD[i]*(1.0e-9);         /* dispersive delay [s] for 100 m of cable at 10 MHz */
        dc[id[ia]] = (l/(c*vf)) + (dd*(l/100.0)/sqrt(fmhz/10.0)); /* M.170 eq. 2 */ 
        }
      }

    } /* for i */

  /* check for un-determined cables */
  for ( i=0; i<MAX_DP_CH; i++ ) { /* iterating by DP-input index */
    if (dc[i]>(FLAG_VAL/2.0)) { printf("[%d/%d] dc[%d] not computed\n",ME_DRANGE,getpid(),i+1); }
    }
  //printf("[%d/%d] Will continue using only DP input channels 1..%d\n",ME_DRANGE,getpid(),imax);

  /* diagnostic readback */  
  //for ( i=0; i<ME_MAX_NDP1*ME_MAX_NDP1CH; i++ ) { /* iterating by DP-input index */
  //  m = i/s.nDP1Ch;
  // p = i%s.nDP1Ch;
  //  printf("i=%3d | s.iDP1Ant[%2d][%2d]=%3d, s.iAntStd[]=%3d length=%5.1f | back-calc length=%5.1f\n",i,m,p,
  //    s.iDP1Ant[m][p],
  //    s.iAntStd[s.iDP1Ant[m][p]-1],
  //    s.fRPDLeng[s.iDP1Ant[m][p]-1],
  //    dc[i]*(c*.83)
  //    );
  //  }   
  //exit(EXIT_SUCCESS);

//  float  fRPDLeng[ME_MAX_NRPD];    /* RPD_LENG[] */
//  float  fRPDVF[ME_MAX_NRPD];      /* RPD_VF[] */
//  float  fRPDDD[ME_MAX_NRPD];      /* RPD_DD[] */
//  float  fRPDA0[ME_MAX_NRPD];      /* RPD_A0[] */ 
//  float  fRPDA1[ME_MAX_NRPD];      /* RPD_A1[] */
//  float  fRPDFref[ME_MAX_NRPD];    /* RPD_FREF[] */
//  float  fRPDStr[ME_MAX_NRPD];     /* RPD_STR[] */
//  int    iRPDAnt[ME_MAX_NRPD];     /* RPD_ANT[] */

  /***********************************/
  /* setting up for main loop ********/
  /***********************************/

  /* setting up for looping */
  nAlt = floor( 90.0/CHK_RES_DEG)+1; /* need both 0 deg and 90 deg */
  nAz  = floor(360.0/CHK_RES_DEG);   /* do not need 360 deg (already have 0) */
  //printf("[%d/%d] nAlt = %d, nAz = %d\n",ME_DRANGE,getpid(),nAlt,nAz);
  iAlt = 0; 
  fAlt = iAlt*CHK_RES_DEG; 
  iAz = 0;
  fAz = iAz*CHK_RES_DEG;
  //printf("[%d/%d] Now computing alt [deg] = ",ME_DRANGE,getpid());

  /********************/
  /* main loop ********/
  /********************/

  dmin = +(1e+20);
  dmax = -(1e+20);

  bDone = 0;
  while (!bDone) {

    fTheta = (90.0-fAlt)*DTR;
    fPhi   = (90.0-fAz )*DTR;    

    // get dg[] from {px[],py[],pz[]} and {fTheta,fPhi}
    // Note this is (more correctly) the time of arrival relative to the time of arrival at origin
    for ( i=0; i<MAX_DP_CH; i++ ) { /* iterating by DP1-input index (per DP ICD) */
      //dg[i] = -( px[i]*cos(fTheta)*sin(fPhi) + py[i]*sin(fTheta)*sin(fPhi) + pz[i]*cos(fTheta) )/c;
      dg[i] = -( px[i]*cos(fPhi)*sin(fTheta) + py[i]*sin(fPhi)*sin(fTheta) + pz[i]*cos(fTheta) )/c;

      //printf("%d %8.3f %8.3f %8.3f %e %e\n",i,px[i],py[i],pz[i],dg[i],dc[i]);

      } 

    // get d[] from dc[]+dg[]; also, find minimum delay
    for ( i=0; i<MAX_DP_CH; i++ ) { /* iterating by DP1-input index (per DP ICD) */
      if ( (px[i]>(FLAG_VAL/2.0)) ||
           (py[i]>(FLAG_VAL/2.0)) ||
           (pz[i]>(FLAG_VAL/2.0)) ||
           (dc[i]>(FLAG_VAL/2.0))   ) {
          
          d[i] = FLAG_VAL;

        } else {

          //d[i] = dc[i]+dg[i];
          d[i] = -(dc[i]+dg[i]);
          if (d[i]<dmin) { dmin=d[i]; }
          if (d[i]>dmax) { dmax=d[i]; }

        }

      } 

    /* decide next step */
    iAz++;
    if (iAz>=nAz) { 
      iAz = 0;            
      iAlt++;
      if (iAlt>=nAlt) { bDone = 1; }
      }
    fAz = iAz*CHK_RES_DEG;
    fAlt = iAlt*CHK_RES_DEG;
    //printf("%4.1f ",fAlt); fflush(stdout);
    if ((90.0-fAlt)<=CHK_RES_DEG) { nAz=1; } /* need only one azimuth for the zenith pointing */
    
    } /* while (!bDone) */

  drng = dmax - dmin;  
  printf("[%d/%d] <delay min>=%.3f ns or %.0f samples\n", ME_DRANGE, getpid(), dmin*1e9, floor(dmin*FS));
  printf("[%d/%d] <delay max>=%.3f ns or %.0f samples\n", ME_DRANGE, getpid(), dmax*1e9, ceil(dmax*FS));
  printf("[%d/%d] <delay range>=%.3f ns or %.0f samples\n", ME_DRANGE, getpid(), drng*1e9, ceil(drng*FS));

  int smin = (int) (floor(dmin*FS/10)-1)*10;
  int smax = (int) (ceil(dmax*FS/10)+1)*10;
  printf("[%d/%d] <sample min>=%d\n", ME_DRANGE, getpid(), smin);
  if( (smax-smin) > 1000 ) {
    printf("[%d/%d] FATAL: expected delay range (%d) > 1000\n", ME_DRANGE, getpid(), smax-smin);
    exit(EXIT_FAILURE);
    }

  sprintf(filename,"%s/mindelay.txt",sSDir);
  if ((fp=fopen(filename,"wb"))==NULL) {
    printf("[%d/%d] FATAL: Can't open '%s'\n",ME_DRANGE,getpid(),filename);
    exit(EXIT_FAILURE);
    }
  fprintf(fp, "%i", smin);
  fclose(fp);

  exit(EXIT_SUCCESS);
  } /* main() */



//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// medrange.c: J. Dowell, UNM, 2022 May 3
//   .1: Updated for NDP
// medrange.c: J. Dowell, UNM, 2021 Jan 20
//   .1: Copied from mefsdfg.c


//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
