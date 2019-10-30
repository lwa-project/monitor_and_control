// ms_mcic_ADP.c: S.W. Ellingson, Virginia Tech, 2010 May 31
// ---
// COMPILE: This file is #include'd in ms_mcic.h.
// ---
// COMMAND LINE: (not applicable)
// ---
// REQUIRES: see ms_mcic.h.  Assumes functions defined there.
// ---
// See end of this file for history.


int LWA_mibupdate_ADP( 
                      GDBM_FILE dbm_ptr,         /* pointer to an open dbm file */
                      int cid,                   /* command, so handler knows how to deal with it */ 
                                                 /* Note: should not be PNG, RPT, or SHT! */
                      char           *cmdata,    /* the DATA field from the *command* message */
                      char           *r_comment, /* R-COMMENT */
                      int            datalen,    /* number of significant bytes in r_comment */
                      long int       ref,        /* REFERENCE field, uniquely identifying command/response pair */ 
                      struct timeval sent_tv     /* time of command message */
                     ) {
  /* This is the handler for the ADP subsystem. */
  
  int eMIBerror = LWA_MIBERR_OK;

  char sMIBlabel[MIB_LABEL_FIELD_LENGTH];

  unsigned char uc;
  unsigned short int usi;
  char tbf_bits[3];
  unsigned int ui; 
  signed int si;
  unsigned long int uli;
  char tbf_trig_time[10];
  char tbf_samples[9];
  unsigned long int ul;
  char tbf_tuning_mask[20];

  char sData[10];
  long int mjd, mpm;

  float f4;
  char tbn_freq[13];
  char tbn_bw[2];
  char tbn_gain[3];    
  
  char cor_navg[10];
  char cor_tuning_mask[20];
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
  float freq=1.0;
  unsigned char ebw;
  unsigned short int gain;
  
  char output[1024];

  switch (cid) {

    case LWA_CMD_TBF:
      /* When we get an "A" in response to the TBF command, use LWA_mibupdate_RPT() */
      /* to update the corresponding non-ICD (i.e., added by me) MIB entries TBF_BITS, */
      /* TBF_TRIG_TIME, TBF_SAMPLES, TBF_TUNING_MASK, TBF_REFERENCE, and TBF_CMD_SENT_MPM */

      //Looking at cmdata:
      //printf("ABC: <%x>\n",(unsigned int)cmdata[0]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[1]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[2]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[3]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[4]);

      /* parse the contents of cmdata (the DATA field form the command message) into the included 4 parameters */
      memset(&uc,0,1);
        memcpy(&uc,cmdata+0,1); 
        sprintf(tbf_bits,"%hhu",uc);
      memset(&si,0,4);
        memcpy(&si,cmdata+1,4);
        //sprintf(tbw_trig_time,"%9u",ui);
        sprintf(tbf_trig_time,"%9d",LWA_i4s_swap(si));
        //printf("ABC: <%u> <%u> <%s>\n",ui,LWA_i4u_swap(ui),tbw_trig_time);
      memset(&si,0,4);
        memcpy(&si,cmdata+5,4);
        //sprintf(tbw_samples,"%8u",ui);
        sprintf(tbf_samples,"%9d",LWA_i4s_swap(si));
        //printf("ABC: <%u> <%u> <%s>\n",ui,LWA_i4u_swap(ui),tbw_samples);
      memset(&uli,0,8);
        memcpy(&uli,cmdata+9,8);
        //sprintf(tbw_tuning_mask,"%20u",uli);
        sprintf(tbf_tuning_mask,"%20d",LWA_i8u_swap(uli));

      /* updating the MIB using the LWA_mibupdate_RPT() function */

      sprintf(sMIBlabel,"MCS_TBF_BITS");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbf_bits, -1 );

      sprintf(sMIBlabel,"MCS_TBF_TRIG_TIME");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbf_trig_time, -1 );

      sprintf(sMIBlabel,"MCS_TBF_SAMPLES");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbf_samples, -1 );

      sprintf(sMIBlabel,"MCS_TBF_TUNING_MASK");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbf_tuning_mask, -1 );
      
      sprintf(sMIBlabel,"MCS_TBF_REFERENCE");
      sprintf(sData,"%9ld",ref);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      sprintf(sMIBlabel,"MCS_TBF_CMD_SENT_MPM");
      LWA_timeval(&sent_tv,&mjd,&mpm);
      sprintf(sData,"%9ld",mpm);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      break;

    case LWA_CMD_TBN:
      /* When we get an "A" in response to the TBN command, use LWA_mibupdate_RPT() */
      /* to update the corresponding non-ICD (i.e., added by me) MIB entries TBN_FREQ, */
      /* TBN_BW, TBN_GAIN, TBN_SUB_SLOT, TBN_REFERENCE, TBN_CMD_SENT_MPM */

      //Looking at cmdata:
      //printf("ABC: <%x>\n",(unsigned int)cmdata[0]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[1]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[2]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[3]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[4]);

      /* parse the contents of cmdata (the DATA field form the command message) into the included 3 parameters */
      memset(&f4,0,4);                  
        memcpy(&f4,cmdata+0,4);          
        sprintf(tbn_freq,"%12.3f",LWA_f4_swap(f4)); /* (12345678.123); swapping endianness */
      memset(&usi,0,2);
        memcpy(&usi,cmdata+4,2);
        sprintf(tbn_bw,"%1u",LWA_i2u_swap(usi));
      memset(&usi,0,2);
        memcpy(&usi,cmdata+6,2);
        sprintf(tbn_gain,"%hu",LWA_i2u_swap(usi));

      /* updating the MIB using the LWA_mibupdate_RPT() function */

      sprintf(sMIBlabel,"MCS_TBN_FREQ");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbn_freq, -1 );

      sprintf(sMIBlabel,"MCS_TBN_BW");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbn_bw, -1 );

      sprintf(sMIBlabel,"MCS_TBN_GAIN");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbn_gain, -1 );

      sprintf(sMIBlabel,"MCS_TBN_REFERENCE");
      sprintf(sData,"%9ld",ref);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      sprintf(sMIBlabel,"MCS_TBN_CMD_SENT_MPM");
      LWA_timeval(&sent_tv,&mjd,&mpm);
      sprintf(sData,"%9ld",mpm);
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
        memcpy(&usi,cmdata+4,8);
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
      memcpy( &tuning,   cmdata+0, 1 );
      
      memset(&freq,0,4);                  
        memcpy(&freq,cmdata+1,4);          
        freq = LWA_f4_swap(freq); /* swapping endianness */
      //memcpy( (&freq)+0, cmdata+5, 1 ); /* flipping endian-ness of freq */
      //memcpy( (&freq)+1, cmdata+4, 1 );
      //memcpy( (&freq)+2, cmdata+3, 1 );
      //memcpy( (&freq)+3, cmdata+2, 1 );

      memcpy( &ebw,      cmdata+5, 1 ); 
      memcpy( (&gain)+0, cmdata+7, 1 ); /* flipping endian-ness of gain */
      memcpy( (&gain)+1, cmdata+6, 1 ); 
      /* ignoring cmdata[9] = subslot */

      //printf("freq=%f %hhu %hhu %hhu %hhu\n",freq, cmdata[5], cmdata[4], cmdata[3], cmdata[2] );

      sprintf(sMIBlabel,"MCS_TUNING%1hhu_FREQ",tuning);
      sprintf(sData,"%12.3f",freq);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      sprintf(sMIBlabel,"MCS_TUNING%1hhu_BW",tuning);
      sprintf(sData,"%1hhu",ebw);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      sprintf(sMIBlabel,"MCS_TUNING%1hhu_GAIN",tuning);
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
      if( !strncmp("TBN", output, 3) ) {
          /* TBN */
          sprintf(sMIBlabel,"MCS_TBN_FREQ");
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "0.0", -1 );
          
          sprintf(sMIBlabel,"MCS_TBN_BW");
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "0", -1 );
          
          sprintf(sMIBlabel,"MCS_TBN_GAIN");
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "0", -1 );
      }
      if( !strncmp("TBF", output, 3) ) {
          /* TBF */
          sprintf(sMIBlabel,"MCS_TBF_BITS");
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "0", -1 );
          
          sprintf(sMIBlabel,"MCS_TBF_TRIG_TIME");
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "0", -1 );
          
          sprintf(sMIBlabel,"MCS_TBF_SAMPLES");
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "0", -1 );
          
          sprintf(sMIBlabel,"MCS_TBF_TUNING_MASK");
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
          for ( i=1; i<=520; i++) {
            sprintf(sMIBlabel,"MCS_CF%03d",i);
            eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "DP_DEFAULT", -1 );
            }
          break;
        case 0: /* all filters have been loaded with specified coefficients */
          for ( i=1; i<=520; i++) {
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
// ms_mcic_ADP.c: J. Dowell, UNM, 2019 Oct 30
//   .1 Convert to using normal GDBM for the database
// ms_mcic_ADP.c: J. Dowell, UNM, 15 Aug 10
//   initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
//
