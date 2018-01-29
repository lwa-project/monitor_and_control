// tpsdm.c: J. Dowell, UNM, 2015 Sep 11
// ---
// COMPILE: gcc -o tpsdm tpsdm.c -I../common
// ---
// COMMAND LINE: see help message (in code below)
// ---
// REQUIRES: 
//   mcs.h via mt.h
// ---
// Reads/interprets mbox/sdm.dat
// needs to see mbox/ssmif.dat and mbox/sdm.dat
// See end of this file for history.

#include "mt.h"

#define TPSDM_RPT_NUL  0
#define TPSDM_RPT_STA  1
#define TPSDM_RPT_SUM  2
#define TPSDM_RPT_ANT  3
#define TPSDM_RPT_FEE  4
#define TPSDM_RPT_RPD  5
#define TPSDM_RPT_SEP  6
#define TPSDM_RPT_ARX  7
#ifdef USE_ADP
#define TPSDM_RPT_ROACH  8
#define TPSDM_RPT_SERVER 9
#else
#define TPSDM_RPT_DP1  8
#define TPSDM_RPT_DP2  9
#endif
#define TPSDM_RPT_DR  10
#define TPSDM_RPT_ANA 11
#define TPSDM_RPT_ANS 12
#define TPSDM_RPT_ANP 13
#define TPSDM_RPT_DPP 14
#define TPSDM_RPT_PWR 15

/*==============================================================*/
/*=== main() ===================================================*/
/*==============================================================*/

int main ( int narg, char *argv[] ) {

  char arg[256];

  static struct ssmif_struct s; // seems to be important that this be static (and others not be)
  struct sdm_struct sdm;
  struct sc_struct sc;

  FILE *fp;
  struct tm *tm;
  char time_string[256];
  struct timeval tv;
  int c,i,j,k,m;
  int eRpt = TPSDM_RPT_NUL;
  int bStatic=0;
  char sDir[256];
  char filename[256];
  int eErr;
  char st1[256]; /* temp string variable */
  char st2[256]; /* temp string variable */

  /* parse command line */
  if (narg<2) {
    printf("\n");
    printf("syntax: $ ./tpsdm <dir> <rpt> <static>\n");
    printf("  <dir>: relative path (no trailing slash) to .dat files (typically: 'mbox')\n");
    printf("           will look here for 'ssmif.dat' and 'sdm.dat', as needed\n");
    printf("  <rpt>: Selects from one of the following reports (note: case is significant):\n");
    printf("          'sta': ('station') header of SSMIF + some 'settings'\n");
    printf("          'sum': ('summary') summary status of Level-1 subsystems (<static>=0 only)\n");
    printf("          --- subsystem information: ---\n");
    printf("          'ANT': List status, other info about antennas (<static>=1 only)\n");    
    printf("          'FEE': List status, other info about FEEs\n"); 
    printf("          'RPD': List status, other info about cables\n");
    printf("          'SEP': List status, other info about SEP ports\n");
    printf("          'ARX': List status, other info about ARX channels\n");
#ifdef USE_ADP
    printf("          'ROACH': List status, other info about ROACH channels\n");
    printf("          'SERVER': List status, other infor about SERVER channels\n");
#else
    printf("          'DP1': List status, other info about DP1 channels\n");
    printf("          'DP2': List status, other info about DP2 channels\n");
#endif
    printf("          'DR':  List status, other info about DRs\n");
    printf("          --- other (cross- or multi-subsystem) reports: ---\n");
    printf("          'ana': List FEE power state and ARX filter & atten settings by stand\n");
    printf("          'ans': List status, other info about analog paths by STATUS\n");
    printf("          'anp': List status, other info about analog paths by PATH\n");
    printf("          'dpp': List status, other info about DP output paths by PATH\n");
    printf("          'pwr': Power rack/port to subsystem mapping table (<static>=1 only)\n");
    printf("  <static>: =0 means use SDM (current info from sdm.dat) [default]\n"); 
    printf("            =1 means use only SSMIF (initialization info from ssmif.dat)\n");
    printf("example: '$ ./tpsdm mbox sta 1'\n");
    printf("\n");
    return;
    }

  if (narg>1) {
      sprintf(sDir,"%s",argv[1]);
    } else {
      printf("FATAL: <dir> not specified\n");
      return;
    }
  //printf("input: <dir>='%s'\n",sDir);

  if (narg>2) { 
      sscanf(argv[2],"%s",arg);
      if (strncmp(arg,"sta",3)==0) { eRpt = TPSDM_RPT_STA; }
      if (strncmp(arg,"sum",3)==0) { eRpt = TPSDM_RPT_SUM; bStatic=0; }
      if (strncmp(arg,"ANT",3)==0) { eRpt = TPSDM_RPT_ANT; bStatic=1; }
      if (strncmp(arg,"FEE",3)==0) { eRpt = TPSDM_RPT_FEE; }
      if (strncmp(arg,"RPD",3)==0) { eRpt = TPSDM_RPT_RPD; }
      if (strncmp(arg,"SEP",3)==0) { eRpt = TPSDM_RPT_SEP; }
      if (strncmp(arg,"ARX",3)==0) { eRpt = TPSDM_RPT_ARX; }
#ifdef USE_ADP
      if (strncmp(arg,"ROACH",5)==0)  { eRpt = TPSDM_RPT_ROACH; }
      if (strncmp(arg,"SERVER",6)==0) { eRpt = TPSDM_RPT_SERVER; }
#else
      if (strncmp(arg,"DP1",3)==0) { eRpt = TPSDM_RPT_DP1; }
      if (strncmp(arg,"DP2",3)==0) { eRpt = TPSDM_RPT_DP2; }
#endif
      if (strncmp(arg,"DR", 2)==0) { eRpt = TPSDM_RPT_DR;  }
      if (strncmp(arg,"ana",3)==0) { eRpt = TPSDM_RPT_ANA; }
      if (strncmp(arg,"ans",3)==0) { eRpt = TPSDM_RPT_ANS; }
      if (strncmp(arg,"anp",3)==0) { eRpt = TPSDM_RPT_ANP; }
      if (strncmp(arg,"dpp",3)==0) { eRpt = TPSDM_RPT_DPP; }
      if (strncmp(arg,"pwr",3)==0) { eRpt = TPSDM_RPT_PWR; bStatic=1; }
      if (eRpt==TPSDM_RPT_NUL) {
        printf("FATAL: invalid <rpt>='%s'\n",arg);
        return;
        }
    } else {
      printf("FATAL: <rpt> not specified\n");
      return;
    }

  if (narg>3) {
    sscanf(argv[3],"%d",&bStatic);
    if ( (bStatic<0) || (bStatic>1) ) {
      printf("FATAL: invalid <static>=%d\n",bStatic);
      return;      
      }
    if ( bStatic && (eRpt==TPSDM_RPT_SUM) ) {
      printf("FATAL: <static>=1 not allowed for <rpt>='sum'\n");
      return;   
      }
    if ( (!bStatic) && (eRpt==TPSDM_RPT_ANT) ) {
      printf("FATAL: <static>=0 not allowed for <rpt>='ANT'\n");
      return;   
      }
    if ( (!bStatic) && (eRpt==TPSDM_RPT_ANS) ) {
      printf("FATAL: <static>=0 not allowed for <rpt>='ans'\n");
      return;   
      }
    if ( (!bStatic) && (eRpt==TPSDM_RPT_PWR) ) {
      printf("FATAL: <static>=0 not allowed for <rpt>='pwr'\n");
      return;   
      }
    }

  //printf("input: <rpt>='%s' (%d)\n",arg,eRpt); 
  //printf("input: <static>=%d\n",bStatic); 

  /* Get SSMIF */
  sprintf(filename,"%s/ssmif.dat",sDir);
  if ((fp=fopen(filename,"rb"))==NULL) {
    printf("[%d/%d] FATAL: Can't open '%s'\n",MT_TPSDM,getpid(),filename);
    return;
    }
  fread(&s,sizeof(struct ssmif_struct),1,fp);
  fclose(fp);

  /* assemble information about analog signal mapping */
  eErr = me_sc_MakeASM( s, &sc );
  if (eErr>0) {
    printf("[%d/%d] FATAL: me_sc_MakeASM() failed with error %d\n",MT_TPSDM,getpid(),eErr);
    return;
    }
  /* assemble information about digital signal mapping */
  eErr = me_sc_MakeDSM( s, &sc ); 
  if (eErr>0) {
    printf("[%d/%d] FATAL: me_sc_MakeDSM() failed with error %d\n",MT_TPSDM,getpid(),eErr);
    return;
    }

  /* Get SDM, if needed */
  if (!bStatic) {
    sprintf(filename,"%s/sdm.dat",sDir);
    if ((fp=fopen(filename,"rb"))==NULL) {
      printf("[%d/%d] FATAL: Can't open '%s'\n",MT_TPSDM,getpid(),filename);
      return;
      }
    fread(&sdm,sizeof(struct sdm_struct),1,fp);
    fclose(fp); 
    }

  /* an initial call to avoid the intermittent-bad-result-on-first-call error */
  gettimeofday(&tv,NULL);
  //printf("[%d/%d] --- Checking gettimeofday(&tv,NULL) --------\n",MT_TPTC,getpid());

  switch(eRpt) {

    case TPSDM_RPT_SUM:

      tm = gmtime(&(sdm.station.tv.tv_sec));
      sprintf(time_string,"%02d%02d%02d %02d:%02d:%02d", 
           (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
      printf( "%s Sta: %-7s '%s'\n", time_string, LWA_saysum(sdm.station.summary), sdm.station.info );

      tm = gmtime(&(sdm.shl.tv.tv_sec));
      sprintf(time_string,"%02d%02d%02d %02d:%02d:%02d", 
           (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
      printf( "%s SHL: %-7s '%s'\n", time_string, LWA_saysum(sdm.shl.summary    ), sdm.shl.info     );

      tm = gmtime(&(sdm.asp.tv.tv_sec));
      sprintf(time_string,"%02d%02d%02d %02d:%02d:%02d", 
           (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
      printf( "%s ASP: %-7s '%s'\n", time_string, LWA_saysum(sdm.asp.summary    ), sdm.asp.info     );

      tm = gmtime(&(sdm.dp.tv.tv_sec));
      sprintf(time_string,"%02d%02d%02d %02d:%02d:%02d", 
           (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
      printf( "%s DP_: %-7s '%s'\n", time_string, LWA_saysum(sdm.dp.summary     ), sdm.dp.info      );

      for (i=0;i<s.nDR;i++) {
        tm = gmtime(&(sdm.dr[i].tv.tv_sec));
        sprintf(time_string,"%02d%02d%02d %02d:%02d:%02d", 
             (tm->tm_year)-100, (tm->tm_mon)+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        printf( "%s DP%1d: %-7s '%s'\n", time_string, i+1, LWA_saysum(sdm.dr[i].summary), sdm.dr[i].info );
        }
  
      break;

    case TPSDM_RPT_STA: 
      printf("SSMIF FORMAT_VERSION: %d\n",s.iFormatVersion); 
      printf("STATION_ID: '%s'\n",s.sStationID);
      printf("GEO_N:   %+9.4f deg\n",s.fGeoN);
      printf("GEO_E:   %+9.4f deg\n",s.fGeoE);
      printf("GEO_EL:  %7.2f m\n",s.fGeoEl);
      printf("N_STD:   %d\n",s.nStd);
      printf("N_FEE:   %d\n",s.nFEE);
      printf("N_RPD:   %d\n",s.nRPD);
      printf("N_SEP:   %d\n",s.nSEP);
      printf("N_ARB:   %d\n",s.nARB);
      printf("N_ARBCH: %d\n",s.nARBCH);
#ifdef USE_ADP
      printf("N_ROACH:   %d\n",s.nRoach);
      printf("N_ROACHCH: %d\n",s.nRoachCh);
#else
      printf("N_DP1:   %d\n",s.nDP1);
      printf("N_DP1CH: %d\n",s.nDP1Ch);
#endif
      printf("N_DR:    %d\n",s.nDR);
      printf("MCS_CRA: %d\n",s.eCRA); /* MCS_CRA */  
#ifdef USE_ADP
                    printf("TBF_GAIN: %2hd",  s.settings.tbf_gain);
      if (!bStatic) printf(       " | %2hd",sdm.settings.tbf_gain);
      printf("\n");
#endif
                    printf("TBN_GAIN: %2hd",  s.settings.tbn_gain);
      if (!bStatic) printf(       " | %2hd",sdm.settings.tbn_gain);
      printf("\n");
                    printf("DRX_GAIN: %2hd",  s.settings.drx_gain); 
      if (!bStatic) printf(       " | %2hd",sdm.settings.drx_gain); 
      printf("\n");
                    printf("ASP: MRP=%hd MUP=%hd",  s.settings.mrp_asp,  s.settings.mup_asp);
      if (!bStatic) printf(  " | MRP=%hd MUP=%hd",sdm.settings.mrp_asp,sdm.settings.mup_asp);
      printf("\n");   
                    printf("DP_: MRP=%hd MUP=%hd",  s.settings.mrp_dp,   s.settings.mup_dp );
      if (!bStatic) printf(  " | MRP=%hd MUP=%hd",sdm.settings.mrp_dp, sdm.settings.mup_dp );
      printf("\n");   
                    printf("DR1: MRP=%hd MUP=%hd",  s.settings.mrp_dr1,  s.settings.mup_dr1);
      if (!bStatic) printf(  " | MRP=%hd MUP=%hd",sdm.settings.mrp_dr1,sdm.settings.mup_dr1);
      printf("\n");   
                    printf("DR2: MRP=%hd MUP=%hd",  s.settings.mrp_dr2,  s.settings.mup_dr2);
      if (!bStatic) printf(  " | MRP=%hd MUP=%hd",sdm.settings.mrp_dr2,sdm.settings.mup_dr2);
      printf("\n");   
                    printf("DR3: MRP=%hd MUP=%hd",  s.settings.mrp_dr3,  s.settings.mup_dr3);
      if (!bStatic) printf(  " | MRP=%hd MUP=%hd",sdm.settings.mrp_dr3,sdm.settings.mup_dr3);
      printf("\n");   
                    printf("DR4: MRP=%hd MUP=%hd",  s.settings.mrp_dr4,  s.settings.mup_dr4);
      if (!bStatic) printf(  " | MRP=%hd MUP=%hd",sdm.settings.mrp_dr4,sdm.settings.mup_dr4);
      printf("\n");   
                    printf("DR5: MRP=%hd MUP=%hd",  s.settings.mrp_dr5,  s.settings.mup_dr5);
      if (!bStatic) printf(  " | MRP=%hd MUP=%hd",sdm.settings.mrp_dr5,sdm.settings.mup_dr5);
      printf("\n");   
                    printf("SHL: MRP=%hd MUP=%hd",  s.settings.mrp_shl,  s.settings.mup_shl);
      if (!bStatic) printf(  " | MRP=%hd MUP=%hd",sdm.settings.mrp_shl,sdm.settings.mup_shl);
      printf("\n");   
                    printf("MCS: MRP=%hd MUP=%hd",  s.settings.mrp_mcs,  s.settings.mup_mcs);
      if (!bStatic) printf(  " | MRP=%hd MUP=%hd",sdm.settings.mrp_mcs,sdm.settings.mup_mcs);
      printf("\n");   
      printf("key: (for TBN_GAIN, DRX_GAIN, and MRP/MUP lines)\n"); 
      printf("  static values"); if (!bStatic) printf(" | dynamic values");
      printf("\n");
      break;

    case TPSDM_RPT_ANT: /* ANT */
      for (i=0;i<(2*s.nStd);i++) { 
        printf("%3d %1d | %1d %+6.2f %+6.2f %2d | %3d %+8.2lf %+8.2lf %+8.2lf\n",
                i+1,
                    s.iAntStat[i],
                          s.iAntOrie[i], /* ANT_ORIE[] */
                              s.fAntTheta[i], /* ANT_THETA[] */
                                     s.fAntPhi[i], /* ANT_PHI[] */
                                            s.eAntDesi[i], /* ANT_DESI[] */
                                                  s.iAntStd[i], /* ANT_STD[] */
                                                      s.fStdLx[s.iAntStd[i]-1], /* STD_LX[] */
                                                              s.fStdLy[s.iAntStd[i]-1], /* STD_LY[] */
                                                                      s.fStdLz[s.iAntStd[i]-1]  /* STD_LZ[] */
              ); 
        }
      printf("key:\n");
      printf("  ant#\n");
      printf("  stat\n");
      printf("| ANT_ORIE (0=NS, 1=EW)\n");
      printf("  ANT_THETA [deg]\n");
      printf("  ANT_PHI   [deg]\n");
      printf("  ANT_DESI\n");
      printf("| ANT_STD (stand that this antenna is part of)\n");
      printf("  STD_LX [m]\n");
      printf("  STD_LY [m]\n");
      printf("  STD_LZ [m]\n");
      break;

    case TPSDM_RPT_FEE: /* FEE */
      for (i=0;i<s.nFEE;i++) { 
        printf("%3d %1d",i+1,s.iFEEStat[i]); 
        if (!bStatic) printf(" %1d",sdm.ssss.eFEEStat[i]); 
        printf(" | %3d %3d",s.iFEEAnt1[i],s.iFEEAnt2[i]); /* FEE_ANT1[] */ /* FEE_ANT2[] */
        printf(" | %+5.1f %+5.1f",s.fFEEGai1[i],s.fFEEGai2[i]); /* FEE_GAI1[] */ /* FEE_GAI2[] */
        printf(" | %2d %2d",s.iFEERack[i],s.iFEEPort[i]); /* FEE_RACK[] */ /* FEE_PORT[] */
        printf(" | %2d %s",s.eFEEDesi[i],s.sFEEID[i]); /* FEE_DESI[] */ /* FEE_ID[] */
        printf("\n");
        }
      printf("key:\n");
      printf("  FEE#\n");
      printf("  stat (static)\n");
      if (!bStatic) printf("  stat (dynamic)\n");
      printf("| FEE_ANT1 (ant# connected to input port 1)\n");
      printf("  FEE_ANT2 (ant# connected to input port 2)\n");
      printf("| FEE_GAI1 [dB] (port 1)\n");
      printf("  FEE_GAI2 [dB] (port 2)\n");
      printf("| FEE_RACK (rack in which this FEEs power supply is located)\n");
      printf("  FEE_PORT (port which controls this FEEs power supply)\n");
      printf("| FEE_DESI\n");
      printf("  FEE_ID\n");
      break;

    case TPSDM_RPT_RPD: /* RPD */
      for (i=0;i<s.nRPD;i++) { 
        printf( "%3d %1d",i+1,s.iRPDStat[i]); 
        if (!bStatic) printf(" %1d",sdm.ssss.eRPDStat[i]);
        printf( " | %3d",s.iRPDAnt[i]); /* RPD_ANT[] */
        printf( " | %6.1f %5.1f %4.1f %8.3e %8.3e %5.1f %5.3f",
                    s.fRPDLeng[i], /* RPD_LENG[] */
                          s.fRPDVF[i],      /* RPD_VF[] */
                                s.fRPDDD[i],      /* RPD_DD[] */
                                      s.fRPDA0[i],      /* RPD_A0[] */
                                            s.fRPDA1[i],      /* RPD_A1[] */
                                                  s.fRPDFref[i]/(1.0e+6),    /* RPD_FREF[] */
                                                        s.fRPDStr[i]);     /* RPD_STR[] */
        printf(" | %2d %10s",s.eRPDDesi[i],s.sRPDID[i]); /* RPD_DESI[] */ /* RPD_ID[] */
        printf("\n");
        }
      printf("key:\n");
      printf("  cable#\n");
      printf("  stat (static)\n");
      if (!bStatic) printf("  stat (dynamic)\n");
      printf("| RPD_ANT (ant# associated with this cable)\n");
      printf("| RPD_LENG [m] (length)\n");
      printf("  RPD_VF [%%] (velocity factor)\n");
      printf("  RPD_DD [ns] (dispersive delay at 10 MHz for 100 m)\n");
      printf("  RPD_A0 [1/m] (\alpha_0 at RPD_FREF in the M.170 model)\n");
      printf("  RPD_A1 (not currently used)\n");
      printf("  RPD_FREF [MHz] (ref. freq. for RPD_A0 and RPD_A1)\n");
      printf("  RPD_STR [unitless] (coefficient of stretching)\n");
      printf("| RPD_DESI\n");
      printf("  RPD_ID\n");
      break;

    case TPSDM_RPT_SEP: /* SEP */
      for (i=0;i<s.nSEP;i++) { 
        printf("%3d %1d",i+1,s.iSEPStat[i]); 
        if (!bStatic) printf(" %1d",sdm.ssss.eSEPStat[i]);
        printf(" | %3d | %4.1f %+4.1f | %2d %10s %10s",
                   s.iSEPAnt[i], /* SEP_ANT[] */
                         s.fSEPLeng[i], /* SEP_LENG[] */
                               s.fSEPGain[i], /* SEP_GAIN[] */
                                        s.eSEPDesi[i], /* SEP_DESI[] */
                                            s.sSEPID[i], /* SEP_ID[] */
                                                 s. sSEPCabl[i]); /* SEP_CABL[] */
        printf("\n");
        }
      printf("key:\n");
      printf("  SEP port #\n");
      printf("  stat (static)\n");
      if (!bStatic) printf("  stat (dynamic)\n");
      printf("| SEP_ANT (ant# associated with this SEP port)\n");
      printf("| SEP_LENG [m]\n");
      printf("  SEP_GAIN [dB]\n");
      printf("| SEP_DESI\n");
      printf("  SEP_ID (label on panel)\n");
      printf("  SEP_CABL (label on cable to ASP)\n");
      break;

    case TPSDM_RPT_ARX: /* ARX */
      for (i=0;i<s.nARB;i++) { 
        for (j=0;j<s.nARBCH;j++) {
          printf( "%2d %2d |",i+1,j+1); 
          printf( " %1d ",s.eARBStat[i][j]);
          if (!bStatic) printf(" %1d",sdm.ssss.eARBStat[i][j]); 
          printf(" | %3d ",s.iARBAnt[i][j]); /* ARB_ANT[][] */
          printf(" | %+5.1f ",s.fARBGain[i][j]); /* ARB_GAIN[][] */
          printf(" | %2d %2d",s.iARBRack[i],s.iARBPort[i]); /* ARB_RACK[] */ /* ARB_PORT[] */
          printf(" | %2d %s %2d %10s %10s",
                     s.eARBDesi[i], /* ARB_DESI[] */
                         s.sARBID[i], /* ARB_ID[] */
                              s.iARBSlot[i], /* ARB_SLOT[] */
                                   s.sARBIN[i][j], /* ARB_IN[][] */
                                       s.sARBOUT[i][j] ); /* ARB_OUT[][] */  
          printf("\n");
          } 
        }
      printf("key:\n");
      printf("  ARX board #\n");
      printf("  channel # on this ARX board\n");
      printf("| stat (static)\n");
      if (!bStatic) printf("  stat (dynamic)\n");
      printf("| ARB_ANT (ant# associated with this channel)\n");
      printf("| ARB_GAIN [dB]\n");
      printf("| ARB_RACK (rack in which this channel's power supply is located)\n");
      printf("  ARB_PORT (port which controls this channel's power supply)\n");
      printf("| ARB_DESI\n");
      printf("  ARB_ID\n");
      printf("  ARB_SLOT\n");
      printf("  ARB_IN (label on input connector for this channel on ASP chassis)\n");
      printf("  ARB_OUT (label on output connector for this channel on ASP chassis)\n");
      break;
#ifdef USE_ADP
    case TPSDM_RPT_ROACH:
      /* Roach */
      for (i=0;i<s.nRoach;i++) { 
        for (j=0;j<s.nRoachCh;j++) {
          printf( "%2d %2d |",i+1,j+1); 
          printf( " %1d",s.eRoachStat[i][j]);
          if (!bStatic) printf( " %1d",sdm.ssss.eRoachStat[i][j]); 
          printf( " | %3d",s.iRoachAnt[i][j]); /* DP1_ANT[][] */
          printf( " | %2d %10s %10s",s.eRoachDesi[i],s.sRoachSlot[i],s.sRoachID[i]); /* DP1_DESI[] */ /* DP1_SLOT[] */ /* DP1_ID[] */
          printf( " | %10s %10s",s.sRoachINR[i][j],s.sRoachINC[i][j]); /* DP1_INR[][] */ /* DP1_INC[][] */
          printf("\n");
          } 
        }
      printf("key:\n");
      printf("  ROACH board #\n");
      printf("  channel # on this board\n");
      printf("| stat (static)\n");
      if (!bStatic) printf("  stat (dynamic)\n");
      printf("| ROACH_ANT (ant# associated with this channel)\n");
      printf("| ROACH_DESI\n");
      printf("  ROACH_SLOT\n");
      printf("  ROACH_ID\n");
      printf("| ROACH_INR (label on input connector for this channel on DP rack)\n");
      printf("  ROACH_INC (label on input connector for this channel on DP chassis)\n");
      break;
      
    case TPSDM_RPT_SERVER: /* Server */
      for (i=0;i<s.nServer;i++) { 
        printf( "%2d %1d",i+1,s.eServerStat[i]); 
        if (!bStatic) printf( " %1d",sdm.ssss.eServerStat[i]); 
        printf(" | %10s %10s %2d\n",
                   s.sServerID[i],
                        s.sServerSlot[i],
                             s.eServerDesi[i] ); 
        }
      printf("key:\n");
      printf("  Server #\n");
      printf("  stat (static)\n");
      if (!bStatic) printf("  stat (dynamic)\n");
      printf("| SERVER_ID\n");
      printf("  SERVER_SLOT\n");
      printf("  SERVER_DESI\n");
      break;
#else
    case TPSDM_RPT_DP1:
      /* DP1 */
      for (i=0;i<s.nDP1;i++) { 
        for (j=0;j<s.nDP1Ch;j++) {
          printf( "%2d %2d |",i+1,j+1); 
          printf( " %1d",s.eDP1Stat[i][j]);
          if (!bStatic) printf( " %1d",sdm.ssss.eDP1Stat[i][j]); 
          printf( " | %3d",s.iDP1Ant[i][j]); /* DP1_ANT[][] */
          printf( " | %2d %10s %10s",s.eDP1Desi[i],s.sDP1Slot[i],s.sDP1ID[i]); /* DP1_DESI[] */ /* DP1_SLOT[] */ /* DP1_ID[] */
          printf( " | %10s %10s",s.sDP1INR[i][j],s.sDP1INC[i][j]); /* DP1_INR[][] */ /* DP1_INC[][] */
          printf("\n");
          } 
        }
      printf("key:\n");
      printf("  DP1 board #\n");
      printf("  channel # on this board\n");
      printf("| stat (static)\n");
      if (!bStatic) printf("  stat (dynamic)\n");
      printf("| DP1_ANT (ant# associated with this channel)\n");
      printf("| DP1_DESI\n");
      printf("  DP1_SLOT\n");
      printf("  DP1_ID\n");
      printf("| DP1_INR (label on input connector for this channel on DP rack)\n");
      printf("  DP1_INC (label on input connector for this channel on DP chassis)\n");
      break;

    case TPSDM_RPT_DP2: /* DP2 */
      for (i=0;i<s.nDP2;i++) { 
        printf( "%2d %1d",i+1,s.eDP2Stat[i]); 
        if (!bStatic) printf( " %1d",sdm.ssss.eDP2Stat[i]); 
        printf(" | %10s %10s %2d\n",
                   s.sDP2ID[i],
                        s.sDP2Slot[i],
                             s.eDP2Desi[i] ); 
        }
      printf("key:\n");
      printf("  DP2 board #\n");
      printf("  stat (static)\n");
      if (!bStatic) printf("  stat (dynamic)\n");
      printf("| DP2_ID\n");
      printf("  DP2_SLOT\n");
      printf("  DP2_DESI\n");
      break;
#endif

    case TPSDM_RPT_DR: /* DR */
      for (i=0;i<s.nDR;i++) { 
        printf( "%2d %1d",i+1,s.eDRStat[i]);
        if (!bStatic) printf( " %1d",sdm.ssss.eDRStat[i]);  
        printf(" | %1d %10s %10s\n", 
                   s.iDRDP[i],
                       s.sDRID[i],    
                            s.sDRPC[i] );  
        }
      printf("key:\n");
      printf("  DR #\n");
      printf("  stat (static)\n");
      if (!bStatic) printf("  stat (dynamic)\n");
      printf("| DR_DP (DP output that this DR is connected to (1-4 are beams, 5=TBW/TBN)\n");
      printf("  DR_ID\n");
      printf("  DP_PC\n");
      break;

    case TPSDM_RPT_ANS: /* ans */
      for (c=3;c>=0;c--) {
        printf("The following Stands/Antennas are Status %d: ",c); 
        m=0;  
        for ( i=0; i<s.nStd; i++ ) {
          for ( k=0; k<2; k++) {
            if (sc.Stand[i].Ant[k].iSS==c) {
            m++;
            printf("%d/%d ",i+1,k);
            }
          }
        }
      printf("\nTotal %d antennas are Status %d.\n",m,c);      
      }
      break;

    case TPSDM_RPT_ANA: /* ana */

      for ( i=0; i<s.nStd; i++ ) {
        printf("%3d |F %2hd |A %2hd %2hd %2hd %2hd",
                i+1, 
                       s.settings.fee[i],
                               s.settings.asp_flt[i], 
                                    s.settings.asp_at1[i], 
                                         s.settings.asp_at2[i], 
                                              s.settings.asp_ats[i]); 
        if (!bStatic) 
        printf(" |F %2hd |A %2hd %2hd %2hd %2hd", 
                    sdm.settings.fee[i],
                            sdm.settings.asp_flt[i], 
                                 sdm.settings.asp_at1[i], 
                                      sdm.settings.asp_at2[i], 
                                           sdm.settings.asp_ats[i]); 
        printf("\n");
        } /* for i */ 
      printf("key:\n");
      printf("   stand #\n");
      printf("|F FEE (1=power on, 0=power off) (static)\n");  
      printf("|A ASP_FLT (0=split,1=full,2=reduced,3=off) (static)\n");
      printf("   ASP_AT1 (0-15) (static)\n");
      printf("   ASP_AT2 (0-15) (static)\n");
      printf("   ASP_ATS (0-15) (static)\n");
      if (!bStatic) {
        printf("|F FEE (1=power on, 0=power off) (dynamic)\n");  
        printf("|A ASP_FLT (0=split,1=full,2=reduced,3=off) (dynamic)\n");
        printf("   ASP_AT1 (0-15) (dynamic)\n");
        printf("   ASP_AT2 (0-15) (dynamic)\n");
        printf("   ASP_ATS (0-15) (dynamic)\n");
        }
      break;

    case TPSDM_RPT_ANP: /* anp */

      if (bStatic) {

        for ( i=0; i<s.nStd; i++ ) {
          for ( k=0; k<2; k++) {
            printf("%3d %1d | %+8.2lf %+8.2lf %+6.2lf |A %1d %3d %1d",
                    i+1, 
                        sc.Stand[i].Ant[k].iSS,
                              sc.Stand[i].fLx,
                                      sc.Stand[i].fLy,
                                              sc.Stand[i].fLz,
                                                         k, 
                                                            sc.Stand[i].Ant[k].iID+1,
                                                                 sc.Stand[i].Ant[k].iStat);     
            printf(" |F %3d %1d",
                      sc.Stand[i].Ant[k].FEE.i+1,
                          sc.Stand[i].Ant[k].FEE.iStat);    
            printf(" |R %3d %1d",
                      sc.Stand[i].Ant[k].RPD.i+1,
                          sc.Stand[i].Ant[k].RPD.iStat); 
            printf(" |S %3d %1d %5s",
                        sc.Stand[i].Ant[k].SEP.i+1,
                            sc.Stand[i].Ant[k].SEP.iStat,
                               sc.Stand[i].Ant[k].SEP.sID); 
            printf(" |A %2d %2d %1d",
                        sc.Stand[i].Ant[k].ARX.i+1,
                            sc.Stand[i].Ant[k].ARX.c+1,
                                sc.Stand[i].Ant[k].ARX.iStat);
            printf(" |D %2d %2d %1d",
                        sc.Stand[i].Ant[k].DP.i+1,
                            sc.Stand[i].Ant[k].DP.c+1,
                                sc.Stand[i].Ant[k].DP.iStat);
            printf("\n");
            } /* for k */
          } /* for i */
 
        printf("key:\n");
        printf("   stand #\n");
        printf("   stat (for this path) (static)\n");
        printf("|  STD_LX [m]\n");
        printf("   STD_LY [m]\n");
        printf("   STD_LZ [m]\n");        
        printf("|A 0 or 1 (indexing antennas on this stand)\n");        
        printf("   ant #\n");
        printf("   stat (of this antenna) (static)\n");             
        printf("|F FEE #\n");
        printf("   stat (of this FEE) (static)\n"); 
        printf("|R cable #\n");
        printf("   stat (of this cable) (static)\n"); 
        printf("|S SEP port #\n");
        printf("   stat (of this SEP port) (static)\n"); 
        printf("   SEP_ID\n");
        printf("|A ARX board #\n");
        printf("   channel # on this ARX board\n");  
        printf("   stat (of this ARX channel) (static)\n");     
        printf("|D DP1 board #\n");
        printf("   channel # on this DP1 board\n");  
        printf("   stat (of this DP1 channel) (static)\n");  

        } else {

        for (i=0;i<s.nStd;i++) { 
          for (j=0;j<2;j++) { 
            printf( "%3d %1d |",i+1,j);
            printf( " %1d",sc.Stand[i].Ant[j].iSS); 
            printf( " %1d",sdm.ant_stat[i][j]); 
            printf("\n");
            }
          }

        printf("key:\n");
        printf("   stand #\n");
        printf("   0 or 1 (indexing antennas on this stand)\n"); 
        printf("|  stat (for this path) (static)\n");
        printf("   stat (for this path) (dynamic)\n");

        } /* if (bStatic) */

      break;


    case TPSDM_RPT_DPP: /* dpp */
      if (bStatic) {

#ifdef USE_ADP
        for ( i=0; i<ME_MAX_NDPOUT; i++ ) {
          if (i<ME_MAX_NDPOUT-1)              { sprintf(st1," Beam #%d",i+1);                     }
          if (i>ME_MAX_NDPOUT-2)              { sprintf(st1," TBN ");         }
          if ( sc.DPO[i].iDR > 0 ) { 
              sprintf(st2," DR%1d %1d",sc.DPO[i].iDR,s.eDRStat[sc.DPO[i].iDR-1]); 
              //printf("<%1d>",s.eDRStat[sc.DPO[i].iDR]); 
            } else {
              strcpy(st2,"");
            }
          printf("%1d %1d |%s |%s\n",
                  i+1, 
                      sc.DPO[i].iStat,
                           st1,st2    );
          } /* for i */

        printf("key:\n");
        printf("  ADP output #\n");
        printf("  stat (for this path) (static)\n");
        printf("| 'DP2'                     or 'TBW/TBN'\n");
        printf("  DP2 board #               or ''\n");
        printf("  stat (for this DP2 board) or ''\n");
        printf("| DR_ID (DR associated with this DP output)\n");
        printf("  stat (for this DR) (static)\n");
#else
        for ( i=0; i<ME_MAX_NDPOUT; i++ ) {
          if (i<2)              { k=1; m = s.eDP2Stat[0]; sprintf(st1," DP2 #%1d %1d",k,m); }
          if ( (i>1) && (i<ME_MAX_NDPOUT-1) ) { k=2; m = s.eDP2Stat[1]; sprintf(st1," DP2 #%1d %1d",k,m); }
          if (i>ME_MAX_NDPOUT-2)              { k=0; m = 0;             sprintf(st1," TBW/TBN ");         }
          if ( sc.DPO[i].iDR > 0 ) { 
              sprintf(st2," DR%1d %1d",sc.DPO[i].iDR,s.eDRStat[sc.DPO[i].iDR-1]); 
              //printf("<%1d>",s.eDRStat[sc.DPO[i].iDR]); 
            } else {
              strcpy(st2,"");
            }
          printf("%1d %1d |%s |%s\n",
                  i+1, 
                      sc.DPO[i].iStat,
                           st1,st2    );
          } /* for i */

        printf("key:\n");
        printf("  DP output #\n");
        printf("  stat (for this path) (static)\n");
        printf("| 'DP2'                     or 'TBW/TBN'\n");
        printf("  DP2 board #               or ''\n");
        printf("  stat (for this DP2 board) or ''\n");
        printf("| DR_ID (DR associated with this DP output)\n");
        printf("  stat (for this DR) (static)\n");
#endif

        } else {

          for (i=0;i<s.nDR;i++) {
            printf( "%2d %1d", i+1, sc.DPO[i].iStat ); 
            if (!bStatic) printf( " %1d", sdm.dpo_stat[i] ); 
            printf("\n");     
            }

        printf("key:\n");
        printf("  DP output #\n");
        printf("  stat (for this path) (static)\n");
        printf("  stat (for this path) (dynamic)\n");
 
        } /* if (bStatic) */
      break;

    case TPSDM_RPT_PWR: /* pwr */
      for (i=0;i<s.nPwrRack;i++) {
        for (j=0;j<s.nPwrPort[i];j++) {
          printf( "%2d %3d %3s %s",i+1,j+1,LWA_sid2str(s.ePwrSS[i][j]),s.sPwrName[i][j]); 
                                           /* PWR_SS[][], converted to a LWA_SID_ value */
                                                                      /* PWR_NAME[][] */
          printf("\n");
          }
        }
      printf("key:\n");
      printf("  rack #\n");
      printf("  port #\n");
      printf("  destination level-1 subsystem ('XXX' is same as 'UNK')\n");
      printf("  destination within this subsystem\n"); 
      break;

    default:
      break;

    } /* switch(eRpt) */

  return 0;
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// tpsdm.c: J. Dowell, UNM, 2018 Jan 29
//   .1 Cleaned up a few compiler warnings
// tpsdm.c: J. Dowell, UNM, 2015 Sep 11
//   .1: Updated for ADP-based stations
// tpsdm.c: S.W. Ellingson, Virginia Tech, 2011 April 05
//   .1: Added new 'settings' parameters to 'sta' report; added 'ana' report
// tpsdm.c: S.W. Ellingson, Virginia Tech, 2011 March 26
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================


