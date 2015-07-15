// ms_mcic_DR_.c: S.W. Ellingson, Virginia Tech, 2010 Jun 07
// ---
// COMPILE: This file is #include'd in ms_mcic.h.
// ---
// COMMAND LINE: (not applicable)
// ---
// REQUIRES: see ms_mcic.h.  Assumes functions defined there.
// ---
// See end of this file for history.


int LWA_mibupdate_DR_( 
                      DBM *dbm_ptr,    /* pointer to an open dbm file */
                      int cid,         /* command, so handler knows how to deal with it */ 
                                       /* Note: should not be PNG, RPT, or SHT! */
                      char *cmdata,    /* the DATA field from the *command* message */
                      char *r_comment, /* R-COMMENT */
                      int  datalen     /* number of significant bytes in "r_comment" */
                     ) {
  /* This is the handler for the DR1, DR2, DR3, DR4, and DR5 subsystems. */

  int eMIBerror = LWA_MIBERR_OK;

  char sTag[17];
  char sStartByte[16];
  char sLength[16];

  int nLength;

  char sDRSUSelected[3];

  char sBufConfig[6];
  char sBufOffset[17];
  char sBufWidth[17];
  char sBufInterval[17];

  switch (cid) {

    case LWA_CMD_INI:
      /* Nothing to do */
      break;

    case LWA_CMD_REC:
      /* Responds with <MJD>_<REFERENCE>, but doesn't seem to be any use for this since */
      /* same info should appear in SCHEDULE-ENTRY-X */
      /* Doing nothing. */
      break;

    case LWA_CMD_DEL:
      /* Nothing to do */
      break;

    case LWA_CMD_STP:
      /* Nothing to do */
      break;

    case LWA_CMD_GET:
      /* When we get an "A" in response to the GET command, use LWA_mibupdate_RPT() */
      /* to update the corresponding non-ICD (i.e., added by me) MIB entries */
      /* GET-TAG, GET-START-BYTE, GET-LENGTH (from command message), and */
      /* GET-DATA (from response message) */

      /* parse the DATA field of the command message */
      sscanf(cmdata,"%16s %15s %15s", sTag, sStartByte, sLength); 

      /* park these in the MIB */
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, "GET-TAG",        sTag,       -1 );
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, "GET-START-BYTE", sStartByte, -1 );
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, "GET-LENGTH",     sLength,    -1 );

      /* place response data in the MIB */
      sscanf(sLength,"%d",&nLength); 
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, "GET-DATA", r_comment, nLength );

      break;

    case LWA_CMD_CPY:
      /* Nothing to do */
      break;

    case LWA_CMD_DMP:
      /* Nothing to do */
      break;

    case LWA_CMD_FMT:
      /* Nothing to do */
      break;

    case LWA_CMD_DWN:
      /* Nothing to do */
      break;

    case LWA_CMD_UP_:
      /* Nothing to do */
      break;

    case LWA_CMD_SEL:
      /* When we get an "A" in response to the GET command, use LWA_mibupdate_RPT() */
      /* to update MIB entry DRSU-SELECTED from the command message */

      /* parse the DATA field of the command message */
      sscanf(cmdata,"%2s", sDRSUSelected); 

      /* park this in the MIB */
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, "DRSU-SELECTED", sDRSUSelected, -1 );

      break;

    case LWA_CMD_BUF:
      /* When we get an "A" in response to the BUF command, use LWA_mibupdate_RPT() */
      /* to update the corresponding non-ICD (i.e., added by me) MIB entries */
      /* BUF-CONFIG (5 characters together), BUF-OFFSET, BUFF-WIDTH, and BUF-INTERVAL (from command message) */

      /* parse the DATA field of the command message */
      sscanf(cmdata,"%5s %16s %16s %16s", sBufConfig, sBufOffset, sBufWidth, sBufInterval); 

      /* park these in the MIB */
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, "BUF-CONFIG",   sBufConfig,   -1 );
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, "BUF-OFFSET",   sBufOffset,   -1 );
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, "BUF-WIDTH",    sBufWidth,    -1 );
      eMIBerror = eMIBerror | LWA_mibupdate_RPT( dbm_ptr, "BUF-INTERVAL", sBufInterval, -1 );

      break;

    case LWA_CMD_SYN:
      /* Nothing to do */
      break;

    case LWA_CMD_SPC:
      /* Nothing to do */
      break;

    // "TST" is not supported for non-development use, according to the ICD
    //case LWA_CMD_TST:
    //  /* Nothing to do */
    //  break;

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
// ms_mcic_DR_.c: S.W. Ellingson, Virginia Tech, 2012 Feb 16
//   added rudimentary support for SPC command
// ms_mcic_DR_.c: S.W. Ellingson, Virginia Tech, 2010 Aug 10
//   initial version
// ms_mcic_ASP.c: S.W. Ellingson, Virginia Tech, 2009 Aug 06
//   initial version, then renamed "ms_mcic_GENERIC.c"

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
//
