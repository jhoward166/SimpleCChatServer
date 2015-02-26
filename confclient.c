/*--------------------------------------------------------------------*/
/* conference client */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h>
#include <time.h> 
#include <errno.h>
#include <stdlib.h>

#define MAXMSGLEN  1024

extern char *  recvtext(int sd);
extern int     sendtext(int sd, char *msg);

extern int     hooktoserver(char *servhost, ushort servport);
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
main(int argc, char *argv[])
{
  int  sock;
	fd_set livesdset;
	int    livesdmax;
	

  /* check usage */
  if (argc != 3) {
    fprintf(stderr, "usage : %s <servhost> <servport>\n", argv[0]);
    exit(1);
  }

  /* get hooked on to the server */
  sock = hooktoserver(argv[1], atoi(argv[2]));
  if (sock == -1)
    exit(1);

  /* keep talking */
	FD_ZERO(&livesdset);
	FD_SET(sock, &livesdset);
	FD_SET(0, &livesdset);
	livesdmax = sock;
  while (1) {
    fd_set readset;
	readset = livesdset;
    /*
      FILL HERE 
      use select() to watch simulataneously for
      inputs from user and messages from server
    */
	if(select(livesdmax+1,&readset, NULL, NULL, 0)==0){
		perror("Select unsuccessful.\n");
		exit(0);
	}
    if (/* FILL HERE: message from server? */FD_ISSET(sock, &readset)!=0) {
      char *msg;
      msg = recvtext(sock);
      if (!msg) {
	/* server killed, exit */
	fprintf(stderr, "error: server died\n");
	exit(1);
      }

      /* display the message */
      printf(">>> %s", msg);

      /* free the message */
      free(msg);
    }

    if (/* FILL HERE: input from keyboard? */FD_ISSET(0, &readset)!=0 ) {
      char      msg[MAXMSGLEN];

      if (!fgets(msg, MAXMSGLEN, stdin))
	exit(0);
      sendtext(sock, msg);
    }
  }
}
/*--------------------------------------------------------------------*/
