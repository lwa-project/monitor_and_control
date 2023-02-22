#include "mcs.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if( argc == 2 ) {
    if( (strncmp(argv[1], "-h", 2) == 0) \
        || (strncmp(argv[1], "--help", 6) == 0) ) {
          printf("./test_tp_connection\n");
          return 1;
        }
  }
  
  char cmd[256];
  sprintf(&cmd[0], "ssh -o ConnectTimeout=5 %s date", LWA_TP_SCP_ADDR);
  int ret = system(cmd);
  
  return ret;
}
