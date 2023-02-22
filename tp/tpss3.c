
  strcpy(keyword,"OBS_TITLE"); strcpy(obs[nobs].OBS_TITLE,"");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      strcpy(obs[nobs].OBS_TITLE,data);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_TARGET"); strcpy(obs[nobs].OBS_TARGET,"");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      strcpy(obs[nobs].OBS_TARGET,data);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_REMPI"); strcpy(obs[nobs].OBS_REMPI,"");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      strcpy(obs[nobs].OBS_REMPI,data);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_REMPO"); strcpy(obs[nobs].OBS_REMPO,"");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      strcpy(obs[nobs].OBS_REMPO,data);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_START_MJD");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%ld",&(obs[nobs].OBS_START_MJD));
      printf("...converts to %ld\n",obs[nobs].OBS_START_MJD);
      if (!bIgnoreActualTime) {
        if ( obs[nobs].OBS_START_MJD<mjd ) {   
          printf("[%d/%d] FATAL: Specified OBS_START_MJD has already passed (it is now %ld)\n",MT_TPSS,getpid(),mjd);  
          exit(EXIT_FAILURE);
          }
        if ( obs[nobs].OBS_START_MJD > (mjd+MAX_MJD_SCHEDULE_AHEAD) ) {   
          printf("[%d/%d] FATAL: Specified OBS_START_MJD too far into future\n",MT_TPSS,getpid());  
          exit(EXIT_FAILURE);
          }
        } /* if (!bIgnoreActualTime) */
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_START_MPM");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%ld",&(obs[nobs].OBS_START_MPM));
      printf("...converts to %ld\n",obs[nobs].OBS_START_MPM);
      if ( ( obs[nobs].OBS_START_MPM<0 ) || ( obs[nobs].OBS_START_MPM>86400999 ) ) {   
        printf("[%d/%d] FATAL: Invalid OBS_START_MPM\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }
      if (!bIgnoreActualTime) {
        if ( ( obs[nobs].OBS_START_MJD==mjd ) && ( obs[nobs].OBS_START_MPM<=mpm ) ) {   
          printf("[%d/%d] FATAL: Specified OBS_START_MPM has already passed (it is now %ld)\n",MT_TPSS,getpid(),mpm);  
          exit(EXIT_FAILURE);
          }
        } /* if (!bIgnoreActualTime) */
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_START"); strcpy(obs[nobs].OBS_START,"");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      strcpy(obs[nobs].OBS_START,data);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_DUR"); obs[nobs].OBS_DUR=0;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%ld",&(obs[nobs].OBS_DUR));
      printf("...converts to %ld\n",obs[nobs].OBS_DUR);
      if ( obs[nobs].OBS_DUR<0 ) {   
        printf("[%d/%d] FATAL: Invalid OBS_DUR\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_DUR+"); strcpy(obs[nobs].OBS_DURp,"");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      strcpy(obs[nobs].OBS_DURp,data);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_MODE");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      if (!(obs[nobs].OBS_MODE = LWA_getmode(data))) { 
        printf("[%d/%d] FATAL: Invalid OBS_MODE\n",MT_TPSS,getpid());   
        exit(EXIT_FAILURE);
        }
      LWA_saymode( obs[nobs].OBS_MODE, mode_string );
      printf("...converts to '%s'\n",mode_string);      
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  /* added 140310 */
  strcpy(keyword,"OBS_BDM"); strcpy(obs[nobs].OBS_BDM,"");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      strcpy(obs[nobs].OBS_BDM,data);
      printf("...converts to '%s'\n",obs[nobs].OBS_BDM);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional keyword */                                                                    break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_RA");  obs[nobs].OBS_RA = -1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%f",&(obs[nobs].OBS_RA));
      printf("...converts to %f\n",obs[nobs].OBS_RA);      
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:                                                                                                        break;
    case TPSS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_DEC");  obs[nobs].OBS_DEC = -1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%f",&(obs[nobs].OBS_DEC));
      printf("...converts to %f\n",obs[nobs].OBS_DEC);      
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:                                                                                                        break;
    case TPSS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_B");  obs[nobs].OBS_B = LWA_BT_SIMPLE;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      if (!(obs[nobs].OBS_B = LWA_getbeamtype(data))) {
        printf("[%d/%d] FATAL: Invalid OBS_B\n",MT_TPSS,getpid());   
        exit(EXIT_FAILURE);
        } 
      LWA_saybeamtype( obs[nobs].OBS_B, mode_string );
      printf("...converts to '%s'\n",mode_string);  
      if (obs[nobs].OBS_B==LWA_BT_SPEC_DELAYS_GAINS) {
        printf("[%d/%d] FATAL: Invalid OBS_B\n",MT_TPSS,getpid());   
        exit(EXIT_FAILURE); 
        }    
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:                                                                                                        break;
    case TPSS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_FREQ1"); obs[nobs].OBS_FREQ1=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%ld",&(obs[nobs].OBS_FREQ1));
      printf("...converts to %ld\n",obs[nobs].OBS_FREQ1);
#if defined(USE_ADP) && USE_ADP
      if ( obs[nobs].OBS_MODE == LWA_OM_TBN ) {
         if ( ( obs[nobs].OBS_FREQ1<65739295 ) || ( obs[nobs].OBS_FREQ1>2037918156 ) ) {   
            printf("[%d/%d] FATAL: OBS_FREQ1 out of range\n",MT_TPSS,getpid());  
            exit(EXIT_FAILURE);
            }
      } else {
         if ( ( obs[nobs].OBS_FREQ1<222417950 ) || ( obs[nobs].OBS_FREQ1>1928352663 ) ) {   
            printf("[%d/%d] FATAL: OBS_FREQ1 out of range\n",MT_TPSS,getpid());  
            exit(EXIT_FAILURE);
            }
         }
#else
      if ( obs[nobs].OBS_MODE == LWA_OM_TBN ) {
         if ( ( obs[nobs].OBS_FREQ1<109565492 ) || ( obs[nobs].OBS_FREQ1>2037918156 ) ) {   
            printf("[%d/%d] FATAL: OBS_FREQ1 out of range\n",MT_TPSS,getpid());  
            exit(EXIT_FAILURE);
            }
      } else {
          if ( ( obs[nobs].OBS_FREQ1<219130984 ) || ( obs[nobs].OBS_FREQ1>1928352663 ) ) {   
            printf("[%d/%d] FATAL: OBS_FREQ1 out of range\n",MT_TPSS,getpid());  
            exit(EXIT_FAILURE);
            }
         }
#endif
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:                                                                                                        break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_FREQ1+"); strcpy(obs[nobs].OBS_FREQ1p,"");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      strcpy(obs[nobs].OBS_FREQ1p,data);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:                                                                                                        break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_FREQ2"); obs[nobs].OBS_FREQ2=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%ld",&(obs[nobs].OBS_FREQ2));
      printf("...converts to %ld\n",obs[nobs].OBS_FREQ2);
#if defined(USE_ADP) && USE_ADP
      if ( obs[nobs].OBS_FREQ2 == 0 ) {
        printf("[%d/%d] WARNING: OBS_FREQ2 is zero, this will be a half beam observation\n",MT_TPSS,getpid());
      } else if ( ( obs[nobs].OBS_FREQ2<222417950 ) || ( obs[nobs].OBS_FREQ2>1928352663 ) ) {   
        printf("[%d/%d] FATAL: OBS_FREQ2 out of range\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }
#else
      if ( obs[nobs].OBS_FREQ2 == 0 ) {
        printf("[%d/%d] WARNING: OBS_FREQ2 is zero, this will be a half beam observation\n",MT_TPSS,getpid());
      } else if ( ( obs[nobs].OBS_FREQ2<219130984 ) || ( obs[nobs].OBS_FREQ2>1928352663 ) ) {   
        printf("[%d/%d] FATAL: OBS_FREQ2 out of range\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }
#endif
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:                                                                                                        break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_FREQ2+"); strcpy(obs[nobs].OBS_FREQ2p,"");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      strcpy(obs[nobs].OBS_FREQ2p,data);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:                                                                                                        break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_BW"); obs[nobs].OBS_BW=0;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&(obs[nobs].OBS_BW));
      printf("...converts to %d\n",obs[nobs].OBS_BW);
#if defined(USE_ADP) && USE_ADP
      if ( obs[nobs].OBS_MODE == LWA_OM_TBN ) {
         if ( ( obs[nobs].OBS_BW<1 ) || ( obs[nobs].OBS_BW>7 ) ) { 		// TODO: Update this when possible
            printf("[%d/%d] FATAL: Invalid OBS_BW\n",MT_TPSS,getpid());
            exit(EXIT_FAILURE);
            }
       } else {
          if ( ( obs[nobs].OBS_BW<1 ) || ( obs[nobs].OBS_BW>7 ) ) { 		
            printf("[%d/%d] FATAL: Invalid OBS_BW\n",MT_TPSS,getpid());
            exit(EXIT_FAILURE);
            }
       }    
#else
      if ( ( obs[nobs].OBS_BW<1 ) || ( obs[nobs].OBS_BW>7 ) ) {   
        printf("[%d/%d] FATAL: Invalid OBS_BW\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }
#endif
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:                                                                                                        break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"OBS_BW+"); strcpy(obs[nobs].OBS_BWp,"");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      strcpy(obs[nobs].OBS_BWp,data);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:                                                                                                        break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  obs[nobs].OBS_STP_N = 0;
