
  strcpy(keyword,"FORMAT_VERSION");
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,data);  
      sscanf(data,"%d",&k);
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }
  if (k!=ME_SSMIF_FORMAT_VERSION) {
    printf("[%d/%d] FATAL: SSMIF FORMAT_VERSION==%d, whereas this code requires %d\n",
           MT_TPRS,getpid(),k,ME_SSMIF_FORMAT_VERSION);   
    exit(EXIT_FAILURE);
    }
  s.iFormatVersion = ME_SSMIF_FORMAT_VERSION;

  strcpy(keyword,"STATION_ID"); strcpy(s.sStationID,"");
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH:    
      //printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,data);  
      if (strlen(data)>2) {
        printf("[%d/%d] FATAL: STATION_ID='%s' is greater than 2 characters\n",MT_TPRS,getpid(),data);   
        exit(EXIT_FAILURE);
        }
      sprintf(s.sStationID,"%s",data);
      printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sStationID);
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }  

  strcpy(keyword,"GEO_N"); s.fGeoN=0.0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%lf",&(s.fGeoN));     
      if ( (s.fGeoN<-90.0) || (s.fGeoN>+90.0) ) {
        printf("[%d/%d] FATAL: GEO_N=%lf is invalid\n",MT_TPRS,getpid(),s.fGeoN);   
        exit(EXIT_FAILURE);
        }
      printf("[%d/%d] %s=%lf\n",MT_TPRS,getpid(),keyword,s.fGeoN);
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    } 

  strcpy(keyword,"GEO_E"); s.fGeoE=0.0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%lf",&(s.fGeoE));     
      if ( (s.fGeoE<-180.0) || (s.fGeoE>+180.0) ) {
        printf("[%d/%d] FATAL: GEO_E=%lf is invalid\n",MT_TPRS,getpid(),s.fGeoE);   
        exit(EXIT_FAILURE);
        }
      printf("[%d/%d] %s=%lf\n",MT_TPRS,getpid(),keyword,s.fGeoE);
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    } 

  strcpy(keyword,"GEO_EL"); s.fGeoEl=0.0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%lf",&(s.fGeoEl));     
      if (s.fGeoEl<0.0) {
        printf("[%d/%d] FATAL: GEO_EL=%lf is invalid\n",MT_TPRS,getpid(),s.fGeoEl);   
        exit(EXIT_FAILURE);
        }
      printf("[%d/%d] %s=%lf\n",MT_TPRS,getpid(),keyword,s.fGeoEl);
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    } 

  strcpy(keyword,"N_STD"); s.nStd=0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(s.nStd));     
      if ( (s.nStd<0) || (s.nStd>ME_MAX_NSTD) ) {
        printf("[%d/%d] FATAL: N_STD=%d is invalid\n",MT_TPRS,getpid(),s.nStd);   
        exit(EXIT_FAILURE);
        }
      printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.nStd);
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }

  /* reading stand positions */
  for ( iStd=0; iStd<s.nStd; iStd++ ) {

    sprintf(keyword,"STD_LX[%d]",iStd+1); s.fStdLx[iStd]=0.0;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%lf",&(s.fStdLx[iStd]));     
        printf("[%d/%d] %s=%lf\n",MT_TPRS,getpid(),keyword,s.fStdLx[iStd]);
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        

    sprintf(keyword,"STD_LY[%d]",iStd+1); s.fStdLy[iStd]=0.0;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%lf",&(s.fStdLy[iStd]));     
        printf("[%d/%d] %s=%lf\n",MT_TPRS,getpid(),keyword,s.fStdLy[iStd]);
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }    


    sprintf(keyword,"STD_LZ[%d]",iStd+1); s.fStdLz[iStd]=0.0;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%lf",&(s.fStdLz[iStd]));     
        printf("[%d/%d] %s=%lf\n",MT_TPRS,getpid(),keyword,s.fStdLz[iStd]);
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }

    } /* for ( iStd */

  /* reading ANT_STD[] */
  for ( iAnt=0; iAnt<(2*s.nStd); iAnt++ ) {

    sprintf(keyword,"ANT_STD[%d]",iAnt+1); s.iAntStd[iAnt]=(((iAnt+1)-1)/2)+1;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.iAntStd[iAnt]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if ( (s.iAntStd[iAnt]<1) || (s.iAntStd[iAnt]>s.nStd) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.iAntStd[iAnt]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iAntStd[iAnt]);    

    } /* for ( iAnt */

  /* reading ANT_ORIE[] */
  for ( iAnt=0; iAnt<(2*s.nStd); iAnt++ ) {

    sprintf(keyword,"ANT_ORIE[%d]",iAnt+1); s.iAntOrie[iAnt] = iAnt % 2;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.iAntOrie[iAnt]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if ( (s.iAntOrie[iAnt]<0) || (s.iAntOrie[iAnt]>1) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.iAntOrie[iAnt]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iAntOrie[iAnt]);    

    } /* for ( iAnt */

  /* reading ANT_STAT[] */
  for ( iAnt=0; iAnt<(2*s.nStd); iAnt++ ) {

    sprintf(keyword,"ANT_STAT[%d]",iAnt+1); s.iAntStat[iAnt] = 3;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.iAntStat[iAnt]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if ( (s.iAntStat[iAnt]<0) || (s.iAntStat[iAnt]>3) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.iAntStat[iAnt]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iAntStat[iAnt]);    

    } /* for ( iAnt */

  /* reading ANT_THETA[] */
  for ( iAnt=0; iAnt<(2*s.nStd); iAnt++ ) {

    sprintf(keyword,"ANT_THETA[%d]",iAnt+1); s.fAntTheta[iAnt] = 0.0;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%f",&(s.fAntTheta[iAnt]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if ( (s.fAntTheta[iAnt]<-90.0) || (s.fAntTheta[iAnt]>+90.0) ) {
      printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,s.fAntTheta[iAnt]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fAntTheta[iAnt]);    

    } /* for ( iAnt */

  /* reading ANT_PHI[] */
  for ( iAnt=0; iAnt<(2*s.nStd); iAnt++ ) {

    sprintf(keyword,"ANT_PHI[%d]",iAnt+1); s.fAntPhi[iAnt] = 0.0;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%f",&(s.fAntPhi[iAnt]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if ( (s.fAntPhi[iAnt]<=-180.0) || (s.fAntPhi[iAnt]>+180.0) ) {
      printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,s.fAntPhi[iAnt]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fAntPhi[iAnt]);    

    } /* for ( iAnt */

  /* reading ANT_DESI (no brackets) */
  sprintf(keyword,"ANT_DESI"); eAntDesi_default = 1;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(eAntDesi_default));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (eAntDesi_default<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,eAntDesi_default);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,eAntDesi_default); 

  /* reading ANT_DESI[] */
  for ( iAnt=0; iAnt<(2*s.nStd); iAnt++ ) {

    sprintf(keyword,"ANT_DESI[%d]",iAnt+1); s.eAntDesi[iAnt] = eAntDesi_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.eAntDesi[iAnt]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if (s.eAntDesi[iAnt]<0) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.eAntDesi[iAnt]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.eAntDesi[iAnt]);    

    } /* for ( iAnt */

  /* reading N_FEE */
  sprintf(keyword,"N_FEE"); s.nFEE = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  printf("<%s>\n",data);
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(s.nFEE));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.nFEE<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.nFEE);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.nFEE); 

  /* reading FEE_ID[] */
  for ( iFEE=0; iFEE<(s.nFEE); iFEE++ ) {

    sprintf(keyword,"FEE_ID[%d]",iFEE+1); sprintf(s.sFEEID[iFEE],"UNK");
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        if (strlen(data)>ME_MAX_FEEID_LENGTH) {
          printf("[%d/%d] FATAL: FEE_ID[%d]='%s' is greater than %d characters\n",
            MT_TPRS,getpid(),iFEE,data,ME_MAX_FEEID_LENGTH);   
          exit(EXIT_FAILURE);
          }
        sprintf(s.sFEEID[iFEE],"%s",data);     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sFEEID[iFEE]);    

    } /* for ( iFEE */

  /* reading FEE_STAT[] */
  for ( iFEE=0; iFEE<(s.nFEE); iFEE++ ) {

    sprintf(keyword,"FEE_STAT[%d]",iFEE+1); s.iFEEStat[iFEE]=3;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.iFEEStat[iFEE]));    
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }     
    if ( (s.iFEEStat[iFEE]<0) || (s.iFEEStat[iFEE]>3) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.iFEEStat[iFEE]);
      exit(EXIT_FAILURE);
      }    
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iFEEStat[iFEE]);    

    } /* for ( iFEE */

  /* reading FEE_DESI (no brackets) */
  sprintf(keyword,"FEE_DESI"); eFEEDesi_default = 1;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(eFEEDesi_default));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (eFEEDesi_default<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,eFEEDesi_default);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,eFEEDesi_default); 

  /* reading FEE_DESI[] */
  for ( iFEE=0; iFEE<s.nFEE; iFEE++ ) {

    sprintf(keyword,"FEE_DESI[%d]",iFEE+1); s.eFEEDesi[iFEE] = eFEEDesi_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.eFEEDesi[iFEE]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if (s.eFEEDesi[iFEE]<0) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.eFEEDesi[iFEE]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.eFEEDesi[iFEE]);    

    } /* for ( iFEE */

  /* reading FEE_GAI1 (no brackets) */
  sprintf(keyword,"FEE_GAI1"); fFEEGai1_default = 35.7;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%f",&(fFEEGai1_default));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (fFEEGai1_default<0.0) {
    printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,fFEEGai1_default);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,fFEEGai1_default); 

  /* reading FEE_GAI1[] */
  for ( iFEE=0; iFEE<s.nFEE; iFEE++ ) {

    sprintf(keyword,"FEE_GAI1[%d]",iFEE+1); s.fFEEGai1[iFEE] = fFEEGai1_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%f",&(s.fFEEGai1[iFEE]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if (s.fFEEGai1[iFEE]<0.0) {
      printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,s.fFEEGai1[iFEE]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fFEEGai1[iFEE]);    

    } /* for ( iFEE */

  /* reading FEE_GAI2 (no brackets) */
  sprintf(keyword,"FEE_GAI2"); fFEEGai2_default = 35.7;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%f",&(fFEEGai2_default));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (fFEEGai2_default<0.0) {
    printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,fFEEGai2_default);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,fFEEGai2_default); 

  /* reading FEE_GAI2[] */
  for ( iFEE=0; iFEE<s.nFEE; iFEE++ ) {

    sprintf(keyword,"FEE_GAI2[%d]",iFEE+1); s.fFEEGai2[iFEE] = fFEEGai2_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%f",&(s.fFEEGai2[iFEE]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if (s.fFEEGai2[iFEE]<0.0) {
      printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,s.fFEEGai2[iFEE]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fFEEGai2[iFEE]);    

    } /* for ( iFEE */
 
  /* reading FEE_ANT1[] */
  for ( iFEE=0; iFEE<s.nFEE; iFEE++ ) {

    sprintf(keyword,"FEE_ANT1[%d]",iFEE+1); s.iFEEAnt1[iFEE] = iFEE*2 + 1;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.iFEEAnt1[iFEE]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if ( (s.iFEEAnt1[iFEE]<0) || (s.iFEEAnt1[iFEE]>(2*s.nStd)) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.iFEEAnt1[iFEE]);
      exit(EXIT_FAILURE);
      } 
    /* check to make sure no other FEE is connected to this antenna */
    for ( k=0; k<iFEE; k++ ) {
      if (s.iFEEAnt1[k]==s.iFEEAnt1[iFEE]) {
        printf("[%d/%d] FATAL: %s=%d is invalid (already claimed by FEE %d)\n",MT_TPRS,getpid(),keyword,s.iFEEAnt1[iFEE],k+1);
        exit(EXIT_FAILURE);        
        }
      } /* for k */
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iFEEAnt1[iFEE]);    

    } /* for ( iFEE */

  /* reading FEE_ANT2[] */
  for ( iFEE=0; iFEE<s.nFEE; iFEE++ ) {

    sprintf(keyword,"FEE_ANT2[%d]",iFEE+1); s.iFEEAnt2[iFEE] = iFEE*2 + 2;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.iFEEAnt2[iFEE]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if ( (s.iFEEAnt2[iFEE]<0) || (s.iFEEAnt2[iFEE]>(2*s.nStd)) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.iFEEAnt2[iFEE]);
      exit(EXIT_FAILURE);
      } 
    /* check to make sure no other FEE is connected to this antenna */
    for ( k=0; k<s.nFEE; k++ ) {
      if (s.iFEEAnt1[k]==s.iFEEAnt2[iFEE]) {
        printf("[%d/%d] FATAL: %s=%d is invalid (already claimed by FEE %d)\n",MT_TPRS,getpid(),keyword,s.iFEEAnt2[iFEE],k+1);
        exit(EXIT_FAILURE);        
        }
      } /* for k */
    for ( k=0; k<iFEE; k++ ) {
      if (s.iFEEAnt2[k]==s.iFEEAnt2[iFEE]) {
        printf("[%d/%d] FATAL: %s=%d is invalid (already claimed by FEE %d)\n",MT_TPRS,getpid(),keyword,s.iFEEAnt2[iFEE],k+1);
        exit(EXIT_FAILURE);        
        }
      } /* for k */
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iFEEAnt2[iFEE]);    

    } /* for ( iFEE */

  /* reading FEE_RACK[] & FEE_PORT[] */
  for ( iFEE=0; iFEE<s.nFEE; iFEE++ ) {

    sprintf(keyword,"FEE_RACK[%d]",iFEE+1); s.iFEERack[iFEE] = 0;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.iFEERack[iFEE]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if ( (s.iFEERack[iFEE]<0) || (s.iFEERack[iFEE]>ME_MAX_RACK) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.iFEERack[iFEE]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iFEERack[iFEE]); 
   
    sprintf(keyword,"FEE_PORT[%d]",iFEE+1); s.iFEEPort[iFEE] = 0;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.iFEEPort[iFEE]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if ( (s.iFEEPort[iFEE]<0) || (s.iFEEPort[iFEE]>ME_MAX_PORT) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.iFEEPort[iFEE]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iFEEPort[iFEE]); 

    } /* for ( iFEE */

  /* reading N_RPD */
  sprintf(keyword,"N_RPD"); s.nRPD = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(s.nRPD));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.nRPD<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.nRPD);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.nRPD); 

  /* reading RPD_ID[] */
  for ( iRPD=0; iRPD<(s.nRPD); iRPD++ ) {

    sprintf(keyword,"RPD_ID[%d]",iRPD+1); sprintf(s.sRPDID[iRPD],"UNK");
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        if (strlen(data)>ME_MAX_RPDID_LENGTH) {
          printf("[%d/%d] FATAL: RPD_ID[%d]='%s' is greater than %d characters\n",
            MT_TPRS,getpid(),iRPD,data,ME_MAX_RPDID_LENGTH);   
          exit(EXIT_FAILURE);
          }
        sprintf(s.sRPDID[iRPD],"%s",data);     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sRPDID[iRPD]);    

    } /* for ( iRPD */

  /* reading RPD_STAT[] */
  for ( iRPD=0; iRPD<(s.nRPD); iRPD++ ) {

    sprintf(keyword,"RPD_STAT[%d]",iRPD+1); s.iRPDStat[iRPD] = 3;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.iRPDStat[iRPD]));  
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    if ( (s.iRPDStat[iRPD]<0) || (s.iRPDStat[iRPD]>3) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.iRPDStat[iRPD]);
      exit(EXIT_FAILURE);
      }   
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iRPDStat[iRPD]);    

    } /* for ( iRPD */

  /* reading RPD_DESI (no brackets) */
  sprintf(keyword,"RPD_DESI "); eRPDDesi_default = 1;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(eRPDDesi_default));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (eRPDDesi_default<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,eRPDDesi_default);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,eRPDDesi_default); 

  /* reading FEE_DESI[] */
  for ( iRPD=0; iRPD<s.nRPD; iRPD++ ) {

    sprintf(keyword,"RPD_DESI[%d]",iRPD+1); s.eRPDDesi[iRPD] = eRPDDesi_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.eRPDDesi[iRPD]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if (s.eRPDDesi[iRPD]<0) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.eRPDDesi[iRPD]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.eRPDDesi[iRPD]);    

    } /* for ( iRPD */

  /* reading RPD_LENG[] */
  for ( iRPD=0; iRPD<(s.nRPD); iRPD++ ) {

    sprintf(keyword,"RPD_LENG[%d]",iRPD+1); s.fRPDLeng[iRPD] = 0.0;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%f",&(s.fRPDLeng[iRPD]));  
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    if (s.fRPDLeng[iRPD]<0.0) {
      printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,s.fRPDLeng[iRPD]);
      exit(EXIT_FAILURE);
      }   
    printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fRPDLeng[iRPD]);    

    } /* for ( iRPD */

  /* reading RPD_VF (no brackets) */
  sprintf(keyword,"RPD_VF "); fRPDVF_default = 83.0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%f",&(fRPDVF_default));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if ( (fRPDVF_default<0.0) || (fRPDVF_default>100.0) ){
    printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,fRPDVF_default);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,fRPDVF_default);

  /* reading RPD_DD (no brackets) */
  sprintf(keyword,"RPD_DD "); fRPDDD_default = 2.4;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%f",&(fRPDDD_default));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (fRPDDD_default<0.0) {
    printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,fRPDDD_default);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,fRPDDD_default);

  /* reading RPD_A0 (no brackets) */
  sprintf(keyword,"RPD_A0 "); fRPDA0_default = 0.00428;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%f",&(fRPDA0_default));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (fRPDA0_default<0.0) {
    printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,fRPDA0_default);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,fRPDA0_default);

  /* reading RPD_A1 (no brackets) */
  sprintf(keyword,"RPD_A1 "); fRPDA1_default = 0.0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%f",&(fRPDA1_default));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,fRPDA1_default);

  /* reading RPD_FREF (no brackets) */
  sprintf(keyword,"RPD_FREF "); fRPDFref_default = 10.0e+6;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%f",&(fRPDFref_default));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (fRPDFref_default<0.0) {
    printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,fRPDFref_default);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,fRPDFref_default);

  /* reading RPD_STR (no brackets) */
  sprintf(keyword,"RPD_STR "); fRPDStr_default = 1.0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%f",&(fRPDStr_default));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (fRPDStr_default<0.0) {
    printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,fRPDStr_default);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,fRPDStr_default);

  /* reading RPD_*[] */
  for ( iRPD=0; iRPD<(s.nRPD); iRPD++ ) {

    sprintf(keyword,"RPD_VF[%d]",iRPD+1); s.fRPDVF[iRPD] = fRPDVF_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%f",&(s.fRPDVF[iRPD]));  
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    if ( (s.fRPDVF[iRPD]<0.0) || (s.fRPDVF[iRPD]>100.0) ) {
      printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,s.fRPDVF[iRPD]);
      exit(EXIT_FAILURE);
      }   
    printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fRPDVF[iRPD]);    

    sprintf(keyword,"RPD_DD[%d]",iRPD+1); s.fRPDDD[iRPD] = fRPDDD_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%f",&(s.fRPDDD[iRPD]));  
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    if ( s.fRPDDD[iRPD]<0.0 ) {
      printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,s.fRPDDD[iRPD]);
      exit(EXIT_FAILURE);
      }   
    printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fRPDDD[iRPD]);  

    sprintf(keyword,"RPD_A0[%d]",iRPD+1); s.fRPDA0[iRPD] = fRPDA0_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%f",&(s.fRPDA0[iRPD]));  
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    if ( s.fRPDA0[iRPD]<0.0 ) {
      printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,s.fRPDA0[iRPD]);
      exit(EXIT_FAILURE);
      }   
    printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fRPDA0[iRPD]);

    sprintf(keyword,"RPD_A1[%d]",iRPD+1); s.fRPDA1[iRPD] = fRPDA1_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%f",&(s.fRPDA1[iRPD]));  
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    if ( s.fRPDA1[iRPD]<0.0 ) {
      printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,s.fRPDA1[iRPD]);
      exit(EXIT_FAILURE);
      }   
    printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fRPDA1[iRPD]);

    sprintf(keyword,"RPD_FREF[%d]",iRPD+1); s.fRPDFref[iRPD] = fRPDFref_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%f",&(s.fRPDFref[iRPD]));  
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    if ( s.fRPDFref[iRPD]<0.0 ) {
      printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,s.fRPDFref[iRPD]);
      exit(EXIT_FAILURE);
      }   
    printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fRPDFref[iRPD]);

    sprintf(keyword,"RPD_STR[%d]",iRPD+1); s.fRPDStr[iRPD] = fRPDStr_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%f",&(s.fRPDStr[iRPD]));  
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    if ( s.fRPDStr[iRPD]<0.0 ) {
      printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,s.fRPDStr[iRPD]);
      exit(EXIT_FAILURE);
      }   
    printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fRPDStr[iRPD]);

    } /* for ( iRPD */

  /* reading RPD_ANT[] */
  for ( iRPD=0; iRPD<(s.nRPD); iRPD++ ) {

    sprintf(keyword,"RPD_ANT[%d]",iRPD+1); s.iRPDAnt[iRPD] = iRPD+1;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.iRPDAnt[iRPD]));  
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    if ( (s.iRPDAnt[iRPD]<(-2*s.nStd)) || (s.iRPDAnt[iRPD]>(2*s.nStd)) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.iRPDAnt[iRPD]);
      exit(EXIT_FAILURE);
      }   
    /* check for multiple assignments to same antenna */
    for (k=0;k<iRPD;k++) {
      if (s.iRPDAnt[k]==s.iRPDAnt[iRPD]) {
        printf("[%d/%d] FATAL: RPD_ANT[%d]==RPD_ANT[%d]==%d\n",MT_TPRS,getpid(),iRPD+1,k+1,s.iRPDAnt[iRPD]);
        exit(EXIT_FAILURE);
        }
      } /* for k */
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iRPDAnt[iRPD]);    

    } /* for ( iRPD */

  /* reading N_SEP */
  sprintf(keyword,"N_SEP"); s.nSEP = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(s.nSEP));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if ((s.nSEP<0) || (s.nSEP>ME_MAX_NSEP)) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.nSEP);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.nSEP); 

  /* reading SEP_ID[] */
  for ( iSEP=0; iSEP<(s.nSEP); iSEP++ ) {

    sprintf(keyword,"SEP_ID[%d]",iSEP+1); sprintf(s.sSEPID[iSEP],"UNK");
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        if (strlen(data)>ME_MAX_SEPID_LENGTH) {
          printf("[%d/%d] FATAL: SEP_ID[%d]='%s' is greater than %d characters\n",
            MT_TPRS,getpid(),iSEP,data,ME_MAX_SEPID_LENGTH);   
          exit(EXIT_FAILURE);
          }
        sprintf(s.sSEPID[iSEP],"%s",data);     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sSEPID[iSEP]);    

    } /* for ( iSEP */

  /* reading SEP_STAT[] */
  for ( iSEP=0; iSEP<(s.nSEP); iSEP++ ) {

    sprintf(keyword,"SEP_STAT[%d]",iSEP+1); s.iSEPStat[iSEP] = 3;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.iSEPStat[iSEP]));  
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    if ( (s.iSEPStat[iSEP]<0) || (s.iSEPStat[iSEP]>3) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.iSEPStat[iSEP]);
      exit(EXIT_FAILURE);
      }   
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iSEPStat[iSEP]);    

    } /* for ( iSEP */

  /* reading SEP_CABL[] */
  for ( iSEP=0; iSEP<(s.nSEP); iSEP++ ) {

    sprintf(keyword,"SEP_CABL[%d]",iSEP+1); sprintf(s.sSEPCabl[iSEP],"UNK");
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        if (strlen(data)>ME_MAX_SEPCABL_LENGTH) {
          printf("[%d/%d] FATAL: SEP_CABL[%d]='%s' is greater than %d characters\n",
            MT_TPRS,getpid(),iSEP,data,ME_MAX_SEPCABL_LENGTH);   
          exit(EXIT_FAILURE);
          }
        sprintf(s.sSEPCabl[iSEP],"%s",data);     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sSEPCabl[iSEP]);    

    } /* for ( iSEP */

  /* reading SEP_LENG (no brackets */
  sprintf(keyword,"SEP_LENG"); fSEPLeng_default = 0.0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%f",&(fSEPLeng_default));  
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                               break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }        
  if (fSEPLeng_default<0.0) {
    printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,fSEPLeng_default);
    exit(EXIT_FAILURE);
    }   
  printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,fSEPLeng_default); 

  /* reading SEP_LENG[] */
  for ( iSEP=0; iSEP<(s.nSEP); iSEP++ ) {

    sprintf(keyword,"SEP_LENG[%d]",iSEP+1); s.fSEPLeng[iSEP] = fSEPLeng_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%f",&(s.fSEPLeng[iSEP]));  
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    if (s.fSEPLeng[iSEP]<0.0)  {
      printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,s.fSEPLeng[iSEP]);
      exit(EXIT_FAILURE);
      }   
    printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fSEPLeng[iSEP]);    

    } /* for ( iSEP */

  /* reading SEP_DESI (no brackets */
  sprintf(keyword,"SEP_DESI"); eSEPDesi_default = 1;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(eSEPDesi_default));  
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }        
  if ( (eSEPDesi_default<0) || (eSEPDesi_default>3) ) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,eSEPDesi_default);
    exit(EXIT_FAILURE);
    }   
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,eSEPDesi_default); 

  /* reading SEP_DESI[] */
  for ( iSEP=0; iSEP<s.nSEP; iSEP++ ) {

    sprintf(keyword,"SEP_DESI[%d]",iSEP+1); s.eSEPDesi[iSEP] = eSEPDesi_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.eSEPDesi[iSEP]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if (s.eSEPDesi[iSEP]<0) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.eSEPDesi[iSEP]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.eSEPDesi[iSEP]);    

    } /* for ( iSEP */

  /* reading SEP_GAIN (no brackets */
  sprintf(keyword,"SEP_GAIN"); fSEPGain_default = 0.0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%f",&(fSEPGain_default));  
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }        
  printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,fSEPGain_default); 

  /* reading SEP_GAIN[] */
  for ( iSEP=0; iSEP<s.nSEP; iSEP++ ) {

    sprintf(keyword,"SEP_GAIN[%d]",iSEP+1); s.fSEPGain[iSEP] = fSEPGain_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%f",&(s.fSEPGain[iSEP]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fSEPGain[iSEP]);    

    } /* for ( iSEP */

  /* reading SEP_GAIN[] */
  for ( iSEP=0; iSEP<s.nSEP; iSEP++ ) {

    sprintf(keyword,"SEP_ANT[%d]",iSEP+1); s.iSEPAnt[iSEP] = iSEP+1;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.iSEPAnt[iSEP]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    /* check for multiple assignments to same antenna */
    for (k=0;k<iSEP;k++) {
      if (s.iSEPAnt[k]==s.iSEPAnt[iSEP]) {
        printf("[%d/%d] FATAL: SEP_ANT[%d]==SEP_ANT[%d]==%d\n",MT_TPRS,getpid(),iSEP+1,k+1,s.iSEPAnt[iSEP]);
        exit(EXIT_FAILURE);
        }
      } /* for k */
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iSEPAnt[iSEP]);    

    } /* for ( iSEP */

  /* reading N_ARB */
  sprintf(keyword,"N_ARB"); s.nARB = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(s.nARB));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if ((s.nARB<0) || (s.nARB>ME_MAX_NARB)) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.nARB);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.nARB); 

  /* reading N_ARBCH */
  sprintf(keyword,"N_ARBCH"); s.nARBCH = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(s.nARBCH));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if ((s.nARBCH<0) || (s.nARBCH>ME_MAX_NARBCH)) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.nARBCH);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.nARBCH); 

  /* reading ARB_ID[] */
  for ( iARB=0; iARB<(s.nARB); iARB++ ) {

    sprintf(keyword,"ARB_ID[%d]",iARB+1); sprintf(s.sARBID[iARB],"UNK");
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        if (strlen(data)>ME_MAX_ARBID_LENGTH) {
          printf("[%d/%d] FATAL: ARB_ID[%d]='%s' is greater than %d characters\n",
            MT_TPRS,getpid(),iARB,data,ME_MAX_ARBID_LENGTH);   
          exit(EXIT_FAILURE);
          }
        sprintf(s.sARBID[iARB],"%s",data);     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sARBID[iARB]);    

    } /* for ( iARB */

  /* reading ARB_SLOT[] */
  for ( iARB=0; iARB<(s.nARB); iARB++ ) {

    sprintf(keyword,"ARB_SLOT[%d]",iARB+1); s.iARBSlot[iARB]=0;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.iARBSlot[iARB]));     
        if (s.iARBSlot[iARB]<0) {
          printf("[%d/%d] FATAL: ARB_SLOT[%d]=%d invalid\n",MT_TPRS,getpid(),iARB,s.iARBSlot[iARB]);   
          exit(EXIT_FAILURE);
          }
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iARBSlot[iARB]);    

    } /* for ( iARB */

  /* reading ARB_DESI (no brackets */
  sprintf(keyword,"ARB_DESI "); eARBDesi_default = 1;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(eARBDesi_default));  
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }        
  if ( (eARBDesi_default<0) || (eARBDesi_default>3) ) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,eARBDesi_default);
    exit(EXIT_FAILURE);
    }   
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,eARBDesi_default); 

  /* reading ARB_DESI[] */
  for ( iARB=0; iARB<s.nARB; iARB++ ) {

    sprintf(keyword,"ARB_DESI[%d]",iARB+1); s.eARBDesi[iARB] = eARBDesi_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.eARBDesi[iARB]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if (s.eARBDesi[iARB]<0) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.eARBDesi[iARB]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.eARBDesi[iARB]);    

    } /* for ( iARB */

  /* reading ARB_RACK[] & ARB_PORT[] */
  for ( iARB=0; iARB<s.nARB; iARB++ ) {

    sprintf(keyword,"ARB_RACK[%d]",iARB+1); s.iARBRack[iARB] = 0;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.iARBRack[iARB]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if ( (s.iARBRack[iARB]<0) || (s.iARBRack[iARB]>ME_MAX_RACK) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.iARBRack[iARB]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iARBRack[iARB]); 
   
    sprintf(keyword,"ARB_PORT[%d]",iARB+1); s.iARBPort[iARB] = 0;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.iARBPort[iARB]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if ( (s.iARBPort[iARB]<0) || (s.iARBPort[iARB]>ME_MAX_PORT) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.iARBPort[iARB]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iARBPort[iARB]); 

    } /* for ( iARB */

  /* reading ARB_STAT[] */
  for ( iARB=0; iARB<(s.nARB); iARB++ ) {
    for (k=0;k<s.nARBCH;k++) {

      sprintf(keyword,"ARB_STAT[%d][%d]",iARB+1,k+1); s.eARBStat[iARB][k] = 3;
      while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
      switch (i) {
        case MERS_PL_KEYWORD_MATCH: 
          sscanf(data,"%d",&(s.eARBStat[iARB][k]));  
          strcpy(data,"");  
          break;
        case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
        case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
        case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
        }        
      if ( (s.eARBStat[iARB][k]<0) || (s.eARBStat[iARB][k]>3) ) {
        printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.eARBStat[iARB][k]);
        exit(EXIT_FAILURE);
        }   
      printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.eARBStat[iARB][k]);    

      } /* for (k */
    } /* for ( iARB */

  /* reading ARB_GAIN (no brackets */
  sprintf(keyword,"ARB_GAIN"); fARBGain_default = 67.0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%f",&(fARBGain_default));  
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }        
  printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,fARBGain_default); 

  /* reading ARB_GAIN[] */
  for ( iARB=0; iARB<s.nARB; iARB++ ) {
    for (k=0;k<s.nARBCH;k++) {

      sprintf(keyword,"ARB_GAIN[%d][%d]",iARB+1,k+1); s.fARBGain[iARB][k] = fARBGain_default;
      while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
      switch (i) {
        case MERS_PL_KEYWORD_MATCH: 
          sscanf(data,"%f",&(s.fARBGain[iARB][k]));     
          strcpy(data,"");  
          break;
        case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
        case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
        case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
        }       
      printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fARBGain[iARB][k]);   
 
      } /* for (k */
    } /* for ( iARB */

  /* reading ARB_ANT[] */
  for ( iARB=0; iARB<s.nARB; iARB++ ) {
    for (k=0;k<s.nARBCH;k++) {

      sprintf(keyword,"ARB_ANT[%d][%d]",iARB+1,k+1); s.iARBAnt[iARB][k] = 0;
      while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
      switch (i) {
        case MERS_PL_KEYWORD_MATCH: 
          sscanf(data,"%d",&(s.iARBAnt[iARB][k]));
          if ( (s.iARBAnt[iARB][k]<(-2*s.nStd)) || (s.iARBAnt[iARB][k]>2*s.nStd) ) {
            printf("[%d/%d] FATAL: s.iARBAnt[%d][%d]=%d is invalid\n",MT_TPRS,getpid(),iARB+1,k+1,s.iARBAnt[iARB][k]);   
            exit(EXIT_FAILURE); 
            }     
          strcpy(data,"");  
          break;
        case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
        case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
        case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
        }       
      printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iARBAnt[iARB][k]);   
 
      } /* for (k */
    } /* for ( iARB */

  /* reading ARB_IN[] */
  for ( iARB=0; iARB<s.nARB; iARB++ ) {
    for (k=0;k<s.nARBCH;k++) {

      sprintf(keyword,"ARB_IN[%d][%d]",iARB+1,k+1); sprintf(s.sARBIN[iARB][k],"UNK");
      while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
      switch (i) {
        case MERS_PL_KEYWORD_MATCH: 
          if (strlen(data)>ME_MAX_ARBID_LENGTH) {
            printf("[%d/%d] FATAL: ARB_IN[%d]='%s' is greater than %d characters\n",
              MT_TPRS,getpid(),iARB,data,ME_MAX_ARBID_LENGTH);   
            exit(EXIT_FAILURE);
            }
          sprintf(s.sARBIN[iARB][k],"%s",data);     
          strcpy(data,"");  
          break;
        case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
        case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
        case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
        }        
      printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sARBIN[iARB][k]);   
 
      } /* for (k */
    } /* for ( iARB */

  /* reading ARB_OUT[] */
  for ( iARB=0; iARB<s.nARB; iARB++ ) {
    for (k=0;k<s.nARBCH;k++) {

      sprintf(keyword,"ARB_OUT[%d][%d]",iARB+1,k+1); sprintf(s.sARBOUT[iARB][k],"UNK");
      while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
      switch (i) {
        case MERS_PL_KEYWORD_MATCH: 
          if (strlen(data)>ME_MAX_ARBID_LENGTH) {
            printf("[%d/%d] FATAL: ARB_OUT[%d]='%s' is greater than %d characters\n",
              MT_TPRS,getpid(),iARB,data,ME_MAX_ARBID_LENGTH);   
            exit(EXIT_FAILURE);
            }
          sprintf(s.sARBOUT[iARB][k],"%s",data);     
          strcpy(data,"");  
          break;
        case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
        case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
        case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
        }        
      printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sARBOUT[iARB][k]);   
 
      } /* for (k */
    } /* for ( iARB */

#if defined(USE_ADP) && USE_ADP
  /* reading N_ROACH */
  sprintf(keyword,"N_ROACH"); s.nRoach = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(s.nRoach));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if ((s.nRoach<0) || (s.nRoach>ME_MAX_NROACH)) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.nRoach);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.nRoach); 

  /* reading N_ROACHCH */
  sprintf(keyword,"N_ROACHCH"); s.nRoachCh = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(s.nRoachCh));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if ((s.nRoachCh<0) || (s.nRoachCh>ME_MAX_NROACHCH)) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.nRoachCh);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.nRoachCh); 

  /* reading ROACH_ID[] */
  for ( iDP1=0; iDP1<s.nRoach; iDP1++ ) {

    sprintf(keyword,"ROACH_ID[%d]",iDP1+1); sprintf(s.sRoachID[iDP1],"UNK");
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        if (strlen(data)>ME_MAX_ROACHID_LENGTH) {
          printf("[%d/%d] FATAL: ROACH_ID[%d]='%s' is greater than %d characters\n",
            MT_TPRS,getpid(),iDP1,data,ME_MAX_ROACHID_LENGTH);   
          exit(EXIT_FAILURE);
          }
        sprintf(s.sRoachID[iDP1],"%s",data);     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sRoachID[iDP1]);   
 
    } /* for ( iDP1 */

  /* reading ROACH_SLOT[] */
  for ( iDP1=0; iDP1<s.nRoach; iDP1++ ) {

    sprintf(keyword,"ROACH_SLOT[%d]",iDP1+1); sprintf(s.sRoachSlot[iDP1],"UNK");
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        if (strlen(data)>ME_MAX_ROACHID_LENGTH) {
          printf("[%d/%d] FATAL: ROACH_SLOT[%d]='%s' is greater than %d characters\n",
            MT_TPRS,getpid(),iDP1,data,ME_MAX_ROACHID_LENGTH);   
          exit(EXIT_FAILURE);
          }
        sprintf(s.sRoachSlot[iDP1],"%s",data);     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sRoachSlot[iDP1]);   
 
    } /* for ( iDP1 */

  /* reading ROACH_DESI[] */
  for ( iDP1=0; iDP1<s.nRoach; iDP1++ ) {

    sprintf(keyword,"ROACH_DESI[%d]",iDP1+1); s.eRoachDesi[iDP1] = 1;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.eRoachDesi[iDP1]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if (s.eRoachDesi[iDP1]<0) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.eRoachDesi[iDP1]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.eRoachDesi[iDP1]);    

    } /* for ( iDP1 */

  /* reading ROACH_STAT[] */
  for ( iDP1=0; iDP1<(s.nRoach); iDP1++ ) {
    for (k=0;k<s.nRoachCh;k++) {

      sprintf(keyword,"ROACH_STAT[%d][%d]",iDP1+1,k+1); s.eRoachStat[iDP1][k] = 3;
      while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
      switch (i) {
        case MERS_PL_KEYWORD_MATCH: 
          sscanf(data,"%d",&(s.eRoachStat[iDP1][k]));  
          strcpy(data,"");  
          break;
        case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
        case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
        case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
        }        
      if ( (s.eRoachStat[iDP1][k]<0) || (s.eRoachStat[iDP1][k]>3) ) {
        printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.eRoachStat[iDP1][k]);
        exit(EXIT_FAILURE);
        }   
      printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.eRoachStat[iDP1][k]);    

      } /* for (k */
    } /* for ( iDP1 */

  /* reading ROACH_INR[] */
  for ( iDP1=0; iDP1<s.nRoach; iDP1++ ) {
    for (k=0;k<s.nRoachCh;k++) {

      sprintf(keyword,"ROACH_INR[%d][%d]",iDP1+1,k+1); sprintf(s.sRoachINR[iDP1][k],"UNK");
      while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
      switch (i) {
        case MERS_PL_KEYWORD_MATCH: 
          if (strlen(data)>ME_MAX_ROACHID_LENGTH) {
            printf("[%d/%d] FATAL: ROACH_INR[%d]='%s' is greater than %d characters\n",
              MT_TPRS,getpid(),iDP1,data,ME_MAX_ROACHID_LENGTH);   
            exit(EXIT_FAILURE);
            }
          sprintf(s.sRoachINR[iDP1][k],"%s",data);     
          strcpy(data,"");  
          break;
        case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
        case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
        case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
        }        
      printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sRoachINR[iDP1][k]);   
 
      } /* for (k */
    } /* for ( iDP1 */

  /* reading ROACH_INC[] */
  for ( iDP1=0; iDP1<s.nRoach; iDP1++ ) {
    for (k=0;k<s.nRoachCh;k++) {

      sprintf(keyword,"ROACH_INC[%d][%d]",iDP1+1,k+1); sprintf(s.sRoachINC[iDP1][k],"UNK");
      while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
      switch (i) {
        case MERS_PL_KEYWORD_MATCH: 
          if (strlen(data)>ME_MAX_ROACHID_LENGTH) {
            printf("[%d/%d] FATAL: ROACH_INC[%d]='%s' is greater than %d characters\n",
              MT_TPRS,getpid(),iDP1,data,ME_MAX_ROACHID_LENGTH);   
            exit(EXIT_FAILURE);
            }
          sprintf(s.sRoachINC[iDP1][k],"%s",data);     
          strcpy(data,"");  
          break;
        case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
        case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
        case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
        }        
      printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sRoachINC[iDP1][k]);   
 
      } /* for (k */
    } /* for ( iDP1 */

  /* reading ROACH_ANT[][] */
  for ( iDP1=0; iDP1<s.nRoach; iDP1++ ) {
    for (k=0;k<s.nRoachCh;k++) {

      sprintf(keyword,"ROACH_ANT[%d][%d]",iDP1+1,k+1); s.iRoachAnt[iDP1][k] = 0;
      while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
      switch (i) {
        case MERS_PL_KEYWORD_MATCH: 
          sscanf(data,"%d",&(s.iRoachAnt[iDP1][k]));
          if ( (s.iRoachAnt[iDP1][k]<0) || (s.iRoachAnt[iDP1][k]>2*s.nStd) ) {
            printf("[%d/%d] FATAL: ROACH_ANT[%d][%d]=%d is invalid\n",MT_TPRS,getpid(),iDP1+1,k+1,s.iRoachAnt[iDP1][k]);   
            exit(EXIT_FAILURE); 
            }     
          strcpy(data,"");  
          break;
        case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
        case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
        case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
        }       
      printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iRoachAnt[iDP1][k]);   
 
      } /* for (k */
    } /* for ( iDP1 */
    
  /* reading N_SERVER */
  sprintf(keyword,"N_SERVER"); s.nServer = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(s.nServer));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if ((s.nServer<0) || (s.nServer>ME_MAX_NSERVER)) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.nServer);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.nServer); 

  /* reading SERVER_ID[] */
  for ( iDP2=0; iDP2<s.nServer; iDP2++ ) {

    sprintf(keyword,"SERVER_ID[%d]",iDP2+1); sprintf(s.sServerID[iDP2],"UNK");
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        if (strlen(data)>ME_MAX_SERVERID_LENGTH) {
          printf("[%d/%d] FATAL: SERVER_ID[%d]='%s' is greater than %d characters\n",
            MT_TPRS,getpid(),iDP2+1,data,ME_MAX_SERVERID_LENGTH);   
          exit(EXIT_FAILURE);
          }
        sprintf(s.sServerID[iDP2],"%s",data);     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sServerID[iDP2]);   
 
    } /* for ( iDP2 */

  /* reading SERVER_SLOT[] */
  for ( iDP2=0; iDP2<s.nServer; iDP2++ ) {

    sprintf(keyword,"SERVER_SLOT[%d]",iDP2+1); sprintf(s.sServerSlot[iDP2],"UNK");
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        if (strlen(data)>ME_MAX_SERVERID_LENGTH) {
          printf("[%d/%d] FATAL: SERVER_SLOT[%d]='%s' is greater than %d characters\n",
            MT_TPRS,getpid(),iDP2+1,data,ME_MAX_SERVERID_LENGTH);   
          exit(EXIT_FAILURE);
          }
        sprintf(s.sServerSlot[iDP2],"%s",data);     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sServerSlot[iDP2]);   
 
    } /* for ( iDP2 */

  /* reading SERVER_STAT[] */
  for ( iDP2=0; iDP2<(s.nServer); iDP2++ ) {

    sprintf(keyword,"SERVER_STAT[%d]",iDP2+1); s.eServerStat[iDP2] = 3;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.eServerStat[iDP2]));  
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    if ( (s.eServerStat[iDP2]<0) || (s.eServerStat[iDP2]>3) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.eServerStat[iDP2]);
      exit(EXIT_FAILURE);
      }   
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.eServerStat[iDP2]);    

    } /* for ( iDP2 */

  /* reading SERVER_DESI[] */
  for ( iDP2=0; iDP2<s.nServer; iDP2++ ) {

    sprintf(keyword,"SERVER_DESI[%d]",iDP2+1); s.eServerDesi[iDP2] = 1;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.eServerDesi[iDP2]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if (s.eServerDesi[iDP2]<0) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.eServerDesi[iDP2]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.eServerDesi[iDP2]);    

    } /* for ( iDP2 */
#else
  
  /* reading N_DP1 */
  sprintf(keyword,"N_DP1"); s.nDP1 = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(s.nDP1));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if ((s.nDP1<0) || (s.nDP1>ME_MAX_NDP1)) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.nDP1);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.nDP1); 

  /* reading N_DP1CH */
  sprintf(keyword,"N_DP1CH"); s.nDP1Ch = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(s.nDP1Ch));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if ((s.nDP1Ch<0) || (s.nDP1Ch>ME_MAX_NDP1CH)) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.nDP1Ch);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.nDP1Ch); 

  /* reading DP1_ID[] */
  for ( iDP1=0; iDP1<s.nDP1; iDP1++ ) {

    sprintf(keyword,"DP1_ID[%d]",iDP1+1); sprintf(s.sDP1ID[iDP1],"UNK");
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        if (strlen(data)>ME_MAX_DP1ID_LENGTH) {
          printf("[%d/%d] FATAL: DP1_ID[%d]='%s' is greater than %d characters\n",
            MT_TPRS,getpid(),iDP1,data,ME_MAX_DP1ID_LENGTH);   
          exit(EXIT_FAILURE);
          }
        sprintf(s.sDP1ID[iDP1],"%s",data);     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sDP1ID[iDP1]);   
 
    } /* for ( iDP1 */

  /* reading DP1_SLOT[] */
  for ( iDP1=0; iDP1<s.nDP1; iDP1++ ) {

    sprintf(keyword,"DP1_SLOT[%d]",iDP1+1); sprintf(s.sDP1Slot[iDP1],"UNK");
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        if (strlen(data)>ME_MAX_DP1ID_LENGTH) {
          printf("[%d/%d] FATAL: DP1_SLOT[%d]='%s' is greater than %d characters\n",
            MT_TPRS,getpid(),iDP1,data,ME_MAX_DP1ID_LENGTH);   
          exit(EXIT_FAILURE);
          }
        sprintf(s.sDP1Slot[iDP1],"%s",data);     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sDP1Slot[iDP1]);   
 
    } /* for ( iDP1 */

  /* reading DP1_DESI[] */
  for ( iDP1=0; iDP1<s.nDP1; iDP1++ ) {

    sprintf(keyword,"DP1_DESI[%d]",iDP1+1); s.eDP1Desi[iDP1] = 1;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.eDP1Desi[iDP1]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if (s.eDP1Desi[iDP1]<0) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.eDP1Desi[iDP1]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.eDP1Desi[iDP1]);    

    } /* for ( iDP1 */

  /* reading DP1_STAT[] */
  for ( iDP1=0; iDP1<(s.nDP1); iDP1++ ) {
    for (k=0;k<s.nDP1Ch;k++) {

      sprintf(keyword,"DP1_STAT[%d][%d]",iDP1+1,k+1); s.eDP1Stat[iDP1][k] = 3;
      while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
      switch (i) {
        case MERS_PL_KEYWORD_MATCH: 
          sscanf(data,"%d",&(s.eDP1Stat[iDP1][k]));  
          strcpy(data,"");  
          break;
        case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
        case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
        case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
        }        
      if ( (s.eDP1Stat[iDP1][k]<0) || (s.eDP1Stat[iDP1][k]>3) ) {
        printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.eDP1Stat[iDP1][k]);
        exit(EXIT_FAILURE);
        }   
      printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.eDP1Stat[iDP1][k]);    

      } /* for (k */
    } /* for ( iDP1 */

  /* reading DP1_INR[] */
  for ( iDP1=0; iDP1<s.nDP1; iDP1++ ) {
    for (k=0;k<s.nDP1Ch;k++) {

      sprintf(keyword,"DP1_INR[%d][%d]",iDP1+1,k+1); sprintf(s.sDP1INR[iDP1][k],"UNK");
      while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
      switch (i) {
        case MERS_PL_KEYWORD_MATCH: 
          if (strlen(data)>ME_MAX_DP1ID_LENGTH) {
            printf("[%d/%d] FATAL: DP1_INR[%d]='%s' is greater than %d characters\n",
              MT_TPRS,getpid(),iDP1,data,ME_MAX_DP1ID_LENGTH);   
            exit(EXIT_FAILURE);
            }
          sprintf(s.sDP1INR[iDP1][k],"%s",data);     
          strcpy(data,"");  
          break;
        case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
        case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
        case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
        }        
      printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sDP1INR[iDP1][k]);   
 
      } /* for (k */
    } /* for ( iDP1 */

  /* reading DP1_INC[] */
  for ( iDP1=0; iDP1<s.nDP1; iDP1++ ) {
    for (k=0;k<s.nDP1Ch;k++) {

      sprintf(keyword,"DP1_INC[%d][%d]",iDP1+1,k+1); sprintf(s.sDP1INC[iDP1][k],"UNK");
      while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
      switch (i) {
        case MERS_PL_KEYWORD_MATCH: 
          if (strlen(data)>ME_MAX_DP1ID_LENGTH) {
            printf("[%d/%d] FATAL: DP1_INC[%d]='%s' is greater than %d characters\n",
              MT_TPRS,getpid(),iDP1,data,ME_MAX_DP1ID_LENGTH);   
            exit(EXIT_FAILURE);
            }
          sprintf(s.sDP1INC[iDP1][k],"%s",data);     
          strcpy(data,"");  
          break;
        case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
        case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
        case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
        }        
      printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sDP1INC[iDP1][k]);   
 
      } /* for (k */
    } /* for ( iDP1 */

  /* reading DP1_ANT[][] */
  for ( iDP1=0; iDP1<s.nDP1; iDP1++ ) {
    for (k=0;k<s.nDP1Ch;k++) {

      sprintf(keyword,"DP1_ANT[%d][%d]",iDP1+1,k+1); s.iDP1Ant[iDP1][k] = 0;
      while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
      switch (i) {
        case MERS_PL_KEYWORD_MATCH: 
          sscanf(data,"%d",&(s.iDP1Ant[iDP1][k]));
          if ( (s.iDP1Ant[iDP1][k]<0) || (s.iDP1Ant[iDP1][k]>2*s.nStd) ) {
            printf("[%d/%d] FATAL: s.iDP1Ant[%d][%d]=%d is invalid\n",MT_TPRS,getpid(),iDP1+1,k+1,s.iDP1Ant[iDP1][k]);   
            exit(EXIT_FAILURE); 
            }     
          strcpy(data,"");  
          break;
        case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
        case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
        case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
        }       
      printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iDP1Ant[iDP1][k]);   
 
      } /* for (k */
    } /* for ( iDP1 */

  /* reading N_DP2 */
  sprintf(keyword,"N_DP2"); s.nDP2 = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(s.nDP2));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if ((s.nDP2<0) || (s.nDP2>ME_MAX_NDP2)) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.nDP2);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.nDP2); 

  /* reading DP2_ID[] */
  for ( iDP2=0; iDP2<s.nDP2; iDP2++ ) {

    sprintf(keyword,"DP2_ID[%d]",iDP2+1); sprintf(s.sDP2ID[iDP2],"UNK");
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        if (strlen(data)>ME_MAX_DP2ID_LENGTH) {
          printf("[%d/%d] FATAL: DP2_ID[%d]='%s' is greater than %d characters\n",
            MT_TPRS,getpid(),iDP2+1,data,ME_MAX_DP2ID_LENGTH);   
          exit(EXIT_FAILURE);
          }
        sprintf(s.sDP2ID[iDP2],"%s",data);     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sDP2ID[iDP2]);   
 
    } /* for ( iDP2 */

  /* reading DP2_SLOT[] */
  for ( iDP2=0; iDP2<s.nDP2; iDP2++ ) {

    sprintf(keyword,"DP2_SLOT[%d]",iDP2+1); sprintf(s.sDP2Slot[iDP2],"UNK");
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        if (strlen(data)>ME_MAX_DP2ID_LENGTH) {
          printf("[%d/%d] FATAL: DP2_SLOT[%d]='%s' is greater than %d characters\n",
            MT_TPRS,getpid(),iDP2+1,data,ME_MAX_DP2ID_LENGTH);   
          exit(EXIT_FAILURE);
          }
        sprintf(s.sDP2Slot[iDP2],"%s",data);     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sDP2Slot[iDP2]);   
 
    } /* for ( iDP2 */

  /* reading DP2_STAT[] */
  for ( iDP2=0; iDP2<(s.nDP2); iDP2++ ) {

    sprintf(keyword,"DP2_STAT[%d]",iDP2+1); s.eDP2Stat[iDP2] = 3;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.eDP2Stat[iDP2]));  
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    if ( (s.eDP2Stat[iDP2]<0) || (s.eDP2Stat[iDP2]>3) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.eDP2Stat[iDP2]);
      exit(EXIT_FAILURE);
      }   
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.eDP2Stat[iDP2]);    

    } /* for ( iDP2 */

  /* reading DP2_DESI[] */
  for ( iDP2=0; iDP2<s.nDP2; iDP2++ ) {

    sprintf(keyword,"DP2_DESI[%d]",iDP2+1); s.eDP2Desi[iDP2] = 1;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.eDP2Desi[iDP2]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
    if (s.eDP2Desi[iDP2]<0) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.eDP2Desi[iDP2]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.eDP2Desi[iDP2]);    

    } /* for ( iDP2 */
#endif
    
  /* reading N_DR */
  sprintf(keyword,"N_DR"); s.nDR = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(s.nDR));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if ((s.nDR<0) || (s.nDR>ME_MAX_NDR)) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.nDR);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.nDR); 

  /* reading DR_STAT[] */
  for ( iDR=0; iDR<(s.nDR); iDR++ ) {

    sprintf(keyword,"DR_STAT[%d]",iDR+1); s.eDRStat[iDR] = 3;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.eDRStat[iDR]));  
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    if ( (s.eDRStat[iDR]<0) || (s.eDRStat[iDR]>3) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.eDRStat[iDR]);
      exit(EXIT_FAILURE);
      }   
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.eDRStat[iDR]);    

    } /* for ( iDR */

  /* reading DR_ID[] */
  for ( iDR=0; iDR<s.nDR; iDR++ ) {

    sprintf(keyword,"DR_ID[%d]",iDR+1); sprintf(s.sDRID[iDR],"UNK");
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        if (strlen(data)>ME_MAX_DRID_LENGTH) {
          printf("[%d/%d] FATAL: DR_ID[%d]='%s' is greater than %d characters\n",
            MT_TPRS,getpid(),iDR+1,data,ME_MAX_DRID_LENGTH);   
          exit(EXIT_FAILURE);
          }
        sprintf(s.sDRID[iDR],"%s",data);     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sDRID[iDR]);   
 
    } /* for ( iDR */

  /* reading DR_PC[] */
  for ( iDR=0; iDR<s.nDR; iDR++ ) {

    sprintf(keyword,"DR_PC[%d]",iDR+1); sprintf(s.sDRPC[iDR],"UNK");
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        if (strlen(data)>ME_MAX_DRID_LENGTH) {
          printf("[%d/%d] FATAL: DR_PC[%d]='%s' is greater than %d characters\n",
            MT_TPRS,getpid(),iDR+1,data,ME_MAX_DRID_LENGTH);   
          exit(EXIT_FAILURE);
          }
        sprintf(s.sDRPC[iDR],"%s",data);     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sDRPC[iDR]);   
 
    } /* for ( iDR */

  /* reading DR_DP[] */
  for ( iDR=0; iDR<s.nDR; iDR++ ) {

    sprintf(keyword,"DR_DP[%d]",iDR+1); s.iDRDP[iDR]=0;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.iDRDP[iDR]));
        if ( (s.iDRDP[iDR]<0) || (s.iDRDP[iDR]>5) ) {
          printf("[%d/%d] FATAL: DR_DP[%d]=%d not valid\n",MT_TPRS,getpid(),iDR+1,s.iDRDP[iDR]);   
          exit(EXIT_FAILURE);
          }    
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.iDRDP[iDR]);   
 
    } /* for ( iDR */

  /* reading N_PWR_RACK */
  sprintf(keyword,"N_PWR_RACK"); s.nPwrRack = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(s.nPwrRack));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if ((s.nPwrRack<0) || (s.nPwrRack>ME_MAX_RACK)) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.nPwrRack);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.nPwrRack); 

  /* reading N_PWR_PORT[] */
  for ( iRack=0; iRack<ME_MAX_RACK; iRack++ ) {

    sprintf(keyword,"N_PWR_PORT[%d]",iRack+1); s.nPwrPort[iRack]=0;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%d",&(s.nPwrPort[iRack]));
        if ( (s.nPwrPort[iRack]<0) || (s.nPwrPort[iRack]>ME_MAX_NPWRPORT) ) {
          printf("[%d/%d] FATAL: N_PWR_PORT[%d]=%d not valid\n",MT_TPRS,getpid(),iRack+1,s.nPwrPort[iRack]);   
          exit(EXIT_FAILURE);
          }    
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }        
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.nPwrPort[iRack]);   
 
    } /* for ( iDR */

  /* reading PWR_SS[][] and PWR_NAME[][] */
  for ( iRack=0; iRack<s.nPwrRack; iRack++ ) {
    for ( iPort=0; iPort<s.nPwrPort[iRack]; iPort++ ) {

      sprintf(keyword,"PWR_SS[%d][%d]",iRack+1,iPort+1); s.ePwrSS[iRack][iPort]=0;
      while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
      switch (i) {
        case MERS_PL_KEYWORD_MATCH: 
          s.ePwrSS[iRack][iPort]=LWA_getsid(data);
          if (s.ePwrSS[iRack][iPort]==0) {
            if (!strncmp(data,"UNK",3)) {
                sprintf(data,"%s","UNK"); /* make sure it's 3 characters */
              } else {
                printf("[%d/%d] FATAL: PWR_SS[%d][%d]='%s' not valid\n",MT_TPRS,getpid(),iRack+1,iPort+1,data);   
                exit(EXIT_FAILURE);
              }
            }   
          strcpy(data,"");  
          break;
        case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
        case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
        case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
        }        
      sprintf(sTemp,"%s",LWA_sid2str(s.ePwrSS[iRack][iPort]));
      if (!strncmp(sTemp,"XXX",3)) { sprintf(sTemp,"UNK"); }
      printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,sTemp);   
  
      sprintf(keyword,"PWR_NAME[%d][%d]",iRack+1,iPort+1); sprintf(s.sPwrName[iRack][iPort],"UNK");
      while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
      switch (i) {
        case MERS_PL_KEYWORD_MATCH:
          if (strlen(data)>ME_MAX_SSNAME_LENGTH) { 
            printf("[%d/%d] FATAL: PWR_NAME[%d][%d]='%s' more than %d characters\n",
              MT_TPRS,getpid(),iRack+1,iPort+1,data,ME_MAX_SSNAME_LENGTH);   
            exit(EXIT_FAILURE);
            } 
          switch (s.ePwrSS[iRack][iPort]) { 
            case 0: /* UNK */
              break;
            case LWA_SID_MCS:
              b = (!strncmp(data,"SCH",3)) || (!strncmp(data,"EXE",3)) || (!strncmp(data,"TP", 2)) ||  
                  (!strncmp(data,"CH", 2)) || (!strncmp(data,"GW ",2));
              if (!b) {
                printf("[%d/%d] FATAL: PWR_NAME='%s' not valid for PWR_SS[%d][%d]=`MCS'\n",
                  MT_TPRS,getpid(),data,iRack+1,iPort+1);   
                exit(EXIT_FAILURE);
                }
              break; 
            case LWA_SID_SHL:
              b = (!strncmp(data,"MCS",3));
              if (!b) {
                printf("[%d/%d] FATAL: PWR_NAME='%s' not valid for PWR_SS[%d][%d]='SHL'\n",
                  MT_TPRS,getpid(),data,iRack+1,iPort+1);   
                exit(EXIT_FAILURE);
                }
              break; 
            case LWA_SID_ASP:
              b = (!strncmp(data,"MCS",3)) || (!strncmp(data,"FEE",3)) || (!strncmp(data,"ARX",3)) ||  
                  (!strncmp(data,"FAN",3));
              if (!b) {
                printf("[%d/%d] FATAL: PWR_NAME='%s' not valid for PWR_SS[%d][%d]='ASP'\n",
                  MT_TPRS,getpid(),data,iRack+1,iPort+1);   
                exit(EXIT_FAILURE);
                }
              break; 
            case LWA_SID_DP_:
              b = (!strncmp(data,"MCS",3)) || (!strncmp(data,"DC1",3)) || (!strncmp(data,"DC1",2)) ||
                  (!strncmp(data,"FAN",2)) || (!strncmp(data,"SYN",2)) || (!strncmp(data,"SWI",2))  ;
              if (!b) {
                printf("[%d/%d] FATAL: PWR_NAME='%s' not valid for PWR_SS[%d][%d]='DP_'\n",
                  MT_TPRS,getpid(),data,iRack+1,iPort+1);   
                exit(EXIT_FAILURE);
                }
              break;
            case LWA_SID_ADP:
              b = (!strncmp(data,"MCS",3)) || (!strncmp(data,"DC1",3)) || (!strncmp(data,"DC1",2)) ||
                  (!strncmp(data,"FAN",2)) || (!strncmp(data,"SYN",2)) || (!strncmp(data,"SWI",2))  ;
              if (!b) {
                printf("[%d/%d] FATAL: PWR_NAME='%s' not valid for PWR_SS[%d][%d]='ADP'\n",
                  MT_TPRS,getpid(),data,iRack+1,iPort+1);   
                exit(EXIT_FAILURE);
                }
              break;
            case LWA_SID_DR1:
            case LWA_SID_DR2:
            case LWA_SID_DR3:
            case LWA_SID_DR4:
            case LWA_SID_DR5:
              b = (!strncmp(data,"PC", 2)) || (!strncmp(data,"DS1",3)) || (!strncmp(data,"DS2",3));  
              if (!b) {
                printf("[%d/%d] FATAL: PWR_NAME='%s' not valid for PWR_SS[%d][%d]='DR?'\n",
                  MT_TPRS,getpid(),data,iRack+1,iPort+1);   
                exit(EXIT_FAILURE);
                }
              break;
            default:
              printf("[%d/%d] FATAL: I don't recognize s.ePwrSS[%d][%d]=%d as a LWA_SID_ value\n",
                MT_TPRS,getpid(),iRack+1,iPort+1,s.ePwrSS[iRack][iPort]);   
              exit(EXIT_FAILURE);
              break;
            } /* switch  (s.ePwrSS[iRack][iPort]) */
          sprintf(s.sPwrName[iRack][iPort],"%s",data); 
          strcpy(data,"");  
                      break;
        case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
        case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
        case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
        }        
      printf("[%d/%d] %s='%s'\n",MT_TPRS,getpid(),keyword,s.sPwrName[iRack][iPort]);   

      } /* for ( iPort */
    } /* for ( iRack */

  /* reading MCS_CRA */
  sprintf(keyword,"MCS_CRA"); s.eCRA = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%d",&(s.eCRA));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: MERS_PL_KEYWORD_MISMATCH\n",MT_TPRS,getpid()); exit(EXIT_FAILURE); break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if ((s.eCRA<0) || (s.eCRA>1)) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.eCRA);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.eCRA); 

  /* reading PC_AXIS_TH */
  sprintf(keyword,"PC_AXIS_TH"); s.fPCAxisTh = 0.0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%f",&(s.fPCAxisTh));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }        
  printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fPCAxisTh); 

  /* reading PC_AXIS_PH */
  sprintf(keyword,"PC_AXIS_PH"); s.fPCAxisPh = 0.0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%f",&(s.fPCAxisPh));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  //if ((s.fPCAxisPh<0.0) || (s.fPCZenithTh>180.0)) {
  //  printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,s.fPCZenithTh);
  //  exit(EXIT_FAILURE);
  //  }  
  printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fPCAxisPh); 

  /* reading PC_ROT */
  sprintf(keyword,"PC_ROT"); s.fPCRot = 0.0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%f",&(s.fPCRot));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  //if ((s.fPCZenithPh<0.0) || (s.fPCZenithPh>=360.0)) {
  //  printf("[%d/%d] FATAL: %s=%f is invalid\n",MT_TPRS,getpid(),keyword,s.fPCZenithPh);
  //  exit(EXIT_FAILURE);
  //  }  
  printf("[%d/%d] %s=%f\n",MT_TPRS,getpid(),keyword,s.fPCRot); 

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  /* reading MRP_ASP */
  sprintf(keyword,"MRP_ASP"); s.settings.mrp_asp = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mrp_asp));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mrp_asp<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mrp_asp);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mrp_asp); 

  /* reading MRP_DP_ */
  sprintf(keyword,"MRP_DP_"); s.settings.mrp_dp = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mrp_dp));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mrp_dp<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mrp_dp);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mrp_dp); 

  /* reading MRP_DR1 */
  sprintf(keyword,"MRP_DR1"); s.settings.mrp_dr1 = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mrp_dr1));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mrp_dr1<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mrp_dr1);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mrp_dr1);

  /* reading MRP_DR2 */
  sprintf(keyword,"MRP_DR2"); s.settings.mrp_dr2 = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mrp_dr2));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mrp_dr2<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mrp_dr2);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mrp_dr2);

  /* reading MRP_DR3 */
  sprintf(keyword,"MRP_DR3"); s.settings.mrp_dr3 = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mrp_dr3));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mrp_dr3<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mrp_dr3);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mrp_dr3);

  /* reading MRP_DR4 */
  sprintf(keyword,"MRP_DR4"); s.settings.mrp_dr4 = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mrp_dr4));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mrp_dr4<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mrp_dr4);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mrp_dr4);

  /* reading MRP_DR5 */
  sprintf(keyword,"MRP_DR5"); s.settings.mrp_dr5 = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mrp_dr5));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mrp_dr5<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mrp_dr5);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mrp_dr5);

  /* reading MRP_SHL */
  sprintf(keyword,"MRP_SHL"); s.settings.mrp_shl = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mrp_shl));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mrp_shl<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mrp_shl);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mrp_shl);

  /* reading MRP_MCS */
  sprintf(keyword,"MRP_MCS"); s.settings.mrp_mcs = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mrp_mcs));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mrp_mcs<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mrp_mcs);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mrp_mcs);

  /* reading MUP_ASP */
  sprintf(keyword,"MUP_ASP"); s.settings.mup_asp = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mup_asp));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mup_asp<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mup_asp);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mup_asp); 

  /* reading MUP_DP_ */
  sprintf(keyword,"MUP_DP_"); s.settings.mup_dp = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mup_dp));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mup_dp<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mup_dp);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mup_dp); 

  /* reading MUP_DR1 */
  sprintf(keyword,"MUP_DR1"); s.settings.mup_dr1 = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mup_dr1));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mup_dr1<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mup_dr1);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mup_dr1);

  /* reading MUP_DR2 */
  sprintf(keyword,"MUP_DR2"); s.settings.mup_dr2 = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mup_dr2));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mup_dr2<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mup_dr2);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mup_dr2);

  /* reading MUP_DR3 */
  sprintf(keyword,"MUP_DR3"); s.settings.mup_dr3 = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mup_dr3));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mup_dr3<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mup_dr3);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mup_dr3);

  /* reading MUP_DR4 */
  sprintf(keyword,"MUP_DR4"); s.settings.mup_dr4 = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mup_dr4));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mup_dr4<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mup_dr4);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mup_dr4);

  /* reading MUP_DR5 */
  sprintf(keyword,"MUP_DR5"); s.settings.mup_dr5 = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mup_dr5));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mup_dr5<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mup_dr5);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mup_dr5);

  /* reading MUP_SHL */
  sprintf(keyword,"MUP_SHL"); s.settings.mup_shl = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mup_shl));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mup_shl<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mup_shl);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mup_shl);

  /* reading MUP_MCS */
  sprintf(keyword,"MUP_MCS"); s.settings.mup_mcs = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.mup_mcs));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if (s.settings.mup_mcs<0) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.mup_mcs);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.mup_mcs);

  /* reading FEE (no brackets) */
  sprintf(keyword,"FEE"); 
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(fee_default));  
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }        
  if ( (fee_default<0) || (fee_default>1) ) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,fee_default);
    exit(EXIT_FAILURE);
    }   
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,fee_default); 

  /* reading FEE[] */
  for ( iFEE=0; iFEE<s.nFEE; iFEE++ ) {

    sprintf(keyword,"FEE[%d]",iFEE+1); s.settings.fee[iFEE] = fee_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%hd",&(s.settings.fee[iFEE]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
  if ( (s.settings.fee[iFEE]<0) || (s.settings.fee[iFEE]>1) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.fee[iFEE]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.fee[iFEE]);    

    } /* for ( iFEE */

  /* reading ASP_FLT (no brackets) */
  sprintf(keyword,"ASP_FLT"); 
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(asp_flt_default));  
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }        
  if ((asp_flt_default<0) || (asp_flt_default>3) ) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,asp_flt_default);
    exit(EXIT_FAILURE);
    }   
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,asp_flt_default); 

  /* reading ASP_FLT[] */
  for ( iStd=0; iStd<s.nStd; iStd++ ) {

    sprintf(keyword,"ASP_FLT[%d]",iStd+1); s.settings.asp_flt[iStd] = asp_flt_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%hd",&(s.settings.asp_flt[iStd]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
  if ( (s.settings.asp_flt[iStd]<0) || (s.settings.asp_flt[iStd]>3) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.asp_flt[iStd]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.asp_flt[iStd]);    

    } /* for ( iStd */

  /* reading ASP_AT1 (no brackets) */
  sprintf(keyword,"ASP_AT1"); 
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(asp_at1_default));  
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }        
  if ((asp_at1_default<0) || (asp_at1_default>15) ) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,asp_at1_default);
    exit(EXIT_FAILURE);
    }   
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,asp_at1_default); 

  /* reading ASP_AT1[] */
  for ( iStd=0; iStd<s.nStd; iStd++ ) {

    sprintf(keyword,"ASP_AT1[%d]",iStd+1); s.settings.asp_at1[iStd] = asp_at1_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%hd",&(s.settings.asp_at1[iStd]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
  if ( (s.settings.asp_at1[iStd]<0) || (s.settings.asp_at1[iStd]>15) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.asp_at1[iStd]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.asp_at1[iStd]);    

    } /* for ( iStd */

  /* reading ASP_AT2 (no brackets) */
  sprintf(keyword,"ASP_AT2"); 
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(asp_at2_default));  
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }        
  if ((asp_at2_default<0) || (asp_at2_default>15) ) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,asp_at2_default);
    exit(EXIT_FAILURE);
    }   
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,asp_at2_default); 

  /* reading ASP_AT2[] */
  for ( iStd=0; iStd<s.nStd; iStd++ ) {

    sprintf(keyword,"ASP_AT2[%d]",iStd+1); s.settings.asp_at2[iStd] = asp_at2_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%hd",&(s.settings.asp_at2[iStd]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
  if ( (s.settings.asp_at2[iStd]<0) || (s.settings.asp_at2[iStd]>15) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.asp_at2[iStd]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.asp_at2[iStd]);    

    } /* for ( iStd */

  /* reading ASP_ATS (no brackets) */
  sprintf(keyword,"ASP_ATS"); 
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(asp_ats_default));  
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }        
  if ((asp_ats_default<0) || (asp_ats_default>15) ) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,asp_ats_default);
    exit(EXIT_FAILURE);
    }   
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,asp_ats_default); 

  /* reading ASP_ATS[] */
  for ( iStd=0; iStd<s.nStd; iStd++ ) {

    sprintf(keyword,"ASP_ATS[%d]",iStd+1); s.settings.asp_ats[iStd] = asp_ats_default;
    while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
    switch (i) {
      case MERS_PL_KEYWORD_MATCH: 
        sscanf(data,"%hd",&(s.settings.asp_ats[iStd]));     
        strcpy(data,"");  
        break;
      case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
      case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
      case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
      }       
  if ( (s.settings.asp_ats[iStd]<0) || (s.settings.asp_ats[iStd]>15) ) {
      printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.asp_ats[iStd]);
      exit(EXIT_FAILURE);
      } 
    printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.asp_ats[iStd]);    

    } /* for ( iStd */

#if defined(USE_ADP) && USE_ADP
  /* reading TBF_GAIN */
  sprintf(keyword,"TBF_GAIN"); s.settings.tbn_gain = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.tbf_gain));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if ((s.settings.tbf_gain<0) || (s.settings.tbf_gain>15)) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.tbf_gain);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.tbf_gain);
#endif
  
  /* reading TBN_GAIN */
  sprintf(keyword,"TBN_GAIN"); s.settings.tbn_gain = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.tbn_gain));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if ((s.settings.tbn_gain<0) || (s.settings.tbn_gain>30)) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.tbn_gain);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.tbn_gain);


  /* reading DRX_GAIN */
  sprintf(keyword,"DRX_GAIN"); s.settings.drx_gain = 0;
  while( (i=mers_parse_line(fp, keyword, data, MERS_VERBOSE)) == MERS_PL_BC_LINE ) { }
  switch (i) {
    case MERS_PL_KEYWORD_MATCH: 
      sscanf(data,"%hd",&(s.settings.drx_gain));     
      strcpy(data,"");  
      break;
    case MERS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected MERS_PL_EOF\n",MT_TPRS,getpid());   exit(EXIT_FAILURE); break;
    case MERS_PL_KEYWORD_MISMATCH:                                                                                           break;
    case MERS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: MERS_PL_OVERLONG_LINE\n",MT_TPRS,getpid());    exit(EXIT_FAILURE); break;
    }       
  if ((s.settings.drx_gain<0) || (s.settings.drx_gain>15)) {
    printf("[%d/%d] FATAL: %s=%d is invalid\n",MT_TPRS,getpid(),keyword,s.settings.drx_gain);
    exit(EXIT_FAILURE);
    } 
  printf("[%d/%d] %s=%d\n",MT_TPRS,getpid(),keyword,s.settings.drx_gain);
