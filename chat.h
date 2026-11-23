//
// Created by Luke Melling on 20/11/2026.
//

#ifndef P2PRINTR_CHAT_H
#define P2PRINTR_CHAT_H

#include <stddef.h>

#define LINE_MAX_LEN 1024

typedef struct {
    char data[LINE_MAX_LEN];
    size_t len;
} LineBuffer;

int chat_take_line(LineBuffer *lb, char *out, size_t out_size);

#endif //P2PRINTR_CHAT_H
