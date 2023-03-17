#include "mcs.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
  // Load in the base IP address for the MCS network
  char ipaddr[INET_ADDRSTRLEN];
  strcpy(&ipaddr[0], "10.1.1.1");
  if( argc == 2 ) {
    if( (strncmp(argv[1], "-h", 2) == 0) \
        || (strncmp(argv[1], "--help", 6) == 0) ) {
          printf("./build_station_start [MCS IP address base]\n");
          printf("where [MCS IP address base] defaults to '%s'\n", ipaddr);
          return 1;
        }
    strncpy(&ipaddr[0], argv[1], INET_ADDRSTRLEN-1);
    ipaddr[INET_ADDRSTRLEN-1] = '\0';
  }
  
  // Validate
  struct sockaddr_in *addr_in;
  if( inet_pton(AF_INET, &ipaddr[0], &addr_in) != 1 ){
    printf("Invalid IP address provided: '%s'\n", ipaddr);
    return 1;
  }
  
  // Trim it down to the first three octets
  char* last = strrchr(&ipaddr[0], '.');
  if( last != NULL ) {
    *last = '\0';
  }
  
  // Start writing the script
  /* Setup */
  printf("#!/bin/bash\n\n");
  printf("# Change to the scheduler's directory\n");
  printf("cd %s\n\n",  LWA_SCH_SCP_DIR);
  /* ARX initial MIB */
  printf("# 1 ARX power supplies, 1 FEE power supplies, and 1 temperature sensors\n");
  printf("./ms_makeMIB_ASP 1 1 2\n\n");
  /* init script */
  printf("# Create an ms_init initialization file called \"shl_asp.dat\"\n");
  printf("echo \\\n'");
  printf("mibinit ASP %s.40 1740 1741\n", ipaddr);
  printf("mcic    ASP\n\n");
  #if defined(LWA_BACKEND_IS_ADP) && LWA_BACKEND_IS_ADP
    printf("mibinit ADP %s.10 1742 1743\n", ipaddr);
    printf("mcic    ADP\n\n");
  #else
    printf("mibinit DP_ %s.10 1742 1743\n", ipaddr);
    printf("mcic    DP_\n\n");
  #endif
  int i=0;
  for(i=0; i<ME_MAX_NDR; ++i) {
    printf("mibinit DR%i %s.%i %i %i\n", i+1, ipaddr, 21+i, 5000+2*i, 5001+2*i);
    printf("mcic    DR%i\n\n", i+1);
  }
  printf("mibinit SHL %s.30 5008 5009\n", ipaddr);
  printf("mcic    SHL' > shl_asp.dat\n\n");
  /* Run the init script */
  printf("# MCS/Scheduler start (allow a few seconds to get everything running)\n");
  printf("./ms_init shl_asp.dat\n");
  printf("sleep 5\n");
  
  return 0;
}
