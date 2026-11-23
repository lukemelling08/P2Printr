#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <poll.h>
#include <signal.h>

#include "chat.h"
#include "net.h"

static void chat_loop(const int fd) {
    struct pollfd fds[2] = {
        {.fd = STDIN_FILENO, .events = POLLIN},
        {.fd = fd, .events = POLLIN},
    };
    char buf[256];

    LineBuffer lb = {0};
    char line[LINE_MAX_LEN];

    while (1) {
        if (poll(fds, 2, -1) < 0) {
            perror("poll");
            return;
        }

        if (fds[0].revents & POLLIN) {
            if (fgets(buf, sizeof buf, stdin) == NULL) {
                return;
            }
            if (net_send_line(fd, buf) < 0) {
                return;
            }
        }

        if (fds[1].revents & (POLLIN | POLLHUP)) {
            if (lb.len == sizeof lb.data) {
                lb.len = 0;
            }
            const ssize_t n = recv(fd, lb.data + lb.len, sizeof lb.data - lb.len, 0);
            if (n <= 0) {
                printf("they disconnected\n");
                return;
            }
            lb.len += (size_t) n;

            while (chat_take_line(&lb, line, sizeof line)) {
                printf("them: %s\n", line);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);
    if (argc > 1 && strcmp(argv[1], "connect") == 0) {
        int fd = net_connect("127.0.0.1", 5000);
        if (fd < 0) {
            return 1;
        }
        printf("connected! type messages (Ctrl-D to quit)\n");

        chat_loop(fd);
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

    chat_loop(client_fd);

    close(client_fd);
    close(fd);
    return 0;
}
