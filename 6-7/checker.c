#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main() {
    int sockfd, n;
    struct sockaddr_in serv_addr;
    char buffer[256];
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(37020);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }
    while (1) {
        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);
        if (n < 0) {
            error("ERROR reading from socket");
        }
        printf("Monitor: Received message %s\n", buffer);
    }
    close(sockfd);
    return 0;
}

