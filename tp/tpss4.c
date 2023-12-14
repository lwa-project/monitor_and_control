// 13023: fixed OBS_BEAM_GAIN[][][][] block

  strcpy(keyword,"OBS_STP_N"); 
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%ld",&(obs[nobs].OBS_STP_N));
      printf("...converts to %ld\n",obs[nobs].OBS_STP_N);
      if ( ( obs[nobs].OBS_STP_N<1 ) || ( obs[nobs].OBS_STP_N>MAX_STP_N ) ) {   
        printf("[%d/%d] FATAL: OBS_STP_N out of range\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_STP_RADEC"); 
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&(obs[nobs].OBS_STP_RADEC));
      printf("...converts to %d\n",obs[nobs].OBS_STP_RADEC);
      if ( ( obs[nobs].OBS_STP_RADEC<0 ) || ( obs[nobs].OBS_STP_RADEC>1 ) ) {   
        printf("[%d/%d] FATAL: OBS_STP_RADEC out of range\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  for ( k=1; k<=obs[nobs].OBS_STP_N; k++ ) {
    printf("[%d/%d] --- Parsing Defined Step #%d of Observation #%d ---\n",MT_TPSS,getpid(),k,nobs);

    sprintf(keyword,"OBS_STP_C1[%d]",k); 
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
        sscanf(data,"%f",&(obs[nobs].OBS_STP_C1[k]));
        printf("...converts to %f\n",obs[nobs].OBS_STP_C1[k]);
        switch (obs[nobs].OBS_STP_RADEC) {
          case 0: /* AZ */
            if ( ( obs[nobs].OBS_STP_C1[k]<0.0 ) || ( obs[nobs].OBS_STP_C1[k]>=360.0 ) ) {   
              printf("[%d/%d] FATAL: OBS_STP_C1[%d] out of range\n",MT_TPSS,getpid(),k);  
              exit(EXIT_FAILURE);
              }
            break;
          case 1: /* RA */
            if ( ( obs[nobs].OBS_STP_C1[k]<0.0 ) || ( obs[nobs].OBS_STP_C1[k]>=24.0 ) ) {   
              printf("[%d/%d] FATAL: OBS_STP_C1[%d] out of range\n",MT_TPSS,getpid(),k);  
              exit(EXIT_FAILURE);
              }
            break;
          }
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
      case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
      }

    sprintf(keyword,"OBS_STP_C2[%d]",k); 
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
        sscanf(data,"%f",&(obs[nobs].OBS_STP_C2[k]));
        printf("...converts to %f\n",obs[nobs].OBS_STP_C2[k]);
        switch (obs[nobs].OBS_STP_RADEC) {
          case 0: /* EL */
            if ( ( obs[nobs].OBS_STP_C2[k]<0.0 ) || ( obs[nobs].OBS_STP_C2[k]>90.0 ) ) {   
              printf("[%d/%d] FATAL: OBS_STP_C2[%d] out of range\n",MT_TPSS,getpid(),k);  
              exit(EXIT_FAILURE);
              }
            break;
          case 1: /* DEC */
            if ( ( obs[nobs].OBS_STP_C2[k]<-90.0 ) || ( obs[nobs].OBS_STP_C2[k]>+90.0 ) ) {   
              printf("[%d/%d] FATAL: OBS_STP_C2[%d] out of range\n",MT_TPSS,getpid(),k);  
              exit(EXIT_FAILURE);
              }
            break;
          }
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
      case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
      }

    sprintf(keyword,"OBS_STP_T[%d]",k); 
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
        sscanf(data,"%ld",&(obs[nobs].OBS_STP_T[k]));
        printf("...converts to %ld\n",obs[nobs].OBS_STP_T[k]);
        if (obs[nobs].OBS_STP_T[k]<MIN_OBS_STEP_LENGTH) {
          printf("[%d/%d] FATAL: obs[nobs].OBS_STP_T[%d] < MIN_OBS_STEP_LENGTH = %d\n",MT_TPSS,getpid(),k,MIN_OBS_STEP_LENGTH);   
          exit(EXIT_FAILURE);
          }
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
      case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
      }

    sprintf(keyword,"OBS_STP_FREQ1[%d]",k); 
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
        sscanf(data,"%ld",&(obs[nobs].OBS_STP_FREQ1[k]));
        printf("...converts to %ld\n",obs[nobs].OBS_STP_FREQ1[k]);
#if (defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP) || (defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP)
        if ( ( obs[nobs].OBS_STP_FREQ1[k]<222417950 ) || ( obs[nobs].OBS_STP_FREQ1[k]>1928352663 ) ) {   
          printf("[%d/%d] FATAL: OBS_STP_FREQ1[%d] out of range\n",MT_TPSS,getpid(),k);  
          exit(EXIT_FAILURE);
          }
#else
        if ( ( obs[nobs].OBS_STP_FREQ1[k]<219130984 ) || ( obs[nobs].OBS_STP_FREQ1[k]>1928352663 ) ) {   
          printf("[%d/%d] FATAL: OBS_STP_FREQ1[%d] out of range\n",MT_TPSS,getpid(),k);  
          exit(EXIT_FAILURE);
          }
#endif
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
      case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
      }

    sprintf(keyword,"OBS_STP_FREQ1+[%d]",k); strcpy(obs[nobs].OBS_STP_FREQ1p[k],"");
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
        strcpy(obs[nobs].OBS_STP_FREQ1p[k],data);
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:                                                                                                        break;
      case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
      }

    sprintf(keyword,"OBS_STP_FREQ2[%d]",k); 
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
        sscanf(data,"%ld",&(obs[nobs].OBS_STP_FREQ2[k]));
        printf("...converts to %ld\n",obs[nobs].OBS_STP_FREQ2[k]);
#if (defined(LWA_BACKEND_IS_NDP) && LWA_BACKEND_IS_NDP) || (defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP)
        if ( obs[nobs].OBS_STP_FREQ2[k] == 0 ) {
          printf("[%d/%d] WARNING: OBS_STP_FREQ2[%d] is zero, this will be a half beam step",MT_TPSS,getpid(),k);
	   } else if ( ( obs[nobs].OBS_STP_FREQ2[k]<222417950 ) || ( obs[nobs].OBS_STP_FREQ2[k]>1928352663 ) ) {   
          printf("[%d/%d] FATAL: OBS_STP_FREQ2[%d] out of range\n",MT_TPSS,getpid(),k);  
          exit(EXIT_FAILURE);
          }
#else
        if ( obs[nobs].OBS_STP_FREQ2[k] == 0 ) {
          printf("[%d/%d] WARNING: OBS_STP_FREQ2[%d] is zero, this will be a half beam step",MT_TPSS,getpid(),k);
	   } else if ( ( obs[nobs].OBS_STP_FREQ2[k]<219130984 ) || ( obs[nobs].OBS_STP_FREQ2[k]>1928352663 ) ) {   
          printf("[%d/%d] FATAL: OBS_STP_FREQ2[%d] out of range\n",MT_TPSS,getpid(),k);  
          exit(EXIT_FAILURE);
          }
#endif
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
      case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
      }

    sprintf(keyword,"OBS_STP_FREQ2+[%d]",k); strcpy(obs[nobs].OBS_STP_FREQ2p[k],"");
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
        strcpy(obs[nobs].OBS_STP_FREQ2p[k],data);
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
      case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
      }

    sprintf(keyword,"OBS_STP_B[%d]",k); 
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
        if (!(obs[nobs].OBS_STP_B[k] = LWA_getbeamtype(data))) {
          printf("[%d/%d] FATAL: Invalid OBS_STP_B[%d]\n",MT_TPSS,getpid(),k);   
          exit(EXIT_FAILURE);
          } 
        LWA_saybeamtype( obs[nobs].OBS_STP_B[k], mode_string );
        printf("...converts to '%s'\n",mode_string); 
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
      case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
      }

    if (obs[nobs].OBS_STP_B[k]==LWA_BT_SPEC_DELAYS_GAINS) {

      for (p=1;p<=2*LWA_MAX_NSTD;p++) {

        sprintf(keyword,"OBS_BEAM_DELAY[%d][%d]",k,p); 
        while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
        switch (i) {
          case TPSS_PL_KEYWORD_MATCH:    
            printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
            sscanf(data,"%hu",&(obs[nobs].OBS_BEAM_DELAY[k][p])); 
            printf("...converts to %hu\n",obs[nobs].OBS_BEAM_DELAY[k][p]); 
            if (obs[nobs].OBS_BEAM_DELAY[k][p]>MAX_BEAM_DELAY) {
              printf("[%d/%d] FATAL: Invalid OBS_BEAM_DELAY[%d]\n",MT_TPSS,getpid(),k); 
              }
            strcpy(data,"");   
            break;
          case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
          case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
          case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
          }

        } /* for p */

      for (p=1;p<=LWA_MAX_NSTD;p++) {
      for (q=1;q<=2;q++) {
      for (r=1;r<=2;r++) {

        sprintf(keyword,"OBS_BEAM_GAIN[%d][%d][%d][%d]",k,p,q,r); 
        while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
        switch (i) {
          case TPSS_PL_KEYWORD_MATCH:    
            printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
            sscanf(data,"%hd",&(obs[nobs].OBS_BEAM_GAIN[k][p][q][r])); 
            printf("...converts to %hd\n",obs[nobs].OBS_BEAM_GAIN[k][p][q][r]); 
            //if (obs[nobs].OBS_BEAM_DELAY[k][p][q][r]>MAX_BEAM_DELAY) {
            // printf("[%d/%d] FATAL: Invalid OBS_BEAM_DELAY[%d]\n",MT_TPSS,getpid(),k); 
            //  }
            strcpy(data,"");   
            break;
          case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
          case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
          case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
          }

        } /* for r */
        } /* for q */
        } /* for p */

      } /*  if (obs[nobs].OBS_STP_B[k]==LWA_BT_SPEC_DELAYS_GAINS) */




    } /* for k */
  
