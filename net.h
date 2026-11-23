//
// Created by Luke Melling on 20/11/2026.
//

#ifndef P2PRINTR_NET_H
#define P2PRINTR_NET_H

int net_listen(int port);
int net_accept(int listen_fd);
int net_connect(const char *ip, int port);
int net_send_line(int fd, const char *line);

#endif //P2PRINTR_NET_H
