#ifndef _PROCESS
#define _PROCESS

int parseRequest(char*, int, char*, int);
void traitementReponse(char*, int, int);
void traitementRequete(int);
void traitementErreurOpen(int, int);
void afficheIp(unsigned char* );


#endif 

