
for (n=0;n<=LWA_MAX_NSTD;n++) { for (p=0;p<=1;p++) { obs[nobs].OBS_FEE[n][p]=-1; } }
for (n=0;n<=LWA_MAX_NSTD;n++) {
  for (p=0;p<=1;p++) {

    sprintf(keyword,"OBS_FEE[%d][%d]",n,p+1);  
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
        sscanf(data,"%d",&(obs[nobs].OBS_FEE[n][p]));
        printf("...converts to %d\n",obs[nobs].OBS_FEE[n][p]);
        if ( ( obs[nobs].OBS_FEE[n][p]<-1 ) || ( obs[nobs].OBS_FEE[n][p]>1 ) ) {   
          printf("[%d/%d] FATAL: OBS_FEE[%d][%d] out of range\n",MT_TPSS,getpid(),n,p+1);  
          return;
          }
        if (n==0) { /* this is intended to apply to all FEEs */
          printf("[%d/%d] ...Applying this setting to this pol of all FEEs\n",MT_TPSS,getpid()); 
          for (k=1;k<=LWA_MAX_NSTD;k++) { 
            obs[nobs].OBS_FEE[k][p] = obs[nobs].OBS_FEE[n][p];  
            //printf("%d %d %d | %d\n",nobs,k,p,obs[nobs].OBS_FEE[k][p]);
            }
          }
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:                                                                                            break;
      case TPSS_PL_KEYWORD_MISMATCH:                                                                               break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
      }
    } /* for p */
  } /* for n */ 

  //printf("%d %d %d\n",obs[1].OBS_FEE[0][0],obs[1].OBS_FEE[1][0],obs[1].OBS_FEE[2][0]);

for (n=0;n<=LWA_MAX_NSTD;n++) { obs[nobs].OBS_ASP_FLT[n]=-1; }
for (n=0;n<=LWA_MAX_NSTD;n++) {

    sprintf(keyword,"OBS_ASP_FLT[%d]",n);  
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
        sscanf(data,"%d",&(obs[nobs].OBS_ASP_FLT[n]));
        printf("...converts to %d\n",obs[nobs].OBS_ASP_FLT[n]);
        if ( ( obs[nobs].OBS_ASP_FLT[n]<-1 ) || ( obs[nobs].OBS_ASP_FLT[n]>3 ) ) {   
          printf("[%d/%d] FATAL: OBS_ASP_FLT[%d] out of range\n",MT_TPSS,getpid(),n);  
          return;
          }
        if (n==0) { /* this is intended to apply to all n */
          for (k=1;k<=LWA_MAX_NSTD;k++) obs[nobs].OBS_ASP_FLT[k] = obs[nobs].OBS_ASP_FLT[n];
          }
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:                                                                                            break;
      case TPSS_PL_KEYWORD_MISMATCH:                                                                               break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
      }

  } /* for n */ 

for (n=0;n<=LWA_MAX_NSTD;n++) { obs[nobs].OBS_ASP_AT1[n]=-1; }
for (n=0;n<=LWA_MAX_NSTD;n++) {

    sprintf(keyword,"OBS_ASP_AT1[%d]",n);  
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
        sscanf(data,"%d",&(obs[nobs].OBS_ASP_AT1[n]));
        printf("...converts to %d\n",obs[nobs].OBS_ASP_AT1[n]);
        if ( ( obs[nobs].OBS_ASP_AT1[n]<-1 ) || ( obs[nobs].OBS_ASP_AT1[n]>15 ) ) {   
          printf("[%d/%d] FATAL: OBS_ASP_AT1[%d] out of range\n",MT_TPSS,getpid(),n);  
          return;
          }
        if (n==0) { /* this is intended to apply to all n */
          for (k=1;k<=LWA_MAX_NSTD;k++) obs[nobs].OBS_ASP_AT1[k] = obs[nobs].OBS_ASP_AT1[n];
          }
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:                                                                                            break;
      case TPSS_PL_KEYWORD_MISMATCH:                                                                               break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
      }

  } /* for n */ 

for (n=0;n<=LWA_MAX_NSTD;n++) {  obs[nobs].OBS_ASP_AT2[n]=-1; }
for (n=0;n<=LWA_MAX_NSTD;n++) {

    sprintf(keyword,"OBS_ASP_AT2[%d]",n); 
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
        sscanf(data,"%d",&(obs[nobs].OBS_ASP_AT2[n]));
        printf("...converts to %d\n",obs[nobs].OBS_ASP_AT2[n]);
        if ( ( obs[nobs].OBS_ASP_AT2[n]<-1 ) || ( obs[nobs].OBS_ASP_AT2[n]>15 ) ) {   
          printf("[%d/%d] FATAL: OBS_ASP_AT2[%d] out of range\n",MT_TPSS,getpid(),n);  
          return;
          }
        if (n==0) { /* this is intended to apply to all n */
          for (k=1;k<=LWA_MAX_NSTD;k++) obs[nobs].OBS_ASP_AT2[k] = obs[nobs].OBS_ASP_AT2[n];
          }
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:                                                                                            break;
      case TPSS_PL_KEYWORD_MISMATCH:                                                                               break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
      }

  } /* for n */

for (n=0;n<=LWA_MAX_NSTD;n++) { obs[nobs].OBS_ASP_ATS[n]=-1; }
for (n=0;n<=LWA_MAX_NSTD;n++) {

    sprintf(keyword,"OBS_ASP_ATS[%d]",n);  
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
        sscanf(data,"%d",&(obs[nobs].OBS_ASP_ATS[n]));
        printf("...converts to %d\n",obs[nobs].OBS_ASP_ATS[n]);
        if ( ( obs[nobs].OBS_ASP_ATS[n]<-1 ) || ( obs[nobs].OBS_ASP_ATS[n]>15 ) ) {   
          printf("[%d/%d] FATAL: OBS_ASP_ATS[%d] out of range\n",MT_TPSS,getpid(),n);  
          return;
          }
        if (n==0) { /* this is intended to apply to all n */
          for (k=1;k<=LWA_MAX_NSTD;k++) obs[nobs].OBS_ASP_ATS[k] = obs[nobs].OBS_ASP_ATS[n];
          }
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:                                                                                            break;
      case TPSS_PL_KEYWORD_MISMATCH:                                                                               break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
      }

  } /* for n */

#ifdef USE_ADP
#else
    sprintf(keyword,"OBS_TBW_BITS");  obs[nobs].OBS_TBW_BITS=12;
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
        sscanf(data,"%d",&(obs[nobs].OBS_TBW_BITS));
        printf("...converts to %d\n",obs[nobs].OBS_TBW_BITS);
        if ( ( obs[nobs].OBS_TBW_BITS!=12 ) && ( obs[nobs].OBS_TBW_BITS!=4 ) ) {   
          printf("[%d/%d] FATAL: OBS_TBW_BITS out of range\n",MT_TPSS,getpid());  
          return;
          }
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:                                                                                            break;
      case TPSS_PL_KEYWORD_MISMATCH:                                                                               break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
      }

    sprintf(keyword,"OBS_TBW_SAMPLES");  
    obs[nobs].OBS_TBW_SAMPLES=12000000; if (obs[nobs].OBS_TBW_BITS==4) obs[nobs].OBS_TBW_SAMPLES=36000000;  /* default values */
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
        sscanf(data,"%ld",&(obs[nobs].OBS_TBW_SAMPLES));
        printf("...converts to %ld\n",obs[nobs].OBS_TBW_SAMPLES);
        if ( ( obs[nobs].OBS_TBW_BITS==12 ) && (obs[nobs].OBS_TBW_SAMPLES>12000000) ) {   
          printf("[%d/%d] FATAL: OBS_TBW_SAMPLES out of range\n",MT_TPSS,getpid());  
          return;
          }
        if ( ( obs[nobs].OBS_TBW_BITS==4 ) && (obs[nobs].OBS_TBW_SAMPLES>36000000) ) {   
          printf("[%d/%d] FATAL: OBS_TBW_SAMPLES out of range\n",MT_TPSS,getpid());  
          return;
          }
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:                                                                                            break;
      case TPSS_PL_KEYWORD_MISMATCH:                                                                               break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
      }
#endif

    sprintf(keyword,"OBS_TBN_GAIN");  obs[nobs].OBS_TBN_GAIN=-1; 
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
        sscanf(data,"%d",&(obs[nobs].OBS_TBN_GAIN));
        printf("...converts to %d\n",obs[nobs].OBS_TBN_GAIN);
#ifdef USE_ADP
        if ( ( obs[nobs].OBS_TBN_GAIN<-1 ) || (obs[nobs].OBS_TBN_GAIN>15) ) {  
#else
        if ( ( obs[nobs].OBS_TBN_GAIN<-1 ) || (obs[nobs].OBS_TBN_GAIN>30) ) {   
#endif
          printf("[%d/%d] FATAL: OBS_TBN_GAIN out of range\n",MT_TPSS,getpid());  
          return;
          }
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:                                                                                            break;
      case TPSS_PL_KEYWORD_MISMATCH:                                                                               break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
      }

    sprintf(keyword,"OBS_DRX_GAIN");  obs[nobs].OBS_DRX_GAIN=-1; 
    while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
    switch (i) {
      case TPSS_PL_KEYWORD_MATCH:    
        printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
        sscanf(data,"%d",&(obs[nobs].OBS_DRX_GAIN));
        printf("...converts to %d\n",obs[nobs].OBS_DRX_GAIN);
        if ( ( obs[nobs].OBS_DRX_GAIN<-1 ) || (obs[nobs].OBS_DRX_GAIN>255) ) {   
          printf("[%d/%d] FATAL: OBS_DRX_GAIN out of range\n",MT_TPSS,getpid());  
          return;
          }
        strcpy(data,"");   
        break;
      case TPSS_PL_EOF:                                                                                            break;
      case TPSS_PL_KEYWORD_MISMATCH:                                                                               break;
      case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
      }



