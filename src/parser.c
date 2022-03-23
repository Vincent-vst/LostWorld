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


/* 
 * cf : http://www.anthonybusson.fr/index.php/teaching
 * */ 
int parseRequest(char* requestFromClient, int requestSize, char* string, int stringSize) {

	char *charPtr[4], *end;

	requestFromClient[requestSize-1]='\0';

	if( (end=strstr(requestFromClient,"\r\n\r\n"))==NULL) return(-1);
	*(end+4)='\0';
	
	charPtr[0]=requestFromClient;	

	charPtr[1]=strchr(requestFromClient,' ');	
	if(charPtr[1]==NULL) return(-1);
	charPtr[2]=strchr(charPtr[1]+1,' ');	
	if(charPtr[2]==NULL) return(-1);
	charPtr[3]=strchr(charPtr[2]+1,'\r');	
	if(charPtr[3]==NULL) return(-1);

	*charPtr[1]='\0';
	*charPtr[2]='\0';
	*charPtr[3]='\0';

	if(strcmp(charPtr[0],"GET")!=0) return(-1);
	if(strcmp(charPtr[2]+1,"HTTP/1.1")!=0) return(-1);
	strncpy(string,charPtr[1]+2,stringSize);

	if(string[stringSize-1]!='\0') {
		fprintf(stderr,"Erreur parseRequest(): la taille de la chaine string n'est pas suffisante (stringSize=%d)\n",stringSize);
		exit(3);
	}
	
	if( *(charPtr[1]+2) == '\0') fprintf(stderr,"DEBUG-SERVEUR: le nom de fichier demande est vide -\nDEBUG-SERVEUR: - on associe donc le fichier par defaut website/index.html\n");
	else fprintf(stderr,"DEBUG-SERVEUR: le nom de fichier demande est %s\n",string);

	if( *(charPtr[1]+2) == '\0') strcpy(string,"website/index.html");

	return(0);
}

void traitementErreurOpen(int confd, int fdFichier) {
	int nbOctets;
	char buffer[1000];
	char error404[]="HTTP/1.1 404 FILE NOT FOUND\r\nContent-Type: text/html\r\n\r\n";
	char error500[]="HTTP/1.1 500 INTERNAL SERVER ERROR\r\nContent-Type: text/html\r\n\r\n";
	if(errno==ENOENT) {
		if(send(confd, error404, sizeof(char)*strlen(error404) , 0)<0) perror (" Erreur send ");
		if((fdFichier = open("website/file404.html", O_RDONLY))<0)perror("Erreur open");
		
		while((nbOctets = read(fdFichier, buffer, 100))>0){
			if(send(confd, buffer, nbOctets , 0)<0) perror (" Erreur send ");
		}
	}
	else if(errno==EACCES) {
		if(send(confd, error500, sizeof(char)*strlen(error500) , 0)<0) perror (" Erreur send ");
		if((fdFichier = open("website/file500.html", O_RDONLY))<0)perror("Erreur open");
		
		while((nbOctets = read(fdFichier, buffer, 100))>0){
			if(send(confd, buffer, nbOctets , 0)<0) perror (" Erreur send ");
		}
	}
}
