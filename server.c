#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    struct sockaddr_in address;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }
    memset(&address, 0, sizeof address);

    address.sin_family = AF_INET;
    address.sin_port = htons(1100);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&address, sizeof address) == -1) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 10) == -1) {
        perror("listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    for(;;) {
        int new_socket = accept(sockfd, NULL, NULL);

        if (new_socket == -1) {
            perror("accept failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }


        char buffer[256];
        bzero(buffer, 256);
        int n = read(new_socket, buffer, sizeof buffer);

        printf("message: %s\n", buffer);
        printf("n: %i\n", n);

        if (shutdown(new_socket, SHUT_RDWR) == -1) {
            perror("shutdown failed");
            close(new_socket);
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        close(new_socket);
    }
    close(sockfd);
    return EXIT_SUCCESS;
}