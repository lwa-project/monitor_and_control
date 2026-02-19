
  strcpy(keyword,"SESSION_ID");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%u",&SESSION_ID); 
      printf("...converts to %u\n",SESSION_ID);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }


  strcpy(keyword,"SESSION_TITLE");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      strcpy(SESSION_TITLE,data);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_REMPI");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      strcpy(SESSION_REMPI,data);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_REMPO");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      strcpy(SESSION_REMPO,data);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); exit(EXIT_FAILURE); break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_CRA"); SESSION_CRA=0;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_CRA);
      printf("...converts to %d\n",SESSION_CRA);
      if ( (SESSION_CRA<0) || (SESSION_CRA>65535) ) {   
        printf("[%d/%d] FATAL: SESSION_CRA not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional keyword */                                                                    break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_DRX_BEAM"); SESSION_DRX_BEAM=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%hd",&SESSION_DRX_BEAM);
      printf("...converts to %hd\n",SESSION_DRX_BEAM);
      if (!( (SESSION_DRX_BEAM==-1) || ( (SESSION_DRX_BEAM>=1) && (SESSION_DRX_BEAM<=ME_MAX_NDPOUT-1) ) )) {   
        printf("[%d/%d] FATAL: SESSION_DRX_BEAM not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional keyword */                                                                    break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_SPC"); strcpy(SESSION_SPC,"");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      strcpy(SESSION_SPC,data);
      printf("...converts to '%s'\n",SESSION_SPC);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional keyword */                                                                    break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MRP_ASP"); SESSION_MRP_ASP=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data);   
      sscanf(data,"%d",&SESSION_MRP_ASP);
      printf("...converts to %d\n",SESSION_MRP_ASP);
      if (SESSION_MRP_ASP<-1) {   
        printf("[%d/%d] FATAL: SESSION_MRP_ASP not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }                 
      strcpy(data,"");  
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MRP_NDP"); SESSION_MRP_NDP=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data);
      sscanf(data,"%d",&SESSION_MRP_NDP);
      printf("...converts to %d\n",SESSION_MRP_NDP);
      if (SESSION_MRP_NDP<-1) {   
        printf("[%d/%d] FATAL: SESSION_MRP_NDP not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }
      strcpy(data,"");                     
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MRP_DR1"); SESSION_MRP_DR1=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_MRP_DR1);
      printf("...converts to %d\n",SESSION_MRP_DR1);
      if (SESSION_MRP_DR1<-1) {   
        printf("[%d/%d] FATAL: SESSION_MRP_DR1 not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MRP_DR2"); SESSION_MRP_DR2=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_MRP_DR2);
      printf("...converts to %d\n",SESSION_MRP_DR2);
      if (SESSION_MRP_DR2<-1) {   
        printf("[%d/%d] FATAL: SESSION_MRP_DR2 not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MRP_DR3"); SESSION_MRP_DR3=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_MRP_DR3);
      printf("...converts to %d\n",SESSION_MRP_DR3);
      if (SESSION_MRP_DR3<-1) {   
        printf("[%d/%d] FATAL: SESSION_MRP_DR3 not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MRP_DR4"); SESSION_MRP_DR4=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_MRP_DR4);
      printf("...converts to %d\n",SESSION_MRP_DR4);
      if (SESSION_MRP_DR4<-1) {   
        printf("[%d/%d] FATAL: SESSION_MRP_DR4 not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MRP_DR5"); SESSION_MRP_DR5=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_MRP_DR5);
      printf("...converts to %d\n",SESSION_MRP_DR5);
      if (SESSION_MRP_DR5<-1) {   
        printf("[%d/%d] FATAL: SESSION_MRP_DR5 not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MRP_SHL"); SESSION_MRP_SHL=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_MRP_SHL);
      printf("...converts to %d\n",SESSION_MRP_SHL);
      if (SESSION_MRP_SHL<-1) {   
        printf("[%d/%d] FATAL: SESSION_MRP_SHL not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MRP_MCS"); SESSION_MRP_MCS=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_MRP_MCS);
      printf("...converts to %d\n",SESSION_MRP_MCS);
      if (SESSION_MRP_MCS<-1) {   
        printf("[%d/%d] FATAL: SESSION_MRP_MCS not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MUP_ASP"); SESSION_MUP_ASP=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data);   
      sscanf(data,"%d",&SESSION_MUP_ASP);
      printf("...converts to %d\n",SESSION_MUP_ASP);
      if (SESSION_MUP_ASP<-1) {   
        printf("[%d/%d] FATAL: SESSION_MUP_ASP not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }                 
      strcpy(data,"");  
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MUP_NDP"); SESSION_MUP_NDP=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data);
      sscanf(data,"%d",&SESSION_MUP_NDP);
      printf("...converts to %d\n",SESSION_MUP_NDP);
      if (SESSION_MUP_NDP<-1) {   
        printf("[%d/%d] FATAL: SESSION_MUP_NDP not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }
      strcpy(data,"");                     
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MUP_DR1"); SESSION_MUP_DR1=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_MUP_DR1);
      printf("...converts to %d\n",SESSION_MUP_DR1);
      if (SESSION_MUP_DR1<-1) {   
        printf("[%d/%d] FATAL: SESSION_MUP_DR1 not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MUP_DR2"); SESSION_MUP_DR2=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_MUP_DR2);
      printf("...converts to %d\n",SESSION_MUP_DR2);
      if (SESSION_MUP_DR2<-1) {   
        printf("[%d/%d] FATAL: SESSION_MUP_DR2 not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MUP_DR3"); SESSION_MUP_DR3=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_MUP_DR3);
      printf("...converts to %d\n",SESSION_MUP_DR3);
      if (SESSION_MUP_DR3<-1) {   
        printf("[%d/%d] FATAL: SESSION_MUP_DR3 not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MUP_DR4"); SESSION_MUP_DR4=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_MUP_DR4);
      printf("...converts to %d\n",SESSION_MUP_DR4);
      if (SESSION_MUP_DR4<-1) {   
        printf("[%d/%d] FATAL: SESSION_MUP_DR4 not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MUP_DR5"); SESSION_MUP_DR5=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_MUP_DR5);
      printf("...converts to %d\n",SESSION_MUP_DR5);
      if (SESSION_MUP_DR5<-1) {   
        printf("[%d/%d] FATAL: SESSION_MUP_DR5 not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MUP_SHL"); SESSION_MUP_SHL=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_MUP_SHL);
      printf("...converts to %d\n",SESSION_MUP_SHL);
      if (SESSION_MUP_SHL<-1) {   
        printf("[%d/%d] FATAL: SESSION_MUP_SHL not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_MUP_MCS"); SESSION_MUP_MCS=-1;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_MUP_MCS);
      printf("...converts to %d\n",SESSION_MUP_MCS);
      if (SESSION_MUP_MCS<-1) {   
        printf("[%d/%d] FATAL: SESSION_MUP_MCS not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_LOG_SCH"); SESSION_LOG_SCH=0;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_LOG_SCH);
      printf("...converts to %d\n",SESSION_LOG_SCH);
      if ( (SESSION_LOG_SCH<0) || (SESSION_LOG_SCH>1) )  {   
        printf("[%d/%d] FATAL: SESSION_LOG_SCH not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_LOG_EXE"); SESSION_LOG_EXE=0;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_LOG_EXE);
      printf("...converts to %d\n",SESSION_LOG_EXE);
      if ( (SESSION_LOG_EXE<0) || (SESSION_LOG_EXE>1) )  {   
        printf("[%d/%d] FATAL: SESSION_LOG_EXE not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_INC_SMIB"); SESSION_INC_SMIB=0;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_INC_SMIB);
      printf("...converts to %d\n",SESSION_INC_SMIB);
      if ( (SESSION_INC_SMIB<0) || (SESSION_INC_SMIB>1) )  {   
        printf("[%d/%d] FATAL: SESSION_INC_SMIB not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }

  strcpy(keyword,"SESSION_INC_DES"); SESSION_INC_DES=0;
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'",MT_TPSS,getpid(),keyword,data); 
      sscanf(data,"%d",&SESSION_INC_DES);
      printf("...converts to %d\n",SESSION_INC_DES);
      if ( (SESSION_INC_DES<0) || (SESSION_INC_DES>1) )  {   
        printf("[%d/%d] FATAL: SESSION_INC_DES not valid\n",MT_TPSS,getpid());  
        exit(EXIT_FAILURE);
        }      
      strcpy(data,"");               
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   exit(EXIT_FAILURE); break;
    case TPSS_PL_KEYWORD_MISMATCH: /* optional */                                                                            break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    exit(EXIT_FAILURE); break;
    }
