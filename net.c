//
// Created by Luke Melling on 20/11/2026.
//

#include "net.h"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int net_listen(const int port) {
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) < 0) {
        perror("setsockopt");
        close(fd);
        return -1;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *) &addr, sizeof addr) < 0) {
        perror("bind");
        close(fd);
        return -1;
    }

    if (listen(fd, 8) < 0) {
        perror("listen");
        close(fd);
        return -1;
    }

    return fd;
}

int net_accept(const int listen_fd) {
    const int fd = accept(listen_fd, NULL, NULL);
    if (fd < 0) {
        perror("accept");
        return -1;
    }
    return fd;
}

int net_connect(const char *ip, const int port) {
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
        fprintf(stderr, "invalid IP: %s\n", ip);
        close(fd);
        return -1;
    }

    if (connect(fd, (struct sockaddr *) &addr, sizeof addr) < 0) {
        perror("connect");
        close(fd);
        return -1;
    }

    return fd;
}

int net_send_line(const int fd, const char *line) {
    const size_t len = strlen(line);
    size_t sent = 0;

    while (sent < len) {
        const ssize_t n = send(fd, line + sent, len - sent, 0);
        if (n < 0) {
            perror("send");
            return -1;
        }
        sent += (size_t) n;
    }
    return 0;
}
