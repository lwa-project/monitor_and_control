#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "me.h"

/*******************************************************************/
/*** main() ********************************************************/
/*******************************************************************/

int main ( int narg, char *argv[] ) {
  /* variables pertaining to the TCP/IP command interface */
  int server_len;
  int client_len;  
  int server_sockfd;                
  int client_sockfd;   
  struct sockaddr_in server_address; /* for network sockets */
  struct sockaddr_in client_address; /* for network sockets */
  struct timeval client_timeout;
  client_timeout.tv_sec = LWA_PTQ_TIMEOUT;
  client_timeout.tv_usec = 0;
  ssize_t client_recvd;
  int flags; /* used as part of scheme for changing accept()'s blocking behavior */
  
  FILE *fp = NULL;
  int i;
  int iErr;
  int eQuit=0;  /* 1 means shutdown commanded; 2 means shutdown due to error */
  
  struct ssmif_struct s;
  struct stat ssmif_stat;
  const char* ssmif_format = 
#include "me_sscom.h"
;

  struct timeval tv;
  char cmd[1024];
  
  /***************************************/
  /*** Setting up SSMIF-related things ***/
  /***************************************/

  /* Read SSMIF */
  if (!(fp = fopen("state/ssmif.dat","rb"))) {
    printf("[%d/%d] FATAL: unable to fopen 'state/ssmif.dat'\n",ME_SSCOM,getpid());
    exit(EXIT_FAILURE);
    }
  memset(&s,0,sizeof(s));
  fread(&s,sizeof(s),1,fp);
  fclose(fp);
  fp = NULL;
  //printf("[%d/%d] Successfully read SSMIF '%s'\n",ME_SSCOM,getpid(),filename_ssmif);
  printf("[%d/%d] Successfully read state/ssmif.dat\n",ME_SSCOM,getpid());
  
  /* SSMIF stat details */
  stat("state/ssmif.dat", &ssmif_stat);

  /***************************************************************************/
  /* set up sockets interface for user-side communication. (I am the server) */ 
  /***************************************************************************/

  server_sockfd = socket(             /* create socket */
                         AF_INET,     /* domain; network sockets */
                         SOCK_STREAM, /* type (TCP-like) */
                         0);          /* protocol (normally 0) */
  if (server_sockfd == -1) {
    printf("[%d/%d] FATAL: socket() failed\n",ME_SSCOM,getpid());
    perror("me_sscom");
    exit(EXIT_FAILURE); 
    }
  /* name socket */
  server_address.sin_family      = AF_INET;                /* network sockets */  
  server_address.sin_addr.s_addr = inet_addr(LWA_IP_MEE);  /* network sockets */
  server_address.sin_port        = htons(LWA_PORT_MES);    /* network sockets */
  server_len = sizeof(server_address);

  i = bind( server_sockfd, 
           (struct sockaddr *) &server_address, 
            server_len );
  if (i == -1) {
    printf("[%d/%d] FATAL: bind() failed (see error message below)\n",ME_SSCOM,getpid());
    perror("me_sscom");
    printf("[%d/%d] If message above is ``Address already in use'':\n",ME_SSCOM,getpid());
    printf("[%d/%d]   (1) Kill any 'me' processes (e.g., '$ sh ./me_crash.sh').\n",ME_SSCOM,getpid());
    printf("[%d/%d]   (2) Wait a few seconds before trying this again.\n",ME_SSCOM,getpid());
    exit(EXIT_FAILURE); 
    }

  /* create a connection queue */
  i = listen(server_sockfd,
             32 );           /* backlog */
  if (i == -1) {
    printf("[%d/%d] FATAL: me_sscom: listen() failed\n",ME_SSCOM,getpid());
    perror("me_sscom");
    exit(EXIT_FAILURE); 
    }
  /* change accept() from blocking to non-blocking */
  flags = fcntl( server_sockfd, F_GETFL, 0 );
  fcntl( server_sockfd, F_SETFL, O_NONBLOCK|flags );
  
  /***************************************/
  /***************************************/
  /*** main program loop *****************/
  /***************************************/
  /***************************************/

  while (eQuit==0) {  /* any higher value means quit */
    /* check TCP/IP input channel for command */

    client_len = sizeof(client_address);
    client_sockfd = accept( server_sockfd,
                            (struct sockaddr *) &client_address, 
                            &client_len );  

    if (!(client_sockfd==-1)) { /* we have a connection... */
      /* make sure we don't block forever */
      setsockopt(client_sockfd, SOL_SOCKET, SO_RCVTIMEO, &client_timeout, sizeof(client_timeout));
      setsockopt(client_sockfd, SOL_SOCKET, SO_SNDTIMEO, &client_timeout, sizeof(client_timeout));

      /* read it into the cmd string */
      memset(&cmd,0,sizeof(cmd));
      client_recvd = read(client_sockfd,&cmd,sizeof(cmd));
      printf("[%d/%d] Received command '%s'\n",ME_SSCOM,getpid(),cmd);

      /* process the command and report back to requester*/
      if (strcmp(cmd,"MODIFIED")==0) {
        sprintf(cmd,"%ld",(long) ssmif_stat.st_mtime);
        write(client_sockfd,cmd,strlen(cmd));
        }
        
      else if (strcmp(cmd,"SIZE")==0) {
        sprintf(cmd,"%ld",(long) sizeof(struct ssmif_struct));
        write(client_sockfd,cmd,strlen(cmd));
        }
        
      else if (strcmp(cmd,"FORMAT")==0) {
        write(client_sockfd,ssmif_format,strlen(ssmif_format));
        }
        
      else if (strcmp(cmd,"GET")==0) {
        write(client_sockfd,&s,sizeof(struct ssmif_struct));
        }
        
      else {
        sprintf(cmd,"Unknown command");
        write(client_sockfd,&cmd,strlen(cmd));
        }
        
      /* close the connection */
      close(client_sockfd);

      } /* if (!(client_sockfd==-1)) {   ...we have a connection... */  
      
    /* avoiding busy wait */
    usleep(1000); /* Go to sleep for 1000 microseconds = 1 ms */

    } /* while () -- end of main program loop */
    
  /* shutdown the socket connection */
  close(server_sockfd);  
  
  /* just in case */
  if( fp != NULL ) {
    fclose(fp);
    fp = NULL;
  }
  
  /* Say goodnight Gracie */
  printf("[%d/%d] me_sscom: Shutdown complete. Bye.\n",ME_SSCOM,getpid());
  fflush(stdout);
  
  exit(EXIT_SUCCESS);
    
  } /* main() */

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// me_sscom.c: J. Dowell, UNM, 2026 Oct 29
//   .1: Initial version

//==================================================================================
//=== BELOW THIS LINE IS SCRATCH ===================================================
//==================================================================================
