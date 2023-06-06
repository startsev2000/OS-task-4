#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void *handle_client(void *arg) {
    int sockfd = *(int *) arg;
    char buffer[256];
    char response[256];
    int n;
    struct sockaddr_in cli_addr, broad_addr;
    socklen_t clilen = sizeof(cli_addr);

    int broadcast = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast);

    broad_addr.sin_family = AF_INET;
    broad_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    broad_addr.sin_port = htons(37020);
    while (1) {
        bzero(buffer, 256);
        n = recvfrom(sockfd, buffer, 255, 0, (struct sockaddr *) &cli_addr, &clilen);
        if (n <= 0) {
            break;
        }
        printf("Nechesov: From Petrov I received %s\n", buffer);

        sprintf(response, "Nechesov: From Petrov I received %s\n", buffer);
        n = sendto(sockfd, response, strlen(response), 0, (struct sockaddr *) &broad_addr, sizeof(broad_addr));
        if (n < 0) {
            error("ERROR writing to socket");
        }
    }
    close(sockfd);
    free(arg);
    pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }
    int *newsockfd = malloc(sizeof(int));
    *newsockfd = sockfd;
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, handle_client, newsockfd) != 0) {
        perror("Could not create thread");
        return 1;
    }
    pthread_join(thread_id, NULL);
    close(sockfd);
    return 0;
}

