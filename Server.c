#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <arpa/inet.h> 
#include <netdb.h>
#include <sys/time.h> 
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stddef.h>
#include <errno.h>
#include "src/parser.c"
#include "src/process.c"


int main() {

	struct addrinfo hints, *res;
	int error, sockfd,  confd, pPid;
	struct sockaddr_in6 donnees;
	socklen_t taille = sizeof(struct sockaddr_in6);
	

	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_INET6; 
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE; 
	
	if((error = getaddrinfo(NULL,"2000",&hints,&res))!=0) gai_strerror(error);
	
	if((sockfd=socket(res->ai_family,res->ai_socktype,res->ai_protocol))<0) perror("Erreur socket():");
	
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,1,sizeof(int)) == -1) perror("Erreur setsockopt() SO_REUSEADDR");
	if(bind(sockfd, (struct sockaddr *) res->ai_addr, res->ai_addrlen )<0) perror("Erreur bind()");

	if(listen(sockfd,1)<0) perror("Erreur listen()");

	while((confd=accept(sockfd, (struct sockaddr*) &donnees, &taille))>=0)
	{
		pPid = getpid();
		fork();
		if(pPid != getpid()){
			traitementRequete(confd);
			afficheIp(donnees.sin6_addr.s6_addr);
			exit(0);
		}
		
		close(confd);
	} 
	
	if(confd<0) perror (" Erreur accept ");
	freeaddrinfo(res);
	
	return 0;
}





