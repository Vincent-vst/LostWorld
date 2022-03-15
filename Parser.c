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


/* Lit la requete du client. Met le nom du fichier demande dans string.
* Si la syntaxe est incorrecte ou si il manque des retour charriots
* on renvoi -1. Autrement la fonction renvoie 0.
* requestFromClient est la chaine de 1000 octets censee contenir la requete provenant du client.
* requestSize doit etre egale a 1000 (et pas a la taille de la chaine de caractere). 
*/
int parseRequest(char* requestFromClient, int requestSize, char* string, int stringSize)
{
	/* charPtr[4] est un tableau de 4 pointeurs pointant sur le debut de la chaine, les 2 espaces 	*/
	/* de la requete (celui	apres le GET et celui apres le nom de fichier) et sur le premier '\r'.	*/
	/* Le pointeur end sera utilise pour mettre un '\0' a la fin du doubl retour charriot.		*/

	char *charPtr[4], *end;

	/* On cherche le double retour charriot	dans requestFromClient
	* suivant les systemes, on utilise \r ou \n (new line, new feed)
	* par convention en http on utilise les deux \r\n mais cela represente en pratique un seul retour charriot.
	* Pour simplifier ici, on ne recherche que les '\n'.
	* On placera un '\0' juste apres le double retour charriot permettant de traiter la requete 
	* comme une chaine de caractere et d'utiliser les fcts de la bibliotheque string.h. 
	*/

	/* Lecture jusqu'au double retour charriot	*/
	requestFromClient[requestSize-1]='\0';//Permet d'utiliser strchr() - attention ne marche pas si requestSize indique la taille de la chaine de caractere

	if( (end=strstr(requestFromClient,"\r\n\r\n"))==NULL) return(-1);
	*(end+4)='\0';
	
	// Verification de la syntaxe (GET fichier HTTP/1.1) 		
	charPtr[0]=requestFromClient;	//Debut de la requete (GET en principe)
	//On cherche le premier espace, code ascii en 0x20 (en hexa), c'est le debut du nom du fichier
	charPtr[1]=strchr(requestFromClient,' ');	
	if(charPtr[1]==NULL) return(-1);
	charPtr[2]=strchr(charPtr[1]+1,' ');	
	if(charPtr[2]==NULL) return(-1);
	charPtr[3]=strchr(charPtr[2]+1,'\r');	
	if(charPtr[3]==NULL) return(-1);

	//On separe les chaines
	*charPtr[1]='\0';
	*charPtr[2]='\0';
	*charPtr[3]='\0';

	if(strcmp(charPtr[0],"GET")!=0) return(-1);
	if(strcmp(charPtr[2]+1,"HTTP/1.1")!=0) return(-1);
	strncpy(string,charPtr[1]+2,stringSize);//On decale la chaine de 2 octets: le premier octet est le '\0', le deuxieme decalage permet de retirer le "/" 

	//Si stringSize n'est pas suffisement grand, la chaine ne contient pas de '\0'. Pour verifier il suffit de tester string[stringSize-1] qui
	// doit etre = '\0' car strncpy remplit la chaine avec des '\0' quand il y a de la place.
	if(string[stringSize-1]!='\0'){
		fprintf(stderr,"Erreur parseRequest(): la taille de la chaine string n'est pas suffisante (stringSize=%d)\n",stringSize);
		exit(3);
	}
	
	//DEBUG - Vous pouvez le supprimer si vous le souhaitez.
	if( *(charPtr[1]+2) == '\0') fprintf(stderr,"DEBUG-SERVEUR: le nom de fichier demande est vide -\nDEBUG-SERVEUR: - on associe donc le fichier par defaut index.html\n");
	else fprintf(stderr,"DEBUG-SERVEUR: le nom de fichier demande est %s\n",string);

	if( *(charPtr[1]+2) == '\0') strcpy(string,"index.html");

	return(0);
}

//La fonction traitementErreurOpen est utilisé quand le programme n'arrive pas à ouvrir le fichier demandé par la requête
//On verifie quelle est la source de l'erreur avec errno et on renvoie les erreurs en fonction 
void traitementErreurOpen(int confd, int fdFichier) {
	int nbOctets;
	char buffer[1000];
	char error404[]="HTTP/1.1 404 FILE NOT FOUND\r\nContent-Type: text/html\r\n\r\n";
	char error500[]="HTTP/1.1 500 INTERNAL SERVER ERROR\r\nContent-Type: text/html\r\n\r\n";
	if(errno==ENOENT)//Le fichier n'existe pas
	{
		if(send(confd, error404, sizeof(char)*strlen(error404) , 0)<0) perror (" Erreur send ");
		if((fdFichier = open("file404.html", O_RDONLY))<0)perror("Erreur open");
		
		while((nbOctets = read(fdFichier, buffer, 100))>0){
			if(send(confd, buffer, nbOctets , 0)<0) perror (" Erreur send ");
		}
	}
	else if(errno==EACCES)//Le droits d'accès au fichier sont restreints
	{
		if(send(confd, error500, sizeof(char)*strlen(error500) , 0)<0) perror (" Erreur send ");
		if((fdFichier = open("file500.html", O_RDONLY))<0)perror("Erreur open");
		
		while((nbOctets = read(fdFichier, buffer, 100))>0){
			if(send(confd, buffer, nbOctets , 0)<0) perror (" Erreur send ");
		}
	}
}
