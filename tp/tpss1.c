
  strcpy(keyword,"PI_ID");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data);  
      strcpy(PI_ID,data);
      strcpy(data,"");  
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   return; break;
    case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); return; break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
    }

  strcpy(keyword,"PI_NAME");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      strcpy(PI_NAME,data); 
      strcpy(data,"");  
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   return; break;
    case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); return; break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
    }


  strcpy(keyword,"PROJECT_ID");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      if (strlen(data)>8) {
        printf("[%d/%d] FATAL: PROJECT_ID data field overlength\n",MT_TPSS,getpid());
        return;
        }
      strcpy(PROJECT_ID,data);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   return; break;
    case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); return; break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
    }

  strcpy(keyword,"PROJECT_TITLE");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      strcpy(PROJECT_TITLE,data);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   return; break;
    case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); return; break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
    }

  strcpy(keyword,"PROJECT_REMPI");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      strcpy(PROJECT_REMPI,data);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   return; break;
    case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); return; break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
    }

  strcpy(keyword,"PROJECT_REMPO");
  while( (i=tpss_parse_line( fpsdf, keyword, data)) == TPSS_PL_BLANK_LINE ) { }
  switch (i) {
    case TPSS_PL_KEYWORD_MATCH:    
      printf("[%d/%d] %s='%s'\n",MT_TPSS,getpid(),keyword,data); 
      strcpy(PROJECT_REMPO,data);
      strcpy(data,"");   
      break;
    case TPSS_PL_EOF:              printf("[%d/%d] FATAL: Unexpected TPSS_PL_EOF\n",MT_TPSS,getpid());   return; break;
    case TPSS_PL_KEYWORD_MISMATCH: printf("[%d/%d] FATAL: TPSS_PL_KEYWORD_MISMATCH\n",MT_TPSS,getpid()); return; break;
    case TPSS_PL_OVERLONG_LINE:    printf("[%d/%d] FATAL: TPSS_PL_OVERLONG_LINE\n",MT_TPSS,getpid());    return; break;
    }

