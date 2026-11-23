#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

#include "net.h"


int main(void) {
    int fd = net_listen(5000);
    if (fd < 0) {
        return 1;
    }
    printf("listening on port 5000...\n");

    int client_fd = net_accept(fd);
    if (client_fd < 0) {
        close(fd);
        return 1;
    }
    printf("someone connected! fd: %d\n", client_fd);

    char buf[256];
    ssize_t n = recv(client_fd, buf, sizeof buf - 1, 0);
    if (n < 0) {
        perror("recv");
    } else {
        buf[n] = '\0';
        printf("received: %s", buf);
    }

    close(client_fd);
    close(fd);
    return 0;
}
