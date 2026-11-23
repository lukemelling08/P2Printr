#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "net.h"


int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "connect") == 0) {
        int fd = net_connect("127.0.0.1", 5000);
        if (fd < 0) {
            return 1;
        }
        printf("connected! type messages (Ctrl-D to quit)\n");

        char line[256];
        while (fgets(line, sizeof line, stdin) != NULL) {
            if (net_send_line(fd, line) < 0) {
                break;
            }
        }
        close(fd);
        return 0;
    }

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
    ssize_t n;
    while ((n = recv(client_fd, buf, sizeof buf - 1, 0)) > 0) {
        buf[n] = '\0';
        printf("received: %s", buf);
    }
    if (n < 0) {
        perror("recv");
    }

    close(client_fd);
    close(fd);
    return 0;
}
