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

int parseRequest(char*, int, char*, int);
void traitementReponse(char*, int, int);
void traitementRequete(int);
void traitementErreurOpen(int, int);
void afficheIp(unsigned char* );


void traitementRequete(int confd) {
	int nbOctets, fdFichier;
	char buffer[1000],  fileName[100];
	
	char error400[]="HTTP/1.1 400 BAD REQUEST\r\nContent-Type: text/html\r\n\r\n";
	if(recv(confd, buffer, sizeof(buffer)*sizeof(char), 0)<0) perror (" Erreur recv ");
	
	if(parseRequest(buffer, sizeof(buffer) , fileName, sizeof(fileName)) == -1 ) {
		if(send(confd, error400, sizeof(char)*strlen(error400) , 0)<0) perror (" Erreur send ");
		if((fdFichier = open("website/file400.html", O_RDONLY))<0)perror("Erreur open");
		
		while((nbOctets = read(fdFichier, buffer, 100))>0){
			if(send(confd, buffer, nbOctets , 0)<0) perror (" Erreur send ");
		}			
	} 

	else {		
		if((fdFichier = open(fileName, O_RDONLY))<0){
			traitementErreurOpen(confd, fdFichier);
		}
		else {
			traitementReponse(fileName, confd, fdFichier);
		}
	}
	close(confd);
}

void traitementReponse(char* fileName, int confd, int fdFichier) {
	int nbOctets = 0;
	char buffer[1000];
	char reponseHtml[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
	char reponseJpg[] = "HTTP/1.1 200 OK\r\nContent-Type: image/jpg\r\n\r\n";
	char reponsePng[] = "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\n\r\n";
	if(strstr(fileName, ".html")) {
		if(send(confd, reponseHtml, sizeof(char)*strlen(reponseHtml) , 0)<0) perror (" Erreur send ");
		
		while((nbOctets = read(fdFichier, buffer, 100))>0){
			if(send(confd, buffer, nbOctets , 0)<0) perror (" Erreur send ");
		}
	}
	else if (strstr(fileName, ".png")) {
		if(send(confd, reponsePng, sizeof(char)*strlen(reponsePng) , 0)<0) perror (" Erreur send ");
		
		while((nbOctets = read(fdFichier, buffer, 100))>0){
			if(send(confd, buffer, nbOctets , 0)<0) perror (" Erreur send ");
		}
	} else if (strstr(fileName, ".jpg")) {
		if(send(confd, reponseJpg, sizeof(char)*strlen(reponseJpg) , 0)<0) perror (" Erreur send ");
	
		while((nbOctets = read(fdFichier, buffer, 100))>0){
			if(send(confd, buffer, nbOctets , 0)<0) perror (" Erreur send ");
		}
	}
}

void afficheIp(unsigned char* ipclient) {
	printf("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n", ipclient[0],ipclient[1],ipclient[2],ipclient[3],ipclient[4],ipclient[5],ipclient[6], ipclient[7],ipclient[8],ipclient[9],ipclient[10],ipclient[11],ipclient[12],ipclient[13],ipclient[14],ipclient[15]);
}
