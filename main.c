#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>



// retourne le nom du fichier demandé par le client 
void return_file () {

	
}


int connection (){
	struct addrinfo hints, *res; 
	int sockfd, confd, error; 

	memset(&hints , 0, sizeof(hints)); 
	
	hints.ai_family = AF_INET6; 
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE;

	if ((error = getaddrinfo(NULL, "2000", &hints, &res))<0) gai_strerror(error);

	if (( sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol) ) <0 ) {
		perror("error socket"); 
		exit(2);
	}

	if ( bind(sockfd, res->ai_addr, res->ai_addrlen) < 0  ){
		perror("error socket"); 
		exit(2);
	}

	if (listen(sockfd, 10) < 0) {
		perror("error socket"); 
		exit(2);
	}

	if (( confd=accept(sockfd, NULL, NULL)) < 0) {
		perror("error socket"); 
		exit(2);
	}

	// retourner le fichier du client ici 
	return_file();


	int nbOctets; 
	char buffer [1024];

	while ((nbOctets = recv(confd, buffer, sizeof(buffer), 0))>0){
		if (write(1, buffer, nbOctets) < 0){
			perror("error socket"); 
			exit(2);
		}
	}

	if (nbOctets <0) perror("error recv"); 
	close(confd); 
	close(sockfd);
	freeaddrinfo(res);

	return 0; 

}




int main(){

	connection();



	return 0; 
	

}
