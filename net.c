//
// Created by Luke Melling on 20/11/2026.
//

#include "net.h"

#include <stdio.h>
#include <sys/socket.h>

int net_listen(void) {
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }
    return fd;
}
