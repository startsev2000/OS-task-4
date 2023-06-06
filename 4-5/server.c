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
    int n;
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    while (1) {
        bzero(buffer, 256);
        n = recvfrom(sockfd, buffer, 255, 0, (struct sockaddr *) &cli_addr, &clilen);
        if (n <= 0) {
            break;
        }
        printf("Nechesov: From Petrov I received %s\n", buffer);
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

