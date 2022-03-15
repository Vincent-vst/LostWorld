//
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
/* #include "Parser.c" */

int parseRequest(char*, int, char*, int);
void traitementReponse(char*, int, int);
void traitementRequete(int);
void traitementErreurOpen(int, int);
void afficheIp(unsigned char* );


void traitementRequete(int confd)
{
	int nbOctets, fdFichier;
	char buffer[1000],  fileName[100];
	
	char error400[]="HTTP/1.1 400 BAD REQUEST\r\nContent-Type: text/html\r\n\r\n";
	if(recv(confd, buffer, sizeof(buffer)*sizeof(char), 0)<0) perror (" Erreur recv ");//Ici on reçoit la requête du client
	
	if(parseRequest(buffer, sizeof(buffer) , fileName, sizeof(fileName)) == -1 ) //On parse la requête afin d'en récuperer le nom du fichier qui doit être ouvert et on le met dans fileName
	//Si la requête est incorrecte on renvoie dans ce bloc pour afficher l'erreur 400
	{
		if(send(confd, error400, sizeof(char)*strlen(error400) , 0)<0) perror (" Erreur send ");
		if((fdFichier = open("file400.html", O_RDONLY))<0)perror("Erreur open");
		
		while((nbOctets = read(fdFichier, buffer, 100))>0){
			if(send(confd, buffer, nbOctets , 0)<0) perror (" Erreur send ");
		}			
	} 
	//Sinon on continue et on renvoie dans ce bloc pour ouvrir le fichier
	else{		
		if((fdFichier = open(fileName, O_RDONLY))<0)//On ouvre le fichier si il y a une erreur on renvoie dans la fonction traitementErreurOpen sinon on traite la Reponse
		{
			traitementErreurOpen(confd, fdFichier);
		}
		else{
			traitementReponse(fileName, confd, fdFichier);
		}
	}
	close(confd);
}


//Cette fonction est utilisé quand le fichier à été ouvert avec succès, alors on renvoie l'en-tête http correspondant au type du fichier puis on affiche le contenu du fichier
void traitementReponse(char* fileName, int confd, int fdFichier) {
	int nbOctets = 0;
	char buffer[1000];
	char reponseHtml[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
	char reponseJpg[] = "HTTP/1.1 200 OK\r\nContent-Type: image/jpg\r\n\r\n";
	char reponsePng[] = "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\n\r\n";
	if(strstr(fileName, ".html"))//Si c'est un fichier html
	{
		if(send(confd, reponseHtml, sizeof(char)*strlen(reponseHtml) , 0)<0) perror (" Erreur send ");
		
		while((nbOctets = read(fdFichier, buffer, 100))>0){
			if(send(confd, buffer, nbOctets , 0)<0) perror (" Erreur send ");
		}
	}
	else if (strstr(fileName, ".png"))//Si c'est un fichier png
	{
		if(send(confd, reponsePng, sizeof(char)*strlen(reponsePng) , 0)<0) perror (" Erreur send ");
		
		while((nbOctets = read(fdFichier, buffer, 100))>0){
			if(send(confd, buffer, nbOctets , 0)<0) perror (" Erreur send ");
		}
	}
	else if (strstr(fileName, ".jpg"))//Si c'est un fichier jpeg
	{
		if(send(confd, reponseJpg, sizeof(char)*strlen(reponseJpg) , 0)<0) perror (" Erreur send ");
	
		while((nbOctets = read(fdFichier, buffer, 100))>0){
			if(send(confd, buffer, nbOctets , 0)<0) perror (" Erreur send ");
		}
	}
}

void afficheIp(unsigned char* ipclient) {
	printf("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n", ipclient[0],ipclient[1],ipclient[2],ipclient[3],ipclient[4],ipclient[5],ipclient[6], ipclient[7],ipclient[8],ipclient[9],ipclient[10],ipclient[11],ipclient[12],ipclient[13],ipclient[14],ipclient[15]);
}
