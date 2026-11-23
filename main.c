#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <poll.h>
#include <signal.h>

#include "chat.h"
#include "net.h"
#include "protocol.h"

static const char *after_prefix(const char *line, const char *prefix) {
    const size_t n = strlen(prefix);
    return strncmp(line, prefix, n) == 0 ? line + n : NULL;
}

static int handle_line(const char *line, ChatState *state) {
    const char *rest;

    if (*state == CHAT_WAITING_REQUEST && ((rest = after_prefix(line, CMD_REQUEST " ")))) {
        printf("%s wants to chat. type accept or decline\n", rest);
        *state = CHAT_INCOMING;
    } else if (*state == CHAT_OUTGOING && strcmp(line, CMD_ACCEPT) == 0) {
        printf("chat accepted!\n");
        *state = CHAT_ACTIVE;
    } else if (*state == CHAT_OUTGOING && strcmp(line, CMD_DECLINE) == 0) {
        printf("they declined\n");
        return -1;
    } else if (*state == CHAT_ACTIVE && (rest = after_prefix(line, CMD_MSG " "))) {
        printf("them: %s\n", rest);
    } else if (strcmp(line, CMD_BYE) == 0) {
        printf("they left the chat\n");
        return -1;
    }
    return 0;
}

static int handle_input(const int fd, char *input, ChatState *state) {
    input[strcspn(input, "\n")] = '\0';

    if (*state == CHAT_INCOMING && strcmp(input, "accept") == 0) {
        printf("chat started!\n");
        *state = CHAT_ACTIVE;
        return net_send_line(fd, CMD_ACCEPT "\n");
    }
    if (*state == CHAT_INCOMING && strcmp(input, "decline") == 0) {
        net_send_line(fd, CMD_DECLINE "\n");
        return -1;
    }
    if (*state == CHAT_ACTIVE) {
        if (strcmp(input, "/close") == 0) {
            net_send_line(fd, CMD_BYE "\n");
            return -1;
        }
        char msg[LINE_MAX_LEN];
        snprintf(msg, sizeof msg, CMD_MSG " %s\n", input);
        return net_send_line(fd, msg);
    }

    printf("can't do that right now\n");
    return 0;
}

static void chat_loop(const int fd, ChatState state) {
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
            if (handle_input(fd, buf, &state) < 0) {
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
                if (handle_line(line, &state) < 0) {
                    return;
                }
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

        if (net_send_line(fd, CMD_REQUEST " luke\n") < 0) {
            close(fd);
            return 1;
        }
        chat_loop(fd, CHAT_OUTGOING);
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

    chat_loop(client_fd, CHAT_WAITING_REQUEST);

    close(client_fd);
    close(fd);
    return 0;
}
