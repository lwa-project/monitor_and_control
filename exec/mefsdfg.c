// mefsdfg.c: S.W. Ellingson, Virginia Tech, 2012 Jan 24
// ---
// COMPILE: gcc -o mefsdfg mefsdfg.c -I../common -lm
// ---
// COMMAND LINE: see help message (in code below)
// ---
// REQUIRES: 
//   mcs.h via me.h
// ---
// Generates delay files for DP
// needs to see state/ssmif.dat
// See end of this file for history.

#include "me.h"
#include <math.h>

#define FS (196.0e+6)      /* [samples/s] sample rate */
#define DTR 0.017453292520 /* pi/180 */
#define FLAG_VAL (1e+20)
#define MAX_DP_CH 520      /* number of DP1 channel inputs (per DP ICD) */

/*==============================================================*/
/*=== main() ===================================================*/
/*==============================================================*/

int main ( int narg, char *argv[] ) {

  char sSDir[256];
  char sDDir[256];
  float fmhz;
  int inpm;
  float alt, az;
  char listfile[256];  

  char filename[256];
  FILE *fp;
  char cmd[256];

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

  double dmin;
  int bFlag;

  unsigned short int ddc;
  unsigned short int ddf;
  unsigned short int ddm[MAX_DP_CH];

  /* the following used for little- to big-endian conversions */
  union {
    unsigned short int i;
    char b[2];
    } i2u;
  char bb;

  int bDone=0;
  FILE *fpl;

  /* parse command line */
  if (narg<4) {
    printf("\n");
    printf("syntax: $ ./mefsdfg <sdir> <ddir> <fmhz> <inpm> + possible additional args\n");
    printf("  <sdir>: relative path (no trailing slash) to ssmif.dat\n");
    printf("  <ddir>: relative path (no trailing slash) to a directory to deposit files\n");
    printf("          Warning: will delete existing files.\n");
    printf("  <fmhz>: center frequency in MHz, used to compute dispersive component of cable delay\n");
    printf("  <inpm>: = 0 means generate a complete set of pointings (lots of files!)\n");
    printf("          = 1 means next two parameters <alt> <az> are altitude [deg] and azimuth [deg]\n");
    printf("              for a single pointing\n");
    printf("          = 2 means next parameter <listfile> is the name of a file containing pointing\n");
    printf("              directions.  Each line is {alt [deg]} {space} {az [deg]}\n");
    printf("example: '$ ./mefsdfg state outdir 74 0'\n");
    printf("\n");
    return;
    }

  if (narg>1) {
      sprintf(sSDir,"%s",argv[1]);
    } else {
      printf("[%d/%d] FATAL: <sdir> not specified\n",ME_MEFSDFG,getpid());
      return;
    }
  //printf("input: <sdir>='%s'\n",sSDir);

  if (narg>2) {
      sprintf(sDDir,"%s",argv[2]);
    } else {
      printf("[%d/%d] FATAL: <ddir> not specified\n",ME_MEFSDFG,getpid());
      return;
    }
  //printf("input: <ddir>='%s'\n",sDDir);

  if (narg>3) {
      sscanf(argv[3],"%f",&fmhz);
    } else {
      printf("[%d/%d] FATAL: <fmhz> not specified\n",ME_MEFSDFG,getpid());
      return;
    }

  if (narg>4) {
      sscanf(argv[4],"%d",&inpm);
    } else {
      printf("[%d/%d] FATAL: <inpm> not specified\n",ME_MEFSDFG,getpid());
      return;
    }

  if (inpm!=2) {
    printf("input <sdir>='%s'\n",sSDir);
    printf("input <ddir>='%s'\n",sDDir);
    printf("input <fmhz>=%f [MHz]\n",fmhz);
    printf("input <inpm>=%d\n",inpm);
    }

  switch (inpm) {
    case 0: /* all pointings */
      break;
    case 1: /* one pointing */
      if (narg>5) {
          sscanf(argv[5],"%f",&alt);
        } else {
          printf("[%d/%d] FATAL: <alt> not specified\n",ME_MEFSDFG,getpid());
          return;
        }  
      if (narg>6) {
          sscanf(argv[6],"%f",&az);
        } else {
          printf("[%d/%d] FATAL: <az> not specified\n",ME_MEFSDFG,getpid());
          return;
        }    
      printf("input <alt>=%f [deg]\n",alt);
      printf("input <az>=%f [deg]\n",az);
      break;
    case 2: /* a list */
      if (narg>5) {
          sscanf(argv[5],"%s",listfile);
        } else {
          printf("[%d/%d] FATAL: <listfile> not specified\n",ME_MEFSDFG,getpid());
          return;
        }  
      //printf("input <listfile>='%s'\n",listfile);
      break;
    default: /* error */
      printf("[%d/%d] FATAL: <inpm>=%d not valid\n",ME_MEFSDFG,getpid(),inpm);
      return;
      break;
    }

  /* Get SSMIF */
  sprintf(filename,"%s/ssmif.dat",sSDir);
  if ((fp=fopen(filename,"rb"))==NULL) {
    printf("[%d/%d] FATAL: Can't open '%s'\n",ME_MEFSDFG,getpid(),filename);
    return;
    }
  fread(&s,sizeof(struct ssmif_struct),1,fp);
  fclose(fp);
  //return;

  ///* assemble information about analog signal mapping */
  //eErr = me_sc_MakeASM( s, &sc );
  //if (eErr>0) {
  //  printf("[%d/%d] FATAL: me_sc_MakeASM() failed with error %d\n",MT_TPSDM,getpid(),eErr);
  //  return;
  //  }
  ///* assemble information about digital signal mapping */
  //eErr = me_sc_MakeDSM( s, &sc ); 
  //if (eErr>0) {
  //  printf("[%d/%d] FATAL: me_sc_MakeDSM() failed with error %d\n",MT_TPSDM,getpid(),eErr);
  //  return;
  //  }

  /*********************************************/
  /* Figure out px[],py[],pz[] *****************/
  /*********************************************/
  /* note: these are indexed 0..519, per DP ICD channel ordering */

  /* flag all antenna locations to begin, so we can detect if one has been missed */
  for ( i=0; i<MAX_DP_CH; i++ ) { /* iterating by DP1-input index (per DP ICD) */
    px[i] = FLAG_VAL; 
    py[i] = FLAG_VAL; 
    pz[i] = FLAG_VAL; 
    }   
  for ( i=0; i<2*ME_MAX_NSTD; i++) {
    id[i] = -1;
    }

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

  /* check for un-determined stand positions */
  if (inpm!=2) {
    for ( i=0; i<MAX_DP_CH; i++ ) { /* iterating by DP-input index */
      if (px[i]>(FLAG_VAL/2.0)) { printf("[%d/%d] px[%d] not computed\n",ME_MEFSDFG,getpid(),i+1); }
      if (py[i]>(FLAG_VAL/2.0)) { printf("[%d/%d] py[%d] not computed\n",ME_MEFSDFG,getpid(),i+1); }
      if (pz[i]>(FLAG_VAL/2.0)) { printf("[%d/%d] pz[%d] not computed\n",ME_MEFSDFG,getpid(),i+1); }
      }
    } 
  //printf("[%d/%d] Will continue using only DP input channels 1..%d\n",ME_MEFSDFG,getpid(),imax);

  /* diagnostic readback */  
  //for ( i=0; i<ME_MAX_NDP1*ME_MAX_NDP1CH; i++ ) { /* iterating by DP-input index */
  //  m = i/s.nDP1Ch;
  //  p = i%s.nDP1Ch;
  //  printf("i=%3d | p: %7.2f %7.2f %7.2f | s.iDP1Ant[%2d][%2d]=%3d, s.iAntStd[]=%3d\n",i,px[i],py[i],pz[i],m,p,
  //    s.iDP1Ant[m][p],
  //    s.iAntStd[s.iDP1Ant[m][p]-1]);
  //  }   
  //return;

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
  if (inpm!=2) {
    for ( i=0; i<MAX_DP_CH; i++ ) { /* iterating by DP-input index */
      if (dc[i]>(FLAG_VAL/2.0)) { printf("[%d/%d] dc[%d] not computed\n",ME_MEFSDFG,getpid(),i+1); }
      }
    } 
  //printf("[%d/%d] Will continue using only DP input channels 1..%d\n",ME_MEFSDFG,getpid(),imax);

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
  //return;

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

  /* create destination directory */
  sprintf(cmd,"mkdir %s 2> /dev/null",sDDir);
  system(cmd);
  sprintf(cmd,"rm -rf %s/*",sDDir);
  system(cmd);

  //return;
  
  /* setting up for looping */
  switch (inpm) {
    case 0: /* all pointings */
      nAlt = floor( 90.0/LWA_RES_DEG)+1; /* need both 0 deg and 90 deg */
      nAz  = floor(360.0/LWA_RES_DEG);   /* do not need 360 deg (already have 0) */
      printf("[%d/%d] nAlt = %d, nAz = %d\n",ME_MEFSDFG,getpid(),nAlt,nAz);
      iAlt = 0; 
      fAlt = iAlt*LWA_RES_DEG; 
      iAz = 0;
      fAz = iAz*LWA_RES_DEG;
      printf("[%d/%d] Now computing alt [deg] = ",ME_MEFSDFG,getpid());
      break;
    case 1: /* single pointing */
      fAlt = alt;
      fAz = az;
      break;
    case 2: /*list */
      if (!(fpl = fopen(listfile,"r"))) {
        printf("[%d/%d] FATAL: can't open listfile='%s'\n",ME_MEFSDFG,getpid(),listfile);
        return;
        }
      fscanf(fpl,"%f %f",&fAlt,&fAz); 
      break;
    }

  /********************/
  /* main loop ********/
  /********************/

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
    dmin = +(1e+20);
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

        }

      } 

    // subtract minimum delay from d[]; convert from seconds to sample periods
    for ( i=0; i<MAX_DP_CH; i++ ) { /* iterating by DP1-input index (per DP ICD) */
      //if ((iAlt==0)&&(iAz==0)) printf("%3d | %10.3le + %10.3le = %10.3le | %10.3le\n",i,dg[i],dc[i],d[i],d[i]-dmin); 
      //if ((iAlt+1==nAlt)) printf("%3d | %10.3le + %10.3le = %10.3le | %10.3le\n",i,dg[i],dc[i],d[i],d[i]-dmin); 
      d[i] -= dmin;
      d[i] *= FS;
      }

    // convert d[] to format DP is expecting
    for ( i=0; i<MAX_DP_CH; i++ ) { /* iterating by DP1-input index (per DP ICD) */

      ddc = (unsigned short int)    d[i];
      ddf = (unsigned short int) ( (d[i]-ddc)*16 );

      if ( (px[i]>(FLAG_VAL/2.0)) ||
           (py[i]>(FLAG_VAL/2.0)) ||
           (pz[i]>(FLAG_VAL/2.0)) ||
           (dc[i]>(FLAG_VAL/2.0))   ) { ddc=0; ddf=0; }

      //if ((iAlt==0)&&(iAz==0)) printf("%3d | %10.3le + %10.3le => %8.3lf | %4hu %2hu\n",i,dg[i],dc[i],d[i],ddc,ddf);

      /* merge coarse and fine delays into single parameter */
      ddm[i] = (ddc<<4) + ddf;
        
      /* convert to big-endian */
      i2u.i = ddm[i]; 
      bb=i2u.b[0]; i2u.b[0]=i2u.b[1]; i2u.b[1]=bb;
      ddm[i] = i2u.i; 

      }

    // write it
    sprintf(filename,"%s/%03d_%03d_%04d.df",sDDir,(int)(fmhz*10),(int)(fAlt*10),(int)(fAz*10));
    //printf("'%s'\n",filename);
    fp=fopen(filename,"wb");
    fwrite(ddm,sizeof(ddm),1,fp);
    fclose(fp);

    /* decide next step */
    switch (inpm) {
      case 0: /* all pointings */
        iAz++;
        if (iAz>=nAz) { 
          iAz = 0;            
          iAlt++;
          if (iAlt>=nAlt) { bDone = 1; }
          }
        fAz = iAz*LWA_RES_DEG;
        fAlt = iAlt*LWA_RES_DEG;
        printf("%4.1f ",fAlt); fflush(stdout);
        if ((90.0-fAlt)<=LWA_RES_DEG) { nAz=1; } /* need only one azimuth for the zenith pointing */
        break;
      case 1: /* single pointing */
        bDone = 1;
        break;
      case 2: /* list */
        fscanf(fpl,"%f %f",&fAlt,&fAz); 
        if (feof(fpl)) {
          bDone=1;
          fclose(fpl);
          }
        break;
      default: /* error */
        break;
      } /* switch (inpm) */
    
    } /* while (!bDone) */

  //printf("\n");
  ////printf("%lu %lu\n",sizeof(unsigned short int),sizeof(ddm));

  return 0;
  } /* main() */



//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// mefsdfg.c: S.W. Ellingson, Virginia Tech, 2012 Jan 24
//   .1: Added option to generate outputs for alt/az list file
// mefsdfg.c: S.W. Ellingson, Virginia Tech, 2012 Jan 10
//   .1: Added option to generate output for single pointing
// mefsdfg.c: S.W. Ellingson, Virginia Tech, 2011 April 26
//   .1: Initial version (used tpsdm.c as a starting point)


//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================

