//
// Created by Haukur Hlöðversson on 23/08/2020.
// https://google.github.io/styleguide/cppguide.html
//
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <algorithm>
#include <map>
#include <vector>

#include <iostream>
#include <sstream>
#include <thread>
#include <map>

void func(int sockfd)
{
    bool finished = false;
    char buffer[1024];
    int n;
    while (!finished) {
        n = 4;
        bzero(buffer, sizeof(buffer));

        buffer[0] = buffer[2] = 'S';
        buffer[1] = 'Y';
        buffer[3] = ' ';
        printf("anonymous@server $ ");
        while ((buffer[n++] = getchar()) != '\n') continue;
        if (strncmp(buffer, "SYS exit", 8) == 0 || strncmp(buffer, "SYS logout", 8) == 0) {
            printf("Client Exit...\n");
            finished = true;
            continue;
        }
        write(sockfd, buffer, sizeof(buffer));
        bzero(buffer, sizeof(buffer));
        read(sockfd, buffer, sizeof(buffer));
        printf("%s", buffer);

        bzero(buffer, sizeof(buffer));
//        std::cout << std::endl;
    }
}

int main(int argc, char *argv[])
{
    // socket_fd = socket file descriptor
    int socket_fd, port;
    struct sockaddr_in serv_addr;

    struct addrinfo hints = {0}, *address_info;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <hostname> port\n", argv[0]);
        exit(1);
    }

    const int status = getaddrinfo(argv[1], argv[2], &hints, &address_info);
    if (status != 0) {
        fprintf(stderr, "%s: %s\n", argv[1], gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    port = atoi(argv[2]);
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in *addr;
    addr = (struct sockaddr_in *)address_info->ai_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr = (struct in_addr)addr->sin_addr;

    if (connect(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(EXIT_FAILURE);
    }

    func(socket_fd);

    close(socket_fd);
    return 0;
}
