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

int main() {


	struct addrinfo hints, *res; 
	int sockfd, error;
	memset(&hints,0,sizeof(hints)); 
	hints.ai_family = AF_UNSPEC; 
	hints.ai_socktype = SOCK_STREAM;

	if(getaddrinfo("www.google.fr", "http",&hints,&res)!=0) gai_strerror(error); 

	if((sockfd=socket(..., ..., ...))<0) perror("Erreur socket():");  

	if(connect(...,...,...)<0) perror("Erreur connect()"); 

	if(send(...,...,...,...)<0) perror("Erreur send()");

	if(recv(...,bufferRec,...,...)<0) perror("Erreur recv");

	return 0; 
}
