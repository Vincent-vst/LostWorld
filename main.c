#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* close() */
#include <sys/socket.h>
#include <netdb.h>
 
int main(void)
{
    int sockfd;
    char host[] = "www.google.com";
    char port[] = "80";
    struct addrinfo hints, *res;
    char message[] = "GET / HTTP/1.1\nHost: www.google.com\n\n";
    unsigned int i;
    char buf[1024];
    int bytes_read;
    int status;
 
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    status = getaddrinfo(host, port, &hints, &res);
    
    if (status != 0) perror("erreur getaddrinfo");

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) perror(" erreur socket");

    status = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if (status == -1) perror("erreur connect");

    freeaddrinfo(res);
    send(sockfd, message, strlen(message), 0);
 
    while (bytes_read > 0) {
        bytes_read = recv(sockfd, buf, 1024, 0);
        if (bytes_read == -1) {
            perror("recv");
        }
        else {
            printf("%.*s", bytes_read, buf);
        }
    } 
 
    close(sockfd);
 
    return 0;
}