// ms_mcic_NDP.c: S.W. Ellingson, Virginia Tech, 2010 May 31
// ---
// COMPILE: This file is #include'd in ms_mcic.h.
// ---
// COMMAND LINE: (not applicable)
// ---
// REQUIRES: see ms_mcic.h.  Assumes functions defined there.
// ---
// See end of this file for history.


int LWA_mibupdate_NDP( 
                      GDBM_FILE dbm_ptr,         /* pointer to an open dbm file */
                      int cid,                   /* command, so handler knows how to deal with it */ 
                                                 /* Note: should not be PNG, RPT, or SHT! */
                      char           *cmdata,    /* the DATA field from the *command* message */
                      char           *r_comment, /* R-COMMENT */
                      int            datalen,    /* number of significant bytes in r_comment */
                      long int       ref,        /* REFERENCE field, uniquely identifying command/response pair */ 
                      struct timeval sent_tv     /* time of command message */
                     ) {
  /* This is the handler for the NDP subsystem. */

  int eMIBerror = LWA_MIBERR_OK;

  char sMIBlabel[MIB_LABEL_FIELD_LENGTH];

  unsigned char uc;
  unsigned short int usi;
  unsigned int ui; 
  signed int si;
  unsigned long int uli;
  char tbt_trig_time[21];
  char tbt_samples[10];
  unsigned long int ul;
  char tbt_tuning_mask[21];

  char sData[13];
  long int mjd, mpm;

  float f4;
  double f8;
  
  char cor_navg[13];
  char cor_tuning_mask[21];
  char cor_gain[3];
  char cor_sub_slot[3];

  signed short int ssi;
  int i;

  unsigned short int beam_id;
  unsigned char subslot;
  char dfile[1024];
  char gfile[1024]; 

  unsigned char beam;
  unsigned char tuning;
  double freq=1.0;
  unsigned char ebw;
  unsigned short int gain;
  unsigned char high_dr;
  
  char output[1024];
  
  switch (cid) {

    case LWA_CMD_TBT:
      /* When we get an "A" in response to the TBT command, use LWA_mibupdate_RPT() */
      /* to update the corresponding non-ICD (i.e., added by me) MIB entries TBT_TRIG_TIME,
      /* TBT_SAMPLES, TBT_TUNING_MASK, TBT_REFERENCE, and TBT_CMD_SENT_MPM */

      //Looking at cmdata:
      //printf("ABC: <%x>\n",(unsigned int)cmdata[0]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[1]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[2]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[3]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[4]);

      /* parse the contents of cmdata (the DATA field form the command message) into the included 4 parameters */
      memset(&uli,0,8);
        memcpy(&uli,cmdata+0,8);
        //sprintf(tbt_trig_time,"%9u",ui);
        sprintf(tbt_trig_time,"%20d",LWA_i8u_swap(si));
        //printf("ABC: <%u> <%u> <%s>\n",ui,LWA_i8u_swap(ui),tbt_trig_time);
      memset(&si,0,4);
        memcpy(&si,cmdata+8,4);
        //sprintf(tbw_samples,"%8u",ui);
        sprintf(tbt_samples,"%9d",LWA_i4s_swap(si));
        //printf("ABC: <%u> <%u> <%s>\n",ui,LWA_i4u_swap(ui),tbw_samples);
      memset(&uli,0,8);
        memcpy(&uli,cmdata+12,8);
        //sprintf(tbw_tuning_mask,"%20u",uli);
        sprintf(tbt_tuning_mask,"%20d",LWA_i8u_swap(uli));

      /* updating the MIB using the LWA_mibupdate_RPT() function */

      sprintf(sMIBlabel,"MCS_TBT_TRIG_TIME");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbt_trig_time, -1 );

      sprintf(sMIBlabel,"MCS_TBT_SAMPLES");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbt_samples, -1 );

      sprintf(sMIBlabel,"MCS_TBT_TUNING_MASK");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbt_tuning_mask, -1 );
      
      sprintf(sMIBlabel,"MCS_TBT_REFERENCE");
      sprintf(sData,"%9ld",ref);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      sprintf(sMIBlabel,"MCS_TBT_CMD_SENT_MPM");
      LWA_timeval(&sent_tv,&mjd,&mpm);
      sprintf(sData,"%9ld",mpm);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      break;

    case LWA_CMD_TBS:
      /* When we get an "A" in response to the TBS command, use LWA_mibupdate_RPT() */
      /* to update the corresponding non-ICD (i.e., added by me) MIB entries FREQ and */
      /* BW. */

      /* recovering parameters from packed binary argument */
      memset(&freq,0,8);
      memcpy(&freq,cmdata,8);
      freq = LWA_f8_swap(freq); /* swapping endianness */
      
      memcpy(&ebw,  cmdata+8, 1);

      sprintf(sMIBlabel,"MCS_TBS_FREQ",beam,tuning);
      sprintf(sData,"%12.3f",freq);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      sprintf(sMIBlabel,"MCS_TBS_BW",beam,tuning);
      sprintf(sData,"%1hhu",ebw);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      break;
      
    case LWA_CMD_COR:
      /* When we get an "A" in response to the COR command, use LWA_mibupdate_RPT() */
      /* to update the corresponding non-ICD (i.e., added by me) MIB entries COR_NAVG, */
      /* COR_TUNING_MASK, COR_GAIN, COR_SUB_SLOT, COR_REFERENCE, COR_CMD_SENT_MPM */

      //Looking at cmdata:
      //printf("ABC: <%x>\n",(unsigned int)cmdata[0]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[1]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[2]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[3]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[4]);

      /* parse the contents of cmdata (the DATA field form the command message) into the included 3 parameters */
      memset(&si,0,4);                  
        memcpy(&si,cmdata+0,4);          
        sprintf(cor_navg,"%12d",LWA_i4s_swap(si)); /* (12345678.123); swapping endianness */
      memset(&uli,0,8);
        memcpy(&uli,cmdata+4,8);
        sprintf(cor_tuning_mask,"%20d",LWA_i8u_swap(uli));
      memset(&usi,0,2);
        memcpy(&usi,cmdata+12,2);
        sprintf(cor_gain,"%2u",LWA_i2u_swap(usi));
       memset(&uc,0,1);
        memcpy(&uc,cmdata+14,1);
        sprintf(cor_sub_slot,"%hhu",uc);

      /* updating the MIB using the LWA_mibupdate_RPT() function */

      sprintf(sMIBlabel,"MCS_COR_NAVG");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, cor_navg, -1 );

      sprintf(sMIBlabel,"MCS_COR_TUNING_MASK");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, cor_tuning_mask, -1 );

      sprintf(sMIBlabel,"MCS_COR_GAIN");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, cor_gain, -1 );

      sprintf(sMIBlabel,"MCS_COR_SUB_SLOT");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, cor_sub_slot, -1 );

      sprintf(sMIBlabel,"MCS_COR_REFERENCE");
      sprintf(sData,"%9ld",ref);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      sprintf(sMIBlabel,"MCS_COR_CMD_SENT_MPM");
      LWA_timeval(&sent_tv,&mjd,&mpm);
      sprintf(sData,"%9ld",mpm);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      break;
      
    case LWA_CMD_DRX:
      /* When we get an "A" in response to the DRX command, use LWA_mibupdate_RPT() */
      /* to update the corresponding non-ICD (i.e., added by me) MIB entries TUNING?_FREQ, */
      /* TUNING?_BW, and TUNING?_GAIN. */

      //printf("freq=%f %hhu %hhu %hhu %hhu\n",freq, cmdata[5], cmdata[4], cmdata[3], cmdata[2] );

      /* recovering parameters from packed binary argument */
      memcpy( &beam,     cmdata+0, 1 );
      memcpy( &tuning,   cmdata+1, 1 );
      
      memset(&freq,0,8);
      memcpy(&freq,cmdata+2,8);
      freq = LWA_f8_swap(freq); /* swapping endianness */
      
      memcpy(&ebw,  cmdata+10, 1);
      memcpy(&gain, cmdata+11, 2);
      gain = LWA_i2u_swap(gain); /* swapping endianness */
      /* ignoring cmdata[9] = high_dr */
      /* ignoring cmdata[10] = subslot */

      //printf("freq=%f %hhu %hhu %hhu %hhu\n",freq, cmdata[5], cmdata[4], cmdata[3], cmdata[2] );

      sprintf(sMIBlabel,"MCS_BEAM%1hhu_T%1hhu_FREQ",beam,tuning);
      sprintf(sData,"%12.3f",freq);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      sprintf(sMIBlabel,"MCS_BEAM%1hhu_T%1hhu_BW",beam,tuning);
      sprintf(sData,"%1hhu",ebw);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      sprintf(sMIBlabel,"MCS_BEAM%1hhu_T%1hhu_GAIN",beam,tuning);
      sprintf(sData,"%2hu",gain);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      //unsigned char beam;
      //unsigned char tuning;
      //float freq;
      //unsigned char ebw;
      //unsigned short int gain;
      //V 0.7.1.3.1     BEAM1_T1_FREQ 		0.000 	a12 	NUL
      //V 0.7.1.3.2     BEAM1_T1_BW 		0 	a1 	NUL
      //V 0.7.1.3.3     BEAM1_T1_GAIN 		0 	a2 	NUL

      break;

    case LWA_CMD_BAM: 
      /* When we get an "A" in response to the BAM command, use LWA_mibupdate_RPT() */
      /* to update the corresponding non-ICD (i.e., added by me) MIB entries */
      /* BEAM?_DFILE, BEAM?_GFILE, BEAM?_TUNING */

      /* Extract parameters from string*/      
      sscanf(cmdata,"%hu %s %s %hhu %hhu",&beam_id,dfile,gfile,&tuning,&subslot);

      /* Save dfile name in MIB as BEAM<beam_id>_DFILE */
      sprintf(sMIBlabel,"MCS_BEAM%hu_DFILE",beam_id);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, dfile, -1 );

      /* Save gfile name in MIB as BEAM<beam_id>_GFILE */
      sprintf(sMIBlabel,"MCS_BEAM%hu_GFILE",beam_id);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, gfile, -1 );

      /* Save tuning in MIB as BEAM<beam_id>_TUNING */
      sprintf(sMIBlabel,"MCS_BEAM%hu_TUNING",beam_id);
      sprintf(sData,"%hhu",tuning);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );
       
      break;

    case LWA_CMD_STP:
      /* When we get an "A" in response to the STP command, use LWA_mibupdate_RPT() */
      /* to update the relevant non-ICD (i.e., added by me) MIB entries */

      /* Extract the parameters from string */
      sscanf(cmdata,"%s", output);
      
      /* Which is it? */
      if( !strncmp("TBT", output, 3) ) {
          /* TBT */
          sprintf(sMIBlabel,"MCS_TBT_TRIG_TIME");
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "0", -1 );
          
          sprintf(sMIBlabel,"MCS_TBT_SAMPLES");
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "0", -1 );
          
          sprintf(sMIBlabel,"MCS_TBT_TUNING_MASK");
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "0", -1 );
      }
      if( !strncmp("TBS", output, 3) ) {
          /* TBS */
          printf(sMIBlabel,"MCS_TBS_FREQ");
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "0", -1 );
          
          sprintf(sMIBlabel,"MCS_TBS_BW");
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "0", -1 );
          
      }
      if( !strncmp("COR", output, 3) ) {
          /* COR */
          sprintf(sMIBlabel,"MCS_COR_NAVG");
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "0", -1 );
          
          sprintf(sMIBlabel,"MCS_COR_TUNING_MASK");
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "0", -1 );
          
          sprintf(sMIBlabel,"MCS_COR_GAIN");
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "0", -1 );
          
          sprintf(sMIBlabel,"MCS_COR_SUB_SLOT");
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "0", -1 );
      }
      if( !strncmp("BEAM", output, 4) ) {
          /* Beam */
          sscanf(output,"BEAM%hu", &beam_id);
          
          sprintf(sMIBlabel,"MCS_BEAM%1hu_DFILE",beam_id);
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "", -1 );
          
          sprintf(sMIBlabel,"MCS_BEAM%1hu_GFILE",beam_id);
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "", -1 );
          
          sprintf(sMIBlabel,"MCS_BEAM%1hu_TUNING",beam_id);
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "0", -1 );
      }
      
      break;
      
    case LWA_CMD_FST:
      /* When we get an "A" in response to the FST command, use LWA_mibupdate_RPT() */
      /* to update the corresponding non-ICD (i.e., added by me) MIB entries CF???. */

      /* Extract "index" */      
      sscanf(cmdata,"%hd %s",&ssi,sData); /* ssi is index */

      /* What happens depends on index: */
      switch (ssi) {
        case -1: /* all filters have been loaded with defaults from DP */
          for ( i=1; i<=2*LWA_MAX_NSTD; i++) {
            sprintf(sMIBlabel,"MCS_CF%03d",i);
            eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "DP_DEFAULT", -1 );
            }
          break;
        case 0: /* all filters have been loaded with specified coefficients */
          for ( i=1; i<=2*LWA_MAX_NSTD; i++) {
            sprintf(sMIBlabel,"MCS_CF%03d",i);
            eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, cmdata+2, -1 );
            }
          break;
        default: /* only specified filter has been loaded with specified coefficients */
          sprintf(sMIBlabel,"MCS_CF%03hd",ssi); 
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, cmdata+2, -1 );
          break;
        } /* switch (ssi) */

      break;

    case LWA_CMD_CLK:
      /* nothing to do, since the DATA field of the outbound command does not */
      /* correspond to anything in the MIB */ 
      //float32 CLK_SET_TIME
      break;

    case LWA_CMD_INI:
      /* nothing to do */
      break;

    default:
      /* command was PNG, RPT, SHT, or something not recognized) */
      eMIBerror = eMIBerror | LWA_MIBERR_SID_CID;
      break; 

    } /* switch (cid) */

  return eMIBerror;
  } 


//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// ms_mcic_ADP.c: J. Dowell, UNM, 2022 May 2
//   initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
//
