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

	char website[] = "www.google.com";

	struct addrinfo hints, *res; 
	int sockfd, error;
	memset(&hints,0,sizeof(hints)); 
	hints.ai_family = AF_UNSPEC; 
	hints.ai_socktype = SOCK_STREAM;
    char message[] = "GET / HTTP/1.1\nHost: www.google.com\n\n";
    char buf[1024];
    int bytes_read;

	if(getaddrinfo(website, "https",&hints,&res)!=0) gai_strerror(error); 

	if((sockfd=socket(res->ai_family, res->ai_socktype, res->ai_protocol))<0) perror("Erreur socket():");  

	if(connect(sockfd,res->ai_addr,res->ai_addrlen)<0) perror("Erreur connect()"); 

	if(send(sockfd,message,strlen(message),0)<0) perror("Erreur send()");

    while (bytes_read > 0){
        bytes_read = recv(sockfd, buf, 1024, 0);
        if (bytes_read == -1) {
            perror("Erreur recv");
        }
        else {
            printf("%.*s", bytes_read, buf);
        }
    } 

	close(sockfd);





	return 0; 
}
