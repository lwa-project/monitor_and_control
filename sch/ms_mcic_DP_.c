// ms_mcic_DP_.c: S.W. Ellingson, Virginia Tech, 2010 May 31
// ---
// COMPILE: This file is #include'd in ms_mcic.h.
// ---
// COMMAND LINE: (not applicable)
// ---
// REQUIRES: see ms_mcic.h.  Assumes functions defined there.
// ---
// See end of this file for history.


int LWA_mibupdate_DP_( 
                      DBM *dbm_ptr,              /* pointer to an open dbm file */
                      int cid,                   /* command, so handler knows how to deal with it */ 
                                                 /* Note: should not be PNG, RPT, or SHT! */
                      char           *cmdata,    /* the DATA field from the *command* message */
                      char           *r_comment, /* R-COMMENT */
                      int            datalen,    /* number of significant bytes in r_comment */
                      long int       ref,        /* REFERENCE field, uniquely identifying command/response pair */ 
                      struct timeval sent_tv     /* time of command message */
                     ) {
  /* This is the handler for the DP subsystem. */
  
  int eMIBerror = LWA_MIBERR_OK;

  char sMIBlabel[MIB_LABEL_FIELD_LENGTH];

  unsigned short int usi;
  char tbw_bits[3];
  unsigned int ui; 
  char tbw_trig_time[10];
  char tbw_samples[9];

  char sData[10];
  long int mjd, mpm;

  float f4;
  char tbn_freq[13];
  char tbn_bw[2];
  char tbn_gain[3];    
  char tbn_sub_slot[3]; 

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

  switch (cid) {

    case LWA_CMD_TBW:
      /* When we get an "A" in response to the TBW command, use LWA_mibupdate_RPT() */
      /* to update the corresponding non-ICD (i.e., added by me) MIB entries TBW_BITS, */
      /* TBW_TRIG_TIME, TBW_SAMPLES, TBW_REFERENCE, and TBW_CMD_SENT_MPM */

      //Looking at cmdata:
      //printf("ABC: <%x>\n",(unsigned int)cmdata[0]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[1]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[2]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[3]);
      //printf("ABC: <%x>\n",(unsigned int)cmdata[4]);

      /* parse the contents of cmdata (the DATA field form the command message) into the included 3 parameters */
      memset(&usi,0,2);
        memcpy(&usi,cmdata+0,1); 
        sprintf(tbw_bits,"%2hu",usi);
      memset(&ui,0,4);
        memcpy(&ui,cmdata+1,4);
        //sprintf(tbw_trig_time,"%9u",ui);
        sprintf(tbw_trig_time,"%9u",LWA_i4u_swap(ui));
        //printf("ABC: <%u> <%u> <%s>\n",ui,LWA_i4u_swap(ui),tbw_trig_time);
      memset(&ui,0,4);
        memcpy(&ui,cmdata+5,4);
        //sprintf(tbw_samples,"%8u",ui);
        sprintf(tbw_samples,"%8u",LWA_i4u_swap(ui));
        //printf("ABC: <%u> <%u> <%s>\n",ui,LWA_i4u_swap(ui),tbw_samples);

      /* updating the MIB using the LWA_mibupdate_RPT() function */

      sprintf(sMIBlabel,"MCS_TBW_BITS");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbw_bits, -1 );

      sprintf(sMIBlabel,"MCS_TBW_TRIG_TIME");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbw_trig_time, -1 );

      sprintf(sMIBlabel,"MCS_TBW_SAMPLES");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbw_samples, -1 );

      sprintf(sMIBlabel,"MCS_TBW_REFERENCE");
      sprintf(sData,"%9ld",ref);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      sprintf(sMIBlabel,"MCS_TBW_CMD_SENT_MPM");
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
        sprintf(tbn_gain,"%2u",LWA_i2u_swap(usi));
      memset(&usi,0,2);
        memcpy(&usi,cmdata+8,1);
        sprintf(tbn_sub_slot,"%2u",usi);

      /* updating the MIB using the LWA_mibupdate_RPT() function */

      sprintf(sMIBlabel,"MCS_TBN_FREQ");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbn_freq, -1 );

      sprintf(sMIBlabel,"MCS_TBN_BW");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbn_bw, -1 );

      sprintf(sMIBlabel,"MCS_TBN_GAIN");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbn_gain, -1 );

      sprintf(sMIBlabel,"MCS_TBN_SUB_SLOT");
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, tbn_sub_slot, -1 );

      sprintf(sMIBlabel,"MCS_TBN_REFERENCE");
      sprintf(sData,"%9ld",ref);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      sprintf(sMIBlabel,"MCS_TBN_CMD_SENT_MPM");
      LWA_timeval(&sent_tv,&mjd,&mpm);
      sprintf(sData,"%9ld",mpm);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, sData, -1 );

      break;

    case LWA_CMD_DRX:
      /* When we get an "A" in response to the DRX command, use LWA_mibupdate_RPT() */
      /* to update the corresponding non-ICD (i.e., added by me) MIB entries BEAM?_T?_FREQ, */
      /* BEAM?_T?_BW, and BEAM?_T?_GAIN. */

      //printf("freq=%f %hhu %hhu %hhu %hhu\n",freq, cmdata[5], cmdata[4], cmdata[3], cmdata[2] );

     /* recovering parameters from packed binary argument */
      memcpy( &beam,     cmdata+0, 1 );
      memcpy( &tuning,   cmdata+1, 1 );
      
      memset(&freq,0,4);                  
        memcpy(&freq,cmdata+2,4);          
        freq = LWA_f4_swap(freq); /* swapping endianness */
      //memcpy( (&freq)+0, cmdata+5, 1 ); /* flipping endian-ness of freq */
      //memcpy( (&freq)+1, cmdata+4, 1 );
      //memcpy( (&freq)+2, cmdata+3, 1 );
      //memcpy( (&freq)+3, cmdata+2, 1 );

      memcpy( &ebw,      cmdata+6, 1 ); 
      memcpy( (&gain)+0, cmdata+8, 1 ); /* flipping endian-ness of gain */
      memcpy( (&gain)+1, cmdata+7, 1 ); 
      /* ignoring cmdata[9] = subslot */

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
      /* BEAM?_DFILE and BEAM?_GFILE */

      /* Extract parameters from string*/      
      sscanf(cmdata,"%hu %s %s %hhu",&beam_id,dfile,gfile,&subslot);

      /* Save dfile name in MIB as BEAM<beam_id>_DFILE */
      sprintf(sMIBlabel,"MCS_BEAM%1hu_DFILE",beam_id);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, dfile, -1 );

      /* Save gfile name in MIB as BEAM<beam_id>_GFILE */
      sprintf(sMIBlabel,"MCS_BEAM%1hu_GFILE",beam_id);
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, gfile, -1 );

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
      f( !strncmp("BEAM", output, 4) ) {
          /* Beam */
          sscanf(output,"BEAM%hu", &beam_id);
          
          sprintf(sMIBlabel,"MCS_BEAM%1hu_DFILE",beam_id);
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "", -1 );
          
          sprintf(sMIBlabel,"MCS_BEAM%1hu_GFILE",beam_id);
          eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, sMIBlabel, "", -1 );
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
// ms_mcic_DP_.c: S.W. Ellingson, Virginia Tech, 2010 May 31
//   added MIB updating in response to accepted TBW and TBN commands
// ms_mcic_DP_.c: S.W. Ellingson, Virginia Tech, 2010 May 25
//   
// ms_mcic_DP_.c: S.W. Ellingson, Virginia Tech, 2009 Aug 16
//   initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
//
