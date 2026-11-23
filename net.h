//
// Created by Luke Melling on 20/11/2026.
//

#ifndef P2PRINTR_NET_H
#define P2PRINTR_NET_H

int net_listen(int port);

int net_accept(int listen_fd);

#endif //P2PRINTR_NET_H
