/*--------------------------------------------------------------------*/
/* conference server */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h>
#include <time.h> 
#include <errno.h>

extern char *  recvtext(int sd);
extern int     sendtext(int sd, char *msg);

extern int     startserver();
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/* main routine */
main(int argc, char *argv[])
{
  int    servsock;    /* server socket descriptor */

  fd_set livesdset;   /* set of live client sockets */
  int    livesdmax;   /* largest live client socket descriptor */

  /* check usage */
  if (argc != 1) {
    fprintf(stderr, "usage : %s\n", argv[0]);
    exit(1);
  }

  /* get ready to receive requests */
  servsock = startserver();
  if (servsock == -1) {
    exit(1);
  }
  /*
    FILL HERE:
    init the set of live clients
  */
	FD_ZERO(&livesdset);
	FD_SET(servsock, &livesdset);
	livesdmax = servsock;
	fd_set readset;
  /* receive requests and process them */
  while (1) {
	int msgflag = 0;
    int    frsock;      /* loop variable */
    /*
      FILL HERE
      wait using select() for
        messages from existing clients and
	connect requests from new clients
    */
	FD_ZERO(&readset);
	readset = livesdset;

  if(select(livesdmax+1,&readset, NULL, NULL, 0)== 0){
		perror("Select failed\n");
		exit(1);
	}
	//printf("here\n");
    /* look for messages from live clients */
    for (frsock=3; frsock <= livesdmax; frsock++){
      /* skip the listen socket */
      /* this case is covered separately */
      if (frsock == servsock) continue;

      if ( /* FILL HERE: message from client 'frsock'? */FD_ISSET(frsock, &readset)!=0) {
	char *  clienthost;  /* host name of the client */
	ushort  clientport;  /* port number of the client */
	
	/*
	  FILL HERE:
	  figure out client's host name and port
	  using getpeername() and gethostbyaddr()
	*/
	struct hostent *cliHost;
	struct sockaddr_in client;
	int cliLen = sizeof(struct sockaddr *);
	getpeername(frsock, (struct sockaddr *)&client , &cliLen);
	clienthost = gethostbyaddr(&(client.sin_addr.s_addr), sizeof(client.sin_addr.s_addr), AF_INET)->h_name;
	clientport = client.sin_port;
	char* msg;
	/* read the message */
	msg = recvtext(frsock);
	if (!msg) {
	  /* disconnect from client */
	  printf("admin: disconnect from '%s(%hu)'\n",
		 clienthost, clientport);

	  /*
	    FILL HERE:
	    remove this guy from the set of live clients
	  */
		FD_CLR(frsock, &livesdset);
		FD_CLR(frsock, &readset);
		msgflag = 1;
		if(frsock == livesdmax){
			int j;
			int temp=0;
			for(j=servsock; j<livesdmax; j++){
				if(FD_ISSET(j,&livesdset)&&j>temp){
					temp = j;
				}
			}
			livesdmax=temp;
		}
	  /* close the socket */
	  close(frsock);
	} else {
	  /*
	    FILL HERE
	    send the message to all live clients
	    except the one that sent the message
	  */
	int i;
	for(i=servsock+1; i<=livesdmax+1; i++){
		if(i!=frsock && (FD_ISSET(i, &livesdset)!=0)){
			sendtext(i,msg);
			msgflag = 1;
		}
	}	

	  /* display the message */
	  printf("%s(%hu): %s", clienthost, clientport, msg);

	  /* free the message */
	  free(msg);
	}
      }
    }
	

    /* look for connect requests */
    if (msgflag == 0 &&(FD_ISSET(frsock, &livesdset)==0)/* FILL HERE: connect request from a new client? */ ) {
      /*
	FILL HERE:
	accept a new connection request
      */
	struct sockaddr_in newClient;
	int leng = sizeof(newClient); 
	int csd;
	csd = accept(servsock, (void *)&newClient, &leng);
      /* if accept is fine? */
      if (csd != -1) {
		if(csd > livesdmax){
			livesdmax = csd;
		}
	char *  clienthost;  /* host name of the client */
	ushort  clientport;  /* port number of the client */

	/*
	  FILL HERE:
	  figure out client's host name and port
	  using gethostbyaddr() and without using getpeername().
	*/
	struct hostent *cliHost;
	int cliLen;
	cliHost = gethostbyaddr(&(newClient.sin_addr.s_addr), sizeof(newClient.sin_addr), AF_INET);
	clienthost = cliHost->h_name;
	clientport = ntohs(newClient.sin_port);
	printf("admin: connect from '%s' at '%hu'\n",
	       clienthost, clientport);

	/*
	  FILL HERE:
	  add this guy to set of live clients
	*/
		FD_SET(frsock, &livesdset);
		
      } else {
	perror("accept");
	exit(0);
      }
    }
  }
}
/*--------------------------------------------------------------------*/
