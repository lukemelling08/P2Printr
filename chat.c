#include "chat.h"

#include <string.h>

int chat_take_line(LineBuffer *lb, char *out, const size_t out_size) {
    const char *nl = memchr(lb->data, '\n', lb->len);
    if (nl == NULL) {
        return 0;
    }

    const size_t used = (size_t) (nl - lb->data) + 1;
    size_t line_len = used - 1;
    if (line_len >= out_size) {
        line_len = out_size - 1;
    }
    memcpy(out, lb->data, line_len);
    out[line_len] = '\0';

    memmove(lb->data, lb->data + used, lb->len - used);
    lb->len -= used;
    return 1;
}
